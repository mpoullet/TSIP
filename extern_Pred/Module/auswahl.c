/*******************************************************************
 *
 * File....: auswahl.c
 * Function: Getter for pixel values, gradients and contexts.
 * Author..: Thomas Schatton
 * Date....: 01/14/2010
 * 18.07.2011 Strutz getPixelByCoordinates() modified according MED rules
 * 19.07.2011 Strutz getGradient() variable grad introduced (better debugging)
 ********************************************************************/
#include "auswahl.h"

#define ORIG_CX
/*------------------------------------------------------------------
 * getPixelByIndex( AUSWAHL *a, int i)
 *
 * Gets a pixel value of the image source by index. Returns
 * BORDERPIXEL if the pixel has not been processed yet.
 *-----------------------------------------------------------------*/
UINT getPixelByIndex( AUSWAHL *a, int i)
{
  return getPixelByCoordinates( a, i / a->src->width, i % a->src->width);
}

/*------------------------------------------------------------------
 * getPixelByCoordinates( AUSWAHL *a, UINT r, UINT c)
 *
 * Gets the value of the pixel at the location (r,c). Returns
 * BORDERPIXEL if the pixel has not been processed yet.
 *-----------------------------------------------------------------*/
UINT getPixelByCoordinates( AUSWAHL *a, int r, int c)
{
	int BORDERPIXEL = a->src->range >> 1;
  /* duplicate border pixels if out of range *
	Strutz
  if (r < 0) 
    r = 0;

  if (c < 0)
    c = 0;
  else if (c >= (int)a->src->width)
    c = a->src->width - 1;
  
  if (r == (int)a->r && c == (int)a->c)
    return c > 0 ? a->src->wert[r][c - 1] : BORDERPIXEL;
*/
	/* following the rules of JPEG-LS's MED predictior	*/


	if (r < 0) return BORDERPIXEL;
	if (c < 0)
	{
		if (r > 0)
		{
			/* take value from first column one row above	*/
			c = 0;
			r--;
		}
		else	return BORDERPIXEL;
	}
	else
	{
		if (c >= (int)a->src->width)
		{
			/* take value from last column same row 	*/
			c = (int)a->src->width - 1;
		}
	}


  /* check for valid pixel coordinates (raster scan) */
    if (r >= (int)a->src->height || r > (int)a->r || 
        (r == (int)a->r && c >= (int)a->c))
    {
      return BORDERPIXEL; /* not yet processed (raster scan) */
    }

  /* block module is enabled */
  if (a->blockMode == TRUE && a->bk != NULL)
  {
    if ((r >= (int)((a->r / a->bk->height) * a->bk->height)) &&
        (c >= (int)((a->c / a->bk->width + 1) * a->bk->width)))
    {
      c = a->c;
    }
  }
          
  /* all checks passed, return the pixel value */
  return a->src->wert[r][c];
}

UINT getKontextSign__( AUSWAHL *a)
{
  KONTEXT *c = a->cx;

  int     gradient;   /* current gradient */
  int     gradCx = 0;   
  unsigned char   i;
          
  /* gradient component */
  for ( i = 0; i < c->cxGrad; i++)
  {
    /* get a gradient */
    gradient = getGradient( a, i);
    /* quantization */
    if      (gradient < -21) gradient = -3;
    else if (gradient <  -7) gradient = -2;
    else if (gradient <  -3) gradient = -1;
    else if (gradient <   3) gradient = 0;
    else if (gradient <   7) gradient = 1;
    else if (gradient <  21) gradient = 2;
    else                     gradient = 3;

    /* combine with previous gradients */
    gradCx += (UINT)powl(7, i) * gradient;
  }
 
  /* return sign */
  return gradCx < 0 ? 1 : 0;
}

#ifdef ORIG_CX
/*------------------------------------------------------------------
 * getKontext( AUSWAHL *a)
 *
 * Computes the context at the currently selected pixel.
 *-----------------------------------------------------------------*/
UINT getKontext( AUSWAHL *a)
{
  KONTEXT *c = a->cx;

  int     gradient;   /* current gradient */
  int     gradCx = 0;
  UINT    sign = 0;       
  float   pxMean = 0; /* mean of all pixels in the context template */
  UINT    shape = 0,  /* binary vector of quantized pixel values */
          gradLk = 0; /* linear combination of quantized gradients */
  unsigned char  i;
          
  /* gradient component */
  for ( i = 0; i < c->cxGrad; i++)
  {
    /* get a gradient */
    gradient = getGradient( a, i);
    /* quantization */
    if      (gradient < -21) gradient = -3;
    else if (gradient <  -7) gradient = -2;
    else if (gradient <  -3) gradient = -1;
    else if (gradient <   3) gradient = 0;
    else if (gradient <   7) gradient = 1;
    else if (gradient <  21) gradient = 2;
    else                     gradient = 3;

    /* combine with previous gradients */
    gradCx += (UINT)powl(7, i) * gradient;
  }
  /* set sign flag if gradLk is negative */
  if (gradCx < 0)
  {
    gradLk = (UINT)abs(gradCx);
    sign = 1u;
  }
  /* combine gradient combination + sign bit */
  gradLk = (gradLk << 1) | sign;

  /* pixel component */
  
  /* mean of pixel values in the context template */
  for ( i = 0; i < c->cxPx; pxMean += getPixel( a, 'a' + i++));
  if (c->cxPx > 0) pxMean /= c->cxPx;

  /* binary quantization of pixel values */
  for ( i = 0; i < c->cxPx; i++)
  {
    /*  1 if pixel value < mean value
     *  0 if pixel value >= mean value */
    if (getPixel( a, 'a' + i) < pxMean) 
      shape |= (1 << i);
  }

  /* combine pixel and gradient number */
  shape |= (gradLk << c->cxPx);

  return shape;
}
#else
/*------------------------------------------------------------------
 * getKontext( AUSWAHL *a)
 *
 * Computes the context at the currently selected pixel.
 *-----------------------------------------------------------------*/
UINT getKontext( AUSWAHL *a)
{
  KONTEXT *c = a->cx;

  int     gradient;   /* current gradient */
  int     gradCx = 0;
  UINT    sign = 0;       
  float   pxMean = 0; /* mean of all pixels in the context template */
  UINT    shape = 0,  /* binary vector of quantized pixel values */
          gradLk = 0; /* linear combination of quantized gradients */
  unsigned char  i;
          
  /* gradient component */
  for ( i = 0; i < c->cxGrad; i++)
  {
    /* get a gradient */
    gradient = getGradient( a, i);
    /* quantization */
    if      (gradient <= -21) gradient = -4;
    else if (gradient <=  -7) gradient = -3;
    else if (gradient <=  -3) gradient = -2;
    else if (gradient <   0)	gradient = -1;
    else if (gradient ==  0)	gradient = 0;
    else if (gradient <   3)	gradient = 1;
    else if (gradient <   7)	gradient = 2;
    else if (gradient <  21)	gradient = 3;
    else											gradient = 4;

    /* combine with previous gradients */
    gradCx += (UINT)powl(9, i) * gradient;
  }
  /* set sign flag if gradLk is negative */
  if (gradCx < 0)
  {
    gradLk = (UINT)abs(gradCx);
    sign = 1u;
  }
  /* combine gradient combination + sign bit */
  //gradLk = (gradLk << 1) | sign;

  return gradLk;
}
#endif

/*------------------------------------------------------------------
 * getPixel( AUSWAHL *a, char val)
 *
 * Returns a pixel value relative to the current position (r,c). If
 * the selected pixel has not been processed yet or is out of the
 * image dimensions, the function will return BORDERPIXEL (as
 * definded in bild.h). The labeling of the pixels in the local
 * neighborhood of (r,c) is as follows:
 * 
 * - a: (r  , c-1)	- d: (r-1, c+1)  - g: (r-2, c-1)  - j: (r-1, c+2)  
 * - b: (r-1, c  )	- e: (r  , c-2)  - h: (r-2, c  )  
 * - c: (r-1, c-1)	- f: (r-1, c-2)  - i: (r-2, c+1)  
 * 	
 * The function also checks whether the pixels D, I and J are in 
 * limits and returns BORDERPIXEL if necessary.
 *-----------------------------------------------------------------*/
UINT getPixel( AUSWAHL *a, char val)
{
  /* pixel selection */
	switch (val)
	{
		case 'a': return getPixelByCoordinates( a, a->r    , a->c - 1);
		case 'b': return getPixelByCoordinates( a, a->r - 1, a->c);
		case 'c': return getPixelByCoordinates( a, a->r - 1, a->c - 1);
		case 'd': return getPixelByCoordinates( a, a->r - 1, a->c + 1);
		case 'e': return getPixelByCoordinates( a, a->r    , a->c - 2);
		case 'f': return getPixelByCoordinates( a, a->r - 1, a->c - 2);
		case 'g': return getPixelByCoordinates( a, a->r - 2, a->c - 1);
		case 'h': return getPixelByCoordinates( a, a->r - 2, a->c);
		case 'i': return getPixelByCoordinates( a, a->r - 2, a->c + 1);
		case 'j': return getPixelByCoordinates( a, a->r - 1, a->c + 2);
//		default:  return BORDERPIXEL;
		default:  return a->src->range >> 1;
	}
}

/*------------------------------------------------------------------
 * getGradient( AUSWAHL *a, char val)
 *
 * Returns a gradient of adjacent pixels in the local neighborhood
 * of the pixel at (r,c). The mapping of the gradients is as follows
 * (for pixel labeling see above):
 * 
 * - 0: A - C   - 2: B - D   - 4: B - H   - 6: C - F   - 8: D - J
 * - 1: C - B   - 3: A - E   - 5: C - G   - 7: D - I
 *-----------------------------------------------------------------*/
int getGradient( AUSWAHL *a, unsigned char val)
{
	int grad;

  /* invalid position (r,c) */
	if (a->r < 0 || a->c < 0 || 
      a->r >= a->src->height || a->c >= a->src->width) val = 10;

  switch (val)
  {
    case 0:
      grad = (int)getPixel( a, 'a') - (int)getPixel( a, 'c');
      break;
    case 1:
      grad = (int)getPixel( a, 'c') - (int)getPixel( a, 'b');
      break;
    case 2:
      grad = (int)getPixel( a, 'b') - (int)getPixel( a, 'd');
      break;
    case 3:
      grad = (int)getPixel( a, 'a') - (int)getPixel( a, 'e');
      break;
    case 4:
      grad = (int)getPixel( a, 'b') - (int)getPixel( a, 'h');
      break;
    case 5:
      grad = (int)getPixel( a, 'c') - (int)getPixel( a, 'g');
      break;
    case 6:
      grad = (int)getPixel( a, 'c') - (int)getPixel( a, 'f');
      break;
    case 7:
      grad = (int)getPixel( a, 'd') - (int)getPixel( a, 'i');
      break;
    case 8:
      grad = (int)getPixel( a, 'd') - (int)getPixel( a, 'j');
      break;
    default:
      // return BORDERPIXEL;
			return a->src->range >> 1;
  }
	return grad;
}

/*------------------------------------------------------------------
 * newAuswahl( ...)
 *
 * Constructor: Allocates and initializes an AUSWAHL structure.
 *-----------------------------------------------------------------*/
int newAuswahl( AUSWAHL **ptrA, BILD *src, BILD *tar, PARAMETER *p, 
               SEITENINFO **s)
{
  AUSWAHL *a;

  TRY( ECALLOC( *ptrA, 1, AUSWAHL, 90));
  a = *ptrA;

  a->r = a->c = 0;

  /* pointer on input image, output image, and side information */
  a->src = src;
  a->tar = tar;
  a->sInfo = s;
  a->db = NULL; /* context data */

  /* block module */
  a->blockMode = p->blockMode;
  if (p->blockMode == TRUE)
  {   
    TRY( newBlock( &(a->bk), tar, p));
  }
  else
    a->bk = NULL;

  /* context module */
  TRY( newKontext( &(a->cx), p->cxGrad, p->cxPx));

  return 0;
}

/*------------------------------------------------------------------
 * freeAuswahl( AUSWAHL *a)
 *
 * Destructor: Frees an AUSWAHL structure.
 *-----------------------------------------------------------------*/
void freeAuswahl( AUSWAHL *a)
{
  /* free nested structures which have been initialized */
  if (a->bk != NULL) freeBlock( a->bk);
  freeKontext( a->cx);
  FREEPTR( a);
}