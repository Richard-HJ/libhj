/*
       NIC_Stats.c      R. Hughes-Jones  The University of Manchester
                      read the network interface counters 
					  Uses ioctl() and SIOCETHTOOL
					  get the number of counters/stats available from the NIC
					  get the counters/stats names from the NIC
					  get the counters/stats data from the NIC

*/
#include <stdio.h>
#include <string.h>
#include <errno.h>              /* in case of error */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>      /* for SIOCETHTOOL */
#include <linux/ethtool.h>

#include "NIC_Stats.h"          /* for interface and snmp counters */

char *NIC_Stats_name[]={        /* The length of this list MUST be less than NIC_STATS_MAX_INDEX */
    "rx_packets",
    "tx_packets",
    "rx_packets_phy",
    "tx_packets_phy",
    "rx_pause_ctrl_phy",
    "tx_pause_ctrl_phy",
    "tx_global_pause_duration",
    "rx_crc_errors_phy",
    "rx_out_of_buffer",
    NULL};


int exec_ioctl(NIC_Stat *s, void *cmd_block)
{
  /* cmd_block contains the request for action and pointer to the reuren data area */
	s->ifr.ifr_data = cmd_block;
	return ioctl(s->soc, SIOCETHTOOL, &s->ifr);
}


void nic_stats_Init( NIC_Stat* s, int soc, char *devname)
/* --------------------------------------------------------------------- */
{
  int ret;
  int i,j;
  struct ethtool_drvinfo drvinfo;
  struct ethtool_gstrings *stats_names;


	s->soc = soc;
        s->nic_stats_supported =0;  /* not supported yet */

	/* find the number of stats we wnat to keep */
	s->num_stats_keep =0;
	for (j=0; (NIC_Stats_name[j]!=NULL); j++){
		s->num_stats_keep++;
		/* set index to not implemented */
		s->nic_stats_index[j]=-1;
	}

	/* check have an interface name - check after getting num of stats so can print titles and remote data */
	if(devname == NULL) return;

	/* Setup our control structures. */
	memset(&s->ifr, 0, sizeof(s->ifr));
	strcpy(s->ifr.ifr_name, devname);

	/* get the number of stats available from the NIC */
	drvinfo.cmd = ETHTOOL_GDRVINFO;
	ret = exec_ioctl(s, &drvinfo);
	if (ret == -1 )return;
	s->num_stats = drvinfo.n_stats;

	/* allocate space for NIC stats names */
	stats_names = calloc(1, (s->num_stats*ETH_GSTRING_LEN + sizeof(struct ethtool_gstrings)));
	/* get the stats names from the NIC */
  	stats_names->cmd = ETHTOOL_GSTRINGS;
	stats_names->string_set = ETH_SS_STATS;
	stats_names->len = s->num_stats;
	ret = exec_ioctl(s, stats_names);
	if (ret == -1 )return;
		
	/* look for the required NIC stats names & put in the index */
	for (i=0; i< s->num_stats; i++){
	    for (j=0; j<s->num_stats_keep; j++){
	        if(strcmp((char*)&stats_names->data[i*ETH_GSTRING_LEN], NIC_Stats_name[j])==0 ){
		    s->nic_stats_index[j]=i;
		    //printf(" i %d j %d stats name %s NIC_Stats_name %s \n", i, j, &stats_names->data[i*ETH_GSTRING_LEN], NIC_Stats_name[j]);
		}
	    }
	}
	
	/* allocate space for NIC stats data */
	s->stats = calloc(1, ((s->num_stats*sizeof(int64)) + sizeof(struct ethtool_stats)));

	//	free (drvinfo);
	free (stats_names);
	s->nic_stats_supported =1;  /* now NIC stats is supported */
}

void nic_stats_Start( NIC_Stat* s)
/* --------------------------------------------------------------------- */
{
  int i;
  int ret;
    /* check if this NIC supports stats - if so read & store them */
    if(s->nic_stats_supported !=1) return;

    /* get the stats data from the NIC */
    s->stats->cmd = ETHTOOL_GSTATS;
    s->stats->n_stats = s->num_stats;
    ret = exec_ioctl(s, s->stats);
    if (ret == -1 )return;

    for (i=0; i<s->num_stats_keep; i++){
		if(s->nic_stats_index[i] >=0) s->nic_stats1[i] = s->stats->data[ s->nic_stats_index[i] ];
	//printf("nic_stats_Start() %d %s index %d %lld \n", i, NIC_Stats_name[i], s->nic_stats_index[i], s->stats->data[ s->nic_stats_index[i]]);
    }

}

void nic_stats_Stop( NIC_Stat* s)
/* --------------------------------------------------------------------- */
{
  int i;
  int ret;
    /* check if this NIC supports stats - if so read & store them */
    if(s->nic_stats_supported !=1) return;

    /* get the stats data from the NIC */
    s->stats->cmd = ETHTOOL_GSTATS;
    s->stats->n_stats = s->num_stats;
    ret = exec_ioctl(s, s->stats);
    if (ret == -1 )return;

    for (i=0; i<s->num_stats_keep; i++){
		if(s->nic_stats_index[i] >=0) s->nic_stats2[i] = s->stats->data[ s->nic_stats_index[i] ];
        //printf("nic_stats_Stop() %d %s index %d %lld \n", i, NIC_Stats_name[i], s->nic_stats_index[i], s->stats->data[ s->nic_stats_index[i]]);
    }
	
}
void nic_stats_Info( NIC_Stat* s, NICinfo *nic_info)
/* --------------------------------------------------------------------- */
{
  int i;
    /* zero NICinfo */
    memset(nic_info, 0, sizeof(NICinfo) );
    
    /* check if this NIC supports stats - if so read & store them */
    if(s->nic_stats_supported !=1) return;

    for (i=0; i<s->num_stats_keep; i++){
		if(s->nic_stats_index[i] >=0) {
			nic_info->stats[i] = s->nic_stats2[i] - s->nic_stats1[i];
		}
		else {
			nic_info->stats[i] = -1; // not implemented
		}
		//printf("nic_stats_Info() %d %s index %d %ld \n", i, NIC_Stats_name[i], s->nic_stats_index[i], nic_info->stats[i]);
    }
    nic_info->num_stats_keep = s->num_stats_keep;
    strcpy(nic_info->if_name, s->ifr.ifr_name ); 
}

void nic_stats_Snap( NIC_Stat* s, NICinfo *nic_info )
/* --------------------------------------------------------------------- */
{

/* take a snapshot of the interface and snmp data
*/

  /* local variables */
  int i;
  
    nic_stats_Stop(s);

    nic_stats_Info( s, nic_info);

    /* copy the data from this snapshot to the previous one */
    for (i=0; i<s->num_stats_keep; i++){
        s->nic_stats1[i] = s->nic_stats2[i];
    }
    return;
}

int64 nic_stats_getValue( NICinfo *nic_info, char *stats_name)
/* --------------------------------------------------------------------- */
/*
return the value of the requested NIC Stats
*/
{
  int i;
  
  if(stats_name == NULL) return (-1);
  for(i=0; NIC_Stats_name[i] != NULL; i++){
      if(strcmp(stats_name, NIC_Stats_name[i])==0 ){
          return (nic_info->stats[i]);      // could be -1 if counter not implemented
      }
  }
  /* not found the requested NIC stats name */
  return (-1);
}

void nic_stats_print_info( NICinfo *nic_info, int option, char loc_rem)
/* --------------------------------------------------------------------- */
{
/*
Print NIC Stats info

  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print titles
                  =2 print data
*/
  nic_stats_print_info_file( nic_info, option, loc_rem, stdout);
}

void nic_stats_print_info_file( NICinfo *nic_info, int option, char loc_rem, FILE *output)
/* --------------------------------------------------------------------- */
{
/*
Print NIC Stats info

  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print titles
                  =2 print data
*/
  int i;
  
  switch(option){
    case 1:  // print titles
        fprintf(output, " %c NIC; interface:;", loc_rem);
	for(i=0; NIC_Stats_name[i] != NULL; i++){
	    fprintf(output, " %s;", NIC_Stats_name[i]);
	}
        break;

    case 2:  // print data
        fprintf(output, " %c NIC; %s;", loc_rem, nic_info->if_name);
        for(i=0; i<nic_info->num_stats_keep; i++){
			if(nic_info->stats[i] >=0) {
				fprintf(output, " %" LONG_FORMAT "d;", nic_info->stats[i]);
			}
			else {
				fprintf(output, " - ;");
			}
        }
  
        break;

    }
    return;
}
