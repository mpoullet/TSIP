/*****************************************************************
*
* File....:	loco_vector_pred.c
* Function:	estimation of next vector based on LOCO for HXC2
* Author..:	Tilo Strutz
* Data....:	19.10.2015
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "hxc.h"
#include "codec.h" /* for max() */
#include "loco.h" /* for MODULO_RANGE(), CLIP() */

int *(q9L[4]);
long *lineL[4][2];

long NL[4][NUM_OF_LOCO_CONTEXTS+2], CL[4][NUM_OF_LOCO_CONTEXTS+2]; 
long AL[4][NUM_OF_LOCO_CONTEXTS+2], BL[4][NUM_OF_LOCO_CONTEXTS+2]; 
/*
CBD
AX
*/

/*---------------------------------------------------------------
 *	vec_pred_finish()
 *---------------------------------------------------------------*/
void vec_pred_finish( unsigned int numOfElements, long *data)
{
	unsigned int comp;

	FREE( data);
	for ( comp = 0; comp < numOfElements; comp++)
	{
		FREE( q9L[comp]);
	}
}

/*---------------------------------------------------------------
 *	vec_pred_init()
 *---------------------------------------------------------------*/
void vec_pred_init( unsigned int width, unsigned int numOfElements, 
									 long *low_lim, long *up_lim, long *range,
									 unsigned int *maxVal, long *data)
{
	unsigned int comp;
  int i, t1, t2, t3, fac, setA;
	
 
	ALLOC( data, (width + 4) * 2 * numOfElements, long);
	/* clean up the two lines */
	memset( data, 0, (width + 4) * 2 * numOfElements * sizeof(long));

	for ( comp = 0; comp < numOfElements; comp++)
	{
		lineL[comp][0] = data + 2 +  comp * 2 * (width + 4);
		lineL[comp][1] = data + 2 +  comp * 2 * (width + 4) + (width + 4);

		range[comp] = maxVal[comp] + 1;
		low_lim[comp] = -(range[comp] / 2);
		up_lim[comp]  = (range[comp] -1) / 2;

    /* Initialize the contexts. All uninitialized values are 0. */
    setA = max( 2, (range[comp] + 32) / 64);
    for ( i = 0; i < NUM_OF_LOCO_CONTEXTS+2; i++)
    {
     	AL[comp][i] = setA;
     	NL[comp][i] = 1;
     	BL[comp][i] = CL[comp][i] = 0;
    }

    /* Adaptive Initialisierung der Kontextschwellen	*/
    {
      if (maxVal[comp] > 4095)   	fac = (4096 + 128) / 256;
      else			fac = (maxVal[comp] + 128) / 256;
    
      if (fac)
      {
    		t1 = fac * (LOCO_S1 - 2) + 2;
    		t2 = fac * (LOCO_S1 - 3) + 3;
    		t3 = fac * (LOCO_S1 - 4) + 4;
      }
      else
      {
				/* pc->MAXVAL < 128; use RANGE as MAXVAL can be zero	*/
				t1 = LOCO_S1 / (256 / range[comp]);	if (t1 < 2) t1 = 2;
				t2 = LOCO_S1 / (256 / range[comp]);	if (t2 < 3) t2 = 3;
				t3 = LOCO_S1 / (256 / range[comp]);	if (t3 < 4) t3 = 4;
      }
      t1 += 3;
      t2 += 5;
      t3 += 7;
    }

		ALLOC( q9L[comp], maxVal[comp] * 2 + 1, int);

  	for ( i = -(signed)maxVal[comp]; i <= (signed)maxVal[comp]; i++)
  	{
	    if (i <= -t3)      	q9L[comp][i+maxVal[comp]] = -4;
	    else if (i <= -t2) 	q9L[comp][i+maxVal[comp]] = -3;
	    else if (i <= -t1) 	q9L[comp][i+maxVal[comp]] = -2;
	    else if (i <= -1)		q9L[comp][i+maxVal[comp]] = -1;
	    else if (i <=  0)		q9L[comp][i+maxVal[comp]] = 0;
	    else if (i <  t1)		q9L[comp][i+maxVal[comp]] = 1;
	    else if (i <  t2)		q9L[comp][i+maxVal[comp]] = 2;
	    else if (i <  t3)		q9L[comp][i+maxVal[comp]] = 3;
	    else								q9L[comp][i+maxVal[comp]] = 4;
  	}

	}
}

/*---------------------------------------------------------------
 *	vec_pred_update()
 *---------------------------------------------------------------*/
void vec_pred_update( int r, int c, long *X, long *Xhat, unsigned int numOfElements, 
										 int *sign, int *state, long *low_lim, long *up_lim,
										 long *range)
{
	long epsilon, reset = 64;
	unsigned int comp;
	for ( comp = 0; comp < numOfElements; comp++)
	{
		//long *Nr = &(NL[comp][0]);
		//long *Ar = &(AL[comp][0]);
		//long *Br = &(BL[comp][0]);
		//long *Cr = &(CL[comp][0]);
		unsigned int cx = state[comp];


		Lx = X[comp];

		if (sign[comp] < 0) 	epsilon = Xhat[comp] - X[comp];
		else 			epsilon = X[comp] - Xhat[comp];

		epsilon = MODULO_RANGE( epsilon, low_lim[comp],
						up_lim[comp], range[comp]);

		/* update the context */
		AL[comp][cx] += abs( epsilon);
		BL[comp][cx] += epsilon;

		/* Rescale N, A and B if the counts get too large */
		if (NL[comp][cx] == reset) 
		{ 
			AL[comp][cx] >>= 1; 
			NL[comp][cx] >>= 1; 
			if (BL[comp][cx] >= 0) BL[comp][cx] >>= 1; 
			else 	BL[comp][cx] = -((1 - BL[comp][cx]) >> 1);
		}

		/* Increment occurence count */
		NL[comp][cx]++; 

		/* Compute the bias correction term */
		if (BL[comp][cx] <= -NL[comp][cx] + 0)
		{
			if (CL[comp][cx] > -128) CL[comp][cx]--; 
			BL[comp][cx] += NL[comp][cx];
			if (BL[comp][cx] <= -NL[comp][cx]) BL[comp][cx] = -NL[comp][cx] + 1;
		} 
		else if (BL[comp][cx] > 0)
		{
			if (CL[comp][cx] < 127) CL[comp][cx]++; 
			BL[comp][cx] -= NL[comp][cx];
			if (BL[comp][cx] > 0) BL[comp][cx] = 0;
		} 
	}
}
/*---------------------------------------------------------------
 *	vec_pred_update()
 *---------------------------------------------------------------*/
void vec_pred_update___( int r, int c, long *X, long *Xhat, unsigned int numOfElements, 
										 int *sign, int *state, long *low_lim, long *up_lim,
										 long *range)
{
	long epsilon, reset = 64;
	unsigned int comp;
	for ( comp = 0; comp < numOfElements; comp++)
	{
		long *Nr2 = &(NL[comp][0]);
		long *Ar2 = &(AL[comp][0]);
		long *Br2 = &(BL[comp][0]);
		long *Cr2 = &(CL[comp][0]);
		unsigned int cx = state[comp];


		Lx = X[comp];

		if (sign[comp] < 0) 	epsilon = Xhat[comp] - X[comp];
		else 			epsilon = X[comp] - Xhat[comp];

		epsilon = MODULO_RANGE( epsilon, low_lim[comp],
						up_lim[comp], range[comp]);

		/* update the context */
		Ar2[cx] += abs( epsilon);
		Br2[cx] += epsilon;

		/* Rescale N, A and B if the counts get too large */
		if (Nr2[cx] == reset) 
		{ 
			Ar2[cx] >>= 1; 
			Nr2[cx] >>= 1; 
			if (Br2[cx] >= 0) Br2[cx] >>= 1; 
			else 	Br2[cx] = -((1 - Br2[cx]) >> 1);
		}

		/* Increment occurence count */
		Nr2[cx]++; 

		/* Compute the bias correction term */
		if (Br2[cx] <= -Nr2[cx] + 0)
		{
			if (Cr[cx] > -128) Cr[cx]--; 
			Br2[cx] += Nr2[cx];
			if (Br2[cx] <= -Nr2[cx]) Br2[cx] = -Nr2[cx] + 1;
		} 
		else if (Br2[cx] > 0)
		{
			if (Cr2[cx] < 127) Cr2[cx]++; 
			Br2[cx] -= Nr2[cx];
			if (Br2[cx] > 0) Br2[cx] = 0;
		} 
	}
}

/*---------------------------------------------------------------
 *	vec_pred()
 *---------------------------------------------------------------*/
void vec_pred( unsigned int width, int r, int c, unsigned int *maxVal,
								unsigned int numOfElements, long *Xhat, 
								int *state, int *sign, unsigned int *k)
{
	int cx;
	long d1, d2, d3;
	unsigned int comp;
	unsigned long maxAC, minAC;

	for ( comp = 0; comp < numOfElements; comp++)
	{
		if (c == 0)	/* linker Bildrand */
		{
			La = Lb;
			/* Lc ist jetz genau der Wert, 
			der in der vorangegangenen Zeile La war	*/
		}
		else if (c == (signed)width - 1)	/* rechter Bildrand */
		{
			Ld = Lb;
		}

		/* Compute the context number */
		d1 = Ld - Lb;
		d2 = Lb - Lc;	/* muss immer berechnet werden, da nach RLC
									* keine gultiger Wert fuer d2 = d1	*/
		d3 = Lc - La;

		cx = 9 * (9 * q9L[comp][d1 + maxVal[comp]]  + 
								  q9L[comp][d2 + maxVal[comp]]) + 
								  q9L[comp][d3 + maxVal[comp]];

		/* Convert to sign/magnitude. */
		sign[comp] = cx < 0 ? -1 : (cx > 0 ? 1 : 0);
		cx = abs( (int)cx);
		state[comp] = cx;


		/* Predict the next value. */
		maxAC = max( La, Lb); 
		minAC = min( La, Lb);

		if (Lc >= (long)maxAC)			Xhat[comp] = minAC;
		else if (Lc <= (long)minAC)	Xhat[comp] = maxAC;
		else			Xhat[comp] = La + Lb - Lc;

		if (state[comp] != 0) /* normally cx==0 forces rlc mode and cx is here always >0	*/
		{
			if (sign[comp] > 0)	Xhat[comp] += CL[comp][cx]; 
			else			Xhat[comp] -= CL[comp][cx]; 
		}
		Xhat[comp] = CLIP( Xhat[comp], 0, (long)maxVal[comp]);

		/* Compute Rice coding paramter */
		/* for (k = 0; (N[cx] << k) < A[cx]; k++); 	*/
		/* "<=" bringt bessere Ergebnisse als nur "<"	*/
		{
			register int nst = NL[comp][cx], At = AL[comp][cx];
			for ( k[comp] = 0; nst < At; nst <<= 1, k[comp]++, assert( k[comp] < 99));
		}

	}
}
