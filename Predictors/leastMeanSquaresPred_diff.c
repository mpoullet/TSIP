/*****************************************************
* File..: leastMeanSquaresPred.c
* Desc..: LMS prediction
* Author: Tilo Strutz
* Date..: 21.08.2017
* changes
* 24.11.2017 adaptive learning rate
* 01.12.2017 differential prediction
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


/*---------------------------------------------------------------
*	medPredictor()
*---------------------------------------------------------------*/
MYINLINE void medPredictor( PicData *pd, int chan, 
													 long *xHat, int x, int y, long RANGEd2)
{
	unsigned long maxAC, minAC;
	static long d1, d2, d3;

	long Ra, Rb, Rc, Rd;

	/*  C B D
	*  
	*/

	if (y == 0)
	{
		Rb = 1 << (pd->bitperchan[chan]-1);
		Rc = 1 << (pd->bitperchan[chan]-1);
		Rd = 1 << (pd->bitperchan[chan]-1);
		if (x == 0) Ra = RANGEd2;
		else Ra = (int)*GetXYValue( pd, chan, x-1u, y);
		/* does not work since maxVal might be different in encoder and decoder
		Rb = (pd->maxVal[chan] + 1) >> 1;
		Rc = (pd->maxVal[chan] + 1) >> 1;
		Rd = (pd->maxVal[chan] + 1) >> 1;
		if (x == 0) Ra = (pd->maxVal[chan] + 1) >> 1;
		else Ra = (int)*GetXYValue( pd, chan, x-1u, y);
		*/
	}
	else 
	{
		Rb = (long)*GetXYValue( pd, chan, x, y-1u);
		if (x == 0)
		{
			Ra = Rb;
			Rc = Rb;
			Rd = (long)*GetXYValue( pd, chan, x+1u, y-1u);
		}
		else
		{
			Ra = (long)*GetXYValue( pd, chan, x-1u, y);
			Rc = (long)*GetXYValue( pd, chan, x-1u, y-1u);
			if (x == (signed)pd->width - 1) Rd = Rb;
			Rd = (long)*GetXYValue( pd, chan, x+1u, y-1u);
		}
	}


	/* Predict the next value. */
	maxAC = max( Ra, Rb); 
	minAC = min( Ra, Rb);

	if (Rc >= (long)maxAC)	*xHat = minAC;
	else if (Rc <= (long)minAC)	*xHat = maxAC;
	else			*xHat = Ra + Rb - Rc;
}

/*--------------------------------------------------------------
* lmsPrediction() 
*-------------------------------------------------------------*/
void lmsPrediction( PicData *pd, int numOfPreds, int searchSpace,
									 int encode_flag)
{
	int x, y;
	unsigned char *err_ptr=NULL;
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
	/*                           X  2  2  0  1  0  1  2  5  5  3  4  5  6  7 11 11  8  9 */
	int rx[MAX_NUMOFPREDS+1] = { 0,-1,-1,-1, 0,-1, 0,-1, 0, 0, 1,-2, 0,-2,-1, 0, 0, 1, 2};
	int ry[MAX_NUMOFPREDS+1] = { 0,-1,-1, 0,-1, 0,-1,-1,-2,-2,-1, 0,-2,-1,-2,-3,-3,-2,-1};
	/*
	** differences
	**     >`´ < 
	**   > >`´ < <
	** > > > < < < <
	** > > ^ x
	*/
	int dxMax, dyMax;
	int p; /* predictor number*/
	int templateMatching_flag = 1;	/* enable template matching*/
	int matchFound_flag = 0;				/* a match has been found */
	int useTemplateMatching_flag = 0;	/* template matching is actually used */
	int equal_flag = 0;	/* all neighbours are equal, enables run mode	*/
	long epsilon, xHat=0; /* clamped estimation error, final estimate	*/
	long range, offset;
	long 	Rx, n, m;
	long posS;	/* current position of target value in search space	*/
	int searchSpaceWidth, searchSpaceHeight, errSpaceWidth, searchSpaceSize;
	/* weights decreasing with increasing distance from current position inside search space	*/
	double posWeigthLUT09[MAX_LS_SEARCHSPACE*2+1] = { 0, 1.00, 0.90, 0.81, 0.73, 0.66, 0.59, 0.53, 0.48, 0.43, 0.39, 
		0.35, 0.31, 0.28, 0.25, 0.23, 0.21, 0.19, 0.17, 0.15, 0.14, 
		0.12, 0.11, 0.10, 0.09, 0.08, 0.07, 0.06, 0.06, 0.05, 0.05, 
		0.04, 0.04, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02}; /*(0.9^n)*/
	double *space=NULL;
	double error;	/* prediction error	*/
	double xHatd = 0., xHatdTM=0, xHatdRM=0; /* intermediate estimates		*/
	double pattern[6]; /* A C B D E F	*/
	double meanOfSamples = 0.; /* mean of samples in search space; avoid compiler warning */
#ifdef MEANFREE
	double meanOfAbsSamples;	/* mean of absolute samples in search space */
	unsigned long numOfAbsSamples;
#endif
	double *weights = NULL;	/* matrix for weights		*/
	double learningRate /*s[MAX_NUMOFPREDS]*/;
	double sumAbsErrors;
	double meanAbsDiff;
	double meanAbsDelta;
	unsigned int absError;	/* absolute rounded estimation error	*/
	unsigned int meanAbsErrors; /* mean absolut estimation error	*/
	unsigned int xAbsDiff; /* difference between horizontal neighbours	*/
	//unsigned int countAbsDiff;
	unsigned int dist, minDist;
	unsigned int runModePenalty = 0; /* for run mode */
	unsigned int templateMatchingPenalty = 0; /* for template matching	*/
	unsigned long Mepsilon = 0; /* symbol to be encoded	*/
	unsigned long numOfSpaceSamples, searchSpace2;
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
	CALLOC( err_ptr, pd->width * pd->height, unsigned char);

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
	CALLOC( space, searchSpaceHeight * searchSpaceWidth, double);
	/* space for data	*/
	errSpaceWidth = (2*searchSpace+1);
	searchSpaceSize =  (searchSpace+1) * errSpaceWidth;

	weights = vector( numOfPreds);	/* weights dependent on position */

	/* for all colour components (channels) do prediction  */
	for (chan = 0; chan < pd->channel; chan++)
	{
		countMatch[chan] = countMatchSuccess[chan] = 0;/* for debugging	*/
		countEqual[chan] = countEqualSuccess[chan] = 0;/* for debugging	*/
		/* parameter for MED predictor	*/
		long RANGE = (pd->maxVal_rct[chan] + 1) ;
		long RANGEd2 = RANGE >> 1;
		long RANGEm1d2 = (pd->maxVal_rct[chan]) >> 1;

		range = (1 << pd->bitperchan[chan]);
		offset = range >> 1;

		learningRate = 6. / (RANGEd2 * RANGEd2); /* initial value	*/

		if (!encode_flag)
		{
			/* copy  original component when in decoding mode */
			memcpy( result->data[chan], pd->data[chan], pd->size * sizeof(uint));	
			pd->offset[chan] = 0; /* overwrite old offset as it becomes obsolete here	*/
		}
		else
		{
			pd->offset[chan] = offset; /* overwrite old offset as it becomes obsolete here	*/
		}

		/* reset prediction parameters for each component	*/
		for ( p = 0; p < numOfPreds; p++)
		{
			weights[p] = 0.0F; /* start with zeros, when differential treatment	*/
		}
		//weights[0] = weights[1] = 0.5F; /* xHat = (A + B) / 2	*/
		//weights[0] = 0.7F; /* xHat = 0.7* A 	*/
		runModePenalty = 0;
		templateMatchingPenalty = 0;
		//countAbsErrors = 0;
		sumAbsErrors = 0;
		meanAbsDiff = 0.;
		//countAbsDiff = 0;
		meanAbsErrors = 1;
		xAbsDiff = 10; /* difference between horizontal neighbours	*/
		meanAbsDelta = 1.;

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

					/* copy search space data plus predictor data
					** duplictate values for non-existing data at image borders
					** ### full copy is not required, can be simplified later ######
					*/
				if (x == 5 && y == 9)
				{
					int z=0;;	
				}
					meanOfSamples = 0.;
					numOfSpaceSamples = 0;
					for (n = 0; n <= searchSpace + dyMax; n++)
					{
						for (m = -searchSpace-dxMax; m <= searchSpace + dxMax; m++)
						{
							int yn = y - n; /* source coordinates	*/
							int xm = x + m;
							int mc, nc;		/* corrected source coordinates	*/
							long posS;

							/* target position of actual sample starting from bottom line	*/
							posS= searchSpace+dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;

							/* check, whether position is available	*/
							if (n == 0 && m >= 0)  /* last line, data not processed yet	*/
							{
								space[posS] = 1 << 30;
								continue;
							}
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
							if (nc == 0 && mc >= 0)
							{ /* do not access unknown data	in current image line (bottom line of space) */
								if (x) /* take left neighbour instead	*/
									space[posS] = pd->data[chan][pos -1 - nc * pd->width];
								else /* there is no left neighbour, take above one	*/
									space[posS] = pd->data[chan][pos  - (nc+1) * pd->width];
							}
							else
								space[posS] = pd->data[chan][pos + mc - nc * pd->width];
#ifdef MEANFREE
							meanOfSamples += space[posS];
							numOfSpaceSamples++;
#endif
						}
					}
#ifdef MEANFREE
					meanOfSamples /= numOfSpaceSamples;
					/* subtract mean from all samples in search space	*/
					meanOfAbsSamples = 0;
					numOfAbsSamples = 0;
					for (n = 0; n <= searchSpace + dyMax; n++)
					{
						long n2 = n*n;
						for (m = -searchSpace-dxMax; m <= searchSpace + dxMax; m++)
						{
							long posS;

							if (n == 0 && m >= 0) continue; /* data not processed yet	*/
							/* position of actual target sample starting from bottom line	*/
							posS = searchSpace+dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;
							space[posS] -= meanOfSamples;
							if (n2 + m*m - 1 > (signed)searchSpace2) continue; /* use half circle search space	*/
							//if (n + m - 1 > searchSpace) continue; /* use triangled search space	*/
							numOfAbsSamples++;
							meanOfAbsSamples += fabs( space[posS]);
						}
					}
					meanOfAbsSamples /= numOfAbsSamples;
#endif
					equal_flag = 0;
					matchFound_flag = 0;
					useTemplateMatching_flag = 0;
					if (templateMatching_flag) /* no run mode in first row or first column	*/
					{
						/* target position of actual sample starting at bottom line	*/
						long	posS= searchSpace+dxMax + (searchSpace + dyMax) * searchSpaceWidth;
						pattern[0] = space[posS-1];
						pattern[1] = space[posS-1-searchSpaceWidth];
						pattern[2] = space[posS  -searchSpaceWidth];
						pattern[3] = space[posS+1-searchSpaceWidth];
						if (dxMax > 1 && dyMax > 1)
						{ /* avoid access outside the space	*/
							pattern[4] = space[posS-2];
							pattern[5] = space[posS  -2*searchSpaceWidth];
						}
						else pattern[4] = pattern[5] = pattern[0];
						if (y > 0 && x > 0) /* no run mode in first row or first column	*/
						{ /* check, whether all neighbours are identical	*/
							equal_flag = 1; /* assume yes	*/
							if (pattern[0] != pattern[1] ||
									pattern[0] != pattern[2] /* ||
									pattern[0] != pattern[3]*/ ) /* do not look to the right	*/
									equal_flag = 0; /* no, its different	*/
							if (pattern[0] != pattern[4] ||
									pattern[0] != pattern[5]) equal_flag = 0;
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
						for (n = searchSpace; n >= 0; n--) /* start in top row of search space	*/
						//for (n = 2*dyMax; n >= 0; n--) /* start in top row of search space	*/
						{ /* include new column of values	*/
							long n2 = n*n;
							if ( n > (signed)y) continue; /* do not use virtual lines	*/
							for (m = -searchSpace; m <= searchSpace; m++)
							//for (m = -dxMax; m <= dxMax; m++)
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
									if (diffSum == 0. && dist <= minDist)
									{	/* if pattern are identical and this position is at least close as the previous
										** closer patterns are preferred!	
										*/
										matchFound_flag = 1;	/* patterns are idential	*/
										minDist = dist;	/* remember shorter distance	*/
										xHatdTM = space[posS];	/* take value at position X'	*/
										
										if (dist == 1)
										{/* equal to left or top neighbour	*/
											useTemplateMatching_flag = 1; /* equal to left ortop neighbour	*/
										}
										if (templateMatchingPenalty == 0 || useTemplateMatching_flag == 1)
										{ /* early stop, if xHat will be estimated based on template matching*/
											useTemplateMatching_flag = 1;
											m = searchSpace + 1; /* force exit of loop	*/
											n = - 1; /* force exit of loop	*/
											continue;
										}
									}
								} /* if (templateMatching_flag)*/

								/* redo the prediction at this position and the updating of weights	*/
								xHatd = space[posS-1]; /* initialise with left neighbour	*/
								for ( p = 0; p < numOfPreds; p++) /* for all neighbours in template	*/
								{
									long posP, posR;
									if (y+dy[p+1] < 0 || x + dx[p+1] < 0) continue; /* do not use pixels outside	*/
									/* position	of sub-predictor	*/
									posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
									/* reference position	for differential treatment	*/
									posR = posS + rx[p+1] + ry[p+1] * searchSpaceWidth; 
									xHatd += (space[posP] - space[posR]) * weights[p];
								}
								error = space[posS] - xHatd; /* prediction error	*/

								/* limitation of prediction error value	*/
								if (error > 2 * meanAbsErrors)
									error = 2.0F * meanAbsErrors;
								else if (error < -(signed)(2 * meanAbsErrors))
									error = -2.0F * meanAbsErrors;

								/* update average absolute error	*/
								absError = (unsigned int)fabs( error);
								if (absError > meanAbsErrors)
								{ /* if higher than usual=> fast update	*/
									meanAbsErrors = (meanAbsErrors + absError) >> 1;
								}
								else
								{ /* if lower than usual=> slow update	*/
									meanAbsErrors = (31 * meanAbsErrors + absError + 16) >> 5;
								}
								/* high errors have bigger impact on performance, 
								** so adapt faster to them as to smaller errors
								*/

							  //xAbsDiff = (7 * xAbsDiff +  abs( space[posS] - space[posS-1]) ) >> 3;
								/* recursive adaptation of absolute difference between neighbouring x-samples	*/
							  xAbsDiff = (unsigned long)((7 * xAbsDiff +  fabs( space[posS] - space[posS-1]) ) * 0.125);
								if (xAbsDiff == 0) xAbsDiff = 1;
								//if (error != 0)
								{
									double delta[MAX_NUMOFPREDS];
									double deltaSum, deltaFac;

									/* normalisation by the mean absolute signal-differencec, 
									** by the mean absolute prediction errors, and
									** by the mean absolute sum of correction terms	
									** max(learningRate): 0.02
									*/
									learningRate = 0.01 / (0.5 + 100 * xAbsDiff * meanAbsErrors * meanAbsDelta);
									learningRate = 0.005 / (0.5 + 100 * xAbsDiff * meanAbsErrors * meanAbsDelta);
							//learningRate = 0.00012;
									//if (n < searchSpace >> 1) learningRate *= 0.5;
									deltaFac = learningRate * error;
									deltaSum = 0;
									for ( p = 0; p < numOfPreds; p++)
									{
										long posP, posR;
										if (y+dy[p+1] < 0 || x + dx[p+1] < 0) continue; /* do not use pixels outside	*/
										posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
										posR = posS + rx[p+1] + ry[p+1] * searchSpaceWidth; /* reference position	*/
										/* correction term dependent on the sample difference	*/
										delta[p] = deltaFac * (space[posP] - space[posR]); 
										deltaSum += delta[p]; /* sum of correction terms	*/
										weights[p] += delta[p]; /* update the weight	*/
										if (weights[10] > 0.1)
										{
											int z=9;
										}
									}
									/* update the mean absolute sum of correction terms		*/
									meanAbsDelta = ( 63 * meanAbsDelta + fabs(deltaSum)) * 0.015625;
								} /* if (error != 0)*/
							} /* for m	*/
						} /* for n	*/
					} /* do the LMS or TM */

					if (matchFound_flag) countMatch[chan]++; /* for debugging	*/
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
						posS = searchSpace+dxMax + (searchSpace + dyMax) * searchSpaceWidth;
						xHatd = space[posS-1]; /* initialise by left neighbour	*/
						for ( p = 0; p < numOfPreds; p++)
						{
							long posP, posR;
							if ( (y+dy[p+1]) < 0 || (x + dx[p+1]) < 0) continue; /* do not use pixels outside	*/
							posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
							posR = posS + rx[p+1] + ry[p+1] * searchSpaceWidth; /* reference position	*/
							xHatd += (space[posP] - space[posR]) * weights[p];
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
					err_ptr[x + y * pd->width] = (unsigned char)max( 0, min( 255, epsilon+128));
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
#ifdef MEANFREE
printf( ", meanAbs = %4.3f", meanOfAbsSamples);
#endif
printf( ", xHatd = %4.1f", xHatd);
printf( ", e%d, m%d", equal_flag, matchFound_flag);
	for ( p = 0; p < numOfPreds; p++)
	{
		printf( ", %4.3f", weights[p]);
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
		if (encode_flag)
			memcpy(pd->data[chan], result->data[chan], pd->size * sizeof(uint));	
		fprintf( stderr, "\n final mu: %f\n", learningRate);
		x=0;
	}		/* chan*/

#ifdef _DEBUGXX
	{
		FILE *file=NULL;

		file = fopen( "absErr.pgm", "wt");
		fprintf( file, "P2\n%d %d\n255", pd->width, pd->height);
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

	for( chan = 0; chan < pd->channel; chan++)
	{
		fprintf( stderr, "\n%d Run Mode: %d detected, %d successful", chan, countEqual[chan], countEqualSuccess[chan]);
		fprintf( stderr, "\n%d  TM Mode: %d detected, %d successful", chan, countMatch[chan], countMatchSuccess[chan]);
	}
	{
		FILE *out = fopen( "error.pgm", "wt");
		fprintf( out, "P2\n#LMS simple\n%d %d", pd->width, pd->height);
		fprintf( out, "\n%d\n", pd->maxVal_orig[0]);
		for ( y = 0; y < (signed)pd->height; y++)
		{
			for ( x = 0; x < (signed)pd->width; x++)
			{
				fprintf( out, "%3d ", err_ptr[x+y*pd->width]);
			}
			fprintf( out, "\n");
		}
		fclose( out);
	}
	/* Zeichenfläche löschen */
	DeletePicData(result);	
	FREE( space);
	FREE( err_ptr);
	free_vector( &weights);
}

