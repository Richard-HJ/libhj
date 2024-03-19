/*
       create_raw_socket.c      R. Hughes-Jones  The University of Manchester
                      create a raw (non-IP) socket and set the socket parameters

*/
/*
   Copyright (c) 2002,2003,2004,2005,2006 Richard Hughes-Jones, University of Manchester
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
#include <stdlib.h>

#include <sys/socket.h>         /* for sockaddr ... */
#include <features.h>    /* for the glibc version number */
#if __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>     /* the L2 protocols */
#else
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>   /* The L2 protocols */
#endif
#include <netinet/in.h>          /* for htons() */
#include <netinet/tcp.h>        /* for tcp_info  struct and TCP_INFO ... */

#include "soc_info.h"           /* for socket routines */


void create_raw_socket( int *soc, SOC_INFO *soc_info, int ethpkt_type)
/* --------------------------------------------------------------------- */
{
/* Open the  raw (non-IP) socket.  ( domain or family, socket type, protocol) */

/* Local variables */
    int value;
    int def_value;
    int len;
    int error;

/* Open the raw ethernet socket. 
   PF_PACKET gives  specific to access the link
   SOCK_RAW  raw packets including the link level header
   ethpkt_type is the  IEEE  802.3  protocol number  in network order.

   0x0800   Internet Protocol packet  
   0x0806   Address Resolution packet 
   0x8035   Reverse Addr Res packet 
   0x6000   DEC Assigned proto
   0x6006   DEC Customer use

   SOCK_PACKET is Linux specific to access the link  
*/
 
   *soc = socket( PF_PACKET, SOCK_RAW, htons(ethpkt_type) );
   if (*soc < 0) {
        perror("Error: raw socket failed to open :" );
        exit(-1);
    }

/* get the default socket size options */
   len = sizeof(def_value);
   def_value=0;
   error = getsockopt(*soc, SOL_SOCKET, SO_RCVBUF, &def_value, (socklen_t *) &len);
   if(!soc_info->quiet) printf("Defaults: RCVBUF %d ",def_value );

   len = sizeof(def_value);
   def_value=0;
   error = getsockopt(*soc, SOL_SOCKET, SO_SNDBUF, &def_value, (socklen_t *) &len);
   if(!soc_info->quiet) printf("SNDBUF %d\n ", def_value );



	if(soc_info->soc_buf_size > 0) {
		/* set the Receive Buffer socket size options */
		len = sizeof(value);
		value = soc_info->soc_buf_size;
		error = setsockopt(*soc, SOL_SOCKET, SO_RCVBUF, &value, len);
		/* get the socket options */
		len = sizeof(value);
		value = 0;
		error = getsockopt(*soc, SOL_SOCKET, SO_RCVBUF, &value, (socklen_t *) &len);
		if (error ) {
			perror("Error: setsockopt( SO_RCVBUF) on socket failed :" );
			exit(-1);
		}
#ifdef LINUX_KERN_2_0
		if (value != soc_info->soc_buf_size) {
			printf("Error: setsockopt( SO_RCVBUF) on socket failed :requested %d got [val] %d bytes \n",soc_info->soc_buf_size, value );
			exit(-1);
		}
#else
		if (value/2 != soc_info->soc_buf_size) {
			printf("Error: setsockopt( SO_RCVBUF) on socket failed :requested %d got [val/2] %d bytes \n",soc_info->soc_buf_size, value/2 );
			exit(-1);
		}
#endif
   
		/* set the Send Buffer socket options */
		len = sizeof(value);
		value = soc_info->soc_buf_size;
		error = setsockopt(*soc, SOL_SOCKET, SO_SNDBUF, &value, len);
		/* get the socket options */
		len = sizeof(value);
		value = 0;
		error = getsockopt(*soc, SOL_SOCKET, SO_SNDBUF, &value, (socklen_t *) &len);
		if (error ) {
			perror("Error: setsockopt( SO_SNDBUF) on socket failed :" );
			exit(-1);
		}
#ifdef LINUX_KERN_2_0
		if (value != soc_info->soc_buf_size) {
			printf("Error: setsockopt( SO_SNDBUF) on socket failed :requested %d got [val] %d bytes \n",soc_info->soc_buf_size, value );
			exit(-1);
		}
#else
		if (value/2 != soc_info->soc_buf_size) {
			printf("Error: setsockopt( SO_SNDBUF) on socket failed :requested %d got [val/2] %d bytes \n",soc_info->soc_buf_size, value/2 );
			exit(-1);
		}
#endif
   }

/* print the values that were set */
	if(!soc_info->quiet){
		if(soc_info->soc_buf_size > 0) {
			printf("Settings: RCVBUF %d SNDBUF %d ", soc_info->soc_buf_size, soc_info->soc_buf_size );
		}
		else{
			printf("Settings: RCVBUF no change SNDBUF no change ");
		}
		printf(" \n" );
	}

}
