/*
	RingBuf.h	R.E. Hughes-Jones  	University of Manchester, ATLAS
	
*/

struct RingBuf {
    char *buf_start;
    char *buf_end;
    int producer_len;
    int consumer_len;
    char *producer_ptr;
    char *producer_ptr_end;
    char *producer_valid_ptr;
    char *consumer_ptr;
    char *consumer_ptr_end;
};


/* Function prototypes */

/* create and initialise the ring buffer */
int RingInit(struct RingBuf *ringbuf, int ring_len, int producer_len, int consumer_len);

/* initialise the ring buffer */
int RingReset(struct RingBuf *ringbuf);

/* request a pointer to place len bytes in the ring buffer */
char *RingProducer(struct RingBuf *ringbuf);

/* request a pointer to retrieve len bytes from the ring buffer */
char *RingConsumer(struct RingBuf *ringbuf, int *len_available);

/* move len bytes from current producer location to that specified by new_location */
char *RingMove(struct RingBuf *ringbuf, int new_location_offset);

/* set marker to indicate that len bytes after the last RingProducer() call are valid */
char *RingProducerValid(struct RingBuf *ringbuf, int len);

