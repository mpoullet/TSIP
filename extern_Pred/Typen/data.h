/*******************************************************************
 *
 * File....: data.h
 * Function: Function prototyping and definition of the DATA 
 *           structure.
 * Author..: Thomas Schatton
 * Date....: 11/02/2010
 *
 ********************************************************************/

#ifndef _DATA_H_
#define _DATA_H_

/* color spaces */
#define RGB    0
#define YCgCo  1
#define YrUrVr 2

#include <string.h>
#include "types.h"
#include "bild.h"
#include "seiteninfo.h"
#include "ginterface.h" /* PGM/PPM Loader/Writer */

/* data structure */
typedef struct {
	BILD		    **img;        /* array of image structures */
  BILD        **psImg;      /* array of predictor selection images */
	SEITENINFO	**sInfo;      /* array of side information structures */
	UINT		    channelCnt;   /* amount of color channels */
	char		    *sInfoFile;   /* path to the side information file */
  char        *psImgFile;   /* path to predictor selection image */
  int         farbraum;     /* current color space */
} DATA;

/* Constructor & Destructor */
int newData( DATA **d, UINT channelCnt, char *sInfoFile, int farbraum,
             char *psImageFile);
void freeData( DATA *data);

/* PicData conversion */
int dataToPicData( PicData **pd, DATA *d);
int picDataToData( DATA **d, PicData *pd);;

/* color space transformation */
//int dataTransform(DATA *d, int toFarbraum);

/* load image data from a PPM or PGM file */
int loadData( DATA **d, char *ppmFile, char *sInfoFile);
/* save image data */
int writeData( DATA *data, char *file); 

#endif