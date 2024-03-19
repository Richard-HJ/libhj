/*
       CPUid.c      R. Hughes-Jones  The University of Manchester
       
       With thanks for advice from Gokhan Unel <Gokhan.Unel@cern.ch>
       and example code from Markus Joos <Markus.Joos@cern.ch>.

*/
/*
   Copyright (c) 2002,2003,2004,2005,2006,2012 Richard Hughes-Jones, University of Manchester
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

#define _GNU_SOURCE  
#include <string.h>
#include <errno.h>              /* in case of error */
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
 
#include "sys_utils.h"           /* for CPUid functions */

/* Access macros for `cpu_set'.  */
/*
#define CPU_SETSIZE __CPU_SETSIZE
#define CPU_SET(cpu, cpusetp)   __CPU_SET (cpu, cpusetp)
#define CPU_CLR(cpu, cpusetp)   __CPU_CLR (cpu, cpusetp)
#define CPU_ISSET(cpu, cpusetp) __CPU_ISSET (cpu, cpusetp)
#define CPU_ZERO(cpusetp)       __CPU_ZERO (cpusetp)
*/


int CPUid_apicid_list[CPUid_MAX_SYS_CPUS];

 int CPUid_init(int quiet)
{
/* --------------------------------------------------------------------- */
/* Make a list of which apic_id is in which CPU. */

    cpu_set_t cur_cpuset;
    cpu_set_t new_cpuset;
    unsigned long cur_mask;
    unsigned long test_mask;
    unsigned int len = sizeof(cur_mask);
    pid_t pid;
    int cpu;
    int i;

    /* initialise the list to non- physical apic nummbers (only 0-255 allowed) */
    for(i=0;i<CPUid_MAX_SYS_CPUS; i++){
      CPUid_apicid_list[i]=-1;
    }

/* read the CPU affinity that is currently allowed for this process */
    pid =0;
    if (sched_getaffinity(pid, len, &cur_cpuset) < 0) {
        perror("CPUid_init: sched_getaffinity");
        return -1;
    }

    test_mask=1;
/* step though the allowed CPUs and read the cpu affinity for that CPU */
    for(i=0;i<CPUid_MAX_SYS_CPUS; i++){
	  cpu = CPU_ISSET(i,&cur_cpuset);
	  if(cpu !=0) {
	      CPU_ZERO(&new_cpuset);
	      CPU_SET(i, &new_cpuset);
	      /* run on this CPU  */
	      if (sched_setaffinity(pid, len, &new_cpuset)) {
		perror("CPUid_init: sched_setaffinity");
		return -1;
	      }
	      /* read the apic */
	      CPUid_apicid_list[i]=-1*CPUid();
	      if(!quiet) printf("i %d mask %lx apicid %d \n", i, test_mask, CPUid_apicid_list[i]);
	  }
    }

/* restore the allowed CPU affinity */
    if (sched_setaffinity(pid, len, &cur_cpuset)) {
        perror("CPUid_init: sched_setaffinity");
        return -1;
    }
    return(0);
}
