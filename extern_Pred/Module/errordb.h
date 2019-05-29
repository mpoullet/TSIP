/*******************************************************************
 *
 * File....: errordb.h
 * Function: Function prototyping and definition of the ERRORDB 
 *           (context data) structure.
 * Author..: Thomas Schatton
 * Date....: 11/04/2010
 *
 ********************************************************************/

#ifndef ERRORDB_H_
#define ERRORDB_H_

#include <math.h>
#include <string.h>
#include "..\Typen\types.h"
#include "context.h"

#define BUFFER 512

#define DB_CXPX 5
#define DB_CXGRAD 5

/* context data structure */
typedef struct {
	float **sse;      /* SSE per predictor and context */
  ULONG *values;    /* number of events per context */   
  UINT  reset,      /* specifies when to halve the values in **sad */
        processed;  /* total count of processed pixels */
  KONTEXT *cx;
  UINT  pCount;     /* number of predictors (columns in **sad) */
} ERRORDB;

/* Construktor & Destructor */
int newErrorDB( ERRORDB **db, UINT pCnt, KONTEXT *cx);
void freeErrorDB( ERRORDB *db);

/* load context data from a file */
int loadErrorDBFromFile( ERRORDB **db, char *file);
/* save context data to a file */
int writeErrorDBToFile( ERRORDB *db, char *file);

/* compute the predictor ranking for the context cx */
int getPredRanking( UINT **ranking, ERRORDB *db, UINT cx);

/* shrink context space of the context data */
int errorDbFitToContext( ERRORDB **ptrDb, KONTEXT *cx);

#endif