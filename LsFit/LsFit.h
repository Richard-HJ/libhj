#ifndef LsFit_h
#define LsFit_h

/**@name LsFit
   
   Utility that uses RealTime to implement a StopWatch.  The StopWatch
   is calibrated during initialisation such that only the time between
   start and stop is measured. */


#ifndef INLINE
#define INLINE extern inline
#endif

typedef struct _LsFit LsFit;

struct _LsFit {
    double sum_x;
    double sum_y;
    double sum_xy;
    double sum_xx;
    int sum_n;
};

extern void LsFit_Initialise(LsFit* lsf);
/* zero the least-squares-fit */

extern void LsFit_Data(LsFit* lsf, double x, double y);
/* add a data point to the least-squares-fit */

extern void LsFit_Fit(LsFit* lsf, int *n, double *m, double *c);
/* calculate the least-squares-fit */

#endif

