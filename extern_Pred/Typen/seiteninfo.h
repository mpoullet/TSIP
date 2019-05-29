/*******************************************************************
 *
 * File....: seiteninfo.h
 * Function: Function prototyping and definition of the SEITENINFO
 *           (side information) structure.
 * Author..: Thomas Schatton
 * Date....: 11/02/2010
 *
 ********************************************************************/

#ifndef SEITENINFO_H_
#define SEITENINFO_H_

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "types.h"

#define BUFFER 512

/* side information structure */
typedef struct {
	UINT length;      /* length of the predictor array */
	BYTE *predictor;  /* list of selected predictors */
} SEITENINFO;

/* Constructor & Destructor */
int newSeiteninfo( SEITENINFO **sInfo, UINT length);
void freeSeiteninfo(SEITENINFO *sInfo);

/* load side information from a file */
int loadSeiteninfosFromFile( SEITENINFO ***sInfo, char *file);
/* write side information to a file */
int writeSeiteninfosToFile( SEITENINFO **sInfo, char *file, 
                             UINT channelCnt);

/* serialization */
int newSeiteninfoFromStream( SEITENINFO **s, UINT *in);
int serializeSeiteninfo( UINT **stream, SEITENINFO *s);

/* print side information on stdout */
void printSeiteninfo(SEITENINFO *sInfo);

/* entropy calculation */
float sInfoEntropie(SEITENINFO *s);

#endif