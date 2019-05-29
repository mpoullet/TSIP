/*******************************************************************
 *
 * File....: bias.c
 * Function: Functions for bias compensation.
 * Author..: Thomas Schatton
 * Date....: 11/04/2010
 * 18.07.2011 Strutz: BILD ->range instead of -> maxValue
 * 18.07.2011 Strutz: no bias compensation in context 0
 ********************************************************************/

#include "bias.h"
#ifdef Q_DEBUG
extern FILE *debug_out;
#endif

/*------------------------------------------------------------------
 * getSignedBiasKontext( AUSWAHL *a)
 *
 * Computes the context at the currently selected pixel as in 
 * JPEG-LS (cx' in [Str09]).
 *-----------------------------------------------------------------*/
static int getSignedBiasKontext( AUSWAHL *a)
{
  int     gradient;   /* current gradient */
  int     gradCx = 0; /* linear combination of quantized gradients */
  unsigned char i;
          
  /* gradient component */
  for ( i = 0; i < 3; i++)
  {
    /* get a gradient */
    gradient = -getGradient( a, i);
    
    /* quantization */
    if      (gradient <= -21) gradient = -4;
    else if (gradient <=  -7) gradient = -3;
    else if (gradient <=  -3) gradient = -2;
    else if (gradient < 0)    gradient = -1;
    else if (gradient <= 0)   gradient = 0;
    else if (gradient <   3)  gradient = 1;
    else if (gradient <   7)  gradient = 2;
    else if (gradient <  21)  gradient = 3;
    else                      gradient = 4;

    /* combine with previous gradients */
    gradCx += ((UINT)powl(9, i) * gradient);
  }

  return gradCx;
}

/*------------------------------------------------------------------
 * rescale( BIAS *b, int e)
 *
 * rescales the bias compensation using the prediction error e
 * and the currently selected position.
 *-----------------------------------------------------------------*/
void rescale( BIAS *b, int e)
{
  /* calculate the current context */
  int cx = getSignedBiasKontext( b->aw);
  if (cx < 0)
  {
    e = -e;
    cx = -cx;
  }
#ifdef Q_DEBUG
	if (debug_out != NULL)
	{
			fprintf( debug_out, "e=%3d cx=%3d A=%3d B=%3d C=%3d N=%3d",
				e, cx, b->A[cx], b->B[cx], b->C[cx], b->N[cx]);
	}
#endif

  /* rescale A, B, N */
  b->A[cx] += abs(e);
  b->Bi[cx] += e;

  if ((unsigned)b->N[cx] == b->reset)
  {
    b->A[cx] >>= 1;
    b->N[cx] >>= 1;
    b->Bi[cx] = b->Bi[cx] >= 0 ? b->Bi[cx] >> 1 : -((1 - b->Bi[cx]) >> 1);
  }
  b->N[cx]++;

  /* calculate the correction value */
  if (b->Bi[cx] <= -b->N[cx])
  {
    if (b->C[cx] > -((int)b->aw->src->range / 2)) 
      b->C[cx]--;

    b->Bi[cx] += b->N[cx];
    
    if (b->Bi[cx] <= -b->N[cx]) 
      b->Bi[cx] = 1 - b->N[cx];
  }
  else if (b->Bi[cx] > 0)
  {
    if (b->C[cx] < ((int)b->aw->src->range / 2) - 1) 
      b->C[cx]++;
    
    b->Bi[cx] -= b->N[cx];
    
    if (b->Bi[cx] > 0) 
      b->Bi[cx] = 0;
  }
}

/*------------------------------------------------------------------
 * getOffset( BIAS *b)
 *
 * Returns the correction value for the context at the selected
 * position.
 *-----------------------------------------------------------------*/
int getOffset( BIAS *b)
{
  int sign = 1,
      cx = getSignedBiasKontext( b->aw);

	if (cx == 0)
	{
		return 0; /* no bias in context == 0	*/
	}
	else 
	{
		if (cx < 0)
		{
			sign = -1;
			cx = abs(cx);
		}
		return (sign * b->C[ cx]);
	}
}

/*------------------------------------------------------------------
 * newBias( BIAS *b, AUSWAHL *a, PARAMETER *p)
 *
 * Constructor: Allocates and initializes a new BIAS structure.
 *-----------------------------------------------------------------*/
int newBias( BIAS **ptrB, AUSWAHL *a, PARAMETER *p)
{
  UINT i, x;
  BIAS *b;

  TRY( ECALLOC( *ptrB, 1, BIAS, 80));
  b = *ptrB;

  b->aw = a;                  /* pointer to selection structure */
  b->reset = p->biasReset;
  
  /* allocation of A, B, C, N */
  TRY( ECALLOC( b->A, 365, long, 81));
  TRY( ECALLOC( b->Bi, 365, long, 81));
  TRY( ECALLOC( b->C, 365, long, 81));
  TRY( ECALLOC( b->N, 365, long, 81));
  
  /* compute floor(RANGE + 32 / 64) */
  x = (a->src->range + 32) >> 6;

  /* initialization of A, B, C, N */
  for ( i = 0; i < 365; i++)
  {
    b->N[i] = 1;
    b->A[i] = x > 2 ? x : 2;
  }

  return 0;
}

/*------------------------------------------------------------------
 * freeBias( BIAS *b)
 *
 * Destructor: Frees a BIAS structure.
 *-----------------------------------------------------------------*/
void freeBias( BIAS *b)
{
  /* free allocated arrays */
  FREEPTR( b->A);
  FREEPTR( b->Bi);
  FREEPTR( b->C);
  FREEPTR( b->N);
  /* free structure */
  FREEPTR( b);
}