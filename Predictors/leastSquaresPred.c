/*****************************************************
* File..: leastSquaresPred.c
* Desc..: LS predictionbased on weighted comb. of simple predictors
* Author: Tilo Strutz
* Date..: 10.08.2017
* chages:
* 23.08.2017 if (templateMatching_flag && y > 0) /* no run mode in first row
* 13.10.2017 cross check with leastSquaresZeroOrderPred.c
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
* lsHighOrderPrediction() 
*-------------------------------------------------------------*/
void lsHighOrderPrediction( PicData *pd, int numOfPreds, int searchSpace,
													 int encode_flag)
{
	uint x, y, chan;
	int err = 0;
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
	int templateMatching_flag = 1;	/* enable template matching*/
	int matchFound_flag = 0;				/* a match has been found */
	int useTemplateMatching_flag = 0;	/* template matching is actually used */
	int equal_flag = 0;	/* all neighbours are equal, enables run mode	*/
	int *errors = NULL;
	long 	epsilon, xHat=0;
	long range, offset;
	long 	Rx, n, m;
	int searchSpaceWidth, searchSpaceHeight, errSpaceWidth, searchSpaceSize;
	/* weights decreasing with increasing distance from current position inside search space	*/
	//double posWeigthLUT09[MAX_LS_SEARCHSPACE*2+1] = { 0, 1.00, 0.90, 0.81, 0.73, 0.66, 0.59, 0.53, 0.48, 0.43, 0.39, 
	//	0.35, 0.31, 0.28, 0.25, 0.23, 0.21, 0.19, 0.17, 0.15, 0.14, 
	//	0.12, 0.11, 0.10, 0.09, 0.08, 0.07, 0.06, 0.06, 0.05, 0.05, 
	//	0.04, 0.04, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02}; /*(0.9^n)*/
	/* when performing weighted LS using SVD, the square roots of the weight sare required	*/
	double posWeigthLUT09SQ[MAX_LS_SEARCHSPACE*2+1] ={0, 1.00, 0.95, 0.90, 0.85, 0.81, 0.77, 0.73, 0.69, 0.66, 0.62, 
		0.59, 0.56, 0.53, 0.50, 0.48, 0.45, 0.43, 0.41, 0.39, 0.37, 
		0.35, 0.33, 0.31, 0.30, 0.28, 0.27, 0.25, 0.24, 0.23, 0.22, 
		0.21, 0.20, 0.19, 0.18, 0.17, 0.16, 0.15, 0.14, 0.14, 0.13}; /*sqrt(0.9^n)*/
	double posWeigthLUT09SQ_[MAX_LS_SEARCHSPACE+1][MAX_LS_SEARCHSPACE+1] = {
		{1.000, 0.949, 0.900, 0.854, 0.810, 0.768, 0.729, 0.692, 0.656, 0.622, 0.590, 0.560, 0.531, 0.504, 0.478, 0.454, 0.430, 0.408, 0.387, 0.368, 0.349, },
		{0.949, 0.928, 0.889, 0.847, 0.805, 0.764, 0.726, 0.689, 0.654, 0.621, 0.589, 0.559, 0.530, 0.503, 0.477, 0.453, 0.430, 0.408, 0.387, 0.367, 0.348, },
		{0.900, 0.889, 0.862, 0.827, 0.790, 0.753, 0.717, 0.681, 0.648, 0.615, 0.584, 0.555, 0.527, 0.500, 0.475, 0.451, 0.428, 0.406, 0.385, 0.366, 0.347, },
		{0.854, 0.847, 0.827, 0.800, 0.768, 0.736, 0.702, 0.670, 0.638, 0.607, 0.577, 0.548, 0.521, 0.495, 0.470, 0.447, 0.424, 0.403, 0.382, 0.363, 0.345, },
		{0.810, 0.805, 0.790, 0.768, 0.742, 0.714, 0.684, 0.654, 0.624, 0.595, 0.567, 0.540, 0.514, 0.488, 0.464, 0.441, 0.419, 0.399, 0.379, 0.360, 0.341, },
		{0.768, 0.764, 0.753, 0.736, 0.714, 0.689, 0.663, 0.636, 0.608, 0.581, 0.555, 0.529, 0.504, 0.480, 0.457, 0.435, 0.414, 0.393, 0.374, 0.355, 0.338, },
		{0.729, 0.726, 0.717, 0.702, 0.684, 0.663, 0.640, 0.615, 0.590, 0.566, 0.541, 0.517, 0.493, 0.470, 0.448, 0.427, 0.406, 0.387, 0.368, 0.350, 0.333, },
		{0.692, 0.689, 0.681, 0.670, 0.654, 0.636, 0.615, 0.594, 0.571, 0.548, 0.526, 0.503, 0.481, 0.459, 0.438, 0.418, 0.399, 0.380, 0.362, 0.344, 0.327, },
		{0.656, 0.654, 0.648, 0.638, 0.624, 0.608, 0.590, 0.571, 0.551, 0.530, 0.509, 0.488, 0.468, 0.447, 0.428, 0.408, 0.390, 0.372, 0.354, 0.338, 0.321, },
		{0.622, 0.621, 0.615, 0.607, 0.595, 0.581, 0.566, 0.548, 0.530, 0.511, 0.492, 0.473, 0.454, 0.435, 0.416, 0.398, 0.380, 0.363, 0.346, 0.330, 0.315, },
		{0.590, 0.589, 0.584, 0.577, 0.567, 0.555, 0.541, 0.526, 0.509, 0.492, 0.475, 0.457, 0.439, 0.421, 0.404, 0.387, 0.370, 0.354, 0.338, 0.323, 0.308, },
		{0.560, 0.559, 0.555, 0.548, 0.540, 0.529, 0.517, 0.503, 0.488, 0.473, 0.457, 0.441, 0.424, 0.408, 0.391, 0.375, 0.360, 0.344, 0.329, 0.315, 0.300, },
		{0.531, 0.530, 0.527, 0.521, 0.514, 0.504, 0.493, 0.481, 0.468, 0.454, 0.439, 0.424, 0.409, 0.394, 0.379, 0.364, 0.349, 0.334, 0.320, 0.306, 0.293, },
		{0.504, 0.503, 0.500, 0.495, 0.488, 0.480, 0.470, 0.459, 0.447, 0.435, 0.421, 0.408, 0.394, 0.380, 0.366, 0.351, 0.338, 0.324, 0.310, 0.297, 0.285, },
		{0.478, 0.477, 0.475, 0.470, 0.464, 0.457, 0.448, 0.438, 0.428, 0.416, 0.404, 0.391, 0.379, 0.366, 0.352, 0.339, 0.326, 0.313, 0.301, 0.288, 0.276, },
		{0.454, 0.453, 0.451, 0.447, 0.441, 0.435, 0.427, 0.418, 0.408, 0.398, 0.387, 0.375, 0.364, 0.351, 0.339, 0.327, 0.315, 0.303, 0.291, 0.279, 0.268, },
		{0.430, 0.430, 0.428, 0.424, 0.419, 0.414, 0.406, 0.399, 0.390, 0.380, 0.370, 0.360, 0.349, 0.338, 0.326, 0.315, 0.304, 0.292, 0.281, 0.270, 0.259, },
		{0.408, 0.408, 0.406, 0.403, 0.399, 0.393, 0.387, 0.380, 0.372, 0.363, 0.354, 0.344, 0.334, 0.324, 0.313, 0.303, 0.292, 0.282, 0.271, 0.261, 0.251, },
		{0.387, 0.387, 0.385, 0.382, 0.379, 0.374, 0.368, 0.362, 0.354, 0.346, 0.338, 0.329, 0.320, 0.310, 0.301, 0.291, 0.281, 0.271, 0.262, 0.252, 0.242, },
		{0.368, 0.367, 0.366, 0.363, 0.360, 0.355, 0.350, 0.344, 0.338, 0.330, 0.323, 0.315, 0.306, 0.297, 0.288, 0.279, 0.270, 0.261, 0.252, 0.243, 0.234, },
		{0.349, 0.348, 0.347, 0.345, 0.341, 0.338, 0.333, 0.327, 0.321, 0.315, 0.308, 0.300, 0.293, 0.285, 0.276, 0.268, 0.259, 0.251, 0.242, 0.234, 0.225, }
	};
	double *space=NULL;
	double xHatd = 0., xHatdTM=0, xHatdRM=0;
	double pattern[6]; /* A C B D E F	*/
	double meanOfSamples;
	double fac[MAX_NUMOFPREDS];
	double *weights = NULL;	/* matrix for weights		*/
	double **jacobi = NULL;	/* copy covariance matrix C	*/
	double *obs = NULL;	/* copy covariance matrix C	*/
	/* matrices and vectors used in solve_lin()	*/
	double **tmpmat = NULL;	/* temporary matrix */
	double **tmpmat2 = NULL;	/* temporary matrix */
	double *s = NULL;	/* singular values */
	double **V = NULL;	/* V matrix */

	unsigned int dist, minDist;
	unsigned int runModePenalty = 0; /* for run mode */
	unsigned int templateMatchingPenalty = 0; /* for template matching	*/
	unsigned long Mepsilon = 0;
	unsigned long numOfTargetSamples=1, numOfSpaceSamples, searchSpace2;
	unsigned long pos, py;
	PicData *result = NULL;
	FILE *out=stdout;

	if (searchSpace < MIN_LS_SEARCHSPACE) searchSpace = MIN_LS_SEARCHSPACE; /* safe guard	*/
	if (searchSpace > MAX_LS_SEARCHSPACE) searchSpace = MAX_LS_SEARCHSPACE; /* safe guard	*/
	if (numOfPreds > MAX_NUMOFPREDS) numOfPreds = MAX_NUMOFPREDS; /* safe guard	*/
	if (numOfPreds < 1) numOfPreds = 1; /* safe guard	*/

	searchSpace2 = searchSpace * searchSpace;

	result = CreatePicData(pd->width, pd->height, pd->channel, 
		pd->bitperchannel, pd->bitperchan);
	CALLOC( errors, pd->size, int);

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
	jacobi = matrix( searchSpaceSize, numOfPreds);	/* Jacobian */
	obs = vector( searchSpaceSize);	/* observations */
	weights = vector( searchSpaceSize);	/* weights dependent on position */
	V = matrix( numOfPreds, numOfPreds);	/* V matrix for SVD */
	s = vector( numOfPreds);	/* singular values for SVD */
	tmpmat = matrix( numOfPreds, numOfPreds);	/* temporary matrix */
	tmpmat2 = matrix( numOfPreds, searchSpaceSize);	/* temporary matrix */

	/* for all colour components (channels) */
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
			/* copy  original component when in decoding mode */
			memcpy( result->data[chan], pd->data[chan], pd->size * sizeof(uint));	
			pd->offset[chan] = 0; /* overwrite old offset as it becomes obsolete here	*/
		}
		else
		{
			pd->offset[chan] = offset; /* overwrite old offset as it becomes obsolete here	*/
		}

		for (y = 0, py = 0; y < pd->height; y++, py += pd->width)
		{
			for (x = 0, pos = py; x < pd->width; x++, pos++)
			{				
				err = 0;
				//if ((signed)x < dxMax || (signed)y < dyMax)
				//{
				//	/* use MED for first lines and rows	*/
				//	medPredictor( pd, chan, &xHat, x, y, RANGEd2);	
				//}
				if ((signed)x == 0 && (signed)y == 0)
				{
					xHatd = xHat = RANGEd2;	
				}
				else 
				{/* do the least squares prediction */

					/* copy search space data plus predictor data
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
							numOfSpaceSamples++;
#endif
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
							  equal_flag = 0;  /* no, its different	*/
							if (pattern[0] != pattern[4] ||
								pattern[0] != pattern[5]) equal_flag = 0;
						}
					}
					if (equal_flag)
					{ /* save run estimate	*/
						xHatdRM = pattern[0];
					}
					if (!equal_flag ||  runModePenalty > 0)
					{	/* do LS or TM and NOT run mode	*/
						/* fill all vectors and matrices
						** and check similarity of patterns
						*/
						i = 0; /* counter for observations (numOfTargetSamples)	*/
						minDist = searchSpace + searchSpace; /* for template-matching mode	*/
						for (n = 0; n <= searchSpace; n++)
						{
							long /*xm,*/ yn;
							long n2 = n*n;
							yn = max(0, (signed)y-n);
							for (m = -searchSpace; m <= searchSpace; m++)
							{
								long posS;	/* current position of target value	*/

								if (n == 0 && m >= 0) continue; /* data not processed yet	*/
								if (n2 + m*m - 1 > (signed)searchSpace2) continue; /* use half circle search space	*/

								/* position of actual target sample starting from bottom line	*/
								posS = searchSpace+dxMax + m + (searchSpace + dyMax - n) * searchSpaceWidth;

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
											n = searchSpace + 1; /* force exit of loop	*/
											continue;
										}
									}
								}/* if (templateMatching_flag)*/

								//weights[i] = posWeigthLUT09SQ[n + abs(m)];
								weights[i] = posWeigthLUT09SQ_[n][abs(m)];
								obs[i] = space[posS] * weights[i]; /* observations	*/
								/* for all predictors					*/
								for (p = 0; p < numOfPreds; p++)
								{
									long posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
									jacobi[i][p] = space[posP] * weights[i]; /* first derivatives	*/
								}
								i++; /* next position (condition)	*/
							}
						} /* for n	*/
						numOfTargetSamples = i;
					}/* do the LS or TM */

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
						/* compute the parameters	*/
						//err = solve_lin( numOfTargetSamples, numOfPreds, obs, weights, jacobi, fac, out);
						/* do the SVD	*/
#ifdef DIRECT_SVD_OF_JACOBI
						//!!!! probably SVD of jacobi takes too much time
						err = singvaldec( jacobi, numOfTargetSamples, numOfPreds, s, V);
#else
						/* do the least squares approximation		*/
						// better to compute J'*J first and then invert
						err =	ls_lin(  numOfTargetSamples, numOfPreds, 
							obs, jacobi, /*weights,*/ fac, out);
#endif

						if (!err)
						{
#ifdef DIRECT_SVD_OF_JACOBI
							double thresh, smax;

							/* check the singular values	*/
							smax = 0.0;
							for (p = 0; p < numOfPreds; p++)
							{
								if (s[p] > smax)	smax = s[p];
							}
							if (smax < TOL_S)
							{
								fprintf( stderr, 
									"\n###\n###     singular matrix, smax = %f",smax);
								fprintf( out, 
									"\n###\n###     singular matrix, smax = %f",smax);

								err = 1;
								//goto endfunc;
							}
							else if (smax > 1.e+31)
							{
								fprintf( stderr, 
									"\n###\n###     degraded matrix, smax = huge");
								fprintf( out, 
									"\n###\n###     degraded matrix, smax = huge");
								err = 1;
								//goto endfunc;
							}
							if (!err)
							{
								thresh = MIN( TOL_S * smax, TOL_S);
								/* invert singular values */
								for (p = 0; p < numOfPreds; p++)
								{
									/* <= in case of smax =0 */
									if (s[p] <= thresh)
										s[p] = 0.0;
									else
										s[p] = 1. / s[p];
								}

								/* V * [diag(1/s[j])] */
								for (i = 0; i < numOfPreds; i++)
								{
									for (p = 0; p < numOfPreds; p++)
									{
										tmpmat[i][p] = V[i][p] * s[p];
									}
								}
								/* multiplication of tmpmat with transposed of U  */
								/* result is: inv(W*J) = (V*inv(S)) * U'	*/
								for (p = 0; p < numOfPreds; p++)
								{
									for (i = 0; i < (signed)numOfTargetSamples; i++)
									{
										int n;
										tmpmat2[p][i] = 0.;
										for (n = 0; n < numOfPreds; n++)
										{
											tmpmat2[p][i] += tmpmat[p][n] * jacobi[i][n];
										}
									}
								}
								/* compute the parameter vector a = inv(W*J)*W*y */
								for (p = 0; p < numOfPreds; p++)
								{
									fac[p] = 0.0;
									for (i = 0; i < (signed)numOfTargetSamples; i++)
									{
										fac[p] += tmpmat2[p][i] *  /* sqrt(weights[i]) * */ obs[i];
									}
								}
#endif
								long	posS= searchSpace+dxMax + (searchSpace + dyMax) * searchSpaceWidth;

								double facSum = 0., invFacSum;
								for (p = 0; p < numOfPreds; p++)
								{
									facSum += fac[p];
								}
								if (facSum <= 0.)
								{
									err = 1;
								}
								else
								{
									//invFacSum = 1. / facSum; /* normalise to a sum equal to one	*/
									invFacSum = 1.; /* use original coefficients	*/
									xHatd = 0.;
									for (p = 0; p < numOfPreds; p++)
									{
										//xHatSum += ((double)pd->data[chan][pos + dx[p+1] + dy[p+1] * pd->width] - meanOfSamples) * fac[p];
										xHatd += space[posS + dx[p+1] + dy[p+1] * searchSpaceWidth] * fac[p] * invFacSum;
									}
								}
#ifdef DIRECT_SVD_OF_JACOBI
							}
#endif
						} /* if !err  LS has been successful */
					} /* if ! templateMatching	*/


					if (err)
					{
						fprintf( stderr, 
							" Unable to solve this linear system! Use MED!\n");
						//fprintf( stderr, "\n Abort!\n");
						//goto endfunc;
						medPredictor( pd, chan, &xHat, x, y, RANGEd2);
						xHatd = xHat;
					}
#ifdef MEANFREE
					xHatd = CLIP( xHatd + meanOfSamples, 0, (long)pd->maxVal_rct[chan]);
#else
					xHatd = CLIP( xHatd, 0, (long)pd->maxVal_rct[chan]);
#endif
					xHat = (long)floor( xHatd + 0.5);
				}/* do the least squares prediction */



				if (encode_flag)
				{
					Rx = (long)*GetXYValue( pd, chan, x, y);

					epsilon = Rx - xHat;
					/* write prediction error value */
					errors[pos] = abs( epsilon);		    			    	

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
			fprintf( stderr, "\r%5d/%5d", y, pd->height);
		}	/* y */	
		/* copy error erray in original component */
		if (encode_flag)
			memcpy(pd->data[chan], result->data[chan], pd->size * sizeof(uint));	
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
	/* Zeichenfläche löschen */
	DeletePicData(result);	
	FREE( space);
	FREE( errors);
	free_vector( &obs);
	free_vector( &weights);
	free_matrix( &jacobi);
	free_vector( &s);
	free_matrix( &V);
	free_matrix( &tmpmat);
	free_matrix( &tmpmat2);
}

#ifdef NOTUSEDANYMORE
/*--------------------------------------------------------------
* lsSingleOrderPrediction() 
*-------------------------------------------------------------*/
void lsSingleOrderPrediction( PicData *pd, int numOfPreds, int searchSpace,
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
	double fac[MAX_NUMOFPREDS];
#ifdef MEANFREE
	double facMax = 1.7;
	double facMin = -facMax;
#else
	double facMax = 1.7;
	double facMin = 1./facMax;
#endif
	double sumProducts[MAX_NUMOFPREDS];
	double sumXProducts[MAX_NUMOFPREDS];
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
	/* space for prediction errors	*/
	errSpaceWidth = (2*searchSpace+1);
	for (p = 0; p < numOfPreds; p++) /* for each predictor	*/
	{
		CALLOC( errSpace[p], (searchSpace+1) * errSpaceWidth, double);
	}

	/* for all colour components (channels) */
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
			/* copy  original component when in decoding mode */
			memcpy( result->data[chan], pd->data[chan], pd->size * sizeof(uint));	
			pd->offset[chan] = 0; /* overwrite old offset as it becomes obsolete here	*/
		}
		else
		{
			pd->offset[chan] = offset; /* overwrite old offset as it becomes obsolete here	*/
		}

		for (y = 0, py = 0; y < pd->height; y++, py += pd->width)
		{
			for (x = 0, pos = py; x < pd->width; x++, pos++)
			{				
				if (x == 0 && y == 0)
				{
					xHatd = xHat = RANGEd2;	
				}
				else 
				{/* do the least squares prediction */
					//if (x == 117 &&  y== 181)
					//{
					//	int z = 9;
					//}

					/* reset values	*/
					for ( i = 0; i < numOfPreds; i++)
					{
						sumProducts[i] = 0;
						sumXProducts[i] = 0;
						for ( p = 0; p < numOfPreds; p++)
						{
							coVar[i][p] = 0;
						}
					}

					/* copy search space data plus predictor data
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
							}
							else if (xm > (signed)pd->width - 1) /* beyond right image border	*/
							{
								mc -= xm - (pd->width - 1);
							}
							if (yn < 0) /* beyond top image border	*/
							{
								nc += yn;
							}
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
					}
					if (!equal_flag ||  runModePenalty > 0)
					{ /* do  NOT run mode but determine linear factors	*/
						numOfTargetSamples = 0; /* count target samples	*/
						minDist = searchSpace + searchSpace; /* for template-matching mode	*/
						for (n = 0; n <= searchSpace; n++)
						{
							long n2 = n*n;
							for (m = -searchSpace; m <= searchSpace; m++)
							{
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
										matchFound_flag = 1;
										minDist = dist;	/* remember shorter distance	*/
										xHatdTM = space[posS];
										if (dist == 1)
										{/* equal to left or top neighbour	*/
											useTemplateMatching_flag = 1; /* equal to left or top neighbour => early stop!	*/
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

								numOfTargetSamples++; /* get number of samples	*/
								posWeight = posWeigthLUT09[n + abs(m)];

								/* for all predictors					*/
								for (p = 0; p < numOfPreds; p++)
								{
									/* current position of predictor value relative to target sample	*/
									long posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
									sumProducts[p]  += space[posP] * space[posP]  * posWeight;
									sumXProducts[p] += space[posS] * space[posP]  * posWeight;
								}
							} /* for m	*/
						}	/* for n	*/
					}/* do the LS or TM */

					if (equal_flag && runModePenalty == 0)
					{	/*  use run mode */
						xHatd = xHatdRM;
					}
					else if (matchFound_flag && useTemplateMatching_flag)
					{	/* use TM mode 	*/
						xHatd = xHatdTM;
					}
					else 
					{
						/* for all predictors determine linear factors	*/
						for (p = 0; p < numOfPreds; p++)
						{
							if (sumProducts[p] == 0.) fac[p] = 1.;
							else		fac[p] = sumXProducts[p] / sumProducts[p];
							if (fac[p] > facMax) 
								fac[p] = facMax; /* safe guard	*/
							else if (fac[p] < facMin) 
								fac[p] = facMin;
						}

						/* use linear coefficients to predict values in search space
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

									diff = space[posS] - space[posP] * fac[p]; /* prediction residual, deviate	*/
									errSpace[p][posE] =	diff;
									sumErrors[p] += diff; /* used for computing the means	*/
								}
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
								sumWeights += posWeight;
								//sumWeights += 1;

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
							int i, j;
							FILE *out = stdout;

							/* check the covariances, it muss not come too close to theroretical maximum	*/
							{
								double threshCoVar, minCoVar, limit;
								for (p = 0; p < numOfPreds; p++)
								{
									for (i = p+1; i < numOfPreds; i++) /* variances do not have to be checked	*/
									{
										minCoVar = min( coVar[p][p], coVar[i][i]);
										threshCoVar = sqrt(coVar[p][p] * coVar[i][i]); /* upper limit */
										//limit = (3*threshCoVar + minCoVar) * 0.25;
										limit = (7*threshCoVar + minCoVar) * 0.125;
										if (coVar[p][i] > limit)
											coVar[p][i] = limit;
									}
								}
							}

							/* 
							* allocate memory
							*/
							//N = numOfPreds; /* single estimates are the observations */
							//weights = matrix( N, N);	/* weights <= 1/covariances */
							if ( numOfPreds == 2)
							{
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
								unsigned char someZero_flag = 0;
								unsigned char corrZero_flag[MAX_NUMOFPREDS]= { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
								for (i = 0; i < numOfPreds; i++)
								{
									/* avoid close to singular matrices, corresponding weight would be zero!	*/
									if (coVar[i][i] == 0.00)
										corrZero_flag[i] = someZero_flag = 1; 
									for (j = i; j < numOfPreds; j++)
									{
										normal[i][j] = coVar[i][j];
										normal[j][i] = coVar[i][j];
									}
								}
								if (!someZero_flag)
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
							{
								medPredictor( pd, chan, &xHat, x, y, RANGEd2);	
							}
							else
							{
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
									if ( (y+dy[p+1]) < 0 || (x + dx[p+1]) < 0) continue; /* do not use pixels outside	*/
									posP = posS + dx[p+1] + dy[p+1] * searchSpaceWidth;
									xHats[p] = space[posP] * fac[p];
									//xHats[p] = ((double)pd->data[chan][pos + dx[p+1] + dy[p+1] * pd->width] - meanOfSamples) * fac[p] + 
									//	meanOfSamples;
									//xHats[p] = CLIP( xHats[p], 0, (long)pd->maxVal_rct[chan]);
								}
								xHatd = 0.;
								//for (i = 0; i < numOfPreds; i++)
								//{
								//	for (j = 0; j < numOfPreds; j++)
								//	{
								//		xHatSum += xHats[j] * weights[i][j];
								//	}
								//}
								for (j = 0; j < numOfPreds; j++)
								{
									double ws=0.;
									for (i = 0; i < numOfPreds; i++)
									{
										ws += weights[i][j];
									}
									xHatd += xHats[j] * ws;
								}
								xHatd = (long)floor( xHatd / sumOfWeights + 0.5);
							}
							if (0 && x == 117 &&  y== 181)
							{
								FILE *out = fopen( "logg.txt", "at");
								fprintf( out, "\nxHat: %d", xHat);
								for (j = 0; j < numOfPreds; j++)
								{
									fprintf( out, "\n%d: %f %f", j, xHats[j], fac[j]);
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
								if (coVar[p][p] == 0.) weight[p] = 99999.;
								else			weight[p] = 1./coVar[p][p];
								sumOfWeights += weight[p];

								//if (minCovar > coVar[p][p]) minCovar = coVar[p][p];
								xHats[p] = (pd->data[chan][pos + dx[p+1] + dy[p+1] * pd->width] - meanOfSamples) * fac[p] + 
									meanOfSamples;
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
#endif