/*
       sock_functions.c      R. Hughes-Jones  The University of Manchester
                      Functions to manipulate socket address structures for both IPv4 and IPv6
                      Happy to acknowledge borrowing form the work of Richard Stevens
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>              /* in case of error */
#include <stdlib.h>
#include  <netdb.h>             /* for getaddrinfo() etc. */

#include <sys/types.h>
#include <sys/socket.h>         /* for sockaddr ... */
#include <netinet/in.h>         /* for sockaddr_in & internet defines ... */
#include <netinet/ip.h>         /* for struct BSD ip ... iphdr is not in DUNIX */
#include <netinet/tcp.h>        /* for TCP_ */
#include <arpa/inet.h>
#include <netinet/tcp.h>        /* for tcp_info  struct and TCP_INFO ... */

#include "soc_info.h"           /* for socket routines */

char * sock_ntop(const struct sockaddr *sa)
/* --------------------------------------------------------------------- */
{
/*
 Convert the IP address contained in the sockaddr struct to a printable string
 use the address family to determine the correct conversion
 Return pointer to the string or NULL if error 
*/
    static char str[128];		/* Unix domain is largest */

	switch (sa->sa_family) {
	    case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return(NULL);
		return(str);
	    }

#ifdef	IPV6
	    case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;
		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
			return(NULL);
		return(str);
	    }
#endif
	    default:
		snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d",
				 sa->sa_family);
		return(str);
	} // end of switch (sa1->sa_family)
    return (NULL);
}

int sock_get_port(const struct sockaddr *sa)
/* --------------------------------------------------------------------- */
{
/*
 Get the IP port contained in the sockaddr struct 
 use the address family to determine the correct conversion
 Return port number in host order OR -1 if error 
*/

	switch (sa->sa_family) {
	    case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;
		return(ntohs(sin->sin_port));
	    }

#ifdef	IPV6
	    case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;
		return(ntohs(sin6->sin6_port));
	    }
#endif
	} // end of switch (sa1->sa_family)
	return (-1); /* unknown family ?? */
}

void sock_set_port(const struct sockaddr *sa, int port)
/* --------------------------------------------------------------------- */
{
/*
 Set the IP port in the sockaddr struct port number is in host order
 use the address family to determine the correct conversion
*/

	switch (sa->sa_family) {
	    case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;
		sin->sin_port = htons((uint16_t)port);
	    }

#ifdef	IPV6
	    case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;
		sin6->sin6_port = htons((uint16_t)port);
	    }
#endif
	} // end of switch (sa1->sa_family)
	return ; /* unknown family ?? */
}

int sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2)
/* --------------------------------------------------------------------- */
{
/*
 Compare the IP addresses contained in the as1 & sa2 sockaddr struct
 use the address family to determine the correct comparison
 Return result from memcmp() == 0 if complete match
*/
	if (sa1->sa_family != sa2->sa_family)
		return(-1);

	switch (sa1->sa_family) {
	    case AF_INET: {
		return(memcmp( &((struct sockaddr_in *) sa1)->sin_addr,
					   &((struct sockaddr_in *) sa2)->sin_addr,
					   sizeof(struct in_addr)));
	    }

#ifdef	IPV6
	    case AF_INET6: {
		return(memcmp( &((struct sockaddr_in6 *) sa1)->sin6_addr,
					   &((struct sockaddr_in6 *) sa2)->sin6_addr,
					   sizeof(struct in6_addr)));
	    }
#endif

	} // end of switch (sa1->sa_family)
    return (-1);
}

void sock_get_tcp_bufsize( int soc_fd, int *recv_bufSize, int *send_bufSize)
/* --------------------------------------------------------------------- */
{
	//get socket buffer size.
	/* Local variables */
	int len;
	int error;
	
	// check the socket is connected
	if( soc_fd < 3) return;
	
	/* get the default socket size options */
    len = sizeof(recv_bufSize);
    error = getsockopt(soc_fd, SOL_SOCKET, SO_RCVBUF, recv_bufSize, (socklen_t *) &len);
    if (error ) {
        perror("Error: getsockopt( SO_RCVBUF ) on socket failed :" );
		exit(-1);
    }
    len = sizeof(send_bufSize);
    error = getsockopt(soc_fd, SOL_SOCKET, SO_SNDBUF, send_bufSize, (socklen_t *) &len);
    if (error ) {
        perror("Error: getsockopt( SO_SNDBUF ) on socket failed :" );
		exit(-1);
    }
}

void sock_get_tcpinfo( int soc_fd, struct tcp_info *tcpInfo)
/* --------------------------------------------------------------------- */
{
	/* get the TCP_INFO data */
	/* Local variables */
	int len;
	int error;
	
	// check the socket is connected
	if( soc_fd < 3) return;
	
    len = sizeof(struct tcp_info);
    error = getsockopt(soc_fd, IPPROTO_TCP, TCP_INFO, (void *)tcpInfo, (socklen_t *) &len);
    if (error ) {
        perror("Error: getsockopt( TCP_INFO ) on socket failed :" );
		exit(-1);
    }
}
