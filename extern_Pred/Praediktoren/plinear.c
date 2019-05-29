/*******************************************************************
 *
 * File....: plinear.c
 * Function: Calculation of estimation value using a non-recursive
 *           linear prediction algorithm.
 * Author..: Thomas Schatton
 * Date....: 11/04/2010
 *
 * 28.10.2015 Strutz bugfix memory leak array plinearW
 ********************************************************************/

#include "plinear.h"

/*------------------------------------------------------------------
 * plinearInitialize()
 *
 * Allocation and initialization of predictor matrices.
 *-----------------------------------------------------------------*/
int plinearInitialize()
{
  UINT i;
  float weights[] = 
    {1.0f,        1.0f,        sqrtf(2.0f), sqrtf(2.0f), 2.0f,
     sqrtf(5.0f), sqrtf(5.0f), 2.0f,        sqrtf(5.0f), sqrtf(5.0f)};

  TRY( newMatrix(&plinearX      , PLINEARLQ_P, PLINEARLQ_P));
  TRY( newMatrix(&plinearXtMulW , PLINEARLQ_P, PLINEARLQ_P));
  TRY( newMatrix(&plinearXtMulWMulX, PLINEARLQ_P, PLINEARLQ_P));
  TRY( newMatrix(&plinearInv    , PLINEARLQ_P, PLINEARLQ_P));
  TRY( newMatrix(&plinearY      , PLINEARLQ_P, PLINEARLQ_P));
  TRY( newMatrix(&plinearW      , PLINEARLQ_P, PLINEARLQ_P));
  TRY( newMatrix(&plinearVx     , PLINEARLQ_P, 1));
  TRY( newMatrix(&plinearVa     , PLINEARLQ_P, 1));

  /* initialize weights (euklid distance) */
  for ( i = 0; i < plinearW->rows; i++)
    plinearW->X[i][i] = weights[i];

  return 0;
}

/*------------------------------------------------------------------
 * plinearFree()
 *
 * Free predictor matrices.
 *-----------------------------------------------------------------*/
void plinearFree()
{
  freeMatrix( plinearX);
  freeMatrix( plinearXtMulW);
  freeMatrix( plinearXtMulWMulX);
  freeMatrix( plinearInv);
  freeMatrix( plinearY);
  freeMatrix( plinearW);		// Strutz
  freeMatrix( plinearVx);
  freeMatrix( plinearVa);
}

/*------------------------------------------------------------------
 * plinearXHat( AUSWAHL *a)
 *
 * Calculation of prediction value
 *-----------------------------------------------------------------*/
int plinearXHat(AUSWAHL *a)
{
  UINT   r;
  float  xH = 0;   /* prediction value */
  float *tmp;      /* pointer on matrix row */
  UINT  exitCode = 0;

  /* calculate pixel coefficients (weights) 
   * (Least-Squares: a[] = ([X^T * W] * X)^-1 * [X^T * W] * x[]) */
  
  /* push all values down by one row, the first row now 
     contains the last row's values */
  tmp = plinearX->X[plinearX->rows - 1];
  for ( r = plinearX->rows - 1; r > 0; r--)
    plinearX->X[r] = plinearX->X[r - 1];
  plinearX->X[0] = tmp;
    
  /* fill values for current position */
  for ( r = 0; r < plinearVx->rows; r++)
    plinearX->X[0][r] = plinearVx->X[r][0] = (float)getPixel( a, (char)('a' + r));
  
  /* use value of predecessor if unable to solve matrix equation */
  xH = plinearX->X[0][0];  

  /* calc. [X^T * W] */
  exitCode = matrixMul( plinearX, plinearW, TRUE, FALSE, plinearXtMulW);

  /* calc. ([X^T * W] * X) */
  if (exitCode == 0)
    exitCode = matrixMul( plinearXtMulW, plinearX, FALSE, FALSE, plinearXtMulWMulX);

  /* inverse */
  if (exitCode == 0)
    exitCode = matrixInv( plinearXtMulWMulX, plinearInv);

  /* calc. (...)^-1 * [X^T * W] */
  if (exitCode == 0)
    exitCode = matrixMul( plinearInv, plinearXtMulW, FALSE, FALSE, plinearY);

  /* calc. a = Y * x */
  if (exitCode == 0)
    exitCode = matrixMul( plinearY, plinearVx, FALSE, FALSE, plinearVa);

  if (exitCode == 0)
  {
    /* calculation successful */
    xH = 0;

    /* compute prediction value */
    for ( r = 0; r < plinearVa->rows; r++)
      xH += plinearVa->X[r][0] * plinearVx->X[r][0];
  }

  if (xH < 0) xH = 0;
  return (UINT)(xH + 0.5); /* return rounded estimation */
}