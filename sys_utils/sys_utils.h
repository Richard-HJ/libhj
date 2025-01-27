/*
 	sys_utils.h     R. Hughes-Jones  The University of Manchester
                  definitions for CPU or system control and query functions
*/
/*
   Copyright (c) 2006,2012  Richard Hughes-Jones, University of Manchester
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

      Version 4.1.0      
        rich  15 May 06  
 */

#ifndef sys_utils_h
#define sys_utils_h


#ifndef INLINE
#define INLINE static inline
//#define INLINE extern inline
#endif

#define CPUid_MAX_SYS_CPUS 32

extern int CPUid_apicid_list[CPUid_MAX_SYS_CPUS];

/* Make a list of which apic_id is in which CPU. */
int CPUid_init(int quiet);

/* for processing the cpu_set_t struct */
#define SET_CPUCORE_LEN_BYTES 128

/* set the cpu affinity of this process */
int set_cpu_affinity (unsigned long new_mask, int quiet);
void cpuset2hex( cpu_set_t *cpuset , char *hex_str);
void hex2cpuset( cpu_set_t *cpuset , char *hex_str);
int	set_cpu_affinity__(cpu_set_t *new_cpuset, int quiet, int action);
void set_cpu_affinity_num (int cpu_affinity_core, int quiet);
void set_cpu_affinity_cpuset (cpu_set_t *new_cpuset, int quiet);


INLINE int CPUid()
/* --------------------------------------------------------------------- */
{
#ifdef ARMv6
  /* assume 1 cpu for now - count from zero */
  return (0);
#else
  unsigned int pa, pb, pc, pd;
  int apic_id;
  int i;
  __asm__("cpuid" : "=a" (pa), "=b" (pb), "=c" (pc), "=d" (pd) : "0" (1));
  apic_id = pb >> 24;

  /* lookup the cpu number that corresponds to this apic id - list set by CPUid_init() */
  for(i=0;i<CPUid_MAX_SYS_CPUS; i++){
      if(apic_id == CPUid_apicid_list[i]) return(i);
  }
  /* no match - return the negative apic_id */
  return (-apic_id);
#endif
}

#endif
