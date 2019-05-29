/*****************************************************
 * File..: med.c
 * Desc..: Implementation MED predictor
 * Author: Tilo Strutz
 * Date..: 19.04.2013
 ******************************************************/
#include <stdlib.h>
#include <string.h>
#include "med_.h"
#include "codec_CoBaLP.h"

/*---------------------------------------------------------------
 *	med_predictor()
 *---------------------------------------------------------------*/
float med_predictor_( TEMPLATE *templ, long min_value, long max_value)
{
	long maxAB, minAB, xHat;
	
	/*  C B D
	 *  A X
	 */
		
	/* Predict the next value. */
	maxAB = max( templ->R[0], templ->R[1]); 
	minAB = min( templ->R[0], templ->R[1]);

	if (templ->R[2] >= maxAB)	xHat = minAB;
	else if (templ->R[2] <= minAB)	xHat = maxAB;
	else
	{
		xHat = templ->R[0] + templ->R[1] - templ->R[2];
		xHat = CLIP( xHat, min_value, max_value);
	}

	return (float)xHat;
}
