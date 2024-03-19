/*
	hist.h	R.E. Hughes-Jones  	University of Manchester, ATLAS
*/
/*
   Copyright (c) 2002,2003,2004,2005,2006,2009 Richard Hughes-Jones, University of Manchester
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

#ifndef HIST_H_LOADED
#define HIST_H_LOADED 

#include "arch.h"               /* define the architecture */

#define HIST_ALL_BINS	1000
#define HIST_OFLOW HIST_ALL_BINS-1
#define HIST_WIDTH 30

struct HIST {
	int32 hist_id;
	int32 low_lim;
	int32 bin_width;
	int32 n_bins;
	int64 mean;
	int64 sd;
	int64 bins[HIST_ALL_BINS];
	char title[32];
	     };



/* Function prototypes */

void h_book(struct HIST *hist_ptr, int32 id, int32 low_lim, int32 bin_width, 
		int32 n_bins, char *char_ptr);
void h_clear(struct HIST *hist_ptr);
void h_fill1(struct HIST *hist_ptr, int64 value);
void h_print(struct HIST *hist_ptr);
void h_output(struct HIST *hist_ptr);
void h_print_file(struct HIST *hist_ptr, FILE *output);
void h_output_file(struct HIST *hist_ptr, FILE *output);
void h_stats (struct HIST *hist_ptr, 
	      int64 *num_in_hist, double *mean, double *sigma, double *coeff_skew, double *coeff_kurtosis,
	      int32 *median, int32 *iq25, int32 *iq75);
void h_prt_stats(struct HIST *hist_ptr);
void h_prt_stats_file(struct HIST *hist_ptr, FILE *output);
void h_copy(struct HIST *in_ptr, struct HIST *out_ptr );

#endif	/*  #ifndef HIST_H_LOADED  */
