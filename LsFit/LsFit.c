/*
      LsFit.c
 */
#include <stdio.h>

#include "LsFit.h"

void LsFit_Initialise(LsFit* lsf)
/* --------------------------------------------------------------------- */
{
/* zero the least-squares-fit */
  lsf->sum_x  =0;
  lsf->sum_y  =0;
  lsf->sum_xy =0;
  lsf->sum_xx =0;
  lsf->sum_n  =0;

}

void LsFit_Data(LsFit* lsf, double x, double y)
/* --------------------------------------------------------------------- */
{
/* add a data point to the least-squares-fit */
  lsf->sum_x  += x;
  lsf->sum_y  += y;
  lsf->sum_xy += x*y;
  lsf->sum_xx += x*x;
  lsf->sum_n ++;
}

void LsFit_Fit(LsFit* lsf, int *n, double *m, double *c)
/* --------------------------------------------------------------------- */
{
/* calculate the least-squares-fit */
  double d;
  d = lsf->sum_n*lsf->sum_xx - lsf->sum_x*lsf->sum_x;

  *m = (lsf->sum_n*lsf->sum_xy - lsf->sum_x*lsf->sum_y)/d;
  *c = (lsf->sum_y*lsf->sum_xx - lsf->sum_xy*lsf->sum_x)/d;
  *n = lsf->sum_n;
}

