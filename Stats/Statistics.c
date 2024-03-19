/*
       Statistics.c      R. Hughes-Jones  The University of Manchester
        Simple statistical functions and printout 
		The use of struct _STATSinfo and the Statistics_Info( Statistics *s, STATSinfo *stats_info) function 
		allows transmission of the information between local and remote processes.
		
		Some concepts are based on work by Marcel Boosten CERN 1998-1999 

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

#include <stdio.h>
#include <string.h>
#include <errno.h>               /* in case of error */
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include "Statistics.h"          /* for Statistics  */



void Statistics_Init( Statistics *s, char *name)
/* --------------------------------------------------------------------- */
{
/* mode =1 print int64
          =2 print double
*/
	/* clear the stats */
	memset(s, 0, sizeof(Statistics));
	s->min = 1e199;
	s->max = -1e199;
	/* LONG_MAX     9223372036854775807L */
	s->min_int = LONG_MAX;
	s->max_int = LONG_MIN;
	
	strncpy(s->name, name, sizeof(s->name)-1); 
}

void Statistics_Info( Statistics *s, STATSinfo *stats_info) 
/* --------------------------------------------------------------------- */
{
/* calc the mean */
	if(s->num > 0) {
		stats_info->mean= (double)s->sum/ (double)s->num;
		stats_info->sigma= sqrt( (double)s->sum_of_squares/(double)s->num -  stats_info->mean*stats_info->mean ) ;
	}
	else {
		stats_info->mean=0.0;
		stats_info->sigma=0.0;
	}
	stats_info->min = s->min;
	stats_info->max = s->max;
	stats_info->min_int = s->min_int;
	stats_info->max_int = s->max_int;
	strcpy(stats_info->name, s->name);
}

void Statistics_Snap( Statistics *s, STATSinfo *stats_info )
/* --------------------------------------------------------------------- */
{

/* take a snapshot of the statistics */
  
	Statistics_Info( s, stats_info); 

	/* clear the stats for the next snapshot*/
	s->num =0;
	s->sum =0.0;
	s->sum_of_squares =0.0;
	s->min = 1e199;
	s->max = -1e199;
	/* LONG_MAX     9223372036854775807L */
	s->min_int = LONG_MAX;
	s->max_int = LONG_MIN;

    return;
}


void Statistics_print_info( STATSinfo *stats_info, int option, char loc_rem)
/* --------------------------------------------------------------------- */
{
/*
Print RDMA Stats info

	loc_rem		= 'L' for local or 'R' for remote data
	option		=1 print titles
				=2 print int64
				=4 print double
				  
*/

  Statistics_print_info_file( stats_info, option, loc_rem, stdout);
}

void Statistics_print_info_file( STATSinfo *stats_info, int option, char loc_rem, FILE *output)
/* --------------------------------------------------------------------- */
{
/*
Print Stattistics info

	loc_rem		= 'L' for local or 'R' for remote data
	option		=1 print titles
				=2 print int64
				=4 print double
				  
*/
  
  switch(option){
    case 1:  // print titles
        fprintf(output, " %c STATS %s: ; min; max; mean; sigma;", loc_rem, stats_info->name);
        break;

    case 2:  // print int64
        fprintf(output, " %c STATS %s: ;", loc_rem, stats_info->name);
		fprintf(output, " %"LONG_FORMAT"d; %"LONG_FORMAT"d; %.4g; %.4g;", stats_info->min_int, stats_info->max_int, stats_info->mean, stats_info->sigma );
        break;

    case 4:  // print double
        fprintf(output, " %c STATS %s: ;", loc_rem, stats_info->name);
		fprintf(output, " %.4g; %.4g; %.4g; %.4g;", stats_info->min, stats_info->max, stats_info->mean, stats_info->sigma );
        break;

    }
    return;
}



