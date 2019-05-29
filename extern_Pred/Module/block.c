/*******************************************************************
 *
 * File....: block.c
 * Function: Functions for block mode processing.
 * Author..: Thomas Schatton
 * Date....: 11/04/2010
 * 18.07.2011 Strutz: BILD ->range instead of ->maxValue
 ********************************************************************/

#include "block.h"

/*------------------------------------------------------------------
 * startRow( BLOCK *b, UINT blockNr)
 *
 * Returns the row of the first pixel in the block indexed by
 * blockNr.
 *-----------------------------------------------------------------*/
UINT startRow( BLOCK *b, UINT blockNr)
{
	return b->height * (blockNr / b->countX);
}

/*------------------------------------------------------------------
 * startCol( BLOCK *b, UINT blockNr)
 *
 * Returns the column of the first pixel in the block indexed by
 * blockNr.
 *-----------------------------------------------------------------*/
UINT startCol( BLOCK *b, UINT blockNr)
{
	return b->width * (blockNr % b->countX);
}

/*------------------------------------------------------------------
 * calcHistogramm( BLOCK *b, UINT blockNr)
 *
 * Computes the histogramm of the block indexed by blockNr.
 *-----------------------------------------------------------------*/
void calcHistogramm( BLOCK *b, UINT blockNr)
{
	UINT r, c, i, rOffset, cOffset;

  /* reset the histogram array */
	for (i = 0; i < b->imgOut->range; b->hBlock[1][i++] = 0);
  /* get position of first pixel */
	rOffset = startRow(b, blockNr);
	cOffset = startCol(b, blockNr);
  /* count pixel values */
	for ( r = rOffset; 
        r < rOffset + b->height && r < b->imgOut->height; r++)
		for ( c = cOffset; 
          c < cOffset + b->width && c < b->imgOut->width; c++)
			b->hBlock[1][b->imgOut->wert[r][c]]++;
}
/*------------------------------------------------------------------
 * calcBlockSSE( BLOCK *b, UINT blockNr)
 *
 * Computes the SSE of the block indexed by blockNr.
 *-----------------------------------------------------------------*/
unsigned long calcBlockSSE( BLOCK *b, UINT blockNr, int range)
{
	UINT r, c, rOffset, cOffset;
	int e, halfRange;
	unsigned long sse;

  /* reset */
	sse = 0;
	halfRange = range >> 1;
  /* get position of first pixel */
	rOffset = startRow( b, blockNr);
	cOffset = startCol( b, blockNr);
  /* count pixel values */
	for ( r = rOffset; 
        r < rOffset + b->height && r < b->imgOut->height; r++)
	{
		for ( c = cOffset; 
          c < cOffset + b->width && c < b->imgOut->width; c++)
		{
			e = (int)b->imgOut->wert[r][c];
			/* this is dependent on the treatment in predictionError()
			 * since the errors are clamped in the range 0...maxVal 
			 * If they were centered at half range, then the correction
			 * simply would be: e -= halfRange;
			 */
			if (e >= halfRange) e -= range; 
			sse += e * e ;
//			sse += abs( e );	/* SAD */
		}
	}
	return sse;
}

/*------------------------------------------------------------------
 * mergeHistograms( BLOCK *b)
 *
 * Adds the values from b->hBlock[1] to b->hBlock[0] and resets
 * b->hBlock[1].
 *-----------------------------------------------------------------*/
void mergeHistograms( BLOCK *b)
{
  UINT i;

  for (i = 0; i < b->imgOut->range; i++) /* all values */
  {
    b->hBlock[0][i] += b->hBlock[1][i];
    b->hBlock[1][i] = 0;
  }
}

/*------------------------------------------------------------------
 * blockSSE( BLOCK *b)
 *
 * Calculates the SSE of the pixels in all processed blocks.
 *-----------------------------------------------------------------*/
float blockSSE( BLOCK *b)
{
	UINT        i;
  ULONG       curCnt;
  float       SSE = 0.0f; 

  /* calculate SSE */
	for ( i = 0; i < b->imgOut->range; i++) /* all values */
  {
    curCnt = b->hBlock[0][i] + b->hBlock[1][i];
    if (curCnt > 0)
      SSE += (curCnt * (ULONG)powl((int)i - (b->imgOut->range >> 1), 2));
  }
	return SSE;
}


/*------------------------------------------------------------------
 * blockEntropie( BLOCK *b)
 *
 * Calculates the entropy of the pixels in all processed blocks.
 *-----------------------------------------------------------------*/
float blockEntropie( BLOCK *b)
{
	long        N = 0;         /* pixel count */
	const float ln2 = logf(2); /* ln(2) */
	float		    lnN,           /* ln(N) */
              H = 0;         /* entropy */
	UINT        i;
  ULONG       curCnt;
  
  /* switch to SSE if parameter is set */
  if (b->selCriterion == 1)
    return blockSSE(b);

  /* count pixels */
  for ( i = 0; i < b->imgOut->range; i++) 
    N = N + b->hBlock[0][i] + b->hBlock[1][i];

  lnN = logf((float)N);

  /* calculate entropy */
	for ( i = 0; i < b->imgOut->range; i++) /* all values */
  {
    curCnt = b->hBlock[0][i] + b->hBlock[1][i];
		if (curCnt > 0) /* do not try to compute ln(0) */
      H += (float)curCnt * (logf((float)curCnt) - lnN);
  }
	H /= -(N * ln2);

	return H;
}

/*------------------------------------------------------------------
 * newBlock( BLOCK **ptrB, BILD *out, PARAMETER p)
 *
 * Constructor: Allocates and initializes a new block structure.
 *-----------------------------------------------------------------*/
int newBlock( BLOCK **ptrB, BILD *out, PARAMETER *p)
{
  UINT i;
  BLOCK *b;

  TRY( ECALLOC( *ptrB, 1, BLOCK, 70));
  b = *ptrB;

  /* initialization */
	b->imgOut = out;                 /* pointer to output image */
	b->width = p->blockWidth;        /* block width */
	b->height = p->blockHeight;      /* block height */
	b->minEntropie = 1e10;           /* minimal entropy */
	b->minSSE = 999999999;						     /* minimal SSE */
	b->bestPredictor = 0;            /* index of best predictor */
  /* calculate number of blocks (per row, per column, and total) */
	b->countX = (UINT)ceil( (float)out->width / b->width); 
	b->countY = (UINT)ceil( (float)out->height / b->height);
	b->count = b->countX * b->countY;
  b->selCriterion = p->selCriterion;

  /* allocate histogram arrays (previous blocks & current block) */
  TRY( ECALLOC( b->hBlock, 2, ULONG*, 71));
	
  for ( i = 0; i < 2; i++)
  {
    TRY( ECALLOC( b->hBlock[i], out->range, ULONG, 71));
  }

	return 0;
}

/*------------------------------------------------------------------
 * freeBlock( BLOCK *b)
 *
 * Destructor: Frees a block structure.
 *-----------------------------------------------------------------*/
void freeBlock( BLOCK *b)
{
  UINT i;

  /* free histogram arrays */
  for ( i = 0; i < 2; i++)
    FREEPTR( b->hBlock[i]);
  FREEPTR( b->hBlock);

  /* free structure */
  free( b);
}