/*
       CPUStat.c      R. Hughes-Jones  The University of Manchester
                      read the cpu usage and number of interrupts from the /procs database

*/

#include <stdio.h>
#include <string.h>
#include <errno.h>              /* in case of error */
#include <signal.h>
#include <unistd.h>             /* sleep() alarm() close() etc... */
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>           /* required for open() */
#include <fcntl.h>              /* required for open() */

#include "CPUStat.h"            /* for CPU usages and interrupt counts */
#include "CPU_net_names.h"      /* CPU & interface names */

char CPUStat_buf[CPUStat_MAX_BUF];

int  CPUStatMax_cpus = NET_SNMP_MAX_CPU+1;

/*
Information from man proc on Kernel 2.4.9

 /proc/interrupts
              This is used to record the number of interrupts per
              each IRQ on (at least) the i386 architechure.  Very
              easy to read formatting, done in ASCII.

/proc/stat
              cpu  3357 0 4313 1362393
                     The number of jiffies (1/100ths of a second)
                     that the system spent in user mode, user
                     mode with low priority (nice), system mode,
                     and the idle task, respectively.  The last
                     value should be 100 times the second entry
                     in the uptime pseudo-file.

              intr 1462898
                     The number of interrupts received from the
                     system boot.

              ctxt 115315
                     The number of context switches that the
                     system underwent.

# kernel 2.6.x (got from study of top and /proc/stat )
# user nice system idle i/owait irq softirq 
# more /proc/stat
cpu  3014 160 1853 16592 3381 77 0
cpu0 3014 160 1853 16592 3381 77 0
intr 275335 250792 396 0 2 1 0 4 0 1 12681 5 2 50 0 9077 2324
ctxt 85705
btime 1102707602
processes 2889
procs_running 1
procs_blocked 0

# more /proc/stat    
cpu  5193 0 75931 4680333
cpu0 5193 0 75931 4680333
page 40788 12268
swap 2 0
intr 63570305 4761457 2920 0 3 3 58730489 3 0 0 29135 0 0 36597 0 5743 3955 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
disk_io: (3,0):(5748,3014,81577,2734,24536) 
ctxt 7475404
btime 1008670888
processes 1346

 more /proc/interrupts 
           CPU0       
  0:    6704480          XT-PIC  timer
  1:       7461          XT-PIC  keyboard
  2:          0          XT-PIC  cascade
 10:          0          XT-PIC  usb-uhci
 11:          6          XT-PIC  eth0
 12:     146275          XT-PIC  PS/2 Mouse
 14:     456917          XT-PIC  ide0
NMI:          0 
ERR:          0

*/

int CPUStat_GetNumCPUs( void)
/* --------------------------------------------------------------------- */
{
  FILE *stats_fd;  // file descriptor
  char line[8192];
  int num_cpu=0;

 /* open the file with the cpu stats and read it all to count the number of cpu lines*/
    if ( (stats_fd = fopen(FILE_STAT, "r") ) == NULL) {
        perror("Error: open FILE_STAT failed :");   /* descr. is used */
        exit(-1);
    }
    /* loop over lines in the file */
    while (fgets(line, sizeof(line), stats_fd) != NULL) {

      if (!strncmp(line, "cpu ", 4)) {   // look for the total line
	// skit it
     }
      else if (!strncmp(line, "cpu", 3)) { // look for a numbered cpu line cpu0 cpu1 ...
	num_cpu++;
     }
    } /* end of while() */

    fclose(stats_fd); 
    return(num_cpu);
}

void CPUStat_Init( void)
/* --------------------------------------------------------------------- */
{
/*
  call to initialise for CPUStats

 if not called, the maximum number of CPUs will be printed
*/

/* get the number of CPUs so know how much to print +1 for the totals */
  CPUStatMax_cpus=CPUStat_GetNumCPUs() +1; 
}

void CPUStat_Start( CPUStat* s)
/* --------------------------------------------------------------------- */
{
  FILE *stats_fd;  // file descriptor
  char line[8192];
  int cpu_no=0;

  /* set the snapshot flag - helps not decode the CPUStat_Start() data */
  s->snapshot = 0;
  s->num_cpu =0;

/* initialise CPUstat */
  memset(s, 0, CPUStat_SIZE);


/* open the file with the interrupt stats and read it all */
/*
    if ( (stats_desc = open(FILE_INTERRUPTS, O_RDONLY) ) == -1) {
        perror("Error: open FILE_INTERRUPTS failed :");   // descr. is used 
        exit(-1);
    }
    nread = read(stats_desc, s->interrupt1_buf, CPUStat_MAX_BUF);
	if(nread == 0) {
        printf("Error: read of interrupt stats file failed \n");   
        exit(-1);
    }		
    s->interrupt1_buf[CPUStat_MAX_BUF-1] = 0;
    close(stats_desc); 
*/ 

 /* open the file with the cpu stats and read it all */
    if ( (stats_fd = fopen(FILE_STAT, "r") ) == NULL) {
        perror("Error: open FILE_STAT failed :");   /* descr. is used */
        exit(-1);
    }
    /* loop over lines in the file */
    while (fgets(line, sizeof(line), stats_fd) != NULL) {

      if (!strncmp(line, "cpu ", 4)) {   // look for the total line
	sscanf(line + 5, "%"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u",
		 &s->info1[0].user, 
		 &s->info1[0].lo_pri, 
		 &s->info1[0].kernel, 
		 &s->info1[0].idle, 
		 &s->info1[0].iowait, 
		 &s->info1[0].irq, 
		 &s->info1[0].softirq,
 		 &s->info1[0].steal, 
		 &s->info1[0].guest, 
		 &s->info1[0].guest_nice);
     }
      else if (!strncmp(line, "cpu", 3)) { // look for a numbered cpu line cpu0 cpu1 ...
 	sscanf(line + 3, "%d ", &cpu_no);
	cpu_no++;  // add one as use [0] for the average
	char *line_ptr;
	line_ptr = strchr(line, ' ');
// 	sscanf(line + 5, "%"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u",
 	sscanf(line_ptr, "%"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u",
		 &s->info1[cpu_no].user, 
		 &s->info1[cpu_no].lo_pri, 
		 &s->info1[cpu_no].kernel, 
		 &s->info1[cpu_no].idle, 
		 &s->info1[cpu_no].iowait, 
		 &s->info1[cpu_no].irq, 
		 &s->info1[cpu_no].softirq,
 		 &s->info1[cpu_no].steal, 
 		 &s->info1[cpu_no].guest, 
		 &s->info1[cpu_no].guest_nice);
	s->num_cpu++;
     }
    } /* end of while() */

    fclose(stats_fd); 
}

void CPUStat_Stop( CPUStat* s)
/* --------------------------------------------------------------------- */
{
  FILE *stats_fd;  // file descriptor
  char line[8192];
  int cpu_no=0;


/* open the file with the interrupt stats and read it all */
/*
    if ( (stats_desc = open(FILE_INTERRUPTS, O_RDONLY) ) == -1) {
        perror("Error: open FILE_INTERRUPTS failed :");   // descr. is used
        exit(-1);
    }
    nread = read(stats_desc, s->interrupt2_buf, CPUStat_MAX_BUF);
	if(nread == 0) {
        printf("Error: read of interrupt stats file failed \n");   
        exit(-1);
    }		
    s->interrupt2_buf[CPUStat_MAX_BUF-1] = 0;
    close(stats_desc); 
*/

 /* open the file with the cpu stats and read it all */
    if ( (stats_fd = fopen(FILE_STAT, "r") ) == NULL) {
        perror("Error: open FILE_STAT failed :");   /* descr. is used */
        exit(-1);
    }
    /* loop over lines in the file */
    while (fgets(line, sizeof(line), stats_fd) != NULL) { 

      if (!strncmp(line, "cpu ", 4)) {   // look for the total line
	sscanf(line + 5, "%"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u",
		 &s->info2[0].user, 
		 &s->info2[0].lo_pri, 
		 &s->info2[0].kernel, 
		 &s->info2[0].idle, 
		 &s->info2[0].iowait, 
		 &s->info2[0].irq, 
		 &s->info2[0].softirq,
		 &s->info2[0].steal, 
 		 &s->info2[0].guest, 
		 &s->info2[0].guest_nice);
     }
      else if (!strncmp(line, "cpu", 3)) { // look for a numbered cpu line cpu0 cpu1 ...
 	sscanf(line + 3, "%d ", &cpu_no);
	cpu_no++;  // add one as use [0] for the average
	char *line_ptr;
	line_ptr = strchr(line, ' ');
// 	sscanf(line + 5, "%"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u",
 	sscanf(line_ptr, "%"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u %"LONG_FORMAT"u",
		 &s->info2[cpu_no].user, 
		 &s->info2[cpu_no].lo_pri, 
		 &s->info2[cpu_no].kernel, 
		 &s->info2[cpu_no].idle, 
		 &s->info2[cpu_no].iowait, 
		 &s->info2[cpu_no].irq, 
		 &s->info2[cpu_no].softirq,
		 &s->info2[cpu_no].steal, 
 		 &s->info2[cpu_no].guest, 
		 &s->info2[cpu_no].guest_nice);
     }
    } /* end of while() */

    fclose(stats_fd); 
}



void CPUStat_Info( CPUStat* s,  CPUinfo *cpuinfo, Interrupt_info  *inter_info)
/* --------------------------------------------------------------------- */
{
/*
 process interrupt and CPU load information from CPUStat_Start() &  CPUStat_Stop() 
 */
  int i;
  int total;


/* for interrupts */
//  CPUStat_GetIntInfo( s, inter_info);


/* record the number of CPUs in use +1 for the average */
  cpuinfo[0].num_cpus = s->num_cpu +1;

/* loop over CPUs calc times used in each mode and % */
    for (i=0; i< s->num_cpu+1; i++){

        total = ( s->info2[i].user + s->info2[i].lo_pri + s->info2[i].kernel + s->info2[i].idle +
		  s->info2[i].iowait + s->info2[i].irq + s->info2[i].softirq ) -
	               (s->info1[i].user + s->info1[i].lo_pri + s->info1[i].kernel + s->info1[i].idle +
			s->info1[i].iowait + s->info1[i].irq + s->info1[i].softirq);
	
	/* check if CPU exists or is active */
	if (total >0){
	   cpuinfo[i].user = 100*(s->info2[i].user - s->info1[i].user)/total; 
	   cpuinfo[i].lo_pri = 100*(s->info2[i].lo_pri - s->info1[i].lo_pri)/total; 
	   cpuinfo[i].kernel = 100*(s->info2[i].kernel - s->info1[i].kernel)/total; 
	   cpuinfo[i].idle = 100*(s->info2[i].idle - s->info1[i].idle)/total;
	   cpuinfo[i].iowait = 100*(s->info2[i].iowait - s->info1[i].iowait)/total;
	   cpuinfo[i].irq = 100*(s->info2[i].irq - s->info1[i].irq)/total;
	   cpuinfo[i].softirq = 100*(s->info2[i].softirq - s->info1[i].softirq)/total;
	   cpuinfo[i].system = 100*(s->info2[i].kernel + s->info2[i].iowait + s->info2[i].irq + s->info2[i].softirq  - 
				    s->info1[i].kernel - s->info1[i].iowait - s->info1[i].irq - s->info1[i].softirq)/total; 
	}
	else {
	   cpuinfo[i].user = 0;
	   cpuinfo[i].lo_pri = 0; 
	   cpuinfo[i].system = 0; 
	   cpuinfo[i].idle = 0;
	   cpuinfo[i].iowait = 0;
	   cpuinfo[i].irq = 0;
	   cpuinfo[i].softirq = 0;
	   cpuinfo[i].kernel = 0;
	}

    }  /* end of for() loop over CPUs */
}

void CPUStat_Snap( CPUStat* s,  CPUinfo *cpuinfo, Interrupt_info  *inter_info)
/* --------------------------------------------------------------------- */
{
/* Take a snapshot of the CPU and interrupt data
   return the delta counts since CPUStat_Start() or the last CPUStat_Snap()
*/

  /* loacl variables */
  int i;


  CPUStat_Stop(s);

  CPUStat_Info( s, cpuinfo, inter_info );

  /* set the snapshot flag - helps not decode the CPUStat_Start() data */
  s->snapshot = 1;

  /* copy the data from this snapshot to the previous one */
    for (i=0; i<NET_SNMP_MAX_IF; i++){
       if(s->inter_info1[i].name[0] != 0){
	   s->inter_info1[i].count = s->inter_info2[i].count;
       }
       else{
 	   s->inter_info1[i].count = 0;
      }
    } 
    for (i=0; i< s->num_cpu+1; i++){
      s->info1[i].user    = s->info2[i].user;
      s->info1[i].lo_pri  = s->info2[i].lo_pri;
      s->info1[i].system  = s->info2[i].system;
      s->info1[i].idle    = s->info2[i].idle;
      s->info1[i].iowait  = s->info2[i].iowait;
      s->info1[i].irq     = s->info2[i].irq;
      s->info1[i].softirq = s->info2[i].softirq;
      s->info1[i].kernel  = s->info2[i].kernel;
    }

}

void CPUStat_print_cpu_info( CPUinfo *cpuinfo, int option, char loc_rem, int extended_output)
/* --------------------------------------------------------------------- */
{
/*
  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print titles
                  =2 print data
                  =3 print titles for a table
                  =4 print data in a table
  extended_output =0
*/
  CPUStat_print_cpu_info_file( cpuinfo, option, loc_rem, extended_output, stdout);
}

void CPUStat_print_cpu_info_file( CPUinfo *cpuinfo, int option, char loc_rem, int extended_output, FILE *output)
/* --------------------------------------------------------------------- */
{
/*
  cpuinfo         if NULL then use locally determined number of CPUs -default max or from CPUStats_Init()
  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print titles
                  =2 print data
                  =3 print titles for a table
                  =4 print data in a table
  extended_output =0
*/
	int i = 0;
	int num_cpus;

  /* titles may be printed prior to knowing the number of CPUs - especially for remote nodes */
	num_cpus= CPUStatMax_cpus;
	if (cpuinfo != NULL) num_cpus = cpuinfo[0].num_cpus;

	switch(option){
		case 1:  // print titles
			for ( i=0; i<num_cpus; i++ ){
				if(i==0) {
					fprintf(output, " <%c CPU>; user cpu %%; low-prio cpu %%; kernel cpu%%; idle cpu%%;", loc_rem);
				}
				else{
					fprintf(output, "; %c%1d CPU; user cpu %%; low-prio cpu %%; kernel cpu%%; idle cpu%%;", loc_rem, i-1);
				}
				if(extended_output) {
					fprintf(output, " iowait cpu %%; irq cpu%%; softirq cpu%%;");
				}
			}
		break;

		case 2:  // print data
		case 4:  // print data in a table
			for ( i=0; i<num_cpus; i++ ){
				if(i==0) {
					fprintf(output,  "%c ; %"LONG_FORMAT"d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;", 
							loc_rem,   
							cpuinfo[i].user, 
							cpuinfo[i].lo_pri, 
							cpuinfo[i].system, 
							cpuinfo[i].idle  );
				}
				else{
					fprintf(output,  "%c%1d ; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;", 
							loc_rem, i-1,   
							cpuinfo[i].user, 
							cpuinfo[i].lo_pri, 
							cpuinfo[i].system, 
							cpuinfo[i].idle  );
				}
				if(extended_output) {
					fprintf(output,  " %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;",
							cpuinfo[i].iowait,
							cpuinfo[i].irq,
							cpuinfo[i].softirq  );
				}
				fprintf(output, " ;");
				if(option == 4) fprintf(output, "\n");
			}
		break;

		case 3:  // print titles for a table
			fprintf(output, " <%c CPU>; user cpu %%; low-prio cpu %%; kernel cpu%%; idle cpu%%;", loc_rem);
			if(extended_output) {
				fprintf(output, " iowait cpu %%; irq cpu%%; softirq cpu%%;");
			}
			fprintf(output, "\n");
		break;

  }

  return;
} /* end of CPUStat_print_cpu_info */

void CPUStat_print_inter_info( Interrupt_info *inter_info, int option, char loc_rem)
/* --------------------------------------------------------------------- */
{
/*
  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print titles
                  =2 print data
*/
  CPUStat_print_inter_info_file( inter_info, option, loc_rem, stdout);
}

void CPUStat_print_inter_info_file( Interrupt_info *inter_info, int option, char loc_rem, FILE *output)
/* --------------------------------------------------------------------- */
{
/*
  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print titles
                  =2 print data
*/
 
  int i;
 
  switch(option){
  case 1:  // print titles
      fprintf(output, " %c int; interrupt device: ; counts ;", loc_rem);
      break;

  case 2:  // print data
      fprintf(output, "%c int;", loc_rem);
      for (i=0; i<NET_SNMP_MAX_IF; i++){
	if(inter_info[i].name[0] != 0){
	  fprintf(output, "%s: ; %" LONG_FORMAT "d; ", &inter_info[i].name[0], inter_info[i].count  );
	}
      }
      break;
  }

  return;

} /* end of CPUStat_print_inter_info */

void CPUStat_GetIntInfo( CPUStat* s,  Interrupt_info  *inter_info)
/* --------------------------------------------------------------------- */
{
  int i,j;
  char *str_loc, *str_eth, *str_local;
  char *next_eol;
  char name1[64], name2[64];           /* hold the interrupt information */
  int num;                             /* interrupt number */
  long long num0;                     /* number of interrupts for this cpu */
  long long num_sum=0;

  int chars_used;                      /* number of chars consumed by sscanf() */

  int num_cpu =0;
  int n;
/* initalise */
    num=0;

    if(s->snapshot ==0){
/* process interrupt information from CPUStat_Start() */

/* scan the string read from /proc/interrupts */
    i=1;

    /* look for the number of cpus by counting CPU0 CPU1 ... */
    str_loc = s->interrupt1_buf;
    next_eol = strstr(str_loc, "\n");
    *next_eol =0; /* so we stop at the end of the 1st line */
    for(n=0;;n++){
        if(str_loc < next_eol){
	    str_loc = strstr(str_loc, "CPU");
	    if(str_loc != NULL){
	        str_loc++;
		num_cpu++;
	    }
	}
	if(str_loc == NULL)break;
    }
    str_loc = s->interrupt1_buf;
    *next_eol ='\n';

/*look for "\n" read each line, look for the timer and  network interfaces */
    for(;;){
        /* skip line 1 */
        str_loc = strstr(str_loc, "\n");
	if(str_loc == NULL)break;
	str_loc++;
	next_eol = strstr(str_loc, "\n");
        if(next_eol == NULL)break;
	chars_used=0;
	/* read the interrupt number and the : */
	sscanf(str_loc, " %d: %n", &num, &chars_used);
	str_loc += chars_used;
	/* loop over number of cpus summing the interrupts */
	num_sum = 0;
	for( j=0; j< num_cpu; j++){
	    sscanf(str_loc, " %Ld %n", &num0, &chars_used);
	    str_loc += chars_used;
	    num_sum += num0;
	}
	/* get text at the end */
	sscanf(str_loc, "%s %n", &name1[0], &chars_used);
	str_loc += chars_used;
	/* get the second block of text with the interface name.
	   this can have multiple names OR in 2.6.25 kernels no name */
	if(chars_used>0){
	    /* check there is a second name */
	    if(next_eol > str_loc){
	        strncpy(name2, str_loc, next_eol-str_loc);
	        name2[next_eol-str_loc] =0;
	    }
	    else{
	        str_loc = next_eol;
	        name2[0] =0;
	    }
	}
	else{
	  name2[0] =0;
	}
	str_eth = strstr(name2, "time");
	str_local = strstr(name2, "Local");
	if(str_eth != NULL && str_local == NULL){
	  s->inter_info1[0].count= num_sum;
	  strncpy(&s->inter_info1[0].name[0], name2, CPUStat_MAX_NAME);
	  s->inter_info1[0].name[CPUStat_MAX_NAME-1] =0; // just in case
	}

	for(j=0; j<CPUStat_MAX_INT_TYPES; j++){
	  str_eth = strstr(name2, int_type[j]);
	  if(str_eth != NULL){
	    if(i<NET_SNMP_MAX_IF){
	      s->inter_info1[i].count = num_sum;
	      strncpy(&s->inter_info1[i].name[0], str_eth, CPUStat_MAX_NAME);
	      s->inter_info1[i].name[CPUStat_MAX_NAME-1] =0; // just in case
	      i++;
	    }
	  }
	}
    } /* end of for(;;) look for the timer and  network interfaces */

    } /* end of snapshot check if have to process the CPUStat_Start() data */

/* process interrupt information from CPUStat_Stop() */
    i=1;
    str_loc = s->interrupt2_buf;

/* scan the string read from /proc/interrupts
look for "\n" read each line, look for the timer and  network interfaces */
    for(;;){
        /* skip line 1 */
        str_loc = strstr(str_loc, "\n");
	if(str_loc == NULL)break;
	str_loc++;
	next_eol = strstr(str_loc, "\n");
        if(next_eol == NULL)break;
	chars_used=0;
	/* read the interrupt number and the : */
	sscanf(str_loc, " %d: %n", &num, &chars_used);
	str_loc += chars_used;
	/* loop over number of cpus summing the interrupts */
	num_sum = 0;
	for( j=0; j< num_cpu; j++){
	    sscanf(str_loc, " %Ld %n", &num0, &chars_used);
	    str_loc += chars_used;
	    num_sum += num0;
	}
	/* get text at the end */
	sscanf(str_loc, "%s %n", &name1[0], &chars_used);
	str_loc += chars_used;
	/* get the second block of text with the interface name.
	   this can have multiple names OR in 2.6.25 kernels no name */
        if(chars_used>0){
	    /* check there is a second name */
	    if(next_eol > str_loc){
	        strncpy(name2, str_loc, next_eol-str_loc);
	        name2[next_eol-str_loc] =0;
	    }
	    else{
	        str_loc = next_eol;
	        name2[0] =0;
	    }
        }
        else{
          name2[0] =0;
        }

	str_eth = strstr(name2, "time");
	str_local = strstr(name2, "Local");
	if(str_eth != NULL && str_local == NULL){
	  s->inter_info2[0].count= num_sum;
	  strncpy(&s->inter_info2[0].name[0], name2, CPUStat_MAX_NAME);
	  s->inter_info2[0].name[CPUStat_MAX_NAME-1] =0; // just in case
	}

	for(j=0; j<CPUStat_MAX_INT_TYPES; j++){
	  str_eth = strstr(name2, int_type[j]);
	  if(str_eth != NULL){
	    if(i<NET_SNMP_MAX_IF){
	      s->inter_info2[i].count= num_sum;
	      strncpy(&s->inter_info2[i].name[0], str_eth, CPUStat_MAX_NAME);
	      s->inter_info2[i].name[CPUStat_MAX_NAME-1] =0; // just in case
	      i++;
	    }
	  }
	}
    }

/* put the number of interrupts this period in the output structure */
    for (i=0; i<NET_SNMP_MAX_IF; i++){
       inter_info[i].count = 0;
       inter_info[i].name[0] = 0;
       if(s->inter_info1[i].name[0] != 0){
	  inter_info[i].count = (s->inter_info2[i].count - s->inter_info1[i].count);
	  strcpy(&inter_info[i].name[0], &s->inter_info1[i].name[0]);
       }
    }

}

