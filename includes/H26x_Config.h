/*****************************************************
 * File..: H26x_Config.h
 * Desc..: contains the configuration structures and defines
 * Author: Michael Stegemann
 * Date..: 12.11.2012
 ******************************************************/


#ifndef H26x_Conf_H
#define H26x_Conf_H

#include "tstypes.h"

/* Determines the evaluation method*/
#define H26xMAD 0
#define H26xMSE 1

/* Determines if the block splitting and prediction selection is only executed for the first channel/ Y*/
#define LumaOnly
#undef LumaOnly

/* These constants should remain fixed */
#define MAX_BLOCKSIZE 16
#define MIN_BLOCKSIZE 4
#define NUM_BLOCK_SIZES 3

#define NUM_INTRA_MODE                      (35)
#define PLANAR	                            ( 0)
#define VER	                                (26)    /* index for intra VERTICAL   mode */
#define HOR					                (10)    /* index for intra HORIZONTAL mode */
#define DC	                                ( 1)
#define MAX_MAD								( 1 << 30 )
#define MODE_INIT  (NUM_INTRA_MODE + 64) /* 34+64 is maximum value in modePd	*/


#define Clip(x,max) Clip3( 0, (max), (x))

/* Table 8-5 – Specification of intraPredAngle */
const int PredAngle[NUM_INTRA_MODE] =
{
    0,   0, /* Invalid */
	/* 2   3   4   5   6   7   8   9  */
    32, 26, 21, 17, 13,  9,  5,  2,
	/*10 11  12  13   14   15   16   17  */
    0, -2, -5, -9, -13, -17, -21, -26,
	/* 18   19   20   21   22  23  24  25  */
    -32, -26, -21, -17, -13, -9, -5, -2,
	/*26   27 28  29  30  31  32  33  34  */
     0,  2,  5,  9, 13, 17, 21, 26, 32
};

/* Table 8-6 – Specification of invAngle */
const int InvPredAngle[NUM_INTRA_MODE] =
{
    0,    0, /* Invalid */
    256,  315,  390, 482, 630, 910, 1638, 4096,
    0, 4096, 1638, 910, 630, 482,  390,  315,
    256,  315,  390, 482, 630, 910, 1638, 4096,
    0, 4096, 1638, 910, 630, 482,  390,  315,
    256
};

/* Contains all user settings for the H26x Prediction */
typedef struct H26xConfiguration
{
    cbool
     availablePredictors[NUM_INTRA_MODE],
     availableBlocks[NUM_BLOCK_SIZES],
     logPredictors,
     logBlocks,
	 logTransformations,
     printErrPic;
    uint
     postTransform,
     localPrediction,
	 largestBlockSize;
	double mse_thresh;

    friend bool operator== ( H26xConfiguration c1, H26xConfiguration c2 );
    friend bool operator!= ( H26xConfiguration c1, H26xConfiguration c2 );

} H26xConfiguration;

int getBlockSizeForIdx ( int Idx );
int getLargestBlockSize ( H26xConfiguration* config );

void writeH26xSettings ( H26xConfiguration* config, BitBuffer *bitbuf );
void readH26xSettings ( H26xConfiguration* config, BitBuffer *bitbuf );

#endif