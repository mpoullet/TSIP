/*****************************************************
 * File..: H26x_Prediction.c
 * Desc..: the main file of the H26x Predictor
 * Author: Michael Stegemann
 * Date..: 12.11.2012
 * 26.11.2012 Strutz addressing modified, comments added
 * 30.11.2012 Strutz forecast of prediction mode
 * 30.11.2012 Strutz: defines for MSE modification forcing similar predictors
 ******************************************************/

#include "H26x_Prediction.h"
/* for DCVal()     			    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 */
unsigned int bSizeToKp1[17]  = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5};
unsigned int bSizeToLog2[17] = {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4};

/*Strutz: there is a tradeoff between low MSE and low costs for 
 * signalling the decisions, values must be optimised dependend on
 * the coding of side information
 * these are factors
 */
#define MSE_OFF_TRANSFORM 0.9	
#define MSE_OFF_DIRECTION 0.8 

/* not used yet: tradeoff between lower MSE via splitting and 
 * transmitting costs for side information
 */
#define MSE_OFF_SPLIT 0.9 

/*------------------------------------------------------------------
* predictPicture(PicData* pd, cbool encode, H26xSettings* settings)
*
* Main prediction routine of the H26x Predictor
*-----------------------------------------------------------------*/

#if LumaOnly

void predictPicture ( PicData* pd, cbool encode, H26xSettings* settings )
{
	/* 
	* stores the largest possible blocksize in the config struct
	* this avoids that the functions gets called to often
	* (performance issue)
	*/
	settings->config->largestBlockSize = getLargestBlockSize ( settings->config );

	/* Determines how many blocks are in the height and width*/
	uint wBlocks = ( int ) ceil ( ( double ) pd->width  / settings->config->largestBlockSize );
	uint hBlocks = ( int ) ceil ( ( double ) pd->height / settings->config->largestBlockSize );


	uint chan, i, j;

	/* The errorPd structure will contain the prediction errors
	* Its data gets initialised to the MAX Value 255 per byte.
	* Using this we can detect if pixel have been processed by the
	* predictor or not. (Important when reconstructing the image)
	*/

	PicData* errorPd;

	ALLOC ( errorPd, 1u, PicData );

	errorPd->width = pd->width;
	errorPd->height = pd->height;
	errorPd->pixels = pd->height * pd->width;
	errorPd->channel = pd->channel;
	errorPd->bitperchannel = pd->bitperchannel;

	for ( i = 0; i < pd->channel; i++ )
	{
		errorPd->bitperchan[i] = pd->bitperchan[i];
		errorPd->maxVal[i] = pd->maxVal[i];
	}

	ALLOC ( errorPd->data, errorPd->channel, uint* );


	/* Main coding routine */
	if ( encode == TRUE )
	{
		/* Starts the log for the written blocks  */
		settings->log = beginLog ( settings );

		/* loops for all channels */
		for ( chan = 0; chan < pd->channel; chan ++ )
		{
			/* raising the bitdepth to account for the transformation gain */
			if ( settings->config->postTransform > 0 )
			{
				settings->newBitdepth[chan] = pd->bitperchan[chan] + 3;
			}
			else
			{
				settings->newBitdepth[chan] = pd->bitperchan[chan];
			}

			ALLOC ( errorPd->data[chan], pd->height * pd->width, uint );
			memset ( errorPd->data[chan], 255, pd->height * pd->width * sizeof ( uint ) );
		}

		/* Loop for each block */
		for ( i = 0; i < hBlocks; i++ )
		{
			for ( j = 0; j < wBlocks; j++ )
			{
				predictBlock ( pd, 
					errorPd, 
					0, 
					settings->config->largestBlockSize, 
					settings->config->largestBlockSize, 
					( j * settings->config->largestBlockSize + i * settings->config->largestBlockSize * pd->width ), 
					settings );
			}
		}

		/* Copy back the prediction error into the main pd */
		for ( chan = 0; chan < pd->channel; chan ++ )
		{
			memcpy ( &pd->data[chan][0], &errorPd->data[chan][0], pd->pixels * sizeof ( uint ) );
			FREE ( errorPd->data[chan]);           
		}

		/* Optional printing of an error picture */
		if ( settings->config->printErrPic == TRUE )
		{
			WriteModePPM (pd,"predModes.ppm",settings);
			WriteErrorPPM ( pd, "errorPic.ppm", settings->newBitdepth);
		}
	}
	else
	{
		for ( chan = 0; chan < pd->channel; chan ++ )
		{

			ALLOC ( errorPd->data[chan], pd->height * pd->width, uint );
			memset ( errorPd->data[chan], 255, pd->height * pd->width * sizeof ( uint ) );

			settings->newBitdepth[chan] = pd->bitperchan[chan];
		}

		/* Loop for each block */
		for ( i = 0; i < hBlocks; i++ )
		{
			for ( j = 0; j < wBlocks; j++ )
			{
				inverseBlock ( pd, 
					errorPd, 
					0, 
					settings->config->largestBlockSize, 
					settings->config->largestBlockSize, ( j * settings->config->largestBlockSize + i * settings->config->largestBlockSize * pd->width ), 
					settings );
			}
		}

		for ( chan = 0; chan < pd->channel; chan ++ )
		{
			memcpy ( &pd->data[chan][0], &errorPd->data[chan][0], pd->pixels * sizeof ( uint ) );
			FREE ( errorPd->data[chan] );

			/* Lower the bitdepth if a transformation has been used */
			if ( settings->config->postTransform > 0 )
			{
				settings->newBitdepth[chan] = pd->bitperchan[chan] - 3;
			}
			else
			{
				settings->newBitdepth[chan] = pd->bitperchan[chan];
			}	
		}
	}
	FREE ( errorPd->data );
	FREE ( errorPd );

	return;

}

#else

/*------------------------------------------------------------------
* predictPicture(PicData* pd, cbool encode, H26xSettings* settings)
*
* Main prediction routine of the H26x Predictor
*-----------------------------------------------------------------*/
void predictPicture( PicData* pd, cbool encode, H26xSettings* settings )
{
	PicData* errorPd;
	PicData *modePd;
	uint chan, i, j, wBlocks, hBlocks;
	
	modePd = NULL; /* array for selected modes	*/

	/* 
	* stores the largest possible blocksize in the config struct
	* this avoids that the functions gets called to often
	* (performance issue)
	*/
	settings->config->largestBlockSize = getLargestBlockSize ( settings->config );

	/* Determines how many blocks are in the height and width*/
	wBlocks = ( int ) ceil ( ( double ) pd->width  / settings->config->largestBlockSize );
	hBlocks = ( int ) ceil ( ( double ) pd->height / settings->config->largestBlockSize );

	

	ALLOC ( errorPd, 1u, PicData );

	errorPd->width = pd->width;
	errorPd->height = pd->height;
	errorPd->size = pd->height * pd->width;
	errorPd->channel = pd->channel;
	errorPd->bitperchannel = pd->bitperchannel;

	for ( i = 0; i < pd->channel; i++ )
	{
		errorPd->bitperchan[i] = pd->bitperchan[i];
		errorPd->maxVal_rct[i] = pd->maxVal_rct[i];
	}

	ALLOC( errorPd->data, errorPd->channel, uint* );

	/* prepare arry for encoders mode decisions	*/
	ALLOC( modePd, 1u, PicData );
	/* round up to a multiple of block size 4x4
	 * could be a quarter, when minimum block size is 4x4
	 * however the adressing is easier this way
	 */
	modePd->width  = (pd->width + 3) / 4 * 4;	
	modePd->height = (pd->height + 3) / 4 * 4; /* */
	modePd->size = modePd->height * modePd->width;
	modePd->channel = pd->channel;
	ALLOC( modePd->data, modePd->channel, uint*);
	for ( chan = 0; chan < pd->channel; chan++ )
	{
		ALLOC( modePd->data[chan], modePd->height * modePd->width, uint);
		/* initialize with maximum vakue plus 1	*/
		for ( i = 0; i < modePd->size; i++) 
		{
			modePd->data[chan][i] = MODE_INIT;
		}
		
		/* dummy initialisation, not used	*/
		modePd->bitperchan[chan] = 0;
		modePd->maxVal_rct[chan] = 0;
	}
	/* dummy initialisation, not used	*/
	modePd->bitperchannel = 0;

	/* Main coding routine */
	if (encode == TRUE)
	{
		/* Starts the log for the written blocks  */
		settings->log = beginLog( settings );

		/* loops for all channels */
		for ( chan = 0; chan < pd->channel; chan ++ )
		{
			/* raising the bitdepth to account for the transformation gain*/
			/* Strutz:  Why here? Shouldn't we wait for the decision, whether transform is used or not?	*/
			/* Stegemann: I ran a few tests but the data showed that the optional raise of 
		    *	the bitdepth has a negative influence on the entropy.	
			*   Further testing is recommended since the testset was rather small (10)
			*/

			if ( settings->config->postTransform > 0 )
			{
				settings->newBitdepth[chan] = pd->bitperchan[chan] + 3;
			}
			else
			{
				settings->newBitdepth[chan] = pd->bitperchan[chan];
			}

			settings->offset = 1 << settings->newBitdepth[chan];
			settings->upperRange = ( ( settings->offset - 1 ) / 2 );
			settings->lowerRange = - ( settings->offset ) / 2;

			/* 
			* Memory is allocated for each run separately to reduce 
			* the amount of data stored simultainouly 
			*/
			ALLOC( errorPd->data[chan], pd->height * pd->width, uint );
			/* The errorPd structure will contain the prediction errors
			* Its data gets initialised to the MAX Value 255 per byte.
			* Using this we can detect if pixel have been processed by the
			* predictor or not. (Important when reconstructing the image)
				*/
			memset( errorPd->data[chan], 255, pd->height * pd->width * sizeof (uint) );

			/* Loop for each block */
			for ( i = 0; i < hBlocks; i++ )
			{
				for ( j = 0; j < wBlocks; j++ )
				{
					predictBlock( pd, 
						errorPd, 
						modePd, 
						chan, 
						settings->config->largestBlockSize, 
						settings->config->largestBlockSize, 
// Strutz		(j * settings->config->largestBlockSize + i * settings->config->largestBlockSize * pd->width ), 
						i * settings->config->largestBlockSize, j * settings->config->largestBlockSize,	/* location	*/
						settings );
				}
			}

			/* Copy back the prediction error into the main pd */
			memcpy( &pd->data[chan][0], &errorPd->data[chan][0], pd->size * sizeof ( uint ) );
			FREE( errorPd->data[chan] );
		}

		/* Optional printing of an error picture */
		if ( settings->config->printErrPic == TRUE )
		{
			//WriteModePPM( pd, "predModes.ppm", settings);
			WriteErrorPPM( pd, "errorPic.ppm", settings->newBitdepth);
		}
	}
	else
	{
		for ( chan = 0; chan < pd->channel; chan ++ )
		{
			ALLOC ( errorPd->data[chan], pd->size, uint);
			memset ( errorPd->data[chan], 255, pd->size * sizeof( uint));

			settings->newBitdepth[chan] = pd->bitperchan[chan];

			settings->offset = 1 << settings->newBitdepth[chan];
			settings->upperRange = settings->offset - 1;

			/* Loop for each block */
			for ( i = 0; i < hBlocks; i++)
			{
				for ( j = 0; j < wBlocks; j++ )
				{
					inverseBlock ( pd, errorPd, chan, settings->config->largestBlockSize, 
						settings->config->largestBlockSize, 
						//( j * settings->config->largestBlockSize + i * settings->config->largestBlockSize * pd->width ),
						i * settings->config->largestBlockSize, j * settings->config->largestBlockSize,	/* location	*/
						settings );
				}
			}

			memcpy( &pd->data[chan][0], &errorPd->data[chan][0], pd->size * sizeof (uint) );
			FREE( errorPd->data[chan]);

			/* Lower the bitdepth if a transformation has been used */
			if (settings->config->postTransform > 0 )
			{
				settings->newBitdepth[chan] = pd->bitperchan[chan] - 3;
			}
			else
			{
				settings->newBitdepth[chan] = pd->bitperchan[chan];
			}
		}

	} /* decoder	*/

//#if _DEBUG
	if (encode == TRUE)
	{
		for (chan = 0; chan < modePd->channel; chan++)
		{
			char filename[512];
			FILE *out=NULL;
			unsigned int x, y;
			sprintf( filename, "modes_%d.pgm", chan);
			out = fopen( filename, "wt");
			fprintf( out, "P2\n");
			fprintf( out, "%d %d\n", modePd->width, modePd->height);
			/* max value is 35 or could be 35+64 if transforms are enabled*/
			fprintf( out, "%d\n", 35 + 64 * ( 1 && settings->config->postTransform) );
			for (y = 0; y < errorPd->height; y++)
			{
				for (x = 0; x < errorPd->width; x++)
				{
					fprintf( out, "%3d ", modePd->data[chan][x+y*modePd->width]);
				}
				fprintf( out, "\n");
			}
			fclose( out);
		}
	}
//#endif

	for (chan = 0; chan < modePd->channel; chan++)
	{
			FREE( modePd->data[chan]);
	}
	FREE( modePd->data);
	FREE( modePd);

	FREE( errorPd->data);
	FREE( errorPd);

	return;

}

#endif


/*------------------------------------------------------------------
 * predictBlock(PicData* pd, PicData* errorPd, uint chan, uint width, 
 * uint height, uint location, H26xSettings* settings)
 *
 * Predicts the block at the given location
 *-----------------------------------------------------------------*/
void predictBlock( PicData* pd, PicData* errorPd, PicData* modePd, uint chan, 
									uint bwidth, uint bheight, /* block size	*/
//									uint location, /* address of top left block corner	*/
									uint loc_y, uint loc_x, /* vertical, horizontal	*/
									H26xSettings* settings )
{

	uint splitWidth ;
	uint splitHeight;
	uint i;
	uint j;
	//	uint subChan;
	int bestMode = 0;
	double bestResult = MAX_MAD;
	double thisResult;
	uint* workingBlock;
	int* currentBest;
	int* errorBlock;
	uint nMode;
	ReferenceSamples* ref;
	uint location; /* all instances should be substituted by loc_y and loc_x !!*/
	location = loc_y * pd->width + loc_x;

	cbool bestTransformation = FALSE;
	uint s_width;	/* size of available block	*/
	uint s_height;
	cbool isBorderBlock = FALSE;
	unsigned int mode_forecast, transform_forecast; /* decision forcast, used for MSE modification	*/
	unsigned long py, pos, bpy, bpos, x, y;

	/* 
	* Will only be executed when the current block
	* doesn`t fit into the picture (border case)
	* It sets safe blocksizes sizes that are guaranteed
	* to stay within the picture. If process was necessary 
	* the isBorderBlock flag is set to true
	*/
	//if ( ( location + width - 1 ) / pd->width != location / pd->width
	//	||
	//	( location + ( height - 1 ) * pd->width ) >= pd->pixels )
	
	s_height = bheight;	/* keep normal block size	*/
	s_width  = bwidth;
	if ( (loc_y + bheight) >  pd->height )
	{
			//s_height = floor ( ( double ) location / pd->width ) + height <= pd->height ? height : pd->height % height;
		s_height = pd->height - loc_y;
		isBorderBlock = TRUE;
	}
	if ( (loc_x + bwidth) >  pd->width)
	{
		//s_width  = ( location % pd->width ) + width <= pd->width ? width  : pd->width  % width;
		s_width  = pd->width  - loc_x;
		isBorderBlock = TRUE;
	}

	/* Gets available reference pixels */
	ref = checkAvailablity ( pd, errorPd, chan, bwidth, 
		location, loc_y, loc_x, settings->newBitdepth[chan] );
	//ref = checkAvailablity ( pd, errorPd, chan, width, height, loc_y*pd->width+loc_x, settings->newBitdepth[chan] );

	/* 
	* Allocate temporary blocks 
	* workingBlock = prediction values
	* errorBlock = prediction erros (residual data)
	* currentBest = copy of the errorBlock with the best MSE score
	*/
	ALLOC( workingBlock, bwidth * bheight, uint );
	memset( workingBlock, 0, bwidth * bheight * sizeof ( uint ) );
	ALLOC( currentBest, bwidth * bheight, int );
	memset( currentBest, 0, bwidth * bheight * sizeof ( int ) );

	ALLOC( errorBlock, bwidth * bheight, int );

	/* Strutz: forecast of next predictor selection */
	{
		/* initialise four neighbours without mode	*/
		int left_pred=-1, topleft_pred=-1, top_pred=-1, topright_pred=-1;
		int left_flag=-1, topleft_flag=-1, top_flag=-1, topright_flag=-1;
		unsigned char cnt, Hpred[NUM_INTRA_MODE], Hflag[2]; /* histograms	*/
		unsigned long posL;
		
		/* reset histograms	*/
		for (i = 0; i < NUM_INTRA_MODE; i++) Hpred[i] = 0;
		Hflag[0] = 0;
		Hflag[1] = 0;

		/* check neighbours if available	*/
		if (loc_x > 0)
		{
			posL = loc_x-1 + loc_y * modePd->width;
			/* if (modePd->data[chan][posL] < MODE_INIT) */
			/* blocks should be always available to the left	*/
			{
				/* take decision from left neighbour, exclude transform flag	*/
				left_pred = modePd->data[chan][posL] & 0x3f; /* 0011 1111 */
				/* extract transform flag	 and  use AND operator prducing 0 or 1 */
				left_flag = (modePd->data[chan][posL] & 0x40) && 1; /* 0100 0000 */
				Hpred[left_pred]++;	/* count	*/
				Hflag[left_flag]++;
			}
			if (loc_y > 0)
			{
				/* take decision from top-left neighbour	*/
				topleft_pred = modePd->data[chan][loc_x-1 + (loc_y-1) * modePd->width] & 0x3f;
				topleft_flag =(modePd->data[chan][loc_x-1 + (loc_y-1) * modePd->width] & 0x40) && 1;
				Hpred[topleft_pred]++;
				Hflag[topleft_flag]++;
			}
		}
		if (loc_y > 0)
		{
			/* take decision from top neighbour	*/
			top_pred = modePd->data[chan][loc_x + (loc_y-1) * modePd->width] & 0x3f;
			top_flag = (modePd->data[chan][loc_x + (loc_y-1) * modePd->width] & 0x40) && 1;
			Hpred[top_pred]++;
			Hflag[top_flag]++;
			if (loc_x + bwidth  < modePd->width)
			{
				posL = loc_x+bwidth + (loc_y-1) * modePd->width;
				/* blocks to the right might be not processed, yet	*/
				if (modePd->data[chan][posL] < MODE_INIT)
				{
					/* take decision from top-right neighbour	*/
					topright_pred = modePd->data[chan][posL] & 0x3f;
					topright_flag =(modePd->data[chan][posL] & 0x40) && 1;
					Hpred[topright_pred]++;
					Hflag[topright_flag]++;
				}
			}
		}
		/* select maximum, set DC mode as default 	*/
		mode_forecast = DC; cnt = 0;
		for (i = 0; i < NUM_INTRA_MODE; i++) 
		{
			if ( Hpred[i] > cnt)
			{
				cnt = Hpred[i];
				mode_forecast = i;
			}
		}
		transform_forecast = 0; cnt = 0;
		for (i = 0; i < 2; i++) 
		{
			if ( Hflag[i] > cnt)
			{
				cnt = Hflag[i];
				transform_forecast = i;
			}
		}
	} /* end of forecatsing	*/

	/* Test all available predictions */
	/* Strutz: if first block, then all predictors produce the same, don't they?	*/
	/* Stegemann: they are indeed the same in the first block. However the gain is neglectible	*/

	for ( nMode = 0; nMode < NUM_INTRA_MODE; nMode++ )
	{
		/* Only test predictor if it is permitted by the options*/
		if ( settings->config->availablePredictors[nMode] == TRUE )
		{
			/* Perform the prediction*/
			predictLuma( ref, workingBlock, nMode, 
				bwidth, /* width of allocated block	*/
				//s_width, /* available width of block	*/
				bwidth, /* block is  extended	*/
				pd->maxVal_rct[chan] );
//orig:				( 1 << settings->newBitdepth[chan] ) - 1 );
			/* workingBlock now contains the predicted values*/

			memset ( errorBlock, 0, bwidth * bheight * sizeof ( int ) );

			/* Loop that calculates the residual data */

			for ( y = 0, bpy = 0; y < s_height; y++, bpy+=bwidth ) /* for all lines of the block */
			{
				for ( x = 0, bpos = bpy; x < s_width; x++, bpos++ ) /* for all columns of the block */
				{
//						errorBlock[j + ( i * width )] = ( int ) pd->data[chan][location + j + ( i * pd->width )] - ( int ) workingBlock[j + ( i * width )];
					errorBlock[bpos] = (int)pd->data[chan][loc_x + x + ((loc_y+ y) * pd->width )]
													- (int)workingBlock[bpos];

					/* Map the range to -maxVal/2 to maxVal/2-1 */
					// Strutz: compute the offset values and range values only once !!
					// Stegemann: Done.

					if ( errorBlock[bpos] > settings->upperRange )
						errorBlock[bpos] -= settings->offset;
					else if ( errorBlock[bpos] < settings->lowerRange)
						errorBlock[bpos] += settings->offset;

				}
			}

				/*  Evaluate the accuracy of the prediction */
#if    H26xMAD == 1
				thisResult = MAD ( errorBlock, bwidth, s_width, s_height );
#elif  H26xMSE == 1
				thisResult = MSE( errorBlock, bwidth /* full*/, s_width, s_height );
				/* reduce MSE, if forecast was correct	*/
				if (nMode == mode_forecast) 
					thisResult *= MSE_OFF_DIRECTION;
#endif

			if ( bestResult > thisResult )
			{
				/* better prediction -> store the information */
				bestMode = nMode;
				bestResult = thisResult;
				bestTransformation = FALSE;

				memcpy( currentBest, errorBlock, bwidth * bheight * sizeof ( int ) );
			}

			/*  If there is nothing to improve, stop testing */
			// Strutz if ( bestResult == 0 ) break;
			if ( bestResult <= settings->config->mse_thresh )
				break;

			if ( isBorderBlock == TRUE )
			{
				isBorderBlock = isBorderBlock;
				/* Transforming the data with uncomplete blocks is not supported */
				// Strutz: should be realized via signal extension (duplication of border line or column)
			} /* 			if ( isBorderBlock == TRUE ) */
			else if ( settings->config->postTransform > 0 )
			{
				switch ( settings->config->postTransform )
				{
				case 1:
					{
						Lifting_5_3( ( long* ) errorBlock, bwidth, bheight, true, true );
						break;
					}
				case 2:
					{
						Lifting_5_3( ( long* ) errorBlock, bwidth, bheight, false, true );
						break;
					}
				}

#if    H26xMAD == 1
				thisResult = MAD ( errorBlock, bwidth, width, height );;
#elif  H26xMSE == 1
				thisResult = MSE( errorBlock, bwidth, s_width, s_height );
				/* reduce MSE, if forecast was correct	*/
				if (transform_forecast) 
					thisResult *= MSE_OFF_TRANSFORM;
				/* and reduce MSE, if forecast was correct	*/
				if (nMode == mode_forecast) 
					thisResult *= MSE_OFF_DIRECTION;
#endif

				if (bestResult > thisResult)
				{
					/* better prediction found */
					bestMode = nMode;
					bestResult = thisResult;
					bestTransformation = TRUE;

					memcpy( currentBest, errorBlock, bwidth * bheight * sizeof (int) );
				}

				// Strutz if ( bestResult == 0 ) break;
				if ( bestResult <= settings->config->mse_thresh )
					break;
			} /* if ( settings->config->postTransform > 0 )*/
		}
	}

	/* copy decision about prediction mode and transform into modPd array	*/
	for( py = loc_y * modePd->width, y = 0; y < s_height; y++, py += modePd->width)
	{
		for (pos = py + loc_x, x = 0; x < s_width; x++, pos++)
		{
			modePd->data[chan][pos] = bestMode + (bestTransformation << 6);
		}
	}


	/* 
	* Get the next allowed smaller block size
	* Function will return 0 if no more splits are available
	*/
	splitWidth  = getNextSmallerBlockSize ( settings->config, bwidth );
	splitHeight = getNextSmallerBlockSize ( settings->config, bheight );

	/* Strutz if ( ! ( bestResult > 0 ) || splitWidth <= 0 || splitHeight <= 0 ) */
	if ( (bestResult <= settings->config->mse_thresh ) || 
		    splitWidth <= 0 || splitHeight <= 0 )
	{
		/*  block acceptable, stop splitting process */
		addBlock( settings->log, bwidth, bestResult, bestMode, bestTransformation,
			loc_y, loc_x);

		for ( y = 0, bpy = 0, py = loc_y*pd->width; y < s_height; y++, bpy += bwidth, py += pd->width )
		{
			for ( x = 0, bpos = bpy, pos = py + loc_x; x < s_width; x++, bpos++, pos++ )
			{
				/* 
				* Copy the best prediction in the error PicData. This is 
				* important since a valid value in the errorPd structure 
				* marks the location as available for prediction in next steps
				*/
				//errorPd->data[chan][location + j + ( i * pd->width )] = 
				//				currentBest[j + ( i * width )] 
				//				+ ( ( 1 << settings->newBitdepth[chan] ) / 2 )  /* offset	*/;
				errorPd->data[chan][pos] = currentBest[bpos] 
								+ ( settings->offset / 2 )  /* offset	*/;
			}
		}

#if LumaOnly
		for (subChan = 1; subChan < errorPd->channel; subChan++)
		{
			predictBlockChroma ( pd, 
				errorPd, 
				subChan, 
				width, 
				height, 
				location, 
				settings,
				bestMode,
				bestTransformation ? settings->config->postTransform : 0);
		}
#endif

	}
	/* Theres still a splitting option */
	else
	{
		if ( bwidth != MIN_BLOCKSIZE || bheight != MIN_BLOCKSIZE )
		{
			/* The block splitting loop */
			for ( i = 0, y = 0; i < bwidth / splitHeight; i++, y += splitHeight/*, location += splitHeight * pd->width*/ )
			{
				for ( j = 0, x = 0; j < bwidth / splitWidth; j++, x += splitWidth/*, location += splitWidth*/ )
				{
					//if ( location < pd->pixels && ( location - j * splitWidth ) / pd->width == location / pd->width)
					if ( loc_x + x < pd->width  &&  loc_y + y < pd->height)
					{
						/* check avoids that the block is placed outside of valid coordinates */
						// predictBlock ( pd, errorPd, chan, splitWidth, splitHeight, location, settings );
						predictBlock( pd, errorPd, modePd, chan, splitWidth, splitHeight, (loc_y+y), loc_x+x, settings );
					}
				}
				//location -= j * splitWidth;
			}
		}
	} /* */

	/* Free up allocated memory*/
	free ( ref->Samples );
	free ( ref );
	free ( workingBlock );
	free ( currentBest );
	free ( errorBlock );
}

#if LumaOnly
/*------------------------------------------------------------------
* predictBlockChroma ( PicData* pd, 
*						PicData* errorPd, 
*						uint chan, 
*						uint width, 
*						uint height, 
*						uint location, 
*						H26xSettings* 
*						settings, uint 
*						predMode, uint postTransform)
*
* predicts the chroma block at given location
*-----------------------------------------------------------------*/
void predictBlockChroma ( PicData* pd, 
												 PicData* errorPd, 
												 uint chan, 
												 uint width, 
												 uint height, 
												 uint location, 
												 H26xSettings* settings, 
												 uint predMode, uint postTransform)
{

	uint splitWidth ;
	uint splitHeight;
	uint i;
	uint j;
	uint* workingBlock;
	int* errorBlock;

	ReferenceSamples* ref;

	uint s_width = 0;
	uint s_height = 0;
	cbool isBorderBlock = FALSE;

	/* 
	* Will only be executed when the current block
	* doesn`t fit into the picture (border case)
	* It sets safe blocksizes sizes that are guaranteed
	* to stay within the picture. If process was necessary 
	* the isBorderBlock flag is set to true
	*/
	if ( ( location + width - 1 ) / pd->width != location / pd->width
		||
		( location + ( height - 1 ) * pd->width ) >= pd->pixels )
	{

		s_width  = ( location % pd->width ) + width <= pd->width ? width  : pd->width  % width;
		s_height = floor ( ( double ) location / pd->width ) + height <= pd->height ? height : pd->height % height;

		isBorderBlock = TRUE;
	}

	/* Gets available reference pixels */
	ref = checkAvailablity ( pd, errorPd, chan, width, height, location, settings->newBitdepth[chan] );


	/* 
	* Allocate temporary blocks 
	* workingBlock = prediction values
	* errorBlock = prediction erros (residual data)
	* currentBest = copy of the errorBlock with the best MSE score
	*/
	ALLOC ( workingBlock, width * height, uint );
	memset ( workingBlock, 0, width * height * sizeof ( uint ) );
	ALLOC ( errorBlock, width * height, int );
	memset ( errorBlock, 0, width * height * sizeof ( int ) );

	predictLuma ( ref, workingBlock, predMode, width, width, ( 1 << settings->newBitdepth[chan] ) - 1 );



	if ( isBorderBlock == TRUE )
	{
		/* Loop that calculates the residual data */
		for ( i = 0; i < s_height; i++ )
		{
			for ( j = 0; j < s_width; j++ )
			{
				errorBlock[j + ( i * width )] = ( int ) pd->data[chan][location + j + ( i * pd->width )] - ( int ) workingBlock[j + ( i * width )];

				/* Map the range to -maxVal/2 to maxVal/2-1 */
				if ( errorBlock[j + ( i * width )] > ( ( ( 1 << settings->newBitdepth[chan] ) - 1 ) / 2 ) )
					errorBlock[j + ( i * width )] -= 1 << settings->newBitdepth[chan];
				else if ( errorBlock[j + ( i * width )] < - ( 1 << settings->newBitdepth[chan] ) / 2 )
					errorBlock[j + ( i * width )] += 1 << settings->newBitdepth[chan];		
			}
		}				
	}
	else
	{
		for ( i = 0; i < height; i++ )
		{
			for ( j = 0; j < width; j++ )
			{
				errorBlock[j + ( i * width )] = ( int ) pd->data[chan][location + j + ( i * pd->width )] - ( int ) workingBlock[j + ( i * width )];

				/* Map the range to -maxVal/2 to maxVal/2-1  */
				if ( errorBlock[j + ( i * width )] > ( ( ( 1 << settings->newBitdepth[chan] ) - 1 ) / 2 ) )
					errorBlock[j + ( i * width )] -= 1 << settings->newBitdepth[chan];
				else if ( errorBlock[j + ( i * width )] < - ( 1 << settings->newBitdepth[chan] ) / 2 )
					errorBlock[j + ( i * width )] += 1 << settings->newBitdepth[chan];
			}
		}
		switch ( postTransform )
		{
		case 1:
			{
				Lifting_5_3 ( ( long* ) errorBlock, width, height, true, true );
				break;
			}
		case 2:
			{
				Lifting_5_3 ( ( long* ) errorBlock, width, height, false, true );
				break;
			}
		}

	}

	if ( isBorderBlock == TRUE )
	{
		for ( i = 0; i < s_height; i++ )
		{
			for ( j = 0; j < s_width; j++ )
			{
				/* 
				* Copy the best prediction in the error PicData. This is 
				* important since a valid value in the errorPd structure 
				* marks the location as available for prediction in next steps
				*/
				errorPd->data[chan][location + j + ( i * pd->width )] = errorBlock[j + ( i * width )] + ( ( 1 << settings->newBitdepth[chan] ) / 2 );
			}
		}
	}
	else
	{
		for ( i = 0; i < height; i++ )
		{
			for ( j = 0; j < width; j++ )
			{
				/* 
				* Copy the best prediction in the error PicData. This is 
				* important since a valid value in the errorPd structure 
				* marks the location as available for prediction in next steps
				*/
				errorPd->data[chan][location + j + ( i * pd->width )] = errorBlock[j + ( i * width )] + ( ( 1 << settings->newBitdepth[chan] ) / 2 );
			}
		}


	}

	/* Free up allocated memory*/
	free ( ref->Samples );
	free ( ref );
	free ( workingBlock );
	free ( errorBlock );
}
#endif

/*------------------------------------------------------------------
* inverseBlock(PicData* pd, PicData* errorPd, uint chan, uint width, uint height, uint location, H26xSettings* settings)
*
* inverses the prediction at given location
*-----------------------------------------------------------------*/
void inverseBlock ( PicData* pd, PicData* errorPd, uint chan, 
									 uint bwidth, uint bheight, uint loc_y, uint loc_x, H26xSettings* settings )
{
	uint splitWidth;
	uint splitHeight;
	uint i;
	uint j;
	//	uint subChan;
	uint* workingBlock;
	int* errorBlock;
	ReferenceSamples* ref;
	H26xBlock* thisBlock;

	uint s_width = 0;
	uint s_height = 0;
	cbool isBorderBlock = FALSE;
	unsigned long x, y, bpos, bpy;

	if ( settings->log->firstNode->bSize == bwidth )
	{

		thisBlock = getBlock ( settings->log );

		/*  Align help sizes in case this is a bordering block */
		//if ( ( location + width - 1 ) / pd->width != location / pd->width
		//	||
		//	( location + ( height - 1 ) * pd->width ) >= pd->pixels )
		s_height = bheight;	/* keep normal block size	*/
		s_width  = bwidth;
		if ( (loc_y + bheight) >  pd->height )
		{
				//s_height = floor ( ( double ) location / pd->width ) + height <= pd->height ? height : pd->height % height;
			s_height = pd->height - loc_y;
			isBorderBlock = TRUE;
		}
		if ( (loc_x + bwidth) >  pd->width)
		{
			//s_width  = ( location % pd->width ) + width <= pd->width ? width  : pd->width  % width;
			s_width  = pd->width  - loc_x;
			isBorderBlock = TRUE;
		}

		/* Allocate temporary blocks */

		ALLOC ( workingBlock, bwidth * bheight, uint );
		memset ( workingBlock, 0, bwidth * bheight * sizeof ( uint ) );

		ALLOC ( errorBlock, bwidth * bheight, int );
		memset ( errorBlock, 0, bwidth * bheight * sizeof ( int ) );

		if ( isBorderBlock == TRUE )
		{

			for ( i = 0; i < s_height; i++ )
			{
				for ( j = 0; j < s_width; j++ )
				{

					errorBlock[j + ( i * bwidth )] = ( int ) pd->data[chan][loc_x + j + ((loc_y + i) * pd->width )] 
							- ( ( settings->offset ) / 2 );
				}
			}
			/*if ( thisBlock->usedTransformation == TRUE )
			{
			switch ( settings->config->postTransform )
			{
			case 1:
			{
			Lifting_5_3 ( ( long* ) errorBlock, s_width, s_height, true, false );
			break;
			}
			case 2:
			{
			Lifting_5_3 ( ( long* ) errorBlock, s_width, s_height, false, false );
			break;
			}
			}
			}*/

			ref = checkAvailablity ( errorPd, errorPd, chan, 
				bwidth, loc_y*pd->width+loc_x, loc_y, loc_x, settings->newBitdepth[chan] );
			predictLuma( ref, workingBlock, thisBlock->nMode, bwidth, bwidth, ( 1 << settings->newBitdepth[chan] ) - 1 );

/*
			for ( i = 0; i < s_height; i++ )
			{
				for ( j = 0; j < s_width; j++ )
				{
					errorBlock[j + ( i * width )] += ( int ) workingBlock[j + ( i * width )];

					if		( errorBlock[j + ( i * width )] > ( ( 1 << settings->newBitdepth[chan] ) - 1 ) )
						errorBlock[j + ( i * width )] -= 1 << settings->newBitdepth[chan];
					else if ( errorBlock[j + ( i * width )] < 0 )
						errorBlock[j + ( i * width )] += 1 << settings->newBitdepth[chan];
				}
			}
*/
			for ( y = 0, bpy = 0; y < s_height; y++, bpy+=bwidth ) /* for all lines of the block */
			{
				for ( x = 0, bpos = bpy; x < s_width; x++, bpos++ ) /* for all columns of the block */
				{
					errorBlock[bpos] += (int)workingBlock[bpos];

					/* reMap  */
					if ( errorBlock[bpos] > settings->upperRange )
						errorBlock[bpos] -= settings->offset;
					else if ( errorBlock[bpos] < 0 )
						errorBlock[bpos] += settings->offset;
				}
			}
		}
		else
		{
			for ( i = 0; i < bheight; i++ )
			{
				for ( j = 0; j < bwidth; j++ )
				{
					errorBlock[j + ( i * bwidth )] = (int)pd->data[chan][loc_x + j + ( (loc_y + i) * pd->width )] 
					    - ( ( settings->offset ) / 2 );
				}
			}
			if ( thisBlock->usedTransformation == TRUE && settings->config->postTransform > 0 )
			{
				switch ( settings->config->postTransform )
				{
				case 1:
					{
						Lifting_5_3 ( ( long* ) errorBlock, bwidth, bheight, true, false );
						break;
					}
				case 2:
					{
						Lifting_5_3 ( ( long* ) errorBlock, bwidth, bheight, false, false );
						break;
					}
				}
			}
			ref = checkAvailablity ( errorPd, errorPd, chan, 
				bwidth, loc_y*pd->width + loc_x, loc_y, loc_x, settings->newBitdepth[chan] );
			predictLuma ( ref, workingBlock, thisBlock->nMode, bwidth, bwidth, ( 1 << settings->newBitdepth[chan] ) - 1 );
			for ( i = 0; i < bheight; i++ )
			{
				for ( j = 0; j < bwidth; j++ )
				{
					errorBlock[j + ( i * bwidth )] += ( int ) workingBlock[j + ( i * bwidth )];

					if		( errorBlock[j + ( i * bwidth )] > settings->upperRange )
						errorBlock[j + ( i * bwidth )] -= settings->offset;
					else if ( errorBlock[j + ( i * bwidth )] < 0 )
						errorBlock[j + ( i * bwidth )] += settings->offset;
				}
			}
		}

		if ( isBorderBlock == TRUE )
		{
			for ( i = 0; i < s_height; i++ )
			{
				for ( j = 0; j < s_width; j++ )
				{
					errorPd->data[chan][loc_x + j + ((loc_y + i) * pd->width )] = ( uint ) errorBlock[j + ( i * bwidth )];
				}
			}
		}
		else
		{
			for ( i = 0; i < bheight; i++ )
			{
				for ( j = 0; j < bwidth; j++ )
				{
					errorPd->data[chan][loc_x + j + ((loc_y + i) * pd->width )] = ( uint ) errorBlock[j + ( i * bwidth )];
				}
			}
		}

#if LumaOnly
		for (subChan = 1; subChan < errorPd->channel; subChan++){
			inverseBlockChroma ( pd, 
				errorPd, 
				subChan, 
				width, 
				height, 
				location, 
				settings,
				thisBlock->nMode,
				thisBlock->usedTransformation ? settings->config->postTransform : 0);
		}
#endif

		free ( ref->Samples );
		free ( ref );
		free ( workingBlock );
		free ( errorBlock );
		free ( thisBlock );

	}
	else
	{

		splitWidth  = getNextSmallerBlockSize ( settings->config, bwidth );
		splitHeight = getNextSmallerBlockSize ( settings->config, bheight );

		if ( bwidth != MIN_BLOCKSIZE || bheight != MIN_BLOCKSIZE )
		{

			for ( i = 0, y = 0; i < bwidth / splitHeight; i++, y += splitHeight/*, location += splitHeight * pd->width*/ )
			{
				for ( j = 0, x = 0; j < bwidth / splitWidth; j++, x += splitWidth/*, location += splitWidth*/ )
				{
					//if ( location < pd->pixels && ( location - j * splitWidth ) / pd->width == location / pd->width)                    
					if ( loc_x + x < pd->width  &&  loc_y + y < pd->height)
					{
						/* check avoids that the block is placed outside of valid coordinates */
						//inverseBlock ( pd, errorPd, chan, splitWidth, splitHeight, location, settings );
						inverseBlock( pd, errorPd, chan, splitWidth, splitHeight, (loc_y+y), loc_x+x, settings );
					}
				}
				//location -= j * splitWidth;
			}
		}
	}
}

#if LumaOnly
/*------------------------------------------------------------------
* inverseBlockChroma(PicData* pd, PicData* errorPd, uint chan, uint width, uint height, uint location, H26xSettings* settings)
*
* inverses the prediction for the chroma block at given location
*-----------------------------------------------------------------*/
void inverseBlockChroma ( PicData* pd, PicData* errorPd, uint chan, uint width, uint height, uint location, H26xSettings* settings, uint predMode, uint postTransform )
{
	uint splitWidth;
	uint splitHeight;
	uint i;
	uint j;
	uint* workingBlock;
	int* errorBlock;
	ReferenceSamples* ref;
	H26xBlock* thisBlock;

	uint s_width = 0;
	uint s_height = 0;
	cbool isBorderBlock = FALSE;

	/*  Align help sizes in case this is a bordering block */
	if ( ( location + width - 1 ) / pd->width != location / pd->width
		||
		( location + ( height - 1 ) * pd->width ) >= pd->pixels )
	{
		s_width  = ( location % pd->width ) + width <= pd->width ? width  : pd->width  % width;
		s_height = floor ( ( double ) location / pd->width ) + height <= pd->height ? height : pd->height % height;

		isBorderBlock = TRUE;
	}

	ALLOC ( workingBlock, width * height, uint );
	memset ( workingBlock, 0, width * height * sizeof ( uint ) );

	ALLOC ( errorBlock, width * height, int );
	memset ( errorBlock, 0, width * height * sizeof ( int ) );

	if ( isBorderBlock == TRUE )
	{
		for ( i = 0; i < s_height; i++ )
		{
			for ( j = 0; j < s_width; j++ )
			{
				errorBlock[j + ( i * width )] = ( int ) pd->data[chan][location + j + ( i * pd->width )] - ( ( 1 << settings->newBitdepth[chan] ) / 2 );
			}
		}

		ref = checkAvailablity ( errorPd, errorPd, chan, width, height, location, settings->newBitdepth[chan] );
		predictLuma ( ref, workingBlock, predMode, width, width, ( 1 << settings->newBitdepth[chan] ) - 1 );

		for ( i = 0; i < s_height; i++ )
		{
			for ( j = 0; j < s_width; j++ )
			{
				errorBlock[j + ( i * width )] += ( int ) workingBlock[j + ( i * width )];

				if		( errorBlock[j + ( i * width )] > ( ( 1 << settings->newBitdepth[chan] ) - 1 ) )
					errorBlock[j + ( i * width )] -= 1 << settings->newBitdepth[chan];
				else if ( errorBlock[j + ( i * width )] < 0 )
					errorBlock[j + ( i * width )] += 1 << settings->newBitdepth[chan];

			}
		}
	}
	else
	{
		for ( i = 0; i < height; i++ )
		{
			for ( j = 0; j < width; j++ )
			{
				errorBlock[j + ( i * width )] = ( int ) pd->data[chan][location + j + ( i * pd->width )] - ( ( 1 << settings->newBitdepth[chan] ) / 2 );
			}
		}
		switch ( postTransform )
		{
		case 1:
			{
				Lifting_5_3 ( ( long* ) errorBlock, width, height, true, false );
				break;
			}
		case 2:
			{
				Lifting_5_3 ( ( long* ) errorBlock, width, height, false, false );
				break;
			}
		}

		ref = checkAvailablity ( errorPd, errorPd, chan, width, height, location, settings->newBitdepth[chan] );
		predictLuma ( ref, workingBlock, predMode, width, width, ( 1 << settings->newBitdepth[chan] ) - 1 );
		for ( i = 0; i < height; i++ )
		{
			for ( j = 0; j < width; j++ )
			{
				errorBlock[j + ( i * width )] += ( int ) workingBlock[j + ( i * width )];

				if		( errorBlock[j + ( i * width )] > ( ( 1 << settings->newBitdepth[chan] ) - 1 ) )
					errorBlock[j + ( i * width )] -= 1 << settings->newBitdepth[chan];
				else if ( errorBlock[j + ( i * width )] < 0 )
					errorBlock[j + ( i * width )] += 1 << settings->newBitdepth[chan];
			}
		}
	}

	if ( isBorderBlock == TRUE )
	{
		for ( i = 0; i < s_height; i++ )
		{
			for ( j = 0; j < s_width; j++ )
			{
				errorPd->data[chan][location + j + ( i * pd->width )] = ( uint ) errorBlock[j + ( i * width )];
			}
		}
	}
	else
	{
		for ( i = 0; i < height; i++ )
		{
			for ( j = 0; j < width; j++ )
			{
				errorPd->data[chan][location + j + ( i * pd->width )] = ( uint ) errorBlock[j + ( i * width )];
			}
		}
	}

	free ( ref->Samples );
	free ( ref );
	free ( workingBlock );
	free ( errorBlock );
}

#endif



/*------------------------------------------------------------------
* ReferenceSamples* checkAvailablity(PicData* pd, PicData* errorPd, int chan, int bSize, int height, int location, uint BitDepth)
*
* checks the availablity of neighbouring pixels and returns a
* pointer to the ReferenceSamples structure containing the samples
*-----------------------------------------------------------------*/
ReferenceSamples* checkAvailablity ( PicData* pd, PicData* errorPd, 
													int chan, int bSize, 
													int location, 
													int loc_y, int loc_x,
													uint BitDepth )
{
	cbool availableUp = FALSE;
	cbool availableLeft = FALSE;
	ReferenceSamples* ref;

	int bSize2;
	uint i, j;

	
	bSize2 = bSize * 2;

	ALLOC ( ref, 1u, ReferenceSamples );
	ALLOC ( ref->Samples, ( bSize2 * 2 ) + 1, uint );

	memset ( ref->Samples, 0, ( ( bSize2 * 2 ) + 1 ) * sizeof ( uint ) );

	/* coordinate validation */
	//if ( location - ( int ) pd->width > 0 )
	if ( loc_y > 0 )
	{
		availableUp = TRUE;
	}

	//if ( ( ( location % ( int ) pd->width ) - 1 ) > 0 )
	if ( loc_x > 0 )
	{
		availableLeft = TRUE;
	}
// Strutz: ref->Top wird für Blöcke am linken Rand nicht korrekt gesetzt
	if ( availableUp == TRUE || availableLeft == TRUE )
	{

		/* 8.4.3.1.1
		* Step one:
		* Start at -1, (nS*2)-1
		*/

		if ( loc_y + bSize2 - 1 < (signed)pd->height )
		{
			// Strutz: what is this check good for?  Is it already initialized?
			/* The errorPd data gets initalised to its MAX value (xx since its unsigned int) 
			*  The mechanism checks if the value is plausible (= the pixel has been processed already)
			*/
			if ( errorPd->data[chan][location - 1 + ( pd->width * (bSize2 - 1) )]	>
				(uint)( 1 << BitDepth ) - 1
				)
			{
				ref->Samples[0] = replaceSample ( pd, errorPd, chan, bSize, 
					location, availableLeft, availableUp, BitDepth );
			}
		}
		else
		{
			ref->Samples[0] = replaceSample ( pd, errorPd, chan, bSize, 
				location, availableLeft, availableUp, BitDepth);
		}

		/*
		* Step two:
		* Fill the rest, if one sample is missing copy the previous
		*/
		for ( i = 0; (signed)i < (bSize2 - 1); i++ )
		{
			if ( ( availableLeft == TRUE ) &&
				// Strutz: ( location - 1 + ( pd->width * ( ( bSize * 2 ) - 2 - i ) ) < pd->pixels ) &&
				( loc_y + bSize2 - 2 - i < pd->height ) &&
				( errorPd->data[chan][location - 1 + ( pd->width * ( bSize2 - 2 - i ) )] <= (unsigned)( 1 << BitDepth ) - 1 )
				)
			{
				ref->Samples[1 + i] = pd->data[chan][location - 1 + (pd->width * ( bSize2 - 2 - i ) )];
			}
			else
			{
				ref->Samples[1 + i] = ref->Samples[i];
			}
		}

		if ( availableLeft == TRUE && availableUp == TRUE )
		{
			ref->Samples[bSize2] = pd->data[chan][location - 1 - pd->width];
		}
		else
		{
			ref->Samples[bSize2] = ref->Samples[bSize2 - 1];
		}

		for ( j = bSize2 + 1, i = 0; (signed)j < bSize2 * 2 + 1; j++, i++ )
		{
			if ( availableUp == TRUE &&
				(int)( ( location - pd->width ) / pd->width ) == (int)( ( location - pd->width + i ) / pd->width ) &&
				errorPd->data[chan][location - pd->width + i] <= (unsigned)( 1 << BitDepth ) - 1
				)
			{
				ref->Samples[j] = pd->data[chan][location - pd->width + i];
			}
			else
			{
				ref->Samples[j] = ref->Samples[j - 1];
			}
		}
	}
	else
	{
		/* Init all to 1 << (BitDepth - 1) */
		for ( i = 0; (signed)i < (2 * bSize2 ) + 1; i++ )
		{
			ref->Samples[i] = 1 << (BitDepth - 1 );
		}
	}

	/* align the pointers */
	/* bSize = 4
	 * M|ABCD|EFGH
	 * -----------
	 * I|
	 * J|
	 * K|
	 * L|
	 * -|----|--
	 * N|
	 * O|
	 * P|
	 * Q|
	 *  LB   L     TL T    TR
	 * |QPON|LKJI| M |ABCD|EFGH
	 */
	 /*
	 * the pointers of a block will always point to the pixel closest to the top left one
	 * e.g. The pointer for the TopRight Block will be E (since its closest to M). 
	 * 
	 * Also: the ref->Samples pointer points to the lowest pixel (Q in this case)
	 */
	 
	ref->TopLeft = ref->Samples + 2 * bSize;
	ref->Top = ref->TopLeft + 1;
	ref->Left = ref->TopLeft - 1;
	ref->TopRight = ref->Top + bSize;
	ref->LeftBottom = ref->Left - bSize;

	return ref;
}

/*------------------------------------------------------------------
*void predictLuma(ReferenceSamples* ref,
*                 uint* errorBlock,
*                 uint nMode,
*                 uint width,
*                 uint bSize,
*                 uint maxVal)
*
* a wrapper function all prediction routines
*-----------------------------------------------------------------*/
void predictLuma( ReferenceSamples* ref,
									uint* errorBlock,
									uint nMode,
									uint width,
									uint bSize,
									uint maxVal )
{
	if ( nMode == PLANAR )
	{
		predictPlanar(
			ref,
			errorBlock,
			width,
			bSize
			);
	}
	else if ( nMode == DC )
	{
		predictDC(
			ref,
			errorBlock,
			width,
			bSize,
			TRUE
			);
	}
	else
	{
		predictAng(
			ref,
			errorBlock,
			width,
			bSize,
			nMode,
			maxVal,
			TRUE
			);
	}
}

/*------------------------------------------------------------------
* double MAD(uint* pd, int width, int height, int fullWidth)
*
* calculates the mean of absolute differences for the given block
*-----------------------------------------------------------------*/
double MAD ( int* errors, int stride, int width, int height )
{
	int i, j;
	int tmp;

	uint sum = 0;

	for ( i = 0; i < height; i++ )
	{
		for ( j = 0; j < width; j++ )
		{
			tmp = errors[j + ( stride * i )];
			sum += abs(tmp);
		}
	}

	return ( double ) sum / ( width * height );
}

/*------------------------------------------------------------------
* double MSE(int* errors, uint expectedVal, int stride, int width, int height)
*
* calculates the mean squared error for the given block
*-----------------------------------------------------------------*/
double MSE( int* errors, int stride, int width, int height )
{
	int i, j;
	int tmp;

	uint sum = 0;

	for ( i = 0; i < height; i++ )
	{
		for ( j = 0; j < width; j++ )
		{
			tmp = errors[j + ( stride * i )];
			sum += tmp * tmp;
		}
	}

	return ( double ) sum / ( width * height );
}

/*------------------------------------------------------------------
* int getDCVal(ReferenceSamples* ref, uint bSize)
*
* calculates the DC Value
*-----------------------------------------------------------------*/
int getDCVal ( ReferenceSamples* ref, uint bSize )
{
	uint sumTop = 0;
	uint sumLeft = 0;
	uint DCVal;
	int i/*, k*/;

	// k = log2 ( bSize ); // Strutz: do not compute the same value all the time
  
	for ( i = 0; i < ( int ) bSize; i++ )
	{
		sumTop  += ref->Top[ i];
		sumLeft += ref->Left[-i];
	}

	/* (8-35) */
	//DCVal = ( sumTop + sumLeft + bSize ) >> ( k + 1 );
	DCVal = ( sumTop + sumLeft + bSize ) >> bSizeToKp1[bSize];
	return DCVal;
}

/*------------------------------------------------------------------
* void predictDC(
*   ReferenceSamples* ref,
*   uint*	 errorBlock,
*   uint	 width,
*   uint	 bSize,
*   cbool	 isLuma
* )
*
* performs the DC prediction
*-----------------------------------------------------------------*/
void predictDC( ReferenceSamples* ref,
								uint*	 errorBlock,
								uint	 width,
								uint	 bSize,
								cbool	 isLuma
								)
{
	uint DCVal = getDCVal( ref, bSize );
	int i, j;

	//Strutz: what about using memset() for a constant value ??
	// Memset is unreliable for writing values larger than 0 on multibyte datatypes
	// Quick example: memset (5) writes the binary "00000101" into ALL of the four bytes of an integer
	// Result: 00000101000001010000010100000101 
	for ( i = 0; i < (signed)bSize; i++ )
	{
		for ( j = 0; j < (signed)bSize; j++ )
		{
			/* (8-40 || 8-39) */
			errorBlock[j + ( i * width )] = DCVal;
		}
	}

	/* from Draft:
	 * 2.	If intraPredMode is equal to Intra_DC, intraFilterType[ nS ][ IntraPredMode ] is set equal to 0
	 * Strutz: this filtering should not be performed here, should it?
	 * Actually what is this filtering god for?
	 * 
	 * Stegemann: further details can be found in section 8.3.3.1.1	Filtering process of neighbouring samples
	 */
	if ( isLuma )
	{
		/* (8-36) */
		errorBlock[0] = ( ref->Top[0] + ref->Left[0] + 2 * errorBlock[0] + 2 ) >> 2;
		for ( i = 1; i < (int)bSize; i++ )
		{
			/* (8-37) */
			errorBlock[i        ] = ( ref->Top [ i] + 3 * errorBlock[i        ] + 2 ) >> 2;
			/* (8-38) */
			errorBlock[i * width] = ( ref->Left[-i] + 3 * errorBlock[i * width] + 2 ) >> 2;
		}
	}
}

/*------------------------------------------------------------------
* void predictPlanar(
*   ReferenceSamples* ref,
*   uint*	 errorBlock,
*   uint	 width,
*   uint	 bSize
* )
*
* performs the planar prediction
*-----------------------------------------------------------------*/
void predictPlanar(
										ReferenceSamples* ref,
										uint*	 errorBlock,
										uint	 width,
										uint	 bSize
										)
{
	int i, j;

	// Strutz: do not compute this value several times, use LUT*/
	// log2(14) = 3, is this correct?
	uint log2Size = bSizeToKp1[bSize]; 
	int horPred;

	int *leftColumn,
		*topRow,
		*bottomRow,
		*rightColumn;

	uint offset2D = bSize;
	uint shift1D = log2Size;
	uint shift2D = shift1D + 1;

	ALLOC ( leftColumn, bSize + 1u, int );
	ALLOC ( topRow, bSize + 1u, int );
	ALLOC ( bottomRow, bSize, int );
	ALLOC ( rightColumn, bSize, int );

	/* Get left and above reference column and row */
	// Strutz: is it really necessary to copy the values from ref to new arrays?
	// Stegemann: since the copies values get modified, the copy is necessary (this method runs
	// multiple times on the same referenc samples)
	//
	for ( i = 0; i < (int)bSize + 1; i++ )
	{
		topRow[i]     = ref->Top[i];
		leftColumn[i] = ref->Left[-i];
	}

	/* Prepare intermediate variables used in interpolation */
	for ( i = 0; i < (int)bSize; i++ )
	{
		bottomRow[i]   = ( int ) ref->LeftBottom[0] - topRow[i];
		rightColumn[i] = ( int ) ref->TopRight[0]   - leftColumn[i];
		topRow[i]      <<= shift1D;
		leftColumn[i]  <<= shift1D;
	}

	/* Generate prediction signal */
	for ( i = 0; i < ( int ) bSize; i++ )
	{
		horPred = leftColumn[i] + offset2D;
		for ( j = 0; j < ( int ) bSize; j++ )
		{
			horPred += rightColumn[i];
			topRow[j] += bottomRow[j];
			errorBlock[i * width + j] = ( ( horPred + topRow[j] ) >> shift2D );
		}
	}
	free ( leftColumn );
	free ( topRow );
	free ( bottomRow );
	free ( rightColumn );
}


/*------------------------------------------------------------------
* void predictAng(
*   ReferenceSamples* ref,
*   uint*	 errorBlock,
*   uint	 width,
*   int	 bSize,
*   uint	 nMode,
*   uint	 maxVal,
*   cbool	 isLuma
* )
*
* performs the angulary prediction
*-----------------------------------------------------------------*/
void predictAng(
								 ReferenceSamples* ref,
								 uint*	 errorBlock,
								 uint	 width,
								 int		 bSize,
								 uint	 nMode,
								 uint	 maxVal,
								 cbool	 isLuma
								 )
{
	cbool   bModeHor         = ( nMode < 18 );
	int    nPredAngle		= PredAngle[nMode];
	int    nInvAngle        = InvPredAngle[nMode];

	int nOffset, iSum;
	int deltaPos = 0;
	int refMainIndex;
	int iIdx;
	int iFact;
	uint* RefBuf;

	ALLOC( RefBuf, 2 * MAX_BLOCKSIZE + 1, uint );

	uint *pucRefMain    = RefBuf + ( ( nPredAngle < 0 ) ? MAX_BLOCKSIZE : 0 );

	int    x, k;

	/* Step 1 */
	/* (8-41) and (8-49) */
	for ( x = 0; x < bSize + 1; x++ )
	{
		pucRefMain[x] = bModeHor ? ref->TopLeft[-x] : ref->TopLeft[x];
	}

	if ( nPredAngle < 0 )
	{
		iSum = 128;
		/* (8-42) or (8-50) */
		for ( x = -1; x > ( bSize * nPredAngle ) >> 5; x-- )
		{
			iSum += nInvAngle;
			nOffset = bModeHor ? ( iSum >> 8 ) : - ( iSum >> 8 );
			pucRefMain[x] = ref->TopLeft[ nOffset ];
		}
	}
	else
	{
		/* (8-43) or (8-51) */
		for ( x = bSize + 1; x < 2 * bSize + 1; x++ )
		{
			pucRefMain[x] = bModeHor ? ref->TopLeft[-x] : ref->TopLeft[x];
		}
	}

	/* Step 2 */
	for ( k = 0; k < bSize; k++ )
	{
		deltaPos += nPredAngle;
		iIdx  = deltaPos >> 5;  /* (8-44) and (8-52) */
		iFact = deltaPos & 31;  /* (8-45) and (8-53) */

		if ( iFact )
		{
			for ( x = 0; x < bSize; x++ )
			{
				refMainIndex           = x + iIdx + 1;
				errorBlock[k * width + x] = ( ( ( 32 - iFact ) * pucRefMain[refMainIndex] + iFact * pucRefMain[refMainIndex + 1] + 16 ) >> 5 );
			}
		}
		else
		{
			for ( x = 0; x < bSize; x++ )
			{
				errorBlock[k * width + x] = pucRefMain[iIdx + 1 + x];
			}
		}
	}

	/* see (8-48) and (8-56) */
	if ( isLuma && ( nPredAngle == 0 ) )
	{
		int offset = bModeHor ? 1 : -1;
		for ( x = 0; x < bSize; x++ )
		{
			errorBlock[x * width] = Clip ( errorBlock[x * width] + ( ( ref->TopLeft[ ( x + 1 ) * offset] - ref->TopLeft[0] ) >> 1 ) , maxVal );
		}
	}

	if ( bModeHor )
	{
		uint tmp;
		for ( k = 0; k < bSize - 1; k++ )
		{
			for ( x = k + 1; x < bSize; x++ )
			{
				tmp                   = errorBlock[k * width + x];
				errorBlock[k * width + x] = errorBlock[x * width + k];
				errorBlock[x * width + k] = tmp;
			}
		}
	}
	free ( RefBuf );
}

/*------------------------------------------------------------------
* uint replaceSample(PicData* pd,
*                 PicData* errorPd,
*                 int chan,
*                 int bSize,
*                 int location,
*                 cbool left,
*                 cbool top,
*                 uint BitDepth
* )
*
* searches for the next available sample during the reference sample
* substitution process
*-----------------------------------------------------------------*/
uint replaceSample ( PicData* pd,
										PicData* errorPd,
										int chan,
										int bSize,
										int location,
										cbool left,
										cbool top,
										uint BitDepth
										)
{
	int i;

	if ( left == TRUE )
	{
		for ( i = 0; i < bSize * 2 - 1; i++ )
		{
			if ( location - 1 + ( pd->width * ( ( bSize * 2 ) - 2 - i ) ) < pd->size )
			{
				if ( errorPd->data[chan][location - 1 + ( pd->width * ( ( bSize * 2 ) - 2 - i ) )] <= (unsigned)(1 << BitDepth ) - 1 )
				{
					return pd->data[chan][location - 1 + ( pd->width * ( ( bSize * 2 ) - 2 - i ) )];
				}
			}
		}
	}
	if ( left == TRUE && top == TRUE )
	{
		if ( errorPd->data[chan][location - 1 - pd->width] <= (unsigned)(1 << BitDepth) - 1 )
		{
			return pd->data[chan][location - 1 - pd->width];
		}
	}
	if ( top == TRUE )
	{
		for ( i = 0; i < bSize * 2; i++ )
		{
			if ( (int)( (location - pd->width) / pd->width) == (int)( (location - pd->width + i) / pd->width) )
				if ( errorPd->data[chan][location - pd->width + i] <= (unsigned)(1 << BitDepth) - 1 )
					return pd->data[chan][location - pd->width + i];
		}
	}
	return NULL;
}