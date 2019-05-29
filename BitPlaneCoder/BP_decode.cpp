/*********************************************************************
* File	 	: BP_decode.cpp
* Purpose	: to decode binary decisions;
*		   	  to rebuild image from bit planes
* Author 	: Mathias Roeppischer
* Date	 	: 06.12.2013
* Changes	: QM-coder (decoder part); - Roeppischer (18.10.2013)
* Last Modif: 01.02.2014
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitbuf.h"
#include "BP.h"
#include "ginterface.h"
#include "qm_coder.h"
#include "tstypes.h"

/*--------------------------------------------------------------------
* undo_map()
*
* for brief description see: BP.h
*-------------------------------------------------------------------*/
void
undo_map( int *p)
{
	if ( !(*p) ) return;

	if ( !(*p & 1) ) *p >>= 1;
	else *p = -( (*p + 1) >> 1 );
}

/*--------------------------------------------------------------------
* G_decode()
*
* for brief description see: BP.h
*-------------------------------------------------------------------*/
void
G_decode( uint *g)
{
	/* let *g be a number with n binary places */
	uint b = *g;

	b >>= 1; /* let b be a *g variant without LSB */

	/* until b is a zero bit vector... */
	while ( b)
	{
		/*
		* ...compare bit values of b and *g (n - 1) times whereas
		* opposite values yield TRUE
		*/
		*g ^= b;

		b >>= 1; /* replace b by its variant without LSB */
	}
}

/*--------------------------------------------------------------------
* byte_in()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
byte_in( BitBuffer *bitbuf, cbool *u_flg, CDC_REG *reg, ulong
		*qm_bytes)
{
	byte
		cpr_byte;

	/* get another QM-compressed byte */
	if (*qm_bytes > 0)
	{
		/* read in new byte of compressed data */
		cpr_byte = BufferGetByte( bitbuf);

		/*
		* decrement number of bytes that are left until termination
		* of decoding
		*/
		(*qm_bytes)--;
	}
	/* signal termination of decoding via QM */
	else
	{
		/*
		* set flag to signal beginning of upcoming unencoded bit
		* stream part
		*/
		*u_flg = TRUE;
		
		/* pretend feeding QM decoder with zero byte */
		return;
	}

	/* fill byte has been detected */
	if (cpr_byte == 0xff)
	{
		/* insert X'FF byte into code register */
		(*reg).c |= 0xff00;
	}
	/* no fill byte has been detected */
	else
	{
		/* insert byte in high order 8 bits of C-low */
		(*reg).c += (cpr_byte << 8);
	}
}

/*--------------------------------------------------------------------
* renorm_dec()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
renorm_dec( BitBuffer *bitbuf, cbool *u_flg, CDC_REG *reg, ulong
		   *qm_bytes)
{
	do
	{
		/* code register has been shifted by 8 bits */
		if ((*reg).cnt == 0)
		{
			/* insert new byte into data bit section of C-low */
			byte_in( bitbuf, u_flg, reg, qm_bytes);

			(*reg).cnt = 8; /* reset SLL countdown */
		}

		(*reg).a <<= 1; /* double augend */

		/*
		* move compressed bit from position 15 of C-low to position 0
		* of Cx
		*/
		(*reg).c <<= 1;

		/* count down remaining SLLs until byte_in() is called */
		(*reg).cnt--;
	}
	/* repeat until size of probability interval at least 0.75 */
	while ( (*reg).a < 0x8000);
}

/*--------------------------------------------------------------------
* BA_decode()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
cbool
BA_decode( BitBuffer *bitbuf, cbool *u_flg, CDC_REG *reg, CTX_IDX
		  *ctx, ulong *qm_bytes)
{
	cbool
		bit_out = (*ctx).mps; /* decoded bit */

	(*reg).a -= (*ctx).q_val; /* calculate MPS sub-interval */

	/*
	* SRL of code register C by 16 bits to gain register Cx (high
	* order bits of C) for comparison with probability register A
	*
	* decode MPS for context index S unless conditional exchange
	* required
	*/
	if ( ((*reg).c >> 16) < (*reg).a )
	{
		/*
		* renormalization (on MPS path) required (size of augend less
		* than 0.75)
		*/
		if ((*reg).a < 0x8000)
		{
			/*
			* size of MPS sub-interval smaller than LPS probability
			* estimate
			*/
			if ((*reg).a < (*ctx).q_val)
			{
				/* assign reverse sense of MPS to decoded bit */
				bit_out = !bit_out;

				est_q_lps( ctx);
			}
			/*
			* size of MPS sub-interval at least equal to LPS
			* probability estimate
			*/
			else est_q_mps( ctx);
		}
		/*
		* neither renormalization nor conditional exchange required
		*/
		else
		{
			return bit_out; /* MPS shall be decoded */
		}
	}
	/*
	* decode LPS for context index S unless conditional exchange
	* required
	*/
	else
	{
		/* subtract augend from register C-x */
		(*reg).c -= (*reg).a << 16;

		/*
		* size of MPS sub-interval at least equal to LPS probability
		* estimate
		*/
		if ((*reg).a >= (*ctx).q_val)
		{
			/*
			* shrink size of probability interval to LPS probability
			* estimate
			*/
			(*reg).a = (*ctx).q_val;

			/* assign reverse sense of MPS to decoded bit */
			bit_out = !bit_out;

			est_q_lps( ctx);
		}
		/*
		* size of MPS sub-interval smaller than LPS probability
		* estimate
		*/
		else
		{
			/*
			* expand size of probability interval to LPS probability
			* estimate
			*/
			(*reg).a = (*ctx).q_val;

			est_q_mps( ctx);
		}
	}

	renorm_dec( bitbuf, u_flg, reg, qm_bytes);

	return bit_out;
}

/*--------------------------------------------------------------------
* init_dec()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
init_dec( BitBuffer *bitbuf, 
					cbool *u_flg, 
					CDC_REG *reg, 
					ulong *qm_bytes)
{
	/*
	* precision of probability interval register assumed to be not
	* limited to 16 bits
	*/
	(*reg).a = 0x10000;

	/* clear code register */
	(*reg).c = 0;

	/* fill data bits of C-low */
	byte_in( bitbuf, u_flg, reg, qm_bytes);

	(*reg).c <<= 8; /* shift 8 data bits from C-low into C-x */

	/* fill data bits of C-low */
	byte_in( bitbuf, u_flg, reg, qm_bytes);

	(*reg).c <<= 8; /* shift 8 data bits from C-low into C-x */

	/*
	* set SLL countdown to zero so that new data byte can be fetched
	* by renorm_dec()
	*/
	(*reg).cnt = 0;
}

/*--------------------------------------------------------------------
* BP_decode()
*
* for brief description see: BP.h
*-------------------------------------------------------------------*/
void
BP_decode( BitBuffer *bitbuf, cbool g_flg, cbool p_flg, PicData *pd)
{
	ulong
		/* number of bytes compressed via QMcoder */
		// *qm_bytes = (ulong *)malloc( sizeof( ulong)),
		qm_bytes,

		/*
		* current position of internal file position indicator
		* before decoding starts for each channel
		*/
		cur_pos,
		/* number of QM compressed bytes per channel */
		num_bytes[3] = {0, 0, 0};

	int
		/*
		* offset from end of bit stream used to read in number of
		* bytes that shall be decoded for each channel
		*/
		chan_off[3] = {0, 0, 0},
		cum_chan_off[3] = {0, 0, 0}, /* cumulative offset */
		k,
		l,
		sl, /* shift logical value for use with chan_off */
		s_PelVal = 0, /* signed pixel value */
		z; /* control variable for bit planes */

	uint
		*PelVal; /* pointer to (color) value of pel */

	rsize_t
		chan, /* control variable for image channel */
		ctxID, /* context instance identifier */
		firstBP[3], /* bit plane from which coding starts */
		j,
		my_stk, /* local stack variable */
		num_cBits, /* number of context bits */
		num_ctx, /* number of context instances */
		*stk, /* pointer to stack variable */
		x, /* control variable for horizontal pels */
		y; /* control variable for vertical pels */

	byte
		tmp,
		/* number of QM compressed bit planes per channel */
		num_bp[3];

	cbool
		bit_out, /* binary arithmetically decoded bit */
		s_flg, /* decode via QM (0) or read in unencoded data (1) */
		/*
		* QM decoder has decoded all bytes for this channel (1) or not
		* (0)
		*/
		// *u_flg = (cbool *)malloc( sizeof( cbool));
		u_flg;

	CTX_IDX
		*ctx, /* pointer to context instance structure */
		*ctx_state = NULL, /* entity of context instances */
		my_ctx; /* local context structure variable */

	CDC_REG
		my_cdc_reg, /* local codec register structure variable */
		*reg; /* pointer to encoder register structure */

	/* initialize pointers to structs */
	ctx = &my_ctx;
	reg = &my_cdc_reg;
	stk = &my_stk;

	/* initialize MSBP for each channel */
	// for ( j = 0; j < 3; j++) firstBP[j] = (*pd).bitperchan[j] - 1;
	for ( j = 0; j < (*pd).channel; j++) 
	{
		uint bpc = 0;
		uint maxV;
		maxV = 	pd->maxVal_pred[j];
		while( maxV)
		{
			maxV >>= 1;
			bpc++;
		}
		firstBP[j] = bpc - 1;
	}

	/* set fixed number of context bits */
	num_cBits = 13;

	/* a total of (2 to power of num_cBits) contexts */
	num_ctx = 1 << num_cBits;

	/* allocate memory for context instances */
	ALLOC( ctx_state, num_ctx, CTX_IDX);

	/* compute offsets for internal file position pointer */
	for ( k = (*pd).channel - 1; k > -1 ; k--)
	{
		/* number of data bits for corresponding channel */
		chan_off[k] = (*pd).size * (*pd).bitperchan[k];

		sl = 0; /* (re-)initialize shift value */

		/*
		* increment shift value until resulting power of two is
		* greater than number of data bits for current channel
		*/
		while ( (1 << sl) < chan_off[k] ) sl++;

		/* compute number of bytes that will be assigned to offset */
		chan_off[k] = sl >> 3;

		/* round up number of bytes if needed */
		if ( (chan_off[k] << 3) < sl) chan_off[k]++;

		for ( l = k; l < (int)(*pd).channel; l++)
		{
			/* compute cumulative offset */
			cum_chan_off[k] += chan_off[l];
		}
	}

	/* memorize currrent position in bit stream */
	cur_pos = ftell( (*bitbuf).file);

	/* apply offset to corresponding channel */
	fseek( (*bitbuf).file, -(cum_chan_off[0] + 2), SEEK_END);

	/*
	* read in first byte revealing the number of QM coded bit planes
	* for the first two channels
	*/
	tmp = (byte)getc( (*bitbuf).file);

	/* split up first byte into halves and extract number */
	num_bp[0] = ( tmp & 0x0f );
	num_bp[1] = ( tmp & 0xf0 ) >> 4;
	
	/*
	* read in second byte revealing the number of QM coded bit planes
	* for third channel
	*/
	num_bp[2] = (byte)getc( (*bitbuf).file);

	/* for all image channels */
	for ( j = 0; j < (*pd).channel; j++)
	{
		/* apply offset to corresponding channel */
		fseek( (*bitbuf).file, -cum_chan_off[j], SEEK_END);

		/* for the length of each offset... */
		for ( k = 0; k < chan_off[j]; k++)
		{
			/* ...get number of bytes to be decompressed */
			num_bytes[j] += getc( (*bitbuf).file) << (8 * k);
		}
	}

	/*
	* reset internal file position indicator to initial position
	*/
	fseek( (*bitbuf).file, cur_pos, SEEK_SET);

	/* for all image channels */
	for ( chan = 0; chan < (*pd).channel; chan++)
	{
		/* initialize QM byte count */
		qm_bytes = num_bytes[chan];

		/*
		* reset 'uncoded' flag to initiate countdown of QM bytes per
		* channel
		*/
		u_flg = FALSE;

		/* reset 'switch' flag to start decoding via QM */
		s_flg = FALSE;

		/* (re-)initialize binary arithmetic decoder */
		init_dec( bitbuf, &u_flg, reg, &qm_bytes);

		/* for all bit planes */
		for ( z = firstBP[chan]; z > -1; z--)
		{
			/* reset context if still decoding via QM */
			if (!s_flg)
			{
				(*ctx).mps = 0; /* reset sense of MPS */
				/* reset index of LPS probability estimate */
				(*ctx).q_idx = 0;
				/* reset LPS probability estimate */
				(*ctx).q_val = 0x5a1d;

				/*
				* advance pointer to context instance with each step
				*/
				for ( j = 0; j < num_ctx; j++)
				{
					memcpy( &(ctx_state[j]), ctx, sizeof( CTX_IDX));
				}
			}

			/*
			* 'uncoded' flag has been set AND all compressed bit
			* planes have been decoded -> read in unencoded bit plane
			* data
			*/
			if (u_flg && num_bp[chan] == 0) s_flg = TRUE;

			/* for all vertical pels */
			for ( y = 0; y < (*pd).height; y++)
			{
				/* for all horizontal pels */
				for ( x = 0; x < (*pd).width; x++)
				{	
					/* decode via QM */
					if (!s_flg)
					{
						/* identify context instance */
						ctxID = mod_con( (int)firstBP[chan], z, pd,
							chan, x, y);

						/* assign context instance via ID */
						ctx = &(ctx_state[ctxID]);

						/* binary arithmetic decoding via QM */
						bit_out = BA_decode( bitbuf, &u_flg, reg, ctx,
							&qm_bytes);

					}
					/* stop QM decoding */
					else
					{
						/* read in unencoded data bits */
						bit_out = BufferGetBit( bitbuf);
					}

					/* get pixel value at current position */
					PelVal = GetXYValue( pd, chan, x, y);

					/*
					* only decoded 1-bit requires inclusion;
					* first align, then include 1-bit in binary
					* representation of (color) value
					*/
					if (bit_out) *PelVal |= (bit_out << z);
				}
			}

			/*
			* decrement number of bit planes that shall be
			* decoded via QM for current channel
			*/
			if (num_bp[chan] > 0) num_bp[chan]--;
		} /* forfor ( z = firstBP[chan] */
	}/* for ( chan = 0;*/

	/* inverse mapping or GRAY decoding required */
	if (p_flg || g_flg)
	{
		for ( chan = 0; chan < (*pd).channel; chan++)
		{
			for ( y = 0; y < (*pd).height; y++)
			{
				for ( x = 0; x < (*pd).width; x++)
				{
					/*
					* get address of (color) value of current channel
					* at sequentialized two-dimensional position
					*/
					PelVal = GetXYValue( pd, chan, x, y);

					/* get back plain binary code */
					if (g_flg) G_decode( PelVal);

					/* prediction took place */
					if (p_flg)
					{
						/* prepare for signed pixel value */
						s_PelVal = *PelVal;

						undo_map( &s_PelVal);

						/* add offset */
						*PelVal = s_PelVal + (
							((*pd).maxVal_pred[chan] + 1) >> 1 );
					}
				}
			}
		}
	}

	/* deallocate memory for all context instances */
	FREE( ctx_state);

	/* deallocate memory for 'uncoded' flag */
	// FREE( u_flg);

	/*
	* deallocate memory for number of QM compressed bytes for last
	* channel
	*/
	// FREE( qm_bytes);
}