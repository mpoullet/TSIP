/*******************************************************************
 *
 * File....: cobalp.c
 * Function: Calculation of estimation value using linear prediction.
 * Author..: Tilo Strutz (CoBaLP)
 *           Thomas Schatton (compatibility adjustments)
 * Date....: 11/04/2010
 * 18.07.2011 Strutz: BILD ->range instead of -> maxValue
 * 19.07.2011 Strutz: bug in cobalpGetContext() fixed
 ********************************************************************/

#include "cobalp.h"

#ifdef Q_DEBUG
extern FILE *debug_out;
#endif

/*------------------------------------------------------------------
 * cobalpInitialize(AUSWAHL *a)
 *
 * Allocation and initialization of weights, mean values and
 * learning rates.
 * Source: CoBALP\encoder.c, lines 125 to 138
 *-----------------------------------------------------------------*/
int cobalpInitialize(AUSWAHL *a)
{
  UINT k, l;
  
  /* allocation */
  TRY( ECALLOC( cobalpLr,    COBALP_NUM_OF_PRED, float, 110));
  TRY( ECALLOC( cobalpW,     COBALP_NUM_OF_CONTEXTS, float*, 110));
  TRY( ECALLOC( cobalpMeanD, COBALP_NUM_OF_CONTEXTS, int*, 110)); 

  for ( k = 0; k < COBALP_NUM_OF_CONTEXTS; k++)
  {
    TRY( ECALLOC( cobalpW[k],     COBALP_NUM_OF_PRED, float, 110));
    TRY( ECALLOC( cobalpMeanD[k], COBALP_NUM_OF_PRED, int, 110));
  }

  /* initialization */
  for ( k = 0; k < COBALP_NUM_OF_CONTEXTS; k++)
  {
		for ( l = 0; l < COBALP_NUM_OF_PRED; l++)
		{
      cobalpW[k][l]     = 0.0f;
      cobalpMeanD[k][l] = a->src->range / 32 + 1;
    }
		/* Strutz: better prediction in the beginning (left neighbour)*/
		cobalpW[k][0] = 1; 
	}

  cobalpLr[0] = cobalpLr[1] = cobalpLr[2] =               0.0003f;
  cobalpLr[3] = cobalpLr[4] =                             0.0002f;
  cobalpLr[5] = cobalpLr[6] = cobalpLr[7] = cobalpLr[8] = 0.0001f;

  return 0;
}

/*------------------------------------------------------------------
 * cobalpFree()
 *
 * Frees CoBaLP arrays.
 *-----------------------------------------------------------------*/
void cobalpFree()
{
  UINT i;

  FREEPTR( cobalpLr);
  for ( i = 0; i < COBALP_NUM_OF_CONTEXTS; i++)
  {
    FREEPTR( cobalpW[i]);
    FREEPTR( cobalpMeanD[i]);
  }
  FREEPTR( cobalpW);
  FREEPTR( cobalpMeanD);
}

/*------------------------------------------------------------------
 * cobalpAdaption(AUSWAHL *a, int pred_err)
 *
 * Adaption of weights.
 * Source: CoBALP\encoder.c, lines 156 to 166
 *-----------------------------------------------------------------*/
void cobalpAdaption(AUSWAHL *a, int pred_err)
{ 
  float diff = (float)pred_err;
  int   *p_mean;
  float *p_w;
  unsigned char l;

  p_mean = cobalpMeanD[cobalpGetContext( a)];
  p_w    = cobalpW[cobalpGetContext( a)];

  for ( l = 0; l < COBALP_NUM_OF_PRED; l++)
  {
    if (getGradient( a, l) != 0)
    {
      p_mean[l] = (p_mean[l] * 7 + abs( getGradient( a, l))) >> 3;
      p_w[l]   += cobalpLr[l] * diff * getGradient( a, l) / p_mean[l];
    }
  }
}

/*------------------------------------------------------------------
 * cobalpXHat( AUSWAHL *a)
 *
 * Calculation of prediction value for the currently selected pixel.
 * Source: CoBALP\predictor.c, lines 128 bis 141 (calculation of
           the variable 'xHat')
 *-----------------------------------------------------------------*/
int cobalpXHat(AUSWAHL *a)
{
  float db = 0.0f;
	int grad, Rb;
  int   ct = cobalpGetContext( a);
  unsigned char i;


#ifdef Q_DEBUG
	if (debug_out != NULL)
	{
			fprintf( debug_out, "ct=%3d ", ct);
	}
#endif
  /* Predict the next value. */
  for ( i = 0; i < 9; i++)
	{
    grad = getGradient( a, i);
    db += cobalpW[ct][i] * grad;
	}
  
  /* limitation of domain of definition to prevent instability */
  db = (float)floor( db + 0.5);
	Rb = getPixel( a, 'b');
  db = db > (float)a->src->range - (float)Rb
        ? (float)a->src->range - (float)Rb
        : db < -((float)Rb)
           ? -((float)Rb)
           : db;
  
  return (int)db + (int)getPixel( a, 'b');
}

/*------------------------------------------------------------------
 * cobalpGetContext( AUSWAHL *a)
 *
 * Computes the current context.
 * Source: CoBALP\predictor.c, lines 57 bis 126 (Calculation of the
 *         variable 'ct')
 *-----------------------------------------------------------------*/
int cobalpGetContext( AUSWAHL *a)
{
  int  ct, fl;
  long mean, maxd;

  if ( abs( getGradient( a, 0)) == 0 && abs( getGradient( a, 1)) == 0
      && abs( getGradient( a, 2)) == 0 )
  {
    ct = 0;
  }
  else /* Context mode. */
  {
    mean = abs( getGradient( a, 2)) + abs( getGradient( a, 1))
           + abs( getGradient( a, 3)) + abs( getGradient( a, 0))
           + abs( getGradient( a, 4)) + abs( getGradient( a, 5));

    ct = 0;

    if      (abs( getGradient( a, 1) * 3) > mean)
      ct = getGradient( a, 1) > 0 ? 1 : 2;
    else if (abs( getGradient( a, 2) * 3) > mean)
      ct = getGradient( a, 2) > 0 ? 3 : 4;
    else if (abs( getGradient( a, 3) * 3) > mean)
      ct = getGradient( a, 3) > 0 ? 5 : 6;
    
    if      (abs( getGradient( a, 0) * 3) > mean)
      ct += getGradient( a, 0) > 0 ?  7 : 14;
    else if (abs( getGradient( a, 4) * 3) > mean)
      ct += getGradient( a, 4) > 0 ? 21 : 28;
    else if (abs( getGradient( a, 5) * 3) > mean)
      ct += getGradient( a, 5) > 0 ? 35 : 42;
    
    /* search highest gradient and decide for horizontal/vertical,
     * if no gradient is significant
     */
    fl = 0;
    maxd = abs( getGradient( a, 1));
    if (maxd < abs( getGradient( a, 2))) 
      maxd = abs( getGradient( a, 2));
    if (maxd < abs( getGradient( a, 3))) 
      maxd = abs( getGradient( a, 3));

    if (maxd < abs( getGradient( a, 0))) 
    {
      maxd = abs( getGradient( a, 0));
      fl = 1;
    }
    if (maxd < abs( getGradient( a, 4))) 
    {
      maxd = abs( getGradient( a, 4));
      fl = 1;
    }
    if (maxd < abs( getGradient( a, 5))) 
    {
      maxd = abs( getGradient( a, 5));
      fl = 1;
    }

    if (fl || ct)
    {
      if      ((maxd << 4) > (mean = getPixel( a, 'a') 
                              + getPixel( a, 'b') + getPixel( a, 'c')
                              + getPixel( a, 'd')))
                                   ct += 49;
      else if ((maxd << 5) > mean) ct += 98;
      else if ((maxd << 6) > mean) ct += 147;
    }
    else
    {
      if      ((maxd << 4) > (mean = getPixel( a, 'a')
                              + getPixel( a, 'b') + getPixel( a, 'c')
                              + getPixel( a, 'd')))
                                   ct += 196;
      else if ((maxd << 5) > mean) ct += 197;
      else if ((maxd << 6) > mean) ct += 198;
    }
  }

  return ct;
}