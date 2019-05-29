/*****************************************************************
*
* File..:	predictorTM.c
* Descr.:	Calculation of estimation value using template matching
* Author.:	Tilo Strutz
* Date..: 	25.03.2013
* changes:
* 26.03.2013 weighting of best 3 matches according distance
* 31.03.2013 use SAD instead of SSE
* 03.04.2013 adaptive decision, whether to blend one, two or three matches
* 08.07.2013 weighted averaging of 5 neighbours, 
*            based on texture similarity and spatial distance
* 29.08.2013 modifications towards subpixel accuracy
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types_CoBaLP.h"
#include "codec_CoBaLP.h"
#include "predictorTM.h"

#include "errmsg.h"
#include "matrix_utils.h"


#define TM_EUCLIDEAN
#undef TM_EUCLIDEAN

//#define TM_SEARCH_SPACE	10
//
#define TM_SEARCH_SPACE	20
//#define TM_SEARCH_SPACE	30
//#define TM_SEARCH_SPACE	50 does not really improve the compression
#define TM_SS_SIZE	(TM_SEARCH_SPACE + (2*TM_SEARCH_SPACE + 1)*TM_SEARCH_SPACE)


/* offsets for addresing the templates	*/
int *offTM = NULL;
int *offTM_r = NULL;
int *offTM_c = NULL;
float *euclid_distTM=NULL;	/* Euclidean distance to match	*/
float *euclid_distTM_left=NULL;	/* Euclidean distance to match	*/
float *euclid_distTM_top=NULL;	/* Euclidean distance to match	*/
float *euclid_distTM_diag=NULL;	/* Euclidean distance to match	*/

/*---------------------------------------------------------------
*	init_predictorTM()
*
*---------------------------------------------------------------*/
void init_predictorTM( IMAGE *imExt, LEAST_SQUARES *p_ls)
{
	int c_off, r_off, i, idx, stop;

	ALLOC( offTM,        TM_SS_SIZE, int);
	ALLOC( offTM_r,       TM_SS_SIZE, int);
	ALLOC( offTM_c,       TM_SS_SIZE, int);
	ALLOC( euclid_distTM, TM_SS_SIZE, float);
	ALLOC( euclid_distTM_left, TM_SS_SIZE, float);
	ALLOC( euclid_distTM_top, TM_SS_SIZE, float);
	ALLOC( euclid_distTM_diag, TM_SS_SIZE, float);

	/* set coordinates of positions within the search space	*/
	idx = 0;
	r_off = 0;	/* same row	*/
	{ /* line to the left of X	position	*/
		for ( c_off = 1; c_off <= TM_SEARCH_SPACE; c_off++)
		{
			/* distance to X position	*/
			euclid_distTM[idx] = (float)sqrt( (double)r_off*r_off + c_off*c_off);
			euclid_distTM_left[idx] = (float)sqrt( (double)r_off*r_off + (c_off+0.5F) * (c_off+0.5F));
			euclid_distTM_top[idx]  = (float)sqrt( (double)(r_off+0.5) * (r_off+0.5F) + c_off*c_off);
			euclid_distTM_diag[idx]  = (float)sqrt( (double)(r_off+0.5) * (r_off+0.5F) + (c_off+0.5F) * (c_off+0.5F) );
			offTM[idx]         = -r_off * imExt->width - c_off;
			offTM_r[idx]       = -r_off;
			offTM_c[idx]       = -c_off;
			idx++;
		}
	}
	/* lines above X	position	*/
	for ( r_off = 1; r_off <= TM_SEARCH_SPACE ; r_off++)
	{
		for ( c_off = -TM_SEARCH_SPACE; c_off <= TM_SEARCH_SPACE; c_off++)
		{
			euclid_distTM[idx] = (float)sqrt( (double)r_off*r_off + c_off*c_off);
			euclid_distTM_left[idx] = (float)sqrt( (double)r_off*r_off + (c_off+0.5F) * (c_off+0.5F));
			euclid_distTM_top[idx]  = (float)sqrt( (double)(r_off+0.5F) * (r_off+0.5F) + c_off*c_off);
			euclid_distTM_diag[idx]  = (float)sqrt( (double)(r_off+0.5F) * (r_off+0.5F) + (c_off+0.5F) * (c_off+0.5F) );
			offTM[idx]				 = -r_off * imExt->width - c_off;
			offTM_r[idx]       = -r_off;
			offTM_c[idx]       = -c_off;
			idx++;
		}
	}
	/* sort according ascending distance	*/
	stop = 1;
	do
	{
		stop = 1;
		for ( i = 1; i < TM_SS_SIZE; i++)
		{
			if (euclid_distTM[i] < euclid_distTM[i-1])
			{
				int tmp;
				float tmp_d;

				tmp_d = euclid_distTM[i];
				euclid_distTM[i] = euclid_distTM[i-1];
				euclid_distTM[i-1] = tmp_d;

				tmp_d = euclid_distTM_left[i];
				euclid_distTM_left[i] = euclid_distTM_left[i-1];
				euclid_distTM_left[i-1] = tmp_d;

				tmp_d = euclid_distTM_top[i];
				euclid_distTM_top[i] = euclid_distTM_top[i-1];
				euclid_distTM_top[i-1] = tmp_d;

				tmp_d = euclid_distTM_diag[i];
				euclid_distTM_diag[i] = euclid_distTM_diag[i-1];
				euclid_distTM_diag[i-1] = tmp_d;

				tmp = offTM[i];
				offTM[i] = offTM[i-1];
				offTM[i-1] = tmp;

				tmp = offTM_r[i];
				offTM_r[i] = offTM_r[i-1];
				offTM_r[i-1] = tmp;

				tmp = offTM_c[i];
				offTM_c[i] = offTM_c[i-1];
				offTM_c[i-1] = tmp;

				stop = 0;
			}
		}
	}
	while (!stop);

	/* for least squares	*/
	p_ls->a = vector( NUM_OF_PARAMS);	/* singular values for SVD */
	p_ls->obs = vector( NUM_OF_OBSERVATIONS);	/* observations */
	p_ls->weights = vector( NUM_OF_OBSERVATIONS);	/* weights for observ. */
	p_ls->J = matrix( NUM_OF_OBSERVATIONS, NUM_OF_PARAMS);	/* Jacobi matrix */
	p_ls->covar = matrix( NUM_OF_PARAMS, NUM_OF_PARAMS);	/* covariance matrix */
}

/*---------------------------------------------------------------
*	clean_predictorTM()
*
*---------------------------------------------------------------*/
void clean_predictorTM( LEAST_SQUARES *p_ls )
{
	FREE( offTM);
	FREE( offTM_r);
	FREE( offTM_c);
	FREE( euclid_distTM);
	FREE( euclid_distTM_left);
	FREE( euclid_distTM_top);
	FREE( euclid_distTM_diag);

	free_vector( &(p_ls->a));
	free_vector( &(p_ls->obs));
	free_vector( &(p_ls->weights));
	free_matrix( &(p_ls->J));
	free_matrix( &(p_ls->covar));
}

/*---------------------------------------------------------------
*	predictorTM()
*
*	  ghi
*  fcbdj
*  eax
*
*---------------------------------------------------------------*/
float predictorTM( IMAGE *imExt, long *lptr, 
									 unsigned long pos, /* is posExt	*/
									 int r, int c, LEAST_SQUARES *p_ls)
{
	int idx, i, j, flag /*, err = 0*/;
	int posA, posB, posC, posD, posE, posF, posG, posI, posJ, posH;
	long diff, dist;
	long xHat_[NUM_OF_OBSERVATIONS];
	long min_dist[NUM_OF_OBSERVATIONS];
	float distance[NUM_OF_OBSERVATIONS];
	float xHat, w[NUM_OF_OBSERVATIONS];
	//double normFac, normFac_off;
	unsigned int cnt;
	unsigned long pos_[NUM_OF_OBSERVATIONS];

	if (r == 0)
	{
		if (c > 0)  return (float)lptr[pos-1]; /* take left neighbour	*/
		else				return (float)(imExt->max_value / 2);
	}
	else if (c == 0)
	{
		if (r > 0)  return (float)lptr[pos-imExt->width];
	}
	else if ( r > 1 && c > 1 && (r+c) > 4 && 
		c < (signed)imExt->width - OFF_LEFT - OFF_RIGHT -2)
	{
		/* go ahead	*/;
	}
	else
	{
		return (float)(0.5 * (lptr[pos-1] + lptr[pos-imExt->width]));
	}

	/* positions within template	*/
	posA = pos-1;
	posB = pos  -imExt->width;
	posC = pos-1-imExt->width;
	posD = pos+1-imExt->width;
	posE = pos-2;
	posF = pos-2-imExt->width;
	posJ = pos+2-imExt->width;
	posG = pos-1-imExt->width-imExt->width;
	posH = pos  -imExt->width-imExt->width;
	posI = pos+1-imExt->width-imExt->width;

	for ( i = 0; i < NUM_OF_OBSERVATIONS; i++)
	{
		min_dist[i] = (long)9999999; /* something arbitrary large	*/
		xHat_[i] = 0;
		pos_[i] = 0;
		distance[i] = 1.;
	}

	/* get normalisation factor	of current postion
	* it would be better to compute these factors only once
	* here it is computed again and again
	*/
	//normFac = lptr[posA] * lptr[posA] + lptr[posB] * lptr[posB] + 
	//					lptr[posC] * lptr[posC] + lptr[posD] * lptr[posD];
	//normFac = 1. / sqrt( 0.25 * normFac);


	cnt = 0;
	/* for entire search space with increasing distance to current position	*/
	for ( idx = 0; idx < TM_SS_SIZE; idx++)
	{
		/* avoid accessing memory outside the lptr array	*/
		if ( (r + OFF_TOP + offTM_r[idx]) < 2) /* 2 corresponds to the template high	*/
			continue;
		if ( (c + OFF_LEFT + offTM_c[idx]) < 2) /* 2 corresponds to the template width	*/
			continue;
		if (   /* 2 corresponds to the template width	*/
			(c + OFF_RIGHT+ offTM_c[idx]) >= ((signed)imExt->width - 2) )
			continue;
		dist = 0;

#ifdef TM_EUCLIDEAN
		diff = lptr[posA] - lptr[posA + offTM[idx]];	/* A */
		dist += diff * diff; /* Euclidean	*/
		diff = lptr[posE] - lptr[posE + offTM[idx]];	/* E */
		dist += diff * diff; /* Euclidean	*/
		diff = lptr[posF] - lptr[posF + offTM[idx]];	/* F */
		dist += diff * diff; /* Euclidean	*/
		diff = lptr[posC] - lptr[posC + offTM[idx]];	/* C */
		dist += diff * diff; /* Euclidean	*/
		diff = lptr[posB] - lptr[posB + offTM[idx]];	/* B */
		dist += diff * diff; /* Euclidean	*/
		diff = lptr[posD] - lptr[posD + offTM[idx]];	/* D */
		dist += diff * diff; /* Euclidean	*/
		diff = lptr[posJ] - lptr[posJ + offTM[idx]];	/* J */
		dist += diff * diff; /* Euclidean	*/

		diff = lptr[posG] - lptr[posG + offTM[idx]];	/* G */
		dist += diff * diff; /* Euclidean	*/
		diff = lptr[posH] - lptr[posH + offTM[idx]];	/* H */
		dist += diff * diff; /* Euclidean	*/
		diff = lptr[posI] - lptr[posI + offTM[idx]];	/* I */
		dist += diff * diff; /* Euclidean	*/

#else
		/* full pixel accuracy	*/
		diff = lptr[posA] - lptr[posA + offTM[idx]];	/* A */
		dist += abs( diff);	/* SAD */
		diff = lptr[posE] - lptr[posE + offTM[idx]];	/* E */
		dist += abs( diff);	/* SAD */
		diff = lptr[posF] - lptr[posF + offTM[idx]];	/* F */
		dist += abs( diff);	/* SAD */
		diff = lptr[posC] - lptr[posC + offTM[idx]];	/* C */
		dist += abs( diff);	/* SAD */
		diff = lptr[posB] - lptr[posB + offTM[idx]];	/* B */
		dist += abs( diff);	/* SAD */
		diff = lptr[posD] - lptr[posD + offTM[idx]];	/* D */
		dist += abs( diff);	/* SAD */
		diff = lptr[posJ] - lptr[posJ + offTM[idx]];	/* J */
		dist += abs( diff);	/* SAD */

		diff = lptr[posG] - lptr[posG + offTM[idx]];	/* G */
		dist += abs( diff);	/* SAD */
		diff = lptr[posH] - lptr[posH + offTM[idx]];	/* H */
		dist += abs( diff);	/* SAD */
		diff = lptr[posI] - lptr[posI + offTM[idx]];	/* I */
		dist += abs( diff);	/* SAD */

#endif

//if(1)
{
		flag = 1;
		for ( i = 0; (i < NUM_OF_OBSERVATIONS) && flag; i++)
		{
			if ( min_dist[i] > dist)
			{
				/* move worse results downwards	*/
				for ( j = NUM_OF_OBSERVATIONS-1; j > i; j--)
				{
					xHat_[j] = xHat_[j-1];
					distance[j] = distance[j-1];
					min_dist[j] = min_dist[j-1];
					// pos_[j] = pos_[j-1]; for LS fitting
				}
				xHat_[i] = lptr[pos + offTM[idx]]; /* X */
				distance[i] = euclid_distTM[idx];
				min_dist[i] = dist;
				// pos_[i] = pos + offTM[idx]; for LS fitting
				cnt++;
				flag = 0; /* as soon a min_dist[i] was set, the other
									* are not of interest anymore
									*/
			}
		}
}

#ifndef TM_EUCLIDEAN
		/* half pixel accuracy to the left	*/
		if ( (c + OFF_LEFT + offTM_c[idx]) >= 3 && 1)
		{
			int off;

			off = offTM[idx] - 1;
			dist = 0;
			diff = lptr[posA] - ((lptr[posA + offTM[idx]] + lptr[posA + off]) >> 1);	/* A */
			dist += abs( diff);	/* SAD */
			diff = lptr[posE] - ((lptr[posE + offTM[idx]] + lptr[posE + off]) >> 1);	/* E */
			dist += abs( diff);	/* SAD */
			diff = lptr[posF] - ((lptr[posF + offTM[idx]] + lptr[posF + off]) >> 1);	/* F */
			dist += abs( diff);	/* SAD */
			diff = lptr[posC] - ((lptr[posC + offTM[idx]] + lptr[posC + off]) >> 1);	/* C */
			dist += abs( diff);	/* SAD */
			diff = lptr[posB] - ((lptr[posB + offTM[idx]] + lptr[posB + off]) >> 1);	/* B */
			dist += abs( diff);	/* SAD */
			diff = lptr[posD] - ((lptr[posD + offTM[idx]] + lptr[posD + off]) >> 1);	/* D */
			dist += abs( diff);	/* SAD */
			diff = lptr[posJ] - ((lptr[posJ + offTM[idx]] + lptr[posJ + off]) >> 1);	/* J */
			dist += abs( diff);	/* SAD */

			diff = lptr[posG] - ((lptr[posG + offTM[idx]] + lptr[posG + off]) >> 1);	/* G */
			dist += abs( diff);	/* SAD */
			diff = lptr[posH] - ((lptr[posH + offTM[idx]] + lptr[posH + off]) >> 1);	/* H */
			dist += abs( diff);	/* SAD */
			diff = lptr[posI] - ((lptr[posI + offTM[idx]] + lptr[posI + off]) >> 1);	/* I */
			dist += abs( diff);	/* SAD */

			flag = 1;
			for ( i = 0; (i < NUM_OF_OBSERVATIONS) && flag; i++)
			{
				if ( min_dist[i] > dist)
				{
					/* move worse results downwards	*/
					for ( j = NUM_OF_OBSERVATIONS-1; j > i; j--)
					{
						xHat_[j] = xHat_[j-1];
						distance[j] = distance[j-1];
						min_dist[j] = min_dist[j-1];
						// pos_[j] = pos_[j-1]; for LS fitting
					}
					xHat_[i] = (lptr[pos + offTM[idx]] + lptr[pos + off]) >> 1; /* X */
					distance[i] = euclid_distTM_left[idx];
					min_dist[i] = dist;
					// pos_[i] = pos + offTM[idx]; for LS fitting
					cnt++;
					flag = 0; /* as soon a min_dist[i] was set, the other
										* are not of interest anymore
										*/
				}
			}
		}
		else
		{
			c=c;
		}

		/* half pixel accuracy to the top	*/
		if ( (r + OFF_TOP + offTM_r[idx]) >= 3 && 1)
		{
			int off;
			off = offTM[idx] - imExt->width;
			dist = 0;
			diff = lptr[posA] - ((lptr[posA + offTM[idx]] + lptr[posA + off] ) >> 1);	/* A */
			dist += abs( diff);	/* SAD */
			diff = lptr[posE] - ((lptr[posE + offTM[idx]] + lptr[posE + off]) >> 1);	/* E */
			dist += abs( diff);	/* SAD */
			diff = lptr[posF] - ((lptr[posF + offTM[idx]] + lptr[posF + off]) >> 1);	/* F */
			dist += abs( diff);	/* SAD */
			diff = lptr[posC] - ((lptr[posC + offTM[idx]] + lptr[posC + off]) >> 1);	/* C */
			dist += abs( diff);	/* SAD */
			diff = lptr[posB] - ((lptr[posB + offTM[idx]] + lptr[posB + off]) >> 1);	/* B */
			dist += abs( diff);	/* SAD */
			diff = lptr[posD] - ((lptr[posD + offTM[idx]] + lptr[posD + off]) >> 1);	/* D */
			dist += abs( diff);	/* SAD */
			diff = lptr[posJ] - ((lptr[posJ + offTM[idx]] + lptr[posJ + off]) >> 1);	/* J */
			dist += abs( diff);	/* SAD */

			diff = lptr[posG] - ((lptr[posG + offTM[idx]] + lptr[posG + off]) >> 1);	/* G */
			dist += abs( diff);	/* SAD */
			diff = lptr[posH] - ((lptr[posH + offTM[idx]] + lptr[posH + off]) >> 1);	/* H */
			dist += abs( diff);	/* SAD */
			diff = lptr[posI] - ((lptr[posI + offTM[idx]] + lptr[posI + off]) >> 1);	/* I */
			dist += abs( diff);	/* SAD */

			flag = 1;
			for ( i = 0; (i < NUM_OF_OBSERVATIONS) && flag; i++)
			{
				if ( min_dist[i] > dist)
				{
					/* move worse results downwards	*/
					for ( j = NUM_OF_OBSERVATIONS-1; j > i; j--)
					{
						xHat_[j] = xHat_[j-1];
						distance[j] = distance[j-1];
						min_dist[j] = min_dist[j-1];
						// pos_[j] = pos_[j-1]; for LS fitting
					}
					xHat_[i] = (lptr[pos + offTM[idx]] + lptr[pos + off]) >> 1; /* X */
					distance[i] = euclid_distTM_top[idx];
					min_dist[i] = dist;
					// pos_[i] = pos + offTM[idx]; for LS fitting
					cnt++;
					flag = 0; /* as soon a min_dist[i] was set, the other
										* are not of interest anymore
										*/
				}
			}
		}
		else
		{
			c=c;
		}

		/* half pixel accuracy to the top	left*/
		if ( (r + OFF_TOP  + offTM_r[idx]) >= 3  && 
				 (c + OFF_LEFT + offTM_c[idx]) >= 3  && 0 /* for some reason, diagonal averaging detoriates results*/
			)
		{
			int offl, offt, offd;
			offl = offTM[idx] -1;
			offt = offTM[idx] - imExt->width;
			offd = offTM[idx] -1 - imExt->width;
			dist = 0;
			diff = lptr[posA] - ((lptr[posA + offTM[idx]] + lptr[posA + offl] + 
														lptr[posA + offt]				+ lptr[posA + offd] + 2) >> 2);	/* A */
			dist += abs( diff);	/* SAD */
			diff = lptr[posE] - ((lptr[posE + offTM[idx]] + lptr[posE + offl] + 
														lptr[posE + offt]				+ lptr[posE + offd] + 2) >> 2);	/* E */
			dist += abs( diff);	/* SAD */
			diff = lptr[posF] - ((lptr[posF + offTM[idx]] + lptr[posF + offl] + 
														lptr[posF + offt]				+ lptr[posF + offd] + 2) >> 2);	/* F */
			dist += abs( diff);	/* SAD */
			diff = lptr[posC] - ((lptr[posC + offTM[idx]] + lptr[posC + offl] + 
														lptr[posC + offt]				+ lptr[posC + offd] + 2) >> 2);	/* C */
			dist += abs( diff);	/* SAD */
			diff = lptr[posB] - ((lptr[posB + offTM[idx]] + lptr[posB + offl] + 
														lptr[posB + offt]				+ lptr[posB + offd] + 2) >> 2);	/* B */
			dist += abs( diff);	/* SAD */
			diff = lptr[posD] - ((lptr[posD + offTM[idx]] + lptr[posD + offl] + 
														lptr[posD + offt]				+ lptr[posD + offd] + 2) >> 2);	/* D */
			dist += abs( diff);	/* SAD */
			diff = lptr[posJ] - ((lptr[posJ + offTM[idx]] + lptr[posJ + offl] + 
														lptr[posJ + offt]				+ lptr[posJ + offd] + 2 ) >> 2);	/* J */
			dist += abs( diff);	/* SAD */

			diff = lptr[posG] - ((lptr[posG + offTM[idx]] + lptr[posG + offl] + 
														lptr[posG + offt]				+ lptr[posG + offd] + 2) >> 2);	/* G */
			dist += abs( diff);	/* SAD */
			diff = lptr[posH] - ((lptr[posH + offTM[idx]] + lptr[posH + offl] + 
														lptr[posH + offt]				+ lptr[posH + offd] + 2) >> 2);	/* H */
			dist += abs( diff);	/* SAD */
			diff = lptr[posI] - ((lptr[posI + offTM[idx]] + lptr[posI + offl] + 
														lptr[posI + offt]				+ lptr[posI + offd] + 2) >> 2);	/* I */
			dist += abs( diff);	/* SAD */

			flag = 1;
			for ( i = 0; (i < NUM_OF_OBSERVATIONS) && flag; i++)
			{
				if ( min_dist[i] > dist)
				{
					/* move worse results downwards	*/
					for ( j = NUM_OF_OBSERVATIONS-1; j > i; j--)
					{
						xHat_[j] = xHat_[j-1];
						distance[j] = distance[j-1];
						min_dist[j] = min_dist[j-1];
						// pos_[j] = pos_[j-1]; for LS fitting
					}
					xHat_[i] = (lptr[pos + offTM[idx]] + lptr[pos + offl] + 
														lptr[pos + offt] + lptr[pos + offd] + 2) >> 2; /* X */
					distance[i] = euclid_distTM_diag[idx];
					min_dist[i] = dist;
					// pos_[i] = pos + offTM[idx]; for LS fitting
					cnt++;
					flag = 0; /* as soon a min_dist[i] was set, the other
										* are not of interest anymore
										*/
				}
			}
		} /* if ( (r + OFF_TOP */
		else
		{
			c=c;
		}

#endif

		/* [i] contains now a sorted list of best matches	*/
	} /* for ( idx = 0; idx < TM_SS_SIZE; idx++) */

	if (cnt == 0)
	{
		xHat = 0.5F * (lptr[pos-1] + lptr[pos-imExt->width]);
	}
	else 	if (cnt == 1)
	{
		xHat = (float)xHat_[0];
	}
	else if (cnt == 2)
	{
		/* weighted sum based on distance to current position	*/
		xHat = (xHat_[0] * distance[1] + 
			xHat_[1] * distance[0]    ) / (distance[0] + distance[1]) ;
	}
	else 
	{
		float sum;
		unsigned int num;

		/* *zero_dist_flag = 0;   is already reset above	*/
		num = min( cnt, NUM_OF_OBSERVATIONS);
		/* weighted sum based on distance (min_dist) to current position	*/
		for (i = 0; i < (signed)num; i++)
		{
			//if (min_dist[i]) /* avoid division by zero	*/
			//{
			//	w[i] = 1./ (min_dist[i] * distance[i]);
			//}
			//else
			//{
			//	w[i] = 9999.;
			//}
			w[i] = 1.F/ (min_dist[i] * distance[i] + 0.01F); /* 0.001F leads is slightly worse*/
		}

		xHat = 0;
		sum = 0;
		for (i = 0; i < (signed)num; i++)
		{
			xHat += xHat_[i] * w[i];
			sum += w[i];
		}
		xHat /= sum;

#ifdef NIXX
		/* linear prediction based on lin LS using 5 template matches	*/
		if (num >= 5 && 0) /* does not work very well	*/
		{ /* five observations required for 4 parameters	*/
			for (i = 0; i < (signed)num; i++)
			{
				/* prepare observations	*/
				p_ls->obs[i] = xHat_[i]; /* = lptr[pos_[i]];	*/

				/* prepare weights	*/
				if (min_dist[i]) /* avoid division by zero	*/
				{
					p_ls->weights[i] = 1. / min_dist[i];
				}
				else
				{
					p_ls->weights[i] = 9999.;
				}

				/* prepare Jacobian, order A, B, C, D	*/
				p_ls->J[i][0] = lptr[pos_[i] - 1];
				p_ls->J[i][1] = lptr[pos_[i] - imExt->width];
				p_ls->J[i][2] = lptr[pos_[i] - 1 - imExt->width];
				p_ls->J[i][3] = lptr[pos_[i] + 1 - imExt->width];
			}
			err = solve_linear( num, 4, 
				p_ls->obs, p_ls->weights, p_ls->J, p_ls->covar, p_ls->a);


			/* check the uncertainty in parameters */
			if (!err && 1)
			{
				unsigned int j, flag = 0;
				double val;

				for (j = 0; j < 4; j++)
				{
					if (p_ls->covar[j][j] >= 0)
					{
						val = sqrt( p_ls->covar[j][j]) * 100. / fabs( p_ls->a[j]);
						if (val > 10) flag++;
					}
				}

				if (!flag)
				{
					//xHat =  
					//			 p_ls->a[0] * lptr[pos -1] + 
					//			 p_ls->a[1] * lptr[pos - imExt->width] + 
					//			 p_ls->a[2] * lptr[pos -1 - imExt->width] + 
					//			 p_ls->a[3] * lptr[pos +1 - imExt->width];
					xHat = 0.5 * (
						xHat + 
						p_ls->a[0] * lptr[pos -1] + 
						p_ls->a[1] * lptr[pos - imExt->width] + 
						p_ls->a[2] * lptr[pos -1 - imExt->width] + 
						p_ls->a[3] * lptr[pos +1 - imExt->width]
					);
				}
				else if (flag)
				{
					c=c;
				}
			} /* if (!err )*/
		} /* if num>= 5 */
#endif

		/* weighted sum based on distance to current position	*/
#ifdef AUCHNIXGUT
		if (min_dist_1st == 0) xHat = xHat_1st;
		else if (min_dist_2nd == 0) xHat = xHat_2nd;
		else if (min_dist_3rd == 0) xHat = xHat_3rd;
		else
		{
			//sum = min_dist_1st + min_dist_2nd + min_dist_3rd;
			//xHat = (xHat_1st * 0.5*(sum - min_dist_1st)  + 
			//				xHat_2nd * 0.5*(sum - min_dist_2nd)  + 
			//				xHat_3rd * 0.5*(sum - min_dist_3rd) ) / sum;
			xHat = 1.0*(xHat_1st * distance_2nd *distance_3rd  + 
				xHat_2nd * distance_1st * distance_3rd  + 
				xHat_3rd * distance_1st * distance_2nd) / 
				(distance_2nd * distance_3rd +
				distance_1st * distance_3rd +
				distance_1st * distance_2nd);
		}
#endif
#ifdef NIXGUT
		/* weighted sum based on distance to current position	*/
		if (min_dist_1st == 0) xHat = xHat_1st;
		else if (min_dist_2nd == 0) xHat = xHat_2nd;
		else if (min_dist_3rd == 0) xHat = xHat_3rd;
		else
		{
			//sum = min_dist_1st + min_dist_2nd + min_dist_3rd;
			//xHat = (xHat_1st * 0.5*(sum - min_dist_1st)  + 
			//				xHat_2nd * 0.5*(sum - min_dist_2nd)  + 
			//				xHat_3rd * 0.5*(sum - min_dist_3rd) ) / sum;
			xHat = 1.0*(xHat_1st * min_dist_2nd * min_dist_3rd  + 
				xHat_2nd * min_dist_1st * min_dist_3rd  + 
				xHat_3rd * min_dist_1st * min_dist_2nd) / 
				(min_dist_2nd * min_dist_3rd +
				min_dist_1st * min_dist_3rd +
				min_dist_1st * min_dist_2nd);
		}
#endif
	} /* 	else if (1)*/

#ifdef ADAPTIVXXX
	else /* adaptive computation	*/
	{
		double sum;
		long average, diff2;

		/* check min_dist values	*/
		average =  (min_dist_3rd + min_dist_1st) >> 1;
		diff = average - min_dist_2nd;
		diff2 = average - min_dist_1st;
		if ( (diff2 < 10 && abs(diff) < 5) || abs(diff) *2 <= diff2 )
		{	/* take all three values	*/
			sum = distance_1st + distance_2nd + distance_3rd;
			/* weighted sum based on distance to current position	*/
			xHat = (xHat_1st * 0.5*(sum - distance_1st)  + 
				xHat_2nd * 0.5*(sum - distance_2nd)  + 
				xHat_3rd * 0.5*(sum - distance_3rd) ) / sum;
		}
		else if  (diff < 0)
		{/* take only the best one	*/
			xHat = (double)xHat_1st;
		}
		else
		{
			/* weighted sum based on distance to current position	*/
			xHat = (xHat_1st * distance_2nd + 
				xHat_2nd * distance_1st    ) / (distance_1st + distance_2nd) ;
		}
	}
#endif
	return (xHat);
}
