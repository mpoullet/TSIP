/*******************************************************************
 *
 * File....: bild.c
 * Function: Functions for manipulating a BILD (image) structure.
 * Author..: Thomas Schatton
 * Date....: 11/02/2010
 * 18.07.2011 Strutz: BILD ->range instead of -> maxValue
 ********************************************************************/

#include "bild.h"

/*------------------------------------------------------------------
 * entropie(BILD *img)
 *
 * Calculates the entropy of the pixel data in an image structure.
 *-----------------------------------------------------------------*/
float entropie(BILD *img)
{
	const float ln2 = logf( 2.);           /* ln(2) */
	float       lnN = logf( (float)img->length); /* ln(N) */
	ULONG       *h = NULL;                /* histogram */
	UINT        i;                      
	float       H = 0;                    /* entropy */

  if (histogramm( &h, img) == 0)
  {
    for ( i = 0; i < img->range; i++) /* all possible values */
	  {
		  if ( h[i] > 0) /* ln(x) only defined for x > 0 */
        H += h[i] * (logf( (float)h[i]) - lnN);
	  }
	  H /= -(img->length * ln2);

    return H;
  }
  else return -1.0; /* error in histogram(..), unable to proceed */
}

/*------------------------------------------------------------------
 * histogramm(ULONG **ptrH, BILD *img)
 *
 * Creates a histogramm of the pixel values in an image structure.
 *-----------------------------------------------------------------*/
int histogramm(ULONG **ptrH, BILD *img)
{
  UINT  r, c;       /* row, column */
	ULONG *h;         /* frequency of occurence for each pixel value */

  TRY( ECALLOC(*ptrH, img->range, ULONG, 13));
  h = *ptrH;

  for ( r = 0; r < img->height; r++)
  {
	  for ( c = 0; c < img->width; c++)
	  {
      /* increase the frequency of occurence for the current
       * pixel's value */
		  h[img->wert[r][c]]++;
	  }
  }

	return 0;
}

/*------------------------------------------------------------------
 * *newBild(BILD **ptrImg, UINT width, UINT height, BYTE bitDepth)
 *
 * Constructor: Allocates and initializes an image structure.
 *-----------------------------------------------------------------*/
int newBild( BILD **ptrImg, UINT width, UINT height, BYTE bitDepth
						/*, UINT maxVal*/)
{
	UINT i;
  BILD *img;

  TRY( ECALLOC( *ptrImg, 1, BILD, 10));
  img = *ptrImg;

  /* image dimensions */
  img->width = width;
  img->height = height;
  img->length = width * height;
  /* range of pixel values */
  img->bitDepth = bitDepth;
	/* worst case scenario
	 * ->range is used for histogramm determination	
	 */
	img->range = (UINT)powl( 2, bitDepth);
	img->maxVal = 0;	/* not known yet	*/

	/* pixel values */
  TRY( ECALLOC( img->wert, height, UINT*, 11));    /* rows */
  for ( i = 0; i < height; i++)
  {
    TRY( ECALLOC( img->wert[i], width, UINT, 12)); /* columns */
  }

	return 0;
}

/*------------------------------------------------------------------
 * freeBild(BILD *img)
 *
 * Destructor: Frees an image structure.
 *-----------------------------------------------------------------*/
void freeBild( BILD *img)
{
	UINT i;

  /* free pixel value arrays */
	if (img != NULL)
  {
    for ( i = 0; i < img->height; i++) FREEPTR( img->wert[i]);
	  FREEPTR( img->wert);
  }
  /* free image structure */
  FREEPTR( img);
}