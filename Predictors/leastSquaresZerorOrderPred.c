/*****************************************************
* File..: leastSquaresZeroOrderPred.c
* Desc..: covariance-based blending of simple predictors
* Author: Tilo Strutz
* Date..: 13.10.2017
* chages:
******************************************************/
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
//#include "med.h" /* for max() and (min)*/
#include "ginterface.h"
#include "codec_CoBaLP.h" /* for CALLOC()*/
#include "matrix_utils.h"
//#include "prototypes.h"
#include "macros.h"  /* for TOL_S */

using namespace System::Runtime::InteropServices;

#include "leastSquaresPred.h"
#define MEANFREE
//
#undef MEANFREE /* seems to be equal or better	*/


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
* lsZeorOrderPrediction() 
*-------------------------------------------------------------*/
void lsZeroOrderPrediction( PicData *pd, int numOfPreds, int searchSpace,
														 int useCorrelation_flag,
														 int encode_flag)
{
	uint x, y, chan;
	/*     141115
	**   13 7 5 816
	** 12 6 2 1 3 917
	** 10 4 0 X
	**
	**                           X  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17
	*/
	int dx[MAX_NUMOFPREDS+1] = { 0,-1, 0,-1, 1,-2, 0,-2,-1, 1, 2,-3, 0,-3,-2,-1, 1, 2, 3};
	int dy[MAX_NUMOFPREDS+1] = { 0, 0,-1,-1,-1, 0,-2,-1,-2,-2,-1, 0,-3,-1,-2,-3,-3,-2,-1};
	int dxMax, dyMax;
	int i, p; /* predictor number*/
	int templateMatching_flag = 1, matchFound_flag = 0, useTemplateMatching_flag = 0;
	int	equal_flag = 0;
	long 	epsilon, xHat;
	long range, offset;
	long 	Rx, n, m;
	int searchSpaceWidth, searchSpaceHeight, errSpaceWidth;
	/* weights decreasing with increasing distance from current position inside search space	*/
	double posWeigthLUT09[MAX_LS_SEARCHSPACE*2+1] = { 0, 1.00, 0.90, 0.81, 0.73, 0.66, 0.59, 0.53, 0.48, 0.43, 0.39, 
		0.35, 0.31, 0.28, 0.25, 0.23, 0.21, 0.19, 0.17, 0.15, 0.14, 
		0.12, 0.11, 0.10, 0.09, 0.08, 0.07, 0.06, 0.06, 0.05, 0.05, 
		0.04, 0.04, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02}; /*(0.9^n)*/
	//double posWeigthLUT08[MAX_NUMOFPREDS+1] = { 0, 1.,.8,.64,.51,.41,.33,.26,.21,.17,.13,.11,
	//	.09,.07,.05,.04,.035,.028,.023}; /*(0.8^n)*/
	double *space=NULL;
	double xHatd = 0., xHatdTM=0, xHatdRM=0;
	double pattern[6]; /* A C B D E F	*/
	double meanOfSamples;
	double *errSpace[MAX_NUMOFPREDS];
//	double fac[MAX_NUMOFPREDS];
//#ifdef MEANFREE
//	double facMax = 1.7;
//	double facMin = -facMax;
//#else
//	double facMax = 1.7;
//	double facMin = 1./facMax;
//#endif
	//double sumProducts[MAX_NUMOFPREDS];
	//double sumXProducts[MAX_NUMOFPREDS];
	double coVar[MAX_NUMOFPREDS][MAX_NUMOFPREDS];
	double sumErrors[MAX_NUMOFPREDS];
	double sumOfWeights;
	double weight[MAX_NUMOFPREDS], xHats[MAX_NUMOFPREDS];
	double meanOfPreds[MAX_NUMOFPREDS];
	double sumWeights;
	double **weights = NULL;	/* matrix for weights		*/
	double **normal = NULL;	/* copy covariance matrix C	*/


	unsigned int dist, minDist;
	unsigned int runModePenalty = 0; /* for run mode */
	unsigned int templateMatchingPenalty = 0; /* for template matching	*/
	unsigned long Mepsilon = 0;
	unsigned long numOfSpaceSamples, searchSpace2;
	unsigned long numOfTargetSamples = 1; /* avoid compiler warning	*/
	unsigned long pos, py;
	PicData *result = NULL;	

	if (searchSpace < MIN_LS_SEARCHSPACE) searchSpace = MIN_LS_SEARCHSPACE; /* safe guard	*/
	if (searchSpace > MAX_LS_SEARCHSPACE) searchSpace = MAX_LS_SEARCHSPACE; /* safe guard	*/
	if (numOfPreds > MAX_NUMOFPREDS) numOfPreds = MAX_NUMOFPREDS; /* safe guard	*/
	if (numOfPreds < 1) numOfPreds = 1; /* safe guard	*/


	searchSpace2 = searchSpace * searchSpace;

	weights = matrix( numOfPreds, numOfPreds);	/* weights <= 1/covariances */
	normal = matrix( numOfPreds, numOfPreds);;	/* observations */

	/* create structure for the prediction error	*/
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
	CALLOC( space, searchSpaceHeight * searchSpaceWidth, double);
	/* space for prediction errors, one for each predictor
	** needed for computation of covariances
	*/
	errSpaceWidth = (2*searchSpace+1);
	for (p = 0; p < numOfPreds; p++) /* for each predictor	*/
	{
		CALLOC( errSpace[p], (searchSpace+1) * errSpaceWidth, double);
	}

	/* for all colour components (channels) separately */
	for (chan = 0; chan < pd->channel; chan++)
	{
		/* parameter for MED predictor	*/
		long RANGE = (pd->maxVal_rct[chan] + 1) ;
		long RANGEd2 = RANGE >> 1;
		long RANGEm1d2 = (pd->maxVal_rct[chan]) >> 1;

		range = (1 << pd->bitperchan[chan]);
		offset = range >> 1;

		if (!encode_flag)
		{
			/* copy received prediction errors into corresponding array when in decoding mode */
			memcpy( result->data[chan], pd->data[chan], pd->size * sizeof(uint));	
			pd->offset[chan] = 0; /* overwrite old offset as it becomes obsolete here	*/
		}
		else
		{
			pd->offset[chan] = offset; /*	*/
		}

		runModePenalty = 0;
		templateMatchingPenalty = 0;
		//countAbsErrors = 0;
		//sumAbsErrors = 0;

		for (y = 0, py = 0; y < pd->height; y++, py += pd->width)
		{	/* for all lines in this component	*/
			for (x = 0, pos = py; x < pd->width; x++, pos++)
			{	/* for all columns in this component	*/		
				if (x == 0 && y == 0)
				{ /* first pixel has a fixed estimation value	*/
					xHatd = xHat = RANGEd2;	
				}
				else 
				{/* do the least squares prediction */

					/* reset values	*/
					for ( i = 0; i < numOfPreds; i++)
					{
						//sumProducts[i] = 0;
						//sumXProducts[i] = 0;
						for ( p = 0; p < numOfPreds; p++)
						{
							coVar[i][p] = 0;
						}
					}

					/* copy search space data plus predictor data into separate array
					** duplictate values for non-existing data at image borders
					*/
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
							if (m >= 0 && n == 0)  /* last line, data not processed yet	*/
							{
								space[posS] = 1 << 30;
								continue;
							}
							mc = m; nc = n;
							/* correction of positions if nrcessary	*/
							if (xm < 0) 	/* beyond left image border	*/
							{
								mc -= xm;
								//if (x == 0) /* first column: take value above	*/
								//{
								//	nc++;
								//}
							}
							else if (xm > (signed)pd->width - 1) /* beyond right image border	*/
							{
								mc -= xm - (pd->width - 1);
								//if (x == pd->width - 1)/* last column: take value above	*/
								//{
								//	if (y == 0)  mc--; /* take value to the left	*/
								//}
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
#endif
							numOfSpaceSamples++;
						}
					}
#ifdef MEANFREE
					meanOfSamples /= numOfSpaceSamples;
					/* subtract mean from all samples in search space	*/
					for (n = 0; n <= searchSpace + dyMax; n++)
					{
						for (m = -searchSpace-dxMax; m <= searchSpace + dxMax; m++)
						{
							/* position of actual target sample starting from bottom line	*/
							long posS = searchSpace+dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;
							space[posS] -= meanOfSamples;
						}
					}
#endif
					/* process search space data	
					** firstly, check run mode 
					*/
					equal_flag = 0;
					matchFound_flag = 0;
					if (templateMatching_flag) /* if runMode and template matching are enabled	*/
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
						else pattern[4] = pattern[5] = pattern[0]; /* simulates equality	*/
						if (y > 0 && x > 0) /* no run mode in first row or first column	*/
						{ /* check, whether all neighbours are identical	*/
							equal_flag = 1; /* assume yes	*/
							if (pattern[0] != pattern[1] ||
									pattern[0] != pattern[2]  ||
									pattern[0] != pattern[4] ||
									pattern[0] != pattern[5]) /* do not look to the right	*/
									equal_flag = 0; /* no, its different	*/
						}
					}
					if (equal_flag) /* values are equal	*/
					{ /* save run estimate	*/
						xHatdRM = pattern[0];
					}
					if (!equal_flag || runModePenalty > 0)
					{ /* no run mode, check template matching	*/
						useTemplateMatching_flag = 0;
						minDist = searchSpace + searchSpace; /* for template-matching mode	*/
						for (n = 0; n <= searchSpace; n++)
						{ /* all lines in search space	*/
							long n2 = n*n;
							if ( n > (signed)y) continue; /* do not use virtual lines	*/
							for (m = -searchSpace; m <= searchSpace; m++)
							{ /* all columns in search space	*/
								long posS;	/* current position of target value	*/
								double posWeight;

								if (m >= 0 && n == 0) continue; /* data not processed yet	*/
								if (n2 + m*m - 1 > (signed)searchSpace2) continue; /* use half circle search space	*/

								/* position of actual target sample starting from bottom line	*/
								posS = searchSpace+dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;

								/* compare actual pattern with pattern in search space, but not
								** in first row or first column
								** do not enter again, when the neighbouring pattern is identical keep its
								** estimate!
								*/
								if (templateMatching_flag && y > 0 && x > 0) //Strutz y/x check could be earlier since only TM is cosidered here
								{
									double diffSum = 0.;
									diffSum += fabs(pattern[0] - space[posS-1]);
									diffSum += fabs(pattern[1] - space[posS-1-searchSpaceWidth]);
									diffSum += fabs(pattern[2] - space[posS  -searchSpaceWidth]);
									diffSum += fabs(pattern[3] - space[posS+1-searchSpaceWidth]);
									if (dxMax > 1 && dyMax > 1)
									{ /* avoid access outside the space	*/
										diffSum += fabs(pattern[4] - space[posS-2]);
										diffSum += fabs(pattern[5] - space[posS  -2*searchSpaceWidth]);
									}
									dist= n + abs(m); /* city-block distance between space pattern und actual pattern	*/
									if (diffSum == 0. && dist <= minDist)
									{	/* if patterns are identical and this position is at least close as the previous
										** closer patterns are preferred!	
										*/
										matchFound_flag = 1;
										minDist = dist;	/* remember shorter distance	*/
										xHatdTM = space[posS];
										if (dist == 1)
										{/* equal to left or top neighbour	*/
											useTemplateMatching_flag = 1; /* equal to left or top neighbour => early stop!	*/
											//templateMatchingPenalty = 0; 
										}
										if (templateMatchingPenalty == 0 || useTemplateMatching_flag == 1)
										{ /* early stop, if xHat will be estimated based on template matching*/
											useTemplateMatching_flag = 1;
											m = searchSpace + 1; /* force exit of loop	*/
											n = searchSpace + 1; /* force exit of loop	*/
											continue;
										}
									}
								}/* if (templateMatching_flag)*/

								posWeight = posWeigthLUT09[n + abs(m)];

								/* for all predictors					*/
								//for (p = 0; p < numOfPreds; p++)
								//{
								//	/* current position of predictor value relative to target sample	*/
								//	long posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
								//	sumProducts[p]  += space[posP] * space[posP]  * posWeight;
								//	sumXProducts[p] += space[posS] * space[posP]  * posWeight;
								//}
							} /* for m	*/
						}	/* for n	*/
					}/* do the TM */

					if (equal_flag && runModePenalty == 0)
					{	/*  use run mode */
						xHatd = xHatdRM;
					}
					else if (useTemplateMatching_flag)
					{	/* use TM mode 	*/
						xHatd = xHatdTM;
					}
					else 
					{
						/* for all predictors determine linear factors	*/
						//for (p = 0; p < numOfPreds; p++)
						//{
						//	if (sumProducts[p] == 0.) fac[p] = 1.;
						//	else		fac[p] = sumXProducts[p] / sumProducts[p];
						//	if (fac[p] > facMax) 
						//		fac[p] = facMax; /* safe guard	*/
						//	else if (fac[p] < facMin) 
						//		fac[p] = facMin;
						//}

						/* 
						** get errors and means
						*/
						for (p = 0; p < numOfPreds; p++)
						{
							sumErrors[p] = 0.;
						}
						for (n = 0; n <= searchSpace; n++)
						{
							long n2 = n*n;
							for (m = -searchSpace; m <= searchSpace; m++)
							{
								long posS = m+searchSpace+dxMax + (searchSpace + dyMax - n) * searchSpaceWidth;
								long posE = m+searchSpace + (searchSpace - n) * errSpaceWidth;
								if (m >= 0 && n == 0) continue; /* data not processed yet	*/
								if (n2 + m*m - 1 > (signed)searchSpace2) continue; /* use half circle search space	*/

								/* for all predictors							*/
								for (p = 0; p < numOfPreds; p++)
								{
									long posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
									double diff;

									diff = space[posS] - space[posP]; /* prediction residual, deviate	*/
									errSpace[p][posE] =	diff;
									sumErrors[p] += diff; /* used for computing the means	*/
								}
								numOfTargetSamples++; /* get number of samples	*/
							}
						}
						for (p = 0; p < numOfPreds; p++)
						{
							meanOfPreds[p] = sumErrors[p] / numOfTargetSamples;
						}

						/* compute covariances	*/
						sumWeights = 0.;
						for (n = 0; n <= searchSpace; n++)
						{
							long n2 = n*n;
							for (m = -searchSpace; m <= searchSpace; m++)
							{
								long posE = m+searchSpace + (searchSpace - n) * errSpaceWidth;
								int p2;
								double posWeight;

								if (m >= 0 && n == 0) continue; /* data not processed yet	*/
								if (n2 + m*m - 1 > (signed)searchSpace2) continue; /* use half circle search space	*/

								posWeight = posWeigthLUT09[n + abs(m)];
								//posWeight = 1;
								sumWeights += posWeight;

								for (p = 0; p < numOfPreds; p++)
								{
									for (p2 = p; p2 < numOfPreds; p2++)
									{
										// richtige Kovarianzen	*/
										//coVar[p][p2] +=	
										//	(errSpace[p ][posE] - meanOfPreds[p ]) *
										//	(errSpace[p2][posE] - meanOfPreds[p2]) * posWeight;
										// ohne Mittelwert
										coVar[p][p2] +=	
											(errSpace[p ][posE] ) *
											(errSpace[p2][posE] ) * posWeight;
									}
								}
							} /* for m	*/
						} /* for n	*/

						/* normalise values	*/
						for (p = 0; p < numOfPreds; p++)
						{
							for (i = p; i < numOfPreds; i++)
							{
								//coVar[p][i] /=	numOfTargetSamples; /* if weights are all equal to 1.0	*/
								coVar[p][i] /=	sumWeights;
							}
						}

						if (useCorrelation_flag)
						{
							int err = 0;
							int ii, j;
							FILE *out = stdout;

							/* check the covariances, it muss not come too close to theroretical maximum	*/
							{
								double threshCoVar, minCoVar, limit;
								for (p = 0; p < numOfPreds; p++)
								{
									for (ii = p+1; ii < numOfPreds; ii++) /* variances do not have to be checked	*/
									{
										minCoVar = min( coVar[p][p], coVar[ii][ii]);
										threshCoVar = sqrt(coVar[p][p] * coVar[ii][ii]); /* upper limit */
										//limit = (3*threshCoVar + minCoVar) * 0.25;
										limit = (7*threshCoVar + minCoVar) * 0.125;
										if (coVar[p][ii] > limit)
											coVar[p][ii] = limit;
									}
								}
							}

							/* 
							** prepare inversion of covariance matrix
							*/
							if ( numOfPreds == 2)
							{ /* simple if only two predictors	*/
								double determinant = coVar[0][0] * coVar[1][1] - coVar[0][1] * coVar[0][1];
								if (determinant <  0.0001) err = 1;
								else
								{
									weights[0][0] = coVar[1][1] / determinant;
									weights[1][0] = -coVar[0][1] / determinant;
									weights[1][1] = coVar[0][0] / determinant;
									weights[0][1] = weights[1][0];
								}
							}
							else
							{
								//unsigned char someZero_flag = 0;
								unsigned char corrZero_flag[MAX_NUMOFPREDS]= { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
								unsigned int countZeroCovar = 0;
								for (i = 0; i < numOfPreds; i++)
								{
									/* avoid close to singular matrices, corresponding weight would be zero!	*/
									if (coVar[i][i] == 0.00)
									{
										countZeroCovar++;
										corrZero_flag[i] = 1;
										//	someZero_flag = 1;
									}
									for (j = i; j < numOfPreds; j++)
									{ /* fill entire matrix	*/
										normal[i][j] = coVar[i][j];
										normal[j][i] = coVar[i][j];
									}
								}
								//if (!someZero_flag)
								if (countZeroCovar < ((unsigned)numOfPreds>>1))
								{
									/* compute inverse covariance matrix (weights), *normal will be changed	*/
									err = svd_inversion( numOfPreds, normal, weights, out);
								}
								else
								{
									/* there is at least one perfect predictor, use high weight for it	*/
									for (i = 0; i < numOfPreds; i++)
									{
										for (j = 0; j < numOfPreds; j++)
										{
											weights[i][j] = 0.;
										}
										if (corrZero_flag[i]) weights[i][i] = 1.;
									}
								}
							}
							if (err)
							{ /* fall back to MED/MAP predictor when inversion is not possible	*/
								medPredictor( pd, chan, &xHat, x, y, RANGEd2);
								xHatd = xHat;
							}
							else
							{ /* do the LS computation	*/
								long	posS = searchSpace+dxMax + (searchSpace + dyMax) * searchSpaceWidth;
								sumOfWeights = 0.;
								for (i = 0; i < numOfPreds; i++)
								{
									for (j = 0; j < numOfPreds; j++)
									{
										sumOfWeights += weights[i][j];
									}
								}
								/* fill observation vector	*/
								for (p = 0; p < numOfPreds; p++)
								{
									long posP;
									posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
									//if ( ((signed)y+dy[p+1]) < 0)
									//{	/* do not use samples outside the space	*/
									//	posP -= ((signed)y+dy[p+1]) * searchSpaceWidth;
									//}
									//if ( ((signed)x + dx[p+1]) < 0)
									//{
									//	posP -= ((signed)x + dx[p+1]);
									//}
									xHats[p] = space[posP]; /* the single estimates	*/
								}
								/* compute tmp= J'*W*y	*/
								xHatd = 0.;
								for (j = 0; j < numOfPreds; j++)
								{
									double ws=0.;
									for (i = 0; i < numOfPreds; i++)
									{
										ws += weights[i][j];
									}
									xHatd += xHats[j] * ws;
								}
								/* compute = inv(J'*W*J)*tmp, while (J'*W*J) is simply =sum(weights)	*/
								// warum hier schon runden?? xHatd = (long)floor( xHatd / sumOfWeights + 0.5);
								xHatd = xHatd / sumOfWeights;
							}
							if (0 && x == 117 &&  y== 181)
							{
								//FILE *
									out = fopen( "logg.txt", "at");
								fprintf( out, "\nxHat: %d", xHat);
								for (j = 0; j < numOfPreds; j++)
								{
									fprintf( out, "\n%d: %f", j, xHats[j]);//, fac[j]);
								}
								fprintf( out, "\nWeights, coVars");
								for (j = 0; j < numOfPreds; j++)
								{
									fprintf( out, "\n");
									for (i = 0; i < numOfPreds; i++)
									{
										fprintf( out, "%f ", weights[j][i]);
									}
								}
								for (j = 0; j < numOfPreds; j++)
								{
									fprintf( out, "\n");
									for (i = 0; i < numOfPreds; i++)
									{
										fprintf( out, "%f ", coVar[j][i]);
									}
								}
								fclose( out);
							}
						}
						else  /* !useCorrelation_flag	*/
						{
							long	posS = searchSpace+dxMax + (searchSpace + dyMax) * searchSpaceWidth;
							/* weighting of predictions
							**
							**  a/wa + b/wb +c/wc +d/wc
							**  -----------------------
							**  1/wa +1/wb + 1/wc +1/wd
							*/
							sumOfWeights = 0.;
							xHatd = 0.;
							for (p = 0; p < numOfPreds; p++)
							{
								long posP;

								posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
								if (coVar[p][p] == 0.) weight[p] = 99999.;
								else			weight[p] = 1./coVar[p][p];
								sumOfWeights += weight[p];

								xHats[p] = space[posP];
								xHats[p] = CLIP( xHats[p], 0, (long)pd->maxVal_rct[chan]);

								xHatd += xHats[p] * weight[p];
							}

							xHatd =  xHatd / sumOfWeights;
						} /* !useCorrelation_flag	*/
					} /* if LS prediction	*/
				}/* do the LS or TM */
				/* clip to actual range*/
#ifdef MEANFREE
					xHatd = CLIP( xHatd + meanOfSamples, 0, (long)pd->maxVal_rct[chan]);
#else
					xHatd = CLIP( xHatd, 0, (long)pd->maxVal_rct[chan]);
#endif
					xHat = (long)floor( xHatd + 0.5);

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
					else if (runModePenalty > 0) 
						runModePenalty--;
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
					else if (templateMatchingPenalty > 0) 
						templateMatchingPenalty--;
				}

			}	/* x */
			fprintf( stderr, "\r%d: %5d/%5d", chan, y, pd->height);
		}	/* y */	
		/* copy error erray in original component */
		if (encode_flag)
			memcpy(pd->data[chan], result->data[chan], pd->size * sizeof(uint));	
	}		/* chan*/

	/* Zeichenfläche löschen */
	DeletePicData(result);	
	FREE( space);
	for (p = 0; p < numOfPreds; p++)
	{
		FREE( errSpace[p]);
	}
	free_matrix( &weights);
	free_matrix( &normal);
}
