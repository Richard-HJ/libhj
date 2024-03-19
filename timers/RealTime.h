/*
 *	RealTime.h - Realtime clock definitions
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

#ifndef RealTime_h
#define RealTime_h

/* *@name RealTime

   Implements the RealTime clock using registers within the Pentium
   processor.  The clock is calibrated against a daytime clock
   provided by the OS during initialisation.  */

#include <sys/time.h>
#include "arch.h"

#ifndef INLINE
#define INLINE static inline
//#define INLINE extern inline
#endif

typedef  int64 RealTime;
extern int64 RealTime_ticks_per_us;
extern double RealTime_ticks_per_us_d;
/*
extern double RealTime_cpu_speed;
extern int64 RealTime_ticks_per_sec;
extern double RealTime_ticks_per_sec_d;
extern int64 RealTime_ticks_per_us;
extern double RealTime_ticks_per_us_d;
*/
//#ifdef ARMv6
/* to deal with 32 bit timer wrap round */
/*
extern RealTime RealTime_num2_33;
extern RealTime RealTime_lastCSR;
extern RealTime RealTime_wrap;
*/
//#endif  /* ifdef ARMv6 */


#define abs(X) ((X)>0?(X):-(X))

extern RealTime realtimeout_alarm;

typedef struct _RealTimeInit RealTimeInit;

struct _RealTimeInit {
    struct timeval tv_s;
    struct timezone tz_s;
    struct timeval tv_e;
    struct timezone tz_e;
    RealTime t0;
    RealTime t1;
    RealTime t2;
    RealTime t3;
};

 int RealTime_Initialise(int quiet);
 void RealTime_Print(RealTime* t);
 void RealTimeInit_Start(RealTimeInit* rti);
 void RealTimeInit_Finish(RealTimeInit* rti);
 double RealTimeInit_CPUSpeed(RealTimeInit* rti);


INLINE RealTime RealTime_Time()
{
/* read the CPU timestamp register to get the "time" */

#ifdef AMD64
/* AMD uses 32 bit registers for this */
    union {
	struct {
	    unsigned int p0;
	    unsigned int p1;
	} part;
	RealTime time;
    } time;
    asm volatile (".byte 0x0f,0x31" : "=a" (time.part.p0), "=d" (time.part.p1));
    return time.time;
#endif  /* ifdef AMD64 */

#ifdef IA64
  /* gcc-IA64 version */
  unsigned long result;

  __asm__ __volatile__("mov %0=ar.itc" : "=r"(result) :: "memory");

  return (RealTime)result;

#endif  /* ifdef IA64 */

#ifdef IA32
  /* gcc-IA32 version */
    union {
	struct {
	    unsigned int p0;
	    unsigned int p1;
	} part;
	RealTime time;
    } time;
    asm volatile (".byte 0x0f,0x31" : "=a" (time.part.p0), "=d" (time.part.p1));
    return time.time;
#endif  /* ifdef IA32 */

#ifdef ARMv6
  /* ARMv6 version */
    RealTime time;
    unsigned int value;
    // read the CCNT Register on ARM1176JZ-F
    asm volatile ("MRC p15, 0, %0, c15, c12, 1\t\n": "=r"(value)); 
    time = (RealTime) value;
    /*  check for 32 bit overflow of CCreg */
     if(time < RealTime_lastCSR){
        RealTime_wrap += RealTime_num2_33; 
    }
    RealTime_lastCSR = time;
    time  += RealTime_wrap;
    return time;
#endif  /* ifdef ARMv6 */
}

INLINE int RealTime_After(RealTime t0, RealTime t1)
{
    return t0>t1;
}

INLINE RealTime RealTime_Plus(RealTime t0, RealTime t1)
{
    return t0+t1;
}

INLINE RealTime RealTime_Minus(RealTime t0, RealTime t1)
{
    return t0-t1;
}

INLINE RealTime RealTime_PlusUs(RealTime t, int delay)
{
    return t + delay * RealTime_ticks_per_us;
}

INLINE RealTime RealTime_PlusUsD(RealTime t, double delay)
{
    return t + (RealTime)(delay * RealTime_ticks_per_us_d);
}

INLINE int RealTime_MinusUs(RealTime t1, RealTime t2)
{
    return (t1-t2)/RealTime_ticks_per_us;
}

INLINE double RealTime_MinusUsD(RealTime t1, RealTime t2)
{
    return (t1-t2)/RealTime_ticks_per_us_d;
}

INLINE int RealTime_PollAlarm()
{
    return RealTime_Time() >= realtimeout_alarm;
}

INLINE void RealTime_ResetTimeOutAlarm()
{
    realtimeout_alarm = 0x7FFFFFFFFFFFFFFFLL;
}

INLINE void RealTime_SetAlarm(RealTime t)
{
    realtimeout_alarm = t;
}

INLINE int RealTime_LaterThan(RealTime t)
{
    return RealTime_Time() > t;
}

#endif          /* RealTime_h */

