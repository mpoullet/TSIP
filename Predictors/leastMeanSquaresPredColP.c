/*****************************************************
* File..: leastMeanSquaresPredColP.c
* Desc..: LMS prediction across colour components
* Author: Tilo Strutz
* Date..: 06.09.2017
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
* lmsPredictionColP() 
*-------------------------------------------------------------*/
void lmsPredictionColP( PicData *pd, int numOfPreds, int searchSpace,
											 int encode_flag)
{
	int x, y;
	unsigned int chan;
	int numOfPredsCol;
	/*     141115
	**   13 7 5 816
	** 12 6 2 1 3 917
	** 10 4 0 X
	**
	**                          X  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17
	*/
	int dx[MAX_NUMOFPREDS+1] = {0,-1, 0,-1, 1,-2, 0,-2,-1, 1, 2,-3, 0,-3,-2,-1, 1, 2, 3};
	int dy[MAX_NUMOFPREDS+1] = {0, 0,-1,-1,-1, 0,-2,-1,-2,-2,-1, 0,-3,-1,-2,-3,-3,-2,-1};
	int dxMax, dyMax;
	int p; /* predictor number*/
	int templateMatching_flag = 1, matchFound_flag = 0;
	int equal_flag[3];
	int absError;
	int searchSpaceWidth, searchSpaceHeight, errSpaceWidth, searchSpaceSize;
	long 	epsilon, xHat=0;
	long offset[3];
	long 	Rx, n, m;
	long posS;	/* current position of target value in search space	*/
	/* parameter for MED predictor	*/
	long RANGE[4];
	long RANGEd2[4];
	long RANGEm1d2[4];
	double *space[3]; /* search space for three components	*/ 
	double error, xHatd = 0., xHatdTM=0, xHatdRM=0;
	double pattern[3][6]; /* A C B D E F	*/
	double meanOfSamples[3];
	double meanOfAbsSamples[3];
	double weights[3][3][MAX_NUMOFPREDS+1];	/* [component to be predicted]
																					** [direction to component]
																					**[predictor position]		*/
	double learningRates[3] = { 1, 1, 1 }, th, mu, muMax[3];
	double sumAbsErrors[3];
	double meanAbsDiff[3];
	unsigned int countAbsErrors[3];
	unsigned int countAbsDiff[3];
	unsigned int dist, minDist;
	unsigned int runModePenalty[3]; /* for run mode */
	unsigned int templateMatchingPenalty[3]; /* for template matching	*/
	unsigned long Mepsilon = 0;
	unsigned long numOfAbsSamples, searchSpace2;
	unsigned long numOfSpaceSamples;
	unsigned long pos, py;
	PicData *result = NULL;
	//FILE *out=stdout;
	/* for debugging	*/
	unsigned long countMatch[3];
	unsigned long countMatchSuccess[3];
	unsigned long countEqual[3];
	unsigned long countEqualSuccess[3];

	if (searchSpace < MIN_LS_SEARCHSPACE) searchSpace = MIN_LS_SEARCHSPACE;/*safe guard*/
	if (searchSpace > MAX_LS_SEARCHSPACE) searchSpace = MAX_LS_SEARCHSPACE;/*safe guard*/
	if (numOfPreds > MAX_NUMOFPREDS) numOfPreds = MAX_NUMOFPREDS; /* safe guard	*/
	if (numOfPreds < 1) numOfPreds = 1; /* safe guard	*/

	numOfPredsCol = max( 1, numOfPreds-6); /* predictors for cross-component	*/
	numOfPredsCol = 4;

	searchSpace2 = searchSpace * searchSpace;

	/* create structure and allocate memory for prediction result	*/
	result = CreatePicData(pd->width, pd->height, pd->channel, 
		pd->bitperchannel, pd->bitperchan);

	/* determine maximal offsets	*/
	dxMax = dyMax = 0;
	for ( p = 1; p <= numOfPreds; p++)
	{
		if (dxMax < abs( dx[p])) dxMax = abs( dx[p]);
		if (dyMax < abs( dy[p])) dyMax = abs( dy[p]);
	}

	/* space for a copy of the actual search space including prediction region	*/
	searchSpaceWidth = 2*searchSpace+1 + 2*dxMax;
	searchSpaceHeight = searchSpace + 1 + dyMax;

	/* space for data	*/
	errSpaceWidth = (2*searchSpace+1);
	searchSpaceSize =  (searchSpace+1) * errSpaceWidth;

	/* for each component, 3 times numOfPreds+1 weights are required	*/
	//weights = matrix( 9, numOfPreds+1);	/* weights dependent on position including current */

	if (!encode_flag)
	{
		/* for all colour components (channels) */
		for (chan = 0; chan < pd->channel; chan++)
		{
			/* copy  original component when in decoding mode */
			memcpy( result->data[chan], pd->data[chan], pd->size * sizeof(uint));	
			/* overwrite old offset as it becomes obsolete here	*/
			pd->offset[chan] = 0; 
		}
		fprintf( stderr, "\n");
	}
	for (chan = 0; chan < pd->channel; chan++)
	{
		CALLOC( space[chan], searchSpaceHeight * searchSpaceWidth, double);
		RANGE[chan] = (pd->maxVal_rct[chan] + 1) ;
		RANGEd2[chan] = RANGE[chan] >> 1;
		RANGEm1d2[chan] = (pd->maxVal_rct[chan]) >> 1;

		offset[chan] = (1 << pd->bitperchan[chan]) >> 1;

		muMax[chan] = 6. / (RANGEd2[chan] * RANGEd2[chan]);
		//muMax = 4. / (RANGEd2 * RANGEd2);
		learningRates[chan] = muMax[chan];
		meanAbsDiff[chan] = 0.;
		countAbsDiff[chan] = 0;
		countAbsErrors[chan] = 0;
		sumAbsErrors[chan] = 0;

		/* reset prediction parameters for each component	*/
		for ( p = 0; p <= numOfPreds; p++)
		{
			weights[chan][0][p] = 0.0F;
			weights[chan][1][p] = 0.0F;
			weights[chan][2][p] = 0.0F;
		}
		countMatch[chan] = countMatchSuccess[chan] = 0;
		countEqual[chan] = countEqualSuccess[chan] = 0;
		equal_flag[chan] = 0; /* no run mode (equal values)	*/
		runModePenalty[chan] = 0;
		templateMatchingPenalty[chan] = 0;

		if (encode_flag)
		{
			pd->offset[chan] = offset[chan]; /* overwrite old offset as it becomes obsolete here	*/
		}
	}
	weights[0][0][1] = 0.7F; /* xHat = 0.7* A, left neighbour for first line	*/
	weights[1][1][1] = 0.7F; /* */
	weights[2][2][1] = 0.7F; /* 	*/

	/* for all pixels in the image	*/
	for (y = 0, py = 0; y < (signed)pd->height; y++, py += pd->width)
	{
		for (x = 0, pos = py; x < (signed)pd->width; x++, pos++)
		{				

			if (x > 0 || y > 0) /* in first position, nothing can be copied	*/
			{ 
				/* copy search space data plus predictor data for all components
				** duplictate values for non-existing data at image borders
				** ### full copy is not required, can be simplified later ######
				*/
				for (chan = 0; chan < pd->channel; chan++)
				{
					meanOfSamples[chan] = 0;
				}
				numOfSpaceSamples = 0;
				for (n = 0; n <= searchSpace + dyMax; n++)
				{
					for (m = -searchSpace-dxMax; m <= searchSpace + dxMax; m++)
					{
						int yn = y - n; /* source coordinates	*/
						int xm = x + m;
						int mc, nc;		/* corrected source coordinates	*/
						long posS2;

						/* target position of actual sample starting from bottom line	*/
						posS2 = searchSpace+dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;
						mc = m; nc = n; /* assume that no correction is required	*/
						/* correction of positions if necessary	*/
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
						if ( (n == 0 && m >= 0) )  /* last line, data not processed yet	*/
						{
							for (chan = 0; chan < pd->channel; chan++)
							{ /* set to dummy value for better debugging	*/
								space[chan][posS2] = 1 << 30;
							}
						}
						else  /* for prediction of chan1,2 the value at current position */
						{			/* from chan=0,1 could be integrated! */
							if (nc == 0 && mc >= 0)
							{ /* do not access unknown data	in bottom line of space */
								//mc = -1;
								for (chan = 0; chan < pd->channel; chan++)
								{
									if (x) /* take left neighbour instead	*/
										space[chan][posS2] = pd->data[chan][pos -1 - nc * pd->width];
									else /* there is no left neighbour, take above one	*/
										space[chan][posS2] = pd->data[chan][pos  - (nc+1) * pd->width];
									meanOfSamples[chan] += space[chan][posS2];
								}
							}
							else /* copy image data into space	*/
							{
								for (chan = 0; chan < pd->channel; chan++)
								{
									space[chan][posS2] = pd->data[chan][pos + mc - nc * pd->width];
									meanOfSamples[chan] += space[chan][posS2];
								}
							}
							numOfSpaceSamples++;
						}
					}/* for (m = */
				} /* for (n = */
				for (chan = 0; chan < pd->channel; chan++)
				{
					meanOfSamples[chan] /= numOfSpaceSamples;
				}
#ifdef _DEBUG_XX
				if( chan == 2 && y == 23 && x == 40)
				{
					FILE *file=NULL;
					file = fopen( "space1.txt", "wt");
					for (n = 0; n <= searchSpace + dyMax; n++)
					{
						for (m = -searchSpace-dxMax; m <= searchSpace + dxMax; m++)
						{
							int yn = y - n; /* source coordinates	*/
							int xm = x + m;
							int mc, nc;		/* corrected source coordinates	*/
							long posS;

							/* target position of actual sample starting from bottom line	*/
							posS = searchSpace+dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;
							fprintf( file, " %3.0f %3.0f %3.0f ", space0[posS], space1[posS], space[posS]);
						}
						fprintf( file, "\n");
					}
					fclose( file);
				}
#endif
#ifdef MEANFREE
				/* subtract mean from all samples in search space	*/
				for (chan = 0; chan < pd->channel; chan++)
				{
					meanOfAbsSamples[chan] = 0;
				}
				numOfAbsSamples = 0;
				for (n = 0; n <= searchSpace + dyMax; n++)
				{
					long n2 = n*n;
					for (m = -searchSpace-dxMax; m <= searchSpace + dxMax; m++)
					{
						long posS2;

						if ( (n == 0 && m >= 0)) continue;
						/* position of actual target sample starting from bottom line	*/
						posS2 = searchSpace+dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;
						for (chan = 0; chan < pd->channel; chan++)
						{
							space[chan][posS2] -= meanOfSamples[chan];
						}

						if (n2 + m*m - 1 > (signed)searchSpace2) continue; /* use half circle search space	*/
						//if (n + m - 1 > searchSpace) continue; /* use triangled search space	*/

						numOfAbsSamples++;
						for (chan = 0; chan < pd->channel; chan++)
						{
							meanOfAbsSamples[chan] += fabs( space[chan][posS2]);
						}
					}
				}
				for (chan = 0; chan < pd->channel; chan++)
				{
					meanOfAbsSamples[chan] /= numOfAbsSamples;
				}
#endif
				/* for all colour components copy and check pattern */
				if (templateMatching_flag)
				{
					posS= searchSpace+dxMax + (searchSpace + dyMax) * searchSpaceWidth;
					for (chan = 0; chan < pd->channel; chan++)
					{ /* copy pattern in any case as it is used also for TM	*/
						/* target position of actual sample starting at bottom line	*/
						pattern[chan][0] = space[chan][posS-1];
						pattern[chan][1] = space[chan][posS-1-searchSpaceWidth];
						pattern[chan][2] = space[chan][posS  -searchSpaceWidth];
						pattern[chan][3] = space[chan][posS+1-searchSpaceWidth];
						if (dxMax > 1 && dyMax > 1)
						{ /* avoid access outside the space	*/
							pattern[chan][4] = space[chan][posS-2];
							pattern[chan][5] = space[chan][posS  -2*searchSpaceWidth];
						}
						else pattern[chan][4] = pattern[chan][5] = pattern[chan][0];

						if (y > 0 && x > 0) /* no run mode in first row or first column	*/
						{ /* check, whether all neighbours are identical	*/
							equal_flag[chan] = 1; /* assume run mode (equal values)	*/
							if (pattern[chan][0] != pattern[chan][1] ||
								pattern[chan][0] != pattern[chan][2] ||
								pattern[chan][0] != pattern[chan][4] ||
								pattern[chan][0] != pattern[chan][5] ) /* do not look to the right	*/
								equal_flag[chan] = 0; /* no, its different	*/
						}
						else equal_flag[chan] = 0;
					} /* for chan	*/
				} /* if (templateMatching_flag)*/
			} /* 	if (x > 0 || y > 0) 		*/

			/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			** for all colour components (channels): do prediction and coding 
			*/
			for (chan = 0; chan < pd->channel; chan++)
			{
				if (x == 0 && y == 0)
				{
					xHatd = xHat = RANGEd2[chan];	
				}
				else 
				{/* do the least mean squares prediction */

					if (chan > 0)
					{ /* include current value from previous channel	*/
						posS = searchSpace + dxMax + (searchSpace + dyMax) * searchSpaceWidth;
						space[0][posS] = pd->data[0][pos];
#ifdef MEANFREE
						space[0][posS] -= meanOfSamples[0];
#endif
						if (chan > 1)
						{ /* include current value from previous channel	*/
							space[1][posS] = pd->data[1][pos];
#ifdef MEANFREE
							space[1][posS] -= meanOfSamples[1];
#endif
						}
					}
					if (equal_flag[chan])
					{ /* save run estimate	*/
						xHatdRM = pattern[chan][0];
						countEqual[chan]++;
					}
					if (!equal_flag[chan] ||  runModePenalty[chan] > 0)
					{ /* do LMS or TM and NOT run mode	*/
						/* train LMS coefficients/weights based on local samples in search space*/
						minDist = searchSpace + searchSpace; /* for template-matching mode	*/
						matchFound_flag = 0; /* reset template matching flag	*/

						/* scan entire search space	*/
						for (n = searchSpace; n >= 0; n--)
						{ /* start at highest distance from actual position	*/
							long n2 = n*n;
							if ( n > (signed)y) continue; /* do not use virtual lines	*/
							for (m = -searchSpace; m <= searchSpace; m++)
							{
								if (n == 0 && m >= 0) continue; /* data not processed yet	*/

								//if (n + m - 1 > searchSpace) continue; /* use triangled search space	*/
								if (n2 + m*m - 1 > (signed)searchSpace2) continue; /* use half circle search space	*/

								/* target position of actual sample starting from bottom line	*/
								posS = searchSpace + dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;

								/* compare actual pattern with pattern in search space, but not
								** in first row or first column
								*/
								if (templateMatching_flag && y > 0 && x > 0)
								{	
									double diffSum = 0.;
									dist= n + abs(m);
									diffSum += fabs(pattern[chan][0] - space[chan][posS-1]);
									diffSum += fabs(pattern[chan][1] - space[chan][posS-1-searchSpaceWidth]);
									diffSum += fabs(pattern[chan][2] - space[chan][posS  -searchSpaceWidth]);
									diffSum += fabs(pattern[chan][3] - space[chan][posS+1-searchSpaceWidth]);
									if (dxMax > 1 && dyMax > 1)
									{ /* avoid access outside the space	*/
										diffSum += fabs(pattern[chan][4] - space[chan][posS-2]);
										diffSum += fabs(pattern[chan][5] - space[chan][posS  -2*searchSpaceWidth]);
									}
									/* ??? should we check also the pattern in other channels	???? including current position if available? */

									if (diffSum == 0. && dist <= minDist)
									{	/* if pattern are identical and this position is at least close as 
										** the previous; closer patterns are preferred!	
										*/
										matchFound_flag = 1;	/* patterns are idential	*/
										minDist = dist;	/* remember shorter distance	*/
										xHatdTM = space[chan][posS];	/* take value at position X'	*/

										if (dist == 1)
										{/* equal to left or top neighbour: trust this match!	*/
											templateMatchingPenalty[chan] = 0; 
										}
									}
								} /* if (templateMatching_flag)*/

								/* redo the prediction at this position and the updating of weights	*/
								xHatd = 0.;
//if (y == 29 && x == 39 && chan == 0) printf( "\n ");
								for ( p = 1; p <= numOfPreds; p++) 
								{/* ignore current position as this value s not known	*/
									long posP;
									if (y + dy[p] < 0 || x + dx[p] < 0) continue;
									/* do not use pixels outside	*/
									posP = posS + dx[p] + dy[p] * searchSpaceWidth;
									xHatd += space[chan][posP] * weights[chan][chan][p];
//if (y == 29 && x == 39 && chan == 0) 
//printf( "\np=%d, space=%5.2f, w=%5.3f", p, space[chan][posP], weights[chan][chan][p]);
								}
								if (chan > 0 && pd->channel == 3)/********************/
								{
									int comp0, comp1;
									for ( p = 1; p <= numOfPredsCol; p++)  /* could be one position more!*/
									{
										long posP;
										if (y + dy[p] < 0 || x + dx[p] < 0) continue; 
										/* do not use pixels outside	*/
										comp0 = (chan+1) % 3; /* get indices of other two components	*/
										comp1 = (chan+2) % 3;
										posP = posS + dx[p] + dy[p] * searchSpaceWidth;
										xHatd += space[comp0][posP] * weights[chan][comp0][p];
										xHatd += space[comp1][posP] * weights[chan][comp1][p];
									}
								}
								if (chan > 0) xHatd += space[0][posS] * weights[chan][0][0];
								if (chan > 1) xHatd += space[1][posS] * weights[chan][1][0];

								error = space[chan][posS] - xHatd;
								/* update average absolute error	*/
								absError = (int)fabs( error);
								sumAbsErrors[chan] += absError;
								countAbsErrors[chan]++;
								if (countAbsErrors[chan] > pd->width)
								{
									countAbsErrors[chan] >>= 1;
									sumAbsErrors[chan] /= 2;
								}

								/* update threshold	*/

								if (countAbsErrors[chan] > 1) /* exlude first error value	*/
									th = 2.0F * sumAbsErrors[chan] / countAbsErrors[chan];
								else
									th = 10; /* default threshold in the beginning	*/

								/* limitation of adaptation	*/
								if (error < 0) error = max( error, -th);
								else error = min( error, th);

								mu = learningRates[chan];
//if (y < 1 && chan == 0) 
//printf( "\nmu=%7.5f, mean=%7.5f, meanAbs=%7.5f", mu, meanOfSamples[chan], meanOfAbsSamples[chan]);
								if (error != 0)
								{
									for ( p = 1; p <= numOfPreds; p++)
									{
										long posP;
										if (y + dy[p] < 0 || x + dx[p] < 0) continue; 
										/* do not use pixels outside	*/
										posP = posS + dx[p] + dy[p] * searchSpaceWidth;
#ifdef MEANFREE
										double delta = mu * error * space[chan][posP] / 
																	(meanOfAbsSamples[chan] + 1);/* avoid division by zero!	*/
#else
										double delta = mu * error * space[chan][posP] / 
																	(meanOfSamples[chan] + 1);/* avoid division by zero!	*/
#endif
										weights[chan][chan][p] += delta;
									}
									if (chan > 0 && pd->channel == 3)/********************/
									{
										int comp0, comp1;
										for ( p = 1; p <= numOfPredsCol; p++) /* could be one position more!	*/
										{
											long posP;
											if (y + dy[p] < 0 || x + dx[p] < 0) continue; 
											/* do not use pixels outside	*/
											posP = posS + dx[p] + dy[p] * searchSpaceWidth;
											comp0 = (chan+1) % 3; /* get indices of other two components	*/
											comp1 = (chan+2) % 3;
#ifdef MEANFREE
											double delta0 = mu * error * space[comp0][posP] / 
																			(meanOfAbsSamples[comp0] + 1);/* avoid division by zero!	*/
											double delta1 = mu * error * space[comp1][posP] / 
																			(meanOfAbsSamples[comp1] + 1);/* avoid division by zero!	*/
#else
											double delta0 = mu * error * space[comp0][posP] / 
																			(meanOfSamples[comp0] + 1);/* avoid division by zero!	*/
											double delta1 = mu * error * space[comp1][posP] / 
																			(meanOfSamples[comp1] + 1);/* avoid division by zero!	*/
#endif
											weights[chan][comp0][p] += delta0;
											weights[chan][comp1][p] += delta1;
										}
									}
									if (chan > 0)
									{
#ifdef MEANFREE
											double delta = mu * error * space[0][posS] / 
																			(meanOfAbsSamples[0] + 1);/* avoid division by zero!	*/
#else
											double delta = mu * error * space[0][posP] / 
																			(meanOfSamples[0] + 1);/* avoid division by zero!	*/
#endif
											weights[chan][0][0] += delta;
									}
									if (chan > 1)
									{
#ifdef MEANFREE
											double delta = mu * error * space[1][posS] / 
																			(meanOfAbsSamples[1] + 1);/* avoid division by zero!	*/
#else
											double delta = mu * error * space[1][posP] / 
																			(meanOfSamples[1] + 1);/* avoid division by zero!	*/
#endif
											weights[chan][1][0] += delta;
									}
								} /* if (error != 0)*/
							} /* for m	*/
						} /* for n	*/
					} /* do the LMS or TM */

					if (matchFound_flag) countMatch[chan]++;
					if (templateMatching_flag && matchFound_flag && templateMatchingPenalty[chan] == 0)
					{	/* TM detected 	*/
						xHatd = xHatdTM;
					}
					else if (templateMatching_flag && equal_flag[chan] && runModePenalty[chan] == 0)
					{	/* run mode detected */
						xHatd = xHatdRM;
					}
					else
					{
						/* do the prediction based on current weights					*/
						xHatd = 0.;
						posS = searchSpace+dxMax + (searchSpace + dyMax) * searchSpaceWidth;
						for ( p = 1; p <= numOfPreds; p++)
						{
							long posP;
							if ( (y + dy[p]) < 0 || (x + dx[p]) < 0) continue; /* do not use pixels outside	*/
							posP = posS + dx[p] + dy[p] * searchSpaceWidth;
							xHatd += space[chan][posP] * weights[chan][chan][p];
//if (y < 1 && chan == 0) 
//printf( "\np=%d, space=%5.2f, w=%5.3f", p, space[chan][posP], weights[chan][chan][p]);
						}
						if (chan > 0 && pd->channel == 3)/********************/
						{
							int comp0, comp1;
							for ( p = 1; p <= numOfPredsCol; p++) /* could be one position more!	*/
							{
								long posP;
								if (y + dy[p] < 0 || x + dx[p] < 0) continue; 
								/* do not use pixels outside	*/
								comp0 = (chan+1) % 3; /* get indices of other two components	*/
								comp1 = (chan+2) % 3;
								posP = posS + dx[p] + dy[p] * searchSpaceWidth;
								xHatd += space[comp0][posP] * weights[chan][comp0][p];
								xHatd += space[comp1][posP] * weights[chan][comp1][p];
							}
						}
						if (chan > 0) xHatd += space[0][posS] * weights[chan][0][0];
						if (chan > 1) xHatd += space[1][posS] * weights[chan][1][0];
					} /* if !run mode ! TM	*/

#ifdef MEANFREE
					xHatd = CLIP( xHatd + meanOfSamples[chan], 0, (long)pd->maxVal_rct[chan]);
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
					epsilon = MODULO_RANGE( epsilon, -RANGEd2[chan],
						RANGEm1d2[chan], RANGE[chan])

						/* ...und in den positiven Bereich verschieben */    	
						// orig: 
						// Mepsilon = epsilon + (range >> 1);	    	
						Mepsilon = epsilon + offset[chan];	    	

					/* Schätzfehler einsetzen */		    	
					*GetXYValue( result, chan, x, y) = (uint)Mepsilon;		    			    	
				}
				else /* decoder */
				{
					/* get prediction-error value	*/
					Mepsilon = (unsigned long)*GetXYValue( result, chan, x, y);
					// epsilon = Mepsilon - (range >> 1);	    	
					epsilon = Mepsilon - offset[chan];	    	
					/* reconstruction of pixel value Rx	*/
					Rx = xHat + epsilon;

					Rx = MODULO_RANGE( Rx, 0,	(int)pd->maxVal_rct[chan], RANGE[chan])

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
					printf( ", mean = %4.3f", meanOfSamples[chan]);
					printf( ", meanAbs = %4.3f", meanOfAbsSamples[chan]);
					printf( ", xHatd = %4.1f", xHatd);
					printf( ", e%d, m%d", equal_flag[chan], matchFound_flag);
					for ( p = 0; p < numOfPreds; p++)
					{
						printf( ", %4.3f", weights[chan][0][p]);
					}
				}
				if (!equal_flag[chan] && !matchFound_flag)
				{
					double diff = Rx - xHatd;

					if (fabs( diff) < 0.5 * meanAbsDiff[chan])
					{	/* faster adaptation	*/
						learningRates[chan] = (learningRates[chan]*3 + min( muMax[chan], 0.0005 / 
																	(diff * diff + 1))) / 4.;
					}
					else
					{
						learningRates[chan] = (learningRates[chan]*15 + min( muMax[chan], 0.0005 / 
																	(diff * diff + 1))) / 16.;
					}
					meanAbsDiff[chan] = (meanAbsDiff[chan] * countAbsDiff[chan] + fabs( diff));
					countAbsDiff[chan]++;
					meanAbsDiff[chan] /= countAbsDiff[chan];
					if (countAbsDiff[chan] > pd->width)
					{
						countAbsDiff[chan] >>= 1;
					}
				}

				if (templateMatching_flag && equal_flag[chan])
				{/* if match does not yield a perfect estimate 
				 ** then put penalty on run mode
				 */
#ifdef MEANFREE
					xHatdRM += meanOfSamples[chan];
#endif
					if (Rx - xHatdRM != 0)
					{
						if (runModePenalty[chan] < (unsigned)searchSpaceWidth) 
							runModePenalty[chan]++;
					}
					else
					{
						countEqualSuccess[chan]++;
						if (runModePenalty[chan] > 0) runModePenalty[chan]--;
					}
				}
				if (templateMatching_flag && matchFound_flag)
				{/* if match does not yield a perfect estimate 
				 ** then put penalty on run mode
				 */
#ifdef MEANFREE
					xHatdTM += meanOfSamples[chan];
#endif
					if (Rx - xHatdTM != 0)
					{
						if (templateMatchingPenalty[chan] < (unsigned)searchSpaceWidth) 
							templateMatchingPenalty[chan]++;
					}
					else 
					{
						countMatchSuccess[chan]++;
						if (templateMatchingPenalty[chan] > 0) templateMatchingPenalty[chan]--;
					}
				}
			}/* for ( chan	*/
		}	/* x */
		fprintf( stderr, "\r %5d/%5d", y, pd->height);
	}	/* y */	
	/* copy error erray in original component */
	//	}		/* chan*/
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
	fprintf( stderr, "\nfinal mu's: %f %f %f\n", learningRates[0], learningRates[1], learningRates[2]);

	for( chan = 0; chan < pd->channel; chan++)
	{
		printf( "\n%d Run Mode: %d detected, %d successful", chan, countEqual[chan], countEqualSuccess[chan]);
		printf( "\n%d  TM Mode: %d detected, %d successful", chan, countMatch[chan], countMatchSuccess[chan]);
	}
	/* Zeichenfläche löschen */
	DeletePicData(result);	
	for (chan = 0; chan < pd->channel; chan++)
	{
		FREE( space[chan]);
		//free_matrix( &weights);
	}
}

