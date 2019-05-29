/*********************************************************************
* File	 	: BP_utility.c
* Purpose	: to declare functions that are jointly used by the QM
*		 	  binary arithmetic encoder and decoder
* Author 	: Mathias Roeppischer
* Date	 	: 04.11.2013
* Last Modif: 01.02.2014
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qm_coder.h"
#include "ginterface.h"

/*--------------------------------------------------------------------
* est_q_mps()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
est_q_mps( CTX_IDX *ctx)
{
	/* get new index for LPS probability estimate */
	ctx->q_idx = Qtbl[ctx->q_idx].nxt_idx_mps;

	/* identify new LPS probability estimate */
	ctx->q_val = Qtbl[ctx->q_idx].q_val;
}

/*--------------------------------------------------------------------
* est_q_lps()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
est_q_lps( CTX_IDX *ctx)
{
	/*
	* invert sense of MPS for context index S
	*/
	if (Qtbl[ctx->q_idx].swt_mps == 1) ctx->mps = !ctx->mps;

	/* get new index for LPS probability estimate */
	ctx->q_idx = Qtbl[ctx->q_idx].nxt_idx_lps;

	/* identify new LPS probability estimate */
	ctx->q_val = Qtbl[ctx->q_idx].q_val;
}

/*--------------------------------------------------------------------
* mod_con()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
rsize_t
mod_con( int firstBP, int z, 
				PicData *pd, 
				rsize_t chan, 
				rsize_t x,
				rsize_t y)
{
	int
		k, /* bit plane control variable */
		pos_x, /* horizontal position of context bit */
		pos_y, /* vertical position of context bit */
		r, /* radius of circle that surrounds context bits */
		sl = -1, /* shift value for computation of instance ID */
		u, /* horizontal pel component */
		v; /* vertical pel component */

	rsize_t
		/*
		* initialize in regard to number of bit planes already
		* processed and number of current bit plane
		*/
		inst_id = 0;

	/* initialize context radius */
	(z == firstBP) ? (r = 2) : (r = 1);

	bool
		b_flg; /* break flag for loop of vertical positions */

	/* for previous and current bit plane */
	//for ( k = z + 1; k > z - 1; k--)
	for ( k = z + 1; k >= z; k--)
	{
		/* use 2D context model for first bit plane */
		if (z == firstBP) k = z;

		/* for all vertical positions */
		for ( v = -r; v <= r; v++)
		{
			b_flg = false; /* reset break flag */

			/* for all horizontal positions */
			for ( u = -r; u <= r; u++)
			{
				/*
				* increment shift value so that each bit in template
				* window can be uniquely identified
				*/
				sl++;

				/* calculate context bit position within image */
				pos_x = x + u;
				pos_y = y + v;

				/*
				* validate context bit position;
				* go to next column if position invalid
				*/
				if ( (pos_x < 0) || (pos_x > (int)pd->width  - 1) ||
					   (pos_y < 0) || (pos_y > (int)pd->height - 1) )
					 continue;

				/* context bits */
				/*
				* A  = (-1,-2,  z)
				* B  = ( 0,-2,  z)
				* C  = ( 1,-2,  z)
				* D  = (-2,-1,  z)
				* E  = (-1,-1,  z)
				* E' = (-1,-1,z+d)
				* F  = ( 0,-1,  z)
				* F' = ( 0,-1,z+d)
				* G  = ( 1,-1,  z)
				* G' = ( 1,-1,z+d)
				* H  = ( 2,-1,  z)
				* I  = (-2, 0,  z)
				* J  = (-1, 0,  z)
				* J' = (-1, 0,z+d)
				* X' = ( 0, 0,z+d)
				* K' = ( 1, 0,  z)
				* N' = (-1, 1,  z)
				* O' = ( 0, 1,  z)
				* P' = ( 1, 1,  z)
				*/

				/* heed template restrictions in current bit plane */
				if (k == z)
				{
					/* current bit plane is first bit plane */
					if (z == firstBP)
					{
						/*
						* go to next column if top left corner of
						* template window
						*/
						if (u == -2 && v == -2) continue;

						/*
						* MSBP -> LSBP with fixed 9-bit context;
						* go to next line if C has been reached
						*/
						if (u > 0 && v == -2) break;
					}

					/*
					* go to end of procedure if position of current
					* pixel to be encoded has been reached
					*/
					if (u > -1 && v > -1)
					{
						b_flg = true;

						break;
					}
				}
				/* heed template restrictions in upper bit plane */
				else
				{
					/*
					* sieving for upper bit plane that has already
					* been encoded;
					* compare LSB of horizontal and vertical offset
					* via AND -> corner coordinates yield true (o);
					* 'x' marks template positions that will be used
					* in modeling the context
					*
					* |o|x|o|
					* |x|x|x|
					* |o|x|o|
					*/
					if ( ((u & 1) & (v & 1)) == TRUE ) continue;
				}

				/* compute context instance ID */
				inst_id += ((*GetXYValue( pd, chan, pos_x, pos_y) >>
					k) & 1) << sl;
			}

			if (b_flg) break; /* go to next bit plane */
		}
	}

	return inst_id;
}