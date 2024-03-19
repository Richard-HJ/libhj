/*
       create_udp_socket.c      R. Hughes-Jones  The University of Manchester
                      create a UDP socket and set the socket parameters

*/

#include <stdio.h>
#include <string.h>
#include <errno.h>              /* in case of error */
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>         /* for sockaddr ... */
#include <netinet/in.h>         /* for sockaddr_in & internet defines ... */
#include <netinet/ip.h>         /* for struct BSD ip ... iphdr is not in DUNIX */
#include <netinet/tcp.h>        /* for TCP_ */
#include <arpa/inet.h>

#include "soc_info.h"           /* for socket routines */

void create_udp_socket( int *soc, SOC_INFO *soc_info)
/* --------------------------------------------------------------------- */
{
  /* wrapper to enable backwards compatibility */
  sock_create_udp_socket( soc, soc_info, AF_INET);
}

void sock_create_udp_socket( int *soc, SOC_INFO *soc_info, int ipfamily)
/* --------------------------------------------------------------------- */
{
/* Open the UDP IP socket.  ( domain or family, socket type, protocol) */
  /* Local variables */
    int value;
    int def_value;
    int tos_value;
    int len;
    int error;

   *soc = socket( ipfamily, SOCK_DGRAM, 0 ); 
   if (*soc < 0) {
        perror("Error: UDP IP socket failed to open :" );
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
   if(!soc_info->quiet) printf("SNDBUF %d ", def_value );

/* get the default TOS settings */
   len = sizeof(def_value);
   def_value=0;
   error = getsockopt(*soc, IPPROTO_IP, IP_TOS, &def_value, (socklen_t *) &len);
   if(!soc_info->quiet){
       printf("IP_TOS 0x%x IP prec %d IP tos %d \n", def_value, ((def_value&IPTOS_PREC_MASK)>>5), ((def_value&IPTOS_TOS_MASK)>>1) );
   }

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

/* set the socket IP options tos and precidence_bits */
   if(soc_info->tos_set == 1){
       len = sizeof(tos_value);
       tos_value = ((soc_info->precidence_bits<<5)&IPTOS_PREC_MASK) | ((soc_info->tos_bits<<1)&IPTOS_TOS_MASK);
       error = setsockopt(*soc, IPPROTO_IP, IP_TOS, &tos_value, len);
       if (error) {
	   perror("Error: setsockopt( IP_TOS) on socket failed :" );
	   exit(-1);
       }
   }

/* set the socket IP options Difserv code point */
   if(soc_info->dscp_set == 1){
       len = sizeof(value);
       value = soc_info->dscp_bits;
       error = setsockopt(*soc, IPPROTO_IP, IP_TOS, &value, len);
       if (error) {
	   perror("Error: setsockopt( IP_TOS dscp ) on socket failed :" );
	   exit(-1);
       }
   }

/* and check TOS is set OK */
    len = sizeof(tos_value);
    tos_value=0;
    error = getsockopt(*soc, IPPROTO_IP, IP_TOS, &tos_value, (socklen_t *) &len);

/* print the values that were set */
	if(!soc_info->quiet){
		if(soc_info->soc_buf_size > 0) {
			printf("Settings: RCVBUF %d SNDBUF %d ", soc_info->soc_buf_size, soc_info->soc_buf_size );
		}
		else{
			printf("Settings: RCVBUF no change SNDBUF no change ");
		}
		printf("IP_TOS 0x%x IP prec %d IP tos %d ", tos_value, (((soc_info->precidence_bits<<5)&IPTOS_PREC_MASK)>>5), (((soc_info->tos_bits<<1)&IPTOS_TOS_MASK)>>1) );
		printf(" \n" );
	}

}
