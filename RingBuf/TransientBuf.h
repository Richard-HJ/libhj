/*
       TransientBuf.h      R. Hughes-Jones  The University of Manchester
                      include file for reading the network interface counters 

*/
/*
   Copyright (c) 2019,2020 Richard Hughes-Jones, University of Manchester
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

#ifndef TRANSIENTBUF_LOADED 
#define TRANSIENTBUF_LOADED true

#include "arch.h"                      /* define the architecture */


struct _TBuf {
	int index;
    int valid;	                       /* =1 if data is valid - i.e. Transient_Add() has been called */
	struct _TBuf *ptr_nextbuf;
	char data[];
};

typedef struct _TBuf TBuf;

/* TransientBuf structure */
struct _TransientBuf {
    int data_len; 
    int ring_len;
    int num_after;
	int stop_after;
	int transient_index;               
	int history_start_index;
	int history_end_index;
	struct _TBuf *ptr_transient;       /* pointer to TBuf that has data when transient occured */
	struct _TBuf *ptr_history_start;   /* pointer to TBuf that is the start of the history for this transient */
	struct _TBuf *ptr_history_end;
	struct _TBuf *ptr_tbuf_ring;
	char TBuf_ring[];                  /* storage for the TransientBuf including the TBuf ring */
};

typedef struct _TransientBuf TransientBuf;


// functions

TransientBuf* Transient_Init( int data_len, int ring_len, int num_after);
char* Transient_Add( TransientBuf *tbuf );
char* Transient_Occured( TransientBuf *tbuf );
char* Transient_GetFirst( TransientBuf *tbuf, int *index, int *valid );
char* Transient_GetNext( TransientBuf *tbuf, int *index, int *valid );
int Transient_GetStartIndex( TransientBuf *tbuf );
int Transient_GetTransientIndex( TransientBuf *tbuf );

#endif /* TRANSIENTBUF_LOADED */
