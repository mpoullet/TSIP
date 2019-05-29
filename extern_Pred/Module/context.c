/*******************************************************************
 *
 * File....: context.c
 * Function: Constructor and Destructor for the context structure.
 * Author..: Thomas Schatton
 * Date....: 11/04/2010
 *
 ********************************************************************/

#include "context.h"
#define ORIG_CX

/*------------------------------------------------------------------
 * *newContext( KONTEXT **ptrC, UINT cxGrad, UINT cxPx)
 *
 * Constructor: Allocates and initializes a new context structure.
 *-----------------------------------------------------------------*/
int newKontext( KONTEXT **ptrC, UINT cxGrad, UINT cxPx)
{
  KONTEXT *c;

  TRY( ECALLOC( *ptrC, 1, KONTEXT, 60));
  c = *ptrC;

  /* initialization */
  c->cxGrad = cxGrad;
  c->cxPx = cxPx;

  /* check for size of context space */
#ifdef ORIG_CX
  c->anzKontexte = 
		(UINT)(powf( 2.0, (float)cxPx) * (powf( 7.0, (float)cxGrad) + 1.0));
#else
	c->anzKontexte = 
		(UINT)(powf( 2.0, (float)cxPx) * (powf( 9.0, (float)cxGrad) + 1.0));
#endif

  /* prevent allocation failures with more than 2^20 contexts */
  if (c->anzKontexte > powl(2, 20)) return 61; /* too many contexts */
  
  return 0;
}

/*------------------------------------------------------------------
 * freeContext( CONTEXT *c)
 *
 * Destructor: Frees a context structure.
 *-----------------------------------------------------------------*/
void freeKontext( KONTEXT *c)
{
  /* free structure */
  FREEPTR( c);
}

