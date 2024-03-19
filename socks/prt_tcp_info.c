/*
     prt_tcp_info.c     R. Hughes-Jones  The University of Manchester
     Print information from TCP socket MIB - RedHat 7.1 Kernel 2.4.2
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>              /* in case of error */
#include <signal.h>
#include <unistd.h>             /* sleep() alarm() close() etc... */
#include <stdlib.h>
 #include <unistd.h>            /* sysconf()  _SC_CLK_TCK */


#include <sys/types.h>
#include <sys/socket.h>         /* for sockaddr ... */
#include <netinet/in.h>         /* for sockaddr_in & internet defines ... */
#include <arpa/inet.h>
#include <linux/socket.h>       /* for SOL_TCP ... */
/* #include <linux/tcp.h> */          /* for tcp_info struct and TCP_INFO ... */
#include <netinet/tcp.h>        /* for tcp_info  struct and TCP_INFO ... */

void prt_tcp_info (int soc)
/* --------------------------------------------------------------------- */
{
   /*
    * Only non-empty body when "tcp_info" struct exists. That is the case when
    * the constant "TCP_INFO" is defined.
    */

#ifdef TCP_INFO

   int len, error, tmp;
   struct tcp_info soc_tcp_info;

   len = sizeof(soc_tcp_info);
   error = getsockopt(soc, IPPROTO_TCP, TCP_INFO, &soc_tcp_info, (socklen_t *) &len);
    if (error ) {
        perror("Error: getsockopt( TCP_INFO ) on socket failed :" );
	exit(-1);
    }
	 printf("\n");
	 tmp = soc_tcp_info.tcpi_state; 
	 printf( "tcpi_state            %d \n", tmp);
	 tmp = soc_tcp_info.tcpi_ca_state; 
	 printf( "tcpi_ca_state         %d \n", tmp);
	 tmp = soc_tcp_info.tcpi_retransmits; 
	 printf( "tcpi_retransmits      %d \n", tmp);
	 tmp = soc_tcp_info.tcpi_probes; 
	 printf( "tcpi_probes           %d \n", tmp);
	 tmp = soc_tcp_info.tcpi_backoff; 
	 printf( "tcpi_backoff          %d \n", tmp);
	 tmp = soc_tcp_info.tcpi_options; 
	 printf( "tcpi_options          %d \n", tmp);
	 tmp = soc_tcp_info.tcpi_snd_wscale; 
	 printf( "tcpi_snd_wscale       %d \n", tmp);
	 tmp = soc_tcp_info.tcpi_rcv_wscale; 
	 printf( "tcpi_rcv_wscale       %d \n", tmp);

	 printf("\n");
	 printf( "tcpi_rto              %d \n", soc_tcp_info.tcpi_rto);
 	 printf( "tcpi_ato              %d \n", soc_tcp_info.tcpi_ato);
 	 printf( "tcpi_snd_mss          %d \n", soc_tcp_info.tcpi_snd_mss);
 	 printf( "tcpi_rcv_mss          %d \n", soc_tcp_info.tcpi_rcv_mss);
	 printf("\n");

  	 printf( "tcpi_unacked          %d \n", soc_tcp_info.tcpi_unacked);
  	 printf( "tcpi_sacked           %d \n", soc_tcp_info.tcpi_sacked);
   	 printf( "tcpi_lost             %d \n", soc_tcp_info.tcpi_lost);
  	 printf( "tcpi_retrans          %d \n", soc_tcp_info.tcpi_retrans);
  	 printf( "tcpi_fackets          %d \n", soc_tcp_info.tcpi_fackets);
	 printf("\n");

        /* Metrics. */
  	 printf( "tcpi_pmtu             %d \n", soc_tcp_info.tcpi_pmtu);
  	 printf( "tcpi_rcv_ssthresh     %d \n", soc_tcp_info.tcpi_rcv_ssthresh); 
   	 printf( "tcpi_rtt              %d \n", soc_tcp_info.tcpi_rtt);
   	 printf( "tcpi_rttvar           %d \n", soc_tcp_info.tcpi_rttvar);
  	 printf( "tcpi_snd_ssthresh     %d \n", soc_tcp_info.tcpi_snd_ssthresh);
  	 printf( "tcpi_snd_cwnd         %d \n", soc_tcp_info.tcpi_snd_cwnd);
  	 printf( "tcpi_advmss           %d \n", soc_tcp_info.tcpi_advmss);
  	 printf( "tcpi_reordering         %d \n", soc_tcp_info.tcpi_reordering);
	 printf("\n");

#endif /* #ifdef TCP_INFO */

}
