/*****************************************************
 * File..: H26x_Prediction.h
 * Desc..: prototypes the main functions
 * Author: Michael Stegemann
 * Date..: 12.11.2012
 ******************************************************/

#ifndef H26x_H
#define H26x_H

#include "ginterface.h"
#include "tstypes.h"
#include "stdafx.h"
#include "H26x_Logging.h"
#include "H26x_Config.h"

#include "iwt.h"

#include <math.h>
#include <string.h>


typedef struct H26xSettings
{
    H26xConfiguration* config;
    H26xLog			 * log;
    uint usedPredictors[NUM_INTRA_MODE],
         usedBlocks[NUM_BLOCK_SIZES],
		 usedTransformations;
    unsigned long      
		 signalingBytes;
    uint newBitdepth[3];
	int 
	 offset,
	 upperRange,
	 lowerRange;

} H26xSettings;

typedef struct ReferenceSamples
{
    uint * Samples, /* all samples, sized 4*bSize+1 */
         * TopLeft, /* upper left samples */
         * Top, /* the samples directly above current block */
         * TopRight, /* extended top samples to the right */
         * Left,/* the samples directly left of the current block */
         * LeftBottom; /* extended left samples downwards */
} ReferenceSamples;

void predictPicture( PicData* pd, cbool encode, H26xSettings* settings );

//void predictBlock ( PicData* pd, PicData* errorPd, uint chan, uint width, uint height,
//									 uint location, H26xSettings* settings );
void predictBlock( PicData* pd, PicData* errorPd, PicData* modePd, uint chan, uint width, uint height,
									 uint i, uint j, H26xSettings* settings );
void predictBlockChroma( PicData* pd, PicData* errorPd, uint chan, 
											uint width, uint height, uint location, 
											H26xSettings* settings, uint predMode, uint postTransform );

void inverseBlock( PicData* pd, PicData* errorPd, uint chan, 
									 uint bwidth, uint bheight, uint loc_y, uint loc_x, H26xSettings* settings );
void inverseBlockChroma( PicData* pd, PicData* errorPd, uint chan, 
												 uint width, uint height, uint location, 
												 H26xSettings* settings, uint predMode, uint postTransform );


ReferenceSamples* checkAvailablity ( PicData* pd, PicData* errorPd, int chan, 
													int bwidth, int location, 
													int loc_y, int loc_x,
													uint BitDepth );
uint replaceSample( PicData* pd, PicData* errorPd, int chan, int bSize, 
									 int location, cbool, cbool, uint );

void predictLuma( ReferenceSamples* ref,
                   uint* errorBlock,
                   uint nMode,
                   uint width,
                   uint bSize ,
                   uint maxVal /* added for the Clip Function*/
                 );

void predictDC(
    ReferenceSamples* ref,
    uint*	 errorBlock,
    uint	 width,
    uint	 bSize,
    cbool	 isLuma
);
int getDCVal ( ReferenceSamples* ref,
               uint bSize );

void predictPlanar (
    ReferenceSamples* ref,
    uint*	 errorBlock,
    uint	 width,
    uint	 bSize
);

void predictAng (
    ReferenceSamples* ref,
    uint*	 errorBlock,
    uint	 width,
    int		 bSize,
    uint	 nMode,
    uint	 maxVal,
    cbool	 isLuma
);


double MAD ( int* errors, int stride, int width, int height );
double MSE ( int* errors, int stride, int width, int height );

unsigned int log2 ( unsigned int x );
int Clip3 ( int minVal, int maxVal, int a );

void WriteErrorPPM(PicData *pd, char *filename, uint newBitDepth[3]);
void WriteModePPM(PicData *pd, char* filename, H26xSettings* settings);
void ModeWriter(PicData *modePd, uint chan, H26xSettings* settings, uint width, uint height, uint location, uint loc_x, uint loc_y);

void getH26xDefaultConfig(H26xConfiguration* config );


#endif