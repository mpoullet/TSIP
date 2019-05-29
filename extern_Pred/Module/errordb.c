/*******************************************************************
 *
 * File....: errordb.c
 * Function: Functions for context data processing.
 * Author..: Thomas Schatton
 * Date....: 11/04/2010
 *
 ********************************************************************/

#include "errordb.h"

/*------------------------------------------------------------------
 * getPredRanking( UINT **ranking, ERRORDB *db, UINT cx)
 *
 * Uses the SAD values of the specified context to create a predictor
 * ranking. The function returns a list containing a list of all
 * predictors (by index) sorted by their SAD in ascending order.
 *-----------------------------------------------------------------*/
int getPredRanking( UINT **ranking, ERRORDB *db, UINT cx)
{
  UINT i, n, temp,
       *rank;
  BOOL swap;

  /* initialize */
  n = db->pCount;
  rank = *ranking;
  for ( i = 0; i < n; i++) rank[i] = i;

  /* sort */
  do
  {
    swap = FALSE;
    for ( i = 0; i < n - 1; i++)
    {
      if ( db->sse[rank[i]][cx] > db->sse[rank[i + 1]][cx])
      {
        temp        = rank[i];
        rank[i]     = rank[i + 1];
        rank[i + 1] = temp;

        swap = TRUE;
      }
    }
    n--;
  }
  while (swap && n > 1);

  return 0;
}

/*------------------------------------------------------------------
 * errorDbFitToContext( ERRORDB **ptrDb, KONTEXT *cx)
 *
 * Shrinks the context space of the context data to comply with the
 * context cx.
 *-----------------------------------------------------------------*/
int errorDbFitToContext( ERRORDB **ptrDb, KONTEXT *cx)
{
  ERRORDB *dbNew, *dbOld;
  UINT c, p, newCx, pxComp, gradSign, range;
  int gradComp, halfRange;

  dbOld = *ptrDb;

  if (dbOld->cx->cxGrad != cx->cxGrad && dbOld->cx->cxPx != cx->cxPx)
  {
    TRY( newErrorDB( &dbNew, dbOld->pCount, cx));
    
    range = (UINT)powl(7, dbNew->cx->cxGrad);
    halfRange = (int)(range / 2);

    for ( c = 0; c < dbOld->cx->anzKontexte; c++)
    {
      /* get pixel component */
      pxComp = c & ((UINT)powl( 2, dbOld->cx->cxPx) - 1);
      /* delete excessive pixels */
      pxComp &= (UINT)(powl( 2, dbNew->cx->cxPx) - 1);

      /* get gradient component */
      gradComp = c >> dbOld->cx->cxPx;
      /* get sign */
      gradSign = gradComp & 1u;
      gradComp >>= 1; /* delete sign from gradient component */
      if (gradSign == 1)
        gradComp = -gradComp;

      /* remove gradients */
      gradComp %= (UINT)powl(7, dbNew->cx->cxGrad);

      /* limit gradient component */
      if (gradComp < -halfRange)
        gradComp += range;
      else if (gradComp > halfRange)
        gradComp -= range;
      
      /* add sign */
      if (gradComp < 0)
      {
        gradSign = 1;
        gradComp = abs(gradComp);
      }
      else
        gradSign = 0;

      /* combine new context */
      newCx = ( (((UINT)gradComp << 1) | gradSign) << dbNew->cx->cxPx) | pxComp;

      /* merge contexts */
      dbNew->values[newCx] += dbOld->values[c];

      for ( p = 0; p < dbOld->pCount; p++)
        dbNew->sse[p][newCx] += dbOld->sse[p][c];

      /* prevent overflow */
      if (dbNew->values[newCx] >= dbNew->reset)
        for ( p = 0; p < dbNew->cx->anzKontexte; 
              dbNew->values[p++] >>= 1);
    }

    freeErrorDB( *ptrDb);
    *ptrDb = dbNew;
  }

  return 0;
}

/*------------------------------------------------------------------
 * loadErrorDBFromFile( ERRORDB **ptrDb, char *file)
 *
 * loads context data from a file to a context data structure.
 *-----------------------------------------------------------------*/
int loadErrorDBFromFile( ERRORDB **ptrDb, char *file)
{
  FILE *in = NULL;	  /* input file */
	char buf[BUFFER];   /* buffer */
  char strVal[12];
	UINT c, p;       /* context, predictor */
	UINT cxGrad, cxPx, pCnt, pxCnt;
  ERRORDB *db;
  KONTEXT *cx;

  /* open file */
	in = fopen( file, "r");
	if ( in == NULL) return 3;
	
	/* read first row (length of side information, channel count */
	do 
	{
		fgets(buf, BUFFER, in);
	}
	while ( buf[0] == '#' || buf[0] == '\n');
	sscanf( buf, "%u %u %u %u", &cxGrad, &cxPx, &pCnt, &pxCnt);
	
	/* create structure */
  TRY( newKontext( &cx, cxGrad, cxPx));
  TRY( newErrorDB( ptrDb, pCnt, cx));
  db = *ptrDb;

  db->processed = pxCnt;

  /* read table in file */
  for ( c = 0; c < db->cx->anzKontexte; c++)  /* all contexts */
  {
    fgets( strVal, 10, in);
    sscanf( strVal, "%u", &(db->values[c]));
    for ( p = 0; p < db->pCount; p++)
    {
      fgets( strVal, 11, in);
      sscanf( strVal, " %f", (&(db->sse[p][c])));
    }	
    fgets( strVal, 1, in); /* new line */
  }

  /* close file */
  fclose( in);

	return 0;
}

/*------------------------------------------------------------------
 * writeErrorDBToFile( ERRORDB *db, char *file)
 *
 * writes context data to a file.
 *-----------------------------------------------------------------*/
int writeErrorDBToFile( ERRORDB *db, char *file)
{
	FILE *out = NULL;                 /* output file */
	UINT c, p;                        /* context, predictor (index) */
       
  /* open file */
	out = fopen( file, "w");
	if ( out == NULL) return 3;

	/* write context parameters, number of predictors, and
   * total pixel count */	
  fprintf( out, "%u %u %u %u\n", 
    db->cx->cxGrad, db->cx->cxPx, db->pCount, db->processed);
	
  /* write table */
  for ( c = 0; c < db->cx->anzKontexte; c++)  /* all contexts */
  {
    fprintf( out, "%10u", db->values[c]);
    for ( p = 0; p < db->pCount; p++)  /* all predictors */
      fprintf( out, " %10u", (UINT)db->sse[p][c]);
    fprintf( out, "\n");
  }
  
  /* close file */
	fclose(out);

  return 0;
}

/*------------------------------------------------------------------
 * newErrorDB( ERRORDB **db, KONTEXT *cx)
 *
 * Constructor: Allocates and initializes a new context data 
 * structure.
 *-----------------------------------------------------------------*/
int newErrorDB( ERRORDB **ptrDb, UINT pCnt, KONTEXT *cx)
{
  UINT i;
  ERRORDB *db;

  TRY( ECALLOC( *ptrDb, 1, ERRORDB, 2));
  db = *ptrDb;

  db->processed = 0;
  db->reset = (UINT)powl(2, 24);
  db->pCount = pCnt;

  TRY( newKontext( &db->cx, cx->cxGrad, cx->cxPx));

  TRY( ECALLOC( db->values, db->cx->anzKontexte, ULONG, 2));
  TRY( ECALLOC( db->sse, db->pCount, float*, 2));
  for ( i = 0; i < db->pCount; i++)
  {
    TRY( ECALLOC( db->sse[i], db->cx->anzKontexte, float, 2));
  }

  return 0;
}

/*------------------------------------------------------------------
 * freeErrorDB( ERRORDB *db)
 *
 * Destructor: Frees a context data structure.
 *-----------------------------------------------------------------*/
void freeErrorDB( ERRORDB *db)
{
  UINT i;

  for ( i = 0; i < db->pCount; i++)
    FREEPTR( db->sse[i]);
  FREEPTR( db->sse);
  FREEPTR( db->values);

  FREEPTR( db);
}