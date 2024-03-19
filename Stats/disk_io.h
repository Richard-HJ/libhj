/*
       disk_io.h      R. Hughes-Jones 
                      include file for reading the disk IO counters 

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
/*
     Modification: 

      Version 1.0.0      
        rich  4 Apr 08 

 */

#include "arch.h"               /* define the architecture */

#define DISKIO_MAX_NAME 16
#define DISKIO_MAX_IF   4


/*
 return to caller variables 
*/
typedef struct _DISK_IOinfo DISK_IOinfo;

struct _DISK_IOinfo {
  char name[DISKIO_MAX_NAME];
  int64 rd_ios;                     /* number of reads completed successfully */
  int64 rd_merged;                  /* number of merged ajacent reads */
  int64 rd_sectors;                 /* number of sectors read successfully */
  int64 rd_ms;                      /* number of ms spent reading */ 

  int64 wr_ios;                     /* number of writes completed successfully */
  int64 wr_merged;                  /* number of merged ajacent writes */
  int64 wr_sectors;                 /* number of sectors written successfully */
  int64 wr_ms;                      /* number of ms spent writeing */ 

  int64 ios_inprog;                 /* number of IOs in progrss */
  int64 ios_ms;                     /* number of ms spent doing IOs */
  int64 ios_ms_wt;                  /* weighted number of ms spent doing IOs */

};


typedef struct _DISK_IOStat DISK_IOStat;

struct _DISK_IOStat {
    DISK_IOinfo diskio_info1[DISKIO_MAX_IF];
    DISK_IOinfo diskio_info2[DISKIO_MAX_IF];
    int mode;                       /* =1 */
    int snapshot;                   /* =1 if doing snapshots */
};


void disk_io_Start( DISK_IOStat* s);
void disk_io_Stop( DISK_IOStat* s);
void disk_io_Info( DISK_IOStat* s, DISK_IOinfo *diskio_info );
void disk_io_Snap( DISK_IOStat* s, DISK_IOinfo *diskio_info );
