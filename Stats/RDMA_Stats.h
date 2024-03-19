/*
       RDMA_Stats.h      R. Hughes-Jones  The University of Manchester
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

#ifndef RDMA_STATS_LOADED
#define RDMA_STATS_LOADED true

#include "arch.h"               /* define the architecture */

#define RDMA_STATS_MAX_NAME  128
#define RDMA_STATS_MAX_INDEX 12

/*
 variables to return to caller  
*/

struct _RDMAinfo {
  int64 hw_stats[RDMA_STATS_MAX_INDEX];  /* holds vaues of RDMA stats e.g. out_of_buffer */
  int64 stats[RDMA_STATS_MAX_INDEX];     /* holds vaues of RDMA stats e.g. port_rcv_packets */
};

typedef struct _RDMAinfo RDMAinfo;

/* control structure */
struct _RDMA_Stat {
    int64 rdma_hw_stats1[RDMA_STATS_MAX_INDEX];		// for /sys/class/infiniband/mlx5_1/ports/1/hw_counters/*
    int64 rdma_hw_stats2[RDMA_STATS_MAX_INDEX];
    int64 rdma_stats1[RDMA_STATS_MAX_INDEX];		// for /sys/class/infiniband/mlx5_1/ports/1/counters/*
    int64 rdma_stats2[RDMA_STATS_MAX_INDEX];
	char name_hw_stats[RDMA_STATS_MAX_NAME];
	char name_stats[RDMA_STATS_MAX_NAME];
};

typedef struct _RDMA_Stat RDMA_Stat;


// functions

void rdma_stats_Init( RDMA_Stat* s, char *devname, int port);
void rdma_stats_Start( RDMA_Stat* s);
void rdma_stats_Stop( RDMA_Stat* s);
void rdma_stats_Info( RDMA_Stat* s, RDMAinfo *rdma_info);
void rdma_stats_Snap( RDMA_Stat* s, RDMAinfo *rdma_info );
void rdma_stats_print_info( RDMAinfo *rdma_info, int option, char loc_rem);
void rdma_stats_print_info_file( RDMAinfo *rdma_info, int option, char loc_rem, FILE *output);

#endif /* RDMA_STATS_LOADED */
