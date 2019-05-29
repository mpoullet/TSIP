/*******************************************************************
 *
 * File....: predictors.c
 * Function: Predictor functions.
 * Author..: Thomas Schatton
 * Date....: 01/13/2010
 *
 ********************************************************************/

#include "predictors.h"

/*------------------------------------------------------------------
 * (*predictor[])(AUSWAHL *a)
 *
 * Function pointer array referencing all predictor functions.
 *-----------------------------------------------------------------*/
UINT (*predictor[])(AUSWAHL *a) =
{
	pMED, pPaeth, pLinearLQ, pGAP, pCoBALP, pTMatching
};

/*------------------------------------------------------------------
 * *getPredictorName( int x)
 *
 * Returns the name of the predictor indexed by x.
 *-----------------------------------------------------------------*/
char *getPredictorName( int x)
{
  switch ( x)
  {
    case 0:
      return "Median Edge Detection";
      break;
    case 1:
      return "Paeth Predictor";
      break;
    case 2:
      return "Linear Prediction (Least-Squares)";
      break;
    case 3:
      return "Gradient Adjusted Predictor (GAP)";
      break;
    case 4:
      return "Lin. Prediction (recursive L.-Sq.) (CoBaLP)";
      break;
    case 5:
      return "Template Matching (non-local)";
      break;
    default:
      return "- Not defined -";
  }
}

/*------------------------------------------------------------------
 * initPredictors( AUSWAHL *a, BOOL *pActive)
 *
 * Initializes predictor variables for all enabled predictors.
 *-----------------------------------------------------------------*/
int initPredictors( AUSWAHL *a, BOOL *pActive, PARAMETER *param)
{
  if (pActive[2] == TRUE) /* pLinearLQ enabled */
    TRY( plinearInitialize());
  if (pActive[4] == TRUE) /* pCoBALP enabled */
    TRY( cobalpInitialize( a));
  if (pActive[5] == TRUE) /* pTMatching enabled */
    TRY( tmatchingInitialize( a, param->tmSigma));

  return 0;
}

/*------------------------------------------------------------------
 * freePredictors( BOOL *pActive)
 *
 * Frees the predictor variables.
 *-----------------------------------------------------------------*/
void freePredictors( BOOL *pActive)
{
  if (pActive[2] == TRUE)
    plinearFree();
  if (pActive[4] == TRUE)
    cobalpFree();
}

/*------------------------------------------------------------------
 * adaptPredictor( AUSWAHL *a, BOOL *pActive, UINT pNr, int pError)
 *
 * Calls predictor adaption functions for the selected predictor
 * if available.
 *-----------------------------------------------------------------*/
void adaptPredictor( AUSWAHL *a, BOOL *pActive, UINT pNr, int pError)
{
  //printf("pNr is %d\n", pNr);
  switch (  pNr)
  {
    case 4:
      //printf("adapting cobalp\n");
      if (pActive[4] == TRUE) cobalpAdaption( a, pError);
      break;
    default:
      break;
  }
}

/*******************************************************************
 *
 * Predictors
 *
 *******************************************************************/

/*------------------------------------------------------------------
 * pMED( AUSWAHL *a)
 *
 * Median Edge Detection
 *-----------------------------------------------------------------*/
UINT pMED( AUSWAHL *a)
{
	UINT aPx, bPx, cPx, minPx, maxPx;

	aPx = getPixel( a, 'a');
	bPx = getPixel( a, 'b');
	cPx = getPixel( a, 'c');

	if (aPx > bPx)
	{
		maxPx = aPx;
		minPx = bPx;
	}
	else
	{
		maxPx = bPx;
		minPx = aPx;
	}

	return (cPx > maxPx 
            ? minPx 
            : (cPx < minPx 
                ? maxPx 
                : aPx - cPx + bPx));
}

/*------------------------------------------------------------------
 * pPaeth( AUSWAHL *a)
 *
 * Paeth Predictor.
 *-----------------------------------------------------------------*/
UINT pPaeth( AUSWAHL *a)
{
	UINT aPx, bPx, cPx, pA, pB, pC, p;

	aPx = getPixel( a, 'a');
	bPx = getPixel( a, 'b');
	cPx = getPixel( a, 'c');

	p = aPx + bPx - cPx;
	pA = abs( (int)(p - aPx));
	pB = abs( (int)(p - bPx));
	pC = abs( (int)(p - cPx));

	return (pA <= pB && pA <= pC 
            ? aPx 
            : (pB <= pC ? bPx : cPx));
}

/*------------------------------------------------------------------
 * pLinearLQ( AUSWAHL *a)
 *
 * Linear prediction using a non-recursive least-squares algorithm
 * for the computation the optimal weights.
 *-----------------------------------------------------------------*/
UINT pLinearLQ( AUSWAHL *a)
{
  return plinearXHat( a);
}


/*------------------------------------------------------------------
 * pGAP( AUSWAHL *a)
 *
 * Gradient Adjusted Predictor.
 *-----------------------------------------------------------------*/
UINT pGAP( AUSWAHL *a)
{
  int dH, dV,   /* horizontal/vertical gradient */
      I,        /* prediction value (intensity) */
      n  = (int)getPixel( a, 'b'),
      w  = (int)getPixel( a, 'a'),
      ne = (int)getPixel( a, 'd'),
      nw = (int)getPixel( a, 'c'),
      nn = (int)getPixel( a, 'h'),
      ww = (int)getPixel( a, 'e');

  dH = abs( w - ww) + abs( n - nw) + abs( ne - n);
  dV = abs( w - nw) + abs( n - nn) + abs( ne - (int)getPixel( a, 'i'));

  if      (dV - dH > 80)  /* sharp horizontal edge */
    I = w;
  else if (dV - dH < -80) /* sharp vertical edge */
    I = n;
  else
  {
    I = (w + n) / 2 + (ne - nw) / 4;

    if      (dV - dH > 32)  /* horizontal edge */
      I = (I + w) / 2;
    else if (dV - dH > 8)   /* weak horizontal edge */
      I = (3 * I + w) / 4;
    else if (dV - dH < -32) /* vertical edge */
      I = (I + n) / 2;
    else if (dV - dH < -8)  /* weak vertical edge */
      I = (3 * I + n) / 4;
  }
  
  return (UINT)I;
}

/*------------------------------------------------------------------
 * pCoBALP( AUSWAHL *a)
 *
 * Linear prediction using a recursive least-squares algorithm for
 * the computation of the optimal weights.
 *-----------------------------------------------------------------*/
UINT pCoBALP( AUSWAHL *a)
{
  return cobalpXHat( a);
}

/*------------------------------------------------------------------
 * pTMatching( AUSWAHL *a)
 *
 * Non-local prediction using a template/pattern matching algorithm.
 *-----------------------------------------------------------------*/
UINT pTMatching( AUSWAHL *a)
{
  return tmatchingXHat( a);
}