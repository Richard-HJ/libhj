/*
       cpu_affinity.c      R. Hughes-Jones  The University of Manchester
       
*/
/*
   Copyright (c) 2012 Richard Hughes-Jones, University of Manchester
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
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

#include "sys_utils.h"           


/* Access macros for `cpu_set'.  */
/*
#define CPU_SETSIZE __CPU_SETSIZE
#define CPU_SET(cpu, cpusetp)   __CPU_SET (cpu, cpusetp)
#define CPU_CLR(cpu, cpusetp)   __CPU_CLR (cpu, cpusetp)
#define CPU_ISSET(cpu, cpusetp) __CPU_ISSET (cpu, cpusetp)
#define CPU_ZERO(cpusetp)       __CPU_ZERO (cpusetp)
*/

int set_cpu_affinity (unsigned long new_mask, int quiet)
/* --------------------------------------------------------------------- */
{
  /* set the cpu affinity of this process with the actions:
get_affinity pid=0 ==> cur_mask
set_affinity pid <== new_mask
get_affinity pid=0 ==> new_mask

set_affinity pid cpu_mask
pid 0 == current
   -1 == parent process
    1 == init
cpu_mask  bitwise cpu_no 3210  ie 0x7 == cpu 2, 1, 0
 */

    unsigned long cur_mask;
	unsigned long one=1;
    cpu_set_t new_cpuset;
    cpu_set_t cur_cpuset;

    unsigned int len = sizeof(new_cpuset);
    int i;
    int cpu;
    pid_t pid =0;
    long my_long;
    int max_cpu = sizeof(my_long)*8; // *8 to make in to bits - expect 64 bits

    /* clear all CPUs from the sets */
    CPU_ZERO(&cur_cpuset);
    CPU_ZERO(&new_cpuset);
    cur_mask =0;

    if(!quiet){
		/* get the affinity for this pid */
        if (sched_getaffinity(pid, len, &cur_cpuset) < 0) {
		perror("sched_getaffinity");
		return (-1);
		}
		/* convert to a mask */
		for(i=0; i<max_cpu; i++){
			cpu = CPU_ISSET(i,&cur_cpuset);
			if(cpu !=0) cur_mask = cur_mask |(one<<i);
		}
		printf("Default affinity: %08lx ",  cur_mask);

    }

    if(new_mask>0){
		/* convert the new mask to a set */
		for(i=0; i<max_cpu; i++){
			if((new_mask & (one<<i))>0) CPU_SET(i, &new_cpuset);
		}
		/* gst the affinity for this pid */
		if (sched_setaffinity(pid, len, &new_cpuset)) {
			perror("sched_setaffinity");
			return (-1);
		}
    }

    if(!quiet){
		/* get the new ie current affinity for this pid */
		CPU_ZERO(&cur_cpuset);
		if (sched_getaffinity(pid, len, &cur_cpuset) < 0) {
			perror("sched_getaffinity");
			return (-1);
		}
		/* convert to a mask */
		cur_mask =0;
		for(i=0; i<max_cpu; i++){
			cpu = CPU_ISSET(i,&cur_cpuset);
			if(cpu !=0) cur_mask = cur_mask |(one<<i);
		}
		printf("Set affinity: %08lx\n ",  cur_mask);
	}
    return (0);
}
