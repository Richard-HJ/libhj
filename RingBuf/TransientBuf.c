/*
	TransientBuf.c	R.E. Hughes-Jones  	University of Manchester

Implement a ring buffer (as a linked list) as a history store of information from the user.
The user data is transperent to the ring buffer and stored as char data[] in the entries of the ring.
The size of the ring storages is determined by data_len and ring_len parameters to Transient_Init( ).

The user requests a pointer to the data area in the next entry with Transient_Add() or Transient_Occured().
Transient_Occured() makes a note of the particular entry and allows the ring to run for int num_after entries.
After receiving the pointer from Transient_Add() or Transient_Occured() the user fills in the data to be stored.

The contents of the ring buffer are accessed using Transient_GetFirst() folloed by calls to Transient_GetNext().

*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "TransientBuf.h"

TransientBuf* Transient_Init( int data_len, int ring_len, int num_after)
/* --------------------------------------------------------------------- */
{
/*
	data_size	size in bytes of user data for each entry
	ring_len	number of entries in the ring buffer 
	num_after	number of entries to store AFTER the transient occured << ring_len
*/
TransientBuf *tbuf;  
TBuf *tbuf_ring;                       /* pointer for the element ring buffer in the TransientBuf struct */
int ring_data_size;
//int TransientBuf_size;
int i;
char *ring_ptr_bytes;

	ring_data_size = (sizeof(TBuf)+data_len);
	tbuf = (TransientBuf*) malloc( sizeof(TransientBuf)+ring_len*ring_data_size);
	/* clear the info block */
	memset(tbuf, 0, sizeof(TransientBuf)+ring_len*ring_data_size);
	
	/* initialise */
	tbuf->data_len = data_len;
	tbuf->ring_len = ring_len;
	tbuf->num_after = num_after;
	tbuf->stop_after = num_after;
	tbuf->transient_index = -1;        /* there is no transient yet */ 
	tbuf->history_start_index =0;

	ring_ptr_bytes = &tbuf->TBuf_ring[0]  ;
	tbuf->ptr_history_start = ( TBuf*) ring_ptr_bytes;
	
	tbuf_ring = tbuf->ptr_history_start;
	for (i=0; i<ring_len; i++){
		tbuf_ring->index = i;
		tbuf_ring->valid = 0;
		ring_ptr_bytes = ring_ptr_bytes + ring_data_size;
		tbuf_ring->ptr_nextbuf = ( TBuf*) ring_ptr_bytes;
		if(i== ring_len-1) break;           // dont change pointer for last entry
		tbuf_ring = tbuf_ring->ptr_nextbuf;
	}
	/* close the ring */
	tbuf_ring->ptr_nextbuf = tbuf->ptr_history_start;
	/* set the transient poiner to the last in the ring - ready for Adding first element */
	tbuf->ptr_transient = tbuf_ring;
	tbuf->ptr_history_end = tbuf_ring;
	
	return (tbuf);
}

char* Transient_Add( TransientBuf *tbuf )
/* --------------------------------------------------------------------- */
{
/* add the data for this entry to the ring buffer 
  
  mark entry as valid
  check if next pointer == start pointer -> if so move start pointer to next entry 
  return pointer to the data area or NULL if transient is complete */
  
  /* check if transient has occured */
	if(tbuf->transient_index != -1){
		if (tbuf->stop_after < 0) return NULL;
		tbuf->stop_after--;
		if (tbuf->stop_after == 0){
			tbuf->ptr_history_end = tbuf->ptr_transient;
			tbuf->history_end_index = tbuf->ptr_transient->index;
		}
	}
	tbuf->ptr_transient = tbuf->ptr_transient->ptr_nextbuf;
	if ((tbuf->ptr_transient == tbuf->ptr_history_start) && (tbuf->ptr_transient->valid == 1)) {
		tbuf->ptr_history_start = tbuf->ptr_transient->ptr_nextbuf;
		tbuf->history_start_index = tbuf->ptr_history_start->index;
	}
	tbuf->ptr_transient->valid =1;
	return ((char*)&tbuf->ptr_transient->data);
}

char* Transient_Occured( TransientBuf *tbuf )
/* --------------------------------------------------------------------- */
{
/* Flag a transient occured
   Then add the data for this entry to the ring buffer 
   return pointer to the data area */
   
	/* check if transient has occured */
	if(tbuf->transient_index != -1) return(NULL);
 
	/* note the data for the transient will be in the next ringbuf entry */
	tbuf->transient_index = tbuf->ptr_transient->ptr_nextbuf->index;
	return (Transient_Add( tbuf ));
}

char* Transient_GetFirst( TransientBuf *tbuf, int *index, int *valid )
/* --------------------------------------------------------------------- */
{
/* return the pointer to the first entry in the ring buffer 
*/
	tbuf->ptr_tbuf_ring = tbuf->ptr_history_start;
	*index = tbuf->ptr_tbuf_ring->index;
	*valid = tbuf->ptr_tbuf_ring->valid;
	return ((char*)&tbuf->ptr_tbuf_ring->data); 
}

char* Transient_GetNext( TransientBuf *tbuf, int *index, int *valid )
/* --------------------------------------------------------------------- */
{
/* return the pointer to the next entry in the ring buffer
return NULL if reached the end of the ring buffer 
*/
	/* check if alread at the end of the ring buffer */
	if(tbuf->ptr_tbuf_ring == NULL) return(NULL);
	/* increment to next entry */
	tbuf->ptr_tbuf_ring = tbuf->ptr_tbuf_ring->ptr_nextbuf;
	/* check not at end of the ring buffer */
	if(tbuf->ptr_tbuf_ring == tbuf->ptr_history_start) return(NULL);
	*index = tbuf->ptr_tbuf_ring->index;
	*valid = tbuf->ptr_tbuf_ring->valid;
	return ((char*)&tbuf->ptr_tbuf_ring->data); 
}

int Transient_GetStartIndex( TransientBuf *tbuf )
/* --------------------------------------------------------------------- */
{
/* return the value of the index to the first entry in the ring buffer
*/
	return (tbuf->history_start_index);
}

int Transient_GetTransientIndex( TransientBuf *tbuf )
/* --------------------------------------------------------------------- */
{
/* return the value of the index to the transient ring buffer when the transient occured
   value = -1 if there was no transient
*/
	return (tbuf->transient_index);
}



