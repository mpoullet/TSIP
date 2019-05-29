/*****************************************************
 * File..: H26x_Logging.h
 * Desc..: prototypes the logging functions
 * Author: Michael Stegemann
 * Date..: 12.11.2012
 ******************************************************/

#ifndef H26x_Logg_H
#define H26x_Logg_H

#include "tstypes.h"

#include "bitbuf.h"
#include "fibocode.h"

/* forward declarations */
struct H26xSettings;
struct H26xConfiguration;

typedef struct H26xBlock
{
    uint	bSize, /* Loggs the blocksize */
            nMode; /* the used prediction mode */
    double  bScore; /* either MAD or MSE */
		uint loc_y, loc_x; /* top-left corner of block	*/
    cbool	usedTransformation;
    H26xBlock *next; /* points to the next element */

} H26xBlock;

typedef struct H26xLog
{

    H26xBlock *firstNode,
              *lastNode;

} H26xLog;

int getBlockSizeForIdx ( int Idx );
int getIdxForBlockSize ( int bSize );
int getLargestBlockSize ( H26xConfiguration* config );
int getNextSmallerBlockSize ( H26xConfiguration* config, int bSize );

H26xLog* beginLog ( H26xSettings* thisLog );
void addBlock( H26xLog* thisLog, uint bSize, double bScore, uint predictor,
							 uint transformation, uint loc_y, uint loc_x);
H26xBlock* getBlock ( H26xLog* thisLog );
void evaluateLog ( H26xSettings* thisLog, BitBuffer *bitbuf, unsigned long bytesBefore);
void reconstructLog ( H26xSettings* thisLog, BitBuffer *bitbuf );

#endif

