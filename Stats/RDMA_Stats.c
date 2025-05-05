/*
       RDMA_Stats.c      R. Hughes-Jones  The University of Manchester
                      read the RDMA counters 

*/
#include <stdio.h>
#include <string.h>
#include <errno.h>              /* in case of error */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "RDMA_Stats.h"          /* for interface and RDMA counters */

#define FILE_RDMASTATS         "/sys/class/infiniband/"
#define FILE_RDMAPORT          "/ports/"
#define FILE_RDMAHW_COUNTERS   "/hw_counters/"
#define FILE_RDMACOUNTERS      "/counters/"

 /* Location of directory 
 /sys/class/infiniband/mlx5_1/ports/1/hw_counters/ *
 */

char *RDMA_Stats_hw_counters[]={        /* The length of this list MUST be less than RDMA_STATS_MAX_INDEX */
    "out_of_buffer",
    "req_cqe_error",
    NULL};

 /* Location of directory 
 /sys/class/infiniband/mlx5_1/ports/1/counters/ *
 */

char *RDMA_Stats_counters[]={        /* The length of this list MUST be less than RDMA_STATS_MAX_INDEX */
    "port_rcv_data",
    "port_rcv_errors",
    "port_rcv_packets",
    "port_xmit_data",
    "port_xmit_discards",
    "port_xmit_packets",
    "unicast_rcv_packets",
    "unicast_xmit_packets",
    NULL};


void rdma_stats_Init( RDMA_Stat *s, char *devname, int port)
/* --------------------------------------------------------------------- */
{
  char port_str[32];
  
	/* clear the stats */
	memset(s, 0, sizeof(RDMA_Stat));

	/* check have an interface name and port */
	if(devname == NULL) return;
	if(port <= 0) return;
	
	sprintf(port_str, "%d", port);
	/* make the file names */
	strcpy(s->name_hw_stats, FILE_RDMASTATS);
	strcat(s->name_hw_stats, devname);
	strcat(s->name_hw_stats, FILE_RDMAPORT);
	strcat(s->name_hw_stats, port_str);

	strcpy(s->name_stats, s->name_hw_stats);
	strcat(s->name_hw_stats, FILE_RDMAHW_COUNTERS);
	strcat(s->name_stats, FILE_RDMACOUNTERS);

}

void rdma_stats_Start( RDMA_Stat* s)
/* --------------------------------------------------------------------- */
{
  int i;
  FILE *stats_file;
  char filename[RDMA_STATS_MAX_NAME];
#define ERROR_MSG_SIZE 256
  char error_msg[ERROR_MSG_SIZE];              /* buffer for error messages */
  
  
/* loop over hw_counters */
	for (i=0; (RDMA_Stats_hw_counters[i]!=NULL); i++){
		filename[0]=0;
		strcpy(filename, s->name_hw_stats);
		strcat(filename, RDMA_Stats_hw_counters[i]);
		/* open the file with the stats and read it */
		if ( (stats_file = fopen(filename, "r") ) == NULL) {
			snprintf(error_msg, ERROR_MSG_SIZE,
					"Error: Could not open RDMA stats file %s", filename );
			perror(error_msg );
			exit(-1);
		}
		if(fscanf(stats_file, "%ld", &s->rdma_hw_stats1[i]) != 1) {
			perror("Error: Could not read hardware counter value" );
			exit(-1);
		}
			
		fclose(stats_file); 
		//printf("nic_stats_Start() %d %s stats %ld \n", i, RDMA_Stats_hw_counters[i], s->rdma_hw_stats1[i]);

	}
  
/* loop over counters */
	for (i=0; (RDMA_Stats_counters[i]!=NULL); i++){
		filename[0]=0;
		strcpy(filename, s->name_stats);
		strcat(filename, RDMA_Stats_counters[i]);
		/* open the file with the stats and read it */
		if ( (stats_file = fopen(filename, "r") ) == NULL) {
			snprintf(error_msg, ERROR_MSG_SIZE,
					"Error: Could not open RDMA stats file %s", filename );
			perror(error_msg );
			exit(-1);
		}
		if(fscanf(stats_file, "%ld", &s->rdma_stats1[i]) != 1) {
			perror("Error: Could not read RDMA stats counter value" );
			exit(-1);
		}
		fclose(stats_file); 
		//printf("nic_stats_Start() %d %s stats %ld \n", i, RDMA_Stats_counters[i], s->rdma_stats1[i]);

	}
 
}

void rdma_stats_Stop( RDMA_Stat* s)
/* --------------------------------------------------------------------- */
{
  int i;
  FILE *stats_file;
  char filename[RDMA_STATS_MAX_NAME];
  
/* loop over hw_counters */
	for (i=0; (RDMA_Stats_hw_counters[i]!=NULL); i++){
		strcpy(filename, s->name_hw_stats);
		strcat(filename, RDMA_Stats_hw_counters[i]);
		/* open the file with the stats and read it */
		if ( (stats_file = fopen(filename, "r") ) == NULL) {
			perror("Error: open RDMA stats file failed :");   /* descr. is used */
			exit(-1);
		}
		if(fscanf(stats_file, "%ld", &s->rdma_hw_stats2[i]) !=1 ) {
			perror("Error: Could not read hardware counter value" );
			exit(-1);
		}
		fclose(stats_file); 
		//printf("nic_stats_Stop() %d %s stats %ld \n", i, RDMA_Stats_hw_counters[i], s->rdma_hw_stats2[i]);

	}
  
/* loop over counters */
	for (i=0; (RDMA_Stats_counters[i]!=NULL); i++){
		strcpy(filename, s->name_stats);
		strcat(filename, RDMA_Stats_counters[i]);
		/* open the file with the stats and read it */
		if ( (stats_file = fopen(filename, "r") ) == NULL) {
			perror("Error: open RDMA stats file failed :");   /* descr. is used */
			exit(-1);
		}
		if(fscanf(stats_file, "%ld", &s->rdma_stats2[i]) !=1 ) {
			perror("Error: Could not read hardware counter value" );
			exit(-1);
		}
		fclose(stats_file); 
		//printf("nic_stats_Stop() %d %s stats %ld \n", i, RDMA_Stats_counters[i], s->rdma_stats2[i]);

	}
}

void rdma_stats_Info( RDMA_Stat* s, RDMAinfo *rdma_info)
/* --------------------------------------------------------------------- */
{
  int i;
    /* zero RDMAinfo */
    memset(rdma_info, 0, sizeof(RDMAinfo) );
 
/* loop over hw_counters */
	for (i=0; (RDMA_Stats_hw_counters[i]!=NULL); i++){
		rdma_info->hw_stats[i]= s->rdma_hw_stats2[i] - s->rdma_hw_stats1[i];
		//printf("nic_stats_Info() %d %s stats %ld \n", i, RDMA_Stats_hw_counters[i], rdma_info->hw_stats[i]);
	}
  
/* loop over counters */
	for (i=0; (RDMA_Stats_counters[i]!=NULL); i++){
		rdma_info->stats[i]= s->rdma_stats2[i] - s->rdma_stats1[i];
		//printf("nic_stats_Info() %d %s stats %ld \n", i, RDMA_Stats_counters[i], rdma_info->stats[i]);
	} 

}

void rdma_stats_Snap( RDMA_Stat* s, RDMAinfo *rdma_info )
/* --------------------------------------------------------------------- */
{

/* take a snapshot of the RDMA data
*/

  /* local variables */
  int i;
  
    rdma_stats_Stop(s);

    rdma_stats_Info( s, rdma_info);

    /* copy the data from this snapshot to the previous one */
/* loop over hw_counters */
	for (i=0; (RDMA_Stats_hw_counters[i]!=NULL); i++){
		s->rdma_hw_stats1[i] = s->rdma_hw_stats2[i];
	}
  
/* loop over counters */
	for (i=0; (RDMA_Stats_counters[i]!=NULL); i++){
		s->rdma_stats1[i] = s->rdma_stats2[i];
	}
    return;
}


void rdma_stats_print_info( RDMAinfo *rdma_info, int option, char loc_rem)
/* --------------------------------------------------------------------- */
{
/*
Print RDMA Stats info

  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print titles
                  =2 print data
*/
  rdma_stats_print_info_file( rdma_info, option, loc_rem, stdout);
}

void rdma_stats_print_info_file( RDMAinfo *rdma_info, int option, char loc_rem, FILE *output)
/* --------------------------------------------------------------------- */
{
/*
Print RDMA Stats info

  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print titles
                  =2 print data
*/
  int i;
  
  switch(option){
    case 1:  // print titles
        fprintf(output, " %c RDMA HW: ;", loc_rem);
		for(i=0; (RDMA_Stats_hw_counters[i]!=NULL); i++){
			fprintf(output, " %s;", RDMA_Stats_hw_counters[i]);
		}
         fprintf(output, " %c RDMA: ;", loc_rem);
		for(i=0; (RDMA_Stats_counters[i]!=NULL); i++){
			fprintf(output, " %s;", RDMA_Stats_counters[i]);
		}
       break;

    case 2:  // print data
        fprintf(output, " %c RDMA HW: ;", loc_rem);
		for(i=0; (RDMA_Stats_hw_counters[i]!=NULL); i++){
			fprintf(output, " %" LONG_FORMAT "d;", rdma_info->hw_stats[i]);
		}
         fprintf(output, " %c RDMA: ;", loc_rem);
		for(i=0; (RDMA_Stats_counters[i]!=NULL); i++){
			fprintf(output, " %" LONG_FORMAT "d;", rdma_info->stats[i]);
		}
        break;

    }
    return;
}
