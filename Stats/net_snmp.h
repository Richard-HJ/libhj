/*
       net_snmp.h      R. Hughes-Jones  The University of Manchester
                      include file for reading the network interface usage and snmp counters 

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


#include "arch.h"               /* define the architecture */
#include "CPU_net_maxnum.h"     /* CPU & interface names */

#define NET_SNMP_MAX_BUF 4000
#define NET_SNMP_MAX_NAME 16

#define SNMP_V4     1
#define SNMP_V6     2
#define SNMP_BOTH   3

/*
 return to caller variables 
*/
typedef struct _NETIFinfo NETIFinfo;

struct _NETIFinfo {
  char name[NET_SNMP_MAX_NAME];
  int64 pktsin;
  int64 bytesin;
  int64 errin;
  int64 dropin;
  int64 pktsout;
  int64 bytesout; 
  int64 errout;
  int64 dropout;
};

typedef struct _SNMPinfo SNMPinfo;

struct _SNMPinfo {

  int64 InDiscards;
  int64 InReceives;
  int64 OutDiscards;
  int64 OutRequests; 
  int64 UDPInDatagrams;
  int64 UDPInErrors;
  int64 UDPOutDatagrams;
  int64 TCPInSegs;
  int64 TCPOutSegs;
  int64 TCPRetransSegs;
  int64 TCPInErrs;
  int64 TCPOutRsts;
  int64 Ip6InDiscards;
  int64 Ip6InReceives;
  int64 Ip6OutDiscards;
  int64 Ip6OutRequests;
  int64 Udp6InDatagrams;
  int64 Udp6InErrors;
  int64 Udp6OutDatagrams;
  int mode;
};

typedef struct _NET_SNMPStat NET_SNMPStat;

struct _NET_SNMPStat {
    char net1_buf[NET_SNMP_MAX_BUF];
    char snmp1_buf[NET_SNMP_MAX_BUF];
    char snmp1_buf6[NET_SNMP_MAX_BUF];
    NETIFinfo net_info1[NET_SNMP_MAX_IF];
    SNMPinfo snmp_info1;
    char net2_buf[NET_SNMP_MAX_BUF];
    char snmp2_buf[NET_SNMP_MAX_BUF];
    char snmp2_buf6[NET_SNMP_MAX_BUF];
    NETIFinfo net_info2[NET_SNMP_MAX_IF];
    SNMPinfo snmp_info2;
    int snapshot;
    int mode;    /* which snmp stats: = 1 IPv4 only =2 IPv6 only =3 Both */
};


void net_snmp_Init( NET_SNMPStat* s, SNMPinfo *snmp_info, int mode);
void net_snmp_Start( NET_SNMPStat* s);
void net_snmp_Stop( NET_SNMPStat* s);
void net_snmp_Info( NET_SNMPStat* s, NETIFinfo *net_info, SNMPinfo *snmp_info );
void net_snmp_Snap( NET_SNMPStat* s, NETIFinfo *net_info, SNMPinfo *snmp_info );
void net_snmp_print_info( NETIFinfo *net_if_info, SNMPinfo *snmp_info, int option, char loc_rem);
void net_snmp_print_info_file( NETIFinfo *net_if_info, SNMPinfo *snmp_info, int option, char loc_rem, FILE *output);
