/*
	RingBuf.c	R.E. Hughes-Jones  	University of Manchester, ATLAS

	    NOTEs: To allow for hardware transfers the ring buffer works with integer sections of data as:
		   Let size of producer requests be p  then Length of ring buffer = m * p
		   AND
		   Let size of consumer requests be c  then Length of ring buffer = n * c

		   i,j,l,m,n are integers
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "RingBuf.h"

int RingInit(struct RingBuf *ringbuf, int ring_len, int producer_len, int consumer_len)
/* ------------------------------------------------------- */
{
/* create and initialise the ring buffer */

  int i;

/* check the lengths are correct */
    i = ring_len / producer_len;
    if(i * producer_len != ring_len) return(-1);
    i = ring_len / consumer_len;
    if(i * consumer_len != ring_len) return(-2);

/* create the ring buffer */
    ringbuf->buf_start = malloc(ring_len);
    if(ringbuf->buf_start == NULL) return(-3);

    ringbuf->buf_end = ringbuf->buf_start + ring_len;
    ringbuf->producer_ptr = ringbuf->buf_start;
    ringbuf->producer_ptr_end = ringbuf->buf_start;
    ringbuf->producer_valid_ptr = NULL;
    ringbuf->consumer_ptr = NULL;
    ringbuf->consumer_ptr_end = ringbuf->buf_start;
    ringbuf->producer_len = producer_len;
    ringbuf->consumer_len = consumer_len;

    return(0);
}

int RingReset(struct RingBuf *ringbuf)
/* ------------------------------------------------------- */
{
/* initialise the ring buffer */

    ringbuf->producer_ptr = ringbuf->buf_start;
    ringbuf->producer_ptr_end = ringbuf->buf_start;
    ringbuf->producer_valid_ptr = NULL;
    ringbuf->consumer_ptr = NULL;
    ringbuf->consumer_ptr_end = ringbuf->buf_start;

    return(0);
}

char* RingProducer(struct RingBuf *ringbuf)
/* ------------------------------------------------------- */
{
/* request a pointer to place len bytes in the ring buffer */
  char *new_start;
  char *new_end;

/* check ringbuffer exists */
    if(ringbuf == NULL) return(NULL);
    if(ringbuf->buf_start == NULL) return(NULL);

/* claculate the end of the data the producer will put into the ring buffer */
    if(ringbuf->producer_ptr_end == ringbuf->buf_end) {
        new_start = ringbuf->buf_start;
        new_end = ringbuf->buf_start + ringbuf->producer_len;
    }
    else {
        new_start = ringbuf->producer_ptr_end;
	new_end = ringbuf->producer_ptr_end + ringbuf->producer_len;
    }

/* check the consumer is not still using this area */
    if(new_start <= ringbuf->consumer_ptr && ringbuf->consumer_ptr < new_end) return(NULL);

    /* if there is data in the ring put the consumer to the start */
    if(ringbuf->consumer_ptr == NULL) ringbuf->consumer_ptr = ringbuf->buf_start;

    ringbuf->producer_ptr = new_start;
    ringbuf->producer_ptr_end = new_end;
    return(new_start);
}

char *RingConsumer(struct RingBuf *ringbuf, int *len_available)
/* ------------------------------------------------------- */
{
/* request a pointer to retrieve len bytes from the ring buffer */
  char *new_start;
  char *new_end;
  *len_available = 0;

/* check ringbuffer exists */
    if(ringbuf == NULL) return(NULL);
    if(ringbuf->buf_start == NULL) return(NULL);
    if(ringbuf->consumer_ptr_end == ringbuf->producer_valid_ptr) return(NULL);

/* calculate the end of the data the consumer will take from the ring buffer */
    if(ringbuf->consumer_ptr_end == ringbuf->buf_end) {
        new_start = ringbuf->buf_start;
        new_end = ringbuf->buf_start + ringbuf->consumer_len;
        /* check if producer has started to use this area - ie producer only filled last of ring buffer */
	/*        if(ringbuf->producer_ptr == ringbuf->buf_end) return(NULL);*/
    }
    else {
        new_start = ringbuf->consumer_ptr_end;
	new_end = ringbuf->consumer_ptr_end + ringbuf->consumer_len;
    }

/* check if the producer is still using this area 
   - note producer_ptr can be in the consumer area if RingProducerValid() has been called */
    if(new_start <= ringbuf->producer_ptr && ringbuf->producer_ptr <= new_end) {
        /* producer_ptr in the consumer area - check that all the block the consumer requested has valid data */        
        if(new_start < ringbuf->producer_valid_ptr && ringbuf->producer_valid_ptr <= new_end) {
	    *len_available = ringbuf->producer_valid_ptr - new_start ;
	}
	else {
	    *len_available = 0;
	    return(NULL);
	}
    }
    else {
        /* producer_ptr NOT in the consumer area - check that all the block the consumer requested has valid data */        
        if(new_start < ringbuf->producer_valid_ptr && ringbuf->producer_valid_ptr <= new_end) {
	    *len_available = ringbuf->producer_valid_ptr - new_start ;
	}
	else *len_available = ringbuf->consumer_len; 
    }
 
    ringbuf->consumer_ptr = new_start;
    ringbuf->consumer_ptr_end = new_start + *len_available;

    return(new_start);
}

char *RingMove(struct RingBuf *ringbuf, int new_location_offset)
     /* ------------------------------------------------------- */
{
/* move len bytes from current producer location to that specified by 
   ringbuf->producer_ptr + new_location_offset * ringbuf->producer_len
*/

  char *new_location;
  char *new_start;
  char *new_end;

/* check ringbuffer exists */
    if(ringbuf == NULL) return(NULL);
    if(ringbuf->buf_start == NULL) return(NULL);

/* claculate the end of the data that the producer will put into the ring buffer */
    new_location = ringbuf->producer_ptr + new_location_offset * ringbuf->producer_len;
    if(new_location >= ringbuf->buf_end) {
        new_start = ringbuf->buf_start + (new_location - ringbuf->buf_end) ;
        new_end = new_start + ringbuf->producer_len;
    }
    else {
        new_start = new_location;
	new_end = new_location + ringbuf->producer_len;
    }
    //    printf(" new_location %d ringbuf->producer_ptr %d new_start %d\n", (int)new_location ,  (int)ringbuf->producer_ptr, (int) new_start);
/* check the consumer is not still using this area */
    if(ringbuf->producer_ptr < ringbuf->consumer_ptr){
        if(new_start < ringbuf->producer_ptr || new_start >= ringbuf->consumer_ptr) return(NULL);
    }
    else {
        if(ringbuf->consumer_ptr <= new_start && new_start < ringbuf->producer_ptr) return(NULL);
    }

/* do the copy */
    memcpy(new_start, ringbuf->producer_ptr, ringbuf->producer_len);

    ringbuf->producer_ptr = new_start;
    ringbuf->producer_ptr_end = new_end;

    return(new_start);
}

char *RingProducerValid(struct RingBuf *ringbuf, int len)
/* ------------------------------------------------------- */
{
/* set ringbuf->producer_valid_len to indicate that len bytes after the last RingProducer() call are valid 
   len = prod_len == all prod_len data valid 
   len = 0        == all data in previous producer block is valid */

/* check ringbuffer exists */
    if(ringbuf == NULL) return(NULL);
    if(ringbuf->buf_start == NULL) return(NULL);

    if(len == 0){
	ringbuf->producer_valid_ptr = ringbuf->producer_ptr;
	/* move the  ringbuf->producer_ptr to ringbuf->producer_ptr_end */
	ringbuf->producer_ptr_end = ringbuf->producer_ptr;
    }
    else{
        ringbuf->producer_valid_ptr = ringbuf->producer_ptr + len;
	/* move the  ringbuf->producer_ptr to ringbuf->producer_ptr_end */
	ringbuf->producer_ptr = ringbuf->producer_ptr_end;
    }


    return (ringbuf->producer_ptr);

}

