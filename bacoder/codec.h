/*****************************************************
 * Datei: codec.h
 * Desc.: Header für Codec
 * Autor: Thomas Schmiedel
 * Datum: 12.11.2010, 07.01.2011, 02.05.2011
 * 27.05.2011 Strutz
 * 03.06.2011 Strutz: new parameters for rYUV
 * 23.01.2012 PicHeader:separate reset_val and maxCount
 * 03.02.2012 new:skip_precoding in PicHeader structures
 * 24.07.2012 new:palette_sorting in PicHeader structures
 * 08.11.2013 BYTES_STRUC
 * 12.03.2014 PredictionType
 * 01.07.2014 new: prototyping for tiles management
 * 01.07.2014 new: header structure for tiles management
 * 01.07.2014 BYTES_STRUC extended
 * 03.05.2016 new: #include "memoryUsage.h"
 ******************************************************/

#ifndef _CODEC_H_
#define _CODEC_H_

#include "tstypes.h"
#include "bitbuf.h"
#include "ginterface.h"
#include "stats.h" // Strutz 
#include "autoConfig.h" // Strutz 
#include "imageFeatures.h" // Strutz 
#include "H26x_Prediction.h"
#include "memoryUsage.h"


#undef RCT_VARIANCE_CRITERION  /* use standard entropy criterion instead	*/

#define 	BITDEPTH(a, b)\
	b = 0;\
	while(a){b++; a >>= 1;}

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

typedef struct
{
	unsigned int num_globheader_bytes;
	unsigned int num_header_bytes;
	unsigned int num_extPred_bytes;
	unsigned int num_H26xPred_bytes;
	unsigned int num_palette_bytes;
	unsigned int num_histMode_bytes;
	unsigned int num_quad_bytes;
	unsigned long num_data_bytes;
	unsigned long num_alpha_bytes;
} BYTES_STRUC;



/* Bildstruktur codieren und laden */
void EncodePic( PicData *pd, 
			 FILE *h_file,
			 void *extparam,
			 AutoConfig *aC,
			 ImageFeatures *iF,
			 INTERMEDIATE_VALS *iv,
			 BYTES_STRUC *bytes);



PicData *DecodePic( FILE *h_file);


extern  char* const thisVersion; /* in bacoder.cpp	*/

#endif

