/*****************************************************************
*
* Datei:	loco_predictor.c
* Funktion:	Berechnung des Schaetzwertes fuer JPEG-LS
* Autor:	Tilo Strutz
* Datum: 	23.05.2001
* 20.5.2011 Strutz: no static declararion for d1, d2, d3
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "loco.h"

char *q9R;
/*
CBD
AX
*/

/*---------------------------------------------------------------
 *	predictor()
 *---------------------------------------------------------------*/
void predictor( IMAGE_LOCO *im, OPTION *op, CON *pc, PRED_STRUC *pp, 
							 int r, int c)
{
	unsigned long maxAC, minAC;
	//static long d1, d2, d3;
	int Q4;
	long d1, d2, d3;

	if (c == 0)	/* linker Bildrand */
	{
		Ra = Rb;
		/* Rc ist jetz genau der Wert, 
		der in der vorangegangenen Zeile Ra war	*/
	}
	else if (c == (signed)im->width - 1)	/* rechter Bildrand */
	{
		Rd = Rb;
	}

	/* Compute the context number */
	d1 = Rd - Rb;
	d2 = Rb - Rc;	/* muss immer berechnet werden, da nach RLC
								* keine gultiger Wert fuer d2 = d1	*/
	d3 = Rc - Ra;

	pp->state = 9 * (9 * q9R[d1 + pc->MAXVAL]  + 
			q9R[d2 + pc->MAXVAL]) + q9R[d3 + pc->MAXVAL];

	/* Convert to sign/magnitude. */
	pp->sgn = pp->state < 0 ? -1 : (pp->state > 0 ? 1 : 0);
	pp->state = (short)abs( pp->state);

	if (op->T4_flag)
	{
		// Q4 = ((Ra + Rb + Rc) / 3) > op->T4; /* logical expression	*/
		//Q4 = ((Ra + Rb+ Rc+ Rd) >> 2) > op->T4; /* logical expression	*/
		Q4 = ((Ra + Rb) >> 1) > op->T4; /* logical expression	*/
		//Q4 = Ra > op->T4; /* logical expression	*/
		pp->state = (short)(pp->state * 2 + Q4);
	}

	/* Test auf RLC-Modus	*/
#ifdef _DEBUG_XXXX
	if ( 0) 
#else
	if ( (abs( d1) <= pc->NEAR) && (abs( d2) <= pc->NEAR) &&
		(abs( d3) <= pc->NEAR) && op->rlc_flag) 
#endif
	{ 
		pp->rlc_flag = 1;
	}
	else /* Context mode. */
	{
		pp->rlc_flag = 0;

		/* Predict the next value. */
		maxAC = max( Ra, Rb); 
		minAC = min( Ra, Rb);

		if (Rc >= (long)maxAC)	pp->xHat = minAC;
		else if (Rc <= (long)minAC)	pp->xHat = maxAC;
		else			pp->xHat = Ra + Rb - Rc;
	}
}
