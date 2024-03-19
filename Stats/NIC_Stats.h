/*
       NIC_Stats.h      R. Hughes-Jones  The University of Manchester
                      include file for reading the network interface counters 

*/
/*
   Copyright (c) 2019 Richard Hughes-Jones, University of Manchester
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

#ifndef NIC_STATS_LOADED
#define NIC_STATS_LOADED true

#include <sys/socket.h>         /* for struct sockaddr in linux/if.h */
#include <net/if.h>             /* for struct ifreq */
#include <linux/ethtool.h>

#include "arch.h"               /* define the architecture */

#define NIC_STATS_MAX_NAME  32
#define NIC_STATS_MAX_INDEX 12

/*
 variables to return to caller  
*/

struct _NICinfo {
  int64 stats[NIC_STATS_MAX_INDEX];  /* holds vaues of NIC stats e.g. rx_out_of_buffer */
  char if_name[NIC_STATS_MAX_NAME];
  int num_stats_keep;                /* the number of NIC stats required */
};

typedef struct _NICinfo NICinfo;

/* control structure */
struct _NIC_Stat {
    struct ethtool_stats *stats;     /* for statictics from NIC */
    struct ifreq ifr;                /* for ioctl() */
    int64 nic_stats1[NIC_STATS_MAX_INDEX];
    int64 nic_stats2[NIC_STATS_MAX_INDEX];
    int soc;                         /* file descriptor of socket */
    int nic_stats_supported;         /* =1 if this NIC provides stats */
    int num_stats;                   /* number of stats in the NIC */
    int num_stats_keep;              /* the number of NIC stats required */
    int nic_stats_index[NIC_STATS_MAX_INDEX];  /* list of indices to stats required from the NIC - must match names of stats */
};

typedef struct _NIC_Stat NIC_Stat;


// functions

void nic_stats_Init( NIC_Stat* s, int soc, char *devname);
void nic_stats_Start( NIC_Stat* s);
void nic_stats_Stop( NIC_Stat* s);
void nic_stats_Info( NIC_Stat* s, NICinfo *nic_info);
void nic_stats_Snap( NIC_Stat* s, NICinfo *nic_info );
void nic_stats_print_info( NICinfo *nic_info, int option, char loc_rem);
void nic_stats_print_info_file( NICinfo *nic_info, int option, char loc_rem, FILE *output);
int64 nic_stats_getValue( NICinfo *nic_info, char *stats_name);

#endif /* NIC_STATS_LOADED */
