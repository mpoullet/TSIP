/*******************************************************************
 *
 * File....: parameter.c
 * Function: Functions for manipulating a PARAMETER structure.
 * Author..: Thomas Schatton
 * Date....: 01/13/2011
 *
 ********************************************************************/

#include "parameter.h"

/*------------------------------------------------------------------
 * serializeParameter( UINT **ptrStream, PARAMETER *p)
 *
 * Converts a parameter structure to an array of UINT. This array
 * could be transmitted as side information in order to ensure an
 * equal configuration when reconstructing an image.
 * - stream[0]:    blockMode-Flag (1 Bit), biasReset (7 Bit),
 *                 -not used- (2 Bit), anzPredictors (6 Bit),
 *                 cxPx (4 Bit), cxGrad (4 Bit),
 *                 wMeanMode-Flag (1 Bit), -not used- (7 Bit).
 * - stream[1]:    blockWidth & blockHeight (2x 2 Byte). only set if
 *                 blockMode is TRUE.
 * - stream[2..3]: list of active predictors.
 * - stream[4..5]: list of active bias concatenations.
 *-----------------------------------------------------------------*/
int serializeParameter( UINT **ptrStream, PARAMETER *p)
{
  UINT tmp, i,  
       length,  /* output length */
       n = 0;   /* current position (index of stream) */
	UINT *stream;	 										

  /* determine output length and allocate the output array */
	length = 1 + (p->blockMode == TRUE ? 1 : 0) + 4;
  TRY( ECALLOC( *ptrStream, length, UINT, 24));
  stream = *ptrStream;

  /* 1st integer */
  tmp = 0;
  /* set highest bit if blockMode is TRUE */
  if ( p->blockMode == TRUE) 
    tmp |= (1 << 31);
  /* biasReset */
  tmp |= (p->biasReset & 0x7F) << 24;
  /* anzPredictors */
  tmp |= (p->anzPredictors & 0x3F) << 16; 
  /* cxPx, cxGrad */
  tmp |= ((p->cxGrad & 0x0F) << 4) << 8;
  tmp |= (p->cxPx & 0x0F) << 8;
  /* wMeanMode */
  if (p->wMeanMode == TRUE)
    tmp |= (1 << 7);
  
  stream[n++] = tmp;

  /* 2nd integer: blockHeight, blockWidth */
  if (p->blockMode == TRUE)
  {
	  tmp = 0;
	  tmp |= (p->blockHeight & 0xFFFF) << 16;
	  tmp |= (p->blockWidth & 0xFFFF);
	  stream[n++] = tmp;
  }

  /* active predictors and bias concatenations */
  for ( i = 0; i < p->anzPredictors && i < 32; i++)	
  {
	  if ( p->predictors[i] == 1) 
      stream[n] |= (1 << i);
	  if ( p->bias[i] == 1)		  
      stream[n + 2] |= (1 << i);
  }
  n++;
  for ( i = 32; i < p->anzPredictors && i < 64; i++) 
  {
	  if (p->predictors[i] == 1) 
      stream[n]	|= (1 << (i - 32));
	  if (p->bias[i] == 1)		  
      stream[n + 2] |= (1 << (i - 32));
  }

	return 0;
}

/*------------------------------------------------------------------
 * print( PARAMETER *p)
 *
 * Prints current parameters on stdout.
 *-----------------------------------------------------------------*/
int printParameter( PARAMETER *p)
{
	char *pred, *bias;
  UINT i = 0;

  TRY( ECALLOC( pred, p->anzPredictors + 1, char, 23));
  TRY( ECALLOC( bias, p->anzPredictors + 1, char, 23));

  for ( i = 0; i < p->anzPredictors; i++)
  {
    pred[i] = p->predictors[i] == FALSE ? '0' : '1';
    bias[i] = p->bias[i]       == FALSE ? '0' : '1';
  }
  pred[p->anzPredictors] = bias[p->anzPredictors] = '\0';

	printf( "\nParameter structure:                                 \n\
- Mode: %s                                                        \n\
        ",
          p->blockMode == TRUE ? "Block" : "Context");

  if (p->blockMode == FALSE)
    printf("(%scalculate weighted means of all predictors)\n",                                        
          p->wMeanMode == TRUE ? "" : "do not ");
  else
    printf("(block width x block height: %dpx x %dpx, using %s comparison)\n",
        p->blockWidth, p->blockHeight, 
        p->selCriterion == 0 ? "Entropy" : "SSE");
  
  printf("- Context:\n\
  - amount of pixels used for modelling contexts:    %d           \n\
  - amount of gradients used for modelling contexts: %d           \n\
- Predictors:                                                     \n\
  - available predictors:       %d                                \n\
  - active predictors:          %s                                \n\
  - bias compensation:          %s                                \n\
  - frequency of bias adaption: %d                                \n\
  - sigma (Template Matching):  %f                               \n",
  p->cxPx,
  p->cxGrad,
  p->anzPredictors,
  pred,
  bias,
  p->biasReset,
  p->tmSigma);

  FREEPTR( pred);
  FREEPTR( bias);

  return 0;
}

/*------------------------------------------------------------------
 * newParameter( PARAMETER **ptrP, UINT anzPredictors)
 *
 * Constructor: Allocates and initializes a parameter structure.
 *-----------------------------------------------------------------*/
int newParameter( PARAMETER **ptrP, UINT anzPredictors)
{
	UINT      i;
  PARAMETER *p;

  TRY( ECALLOC( *ptrP, 1, PARAMETER, 20));
	p = *ptrP;

  /* block */
  p->blockMode = TRUE;
  p->blockHeight = 0;
  p->blockWidth = 0;
  /* context */
  p->cxPx = 0;
  p->cxGrad = 3;
  p->wMeanMode = FALSE;
  /* predictors, bias concatenation */
  p->anzPredictors = anzPredictors;
  TRY( ECALLOC( p->predictors, anzPredictors, BOOL, 21));
  for (i = 0; i < anzPredictors; i++)
    p->predictors[i] = TRUE;
  
  TRY( ECALLOC( p->bias, anzPredictors, BOOL, 22));
	p->biasReset = 64;
  p->tmSigma = 3.0f;
  p->selCriterion = 0;

  return 0;
}

/*------------------------------------------------------------------
 * newParameterFromStream( PARAMETER **ptrP, UINT *in)
 *
 * Converts a serialized parameter stream to a parameter structure.
 *-----------------------------------------------------------------*/
int newParameterFromStream( PARAMETER **ptrP, UINT *in)
{
	UINT tmp, i, 
       n = 0;   /* current position in stream (index) */
  PARAMETER *p;

  p = *ptrP;

  /* read amount of predictors and initialize a new structure */
	tmp = (in[n] >> 16) & 0x3F;
	TRY( newParameter( &p, tmp))
    
  /* read first integer */
	p->blockMode = (in[n] >> 31) == 1 ? TRUE : FALSE;
  p->biasReset = (in[n] >> 24) & 0x7F;
  p->cxGrad = (in[n] >> 12) & 0x0F;
  p->cxPx = (in[n] >> 8) & 0x0F;
  p->wMeanMode = (in[n] >> 7) == 1 ? TRUE : FALSE;

	n++;
  /* read second integer (block height, block width) */
	if (p->blockMode == TRUE)
	{
		p->blockHeight = (in[n] >> 16) & 0xFFFF;
		p->blockWidth = in[n] & 0xFFFF;
		n++;
	}
	/* read active predictors and bias concatenations */
	for ( i = 0; i < p->anzPredictors && i < 32; i++)
	{
		if (((in[n]	>> i) & 1) == 1) 
      p->predictors[i] = 1;
		if (((in[n + 2] >> i) & 1) == 1) 
      p->bias[i] = 1;
	}
	n++;
	for ( i = 32; i < p->anzPredictors && i < 64; i++)
	{
		if (((in[n]	>> (i - 32)) & 1) == 1) 
      p->predictors[i] = 1;
		if (((in[n + 2] >> (i - 32)) & 1) == 1) 
      p->bias[i] = 1;
	}

	return 0;
}

/*------------------------------------------------------------------
 * freeParameter( PARAMETER *p)
 *
 * Destructors: Frees a parameter structure.
 *-----------------------------------------------------------------*/
void freeParameter( PARAMETER *p)
{
  /* free arrays */
  FREEPTR( p->predictors);
  FREEPTR( p->bias);
  /* free structure */
  FREEPTR( p);
}