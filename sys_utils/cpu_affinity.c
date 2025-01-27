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
#include <string.h>

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


void cpuset2hex( cpu_set_t *cpuset , char *hex_str)
/* --------------------------------------------------------------------- */
{
	/* return the enabled cores in the cpu_set_t struct as a hex string 
	the length of the hex_string must match the max number of cores in  cpu_set_t struct
	currently this is 1024 so a string length of 128+1 bytes is required */
    int cpu;
	int n, i, j;
	unsigned char byte_mask[128];
	char *str_ptr;
	int last_non_zero=0;
	n=0;
	
	/* ensure byte_mask is zero */
	for(i=0; i<128; i++){
		byte_mask[i]=0;
	}
	
	/* identify the enabled cores putting them into 8 bit masks */
	for(j=0; j<1024; j=j+8){
		for(i=0; i<8; i++){
			cpu = CPU_ISSET(i+j,cpuset);
			if(cpu !=0) byte_mask[n] = byte_mask[n] |(1<<i);
			//printf(" j %d i %d j+i %d cpu %d byte_mask %02x\n", j, i, j+i, cpu, byte_mask[n]);
		}
		if(byte_mask[n] !=0) last_non_zero=n;  // record the last used mask
		//printf(" ========== j %d n %d byte_mask %02x last_non_zero %d\n", j, n, byte_mask[n], last_non_zero);
		n++;
	}
	
	str_ptr = hex_str;
	/* form the hex string starting form the largest enabled core */
	for(n=last_non_zero; n>-1; n=n-1){
		//printf("   addres hex_str %p \n", hex_str);
		if(n == last_non_zero){
			sprintf(hex_str, "%2X", byte_mask[n]);
		} else{
			sprintf(hex_str, "%02X", byte_mask[n]);
		}
		printf(" n %d hex_str %s \n", n, hex_str);
		hex_str=hex_str+2;
	}
	*hex_str=0;
	
	printf("cpu_str %s\n", str_ptr);
	hex_str=str_ptr;
	
}

void hex2cpuset( cpu_set_t *cpuset , char *hex_str)
/* --------------------------------------------------------------------- */
{
	/* enable the cores in the cpu_set_t struct from the hex string */
	size_t len;
	int n;
	char* c;
	int intc;
	int i, j;
	int value;
	int last_char;
	char* ptr;
	char* hex_ptr;
	
	len = strlen(hex_str);
	printf("len %d\n", (int)len);
	printf("hex_str %s\n", hex_str);
	
	hex_ptr = hex_str;   // local
	printf("hex_ptr %p \n", hex_ptr);
	
	/* check for and remove any 0x hex starter */
	ptr = strstr(hex_str, "0x");
	printf("ptr %p \n", ptr);
	if(ptr != hex_ptr || len <= 2) {
		printf(" Affinity hexadecimal string not properly formed \n");
		exit(-1);
	}			
	last_char = 1;  // 2 char onwards skip the 0x

	// use len-1 to avoid the terminating zero	
	j=0;
	for(n=len-1; n>last_char; n=n-1){
		c = (hex_str+n);
		intc = (int) *c;
		printf(" n %d hexchar %c %d ", n, *c, intc);
		// read as hex
		sscanf(c, "%1x", &value);
		printf(" hex value %d\n", value);
		for( i=0; i<4; i++){
			if((value &(1<<i)) > 0){
				CPU_SET(i+j,cpuset);
				printf("CPU_SET %d\n", i+j);
			}
		}
		j = j +4;
	}

}

int	set_cpu_affinity__(cpu_set_t *new_cpuset, int quiet, int action)
/* --------------------------------------------------------------------- */
{
/* set the cpu affinity of this process with the actions:
get_affinity pid=0 ==> cur_set
set_affinity pid <== new_set
get_affinity pid=0 ==> new_set

set_affinity pid 
pid 0 == current
   -1 == parent process
    1 == init

action =1 do set_affinity pid <== new_set
	   =0 do not change affinity
*/
	cpu_set_t cur_cpuset;
    pid_t pid =0;
	char *hex_str;
	hex_str = (char*) malloc(SET_CPUCORE_LEN_BYTES+1);
    unsigned int len = sizeof(cpu_set_t);

	/* read the current affinity settings */
    if(!quiet){
    /* clear all CPUs from the sets */
		CPU_ZERO(&cur_cpuset);
		/* get the affinity for this pid */
        if (sched_getaffinity(pid, len, &cur_cpuset) < 0) {
			perror("sched_getaffinity");
			return (-1);
		}
		cpuset2hex(&cur_cpuset , hex_str);
		printf("Default affinity: %s ", hex_str);
	}

	/* set affinity for this pid*/
    if(action == 1){
		if (sched_setaffinity(pid, len, new_cpuset)) {
			perror("sched_setaffinity");
			return (-1);
		}
    }

	/* read the new affinity settings */
    if(!quiet){
    /* clear all CPUs from the sets */
		CPU_ZERO(&cur_cpuset);
		/* get the affinity for this pid */
        if (sched_getaffinity(pid, len, &cur_cpuset) < 0) {
			perror("sched_getaffinity");
			return (-1);
		}
		cpuset2hex(&cur_cpuset , hex_str);
		printf("Set affinity: %s ", hex_str);
	}
	
	return(0);
}

void	set_cpu_affinity_num (int core, int quiet)
/* --------------------------------------------------------------------- */
{
	/* cpu_affinity_core = -1 == do not set anything just report */
    cpu_set_t new_cpuset;
printf("set_cpu_affinity_num core %d \n", core);
	if( core != -1){
		CPU_ZERO(&new_cpuset);
		CPU_SET(core, &new_cpuset);
		set_cpu_affinity__(&new_cpuset, quiet, 1);
	} else {
		set_cpu_affinity__(&new_cpuset, quiet, 0);
	}
}

void	set_cpu_affinity_cpuset (cpu_set_t *new_cpuset, int quiet)
/* --------------------------------------------------------------------- */
{
	/* only called if new_cpuset has been filled in */
	set_cpu_affinity__(new_cpuset, quiet, 1);
}
