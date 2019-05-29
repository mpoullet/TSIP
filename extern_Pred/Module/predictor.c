/*******************************************************************
 *
 * File....: predictor.c
 * Function: Functions for pixel prediction.
 * Author..: Thomas Schatton
 * Date....: 11/04/2010
 * 18.07.2011 Strutz: BILD ->range instead of -> maxValue
 * 19.07.2011 predictionError() separate en- and decoding
 * 27.03.2014 Strutz: e += p->halfRange; before modulo operation
 ********************************************************************/

#include <assert.h>
#include "predictor.h"

#ifdef Q_DEBUG
extern FILE *debug_out;
#endif

#define MODULO_RANGE2( e, l, u, r) 		\
	(e) < (l) ? (e) + (r) : 		\
		(e) > (u) ? (e) - (r) : (e);	


/*------------------------------------------------------------------
 * predictorIsActive( PREDICTOR_S *p, UINT pNr)
 *
 * Checks whether the predictor specified by the global index pNr
 * is active and returns the predictor's index in *assoc if the
 * predictor is active.
 *-----------------------------------------------------------------*/
/* Strutz: substituted by inline operations
int predictorIsActive( PREDICTOR_S *p, UINT pNr)
{
  UINT i;

  for ( i = 0; i < p->activePredictors && p->assoc[i] != pNr; i++);

  return i >= p->activePredictors ? -1 : (int)i;
}
*/
/*------------------------------------------------------------------
 * xHat( PREDICTOR_S *p, UINT pNr)
 *
 * Calculates the prediction value for the currently selected pixel
 * using the predictor indexed by pNr.
 *-----------------------------------------------------------------*/
UINT xHat( PREDICTOR_S *p, UINT pNr)
{
	int ret;  /* return value */

  /* predictor function call */
	ret = (*predictor[p->assoc[pNr]])(p->a);
	
  /* add correction value if bias compensation is active */
  if (p->biasActive[pNr] == TRUE)
    ret += getOffset( p->bias[pNr]);
  
  /* truncate to pixel value range */
  if (ret >= p->range) ret = p->range - 1;
  if (ret < 0)         ret = 0;

	return (UINT)ret;
}

/*------------------------------------------------------------------
 * xHatW( PREDICTOR_S *p, float *w, UINT **x)
 *
 * Calculates the weighted mean of the prediction values of all
 * activated predictors. The weights are specified by *w. *x holds
 * the prediction values returned by each predictor.
 *-----------------------------------------------------------------*/
UINT xHatW( PREDICTOR_S *p, float *w, UINT **x)
{
  float   xMean = 0,	/* mean value */
          wSum = 0;   /* sum of weights */
  UINT    pNr;

  /* calculate mean prediction value */
  for ( pNr = 0; pNr < p->activePredictors; pNr++)
  {
    (*x)[pNr] = xHat( p, pNr); /* calculate prediction value */
    xMean += w[pNr] * (float)(*x)[pNr];
    wSum += w[pNr];
  }
	xMean /= wSum;

  return (UINT)floor( xMean + 0.5);
}

/*------------------------------------------------------------------
 * predictionError( PREDICTOR_S *p, UINT xH)
 *
 * Calculates the prediction error (or reconstructs the original
 * pixel value if reconstruction is activated). The prediction error
 * will be returned and written to the output image.
 *-----------------------------------------------------------------*/
unsigned int predictionError( PREDICTOR_S *p, unsigned int xH)
{
  int e, x;

#ifdef OLDXX
	/* compute prediction error e = x - xHat
   * (or reconstruct pixel value x = e + xHat)
	 */
	e = p->reconstruction == FALSE 
    ? (int)p->a->src->wert[p->a->r][p->a->c] - (int)xH   /* prediction e = x-XHat*/
    : (int)p->a->src->wert[p->a->r][p->a->c] + (int)xH;  /* reconstruction x = e+XHat*/
	
  /* domain adjustment (modulo) */
	if      (e >= p->halfRange) e -= p->range;
  else if (e < -p->halfRange) e += p->range;
  
  /* move error to pixel value domain */
	e += p->halfRange;

	//assert( e < p->range); 

	/*  Strutz: is required for reconstruction
	 * two separate functions should be considered
	 */
	if (e >= p->range)
	{
		e -= p->range; 
	}
	/**/

  /* write error to output image */
	p->a->tar->wert[p->a->r][p->a->c] = (UINT)e;

  return (UINT)e;
#else
	if (p->reconstruction == FALSE)
	{
		/* encoding	*/
		e = (int)p->a->src->wert[p->a->r][p->a->c] - (int)xH;  /* prediction e = x-XHat*/

		/* domain adjustment (modulo) */
		//e += p->halfRange; // Strutz: this offset should lead to a unimodal distribution
												// but for some reason it prevents lossless decoding
		//if (e < 0) e += p->range;
		//else if (e >= p->range) e -= p->range; // modulo: result is in range of 0..p->range

		e = MODULO_RANGE2( e, -p->halfRange,	p->halfRangem1, p->range);
		e += p->halfRange;	/* shift to non-negative range	*/

		/* write error to output image */
		p->a->tar->wert[p->a->r][p->a->c] = (UINT)e;

		return e;
	}
	else
	{
		/* decoding	*/
		e = (int)p->a->src->wert[p->a->r][p->a->c] - p->halfRange;
		x = e + (int)xH;  /* reconstruction x = e+XHat*/
		x = MODULO_RANGE2( x, 0, p->range-1, p->range);


		// x = (int)p->a->src->wert[p->a->r][p->a->c] + (int)xH;  /* reconstruction x = e+XHat*/

		/* domain adjustment (modulo) */
		//x -= p->halfRange; // Strutz undo the offset
		// if      (x >= p->range) x -= p->range;
		//else if (x < 0) x += p->range;

		/* write error to output image */
		p->a->tar->wert[p->a->r][p->a->c] = (UINT)x;

		return x;
	}
#endif
}  

/*------------------------------------------------------------------
 * writePredictionError( PREDICTOR_S *p, UINT pNr, BOOL doRescale)
 *
 * Calculates the prediction error for the currently selected pixel
 * using the predictor indexed by pNr and writes it to the output
 * image. The calculated value will be used for bias rescaling and
 * predictor adaption if doRescale is set true.
 *-----------------------------------------------------------------*/
void writePredictionError( PREDICTOR_S *p, UINT pNr, BOOL doRescale)
{
	AUSWAHL *a = p->a;  /* pointer to selection structure */
	UINT    xH;         /* prediction value */
  int     pError;

	/* calculate prediction error */
  xH = xHat( p, pNr);
	predictionError( p, xH);

  pError = p->reconstruction == FALSE 
    ? (int)a->src->wert[a->r][a->c] - (int)xH  /* prediction */
    : (int)a->tar->wert[a->r][a->c] - (int)xH; /* reconstruction */
     
  /* domain adjustment * Strutz this can change the direction of adaptation
	if      (pError >= p->halfRange) pError -= p->range;
  else if (pError < -p->halfRange) pError += p->range;
/**/
#ifdef Q_DEBUG
	if (debug_out != NULL)
	{
		if (p->reconstruction == FALSE)
			fprintf( debug_out, "(%3d,%3d) x=%3d xHat=%3d ",	a->r, a->c, a->src->wert[a->r][a->c], xH);
		else
			fprintf( debug_out, "(%3d,%3d) x=%3d xHat=%3d ",	a->r, a->c, pError, xH);
	}
#endif
 
  /* rescale bias if activated */
  if (p->biasActive[pNr] == TRUE && doRescale == TRUE)
    rescale( p->bias[pNr], pError);

  /* predictor adaption if needed */
  if (doRescale == TRUE)
    adaptPredictor( a, p->pActive, p->assoc[pNr], pError);

#ifdef Q_DEBUG
	if (debug_out != NULL)
	{
			fprintf( debug_out, "\n");
	}
#endif
}

/*------------------------------------------------------------------
 * writePredictionErrorW( ...)
 *
 * Calculates the prediction error for the currently selected pixel
 * using the weighted mean of the prediction values of all predictors
 * as xHat. The result will be written to the output image. The
 * predictor weights are specified by *w. *xH holds the prediction
 * values of each predictor. The prediction error will be used for
 * bias rescaling and predictor adaption if doRescale is true.
 *-----------------------------------------------------------------*/
int writePredictionErrorW( PREDICTOR_S *p, float *w, UINT **xH, 
                           BOOL doRescale)
{
  AUSWAHL *a = p->a;  /* pointer to selection structure */
  UINT    pNr;
  int     pError, pMean;
	unsigned int xHatmean;

	/* encoder: calculate mean prediction error
	 * decoder: calculate reconstruction value x
	 */
  xHatmean = xHatW( p, w, xH);
  pMean = predictionError( p, xHatmean);
 
#ifdef Q_DEBUG
	if (debug_out != NULL)
	{
		if (p->reconstruction == FALSE)
			fprintf( debug_out, "(%3d,%3d) x=%3d xHat=%3d ",	
				a->r, a->c, a->src->wert[a->r][a->c], xHatmean);
		else
			fprintf( debug_out, "(%3d,%3d) x=%3d xHat=%3d ",	
				a->r, a->c, a->tar->wert[a->r][a->c], xHatmean);
	}
#endif
 
  /* calculate individual prediction errors */
  for ( pNr = 0; pNr < p->activePredictors; pNr++)
  {
    pError = p->reconstruction == FALSE 
    ? (int)a->src->wert[a->r][a->c] - (int)(*xH)[pNr]  /* prediction */
    : (int)a->tar->wert[a->r][a->c] - (int)(*xH)[pNr]; /* reconstruction */
      
    /* domain adjustment * Strutz this can change the direction of adaptation
	  if      (pError >= p->halfRange) pError -= p->range;
    else if (pError < -p->halfRange) pError += p->range;
/**/
    /* bias rescaling if bias is activated */
    if (p->biasActive[pNr] == TRUE && doRescale == TRUE)
      rescale( p->bias[pNr], pError);

    /* predictor adaption if needed */
    if (doRescale == TRUE)
      adaptPredictor( a, p->pActive, p->assoc[pNr], pError);
  }

#ifdef Q_DEBUG
	if (debug_out != NULL)
	{
			fprintf( debug_out, "\n");
	}
#endif

  return pMean;
}

/*------------------------------------------------------------------
 * restorePredictionErrorB( PREDICTOR_S *p, UINT pNr)
 *
 * Restores the prediction error for the currently selected pixel
 * using the predictor indexed by pNr. The calculated value will be 
 * used for bias rescaling and predictor adaption.
 * The currently selected pixel has to be the correctly
 * reconstructed value.
 *-----------------------------------------------------------------*/
void restorePredictionErrorB( PREDICTOR_S *p, UINT pNr)
{
	AUSWAHL *a = p->a;  /* pointer to selection structure */
  int     x, xH, pError;

	/* calculate prediction error */
  xH = (int)xHat( p, pNr);
  x = (int)p->a->tar->wert[p->a->r][p->a->c];
	
  pError = x - xH;
     
  /* domain adjustment *  Strutz this can change the direction of adaptation
	if      (pError >= p->halfRange) pError -= p->range;
  else if (pError < -p->halfRange) pError += p->range;
 /**/
  /* rescale bias if activated */
  if (p->biasActive[pNr] == TRUE)
    rescale( p->bias[pNr], pError);

  /* predictor adaption */
  adaptPredictor( a, p->pActive, p->assoc[pNr], pError);
}

/*------------------------------------------------------------------
 * newPredictor( ...)
 *
 * Constructor: Allocates and initializes a new PREDICTOR_S 
 * structure.
 *-----------------------------------------------------------------*/
int newPredictor( PREDICTOR_S **ptrP, AUSWAHL *a, PARAMETER *param, 
                  BOOL reconstruction)
{
	UINT i, j;
  PREDICTOR_S *p;

  TRY( ECALLOC( *ptrP, 1, PREDICTOR_S, 50));
  p = *ptrP;

	p->a = a;  /* pointer to selection structure */
	p->reconstruction = reconstruction;

  /* count activated predictors */
  p->activePredictors = 0;
	for ( i = 0; i < param->anzPredictors; i++) 
    if (param->predictors[i] == TRUE) p->activePredictors++;
  
	/* allocate all arrays */
  TRY( ECALLOC( p->assoc,      p->activePredictors,  UINT, 51));
  TRY( ECALLOC( p->biasActive, p->activePredictors,  BOOL, 52));
  TRY( ECALLOC( p->bias,       p->activePredictors,  BIAS*, 53));
  TRY( ECALLOC( p->pActive,    param->anzPredictors, BOOL, 54));

  /* generate index associations */
  j = 0;
  for (i = 0; i < param->anzPredictors; i++)
  {
	  if (param->predictors[i] == TRUE) 
	  {
      p->pActive[i] = TRUE;
		  p->assoc[j] = i;
      if (param->bias[i] == TRUE) p->biasActive[j] = TRUE;
		  j++;
	  }
  }
  	
  /* initialize bias structures */
  for (i = 0; i < p->activePredictors; i++)
  {
    if (p->biasActive[i] == TRUE)
    {    
      TRY( newBias( &(p->bias[i]), a, param));
    }
    else
      p->bias[i] = NULL;
  }     
    
  /* variables for range */
  p->range = a->src->range;   /* size of domain */
  p->halfRange = p->range >> 1;   /* half size of domain */
  /* Strutz: problem: if U and V get a different offset (halfRange) 
	 * then interleaving YYuvuv is not optimal, because nor runs of 
	 * equal values can be found before BWT
	 */
  p->halfRangem1 = (p->range - 1) >> 1;   /* half size of domain */
  /* initialize predictors */
  TRY( initPredictors( a, p->pActive, param));

	return 0;
}

/*------------------------------------------------------------------
 * freePredictor( PREDICTOR_S *p)
 *
 * Destructor: Frees a PREDICTOR_S structure.
 *-----------------------------------------------------------------*/
void freePredictor( PREDICTOR_S *p)
{
  UINT i;

  /* free predictor variables */
  freePredictors( p->pActive);
  /* free bias structures */
  for ( i = 0; i < p->activePredictors; i++)
    if ( p->bias[i] != NULL) freeBias( p->bias[i]);
  /* free arrays */
  FREEPTR( p->assoc);
  FREEPTR( p->biasActive);
  FREEPTR( p->bias);
  FREEPTR( p->pActive);
  /* free structure */
  FREEPTR( p);
}