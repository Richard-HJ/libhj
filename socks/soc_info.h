/*
       soc_info.h     R. Hughes-Jones  The University of Manchester
                      include file for creating UDP and TCP sockets and setting the socket parameters

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

typedef struct _SOC_INFO SOC_INFO;

    struct _SOC_INFO {
    int soc_buf_size;                           /* send & recv buffer size bytes */

    int precidence_bits;                        /* precidence bits for the TOS field of the IP header IPTOS_TOS_MASK = 0x1E */
    int tos_bits;                               /* tos bits for the TOS field of the IP header IPTOS_PREC_MASK = 0xE0 */
    int tos_set;                                /* flag =1 if precidence or tos bits set */
    int dscp_bits;                              /* difserv code point bits for the TOS field of the IP header */
    int dscp_set;                               /* flag =1 if dscp bits set - alternative to precidence or tos bits set */

    int enable_nodelay;                         /* =1 to enable Nagle and delay-ack */
    int quiet;                                  /* set =1 for just printout of results - monitor mode */
    };

/* create a raw (nonIP) socket and set parameters */
void create_raw_socket( int *soc, SOC_INFO *soc_info, int ethpkt_type);

/* create a UDP socket and set parameters */
void create_udp_socket( int *soc, SOC_INFO *soc_info);
void sock_create_udp_socket( int *soc, SOC_INFO *soc_info, int ipfamily);

/* create a TCP socket and set parameters */
void create_tcp_socket( int *soc, SOC_INFO *soc_info);
void sock_create_tcp_socket( int *soc, SOC_INFO *soc_info, int ipfamily);

/* Convert the IP address in sockadd to a printable string */
char * sock_ntop(const struct sockaddr *sa);

/* Get the IP port contained in the sockaddr struct */
int sock_get_port(const struct sockaddr *sa);

/* Set the IP port contained in the sockaddr struct */
void sock_set_port(const struct sockaddr *sa, int port);

/* Compare the IP addresses */
int sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2);

//get socket buffer size.
void sock_get_tcp_bufsize( int soc_fd, int *recv_bufSize, int *send_bufSize);

/* get the TCP_INFO data */
void sock_get_tcpinfo( int soc_fd, struct tcp_info *tcpInfo);
