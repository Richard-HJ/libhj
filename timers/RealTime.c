/*
 *	RealTime.c - Realtime clock functions
 *	Copyright (C) 1998-1999 Marcel Boosten
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>             /* for rand() */

#include "RealTime.h"
#include "../Stats/Statistics.h"

#define REALTIMEINIT_MAX 1000
#define MAX_TOD_DELTA 30000

double RealTime_cpu_speed = 0.0;
int64 RealTime_ticks_per_sec = 0;
double RealTime_ticks_per_sec_d = 0.0;
int64 RealTime_ticks_per_us = 0;
double RealTime_ticks_per_us_d = 0.0;

#ifdef ARMv6
/* to deal with 32 bit timer wrap round */
RealTime RealTime_num2_33 = 0x100000000;
RealTime RealTime_lastCSR =0;
RealTime RealTime_wrap =0;
#endif  /* ifdef ARMv6 */

RealTime realtimeout_alarm;

/* forwrd declarations */
double call_rand(int count);

void RealTime_Print(RealTime* t)
{
    printf("RealTime = %"LONG_FORMAT"d = %"LONG_FORMAT"d secs %"LONG_FORMAT"d us\n",
	  *t,
	  *t/RealTime_ticks_per_sec,
	  (*t - (*t/RealTime_ticks_per_sec)*RealTime_ticks_per_sec)/RealTime_ticks_per_us
	  );
}

void RealTimeInit_Start(RealTimeInit* rti)
{
/* measure the CPU time_stamp time before & after time of day 
   - use to check for long delay in gettimeofday() */
    rti->t0 = RealTime_Time();
    gettimeofday(&(rti->tv_s),&(rti->tz_s));
    rti->t1 = RealTime_Time();
}

void RealTimeInit_Finish(RealTimeInit* rti)
{
    rti->t2 = RealTime_Time();
    gettimeofday(&(rti->tv_e),&(rti->tz_e));
    rti->t3 = RealTime_Time();
}

double RealTimeInit_CPUSpeed(RealTimeInit* rti)
{
/* claculate the conversion mapping for CPU time_stamp time to us */
    long seconds = (rti->tv_e.tv_sec) - (rti->tv_s.tv_sec);
    long useconds = (rti->tv_e.tv_usec) - (rti->tv_s.tv_usec);
    long DayTime_diff = 1e6*seconds+useconds;
    double cpu_speed = (double)(rti->t3-rti->t1)/(double)DayTime_diff;

    if ((rti->t3 - rti->t2 < MAX_TOD_DELTA)&&(rti->t1 - rti->t0 < MAX_TOD_DELTA)) {
	return cpu_speed;
    }
    return -1.0;
}


int RealTime_Initialise(int quiet)
{
    RealTimeInit rtis[REALTIMEINIT_MAX];
    int i;
    Statistics s;
    double cpu_speed;
    int n_loop;
    int loop_count=5000;
    double rand_sum=0.0;

    if(!quiet) printf("Initialising real-time clock.\n");
    Statistics_Init(&s, "RealTime");
    for(i=0;i<REALTIMEINIT_MAX;i++) {
	RealTimeInit_Start(&rtis[i]);
    }
/* sleep by burning CPU - gets round cpuidle & cpufreq issues */
	for(n_loop=0; n_loop<loop_count; n_loop++){
	    rand_sum = rand_sum + call_rand(1000);
	}

    for(i=0;i<REALTIMEINIT_MAX;i++) {
	RealTimeInit_Finish(&rtis[i]);
   }
    for(i=0;i<REALTIMEINIT_MAX;i++) {
	cpu_speed = RealTimeInit_CPUSpeed(&rtis[i]);
	if (cpu_speed >= 0) {
	    Statistics_Add(&s,cpu_speed);
	}
    }
   if(!quiet) printf("CPU speed = %11.7f +- %9.7f MHz (%11.7f - %11.7f)\n",
	          Statistics_Mean(&s),Statistics_StdDev(&s),Statistics_Min(&s),Statistics_Max(&s));
/* set the global variables Note cpu_speed is in MHz */
    RealTime_cpu_speed = cpu_speed;
    RealTime_ticks_per_sec = cpu_speed * 1e6;
    RealTime_ticks_per_sec_d = cpu_speed * 1e6;
    RealTime_ticks_per_us = cpu_speed;
    RealTime_ticks_per_us_d = cpu_speed;

    return 0;
}

double call_rand(int count)
/* --------------------------------------------------------------------- */
{
  double sum=0.0;
  int i;

  for(i=0; i<count; i++){
    sum = sum+ (double) rand() / (double) RAND_MAX;
  }
  return (sum);
}

int RealTime_Initialise_sleep(int quiet)
{
    RealTimeInit rtis[REALTIMEINIT_MAX];
    int i;
    Statistics s;
    double cpu_speed;

    if(!quiet) printf("Initialising real-time clock.\n");
    Statistics_Init(&s, "RealTime");
    for(i=0;i<REALTIMEINIT_MAX;i++) {
	RealTimeInit_Start(&rtis[i]);
    }
    sleep(2);
    for(i=0;i<REALTIMEINIT_MAX;i++) {
	RealTimeInit_Finish(&rtis[i]);
   }
    for(i=0;i<REALTIMEINIT_MAX;i++) {
	cpu_speed = RealTimeInit_CPUSpeed(&rtis[i]);
	if (cpu_speed >= 0) {
	    Statistics_Add(&s,cpu_speed);
	}
    }
   if(!quiet) printf("CPU speed = %11.7f +- %9.7f MHz (%11.7f - %11.7f)\n",
	          Statistics_Mean(&s),Statistics_StdDev(&s),Statistics_Min(&s),Statistics_Max(&s));
/* set the global variables Note cpu_speed is in MHz */
    RealTime_cpu_speed = cpu_speed;
    RealTime_ticks_per_sec = cpu_speed * 1e6;
    RealTime_ticks_per_sec_d = cpu_speed * 1e6;
    RealTime_ticks_per_us = cpu_speed;
    RealTime_ticks_per_us_d = cpu_speed;

    return 0;
}
