/*
       net_snmp.c      R. Hughes-Jones  The University of Manchester
                      read the network interface usage and snmp counters 

*/
/*
   Copyright (c) 2002,2003,2004,2005,2006,2007,2008,2009,2010,2013,2014,2015,2016,2017,2018,2019,2020 Richard Hughes-Jones, University of Manchester
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

#include "net_snmp.h"           /* for interface and snmp counters */
#include "CPUStat.h"            /* for CPU usages and interrupt counts */
#include "CPU_net_maxnum.h"

int init_called =0;             /* set to =1 when snmp_init() has been called */

/*
Ip6InMcastPkts
Ip6OutMcastPkts
*/
/* note the order of the names must match that in struct _SNMPinfo */
char *snmp_V6_name[]={ 
    "Ip6InDiscards",
    "Ip6InReceives",
    "Ip6OutDiscards",
    "Ip6OutRequests",
    "Udp6InDatagrams",
    "Udp6InErrors",
    "Udp6OutDatagrams",
    NULL};


/* forward references */
void parse_snmp_text(char *snmp_buf, SNMPinfo *snmp_info );
void parse_snmp6_text(char *str_loc, SNMPinfo *snmp_info );

/*
Information from proc on Kernel 2.4.20

/proc/net/dev
Inter-|   Receive                                                |  Transmit
 face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls
 carrier compressed
    lo:215323545  903504    0    0    0     0          0         0 215323545  903504    0    0    0    
 0       0          0
  eth0:982962021 19644776    0    0    0     0          0    190522 2565513077 40110140    0    0    0 
    0       0          0
  eth1:536283629 1802897920  165  165  138     0          0         0 2237745107 3199981068    0    0  
  0     0       0          0

/proc/net/snmp

/proc/net/snmp6

*/
void net_snmp_Init( NET_SNMPStat* s, SNMPinfo *snmp_info, int mode)
/* --------------------------------------------------------------------- */
{
	/* 
	mode = SNMP_V4 or SNMP_V6
	*/
	s->mode = mode;
	snmp_info->mode =mode;
	init_called =1;
}

void net_snmp_Start( NET_SNMPStat* s)
/* --------------------------------------------------------------------- */
{
  int stats_desc;
  int i;
  int nread;

/* initialise NET_SNMPstat */
    for(i=0;i<NET_SNMP_MAX_IF;i++){
      s->net_info1[i].pktsin   = 0;
      s->net_info1[i].bytesin  = 0;
      s->net_info1[i].errin  = 0;
      s->net_info1[i].dropin  = 0;
      s->net_info1[i].pktsout  = 0;
      s->net_info1[i].bytesout = 0;
      s->net_info1[i].errout  = 0;
      s->net_info1[i].dropout  = 0;

      s->net_info2[i].pktsin   = 0;
      s->net_info2[i].bytesin  = 0;
      s->net_info2[i].errin  = 0;
      s->net_info2[i].dropin  = 0;
      s->net_info2[i].pktsout  = 0;
      s->net_info2[i].bytesout = 0;
      s->net_info2[i].errout  = 0;
      s->net_info2[i].dropout  = 0;
    }

    s->snmp_info1.InReceives     = 0;
    s->snmp_info1.InDiscards     = 0;
    s->snmp_info1.OutRequests    = 0;
    s->snmp_info1.OutDiscards    = 0;
    s->snmp_info1.UDPInDatagrams = 0;
    s->snmp_info1.UDPInErrors    = 0;
    s->snmp_info1.UDPOutDatagrams= 0;
    s->snmp_info1.TCPInSegs      = 0;
    s->snmp_info1.TCPOutSegs     = 0;
    s->snmp_info1.TCPRetransSegs = 0;
    s->snmp_info1.TCPInErrs      = 0;
    s->snmp_info1.TCPOutRsts     = 0;

    s->snmp_info2.InReceives     = 0;
    s->snmp_info2.InDiscards     = 0;
    s->snmp_info2.OutRequests    = 0;
    s->snmp_info2.OutDiscards    = 0;
    s->snmp_info2.UDPInDatagrams = 0;
    s->snmp_info2.UDPInErrors    = 0;
    s->snmp_info2.UDPOutDatagrams= 0;
    s->snmp_info2.TCPInSegs      = 0;
    s->snmp_info2.TCPOutSegs     = 0;
    s->snmp_info2.TCPRetransSegs = 0;
    s->snmp_info2.TCPInErrs      = 0;
    s->snmp_info2.TCPOutRsts     = 0;

 /* UN set the snapshot flag */
  s->snapshot = 0;
/* set default options ie IPv4 only */
  if(init_called ==0){
      s->mode = SNMP_V4;
  }
  s->snmp_info1.mode = s->mode;
  s->snmp_info2.mode = s->mode;

/* open the file with the interface stats and read it all */
    if ( (stats_desc = open(FILE_NETDEV, O_RDONLY) ) == -1) {
        perror("Error: open FILE_NETDEV failed :");   /* descr. is used */
        exit(-1);
    }
    nread = read(stats_desc, s->net1_buf, NET_SNMP_MAX_BUF);
	if(nread == 0) {
        printf("Error: read of stats file failed \n");   
        exit(-1);
    }		
    s->net1_buf[NET_SNMP_MAX_BUF-1] = 0;
    close(stats_desc); 

 /* open the file(s) with the snmp and read it all - always read IPv4 */
	if((s->mode & SNMP_V4) ==  SNMP_V4){
		if ( (stats_desc = open(FILE_NETSNMP, O_RDONLY) ) == -1) {
			perror("Error: open FILE_NETSNMP failed :");   /* descr. is used */
			exit(-1);
		}
		nread = read(stats_desc, s->snmp1_buf, NET_SNMP_MAX_BUF);
		if(nread == 0) {
			printf("Error: read of snmp4 file failed \n");   
			exit(-1);
		}		
		s->snmp1_buf[NET_SNMP_MAX_BUF-1] = 0;
		close(stats_desc); 
	} /* end IPv4 */
    if((s->mode & SNMP_V6) ==  SNMP_V6){
        if ( (stats_desc = open(FILE_NETSNMP6, O_RDONLY) ) == -1) {
			perror("Error: open FILE_NETSNMP6 failed :");   /* descr. is used */
			exit(-1);
		}
		nread = read(stats_desc, s->snmp1_buf6, NET_SNMP_MAX_BUF);
		if(nread == 0) {
			printf("Error: read of snmp6 file failed \n");   
			exit(-1);
		}		
		s->snmp1_buf6[NET_SNMP_MAX_BUF-1] = 0;
		close(stats_desc); 
    } /* end IPv6 */
}


void net_snmp_Stop( NET_SNMPStat* s)
/* --------------------------------------------------------------------- */
{
  int stats_desc;
  int nread;

/* open the file with the interface stats and read it all */
    if ( (stats_desc = open(FILE_NETDEV, O_RDONLY) ) == -1) {
        perror("Error: open FILE_NETDEV failed :");   /* descr. is used */
        exit(-1);
    }
    nread = read(stats_desc, s->net2_buf, NET_SNMP_MAX_BUF);
	if(nread == 0) {
        printf("Error: read of stats file failed \n");   
        exit(-1);
    }		
    s->net2_buf[NET_SNMP_MAX_BUF-1] = 0;
    close(stats_desc); 
 
/* open the file(s) with the snmp and read it all - always read IPv4 */
	if((s->mode & SNMP_V4) ==  SNMP_V4){
		if ( (stats_desc = open(FILE_NETSNMP, O_RDONLY) ) == -1) {
			perror("Error: open FILE_NETSNMP failed :");   /* descr. is used */
			exit(-1);
		}
		nread = read(stats_desc, s->snmp2_buf, NET_SNMP_MAX_BUF);
		if(nread == 0) {
			printf("Error: read of snmp4 file failed \n");   
			exit(-1);
		}		
		s->snmp2_buf[NET_SNMP_MAX_BUF-1] = 0;
		close(stats_desc); 
	} /* end IPv4 */
    if((s->mode & SNMP_V6) ==  SNMP_V6){
		if ( (stats_desc = open(FILE_NETSNMP6, O_RDONLY) ) == -1) {
	    perror("Error: open FILE_NETSNMP6 failed :");   /* descr. is used */
	    exit(-1);
		}
		nread = read(stats_desc, s->snmp2_buf6, NET_SNMP_MAX_BUF);
		if(nread == 0) {
			printf("Error: read of snmp6 file failed \n");   
			exit(-1);
		}		
		s->snmp2_buf6[NET_SNMP_MAX_BUF-1] = 0;
		close(stats_desc); 
    } /* end IPv6 */
}


void net_snmp_Info( NET_SNMPStat* s,  NETIFinfo *net_if_info, SNMPinfo *snmp_info )
/* --------------------------------------------------------------------- */
{
  int i;
  char *str_loc, *str_name;
  char name1[64];                               /* hold the interface name eg eth1 */
  int64 pktsin, pktsout, bytesin, bytesout;     /* interface counter values */
  int64 errin, errout, dropin, dropout;     /* interface counter values */
  int64 num2, num3, num4, num5 ;    /* dummy */
  /* the lines about wrap32bit are to get round compiler sign issues */
  int64 wrap32bit = 2147483647;
  wrap32bit = wrap32bit*2 +1;
  int size;

/* initalise */
    num2 =0;
    num3 =0;
    num4 =0;
    num5 =0;

/* process interrupt information from net_snmp_Start() */
    if(s->snapshot ==0){
/* scan the string read from /proc/net/dev */
    i=0;
    str_loc = s->net1_buf;
/* look for "\n" skip 2 text lines then read each line, look for the network interfaces */
    str_loc = strstr(str_loc, "\n");
    if(str_loc == NULL)goto END_UP;
    str_loc++;

    for(;;){
        str_name = strstr(str_loc, "\n");
		if(str_name == NULL)break;
		str_name++;
        str_loc = strstr(str_name, ":");
		if(str_loc == NULL)break;
		strncpy(&name1[0], str_name, (size_t)(str_loc - str_name));
		name1[(int)(str_loc - str_name)] =0;
		str_loc++;
		sscanf(str_loc, "%"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d ", 
			&bytesin, &pktsin, &errin,&dropin,&num2,&num3,&num4,&num5,
			&bytesout, &pktsout, &errout,&dropout );

		if(i<NET_SNMP_MAX_IF){
			s->net_info1[i].pktsin = pktsin;
			s->net_info1[i].bytesin = bytesin;
			s->net_info1[i].errin = errin;
			s->net_info1[i].dropin = dropin;
			s->net_info1[i].pktsout = pktsout;
			s->net_info1[i].bytesout = bytesout;
			s->net_info1[i].errout = errout;
			s->net_info1[i].dropout = dropout;
//			strncpy(&s->net_info1[i].name[0], name1, NET_SNMP_MAX_NAME);
			size = strnlen(name1, NET_SNMP_MAX_NAME-1);
			memcpy(&s->net_info1[i].name[0], name1, size);
			s->net_info1[i].name[NET_SNMP_MAX_NAME-1] =0; // just in case
			i++;
		}
    }  /* end of for(;;) */
 

/* scan the string read from /proc/net/snmp */
	if((s->mode & SNMP_V4) ==  SNMP_V4) parse_snmp_text(&s->snmp1_buf[0], &s->snmp_info1);

/* scan the string read from /proc/net/snmp6 */
	if((s->mode & SNMP_V6) ==  SNMP_V6) parse_snmp6_text(&s->snmp1_buf6[0], &s->snmp_info1);

    } /* end of check if have to process the snmp_Start() data */


/* process interrupt information from net_snmp_Stop() */
/* scan the string read from /proc/net/dev */
    i=0;
    str_loc = s->net2_buf;
/* look for "\n" skip 2 text lines then read each line, look for the network interfaces */
    str_loc = strstr(str_loc, "\n");
    if(str_loc == NULL)goto END_UP;
    str_loc++;

    for(;;){
        str_name = strstr(str_loc, "\n");
		if(str_name == NULL)break;
		str_name++;
        str_loc = strstr(str_name, ":");
		if(str_loc == NULL)break;
		strncpy(&name1[0], str_name, (size_t)(str_loc - str_name));
		str_loc++;
		sscanf(str_loc, "%"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d ", 
			&bytesin, &pktsin, &errin,&dropin,&num2,&num3,&num4,&num5,
			&bytesout, &pktsout, &errout,&dropout );


		if(i<NET_SNMP_MAX_IF){
			s->net_info2[i].pktsin = pktsin;
			s->net_info2[i].bytesin = bytesin;
			s->net_info2[i].errin = errin;
			s->net_info2[i].dropin = dropin;
			s->net_info2[i].pktsout = pktsout;
			s->net_info2[i].bytesout = bytesout;
			s->net_info2[i].errout = errout;
			s->net_info2[i].dropout = dropout;
//			strncpy(&s->net_info2[i].name[0], name1, NET_SNMP_MAX_NAME);
			size = strnlen(name1, NET_SNMP_MAX_NAME-1);
			memcpy(&s->net_info2[i].name[0], name1, size);
			s->net_info2[i].name[NET_SNMP_MAX_NAME-1] =0; // just in case
			i++;
		}
    }  /* end of for(;;) */

/* scan the string read from /proc/net/snmp */
    if((s->mode & SNMP_V4) ==  SNMP_V4) parse_snmp_text(&s->snmp2_buf[0], &s->snmp_info2);

/* scan the string read from /proc/net/snmp6 */
    if((s->mode & SNMP_V6) ==  SNMP_V6) parse_snmp6_text(&s->snmp2_buf6[0], &s->snmp_info2);

 END_UP:

/* return information - the if(x<0) test allows for 32bit counter wrap-round */
	/* interface counters */
    for(i=0; i<NET_SNMP_MAX_IF; i++){
        net_if_info[i].name[0] = 0;
		if(s->net_info1[i].name[0] != 0){
			strcpy(&net_if_info[i].name[0], &s->net_info1[i].name[0]);
			net_if_info[i].pktsin   = s->net_info2[i].pktsin   - s->net_info1[i].pktsin;
			if(net_if_info[i].pktsin < 0) net_if_info[i].pktsin = net_if_info[i].pktsin + wrap32bit;
			net_if_info[i].pktsout  = s->net_info2[i].pktsout  - s->net_info1[i].pktsout;
			if(net_if_info[i].pktsout < 0) net_if_info[i].pktsout = net_if_info[i].pktsout + wrap32bit;
			
			net_if_info[i].bytesin  = s->net_info2[i].bytesin  - s->net_info1[i].bytesin;
			if(net_if_info[i].bytesin < 0) net_if_info[i].bytesin = net_if_info[i].bytesin + wrap32bit;
			net_if_info[i].bytesout = s->net_info2[i].bytesout - s->net_info1[i].bytesout;
			if(net_if_info[i].bytesout < 0) net_if_info[i].bytesout = net_if_info[i].bytesout + wrap32bit;
			
			net_if_info[i].errin   = s->net_info2[i].errin   - s->net_info1[i].errin;
			if(net_if_info[i].errin < 0) net_if_info[i].errin = net_if_info[i].errin + wrap32bit;
			net_if_info[i].errout  = s->net_info2[i].errout  - s->net_info1[i].errout;
			if(net_if_info[i].errout < 0) net_if_info[i].errout = net_if_info[i].errout + wrap32bit;

			net_if_info[i].dropin   = s->net_info2[i].dropin   - s->net_info1[i].dropin;
			if(net_if_info[i].dropin < 0) net_if_info[i].dropin = net_if_info[i].dropin + wrap32bit;
			net_if_info[i].dropout  = s->net_info2[i].dropout  - s->net_info1[i].dropout;
			if(net_if_info[i].dropout < 0) net_if_info[i].dropout = net_if_info[i].dropout + wrap32bit;
			
       }
       else{
			net_if_info[i].pktsin = 0;
			net_if_info[i].pktsout = 0;
			net_if_info[i].errin = 0;
			net_if_info[i].errout = 0;
			net_if_info[i].bytesin = 0;
			net_if_info[i].bytesout = 0;
			net_if_info[i].dropin = 0;
			net_if_info[i].dropout = 0;
       }
    }
	
    /*snmp */
	snmp_info->mode = s->mode;
	
    snmp_info->InReceives     = s->snmp_info2.InReceives     - s->snmp_info1.InReceives;
    if(snmp_info->InReceives <0) snmp_info->InReceives = snmp_info->InReceives + wrap32bit;
    snmp_info->InDiscards     = s->snmp_info2.InDiscards     - s->snmp_info1.InDiscards;
    if(snmp_info->InDiscards <0) snmp_info->InDiscards = snmp_info->InDiscards + wrap32bit;
    snmp_info->OutRequests    = s->snmp_info2.OutRequests    - s->snmp_info1.OutRequests;
    if(snmp_info->OutRequests <0) snmp_info->OutRequests = snmp_info->OutRequests + wrap32bit;
    snmp_info->OutDiscards    = s->snmp_info2.OutDiscards    - s->snmp_info1.OutDiscards;
    if(snmp_info->OutDiscards <0) snmp_info->OutDiscards = snmp_info->OutDiscards + wrap32bit;
    snmp_info->UDPInDatagrams = s->snmp_info2.UDPInDatagrams - s->snmp_info1.UDPInDatagrams;
    if(snmp_info->UDPInDatagrams <0) snmp_info->UDPInDatagrams = snmp_info->UDPInDatagrams + wrap32bit;
    snmp_info->UDPInErrors    = s->snmp_info2.UDPInErrors    - s->snmp_info1.UDPInErrors;
    if(snmp_info->UDPInErrors <0) snmp_info->UDPInErrors = snmp_info->UDPInErrors + wrap32bit;
    snmp_info->UDPOutDatagrams = s->snmp_info2.UDPOutDatagrams - s->snmp_info1.UDPOutDatagrams;
    if(snmp_info->UDPOutDatagrams <0) snmp_info->UDPOutDatagrams = snmp_info->UDPOutDatagrams + wrap32bit;
    snmp_info->TCPInSegs      = s->snmp_info2.TCPInSegs      - s->snmp_info1.TCPInSegs;
    if(snmp_info->TCPInSegs <0) snmp_info->TCPInSegs = snmp_info->TCPInSegs + wrap32bit;
    snmp_info->TCPOutSegs     = s->snmp_info2.TCPOutSegs     - s->snmp_info1.TCPOutSegs;
    if(snmp_info->TCPOutSegs <0) snmp_info->TCPOutSegs = snmp_info->TCPOutSegs + wrap32bit;
    snmp_info->TCPRetransSegs = s->snmp_info2.TCPRetransSegs - s->snmp_info1.TCPRetransSegs ;
    if(snmp_info->TCPRetransSegs <0) snmp_info->TCPRetransSegs = snmp_info->TCPRetransSegs + wrap32bit;
    snmp_info->TCPInErrs      = s->snmp_info2.TCPInErrs      - s->snmp_info1.TCPInErrs;
    if(snmp_info->TCPInErrs <0) snmp_info->TCPInErrs = snmp_info->TCPInErrs + wrap32bit;
    snmp_info->TCPOutRsts     = s->snmp_info2.TCPOutRsts     - s->snmp_info1.TCPOutRsts;
    if(snmp_info->TCPOutRsts <0) snmp_info->TCPOutRsts = snmp_info->TCPOutRsts + wrap32bit;
    snmp_info->Ip6InReceives     = s->snmp_info2.Ip6InReceives     - s->snmp_info1.Ip6InReceives;
    if(snmp_info->Ip6InReceives <0) snmp_info->Ip6InReceives = snmp_info->Ip6InReceives + wrap32bit;
    snmp_info->Ip6InDiscards     = s->snmp_info2.Ip6InDiscards     - s->snmp_info1.Ip6InDiscards;
    if(snmp_info->Ip6InDiscards <0) snmp_info->Ip6InDiscards = snmp_info->Ip6InDiscards + wrap32bit;
    snmp_info->Ip6OutRequests    = s->snmp_info2.Ip6OutRequests    - s->snmp_info1.Ip6OutRequests;
    if(snmp_info->Ip6OutRequests <0) snmp_info->Ip6OutRequests = snmp_info->Ip6OutRequests + wrap32bit;
    snmp_info->Ip6OutDiscards    = s->snmp_info2.Ip6OutDiscards    - s->snmp_info1.Ip6OutDiscards;
    if(snmp_info->Ip6OutDiscards <0) snmp_info->Ip6OutDiscards = snmp_info->Ip6OutDiscards + wrap32bit;
    snmp_info->Udp6InDatagrams = s->snmp_info2.Udp6InDatagrams - s->snmp_info1.Udp6InDatagrams;
    if(snmp_info->Udp6InDatagrams <0) snmp_info->Udp6InDatagrams = snmp_info->Udp6InDatagrams + wrap32bit;
    snmp_info->Udp6InErrors    = s->snmp_info2.Udp6InErrors    - s->snmp_info1.Udp6InErrors;
    if(snmp_info->Udp6InErrors <0) snmp_info->Udp6InErrors = snmp_info->Udp6InErrors + wrap32bit;
    snmp_info->Udp6OutDatagrams = s->snmp_info2.Udp6OutDatagrams - s->snmp_info1.Udp6OutDatagrams;
    if(snmp_info->Udp6OutDatagrams <0) snmp_info->Udp6OutDatagrams = snmp_info->Udp6OutDatagrams + wrap32bit;
  
}

void parse_snmp_text(char *str_loc, SNMPinfo *snmp_info )
/* --------------------------------------------------------------------- */
{
  /* read through the lines of snmp text in str_loc
the lines look like:-
Ip: Forwarding DefaultTTL InReceives InHdrErrors InAddrErrors ForwDatagrams InUnknownProtos InDiscards InDelivers OutRequests OutDiscards OutNoRoutes R
easmTimeout ReasmReqds ReasmOKs ReasmFails FragOKs FragFails FragCreates
Ip: 2 64 884053 0 1411 0 0 0 881504 103684 0 0 0 0 0 0 0 0 0
Icmp: InMsgs InErrors InDestUnreachs InTimeExcds InParmProbs InSrcQuenchs InRedirects InEchos InEchoReps InTimestamps InTimestampReps InAddrMasks InAdd
rMaskReps OutMsgs OutErrors OutDestUnreachs OutTimeExcds OutParmProbs OutSrcQuenchs OutRedirects OutEchos OutEchoReps OutTimestamps OutTimestampReps Ou
tAddrMasks OutAddrMaskReps
Icmp: 1149 0 1149 0 0 0 0 0 0 0 0 0 0 1149 0 1149 0 0 0 0 0 0 0 0 0 0
IcmpMsg: InType3 OutType3
IcmpMsg: 1149 1149
Tcp: RtoAlgorithm RtoMin RtoMax MaxConn ActiveOpens PassiveOpens AttemptFails EstabResets CurrEstab InSegs OutSegs RetransSegs InErrs OutRsts
Tcp: 1 200 120000 -1 14 1 11 1 0 876268 97268 2 0 5
Udp: InDatagrams NoPorts InErrors OutDatagrams RcvbufErrors SndbufErrors
Udp: 4091 0 0 5267 0 0
UdpLite: InDatagrams NoPorts InErrors OutDatagrams RcvbufErrors SndbufErrors
UdpLite: 0 0 0 0 0 0

     take the first token on the line eg Ip: or Tcp: or Udp: and see if we want this
     if so - skip this line of text and read the next line of data.
     if not - skip this line of text
  */

  int text_len;
  char *text_end;
  char *delim_loc;
  char name[16];
  char name1[64];                               /* hold the interface name eg eth1 */
  int64 num0, num1, num2, num3, num4, num5, num6 ;    /* dummy */

/* initalise */
    num0 =0;
    num1 =0;
    num2 =0;
    num3 =0;
    num4 =0;
    num5 =0;

    text_len = strlen(str_loc);
    text_end = str_loc + text_len;
    while(str_loc < text_end){
        /* look for ":" - the end of the first token */
        delim_loc = strstr(str_loc, ":");
	if(delim_loc == NULL)goto END_UP;
	*delim_loc =0;
	strncpy(name, str_loc, 16);
	name[15]=0;
	str_loc = delim_loc +1;

	if(strcmp(name, "Ip") ==0){
	    /* skip the line of text */
	    str_loc = strstr(str_loc, "\n");
	    if(str_loc == NULL)goto END_UP;
	    str_loc++;
	    sscanf(str_loc, "%s %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d  %"LONG_FORMAT"d ", 
		   &name1[0], &num0,&num1, &snmp_info->InReceives,  &num2,&num3,&num4,&num5, 
		   &snmp_info->InDiscards, &num6, &snmp_info->OutRequests, &snmp_info->OutDiscards );
	    /* skip to the end of the line */
	    str_loc = strstr(str_loc, "\n");
	    if(str_loc == NULL)goto END_UP;
	    str_loc++;
	}

	else if(strcmp(name, "Tcp") ==0){
	    /* skip the line of text */
	    str_loc = strstr(str_loc, "\n");
	    if(str_loc == NULL)goto END_UP;
	    str_loc++;
	    sscanf(str_loc, "%s %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d", 
		   &name1[0], &num0,&num1,&num2,&num3,&num4,&num5,&num0,&num1,&num2, 
		   &snmp_info->TCPInSegs, &snmp_info->TCPOutSegs, &snmp_info->TCPRetransSegs,
		   &snmp_info->TCPInErrs, &snmp_info->TCPOutRsts);

	    /* skip to the end of the line */
	    str_loc = strstr(str_loc, "\n");
	    if(str_loc == NULL)goto END_UP;
	    str_loc++;
	}

	else if(strcmp(name, "Udp") ==0){
	    /* skip the line of text */
	    str_loc = strstr(str_loc, "\n");
	    if(str_loc == NULL)goto END_UP;
	    str_loc++;
	    sscanf(str_loc, "%s %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d %"LONG_FORMAT"d ",
		   &name1[0],  &snmp_info->UDPInDatagrams,  &num0,  
		   &snmp_info->UDPInErrors, &snmp_info->UDPOutDatagrams);
	    
	    /* skip to the end of the line */
	    str_loc = strstr(str_loc, "\n");
	    if(str_loc == NULL)goto END_UP;
	    str_loc++;
	}
	
	else{
	    /* dont want this - skip the line */
	    str_loc = strstr(str_loc, "\n");
	    if(str_loc == NULL)goto END_UP;
	    str_loc++;
	}
    }

 END_UP:
    return;
}

void parse_snmp6_text(char *str_loc, SNMPinfo *snmp_info )
/* --------------------------------------------------------------------- */
{
  int text_len;
  char *text_end;
  char name[64];
  int64 num0;
  int64 *num_ptr;
  int i;

/* initalise */
    num0 =0;
    num_ptr = &snmp_info->Ip6InDiscards;

    text_len = strlen(str_loc);
    text_end = str_loc + text_len;
    while(str_loc < text_end){

		sscanf(str_loc, "%s %"LONG_FORMAT"d ", &name[0], &num0);
		for(i=0; snmp_V6_name[i]!=NULL; i++){
			if(strcmp(name, snmp_V6_name[i]) ==0){
				*(num_ptr +i) = num0;
				//printf("i=%d  %s %"LONG_FORMAT"d \n", i, name, num0);
			}
		}	
		/* skip to the end of the line */
		str_loc = strstr(str_loc, "\n");
		if(str_loc == NULL)goto END_UP;
		str_loc++;
    }

 END_UP:
    return;
}

void net_snmp_Snap( NET_SNMPStat* s,  NETIFinfo *net_if_info, SNMPinfo *snmp_info )
/* --------------------------------------------------------------------- */
{
  /* take a snapshot of the interface and snmp data
return the delta counts since net_snmp_Start() or the last net_snmp_Snap()
Allow for 32 bit counter wrap
  */

  /* loacl variables */
  int i;


  net_snmp_Stop(s);

  net_snmp_Info( s, net_if_info, snmp_info );

  /* set the snapshot flag - helps not decode the net_snmp_Start() data */
  s->snapshot = 1;

  /* copy the data from this snapshot to the previous one */

    for(i=0; i<NET_SNMP_MAX_IF; i++){
       if(s->net_info1[i].name[0] != 0){
	    s->net_info1[i].pktsin   = s->net_info2[i].pktsin ;
	    s->net_info1[i].pktsout  = s->net_info2[i].pktsout;
	    s->net_info1[i].bytesin  = s->net_info2[i].bytesin;
	    s->net_info1[i].bytesout = s->net_info2[i].bytesout;
	    s->net_info1[i].errin    = s->net_info2[i].errin ;
	    s->net_info1[i].errout   = s->net_info2[i].errout;
	    s->net_info1[i].dropin   = s->net_info2[i].dropin ;
	    s->net_info1[i].dropout  = s->net_info2[i].dropout;
       }
       else{
	   s->net_info1[i].pktsin = 0;
	   s->net_info1[i].pktsout = 0;
	   s->net_info1[i].bytesin = 0;
	   s->net_info1[i].bytesout = 0;
	   s->net_info1[i].errin = 0;
	   s->net_info1[i].errout = 0;
	   s->net_info1[i].dropin = 0;
	   s->net_info1[i].dropout = 0;
       }
    }
    /*snmp */
    s->snmp_info1.InReceives     = s->snmp_info2.InReceives;
    s->snmp_info1.InDiscards     = s->snmp_info2.InDiscards;
    s->snmp_info1.OutRequests    = s->snmp_info2.OutRequests;
    s->snmp_info1.OutDiscards    = s->snmp_info2.OutDiscards;
    s->snmp_info1.UDPInDatagrams = s->snmp_info2.UDPInDatagrams;
    s->snmp_info1.UDPInErrors    = s->snmp_info2.UDPInErrors;
    s->snmp_info1.UDPOutDatagrams = s->snmp_info2.UDPOutDatagrams;
    s->snmp_info1.TCPInSegs      = s->snmp_info2.TCPInSegs;
    s->snmp_info1.TCPOutSegs     = s->snmp_info2.TCPOutSegs;
    s->snmp_info1.TCPRetransSegs = s->snmp_info2.TCPRetransSegs;
    s->snmp_info1.TCPInErrs      = s->snmp_info2.TCPInErrs;
    s->snmp_info1.TCPOutRsts     = s->snmp_info2.TCPOutRsts;

    s->snmp_info1.Ip6InReceives     = s->snmp_info2.Ip6InReceives;
    s->snmp_info1.Ip6InDiscards     = s->snmp_info2.Ip6InDiscards;
    s->snmp_info1.Ip6OutRequests    = s->snmp_info2.Ip6OutRequests;
    s->snmp_info1.Ip6OutDiscards    = s->snmp_info2.Ip6OutDiscards;
    s->snmp_info1.Udp6InDatagrams   = s->snmp_info2.Udp6InDatagrams;
    s->snmp_info1.Udp6InErrors      = s->snmp_info2.Udp6InErrors;
    s->snmp_info1.Udp6OutDatagrams  = s->snmp_info2.Udp6OutDatagrams;

}

void net_snmp_print_info( NETIFinfo *net_if_info, SNMPinfo *snmp_info, int option, char loc_rem)
/* --------------------------------------------------------------------- */
{
/*
Print interface & snmp info

  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print UDP titles
                  =2 print UDP data
                  =3 print TCP titles
                  =4 print TCP data
                  =5 print both UDP & TCP titles
                  =6 print both UDP & TCP data
*/
  net_snmp_print_info_file( net_if_info, snmp_info, option, loc_rem, stdout);
}

void net_snmp_print_info_file_OLD( NETIFinfo *net_if_info, SNMPinfo *snmp_info, int option, char loc_rem, FILE *output)
/* --------------------------------------------------------------------- */
{
/*
Print interface & snmp info

  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print UDP titles
                  =2 print UDP data
                  =3 print TCP titles
                  =4 print TCP data
                  =5 print both UDP & TCP titles
                  =6 print both UDP & TCP data
*/
  int i;

/* set default options ie IPv4 only */
  if(init_called ==0){
      snmp_info->mode = SNMP_V4;
  }

  /* common output */
  switch(option){
  case 1:  // print UDP titles
  case 5:  // print both UDP & TCP titles
    /* interface titles */
    fprintf(output, " %c if; interface:; pktsin; bytesin; errin; dropin; pktsout; bytesout; errout; dropout;", loc_rem);
	
    /* snmp titles IPv4 */
	if((snmp_info->mode & SNMP_V4) ==  SNMP_V4) {
		fprintf(output, " %c snmp; InReceives; InDiscards; OutRequests; OutDiscards;", loc_rem); 
		fprintf(output, " UDPInDatagrams; UDPInErrors; UDPOutDatagrams;");
	}
	
	/* snmp titles IPv6 */
	if((snmp_info->mode & SNMP_V6) ==  SNMP_V6) {
		fprintf(output, " %c snmp6;", loc_rem); 
		for(i=0; snmp_V6_name[i] != NULL  ; i++){
			fprintf(output, " %s;", snmp_V6_name[i]); 
		}
	}
      break;
  case 3:  // print TCP titles
    /* interface titles */
    fprintf(output, " %c if; interface:; pktsin; bytesin; errin; dropin; pktsout; bytesout; errout; dropout;", loc_rem);

	/* SNMP TCP titles */
		fprintf(output, "%c TCP;", loc_rem);
		fprintf(output, " TCPInSegs; TCPInErrs; TCPOutSegs; TCPOutRsts; TCPRetransSegs;"); 
 	  
      break;

  case 2:  // print UDP data
  case 6:  // print both UDP & TCP data
      /* interface counts */
      fprintf(output, "%c if;", loc_rem);
	for (i=0; i<NET_SNMP_MAX_IF; i++){
		if(net_if_info[i].name[0] != 0){
			fprintf(output, "%s: ; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;", 
				&net_if_info[i].name[0], 
				net_if_info[i].pktsin, net_if_info[i].bytesin,
				net_if_info[i].errin, net_if_info[i].dropin,
				net_if_info[i].pktsout, net_if_info[i].bytesout,
				net_if_info[i].errout, net_if_info[i].dropout);
		}
	}

     /* snmp counts IPv4 */
	if((snmp_info->mode & SNMP_V4) ==  SNMP_V4) {
		fprintf(output, "%c snmp;", loc_rem);
		fprintf(output, " %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;",
				snmp_info->InReceives, snmp_info->InDiscards,
				snmp_info->OutRequests, snmp_info->OutDiscards);
		fprintf(output, " %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;", 
				snmp_info->UDPInDatagrams, snmp_info->UDPInErrors, snmp_info->UDPOutDatagrams);

	}
	
     /* snmp counts IPv6 */
	if((snmp_info->mode & SNMP_V6) ==  SNMP_V6) {
		fprintf(output, " %c snmp6;", loc_rem); 
		fprintf(output, " %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;",
				snmp_info->Ip6InReceives, snmp_info->Ip6InDiscards,
				snmp_info->Ip6OutRequests, snmp_info->Ip6OutDiscards);
		fprintf(output, " %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;", 
				snmp_info->Udp6InDatagrams, snmp_info->Udp6InErrors, snmp_info->Udp6OutDatagrams);
	}
      break;
	  
  case 4:  // print TCP data
	/* interface counts */
	fprintf(output, "%c if;", loc_rem);
	for (i=0; i<NET_SNMP_MAX_IF; i++){
		if(net_if_info[i].name[0] != 0){
			fprintf(output, "%s: ; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;", 
				&net_if_info[i].name[0], 
				net_if_info[i].pktsin, net_if_info[i].bytesin,
				net_if_info[i].errin, net_if_info[i].dropin,
				net_if_info[i].pktsout, net_if_info[i].bytesout,
				net_if_info[i].errout, net_if_info[i].dropout);
		}
	}

	// print TCP data   
	fprintf(output, "%c TCP;", loc_rem);
	fprintf(output, " %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;", 
			snmp_info->TCPInSegs, snmp_info->TCPInErrs, snmp_info->TCPOutSegs, 
			snmp_info->TCPOutRsts, snmp_info->TCPRetransSegs);
      break;
  }

  return;

} /* end of  net_snmp_print_info */


void net_print_info( NETIFinfo *net_if_info, SNMPinfo *snmp_info, int option, char loc_rem)
/* --------------------------------------------------------------------- */
{
/*
Print interface info

  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print interface info titles
                  =2 print interface data
                  =3 print titles for a table
                  =4 print data in a table
*/
	net_print_info_file( net_if_info, snmp_info, option, loc_rem, stdout);
}

void net_print_info_file( NETIFinfo *net_if_info, SNMPinfo *snmp_info, int option, char loc_rem, FILE *output)
/* --------------------------------------------------------------------- */
{
/*
Print interface info

  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print interface info titles
                  =2 print interface data
                  =3 print titles for a table
                  =4 print data in a table
*/
  int i;


	switch(option){
		case 1:  // print net i/f titles
		/* interface titles */
			fprintf(output, " %c if; interface:; pktsin; bytesin; errin; dropin; pktsout; bytesout; errout; dropout;", loc_rem);
		break;

		case 2:  // print net i/f data
		/* interface counts */
			fprintf(output, "%c if;", loc_rem);
			for (i=0; i<NET_SNMP_MAX_IF; i++){
				if(net_if_info[i].name[0] != 0){
					fprintf(output, "%s: ; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;", 
					&net_if_info[i].name[0], 
					net_if_info[i].pktsin, net_if_info[i].bytesin,
					net_if_info[i].errin, net_if_info[i].dropin,
					net_if_info[i].pktsout, net_if_info[i].bytesout,
					net_if_info[i].errout, net_if_info[i].dropout);
				}
			}
		break;
		
		case 3:  // print titles for a table
			fprintf(output, " %c if; interface:; pktsin; bytesin; errin; dropin; pktsout; bytesout; errout; dropout;", loc_rem);
			fprintf(output, "\n");
		break;

		case 4:  // print data in a table
			for (i=0; i<NET_SNMP_MAX_IF; i++){
				if(net_if_info[i].name[0] != 0){
					fprintf(output, "%s: ; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;", 
					&net_if_info[i].name[0], 
					net_if_info[i].pktsin, net_if_info[i].bytesin,
					net_if_info[i].errin, net_if_info[i].dropin,
					net_if_info[i].pktsout, net_if_info[i].bytesout,
					net_if_info[i].errout, net_if_info[i].dropout);	
					fprintf(output, "\n");
				}
			}
		break;
	}

  return;

} /* end of  net_print_info_file */

void snmp_print_info( NETIFinfo *net_if_info, SNMPinfo *snmp_info, int option, char loc_rem)
/* --------------------------------------------------------------------- */
{
/*
Print snmp info

  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print UDP titles
                  =2 print UDP data
                  =3 print TCP titles
                  =4 print TCP data
                  =5 print both UDP & TCP titles
                  =6 print both UDP & TCP data
*/
	snmp_print_info_file( net_if_info, snmp_info, option, loc_rem, stdout);
}

void snmp_print_info_file( NETIFinfo *net_if_info, SNMPinfo *snmp_info, int option, char loc_rem, FILE *output)
/* --------------------------------------------------------------------- */
{
/*
Print snmp info

  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print UDP titles
                  =2 print UDP data
                  =3 print TCP titles
                  =4 print TCP data
                  =5 print both UDP & TCP titles
                  =6 print both UDP & TCP data
*/
  int i;

/* set default options ie IPv4 only */
	if(init_called ==0){
		snmp_info->mode = SNMP_V4;
	}

	switch(option){
		case 1:  // print UDP titles
		case 5:  // print both UDP & TCP titles
			/* snmp titles IPv4 */
			if((snmp_info->mode & SNMP_V4) ==  SNMP_V4) {
				fprintf(output, " %c snmp; InReceives; InDiscards; OutRequests; OutDiscards;", loc_rem); 
				fprintf(output, " UDPInDatagrams; UDPInErrors; UDPOutDatagrams;");
			}
	
			/* snmp titles IPv6 */
			if((snmp_info->mode & SNMP_V6) ==  SNMP_V6) {
				fprintf(output, " %c snmp6;", loc_rem); 
				for(i=0; snmp_V6_name[i] != NULL  ; i++){
					fprintf(output, " %s;", snmp_V6_name[i]); 
				}
			}
		break;
		
		case 3:  // print TCP titles
			/* SNMP TCP titles */
			fprintf(output, "%c TCP;", loc_rem);
			fprintf(output, " TCPInSegs; TCPInErrs; TCPOutSegs; TCPOutRsts; TCPRetransSegs;");  
		break;

		case 2:  // print UDP data
		case 6:  // print both UDP & TCP data
			/* snmp counts IPv4 */
			if((snmp_info->mode & SNMP_V4) ==  SNMP_V4) {
				fprintf(output, "%c snmp;", loc_rem);
				fprintf(output, " %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;",
					snmp_info->InReceives, snmp_info->InDiscards,
					snmp_info->OutRequests, snmp_info->OutDiscards);
				fprintf(output, " %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;", 
					snmp_info->UDPInDatagrams, snmp_info->UDPInErrors, snmp_info->UDPOutDatagrams);
			}	
	
			/* snmp counts IPv6 */
			if((snmp_info->mode & SNMP_V6) ==  SNMP_V6) {
				fprintf(output, " %c snmp6;", loc_rem); 
				fprintf(output, " %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;",
					snmp_info->Ip6InReceives, snmp_info->Ip6InDiscards,
					snmp_info->Ip6OutRequests, snmp_info->Ip6OutDiscards);
				fprintf(output, " %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;", 
					snmp_info->Udp6InDatagrams, snmp_info->Udp6InErrors, snmp_info->Udp6OutDatagrams);
			}
		break;
	  
		case 4:  // print TCP data
			// print TCP data   
			fprintf(output, "%c TCP;", loc_rem);
			fprintf(output, " %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d; %" LONG_FORMAT "d;", 
				snmp_info->TCPInSegs, snmp_info->TCPInErrs, snmp_info->TCPOutSegs, 
				snmp_info->TCPOutRsts, snmp_info->TCPRetransSegs);
		break;
	}

	return;

} /* end of  snmp_print_info_file */


void net_snmp_print_info_file( NETIFinfo *net_if_info, SNMPinfo *snmp_info, int option, char loc_rem, FILE *output)
/* --------------------------------------------------------------------- */
{
/*
Print interface & snmp info

  loc_rem         = 'L' for local or 'R' for remote data
  option          =1 print UDP titles
                  =2 print UDP data
                  =3 print TCP titles
                  =4 print TCP data
                  =5 print both UDP & TCP titles
                  =6 print both UDP & TCP data
*/

/* set default options ie IPv4 only */
	if(init_called ==0){
		snmp_info->mode = SNMP_V4;
	}

	/* common output */
	switch(option){
		case 1:  // print UDP titles
		case 5:  // print both UDP & TCP titles
			/* interface titles */
			net_print_info_file( net_if_info, snmp_info, 1, loc_rem, stdout);	
			/* snmp UDP titles IPv4 or IPv6 */
			snmp_print_info_file( net_if_info, snmp_info, 1, loc_rem, stdout);	
		break;

		case 3:  // print TCP titles
			/* interface titles */
			net_print_info_file( net_if_info, snmp_info, 1, loc_rem, stdout);
			/* SNMP TCP titles */
			snmp_print_info_file( net_if_info, snmp_info, 3, loc_rem, stdout);		  
		break;

		case 2:  // print UDP data
		case 6:  // print both UDP & TCP data
			/* interface counts */
			net_print_info_file( net_if_info, snmp_info, 2, loc_rem, stdout);

			/* snmp counts */
			snmp_print_info_file( net_if_info, snmp_info, 2, loc_rem, stdout);		  
		break;
	  
		case 4:  // print snmp TCP data
			/* interface counts */
			net_print_info_file( net_if_info, snmp_info, 2, loc_rem, stdout);

			// print TCP data   
			snmp_print_info_file( net_if_info, snmp_info, 4, loc_rem, stdout);		  
		break;
	}

	return;

} /* end of  net_snmp_print_info */
