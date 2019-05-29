/*****************************************************************
 *
 * File...:	find_colourspace4.c
 * Purpose:	determine best colourspace based on entropy
 * Author.:	Tilo Strutz
 * Date...: 18.04.2012
 * changes:
 * 14.05.2012 use MED prediction indtead of left neighbour
 * 15.05.2012 JPEG2000: use 'int' for arrays instead of unsigned int
 *					RGB_ instead RGB
 * 26.09.2013 new space: PEI09
 * 27.09.2013 pred_flag in find_colourspace()
 * 18.12.2013 preference to RGB if entropy is identical
 * 13.02.2014 bugfix, EY[9] is reserved for PEI, do not select
 * 13.04.2015 check set_size != 0 before use
 *****************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "types.h"
#include "colour4.h"
#include "codec.h" /* fpr max() and min()	*/

#ifndef MODULO_SELECTION

#define VERBOSE2

#define NUM_UV (15+1) /* plus PEI09, number of different computations for U and V	*/
#define NUM_Y (9+1)  /* plus PEI09, number of different computations for Y	*/
#define UV_COMPO 13 /* number of NUM_UV without copies from Y	 (R, B G) */

/*---------------------------------------------------------------
 *	find_colourspace4()
 *---------------------------------------------------------------*/
unsigned char find_colourspace4( IMAGEc *im, unsigned int *r_ptr, 
										 unsigned int *g_ptr, unsigned int *b_ptr,
										 unsigned long set_size,
										 unsigned int *Yi, unsigned int *UVj,
										 double entropy[],
										 unsigned int y_start,
										 unsigned int y_end,
										 unsigned int x_start,
										 unsigned int x_end,
										 unsigned char pred_flag
										 )
{
	unsigned char ct_type;
	int i; /* overlap of single R, G or B	*/
	int counter, step_size;
	int *YUV_data[NUM_Y+UV_COMPO];	/* two lines of an image, for each Y and UV	*/
	int *YUV_line[NUM_Y+UV_COMPO][2], *tmp;	/* pointer to two successive lines	*/
	long maxAB, minAB, w;
	double log2, logN;
	double EY[NUM_Y], ent;	/* entropy in Y component	*/
	double EUV[NUM_UV];			/* entropy in U or V component	*/
	double min_ent; 
	unsigned int min_Y_idx, min_UV_idx;
	unsigned long x, y, pos, py, N;
	unsigned long HY[NUM_Y][1024];	/* histogramm for Y values	*/
	unsigned long HUV[NUM_UV][1024];	/* histogramm for UV values	*/

unsigned int ct_array[10][12] = {	/* all colour spaces	*/
		{A1_1, A1_2, A1_3, A1_4, A1_5, A1_6, A1_7, A1_8, A1_9, A1_10, A1_11, A1_12},
		{A2_1, A2_2, A2_3, A2_4, A2_5, A2_6, A2_7, A2_8, A2_9, A2_10, A2_11, A2_12},
		{A3_1, A3_2, A3_3, A3_4, A3_5, A3_6, A3_7, A3_8, A3_9, A3_10, A3_11, A3_12},
		{A4_1, A4_2, A4_3, A4_4, A4_5, A4_6, A4_7, A4_8, A4_9, A4_10, A4_11, A4_12},
		{A5_1, A5_2, A5_3, A5_4, A5_5, A5_6, A5_7, A5_8, A5_9, A5_10, A5_11, A5_12},
		{A6_1, A6_2, A6_3, A6_4, A6_5, A6_6, A6_7, A6_8, A6_9, A6_10, A6_11, A6_12},
		{A7_1, A7_2, A7_3, A7_4, A7_5, A7_6, A7_7, A7_8, A7_9, A7_10, A7_11, A7_12},
		{A8_1, A8_2, A8_3, A8_4, A8_5, A8_6, A8_7, A8_8, A8_9, A8_10, A8_11, A8_12},
		{A9_1, A9_2, A9_3, A9_4, A9_5, A9_6, A9_7, A9_8, A9_9, A9_10, A9_11, A9_12},
		{B1_1, B1_2, B2_1, B2_3, B3_2, B3_3, B4_1, B5_2, B6_3, PEI09,  RGBc , RGBc}
	};

	/* transform single U V channels *
	 * UV possibilities
	 * 0   1   -1      0		R-G
	 * 1   0    1     -1	  G-B
	 * 2   1    0     -1	  R-B
	 * 3   1/4	3/4	  -1		B-(R+3G)/4
	 * 4   3/4	1/4	  -1		B-(3R+G)/4
	 * 5  -1/4		1	  3/4		G-(R+3B)/4
	 * 6   1    -3/4	-1/4	R-(B+3G)/4
	 * 7   1    -1/4	-3/4	R-(3B+G)/4
	 * 8   3/4	-1		1/4		G-(B+3R)/4
	 * 9   1/2	1/2   -1		B-(R+G)/2
	 *10   1/2	-1	  1/2		G-(R+B)/2
	 *11   1	 -1/2		-1/2	R-(B+G)/2
	 *12 -87/256 -169/256  1		PEI09
	 *13   0	   1     0		copy from Y
	 *14   1	   0     0	
	 *15   0     0     1
	 *
	 * Y possibilities
	 *0  0	   1     0	
	 *1  1	   0     0	
	 *2  0     0     1
	 *3  1/2	1/2   0	
	 *4  0  	1/2   1/2	
	 *5  1/2	 0    1/2
	 *6  1/4	1/2   1/4	
	 *7  1/2	1/4   1/4	
	 *8  1/4	1/4   1/2	
	 *9 -0.297 0.589 0.113	PEI09
	 *
	 */
	w = im->width;

	/* reset frequency counts	*/
	for ( pos = 0; pos < 1024; pos++) 
	{
		for ( i = 0; i < NUM_Y; i++)
		{
			HY[i][pos] = 0;
		}
		for ( i = 0; i < NUM_UV; i++)
		{
			HUV[i][pos] = 0;
		}
	}

	/* examine only approximately 10000 pixels	*/
	if (set_size) /* safe guard */
		step_size = ((im->width-1) * im->height) / set_size;
	else step_size = 1;
	if ( (step_size % (im->width-1)) == 0)
	{
		step_size++;
	}
//step_size = 1;

	N = 0;

	/* 21 two-lines arrays for 23 different YUV computations	*/
	for ( i = 0; i < NUM_Y+UV_COMPO; i++)
	{
		// YUV_data[i] = (int*)malloc( im->width * 2 * sizeof(int));
		ALLOC( YUV_data[i], im->width * 2, int);
		YUV_line[i][0] = YUV_data[i];
		YUV_line[i][1] = YUV_data[i]  + im->width ;
	}

	if (set_size == im->size)
	{
		/* use entire image	*/
		for (y = y_start, py = y_start * im->width; y < im->height && y < y_end; y++, py+=im->width)
		{
			for (x = x_start, pos = py+x_start; x < x_end && x < im->width; x++, pos++)
			{
				/* determine all different Y values, first line	*/
				YUV_line[0][0][x] = g_ptr[pos];				/*  0	1	 0	*/
				YUV_line[1][0][x] = r_ptr[pos];				/*  1	0	 0	*/
				YUV_line[2][0][x] = b_ptr[pos];				/*  0	0	 1	*/

				YUV_line[3][0][x] = (r_ptr[pos] + g_ptr[pos]) >> 1;				/*  1/2	1/2	 0	*/
				YUV_line[4][0][x] = (g_ptr[pos] + b_ptr[pos]) >> 1;				/*  0	 1/2	1/2	*/
				YUV_line[5][0][x] = (r_ptr[pos] + b_ptr[pos]) >> 1;				/*  1/2	0	 1/2	*/

				YUV_line[6][0][x] = (r_ptr[pos] + g_ptr[pos] + 
														g_ptr[pos] + b_ptr[pos]) >> 2;	/*  1/4	1/2	 1/4	*/
				YUV_line[7][0][x] = (r_ptr[pos] + r_ptr[pos] + 
														g_ptr[pos] + b_ptr[pos]) >> 2;	/*  1/2	1/4	 1/4	*/
				YUV_line[8][0][x] = (r_ptr[pos] + g_ptr[pos] + 
														b_ptr[pos] + b_ptr[pos]) >> 2;	/*  1/4	1/4	 1/2	*/
				{
					int Cb, Cr;
					Cr = r_ptr[pos] - (int)g_ptr[pos];	/* R-G	*/
					Cb = b_ptr[pos] - ((87*r_ptr[pos] + 169*g_ptr[pos]) >> 8);	
					YUV_line[9][0][x] = g_ptr[pos] + ((86*Cr + 29*Cb) >> 8);	/*  PEI09	*/
				}

				/* determine all different UV values, first line	*/
				YUV_line[10][0][x] = r_ptr[pos] - g_ptr[pos];	/*    1   -1     0	*/
				YUV_line[11][0][x] = g_ptr[pos] - b_ptr[pos];	/*    0    1    -1	*/
				YUV_line[12][0][x] = r_ptr[pos] - b_ptr[pos];	/*    1   0     -1	*/
				
				YUV_line[13][0][x] = YUV_line[11][0][x] + (YUV_line[10][0][x] >> 2);	/*   1/4	3/4	  -1			G-B +(R-G)/4 */
				YUV_line[14][0][x] = YUV_line[12][0][x] - (YUV_line[10][0][x] >> 2);	/*   3/4	1/4	  -1			R-B -(R-G)/4 */
				YUV_line[15][0][x] = YUV_line[11][0][x] - (YUV_line[12][0][x] >> 2);	/*   -1/4	1    -3/4			G-B -(R-B)/4 */
				YUV_line[16][0][x] = YUV_line[10][0][x]  + (YUV_line[11][0][x] >> 2);	/*   1    -3/4	-1/4		R-G +(G-B)/4 */
				YUV_line[17][0][x] = YUV_line[12][0][x] - (YUV_line[11][0][x] >> 2);	/*   1		-1/4	3/4			R-B -(G-B)/4 */
				YUV_line[18][0][x] = YUV_line[10][0][x]  - (YUV_line[12][0][x] >> 2);	/*   3/4	-1		1/4			R-G -(R-B)/4 */

				YUV_line[19][0][x] = YUV_line[11][0][x] + (YUV_line[10][0][x] >> 1);	/*  1/2	1/2   -1		G-B + (R-G)/2	*/
				YUV_line[20][0][x] = YUV_line[10][0][x] - (YUV_line[12][0][x] >> 1);	/*  1/2	 -1	1/2			R-G - (R-B)/2 */
				YUV_line[21][0][x] = YUV_line[10][0][x] + (YUV_line[11][0][x] >> 1);	/*  1	-1/2  -1/2			R-G + (G-B)/2 */

				YUV_line[22][0][x] = b_ptr[pos] - ((87*r_ptr[pos] +169*g_ptr[pos]) >> 8);	/* -87/256  1	-169/256 */
				
				if ( x == x_start || y == y_start) continue; /* first line and first column are needed for prediction	*/

				N++;	/* count number of selected pixels	*/

				/* do the prediction	*/
				if (pred_flag)
				{
					/* do the prediction for all Y values in line	*/
					for ( i = 0; i < NUM_Y; i++)	/* all Y components	*/
					{
						long A, B, C, xHat;
						A = YUV_line[i][0][x-1];
						B = YUV_line[i][1][x];
						C = YUV_line[i][1][x-1];
						maxAB = max( A, B); 
						minAB = min( A, B);

						if (C >= maxAB)	xHat = minAB;
						else if (C <= minAB)	xHat = maxAB;
						else			xHat = A + B - C;
						assert(YUV_line[i][0][x] - xHat + 511 >= 0);
						assert(YUV_line[i][0][x] - xHat + 511 < 1024);

						/* increment counter for this prediction error plus offset	*/
						HY[i][ YUV_line[i][0][x] - xHat + 511 ]++;
					}
					/* do the prediction for all UV values in line	*/
					for ( i = NUM_Y; i < NUM_Y+UV_COMPO; i++)	/* all UV components	*/
					{
						long A, B, C, xHat;
						A = YUV_line[i][0][x-1];
						B = YUV_line[i][1][x];
						C = YUV_line[i][1][x-1];
						maxAB = max( A, B); 
						minAB = min( A, B);

						if (C >= maxAB)	xHat = minAB;
						else if (C <= minAB)	xHat = maxAB;
						else			xHat = A + B - C;

						assert(YUV_line[i][0][x] - xHat + 511 >= 0);
						assert(YUV_line[i][0][x] - xHat + 511 < 1024);

						/* increment counter for this prediction error plus offset	*/
						HUV[i-NUM_Y][ YUV_line[i][0][x] - xHat + 511 ]++;	
					} /* for i=9	*/
				}
				else /* do not use prediction	*/
				{
					/* increment counters for for all Y values	*/
					for ( i = 0; i < NUM_Y; i++)	/* all Y components	*/
					{
						assert( YUV_line[i][0][x] + 511 >= 0);
						assert( YUV_line[i][0][x] + 511 < 1024);
						HY[i][ YUV_line[i][0][x] + 511 ]++;
					}
					/* increment counters for for all UV values	*/
					for ( i = NUM_Y; i < NUM_Y+UV_COMPO; i++)	/* all UV components	*/
					{
						assert( YUV_line[i][0][x] + 511 >= 0);
						assert( YUV_line[i][0][x] + 511 < 1024);
						HUV[i-NUM_Y][ YUV_line[i][0][x] + 511 ]++;
					}
				}
			}	/* for x	*/

			/* exchange lines	*/
			for ( i = 0; i < NUM_Y+UV_COMPO; i++)	/* all YUV components	*/
			{
				tmp = YUV_line[i][0];
				YUV_line[i][0] = YUV_line[i][1];
				YUV_line[i][1] = tmp;
			}
		}	/* for y	*/
	} /* if full image	*/
	else
	{
		/* use subset of image	*/
		if (y_start == 0) y_start++;
		if (x_start == 0) x_start++;
		counter = step_size;

		for (y = y_start, py = y_start * im->width; y < im->height && y < y_end; y++, py+=im->width)
		{
			for (x = x_start, pos = py+x_start; x < x_end && x < im->width; x++, pos++)
			{
				counter--;
				if (counter > 1) continue; /* skip this pixel	*/
				if (counter == 0)	/* now count the prediction error	*/
				{
					counter = step_size;
					N++;	/* count number of selected pixels	*/


					/* current position X	*/
					YUV_line[0][0][x] = g_ptr[pos];				/*  0	1	 0	*/
					YUV_line[1][0][x] = r_ptr[pos];				/*  1	0	 0	*/
					YUV_line[2][0][x] = b_ptr[pos];				/*  0	0	 1	*/

					YUV_line[3][0][x] = (r_ptr[pos] + g_ptr[pos]) >> 1;				/*  1/2	1/2	 0	*/
					YUV_line[4][0][x] = (g_ptr[pos] + b_ptr[pos]) >> 1;				/*  0	 1/2	1/2	*/
					YUV_line[5][0][x] = (r_ptr[pos] + b_ptr[pos]) >> 1;				/*  1/2	0	 1/2	*/

					YUV_line[6][0][x] = (r_ptr[pos] + g_ptr[pos] + 
															g_ptr[pos] + b_ptr[pos]) >> 2;	/*  1/4	1/2	 1/4	*/
					YUV_line[7][0][x] = (r_ptr[pos] + r_ptr[pos] + 
															g_ptr[pos] + b_ptr[pos]) >> 2;	/*  1/2	1/4	 1/4	*/
					YUV_line[8][0][x] = (r_ptr[pos] + g_ptr[pos] + 
															b_ptr[pos] + b_ptr[pos]) >> 2;	/*  1/4	1/4	 1/2	*/

					{
						int Cb, Cr;
						Cr = r_ptr[pos] - (int)g_ptr[pos];	/* R-G	*/
						Cb = b_ptr[pos] - ((87*r_ptr[pos] + 169*g_ptr[pos]) >> 8);	
						YUV_line[9][0][x] = g_ptr[pos] + ((86*Cr + 29*Cb) >> 8);	/*  PEI09	*/
					}
					/* UV */
					YUV_line[10][0][x] = r_ptr[pos] - g_ptr[pos];	/*    1   -1     0	*/
					YUV_line[11][0][x] = g_ptr[pos] - b_ptr[pos];	/*    0    1    -1	*/
					YUV_line[12][0][x] = r_ptr[pos] - b_ptr[pos];	/*    1   0     -1	*/
					
					YUV_line[13][0][x] = YUV_line[11][0][x] + (YUV_line[10][0][x] >> 2);	/*   1/4	3/4	  -1			G-B +(R-G)/4 */
					YUV_line[14][0][x] = YUV_line[12][0][x] - (YUV_line[10][0][x] >> 2);	/*   3/4	1/4	  -1			R-B -(R-G)/4 */
					YUV_line[15][0][x] = YUV_line[11][0][x] - (YUV_line[12][0][x] >> 2);	/*   -1/4	1    -3/4			G-B -(R-B)/4 */
					YUV_line[16][0][x] = YUV_line[10][0][x]  + (YUV_line[11][0][x] >> 2);	/*   1    -3/4	-1/4		R-G +(G-B)/4 */
					YUV_line[17][0][x] = YUV_line[12][0][x] - (YUV_line[11][0][x] >> 2);	/*   1		-1/4	3/4			R-B -(G-B)/4 */
					YUV_line[18][0][x] = YUV_line[10][0][x]  - (YUV_line[12][0][x] >> 2);	/*   3/4	-1		1/4			R-G -(R-B)/4 */

					YUV_line[19][0][x] = YUV_line[11][0][x] + (YUV_line[10][0][x] >> 1);	/*  1/2	1/2   -1		G-B + (R-G)/2	*/
					YUV_line[20][0][x] = YUV_line[10][0][x] - (YUV_line[12][0][x] >> 1);	/*  1/2	 -1	1/2			R-G - (R-B)/2 */
					YUV_line[21][0][x] = YUV_line[10][0][x] + (YUV_line[11][0][x] >> 1);	/*  1	-1/2  -1/2			R-G + (G-B)/2 */

					YUV_line[22][0][x] = b_ptr[pos] - ((87*r_ptr[pos] +169*g_ptr[pos]) >> 8);	/* -87/256  1	-169/256 */

					/* left position A	*/
					YUV_line[0][0][x-1] = g_ptr[pos-1];				/*  0	1	 0	*/
					YUV_line[1][0][x-1] = r_ptr[pos-1];				/*  1	0	 0	*/
					YUV_line[2][0][x-1] = b_ptr[pos-1];				/*  0	0	 1	*/

					YUV_line[3][0][x-1] = (r_ptr[pos-1] + g_ptr[pos-1]) >> 1;				/*  1/2	1/2	 0	*/
					YUV_line[4][0][x-1] = (g_ptr[pos-1] + b_ptr[pos-1]) >> 1;				/*  0	 1/2	1/2	*/
					YUV_line[5][0][x-1] = (r_ptr[pos-1] + b_ptr[pos-1]) >> 1;				/*  1/2	0	 1/2	*/

					YUV_line[6][0][x-1] = (r_ptr[pos-1] + g_ptr[pos-1] + 
																g_ptr[pos-1] + b_ptr[pos-1]) >> 2;	/*  1/4	1/2	 1/4	*/
					YUV_line[7][0][x-1] = (r_ptr[pos-1] + r_ptr[pos-1] + 
																g_ptr[pos-1] + b_ptr[pos-1]) >> 2;	/*  1/2	1/4	 1/4	*/
					YUV_line[8][0][x-1] = (r_ptr[pos-1] + g_ptr[pos-1] + 
																b_ptr[pos-1] + b_ptr[pos-1]) >> 2;	/*  1/4	1/4	 1/2	*/
					{
						int Cb, Cr;
						Cr = r_ptr[pos-1] - (int)g_ptr[pos-1];	/* R-G	*/
						Cb = b_ptr[pos-1] - ((87*r_ptr[pos-1] + 169*g_ptr[pos-1]) >> 8);	
						YUV_line[9][0][x-1] = g_ptr[pos-1] + ((86*Cr + 29*Cb) >> 8);	/*  PEI09	*/
					}

					/* UV */
					YUV_line[10][0][x-1]  = r_ptr[pos-1] - g_ptr[pos-1];	/*    1   -1     0	*/
					YUV_line[11][0][x-1] = g_ptr[pos-1] - b_ptr[pos-1];	/*    0    1    -1	*/
					YUV_line[12][0][x-1] = r_ptr[pos-1] - b_ptr[pos-1];	/*    1   0     -1	*/
					
					YUV_line[13][0][x-1] = YUV_line[11][0][x-1] + (YUV_line[10][0][x-1] >> 2);	/*   1/4	3/4	  -1			G-B +(R-G)/4 */
					YUV_line[14][0][x-1] = YUV_line[12][0][x-1] - (YUV_line[10][0][x-1] >> 2);	/*   3/4	1/4	  -1			R-B -(R-G)/4 */
					YUV_line[15][0][x-1] = YUV_line[11][0][x-1] - (YUV_line[12][0][x-1] >> 2);	/*   -1/4	1    -3/4			G-B -(R-B)/4 */
					YUV_line[16][0][x-1] = YUV_line[10][0][x-1]  + (YUV_line[11][0][x-1] >> 2);	/*   1    -3/4	-1/4		R-G +(G-B)/4 */
					YUV_line[17][0][x-1] = YUV_line[12][0][x-1] - (YUV_line[11][0][x-1] >> 2);	/*   1		-1/4	3/4			R-B -(G-B)/4 */
					YUV_line[18][0][x-1] = YUV_line[10][0][x-1]  - (YUV_line[12][0][x-1] >> 2);	/*   3/4	-1		1/4			R-G -(R-B)/4 */

					YUV_line[19][0][x-1] = YUV_line[11][0][x-1] + (YUV_line[10][0][x-1] >> 1);	/*  1/2	1/2   -1		G-B + (R-G)/2	*/
					YUV_line[20][0][x-1] = YUV_line[10][0][x-1] - (YUV_line[12][0][x-1] >> 1);	/*  1/2	 -1	1/2			R-G - (R-B)/2 */
					YUV_line[21][0][x-1] = YUV_line[10][0][x-1] + (YUV_line[11][0][x-1] >> 1);	/*  1	-1/2  -1/2			R-G + (G-B)/2 */

					YUV_line[22][0][x-1] = b_ptr[pos-1] - ((87*r_ptr[pos-1] +169*g_ptr[pos-1]) >> 8);	/* -87/256  1	-169/256 */

					/* top left position C	*/
					YUV_line[0][1][x-1] = g_ptr[pos-1-w];				/*  0	1	 0	*/
					YUV_line[1][1][x-1] = r_ptr[pos-1-w];				/*  1	0	 0	*/
					YUV_line[2][1][x-1] = b_ptr[pos-1-w];				/*  0	0	 1	*/

					YUV_line[3][1][x-1] = (r_ptr[pos-1-w] + g_ptr[pos-1-w]) >> 1;				/*  1/2	1/2	 0	*/
					YUV_line[4][1][x-1] = (g_ptr[pos-1-w] + b_ptr[pos-1-w]) >> 1;				/*  0	 1/2	1/2	*/
					YUV_line[5][1][x-1] = (r_ptr[pos-1-w] + b_ptr[pos-1-w]) >> 1;				/*  1/2	0	 1/2	*/

					YUV_line[6][1][x-1] = (r_ptr[pos-1-w] + g_ptr[pos-1-w] + 
																g_ptr[pos-1-w] + b_ptr[pos-1-w]) >> 2;	/*  1/4	1/2	 1/4	*/
					YUV_line[7][1][x-1] = (r_ptr[pos-1-w] + r_ptr[pos-1-w] + 
																g_ptr[pos-1-w] + b_ptr[pos-1-w]) >> 2;	/*  1/2	1/4	 1/4	*/
					YUV_line[8][1][x-1] = (r_ptr[pos-1-w] + g_ptr[pos-1-w] + 
																b_ptr[pos-1-w] + b_ptr[pos-1-w]) >> 2;	/*  1/4	1/4	 1/2	*/
					{
						int Cb, Cr;
						Cr = r_ptr[pos-1-w] - (int)g_ptr[pos-1-w];	/* R-G	*/
						Cb = b_ptr[pos-1-w] - ((87*r_ptr[pos-1-w] + 169*g_ptr[pos-1-w]) >> 8);	
						YUV_line[9][1][x-1] = g_ptr[pos-1-w] + ((86*Cr + 29*Cb) >> 8);	/*  PEI09	*/
					}

					/* UV */
					YUV_line[10][1][x-1] = r_ptr[pos-1-w] - g_ptr[pos-1-w];	/*    1   -1     0	*/
					YUV_line[11][1][x-1] = g_ptr[pos-1-w] - b_ptr[pos-1-w];	/*    0    1    -1	*/
					YUV_line[12][1][x-1] = r_ptr[pos-1-w] - b_ptr[pos-1-w];	/*    1   0     -1	*/
					
					YUV_line[13][1][x-1] = YUV_line[11][1][x-1] + (YUV_line[10][1][x-1] >> 2);	/*   1/4	3/4	  -1			G-B +(R-G)/4 */
					YUV_line[14][1][x-1] = YUV_line[12][1][x-1] - (YUV_line[10][1][x-1] >> 2);	/*   3/4	1/4	  -1			R-B -(R-G)/4 */
					YUV_line[15][1][x-1] = YUV_line[11][1][x-1] - (YUV_line[12][1][x-1] >> 2);	/*   -1/4	1    -3/4			G-B -(R-B)/4 */
					YUV_line[16][1][x-1] = YUV_line[10][1][x-1] + (YUV_line[11][1][x-1] >> 2);	/*   1    -3/4	-1/4		R-G +(G-B)/4 */
					YUV_line[17][1][x-1] = YUV_line[12][1][x-1] - (YUV_line[11][1][x-1] >> 2);	/*   1		-1/4	3/4			R-B -(G-B)/4 */
					YUV_line[18][1][x-1] = YUV_line[10][1][x-1] - (YUV_line[12][1][x-1] >> 2);	/*   3/4	-1		1/4			R-G -(R-B)/4 */

					YUV_line[19][1][x-1] = YUV_line[11][1][x-1] + (YUV_line[10][1][x-1] >> 1);	/*  1/2	1/2   -1		G-B + (R-G)/2	*/
					YUV_line[20][1][x-1] = YUV_line[10][1][x-1] - (YUV_line[12][1][x-1] >> 1);	/*  1/2	 -1	1/2			R-G - (R-B)/2 */
					YUV_line[21][1][x-1] = YUV_line[10][1][x-1] + (YUV_line[11][1][x-1] >> 1);	/*  1	-1/2  -1/2			R-G + (G-B)/2 */

					YUV_line[22][1][x-1] = b_ptr[pos-1-w] - ((87*r_ptr[pos-1-w] +169*g_ptr[pos-1-w]) >> 8);	/* -87/256  1	-169/256 */

					/* top  position B	*/
					YUV_line[0][1][x] = g_ptr[pos-w];				/*  0	1	 0	*/
					YUV_line[1][1][x] = r_ptr[pos-w];				/*  1	0	 0	*/
					YUV_line[2][1][x] = b_ptr[pos-w];				/*  0	0	 1	*/

					YUV_line[3][1][x] = (r_ptr[pos-w] + g_ptr[pos-w]) >> 1;				/*  1/2	1/2	 0	*/
					YUV_line[4][1][x] = (g_ptr[pos-w] + b_ptr[pos-w]) >> 1;				/*  0	 1/2	1/2	*/
					YUV_line[5][1][x] = (r_ptr[pos-w] + b_ptr[pos-w]) >> 1;				/*  1/2	0	 1/2	*/

					YUV_line[6][1][x] = (r_ptr[pos-w] + g_ptr[pos-w] + 
																g_ptr[pos-w] + b_ptr[pos-w]) >> 2;	/*  1/4	1/2	 1/4	*/
					YUV_line[7][1][x] = (r_ptr[pos-w] + r_ptr[pos-w] + 
																g_ptr[pos-w] + b_ptr[pos-w]) >> 2;	/*  1/2	1/4	 1/4	*/
					YUV_line[8][1][x] = (r_ptr[pos-w] + g_ptr[pos-w] + 
																b_ptr[pos-w] + b_ptr[pos-w]) >> 2;	/*  1/4	1/4	 1/2	*/
					{
						int Cb, Cr;
						Cr = r_ptr[pos-w] - (int)g_ptr[pos-w];	/* R-G	*/
						Cb = b_ptr[pos-w] - ((87*r_ptr[pos-w] + 169*g_ptr[pos-w]) >> 8);	
						YUV_line[9][1][x] = g_ptr[pos-w] + ((86*Cr + 29*Cb) >> 8);	/*  PEI09	*/
					}

					/* UV */
					YUV_line[10][1][x] = r_ptr[pos-w] - g_ptr[pos-w];	/*    1   -1     0	*/
					YUV_line[11][1][x] = g_ptr[pos-w] - b_ptr[pos-w];	/*    0    1    -1	*/
					YUV_line[12][1][x] = r_ptr[pos-w] - b_ptr[pos-w];	/*    1   0     -1	*/
					
					YUV_line[13][1][x] = YUV_line[11][1][x] + (YUV_line[10][1][x] >> 2);	/*   1/4	3/4	  -1			G-B +(R-G)/4 */
					YUV_line[14][1][x] = YUV_line[12][1][x] - (YUV_line[10][1][x] >> 2);	/*   3/4	1/4	  -1			R-B -(R-G)/4 */
					YUV_line[15][1][x] = YUV_line[11][1][x] - (YUV_line[12][1][x] >> 2);	/*   -1/4	1    -3/4			G-B -(R-B)/4 */
					YUV_line[16][1][x] = YUV_line[10][1][x]  + (YUV_line[11][1][x] >> 2);	/*   1    -3/4	-1/4		R-G +(G-B)/4 */
					YUV_line[17][1][x] = YUV_line[12][1][x] - (YUV_line[11][1][x] >> 2);	/*   1		-1/4	3/4			R-B -(G-B)/4 */
					YUV_line[18][1][x] = YUV_line[10][1][x]  - (YUV_line[12][1][x] >> 2);	/*   3/4	-1		1/4			R-G -(R-B)/4 */

					YUV_line[19][1][x] = YUV_line[11][1][x] + (YUV_line[10][1][x] >> 1);	/*  1/2	1/2   -1		G-B + (R-G)/2	*/
					YUV_line[20][1][x] = YUV_line[10][1][x]  - (YUV_line[12][1][x] >> 1);	/*  1/2	 -1	1/2			R-G - (R-B)/2 */
					YUV_line[21][1][x] = YUV_line[10][1][x]  + (YUV_line[11][1][x] >> 1);	/*  1	-1/2  -1/2			R-G + (G-B)/2 */

					YUV_line[22][1][x] = b_ptr[pos-w] - ((87*r_ptr[pos-w] +169*g_ptr[pos-w]) >> 8);	/* -87/256  1	-169/256 */

					/* do the prediction	*/
					if (pred_flag)
					{
						for ( i = 0; i < NUM_Y; i++)	/* all Y components	*/
						{
							long A, B, C, xHat;
							A = YUV_line[i][0][x-1];
							B = YUV_line[i][1][x];
							C = YUV_line[i][1][x-1];
							maxAB = max( A, B); 
							minAB = min( A, B);

							if (C >= maxAB)	xHat = minAB;
							else if (C <= minAB)	xHat = maxAB;
							else			xHat = A + B - C;

							HY[i][ YUV_line[i][0][x] - xHat + 511 ]++;	
						}
						for ( i = NUM_Y; i < NUM_Y+UV_COMPO; i++)	/* all UV components	*/
						{
							long A, B, C, xHat;
							A = YUV_line[i][0][x-1];
							B = YUV_line[i][1][x];
							C = YUV_line[i][1][x-1];
							maxAB = max( A, B); 
							minAB = min( A, B);

							if (C >= maxAB)	xHat = minAB;
							else if (C <= minAB)	xHat = maxAB;
							else			xHat = A + B - C;


							HUV[i-NUM_Y][ YUV_line[i][0][x] - xHat + 511 ]++;	
						} /* for i=9	*/
					} /* pred_flag*/
					else
					{
						for ( i = 0; i < NUM_Y; i++)	/* all Y components	*/
						{
							assert( YUV_line[i][0][x] + 511 >= 0);
							assert( YUV_line[i][0][x] + 511 < 1024);
							HY[i][ YUV_line[i][0][x] + 511]++;	
						}
						for ( i = NUM_Y; i < NUM_Y+UV_COMPO; i++)	/* all UV components	*/
						{
							assert( YUV_line[i][0][x] + 511 >= 0);
							assert( YUV_line[i][0][x] + 511 < 1024);
							HUV[i-NUM_Y][ YUV_line[i][0][x] + 511]++;
						}
					}
				} /*	if (counter == 0)	*/
			}	/* for x	*/
		}	/* for y	*/
	}


	/* copy identical counts	*/
	for ( pos = 0; pos < 1024; pos++) 
	{ /* not use below, instead utilising the HY/EY values		*/
		HUV[13][pos] = HY[0][pos];	/* G	*/
		HUV[14][pos] = HY[1][pos];	/* R	*/
		HUV[15][pos] = HY[2][pos];	/* B	*/
	}

	if (N == 0)
	{
		/* can this happen ?	*/
		return A7_1;
	}
	/* compute entropys	*/
	//N = (im->width-1) * im->height;
	logN = log( (double)N);
	log2 = log( 2.0);

	/* compute entropy of Y components and determine best	*/
	min_ent = 9999999.;
	min_Y_idx = 0;
	for ( i = 0; i < NUM_Y; i++)
	{
		ent = 0.0;
		for ( pos = 0; pos < 1024; pos++) 
		{
			if (HY[i][pos] )
			{	
				// E = - sum pi * log2(pi)
				// E = - sum pi * log(pi) / log(2)
				// E = - 1/log(2) * sum pi * log(pi)
				// E = - 1/log(2) * sum hi/N * log(hi/N)
				// E = - 1/(N*log(2)) * sum hi * (log(hi) - log(N))
				ent -= (double)HY[i][pos] * (log( (double)HY[i][pos]) -logN);
			}
		}
		EY[i] = ent / (N * log2);
		/* 9 is reserved vor Pei06, inclusion could falsely lead to Bx_x	*/
		if (min_ent > EY[i] && i < 9) 
		{
			min_ent = EY[i];
			min_Y_idx = i;
		}
	}
	entropy[0] = EY[min_Y_idx];

	/* compute entropy of UV components	*/
	for ( i = 0; i < NUM_UV; i++)
	{
		ent = 0.0;
		for ( pos = 0; pos < 1024; pos++) 
		{
			if (HUV[i][pos] )
			{	
				// E = - sum pi * log2(pi)
				// E = - sum pi * log(pi) / log(2)
				// E = - 1/log(2) * sum pi * log(pi)
				// E = - 1/log(2) * sum hi/N * log(hi/N)
				// E = - 1/(N*log(2)) * sum hi * (log(hi) - log(N))
				ent -= (double)HUV[i][pos] * (log( (double)HUV[i][pos]) -logN);
			}
		}
		EUV[i] = ent / (N * log2);
	}


	/* check all possible UV combinations */
	
	min_ent = EUV[0] + EUV[1]; /* R-G	G-B	*/

	entropy[1] = EUV[0];
	entropy[2] = EUV[1];
	min_UV_idx = 0;

	ent = EUV[0] + EUV[2]; /* R-G	R-B	*/
	if (ent < min_ent)
	{
		min_ent = ent;
		min_UV_idx = 1;
		entropy[1] = EUV[0];
		entropy[2] = EUV[2];
	}

	ent = EUV[1] + EUV[2]; /* G-R R-B	*/
	if (ent < min_ent)
	{
		min_ent = ent;
		min_UV_idx = 2;
		entropy[1] = EUV[1];
		entropy[2] = EUV[2];
	}

	ent = EUV[0] + EUV[3]; /* R-G		B-(R+3G)/4	*/
	if (ent < min_ent)
	{
		min_ent = ent;
		min_UV_idx = 3;
		entropy[1] = EUV[0];
		entropy[2] = EUV[3];
	}

	ent = EUV[0] + EUV[4]; /* R-G		B-(3R+G)/4		*/
	if (ent < min_ent)
	{
		min_ent = ent;
		min_UV_idx = 4;
		entropy[1] = EUV[0];
		entropy[2] = EUV[4];
	}

	ent = EUV[2] + EUV[5]; /* R-B	G-(R+3B)/4 */
	if (ent < min_ent)
	{
		min_ent = ent;
		min_UV_idx = 5;
		entropy[1] = EUV[2];
		entropy[2] = EUV[5];
	}

	ent = EUV[1] + EUV[6]; /* B-G	R-(B+3G)/4	*/
	if (ent < min_ent)
	{
		min_ent = ent;
		min_UV_idx = 6;
		entropy[1] = EUV[1];
		entropy[2] = EUV[6];
	}

	ent = EUV[1] + EUV[7]; /* B-G	R-(3B+G)/4	*/
	if (ent < min_ent)
	{
		min_ent = ent;
		min_UV_idx = 7;
		entropy[1] = EUV[1];
		entropy[2] = EUV[7];
	}

	ent = EUV[2] + EUV[8]; /* R-B	G-(B+3R)/4	*/
	if (ent < min_ent)
	{
		min_ent = ent;
		min_UV_idx = 8;
		entropy[1] = EUV[2];
		entropy[2] = EUV[8];
	}

	ent = EUV[0] + EUV[9]; /* R-G	B-(R+G)/2	*/
	if (ent < min_ent)
	{
		min_ent = ent;
		min_UV_idx = 9;
		entropy[1] = EUV[0];
		entropy[2] = EUV[9];
	}

	ent = EUV[2] + EUV[10]; /* R-B	G-(R+B)/2	*/
	if (ent < min_ent)
	{
		min_ent = ent;
		min_UV_idx = 10;
		entropy[1] = EUV[2];
		entropy[2] = EUV[10];
	}

	ent = EUV[1] + EUV[11]; /* B-G	R-(B+G)/2	*/
	if (ent < min_ent)
	{
		min_ent = ent;
		min_UV_idx = 11;
		entropy[1] = EUV[1];
		entropy[2] = EUV[11];
	}

	ct_type = (unsigned char) ct_array[min_Y_idx][min_UV_idx];
  *Yi = min_Y_idx;
  *UVj = min_UV_idx;

	min_ent += EY[min_Y_idx]; /* combine entropies from UV and Y	*/
	/*---------------------------------------------------*/

	/* check RGB	*/
	ent = EY[0] + EY[1] + EY[2];
	if (ent <= min_ent)	/* preference to RGB if entropy is identical	*/
	{
		min_ent = ent;
		ct_type = RGBc;
		*Yi = 9; *UVj = 11;
		entropy[0] = EY[0];
		entropy[1] = EY[1];
		entropy[2] = EY[2];
	}

	/* check  R-G G B	*/
	ent = EUV[0] + EY[0] + EY[2];
	if (ent < min_ent)
	{
		min_ent = ent;
		ct_type = B1_1;
		*Yi = 9; *UVj = 0;
		entropy[0] = EUV[0];
		entropy[1] = EY[0];
		entropy[2] = EY[2];
	}

	/* check  R<=>G:  G-R R B	*/
	ent = EUV[0] + EY[1] + EY[2];
	if (ent < min_ent-0.015) /* penalty	*/
	{
		min_ent = ent;
		ct_type = B2_1;
		*Yi = 9; *UVj = 2;
		entropy[0] = EUV[0];
		entropy[1] = EY[1];
		entropy[2] = EY[2];
	}

	/* check  B<=>G:  R-B B G	*/
	ent = EUV[2] + EY[2] + EY[0];
	if (ent < min_ent)
	{
		min_ent = ent;
		ct_type = B3_3;;
		*Yi = 9; *UVj = 5;
		entropy[0] = EUV[2];
		entropy[1] = EY[2];
		entropy[2] = EY[0];
	}

	/* check  R<=>B: B-G G R	*/
	ent = EUV[1] + EY[0] + EY[1];
	if (ent < min_ent)
	{
		min_ent = ent;
		ct_type = B1_2;
		*Yi = 9; *UVj = 1;
		entropy[0] = EUV[1];
		entropy[1] = EY[0];
		entropy[2] = EY[1];
	}

	/* check R->G->B->R: G-B B R	*/
	ent = EUV[1] + EY[2] + EY[1];
	if (ent < min_ent-0.015) /* penalty	*/
	{
		min_ent = ent;
		ct_type = B3_2;
		*Yi = 9; *UVj = 4;
		entropy[0] = EUV[1];
		entropy[1] = EY[2];
		entropy[2] = EY[1];
	}

	/* check R->B->G->R: B-R R G	*/
	ent = EUV[2] + EY[1] + EY[0];
	if (ent < min_ent)
	{
		min_ent = ent;
		ct_type = B2_3;
		*Yi = 9; *UVj = 3;
		entropy[0] = EUV[0];
		entropy[1] = EY[0];
		entropy[2] = EY[2];
	}

	/* check  (G+R)/2  R-G  B	*/
	ent = EY[3] + EUV[0] + EY[2];
	if (ent < min_ent)
	{
		min_ent = ent;
		ct_type = B4_1;
		*Yi = 9; *UVj = 6;
		entropy[0] = EY[3];
		entropy[1] = EUV[0];
		entropy[2] = EY[2];
	}

	/* check  B<=>G: (B+R)/2  R-B  G	*/
	ent = EY[5] + EUV[2] + EY[0];
	if (ent < min_ent)
	{
		min_ent = ent;
		ct_type = B6_3;
		*Yi = 9; *UVj = 8;
		entropy[0] = EY[5];
		entropy[1] = EUV[2];
		entropy[2] = EY[0];
	}

	/* check R->G->B->R: (B+G)/2  G-B  R	*/
	ent = EY[4] + EUV[1] + EY[1];
	if (ent < min_ent)
	{
		min_ent = ent;
		ct_type = B5_2;
		*Yi = 9; *UVj = 7;
		entropy[0] = EY[4];
		entropy[1] = EUV[1];
		entropy[2] = EY[1];
	}

	/* check PEI09	*/
	ent = EY[9] + EUV[0] + EUV[12];
	if (ent < min_ent)
	{
		min_ent = ent;
		ct_type = PEI09;
		*Yi = 9; *UVj = 9;
		entropy[0] = EY[9];
		entropy[1] = EUV[0];
		entropy[2] = EUV[12];
	}

	/* check R->B->G->R: (R+B)/2  B-R  G  same as B6_3	*/


#ifdef VERBOSE2_
	printf( " %6.3f", min_ent);
#endif
	for ( i = 0; i < NUM_Y+UV_COMPO; i++)
	{
		free( YUV_data[i]);
	}
	return ct_type;
}
#endif /* no MODULO_SELECTION */