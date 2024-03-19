/*
       CPUStat.h      R. Hughes-Jones  The University of Manchester
                      Include file for reading the cpu usage and 
		      number of interrupts from the /proc database

*/
/*
   Copyright (c) 2002,2003,2004,2005,2006,2007,2008,2009,2010,2013 Richard Hughes-Jones, University of Manchester
   All rights reserved.

   Redistribution and use in source and binary forms, with or
   without modification, are permitted provided that the following
   conditions are met:

     o Redistributions of source code must retain the above
       copyright notice, this list of conditions and the following
       disclaimer. 
     o Redistributions in binary form must reproduce the above
       copyright notice, this list of conditions and the following
       disclaimer in the documentation and/or other materials
       provided with the distribution. 

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/
/*
     Modification: 

      Version 3.2-6      
        rich  30 Jan 04  Pad CPUStat to multiple of 8 bytes
 */

#ifndef CPUSTAT_H_LOADED
#define CPUSTAT_H_LOADED 


#include "arch.h"               /* define the architecture */
#include "CPU_net_maxnum.h"     /* CPU & interface names */

#define CPUStat_MAX_BUF 500000
#define CPUStat_MAX_NAME 16

typedef struct _CPUinfo CPUinfo;

/*
 Order of variables in struct _CPUinfo follows info from kernel 2.6.x 
  user nice system idle i/owait irq softirq 
  .kernel is used to return the 2.6.x value of "system mode" 
  while .system is used to return the sum of .iowait + .irg + .softirq  [backwards compatability]
*/


struct _CPUinfo {
  int64 user;
  int64 lo_pri;
  int64 system;
  int64 idle; 
  int64 iowait; 
  int64 irq; 
  int64 softirq; 
  int64 steal;
  int64 kernel;
  int64 guest; 
  int64 guest_nice;
  int num_cpus;
};


typedef struct _Interrupt_info Interrupt_info;

struct _Interrupt_info {
  char name[CPUStat_MAX_NAME];
  int64 count;
};

typedef struct _CPUStat CPUStat;

struct _CPUStat {
    int32 num_cpu;                       /* number of CPUs found */
    int32 pad;                           /* make structure multiple of 8 bytes - 64 bit arch */
    CPUinfo info1[NET_SNMP_MAX_CPU+1];
    Interrupt_info  inter_info1[NET_SNMP_MAX_IF];  
    char interrupt1_buf[CPUStat_MAX_BUF];
    CPUinfo info2[NET_SNMP_MAX_CPU+1];
    Interrupt_info  inter_info2[NET_SNMP_MAX_IF];  
    char interrupt2_buf[CPUStat_MAX_BUF];
    int snapshot;                        /* =1 for snapshots - dont process  CPUStat_Start() data */
};

#define CPUStat_SIZE	(sizeof(struct _CPUStat))
/*
 CPUinfo info1[0] is the cpu usage percentage totals over all CPUs, [i=1..4] for cpu 1..4
 Interrupt_info  inter_info1[0] is the number of interrupts for the timer 
                 [1..5] the number of interrupts for ethernet inetfaces eth0 .. eth3 
                 Note: on SMP machines, these values are summed over the CPUs in use.
 */

/*
 ***************************************************************************
 * System files containing statistics
 ***************************************************************************
 */

/* Files */
#define FILE_PROC		"/proc"
#define FILE_CPUINFO		"/proc/cpuinfo"
#define FILE_STAT		"/proc/stat"
#define FILE_INTERRUPTS		"/proc/interrupts"
#define FILE_DISKSTATS		"/proc/diskstats"
#define FILE_NETDEV		"/proc/net/dev"
#define FILE_NETSNMP		"/proc/net/snmp"
#define FILE_NETSNMP6		"/proc/net/snmp6"


/*
 ***************************************************************************
 * Function calls
 ***************************************************************************
 */

void CPUStat_Init( void);
void CPUStat_Start( CPUStat* s);
void CPUStat_Stop( CPUStat* s);
void CPUStat_Info( CPUStat* s, CPUinfo *cpuinfo, Interrupt_info  *inter_info);
void CPUStat_Snap( CPUStat* s, CPUinfo *cpuinfo, Interrupt_info  *inter_info);
void CPUStat_print_cpu_info( CPUinfo *cpuinfo, int option, char loc_rem, int extended_output);
void CPUStat_print_inter_info( Interrupt_info *inter_info, int option, char loc_rem);
void CPUStat_print_cpu_info_file( CPUinfo *cpuinfo, int option, char loc_rem, int extended_output, FILE *output);
void CPUStat_print_inter_info_file( Interrupt_info *inter_info, int option, char loc_rem, FILE *output);
int CPUStat_GetNumCPUs( void);

#endif	/*  #ifndef CPUSTAT_H_LOADED  */
