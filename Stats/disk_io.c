/*
       disk_io.c      R. Hughes-Jones  
                      read the disk IO counters 
*/
/*
   Copyright (c) 2008  Richard Hughes-Jones
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

#include <stdio.h>
#include <string.h>
#include <errno.h>              /* in case of error */
#include <signal.h>
#include <unistd.h>             /* sleep() alarm() close() etc... */
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>                            /* required for open() */
#include <fcntl.h>                               /* required for open() */

#include "disk_io.h"            /* for interface and snmp counters */
#include "CPUStat.h"            /* for CPU usages and interrupt counts */


/*
Information from proc on Kernel 2.6.

/proc/diskstats


*/

void read_diskstats( DISK_IOinfo *diskio_info)
/* --------------------------------------------------------------------- */
{
  FILE *stats_file;
#define LINE_LEN 256
  char line[LINE_LEN];
  char dev_name[LINE_LEN];
  int i;
  int num_elements;
  unsigned int major;
  unsigned int minor;
  int64 rd_ios, rd_merged, rd_sectors, rd_ms;
  int64 wr_ios, wr_merged, wr_sectors, wr_ms;
  int64  ios_inprog, ios_ms, ios_ms_wt;

/* open the file with the disk stats and read it all */
    if ( (stats_file = fopen(FILE_DISKSTATS, "r") ) == NULL) {
        perror("Error: open FILE_DISKSTATS failed :"); 
        exit(-1);
    }

    i =0;
    while(fgets(line, LINE_LEN, stats_file) != NULL){
        num_elements = sscanf(line, "%u %u %s \
                                  %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d \
			          %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d \
                                  %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d ",
			      &major, &minor , dev_name,
			      &rd_ios, &rd_merged, &rd_sectors, &rd_ms,
			      &wr_ios, &wr_merged, &wr_sectors, &wr_ms,
			      &ios_inprog, &ios_ms, &ios_ms_wt );

	if(num_elements == 14 && ((rd_ios + wr_ios)> 0)){
	    if(i < DISKIO_MAX_IF){
	        strncpy(&diskio_info[i].name[0], &dev_name[0], DISKIO_MAX_NAME);
		diskio_info[i].name[DISKIO_MAX_NAME-1] =0;
	        diskio_info[i].rd_ios      = rd_ios;
		diskio_info[i].rd_merged   = rd_merged;
		diskio_info[i].rd_sectors  = rd_sectors;
		diskio_info[i].rd_ms       = rd_ms;
		diskio_info[i].wr_ios      = wr_ios;
		diskio_info[i].wr_merged   = wr_merged;
		diskio_info[i].wr_sectors  = wr_sectors;
		diskio_info[i].wr_ms       = wr_ms;
		diskio_info[i].ios_inprog  = ios_inprog;
		diskio_info[i].ios_ms      = ios_ms;
		diskio_info[i].ios_ms_wt   = ios_ms_wt;
		i++;
	    }
	}
    } /* end of while */

    fclose(stats_file); 
}

void disk_io_Start( DISK_IOStat* s)
/* --------------------------------------------------------------------- */
{
  int i;

/* initialise NET_SNMPstat */
    for(i=0; i<DISKIO_MAX_IF; i++){
	    s->diskio_info1[i].name[0]     = 0;
	    s->diskio_info1[i].rd_ios      = 0;
	    s->diskio_info1[i].rd_merged   = 0;
	    s->diskio_info1[i].rd_sectors  = 0;
	    s->diskio_info1[i].rd_ms       = 0;
	    s->diskio_info1[i].wr_ios      = 0;
	    s->diskio_info1[i].wr_merged   = 0;
	    s->diskio_info1[i].wr_sectors  = 0;
	    s->diskio_info1[i].wr_ms       = 0;
	    s->diskio_info1[i].ios_inprog  = 0;
	    s->diskio_info1[i].ios_ms      = 0;
	    s->diskio_info1[i].ios_ms_wt   = 0;

	    s->diskio_info2[i].name[0]     = 0;
	    s->diskio_info2[i].rd_ios      = 0;
	    s->diskio_info2[i].rd_merged   = 0;
	    s->diskio_info2[i].rd_sectors  = 0;
	    s->diskio_info2[i].rd_ms       = 0;
	    s->diskio_info2[i].wr_ios      = 0;
	    s->diskio_info2[i].wr_merged   = 0;
	    s->diskio_info2[i].wr_sectors  = 0;
	    s->diskio_info2[i].wr_ms       = 0;
	    s->diskio_info2[i].ios_inprog  = 0;
	    s->diskio_info2[i].ios_ms      = 0;
	    s->diskio_info2[i].ios_ms_wt   = 0;
    }


 /* UN set the snapshot flag */
  s->snapshot = 0;

  read_diskstats( s->diskio_info1);

}

void disk_io_Stop( DISK_IOStat* s)
/* --------------------------------------------------------------------- */
{

  read_diskstats( s->diskio_info2);

}


void disk_io_Info( DISK_IOStat* s, DISK_IOinfo *diskio_info )
/* --------------------------------------------------------------------- */
{
  int i;
  /* the lines about wrap32bit are to get round compiler sign issues */
  int64 wrap32bit = 2147483647;
  wrap32bit = wrap32bit*2 +1;


/* return information - the if(x<0) test allows for 32bit counter wrap-round */
    for(i=0; i<DISKIO_MAX_IF; i++){
        diskio_info[i].name[0] = 0;
       if(s->diskio_info1[i].name[0] != 0){
	    strcpy(&diskio_info[i].name[0], &s->diskio_info1[i].name[0]);
	    diskio_info[i].rd_ios     = s->diskio_info2[i].rd_ios     - s->diskio_info1[i].rd_ios;
	    diskio_info[i].rd_merged  = s->diskio_info2[i].rd_merged  - s->diskio_info1[i].rd_merged;
	    diskio_info[i].rd_sectors = s->diskio_info2[i].rd_sectors - s->diskio_info1[i].rd_sectors;
	    diskio_info[i].rd_ms      = s->diskio_info2[i].rd_ms      - s->diskio_info1[i].rd_ms;
	    diskio_info[i].wr_ios     = s->diskio_info2[i].wr_ios     - s->diskio_info1[i].wr_ios;
	    diskio_info[i].wr_merged  = s->diskio_info2[i].wr_merged  - s->diskio_info1[i].wr_merged;
	    diskio_info[i].wr_sectors = s->diskio_info2[i].wr_sectors - s->diskio_info1[i].wr_sectors;
	    diskio_info[i].wr_ms      = s->diskio_info2[i].wr_ms      - s->diskio_info1[i].wr_ms;
	    diskio_info[i].ios_inprog = s->diskio_info2[i].ios_inprog - s->diskio_info1[i].ios_inprog;
	    diskio_info[i].ios_ms     = s->diskio_info2[i].ios_ms     - s->diskio_info1[i].ios_ms;
	    diskio_info[i].ios_ms_wt  = s->diskio_info2[i].ios_ms_wt  - s->diskio_info1[i].ios_ms_wt;
	    //	    if(diskio_info[i]. < 0) diskio_info[i]. = diskio_info[i]. + wrap32bit;
       }
       else{
	    diskio_info[i].rd_ios      = 0;
	    diskio_info[i].rd_merged   = 0;
	    diskio_info[i].rd_sectors  = 0;
	    diskio_info[i].rd_ms       = 0;
	    diskio_info[i].wr_ios      = 0;
	    diskio_info[i].wr_merged   = 0;
	    diskio_info[i].wr_sectors  = 0;
	    diskio_info[i].wr_ms       = 0;
	    diskio_info[i].ios_inprog  = 0;
	    diskio_info[i].ios_ms      = 0;
	    diskio_info[i].ios_ms_wt   = 0;
       }
    }

}

void disk_io_Snap( DISK_IOStat* s, DISK_IOinfo *diskio_info )
/* --------------------------------------------------------------------- */
{
  /* take a snapshot of the disk IO data
return the delta counts since disk_io_Start() or the last disk_io_Snap()
  */

  /* local variables */
  int i;


  disk_io_Stop(s);

  disk_io_Info( s, diskio_info );

  /* set the snapshot flag - helps not decode the net_snmp_Start() data */
  s->snapshot = 1;

  /* copy the data from this snapshot to the previous one */

    for(i=0; i<DISKIO_MAX_IF; i++){
       if(s->diskio_info1[i].name[0] != 0){
	    s->diskio_info1[i].rd_ios      = s->diskio_info2[i].rd_ios ;
	    s->diskio_info1[i].rd_merged   = s->diskio_info2[i].rd_merged;
	    s->diskio_info1[i].rd_sectors  = s->diskio_info2[i].rd_sectors;
	    s->diskio_info1[i].rd_ms       = s->diskio_info2[i].rd_ms;
	    s->diskio_info1[i].wr_ios      = s->diskio_info2[i].wr_ios;
	    s->diskio_info1[i].wr_merged   = s->diskio_info2[i].wr_merged;
	    s->diskio_info1[i].wr_sectors  = s->diskio_info2[i].wr_sectors;
	    s->diskio_info1[i].ios_inprog  = s->diskio_info2[i].ios_inprog;
	    s->diskio_info1[i].ios_ms      = s->diskio_info2[i].ios_ms;
	    s->diskio_info1[i].ios_ms_wt   = s->diskio_info2[i].ios_ms_wt;
       }
       else{
	    s->diskio_info1[i].rd_ios      = 0;
	    s->diskio_info1[i].rd_merged   = 0;
	    s->diskio_info1[i].rd_sectors  = 0;
	    s->diskio_info1[i].rd_ms       = 0;
	    s->diskio_info1[i].wr_ios      = 0;
	    s->diskio_info1[i].wr_merged   = 0;
	    s->diskio_info1[i].wr_sectors  = 0;
	    s->diskio_info1[i].ios_inprog  = 0;
	    s->diskio_info1[i].ios_ms      = 0;
	    s->diskio_info1[i].ios_ms_wt   = 0;
       }
    }

}

