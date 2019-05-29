/*****************************************************
* File..: leastMeanSquaresPredCol.c
* Desc..: LMS prediction across colour components
* Author: Tilo Strutz
* Date..: 04.09.2017
******************************************************/
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "ginterface.h"
#include "codec_CoBaLP.h" /* for CALLOC()*/
#include "matrix_utils.h"

using namespace System::Runtime::InteropServices;

#include "leastSquaresPred.h"
#define MEANFREE
//#undef MEANFREE /* seems to be equal or better	*/



/*--------------------------------------------------------------
* lmsPredictionCol() 
*-------------------------------------------------------------*/
void lmsPredictionCol( PicData *pd, int numOfPreds, int searchSpace,
									 int encode_flag)
{
	int x, y;
	unsigned int chan;
	/*     141115
	**   13 7 5 816
	** 12 6 2 1 3 917
	** 10 4 0 X
	**
	**                           X  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17
	*/
	int dx[MAX_NUMOFPREDS+1] = { 0,-1, 0,-1, 1,-2, 0,-2,-1, 1, 2,-3, 0,-3,-2,-1, 1, 2, 3};
	int dy[MAX_NUMOFPREDS+1] = { 0, 0,-1,-1,-1, 0,-2,-1,-2,-2,-1, 0,-3,-1,-2,-3,-3,-2,-1};
	int dxMax, dyMax, dyMin;
	int p; /* predictor number*/
	/*     
	**     10 
	**    5 2 6 
	**  9 1 X 311
	**    8 4 7
	**     12
	**                             X  1  2  3  4  5  6  7  8  9 10 11 12
	*/
	int dxC[NUMOF_COL_LSPREDS] = { 0,-1, 0, 1, 0,-1, 1, 1,-1,-2, 0, 2, 0};
	int dyC[NUMOF_COL_LSPREDS] = { 0, 0,-1, 0, 1,-1,-1, 1, 1, 0,-2, 0, 2};
	int numOfPredsCol = NUMOF_COL_LSPREDS;
	//int numOfPredsCol = 1; /* take only value at current position ###################*/
	int templateMatching_flag = 1, matchFound_flag = 0;
	int equal_flag = 0;
	int absError;
	long 	epsilon, xHat=0;
	long range, offset;
	long 	Rx, n, m;
	long posS;	/* current position of target value in search space	*/
	int searchSpaceWidth, searchSpaceHeight, errSpaceWidth, searchSpaceSize;
	double *space=NULL, *space0=NULL, *space1=NULL;
	double error, xHatd = 0., xHatdTM=0, xHatdRM=0;
	double pattern[6]; /* A C B D E F	*/
	double pattern0[6]; /* A C B D E F	*/
	double pattern1[6]; /* A C B D E F	*/
	double meanOfSamples = 0,  meanOfSamples0 = 0,  meanOfSamples1 = 0;
	double meanOfAbsSamples, meanOfAbsSamples0, meanOfAbsSamples1;
	double *weights = NULL;	/* matrix for weights		*/
	double *weights0 = NULL;	/* matrix for weights		*/
	double *weights1 = NULL;	/* matrix for weights		*/
	double learningRates[MAX_NUMOFPREDS], th, mu, muMax;
	double sumAbsErrors = 0;
	double meanAbsDiff = 0.;
	unsigned int countAbsErrors=0;
	unsigned int countAbsDiff = 0;
	unsigned int dist, minDist;
	unsigned int runModePenalty = 0; /* for run mode */
	unsigned int templateMatchingPenalty = 0; /* for template matching	*/
	unsigned long Mepsilon = 0;
	unsigned long numOfAbsSamples, numOfAbsSamples0, numOfAbsSamples1, searchSpace2;
	unsigned long numOfSpaceSamples, numOfSpace0Samples, numOfSpace1Samples;
	unsigned long pos, py;
	PicData *result = NULL;
	//FILE *out=stdout;
	/* for debugging	*/
	unsigned long countMatch[3];
	unsigned long countMatchSuccess[3];
	unsigned long countEqual[3];
	unsigned long countEqualSuccess[3];

	if (searchSpace < MIN_LS_SEARCHSPACE) searchSpace = MIN_LS_SEARCHSPACE; /* safe guard	*/
	if (searchSpace > MAX_LS_SEARCHSPACE) searchSpace = MAX_LS_SEARCHSPACE; /* safe guard	*/
	if (numOfPreds > MAX_NUMOFPREDS) numOfPreds = MAX_NUMOFPREDS; /* safe guard	*/
	if (numOfPreds < 1) numOfPreds = 1; /* safe guard	*/

	searchSpace2 = searchSpace * searchSpace;

	result = CreatePicData(pd->width, pd->height, pd->channel, 
		pd->bitperchannel, pd->bitperchan);

	/* determine maximal offsets	*/
	dxMax = dyMax = 0;
	for ( p = 1; p <= numOfPreds; p++)
	{
		if (dxMax < abs( dx[p])) dxMax = abs( dx[p]);
		if (dyMax < abs( dy[p])) dyMax = abs( dy[p]);
	}
	dyMin = searchSpace;
	for ( p = 0; p < numOfPredsCol; p++)
	{
		if (dyMax < abs( dyC[p])) dyMax = abs( dyC[p]);
		if (dyMin > dyC[p]) dyMin = dyC[p];
	}
	for ( p = 0; p < numOfPreds; p++)
	{
		learningRates[p] = 0.0001F;
		learningRates[p] = 0.00005F;
		learningRates[p] = 0.0002F;
	}
	//learningRates[0] = learningRates[1] = learningRates[2] = 0.0004F;
	//learningRates[3] = learningRates[4] = 0.0002F;

	/* space for a copy of the actual search space including prediction region	*/
	searchSpaceWidth = 2*searchSpace+1 + 2*dxMax;
	searchSpaceHeight = searchSpace + 1 + dyMax - dyMin;
	CALLOC( space, searchSpaceHeight * searchSpaceWidth, double);
	if (pd->channel > 1)
	{	/* only for colour images	*/
		CALLOC( space0, searchSpaceHeight * searchSpaceWidth, double);
		CALLOC( space1, searchSpaceHeight * searchSpaceWidth, double);
	}
	/* space for data	*/
	errSpaceWidth = (2*searchSpace+1);
	searchSpaceSize =  (searchSpace+1) * errSpaceWidth;

	weights = vector( numOfPreds);	/* weights dependent on position */
	weights0 = vector( numOfPredsCol);	/* weights dependent on position */
	weights1 = vector( numOfPredsCol);	/* weights dependent on position */

	if (!encode_flag)
	{
		/* for all colour components (channels) */
		for (chan = 0; chan < pd->channel; chan++)
		{
				/* copy  original component when in decoding mode */
				memcpy( result->data[chan], pd->data[chan], pd->size * sizeof(uint));	
				pd->offset[chan] = 0; /* overwrite old offset as it becomes obsolete here	*/
		}
	}
	/* for all colour components (channels) */
	for (chan = 0; chan < pd->channel; chan++)
	{
		countMatch[chan] = countMatchSuccess[chan] = 0;
		countEqual[chan] = countEqualSuccess[chan] = 0;

		/* parameter for MED predictor	*/
		long RANGE = (pd->maxVal_rct[chan] + 1) ;
		long RANGEd2 = RANGE >> 1;
		long RANGEm1d2 = (pd->maxVal_rct[chan]) >> 1;

		range = (1 << pd->bitperchan[chan]);
		offset = range >> 1;

		muMax = 6. / (RANGEd2 * RANGEd2);
		//muMax = 4. / (RANGEd2 * RANGEd2);
		learningRates[0] = muMax;

		if (encode_flag)
		{
			pd->offset[chan] = offset; /* overwrite old offset as it becomes obsolete here	*/
		}

		/* reset prediction parameters for each component	*/
		for ( p = 0; p < numOfPreds; p++)
		{
			weights[p] = 0.0F;
		}
		// weights[0] = weights[1] = 0.5F; /* xHat = (A + B) / 2	*/
		weights[0] = 0.7F; /* xHat = 0.7* A 	*/
		for ( p = 0; p < numOfPredsCol; p++)
		{
			weights0[p] = 0.0F;
		}
		for ( p = 0; p < numOfPredsCol; p++)
		{
			weights1[p] = 0.0F;
		}
		runModePenalty = 0;
		templateMatchingPenalty = 0;
		countAbsErrors = 0;
		sumAbsErrors = 0;

		for (y = 0, py = 0; y < (signed)pd->height; y++, py += pd->width)
		{
			for (x = 0, pos = py; x < (signed)pd->width; x++, pos++)
			{				
				if (x == 0 && y == 0)
				{
					xHatd = xHat = RANGEd2;	
				}
				else 
				{/* do the least mean squares prediction */
					//if (x == 82 &&  y== 25)
					//{
					//	int z = 9;
					//}

					/* copy search space data plus predictor data
					** duplictate values for non-existing data at image borders
					** ### full copy is not required, can be simplified later ######
					*/
					meanOfSamples = meanOfSamples0 = meanOfSamples1 = 0.;
					numOfSpaceSamples = numOfSpace0Samples = numOfSpace1Samples = 0;
					for (n = dyMin; n <= searchSpace + dyMax; n++)
					{
						for (m = -searchSpace-dxMax; m <= searchSpace + dxMax; m++)
						{
							int yn = y - n; /* source coordinates	*/
							int xm = x + m;
							int mc, nc;		/* corrected source coordinates	*/
							long posS2;

							/* target position of actual sample starting from bottom line	*/
							posS2 = searchSpace+dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;
							mc = m; nc = n;
							/* correction of positions if nrcessary	*/
							if (xm < 0) 	/* beyond left image border	*/
							{
								mc -= xm;
							}
							else if (xm > (signed)pd->width - 1) /* beyond right image border	*/
							{
								mc -= xm - (pd->width - 1);
							}
							if (yn < 0) /* beyond top image border	*/
							{
								nc += yn;
							}
							else if (yn > (signed)pd->height - 1) /* beyond bottom image border	*/
							{
								nc += yn - (pd->height - 1);
							}


							/* check, whether position is available	*/
							if (n < 0 || (n == 0 && m >= 0) )  /* last line, data not processed yet	*/
							{
								space[posS2] = 1 << 30;
							}
							else 
							{
								if (nc == 0 && mc >= 0)
								{ /* do not access unknown data	in bottom line of space */
									//mc = -1;
									if (x)
										space[posS2] = pd->data[chan][pos -1 - nc * pd->width];
									else /* there is no left neighbour, take above one	*/
										space[posS2] = pd->data[chan][pos  - (nc+1) * pd->width];
								}
								else space[posS2] = pd->data[chan][pos + mc - nc * pd->width];
								meanOfSamples += space[posS2];
								numOfSpaceSamples++;
							}

							if ( chan > 0)
							{	/* copy values from first component	*/
								space0[posS2] = pd->data[0][pos + mc - nc * pd->width];
								meanOfSamples0 += space0[posS2];
								numOfSpace0Samples++;
							}
							if ( chan > 1)
							{	/* copy values from first component	*/
								space1[posS2] = pd->data[1][pos + mc - nc * pd->width];
								meanOfSamples1 += space1[posS2];
								numOfSpace1Samples++;
							}
						}
					} /* for (n = */
					meanOfSamples /= numOfSpaceSamples;
					if ( chan > 0)
					{	/* copy values from first component	*/
						meanOfSamples0 /= numOfSpace0Samples;
					}
					if ( chan > 1)
					{	/* copy values from first component	*/
						meanOfSamples1 /= numOfSpace1Samples;
					}
#ifdef _DEBUG_XX
if( chan == 2 && y == 278 && x == 20)
{
	FILE *file=NULL;
	file = fopen( "space1.txt", "wt");
	for (n = dyMin; n <= searchSpace + dyMax; n++)
	{
		for (m = -searchSpace-dxMax; m <= searchSpace + dxMax; m++)
		{
			int yn = y - n; /* source coordinates	*/
			int xm = x + m;
			int mc, nc;		/* corrected source coordinates	*/
			long posS;

			/* target position of actual sample starting from bottom line	*/
			posS = searchSpace+dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;
			fprintf( file, " %3.0f", space1[posS]);
		}
			fprintf( file, "\n");
	}
	fclose( file);
}
#endif
#ifdef MEANFREE
					/* subtract mean from all samples in search space	*/
					meanOfAbsSamples = meanOfAbsSamples0 = meanOfAbsSamples1 = 0;
					numOfAbsSamples = numOfAbsSamples0 = numOfAbsSamples1 = 0;
					for (n = dyMin; n <= searchSpace + dyMax; n++)
					{
						long n2 = n*n;
						for (m = -searchSpace-dxMax; m <= searchSpace + dxMax; m++)
						{
							long posS2;

							/* position of actual target sample starting from bottom line	*/
							posS2 = searchSpace+dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;
							if ( chan > 0)
							{	/* copy values from first component	*/
								space0[posS2] -= meanOfSamples0;
							}
							if ( chan > 1)
							{	/* copy values from first component	*/
								space1[posS2] -= meanOfSamples1;
							}
							if (n < 0 || (n == 0 && m >= 0))
							{
								; /* data not processed yet	*/
							}
							else
								space[posS2] -= meanOfSamples;

							if (n2 + m*m - 1 > (signed)searchSpace2) continue; /* use half circle search space	*/
							//if (n + m - 1 > searchSpace) continue; /* use triangled search space	*/
							if (n < 0 || (n == 0 && m >= 0))
							{
								; /* data not processed yet	*/
							}
							else
							{
								numOfAbsSamples++;
								meanOfAbsSamples += fabs( space[posS2]);
							}
							if ( chan > 0)
							{	/* copy values from first component	*/
								meanOfAbsSamples0 += fabs( space0[posS2]);
								numOfAbsSamples0++;
							}
							if ( chan > 1)
							{	/* copy values from first component	*/
								meanOfAbsSamples1 += fabs( space1[posS2]);
								numOfAbsSamples1++;
							}
						}
					}
					meanOfAbsSamples /= numOfAbsSamples;
					meanOfAbsSamples0 /= numOfAbsSamples;
					meanOfAbsSamples1 /= numOfAbsSamples;
#endif
					equal_flag = 0;
					matchFound_flag = 0;
					if (templateMatching_flag) /* no run mode in first row or first column	*/
					{
						/* target position of actual sample starting at bottom line	*/
						long	posS2= searchSpace+dxMax + (searchSpace + dyMax) * searchSpaceWidth;
						pattern[0] = space[posS2-1];
						pattern[1] = space[posS2-1-searchSpaceWidth];
						pattern[2] = space[posS2  -searchSpaceWidth];
						pattern[3] = space[posS2+1-searchSpaceWidth];
						if (dxMax > 1 && dyMax > 1)
						{ /* avoid access outside the space	*/
							pattern[4] = space[posS2-2];
							pattern[5] = space[posS2  -2*searchSpaceWidth];
						}
						else pattern[4] = pattern[5] = pattern[0];
						if ( chan > 0)
						{
							pattern0[0] = space0[posS2-1];
							pattern0[1] = space0[posS2-1-searchSpaceWidth];
							pattern0[2] = space0[posS2  -searchSpaceWidth];
							pattern0[3] = space0[posS2+1-searchSpaceWidth];
						}
						if ( chan > 1)
						{
							pattern1[0] = space1[posS2-1];
							pattern1[1] = space1[posS2-1-searchSpaceWidth];
							pattern1[2] = space1[posS2  -searchSpaceWidth];
							pattern1[3] = space1[posS2+1-searchSpaceWidth];
						}
						if (y > 0 && x > 0) /* no run mode in first row or first column	*/
						{ /* check, whether all neighbours are identical	*/
							equal_flag = 1; /* assume yes	*/
							if (pattern[0] != pattern[1] ||
									pattern[0] != pattern[2] ||
									pattern[0] != pattern[4] ||
									pattern[0] != pattern[5] ) /* do not look to the right	*/
									equal_flag = 0; /* no, its different	*/
							// check components separately as this can be beneficial after RCT
							//if ( chan > 0)
							//{
							//	if (pattern0[0] != pattern0[1] ||
							//			pattern0[0] != pattern0[2] ) /* do not look to the right	*/
							//			equal_flag = 0; /* no, its different	*/
							//}
							//if ( chan > 1)
							//{
							//	if (pattern1[0] != pattern1[1] ||
							//			pattern1[0] != pattern1[2] ) /* do not look to the right	*/
							//			equal_flag = 0; /* no, its different	*/
							//}
						}
					}
					if (equal_flag)
					{ /* save run estimate	*/
						xHatdRM = pattern[0];
						countEqual[chan]++;
					}
					if (!equal_flag ||  runModePenalty > 0)
					{ /* do LMS or TM and NOT run mode	*/
						/* update LMS coefficients/weights based on local samples in search space	*/
						minDist = searchSpace + searchSpace; /* for template-matching mode	*/
						for (n = searchSpace; n >= 0; n--)
						{ /* include new column of values	*/
							long n2 = n*n;
							if ( n > (signed)y) continue; /* do not use virtual lines	*/
							for (m = -searchSpace; m <= searchSpace; m++)
							{ /* include new column of values	*/

								if (n == 0 && m >= 0) continue; /* data not processed yet	*/

								//if (n + m - 1 > searchSpace) continue; /* use triangled search space	*/
								if (n2 + m*m - 1 > (signed)searchSpace2) continue; /* use half circle search space	*/

								/* target position of actual sample starting from bottom line	*/
								posS = searchSpace + dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;

								/* compare actual pattern with pattern in search space, but not
								** in first row or first column
								** do not enter again, when the neighbouring pattern is identical keep its
								** estimate!
								*/
								if (templateMatching_flag && y > 0 && x > 0)
								{	
									double diffSum = 0.;
									dist= n + abs(m);
									diffSum += fabs(pattern[0] - space[posS-1]);
									diffSum += fabs(pattern[1] - space[posS-1-searchSpaceWidth]);
									diffSum += fabs(pattern[2] - space[posS  -searchSpaceWidth]);
									diffSum += fabs(pattern[3] - space[posS+1-searchSpaceWidth]);
									if (dxMax > 1 && dyMax > 1)
									{ /* avoid access outside the space	*/
										diffSum += fabs(pattern[4] - space[posS-2]);
										diffSum += fabs(pattern[5] - space[posS  -2*searchSpaceWidth]);
									}
									// check components separately as this can be beneficial after RCT
									//if ( chan > 0)
									//{
									//	diffSum += fabs(pattern0[0] - space0[posS-1]);
									//	diffSum += fabs(pattern0[1] - space0[posS-1-searchSpaceWidth]);
									//	diffSum += fabs(pattern0[2] - space0[posS  -searchSpaceWidth]);
									//	diffSum += fabs(pattern0[3] - space0[posS+1-searchSpaceWidth]);
									//}
									//if ( chan > 1)
									//{
									//	diffSum += fabs(pattern1[0] - space1[posS-1]);
									//	diffSum += fabs(pattern1[1] - space1[posS-1-searchSpaceWidth]);
									//	diffSum += fabs(pattern1[2] - space1[posS  -searchSpaceWidth]);
									//	diffSum += fabs(pattern1[3] - space1[posS+1-searchSpaceWidth]);
									//}
									if (diffSum == 0. && dist <= minDist)
									{	/* if pattern are identical and this position is at least close as the previous
										** closer patterns are preferred!	
										*/
										matchFound_flag = 1;	/* patterns are idential	*/
										minDist = dist;	/* remember shorter distance	*/
										xHatdTM = space[posS];	/* take value at position X'	*/
										
										if (dist == 1)
										{/* equal to left or top neighbour	*/
											templateMatchingPenalty = 0; 
										}
									}
								} /* if (templateMatching_flag)*/

								/* redo the prediction at this position and the updating of weights	*/
								xHatd = 0.;
//if (y == 29 && x == 39 && chan == 0) printf( "\n ");
								for ( p = 0; p < numOfPreds; p++)
								{
									long posP;
									if (y+dy[p+1] < 0 || x + dx[p+1] < 0) continue; /* do not use pixels outside	*/
									posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
									xHatd += space[posP] * weights[p];
//if (y == 29 && x == 39 && chan == 0) 
//printf( "\np=%d, space=%5.2f, w=%5.3f", p, space[posP], weights[p]);
								}
								if ( chan > 0)
								{
									for ( p = 0; p < numOfPredsCol; p++)
									{
										long posP;
										if (y+dyC[p] < 0 || x + dxC[p] < 0) continue; /* do not use pixels outside	*/
										posP = posS + dxC[p] + dyC[p] * searchSpaceWidth;
										xHatd += space0[posP] * weights0[p];
									}
								}
								if ( chan > 1)
								{
									for ( p = 0; p < numOfPredsCol; p++)
									{
										long posP;
										if (y+dyC[p] < 0 || x + dxC[p] < 0) continue; /* do not use pixels outside	*/
										posP = posS + dxC[p] + dyC[p] * searchSpaceWidth;
										xHatd += space1[posP] * weights1[p];
									}
								}

								error = space[posS] - xHatd;

								/* update average absolute error	*/
								absError = (int)fabs( error);
								sumAbsErrors += absError;
								countAbsErrors++;
								if (countAbsErrors > pd->width)
								{
									countAbsErrors >>= 1;
									sumAbsErrors /= 2;
								}

								/* update threshold	*/

								if (countAbsErrors > 1) /* exlude first error value	*/
									th = 2.0F * sumAbsErrors / countAbsErrors;
								else
									th = 10; /* default threshold in the beginning	*/

								/* limitation of adaptation	*/
								if (error < 0) error = max( error, -th);
								else error = min( error, th);

								mu = learningRates[0];
//if (y < 1 && chan == 0) 
//printf( "\nmu=%7.5f, mean=%7.5f, meanAbs=%7.5f", mu, meanOfSamples, meanOfAbsSamples);
								if (error != 0)
								{
									for ( p = 0; p < numOfPreds; p++)
									{
										long posP;
										if (y+dy[p+1] < 0 || x + dx[p+1] < 0) continue; /* do not use pixels outside	*/
										posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
										//if (space[posP] != 0)
										{
#ifdef MEANFREE
											double delta = mu * error * space[posP] / (meanOfAbsSamples + 1);/* avoid division by zero!	*/
#else
											double delta = mu * error * space[posP] / (meanOfSamples + 1);/* avoid division by zero!	*/
#endif
											weights[p] += delta;
										}
									}
									if ( chan > 0)
									{
										for ( p = 0; p < numOfPredsCol; p++)
										{
											long posP;
											if (y+dyC[p] < 0 || x + dxC[p] < 0) continue; /* do not use pixels outside	*/
											posP = posS + dxC[p] + dyC[p] * searchSpaceWidth;
#ifdef MEANFREE
											double delta = mu * error * space0[posP] / (meanOfAbsSamples0 + 1);/* avoid division by zero!	*/
#else
											double delta = mu * error * space0[posP] / (meanOfSamples0 + 1);/* avoid division by zero!	*/
#endif
											weights0[p] += delta;
										}
									}
									if ( chan > 1)
									{
										for ( p = 0; p < numOfPredsCol; p++)
										{
											long posP;
											if (y+dyC[p] < 0 || x + dxC[p] < 0) continue; /* do not use pixels outside	*/
											posP = posS + dxC[p] + dyC[p] * searchSpaceWidth;
#ifdef MEANFREE
											double delta = mu * error * space1[posP] / (meanOfAbsSamples1 + 1);/* avoid division by zero!	*/
#else
											double delta = mu * error * space1[posP] / (meanOfSamples1 + 1);/* avoid division by zero!	*/
#endif
											weights1[p] += delta;
										}
									}
								}
							} /* for m	*/
						} /* for n	*/
					} /* do the LMS or TM */

					if (matchFound_flag) countMatch[chan]++;
					if (templateMatching_flag && matchFound_flag && templateMatchingPenalty == 0)
					{	/* TM detected 	*/
						xHatd = xHatdTM;
					}
					else if (templateMatching_flag && equal_flag && runModePenalty == 0)
					{	/* run mode detected */
						xHatd = xHatdRM;
					}
					else
					{
						/* do the prediction based on current weights					*/
						xHatd = 0.;
						posS = searchSpace+dxMax + (searchSpace + dyMax) * searchSpaceWidth;
						for ( p = 0; p < numOfPreds; p++)
						{
							long posP;
							if ( (y+dy[p+1]) < 0 || (x + dx[p+1]) < 0) continue; /* do not use pixels outside	*/
							posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
							xHatd += space[posP] * weights[p];
//if (y < 1 && chan == 0) 
//printf( "\np=%d, space=%5.2f, w=%5.3f", p, space[posP], weights[p]);
						}
						if ( chan > 0)
						{
							for ( p = 0; p < numOfPredsCol; p++)
							{
								long posP;
								if ( (y+dyC[p]) < 0 || (x + dxC[p]) < 0) continue; /* do not use pixels outside	*/
								posP = posS + dxC[p] + dyC[p] * searchSpaceWidth;
								xHatd += space0[posP] * weights0[p];
							}
						}
						if ( chan > 0)
						{
							for ( p = 0; p < numOfPredsCol; p++)
							{
								long posP;
								if ( (y+dyC[p]) < 0 || (x + dxC[p]) < 0) continue; /* do not use pixels outside	*/
								posP = posS + dxC[p] + dyC[p] * searchSpaceWidth;
								xHatd += space1[posP] * weights1[p];
							}
						}
					} /* if !run mode ! TM	*/

#ifdef MEANFREE
					xHatd = CLIP( xHatd + meanOfSamples, 0, (long)pd->maxVal_rct[chan]);
#else
					xHatd = CLIP( xHatd, 0, (long)pd->maxVal_rct[chan]);
#endif
					xHat = (long)floor( xHatd + 0.5);
				}/* do the least-mean squares prediction */



				if (encode_flag)
				{
					Rx = (long)*GetXYValue( pd, chan, x, y);

					epsilon = Rx - xHat;

					/* Bound Rx. */		
					Rx = CLIP( Rx, 0, (int)pd->maxVal_rct[chan]);

					/* Praediktionsfehler modulo	*/
					epsilon = MODULO_RANGE( epsilon, -RANGEd2,
						RANGEm1d2, RANGE)

						/* ...und in den positiven Bereich verschieben */    	
						// orig: 
						// Mepsilon = epsilon + (range >> 1);	    	
						Mepsilon = epsilon + offset;	    	

					/* Schätzfehler einsetzen */		    	
					*GetXYValue( result, chan, x, y) = (uint)Mepsilon;		    			    	
				}
				else /* decoder */
				{
					/* get prediction-error value	*/
					Mepsilon = (unsigned long)*GetXYValue( result, chan, x, y);
					// epsilon = Mepsilon - (range >> 1);	    	
					epsilon = Mepsilon - offset;	    	
					/* reconstruction of pixel value Rx	*/
					Rx = xHat + epsilon;

					Rx = MODULO_RANGE( Rx, 0,	(int)pd->maxVal_rct[chan], RANGE)

						/* bound Rx */
						Rx = CLIP( Rx, 0, (int)pd->maxVal_rct[chan]);

					/* write reconstructed value */
					*GetXYValue( pd, chan, x, y) = (uint)Rx;		    			    	
				}


				/* first pixel, do not update anything	*/
				if ((signed)x == 0 && (signed)y == 0)
				{
					continue;
				}
if(0 && chan==0)
{
printf( "\n x,y= %4d,%4d", x, y);
printf( ", mean = %4.3f", meanOfSamples);
printf( ", meanAbs = %4.3f", meanOfAbsSamples);
printf( ", xHatd = %4.1f", xHatd);
printf( ", e%d, m%d", equal_flag, matchFound_flag);
	for ( p = 0; p < numOfPreds; p++)
	{
		printf( ", %4.3f", weights[p]);
	}
}
				if (!equal_flag && !matchFound_flag)
				{
					double diff = Rx - xHatd;

					if (fabs( diff) < 0.5 * meanAbsDiff)
					{	/* faster adaptation	*/
						learningRates[0] = (learningRates[0]*3 + min( muMax, 0.0005 / (diff * diff + 1))) / 4.;
					}
					else
					{
						learningRates[0] = (learningRates[0]*15 + min( muMax, 0.0005 / (diff * diff + 1))) / 16.;
					}
					meanAbsDiff = (meanAbsDiff * countAbsDiff + fabs( diff));
					countAbsDiff++;
					meanAbsDiff /= countAbsDiff;
					if (countAbsDiff > pd->width)
					{
						countAbsDiff >>= 1;
					}
				}

				if (templateMatching_flag && equal_flag)
				{/* if match does not yield a perfect estimate then look for more matches next time	*/
#ifdef MEANFREE
					xHatdRM += meanOfSamples;
#endif
					if (Rx - xHatdRM != 0)
					{
						if (runModePenalty < (unsigned)searchSpaceWidth) 
							runModePenalty++;
					}
					else
					{
						countEqualSuccess[chan]++;
						if (runModePenalty > 0) runModePenalty--;
					}
				}
				if (templateMatching_flag && matchFound_flag)
				{/* if match does not yield a perfect estimate then look for more matches next time	*/
#ifdef MEANFREE
					xHatdTM += meanOfSamples;
#endif
					if (Rx - xHatdTM != 0)
					{
						if (templateMatchingPenalty < (unsigned)searchSpaceWidth) 
							templateMatchingPenalty++;
					}
					else 
					{
						countMatchSuccess[chan]++;
						if (templateMatchingPenalty > 0) templateMatchingPenalty--;
					}
				}
			}	/* x */
			fprintf( stderr, "\r%d: %5d/%5d", chan, y, pd->height);
		}	/* y */	
		/* copy error erray in original component */
	}		/* chan*/
	for( chan = 0; chan < pd->channel; chan++)
	{
		if (encode_flag)
			memcpy(pd->data[chan], result->data[chan], pd->size * sizeof(uint));	
	}		/* chan*/

#ifdef _DEBUGXX
	{
		FILE *file=NULL;

		file = fopen( "absErr.pgm", "wt");
		for ( y= 0; y < pd->height; y++)
		{
			for( x=0; x < pd->width; x++)
			{
				fprintf( file, " %3d", errors[x+y*pd->width]);
			}
			fprintf( file, "\n");
		}
		fclose(file);
	}
#endif
	//fprintf( stderr, "\nfinal mu: %f", learningRates[0]);

	for( chan = 0; chan < pd->channel; chan++)
	{
		printf( "\n%d Run Mode: %d detected, %d successful", chan, countEqual[chan], countEqualSuccess[chan]);
		printf( "\n%d  TM Mode: %d detected, %d successful", chan, countMatch[chan], countMatchSuccess[chan]);
	}

	/* Zeichenfläche löschen */
	DeletePicData(result);	
	FREE( space);
	if (pd->channel > 1)
	{	/* only for colour images	*/
		FREE( space0);
		FREE( space1);
	}
	free_vector( &weights);
	free_vector( &weights0);
	free_vector( &weights1);
}

