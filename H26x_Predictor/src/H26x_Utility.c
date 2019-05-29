/*****************************************************
* File..: H26x_Utility.c
* Desc..: implements assisting functions
* Author: Michael Stegemann
* Date..: 12.11.2012
******************************************************/

#include "H26x_Logging.h"
#include "H26x_Prediction.h"
#include "H26x_Config.h"

#include <assert.h>

extern unsigned long numBytes;
extern char* const thisVersion; /* bacoder.cpp	*/

/*------------------------------------------------------------------
* getBlockSizeForIdx(int Idx)
*
* Returns the blocksize for the provided index
*-----------------------------------------------------------------*/
int getBlockSizeForIdx ( int Idx )
{
	switch ( Idx )
	{
	case 2:
		return 16;
		break;
	case 1:
		return 8;
		break;
	case 0:
		return 4;
		break;
	}
	return 0;
}

/*------------------------------------------------------------------
* getIdxForBlockSize(int bSize)
*
* Returns the index for the provided blocksize
*-----------------------------------------------------------------*/
int getIdxForBlockSize ( int bSize )
{
	switch ( bSize )
	{
	case 16:
		return 2;
		break;
	case 8:
		return 1;
		break;
	case 4:
		return 0;
		break;
	}
	return 0;
}

/*------------------------------------------------------------------
* getLargestBlockSize(H26xConfiguration* config)
*
* Returns the largest blocksize, allowed in the current configuration
*-----------------------------------------------------------------*/
int getLargestBlockSize ( H26xConfiguration* config )
{

	int i;
	for ( i = NUM_BLOCK_SIZES - 1; i >= 0; i-- )
	{
		if ( config->availableBlocks[i] == TRUE )
			return getBlockSizeForIdx ( i );
	}
	return 0;
}

/*------------------------------------------------------------------
* getNextSmallerBlockSize(H26xConfiguration* config, int bSize)
*
* Returns the next smaller Blocksize
*-----------------------------------------------------------------*/
int getNextSmallerBlockSize ( H26xConfiguration* config, int bSize )
{

	int i;
	for ( i = getIdxForBlockSize ( bSize ) - 1; i >= 0; i-- )
	{
		if ( config->availableBlocks[i] == TRUE )
			return getBlockSizeForIdx ( i );
	}
	return 0;
}


/*------------------------------------------------------------------
* H26xLog* beginLog(H26xSettings* H26x_Set)
*
* Initialises the H26x_Log Structure for usage and returns a pointer
* to the list
*-----------------------------------------------------------------*/
H26xLog* beginLog ( H26xSettings* H26x_Set )
{

	H26xLog* thisLog;
	ALLOC ( thisLog, 1u, H26xLog )
		thisLog->firstNode = NULL;
	thisLog->lastNode = NULL;

	for ( int i = 0; i < NUM_INTRA_MODE; i++ )
	{
		H26x_Set->usedPredictors[i] = 0;
	}
	for ( int i = 0; i < NUM_BLOCK_SIZES; i++ )
	{
		H26x_Set->usedBlocks[i] = 0;
	}

	H26x_Set->usedTransformations = 0;

	return  thisLog;
}


/*------------------------------------------------------------------
* addBlock(H26xLog* thisLog, uint bSize, double bScore, uint predictor, uint transformation)
*
* Adds a new block to the H26x_Log structe
*-----------------------------------------------------------------*/
void addBlock( H26xLog* thisLog, uint bSize, double bScore, uint predictor, 
							uint transformation, uint loc_y, uint loc_x)
{

	if ( thisLog->firstNode == NULL )
	{
		ALLOC ( thisLog->firstNode, 1u, H26xBlock );
		thisLog->firstNode->bScore = bScore;
		thisLog->firstNode->bSize  = bSize;
		thisLog->firstNode->nMode  = predictor;
		thisLog->firstNode->usedTransformation = (cbool)transformation;
		thisLog->firstNode->loc_y = loc_y;
		thisLog->firstNode->loc_x = loc_x;
		thisLog->firstNode->next = NULL;
		thisLog->lastNode = thisLog->firstNode;
	}
	else
	{
		ALLOC ( thisLog->lastNode->next, 1u, H26xBlock );
		thisLog->lastNode->next = thisLog->lastNode->next;
		thisLog->lastNode = thisLog->lastNode->next;
		thisLog->lastNode->bScore = bScore;
		thisLog->lastNode->bSize  = bSize;
		thisLog->lastNode->nMode  = predictor;
		thisLog->lastNode->usedTransformation = (cbool)transformation;
		thisLog->firstNode->loc_y = loc_y;
		thisLog->firstNode->loc_x = loc_x;
		thisLog->lastNode->next = NULL;
	}

}

/*------------------------------------------------------------------
* H26xBlock* getBlock(H26xLog* thisLog)
*
* Extracts the first Block in the H26x_Log
*-----------------------------------------------------------------*/
H26xBlock* getBlock( H26xLog* thisLog )
{

	/*  
	*	When using this function the calling function need to
	*  free the returned block since the pointer orphans.
	*/
	H26xBlock* firstBlock;

	if ( thisLog->firstNode == NULL )
	{
		thisLog->firstNode = thisLog->firstNode;
	}

	firstBlock = thisLog->firstNode;

	thisLog->firstNode = thisLog->firstNode->next;

	return firstBlock;
}

/*------------------------------------------------------------------
* evaluateLog(H26xSettings* thisLog, BitBuffer *bitbuf)
*
* Evaluates the log and writes the contents to the bitbuf stream
*-----------------------------------------------------------------*/
void evaluateLog( H26xSettings* thisLog, BitBuffer *bitbuf, unsigned long bytesBefore )
{
	int blockIdx;
	int i, l, index; /* Strutz	*/
	unsigned char lut[NUM_INTRA_MODE]; /* Strutz	*/
	unsigned char cval; /* Strutz	*/

	/* init LUT	*/
	for ( i = 0; i < NUM_INTRA_MODE; i++) lut[i] = (unsigned char)i;

	/* get the first block */
	H26xBlock* currentBlock = thisLog->log->firstNode;
	H26xBlock* freeBlock = thisLog->log->firstNode;

	/* and loop until the end is reached... */
	while ( currentBlock != NULL )
	{
		/* evaluate stuff */

		thisLog->usedPredictors[currentBlock->nMode]++;
		if (currentBlock->usedTransformation) {thisLog->usedTransformations++;}
		blockIdx = getIdxForBlockSize ( currentBlock->bSize );
		thisLog->usedBlocks[blockIdx]++;
		/*  no use yet // currentBlock->bScore */

		/* compose log */
		// Strutz thisLog->signalingBits += WriteFiboToStream ( bitbuf, currentBlock->bSize );
		/* block size to index conversion	*/
		rice_encoding( bitbuf, getIdxForBlockSize(currentBlock->bSize), 0);

		// Strutz thisLog->signalingBits += WriteFiboToStream ( bitbuf, currentBlock->nMode );
		/* move to front MTF */
		cval = (unsigned char)currentBlock->nMode;
		assert( cval < NUM_INTRA_MODE);

		/* Welchen Index hat dieser in der Liste? */
		for (index = 0; lut[index] != cval; index++);

		rice_encoding( bitbuf, index, 3);

		/* MTF, Aktuellen Wert holen, Werte nach oben verschieben */	
		for (l = index; l > 0; l--)
		{
			lut[l] = lut[l - 1u];
		}
		/* Den aktuellen Wert an die
		freigewordene Stelle schreiben */
		lut[0] = cval;		

		if ( thisLog->config->postTransform > 0 )
		{
			// Strutz  thisLog->signalingBits += WriteFiboToStream ( bitbuf, currentBlock->usedTransformation );
			assert( currentBlock->usedTransformation < 2);
			BufferPutBit( bitbuf, currentBlock->usedTransformation);
		}

		/* clean on the run */
		freeBlock = currentBlock;
		/* update the pointer */
		currentBlock = currentBlock->next;

		FREE ( freeBlock );
	}

	// Strutz WriteFiboToStream ( bitbuf, 255 );
	rice_encoding( bitbuf, 3, 0);

	thisLog->signalingBytes = numBytes - bytesBefore;

	FREE ( thisLog->log );
}

/*------------------------------------------------------------------
* evaluateLog(H26xSettings* thisLog, BitBuffer *bitbuf)
*
* Evaluates the log and writes the contents to the bitbuf stream
*-----------------------------------------------------------------*/
void evaluateLog_( H26xSettings* thisLog, BitBuffer *bitbuf, unsigned long bytesBefore )
{
	int blockIdx;

	/* get the first block */
	H26xBlock* currentBlock = thisLog->log->firstNode;
	H26xBlock* freeBlock = thisLog->log->firstNode;

	/* and loop until the end is reached... */
	while ( currentBlock != NULL )
	{
		/* evaluate stuff */

		thisLog->usedPredictors[currentBlock->nMode]++;
		blockIdx = getIdxForBlockSize ( currentBlock->bSize );
		thisLog->usedBlocks[blockIdx]++;
		/*  no use yet // currentBlock->bScore */

		/* compose log */
		WriteFiboToStream ( bitbuf, currentBlock->bSize );
		WriteFiboToStream ( bitbuf, currentBlock->nMode );
		if ( thisLog->config->postTransform > 0 )
		{
			WriteFiboToStream ( bitbuf, currentBlock->usedTransformation );
		}

		/* clean on the run */
		freeBlock = currentBlock;
		/* update the pointer */
		currentBlock = currentBlock->next;

		FREE ( freeBlock );
	}

	WriteFiboToStream ( bitbuf, 255 );

	thisLog->signalingBytes = numBytes - bytesBefore;

	FREE ( thisLog->log );
}


/*------------------------------------------------------------------
* reconstructLog(H26xSettings* thisLog, BitBuffer *bitbuf)
*
* reconstructs the log from the bitbuf stream
*-----------------------------------------------------------------*/
void reconstructLog( H26xSettings* thisLog, BitBuffer *bitbuf)
{
	uint bsize = 4;
	uint predictor;
	uint transformation = 0;
	unsigned long val;
	int i, k; /* Strutz	*/
	unsigned char lut[NUM_INTRA_MODE]; /* Strutz	*/
	unsigned char cval;
//	unsigned int loc_y=0, loc_x=0;

	/* init LUT	*/
	for ( i = 0; i < NUM_INTRA_MODE; i++) lut[i] = (unsigned char)i;

	thisLog->log = beginLog ( thisLog );

	// Strutz while ( ( val = GetFiboFromStream( bitbuf) ) != 255 )
	while ( ( val = rice_decoding( bitbuf, 0)) != 3 )
	{
		/* index to block size conversion	*/
		bsize = getBlockSizeForIdx( val);

		// Strutz predictor = GetFiboFromStream( bitbuf);
		cval = (unsigned char)rice_decoding( bitbuf, 3);
		assert( cval < NUM_INTRA_MODE);

		predictor = lut[cval];
		/* Aktuellen Wert holen, Werte nach oben verschieben */	
		for (k = cval; k > 0; k--)
		{
			lut[k] = lut[k - 1u];
		}
		/* Den aktuellen Wert an die freigewordene Stelle schreiben */
		lut[0] = (unsigned char)predictor;		

		if ( thisLog->config->postTransform > 0 )
		{
			// transformation = (cbool)GetFiboFromStream( bitbuf);
			transformation= BufferGetBit( bitbuf);
		}
		// Strutz addBlock ( thisLog->log, val, 0, predictor, transformation );
		addBlock( thisLog->log, bsize, 0, predictor, transformation,
			0, 0); /* loc_y, loc_x not known yet	*/
	}
}

/*------------------------------------------------------------------
* reconstructLog(H26xSettings* thisLog, BitBuffer *bitbuf)
*
* reconstructs the log from the bitbuf stream
*-----------------------------------------------------------------*/
void reconstructLog_( H26xSettings* thisLog, BitBuffer *bitbuf )
{
	uint val;
	uint predictor;
	uint transformation = 0;

	thisLog->log = beginLog ( thisLog );

	while ( ( val = GetFiboFromStream( bitbuf) ) != 255 )
	{
		predictor = GetFiboFromStream( bitbuf);
		if ( thisLog->config->postTransform > 0 )
		{
			transformation = (cbool)GetFiboFromStream( bitbuf);
		}
		addBlock ( thisLog->log, val, 0, predictor, transformation, 0, 0 );
	}
}

/*------------------------------------------------------------------
* writeH26xSettings(H26xConfiguration* config, BitBuffer *bitbuf)
*
* writes the H26x Settings to the bitbuf stream
*-----------------------------------------------------------------*/
void writeH26xSettings( H26xConfiguration* config, BitBuffer *bitbuf )
{
	int i;

	WriteFiboToStream ( bitbuf, (uint)NUM_BLOCK_SIZES );
	for ( i = 0; i < NUM_BLOCK_SIZES; i++ )
	{
		WriteFiboToStream ( bitbuf, (uint)config->availableBlocks[i] );
	}

	WriteFiboToStream ( bitbuf, (uint)NUM_INTRA_MODE );
	for ( i = 0; i < NUM_INTRA_MODE; i++ )
	{
		WriteFiboToStream ( bitbuf, (uint)config->availablePredictors[i] );
	}

	WriteFiboToStream ( bitbuf, (uint)config->postTransform );
	WriteFiboToStream ( bitbuf, (uint)config->localPrediction );
}

/*------------------------------------------------------------------
* readH26xSettings (H26xConfiguration* config, BitBuffer *bitbuf)
*
* reads the H26x settings from the bitbuf stream
*-----------------------------------------------------------------*/
void readH26xSettings( H26xConfiguration* config, BitBuffer *bitbuf )
{
	uint i;
	uint blocks;
	uint predictors;

	blocks = GetFiboFromStream( bitbuf);
	for ( i = 0; i < blocks; i++ )
	{
		config->availableBlocks[i] = (cbool) GetFiboFromStream( bitbuf);
	}

	predictors = GetFiboFromStream( bitbuf);
	for ( i = 0; i < (int)predictors; i++ )
	{
		config->availablePredictors[i] = (cbool)GetFiboFromStream( bitbuf);
	}

	config->postTransform = GetFiboFromStream( bitbuf);
	config->localPrediction = GetFiboFromStream( bitbuf);
}


/*--------------------------------------------------------
* ==()
* Overloads the compare operator of two Configuration structs
*-------------------------------------------------------*/
bool operator== ( H26xConfiguration c1, H26xConfiguration c2 )
{
	bool equal = true;

	for ( int i = 0; i < NUM_INTRA_MODE; i++ )
	{
		if ( c1.availablePredictors[i] != c2.availablePredictors[i] )
		{
			equal = false;
			break;
		}
	}

	for ( int i = 0; i < NUM_BLOCK_SIZES; i++ )
	{
		if ( c1.availableBlocks[i] != c2.availableBlocks[i] )
		{
			equal = false;
			break;
		}
	}
	return (
		equal &&
		c1.postTransform == c2.postTransform &&
		c1.localPrediction == c2.localPrediction

		);
}
/*--------------------------------------------------------
* !=()
* Overloads the compare operator of two Configuration structs
*-------------------------------------------------------*/
bool operator!= ( H26xConfiguration c1, H26xConfiguration c2 )
{
	return ! ( c1 == c2 );
}

/*------------------------------------------------------------------
* unsigned int log2( unsigned int x )
*
* turns the result of the log2.
* Warning: Only works for numbers that are power of 2
*-----------------------------------------------------------------*/
unsigned int log2 ( unsigned int x )
{
	unsigned int ans = 0 ;
	while ( x >>= 1 ) ans++;
	return ans;
}

/*------------------------------------------------------------------
* int Clip3 ( int minVal, int maxVal, int a )
*
* limits the given value to the range: minVal <= a <= maxVal
*-----------------------------------------------------------------*/
int Clip3 ( int minVal, int maxVal, int a )
{
	if ( a < minVal )
		a = minVal;
	else if ( a > maxVal )
		a = maxVal;
	return a;
}

/*------------------------------------------------------------------
 * void WriteErrorPPM(PicData *pd, char *filename)
 *
 * a customized version of the normal PPM Writer that produces
 * easier to understand error pictures
 *-----------------------------------------------------------------*/
void WriteErrorPPM(PicData *pd, char *filename, uint newBitDepth[3]) {

	FILE *outfile = NULL;
	uint i, k;
	int ival; // Strutz

	/* Datei im Schreibmodus öffnen */
	outfile = fopen(filename, "wb");
	if (outfile == NULL)
	{
		fprintf(stderr, "failed to open file: %s\n", filename);
		ERRCODE = PPM_OPEN_FAIL;
		return;
	}

	/* Enthält das Bild nur einen Kanal,
	so wird es als PGM gespeichert */
	if (pd->channel == 1u)
	{
		fprintf(outfile, "P5\n");
	}
	else
	{
		fprintf(outfile, "P6\n");
	}

	/* Header schreiben */
	fprintf(outfile, "# Created by TSIPcoder %s, HfTL\n", thisVersion);
	fprintf(outfile, "%u %u\n", pd->width, pd->height);
	// fprintf(outfile, "%u\n", (1u << max(newBitDepth[0],newBitDepth[1]))- 1u);
	fprintf(outfile, "%u\n", 255);

	/* Farbwerte speichern */	
	for (i = 0; i < pd->size; i++)
	{
		for (k = 0; k < pd->channel; k++)
		{
			// Strutz val = abs( ( int )( pd->data[k][i] - ( ( 1 << newBitDepth[k] ) / 2 )));	
			/* centring at 128 */
			ival = 128 + (int)pd->data[k][i] - ( (1 << newBitDepth[k]) / 2 );	
			/* limitation to the range of 0...255 */
			ival = max(0, min( 255, ival));
			fputc( ival, outfile);		

#ifdef NIXX
			if (max(newBitDepth[0],newBitDepth[1]) <= 8u) /* Nur ein Byte pro Kanal verwenden */
			{
				fputc(val, outfile);		
			}
			else /* 2 Byte pro Kanal, 16 Bit separat speichern */
			{
				fputc((val & 0xFF00u) >> 8u, outfile);
				fputc( val & 0x00FFu, outfile);	
			}
#endif
		}	
	}		

	fclose(outfile);
}

/*------------------------------------------------------------------
 * void WriteModePPM( )
 *
 *-----------------------------------------------------------------*/
void WriteModePPM( PicData *pd, H26xSettings* settings)
{
	PicData * modePd;

	modePd = CreatePicData( pd->width, pd->height, 3, 8, pd->bitperchan);

	uint wBlocks = (int)ceil( (double)pd->width  / settings->config->largestBlockSize );
	uint hBlocks = (int)ceil( (double)pd->height / settings->config->largestBlockSize );

	H26xBlock* firstBlockTmp = settings->log->firstNode;

	uint chan = 0;
	uint i; 
	uint j;

#ifndef LumaOnly
	for ( chan = 0; chan < pd->channel; chan ++ )
	{
#endif
		for ( i = 0; i < hBlocks; i++ )
		{
			for ( j = 0; j < wBlocks; j++ )
			{
				ModeWriter( 
					modePd, 
					chan,
					settings,  
					settings->config->largestBlockSize, 
					settings->config->largestBlockSize, 
					( j * settings->config->largestBlockSize + i * settings->config->largestBlockSize * pd->width ),
					j * settings->config->largestBlockSize,
					i * settings->config->largestBlockSize * pd->width
				);
			}
		}
#ifndef LumaOnly
	}
#endif

	settings->log->firstNode = firstBlockTmp;
	//WritePPM( modePd, filename);
	DeletePicData( modePd);
}

/*------------------------------------------------------------------
 * ModeWriter()
 *------------------------------------------------------------------*/
void ModeWriter( PicData *modePd, uint chan, H26xSettings* settings, uint width, 
								uint height, uint location, uint loc_x, uint loc_y)
{
	uint splitWidth;
	uint splitHeight;
	uint i;
	uint j;
	//	uint subChan;
	//    uint* workingBlock;
	//    int* errorBlock;
	//    ReferenceSamples* ref;
	H26xBlock* thisBlock;

	uint s_width = 0;
	uint s_height = 0;
	cbool isBorderBlock = FALSE;

	if ( settings->log->firstNode->bSize == width )
	{
		thisBlock = getBlock ( settings->log );

		/*  Align help sizes in case this is a bordering block */
		if ( ( location + width - 1 ) / modePd->width != location / modePd->width
			||
			( location + ( height - 1 ) * modePd->width ) >= modePd->size)
		{
			s_width  = ( location % modePd->width ) + width <= modePd->width ? width  : modePd->width  % width;
			s_height = floor ( (double)location / modePd->width ) + height <= modePd->height ? height : modePd->height % height;

			isBorderBlock = TRUE;
		}

		if ( isBorderBlock == TRUE )
		{

			for ( i = 0; i < s_height; i++ )
			{
				for ( j = 0; j < s_width; j++ )
				{
					modePd->data[chan][location + j + ( i * modePd->width )] = thisBlock->nMode * 7;
				}
			}
		}
		else
		{
			for ( i = 0; i < height; i++ )
			{
				for ( j = 0; j < width; j++ )
				{
					modePd->data[chan][location + j + ( i * modePd->width )] = thisBlock->nMode * 7;
				}
			}
		}
	}
	else
	{
		splitWidth  = getNextSmallerBlockSize ( settings->config, width );
		splitHeight = getNextSmallerBlockSize ( settings->config, height );

		if ( width != MIN_BLOCKSIZE || height != MIN_BLOCKSIZE )
		{

			for ( i = 0; i < width / splitHeight; i++, location += splitHeight * modePd->width )
			{
				for ( j = 0; j < width / splitWidth; j++, location += splitWidth )
				{
					if ( location < modePd->size && ( location - j * splitWidth ) / modePd->width == location / modePd->width)                    
					{
						/* check avoids that the block is placed outside of valid coordinates */
						ModeWriter( modePd, 
							chan,
							settings,  
							splitWidth, 
							splitHeight, 
							location,
							loc_x + j,
							loc_y + i
							);
					}
				}
				location -= j * splitWidth;
			}
		}
	}
}

/*------------------------------------------------------------------
 * getH26xDefaultConfig()
 *------------------------------------------------------------------*/
void getH26xDefaultConfig( H26xConfiguration* config )
{
	for (int i = 0; i < NUM_INTRA_MODE; i++)
	{
		config->availablePredictors[i] = 1;
	}

	for (int i = 0; i < NUM_BLOCK_SIZES; i++)
	{
		config->availableBlocks[i] = 1;
	}

	config->logPredictors = 1;
	config->logBlocks = 1;
	config->logTransformations = 1;
	config->printErrPic = 0;

	config->postTransform = 0;
	config->localPrediction = 0;

	config->mse_thresh = 0.5; 
}