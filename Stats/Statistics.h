/*
       Statistics.h      R. Hughes-Jones  The University of Manchester
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

#ifndef STATISTICS_LOADED
#define STATISTICS_LOADED true

/**@name Statistics
   
   Functions that allow the calculation of the average and standard
   deviation of a large set of numbers.  */



#include <math.h>

#ifndef INLINE
#define INLINE static inline
//#define INLINE extern inline
#endif

#include "arch.h"               /* define the architecture */

/* for access by users */
struct _STATSinfo {
	double mean;
	double sigma;	
	int64 num;	
	double min;
	double max;
	int64 min_int;	
	int64 max_int;
	char name[16];
};

typedef struct _STATSinfo STATSinfo;
typedef struct _Statistics Statistics;

struct _Statistics {
	int64 num;
	double sum;
	double sum_of_squares;
	double min;
	double max;
	int64  min_int;
	int64  max_int;
	char name[16];

};



// functions

void Statistics_Init( Statistics *s, char *name);
void Statistics_Snap( Statistics *s, STATSinfo *stats_info );
void Statistics_Info( Statistics *s, STATSinfo *stats_info);
void Statistics_print_info( STATSinfo *stats_info, int option, char loc_rem);
void Statistics_print_info_file( STATSinfo *stats_info, int option, char loc_rem, FILE *output);


INLINE void Statistics_Add(Statistics* s, double value)
{
  /* include value in the statistics */
  s->num++;
  s->sum += value;
  s->sum_of_squares += value*value;
  
  if (s->min > value) s->min = value;
  if (s->max < value) s->max = value;

}

INLINE void Statistics_Add_int(Statistics* s, int64 value)
{
  /* include value in the statistics */
  s->num++;
  s->sum += value;
  s->sum_of_squares += value*value;
  
  if (s->min_int > value) s->min_int = value;
  if (s->max_int < value) s->max_int = value;

}

INLINE double Statistics_Mean(Statistics* s)
{
  if (s->num >0) return (s->sum / s->num); 
  else  return 0;
}

INLINE double Statistics_Var(Statistics* s)
{
  double mean = Statistics_Mean(s);
  double var= (s->num >0)? (s->sum_of_squares/s->num) - mean*mean: 0.0;
  if (var<0) {
    return 0.0;
  } else {
    return var;
  }
}

INLINE double Statistics_StdDev(Statistics* s)
{
  return sqrt(Statistics_Var(s));
}

INLINE double Statistics_Min(Statistics* s)
{
  return s->min;
}

INLINE double Statistics_Max(Statistics* s)
{
  return s->max;
}

#endif /* STATISTICS_LOADED */
