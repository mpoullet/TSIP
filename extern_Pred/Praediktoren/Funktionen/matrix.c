/*******************************************************************
 *
 * File....: matrix.c
 * Function: Matrix operations.
 * Author..: Thomas Schatton
 * Date....: 11/04/2010
 *
 ********************************************************************/

#include "matrix.h"

/*------------------------------------------------------------------
 * newMatrix(MATRIX *m, UINT rows, UINT columns)
 *
 * Constructor: Allocates a new matrix structure with the specified
 * dimensions.
 *-----------------------------------------------------------------*/
int newMatrix(MATRIX **ptrM, UINT rows, UINT columns)
{
  UINT   i;
  MATRIX *m;

  /* allocation */
  TRY( ECALLOC( *ptrM, 1, MATRIX, 100));

  m = *ptrM;

  TRY( ECALLOC( m->X, rows, float*, 101));  /* allocate rows */
  for ( i = 0; i < rows; i++)
  {
    TRY( ECALLOC( m->X[i], columns, float, 102)); /* allocate columns */
  }

  /* initialization */
  m->rows = rows;
  m->columns = columns;

  return 0;
}

/*------------------------------------------------------------------
 * freeMatrix(MATRIX *M)
 *
 * Destructor: Frees a matrix structure.
 *-----------------------------------------------------------------*/
void freeMatrix(MATRIX *M)
{
  UINT i;

  /* free array of matrix values */
	if (M != NULL)
  {
    for ( i = 0; i < M->rows; i++) FREEPTR( M->X[i]);
	  FREEPTR( M->X);
  }
  /* free matrix structure */
  FREEPTR( M);
}

/*------------------------------------------------------------------
 * matrixInv(MATRIX *A, MATRIX *Inv)
 *
 * Writes the inverse matrix of matrix A to Inv. Returns an error
 * code if the matrix is not inversable.
 * The function uses the Gauss-Jordan algorithm.
 *-----------------------------------------------------------------*/
int matrixInv( MATRIX *A, MATRIX *Inv)
{
  UINT        r, c, s,      /* row, column, step */
              pR;           /* pivot row */
  float       f, max;       /* pivot coefficient, pivot maximum */
  float       e = 0.01f;    /* accuracy */

  if (A == NULL || Inv == NULL) 
    return 102;
  else
  {
    /* check for square matrix */
    if (!(A->columns == A->rows == Inv->columns == Inv->rows))
      return 103;
    else
    {
      /* initialize output with an identity matrix */
      for ( r = 0; r < Inv->rows; r++)
        for ( c = 0; c < Inv->columns; c++)
          Inv->X[r][c] = r == c ? 1.0f : 0.0f;

      /* elimination steps */
      s = 0;
      do
      {
        /* pivotization */
        max = (float)fabs( A->X[s][s]);
        pR = s;
        for ( r = s + 1; r < A->rows; r++)
          if (fabs( A->X[r][s]) > max)
          {
            max = (float)fabs( A->X[r][s]);
            pR = r;
          }
        if (max < e) return 104; /* no solution exists */
        else
        {
          /* swap rows if necessary */
          if (pR != s)
          {
            float tmp;

            for ( c = 0; c < A->columns; c++)
            {
              tmp         = A->X[s][c];
              A->X[s][c]  = A->X[pR][c];
              A->X[pR][c] = tmp;
              
              tmp           = Inv->X[s][c];
              Inv->X[s][c]  = Inv->X[pR][c];
              Inv->X[pR][c] = tmp;
            }
          }

          /* divide elimination row by pivot coefficient */
          f = A->X[s][s];
          for ( c = 0; c < A->columns; c++)
          {
            A->X[s][c]   /= f;
            Inv->X[s][c] /= f;
          }

          /* elimination */
          for ( r = 0; r < A->rows; r++)
          {
            if (r != s)
            {
              f = -A->X[r][s];
              for ( c = s; c < A->columns; c++)
                A->X[r][c] += f * A->X[s][c];
              for ( c = 0; c < Inv->columns; c++)
                Inv->X[r][c] += f * Inv->X[s][c];
            }
          }
        }
        s++;
      }
      while ( s < A->rows);
    }
  }
  
  return 0;
}

/*------------------------------------------------------------------
 * int matrixMul(MATRIX *A, MATRIX *B, BOOL tpA, BOOL tpB, MATRIX *X)
 *
 * Writes the matrix product of A * B in X. tpA and tpB specify
 * whether the respective input matrices should be transponated
 * prior to multiplication.
 * Returns an error code if the multiplication is not possible
 * because of invalid matrix dimensions.
 *-----------------------------------------------------------------*/
int matrixMul(MATRIX *A, MATRIX *B, BOOL tpA, BOOL tpB, MATRIX *X)
{
  UINT   r, c, k,   
         r1, r2,    /* row counts */
         c1, c2;    /* column counts */
  float  xA, xB;    /* single value in A, B */

  if (A == NULL || B == NULL || X == NULL) 
    return 102;
  else
  {
    /* get r1, r2, c1, c2 */
    r1 = tpA == FALSE ? A->rows    : A->columns;
    c1 = tpA == FALSE ? A->columns : A->rows;
    r2 = tpB == FALSE ? B->rows    : B->columns;
    c2 = tpB == FALSE ? B->columns : B->rows;

    /* check dimensions */
    if ((r2 == c1) && (X->rows == r1) && (X->columns == c2))
    {
      for ( r = 0; r < X->rows; r++)      /* all rows */
      {
        for ( c = 0; c < X->columns; c++) /* all columns */
        {
          X->X[r][c] = 0;

          for ( k = 0; k < c1; k++)       /* sum */
          {
            /* get current values of A and B */
            xA = tpA == FALSE ? A->X[r][k] : A->X[k][r];
            xB = tpB == FALSE ? B->X[k][c] : B->X[c][k];
            /* add product to total sum */
            X->X[r][c] += xA * xB;
          }
        }
      }
    } 
    else return 105;
  }

  return 0;
}

/*------------------------------------------------------------------
 * matrixPrint(MATRIX *A)
 *
 * Writes the values of a matrix on stdout.
 *-----------------------------------------------------------------*/
void matrixPrint(MATRIX *A)
{
  UINT r, c; /* row, column */

  if (A != NULL)
  {
    printf( "Matrix (%dx%d):\n", A->rows, A->columns);
    for ( r = 0; r < A->rows; r++)
    {
      printf( "[ ");
      for ( c = 0; c < A->columns; c++)
        printf( "%4.2f ", A->X[r][c]); 
      printf( "]\n");
    }
  }
  else printf( "NULL\n");
}