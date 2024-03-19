/*
	hist.c	R.E. Hughes-Jones  	University of Manchester, ATLAS
	fast histogram package

          31 Jul 04 Rich extend precision in printf for  h_prt_stats ()
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

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>


#include "hist.h"


void h_book(struct HIST *hist_ptr, int32 id, int32 low_lim, int32 bin_width, 
		int32 n_bins, char *char_ptr)
/* ------------------------------------------------------- */
{
/* setup the histogram */
char  *char_dest;
int i;

	if(hist_ptr == NULL) return;
	hist_ptr->hist_id= id;
	hist_ptr->low_lim= low_lim;
	hist_ptr->bin_width= bin_width;
	hist_ptr->n_bins= n_bins;
	char_dest= hist_ptr->title;
	for(i=0; i<31 && (*char_ptr !=0); i++)
	{
	   *char_dest++= *char_ptr++;
	}
	*char_dest= 0;
/* zero the bins */
        h_clear(hist_ptr );
}

void h_clear(struct HIST *hist_ptr)
/* ------------------------------------------------------- */
{
int i;
/* clear the histogram contents */
	if(hist_ptr == NULL) return;
	hist_ptr->mean= 0;
	hist_ptr->sd= 0;
	for(i=0; i<HIST_ALL_BINS; i++)
	{
            hist_ptr->bins[i]= 0;
        }
}

void h_fill1(struct HIST *hist_ptr, int64 value)
/* ------------------------------------------------------- */
{
register int bin;

/* For speed assume user knows what she is doing
	if(hist_ptr == NULL) return;
*/
/* calc the bin no -- +1 as bins[0] == underflow */
	bin= ((value - hist_ptr->low_lim)/hist_ptr->bin_width) +1;
	if(bin < 0) bin=0;
	if(bin > hist_ptr->n_bins) bin= HIST_OFLOW;
        hist_ptr->bins[bin] = hist_ptr->bins[bin] + 1;
/* sum for mean */
	if((bin > 0)  && (bin <HIST_OFLOW)) hist_ptr->mean = hist_ptr->mean + value;
}


void h_print (struct HIST *hist_ptr)
/* ------------------------------------------------------- */
{
  /* print histogram bins as scaled character plot */

  h_print_file (hist_ptr, stdout);
}

void h_print_file(struct HIST *hist_ptr, FILE *output) 
/* ------------------------------------------------------- */
{
int64 total, max;
int32  bin_edge;
int i, j, n;
double mean_float;

	if(hist_ptr == NULL) return;
/*  find the max count in a bin and the total no. counts in histo */
	total= 0;
        max= 0;

	for(i=1; i<= hist_ptr->n_bins; i++)
	{
	   total = total + hist_ptr->bins[i];
	   if(hist_ptr->bins[i] > max ) max = hist_ptr->bins[i];
	}

/* calc the mean */
	if(total > 0) 
	{
		mean_float= (double) hist_ptr->mean/ (double)total;
	}
	else mean_float=0;
	fprintf(output, "Hist %8d %s\n counts %" LONG_FORMAT "d mean %.2g underflows %" LONG_FORMAT "d overflows %" LONG_FORMAT "d\n",
			hist_ptr->hist_id, hist_ptr->title,
			total, mean_float,
			hist_ptr->bins[0], hist_ptr->bins[HIST_OFLOW]);
/* calc the scale factor -- allow for 0 */
	if(max == 0) max= 1;
/* print the lines */
	bin_edge= hist_ptr->low_lim;
	for(i=1; i<= hist_ptr->n_bins; i++)
	{
	   fprintf(output, "%8d %8" LONG_FORMAT "d", bin_edge, hist_ptr->bins[i]);
           n = (int)( (hist_ptr->bins[i] * (int64)HIST_WIDTH) /max );
	   for(j=0; j< n; j++) printf("*");
	   fprintf(output, "\n");
           bin_edge = bin_edge + hist_ptr->bin_width;
        }
	return;
}

void h_output(struct HIST *hist_ptr)
/* ------------------------------------------------------- */
{
  /* print histogram bins as bin ; value */
  h_output_file(hist_ptr, stdout);

  return;
}

void h_output_file(struct HIST *hist_ptr, FILE *output)
/* ------------------------------------------------------- */
{
  /* print bins as bin ; value */

int64 total;
int bin_edge;
int i;
double mean_float;

	if(hist_ptr == NULL) return;
/*  find the max count in a bin and the total no. counts in histo */
	total= 0;

	for(i=1; i<= hist_ptr->n_bins; i++)
	{
	   total = total + hist_ptr->bins[i];
	}

/* calc the mean */
	if(total > 0) 
	{
		mean_float=  (double)hist_ptr->mean/ (double)total;
	}
	else mean_float=0;
	fprintf(output, "Hist %8d %s\n counts %" LONG_FORMAT "d mean %.2f underflows %" LONG_FORMAT "d overflows %" LONG_FORMAT "d\n",
			hist_ptr->hist_id, hist_ptr->title,
			total, mean_float,
			hist_ptr->bins[0], hist_ptr->bins[HIST_OFLOW]);
/* print the bins and contents */
	bin_edge = hist_ptr->low_lim;
	for(i=1; i<= hist_ptr->n_bins; i++)
	{
	   fprintf(output, "%d ; %" LONG_FORMAT "d\n", bin_edge, hist_ptr->bins[i]);
           bin_edge = bin_edge + hist_ptr->bin_width;
        }
	return;
}

void h_stats (struct HIST *hist_ptr, 
	      int64 *num_in_hist, double *mean, double *sigma, 
	      double *coeff_skew, double *coeff_kurtosis, 
	      int32 *median, int32 *iq25, int32 *iq75)
/* ------------------------------------------------------- */
{
  int i;
  int32 bin_edge;
  int64 bin_sum;
  double delta;
  double mu2_sum =0;
  double mu3_sum =0;
  double mu4_sum =0;
  int64 cumulative25;
  int64 cumulative50;
  int64 cumulative75;

  /* zero output */
	*num_in_hist =0;
        *mean = 0.0;
        *sigma = 0.0;
        *coeff_skew = 0.0;
        *coeff_kurtosis = 0.0; 
	*median = 0;
        *iq25 = 0;
        *iq75 = 0;
	if(hist_ptr == NULL) return;
/*  find the total no. counts in histo */
	for(i=1; i<= hist_ptr->n_bins; i++)
	{
	  *num_in_hist = *num_in_hist + hist_ptr->bins[i];
	}
	if( *num_in_hist <= 0) return; 

/* calc the mean */
	*mean=  (double)hist_ptr->mean/ (double)*num_in_hist;
/* calc the higher moments mu2_ ... */
	bin_edge= hist_ptr->low_lim;
	for(i=1; i<= hist_ptr->n_bins; i++)
	{
	   delta = (double)bin_edge - *mean;
           mu2_sum += (double)hist_ptr->bins[i] * delta*delta;
           mu3_sum += (double)hist_ptr->bins[i] * delta*delta*delta;
           mu4_sum += (double)hist_ptr->bins[i] * delta*delta*delta*delta;
           bin_edge += hist_ptr->bin_width;
        }
	*sigma = sqrt(mu2_sum/(double)(*num_in_hist) );
	*coeff_skew = mu3_sum /((double)(*num_in_hist) * *sigma * *sigma * *sigma); 
	*coeff_kurtosis = (mu4_sum /((double)(*num_in_hist) * *sigma * *sigma ) ) -3.0; 

	/* calc quartiles and median */
	bin_sum=0;
	bin_edge= hist_ptr->low_lim;
	cumulative25 = 0.25* (*num_in_hist);
	cumulative50 = 0.50* (*num_in_hist);
	cumulative75 = 0.75* (*num_in_hist);

	for(i=1; i<= hist_ptr->n_bins; i++)
	{
	   bin_sum += hist_ptr->bins[i];
	   if( (*iq25 == 0) && (bin_sum >= cumulative25) ) *iq25 = bin_edge;
	   if( (*median == 0) && (bin_sum >= cumulative50) ) *median = bin_edge;
	   if( (*iq75 == 0) && (bin_sum >= cumulative75) ) *iq75 = bin_edge;
           bin_edge= bin_edge+ hist_ptr->bin_width;
	}
}

void h_prt_stats (struct HIST *hist_ptr)
/* ------------------------------------------------------- */
{
  /* calculate the stats for this histogram 
     and print them 
  */
  h_prt_stats_file (hist_ptr, stdout);
}

void h_prt_stats_file (struct HIST *hist_ptr, FILE *output)
/* ------------------------------------------------------- */
{
  /* calculate the stats for this histogram 
     and print them 
  */
int64   num_in_hist;
double  mean;
double  sigma;
double  coeff_skew;
double  coeff_kurtosis; 
int32  median;
int32  iq25;
int32  iq75;

  /* calculate the stats for this histogram */
      h_stats( hist_ptr, &num_in_hist, &mean, &sigma, &coeff_skew, &coeff_kurtosis, 
                     &median, &iq25, &iq75);
      fprintf(output, " counts %" LONG_FORMAT "d; underflows %" LONG_FORMAT "d; overflows %" LONG_FORMAT "d; \n",
	     num_in_hist, hist_ptr->bins[0], hist_ptr->bins[HIST_OFLOW]);
      fprintf(output, " mean %.4g sigma %.4g; coeff_skew %.4g; coeff_kurtosis %.4g; \n",
	     mean, sigma, coeff_skew, coeff_kurtosis);
      fprintf(output, " median %d; 25-quartile %d; 75-quartile %d; \n", 
	     median, iq25, iq75);
}

void h_copy(struct HIST *in_ptr, struct HIST *out_ptr )
/* ------------------------------------------------------- */
{
/* copy contents of histogram *in_ptr to *out_ptr 
with byte swapping if needed 
*/

#ifdef BYTE_SWAP_ON
/* for non Intel architecture Apple Sun */
    out_prt->hist_id = i4swap(in_ptr->hist_id );
    out_prt->low_lim = i4swap(in_ptr->low_lim );
    out_prt->bin_width = i4swap(it_ptr->bin_width );
    out_prt->n_bins = i4swap(in_ptr->n_bins );
    out_prt->mean = i8swap(in_ptr->mean );
    out_prt->sd = i8swap(in_ptr->sd );
    for(i=0; i<HIST_ALL_BINS; i++)
    {
        out_ptr->bins[i]= i8swap(in_ptr->bins[i]) ;
    }
    for(i=0; i<32; i++){
        out_ptr->title[i]= i8swap(in_ptr->title[i]) ;
    }
    
#else
/* for Intel HW */
     memcpy(out_ptr, in_ptr, sizeof( struct HIST) );

#endif    /* ifdef BYTE_SWAP_ON */

}
