/*********************************************************************
* File	 	: QM_encode.cpp
* Purpose	: to decompose image into bit planes;
*		 	  to encode binary decisions
* Author 	: Mathias Roeppischer
* Date	 	: 06.12.2013
* Changes	: QM-coder (encoder part); - Roeppischer (18.10.2013)
* Last Modif: 01.02.2014
* 31.03.2014 Strutz: write into buffer array not into file
*********************************************************************/
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitbuf.h"
#include "BP.h"
#include "ginterface.h"
#include "qm_coder.h"
#include "tstypes.h"

/*--------------------------------------------------------------------
* byte_out()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
byte_out( unsigned char *bit_array, 
				 byte *nxt_byte, /* carry bits */
				 CDC_REG *reg,
		 rsize_t *stk, 
		 ulong *qm_bytes /* number of written bytes	*/
		 )
{
	uword
		temp; /* temporarily stores output byte and carry bit */

	/*
	* halve code register 19 times to align output bits with low
	* order bits of temp
	*/
	temp = reg->c >> 19;

	/*
	* clear high order 13 bits of code register because first 8 bits
	* of them were assigned to new byte and last 4 bits shall
	* always be 0 and carry bit shall not be set
	*/
	reg->c &= 0x7ffff;

	/* carry-over has occured */
	if (temp > 0xff)
	{
		/* add carry bit to next compressed byte */
		(*nxt_byte)++;

		/* next compressed byte was X'FE byte and is now fill byte */
		if (*nxt_byte == 0xff)
		{
			/* write X'FF byte to bit stream */
			bit_array[*qm_bytes] = 0xff;
			/*
			* increment number of QM compressed bytes for current
			* channel
			*/
			(*qm_bytes)++;
		}

		/*
		* place stacked bytes converted to 0s by carry-over in
		* entropy-coded segment
		*/
		while ( *stk > 0)
		{
			/*
			* write next compressed byte to bit stream if it is not a
			* fill byte
			*/
			if (*nxt_byte < 0xff)
			{
				bit_array[*qm_bytes] = (*nxt_byte);
				/* one more QM compressed byte for current channel */
				(*qm_bytes)++;
			}

			/* next compressed byte will be stacked 0-byte */
			*nxt_byte = 0x00;

			(*stk)--; /* decrement stack counter */
		}

		/*
		* remove carry bit from temporary variable before assigning it
		* to new byte of compressed data
		*/
		temp &= 0xff;
	}
	/* no carry-over has occured */
	else
	{
		/* output byte is fill byte */
		if (temp == 0xff)
		{
			/*
			* increment stack counter to delay data output until
			* carry-over has been resolved
			*/
			(*stk)++;

			return;
		}
		/* carry-over has been resolved */
		else
		{
			/*
			* place stacked X'FF bytes in entropy-coded segment after
			* carry-over has been resolved
			*/
			while ( *stk > 0)
			{
				/*
				* write next compressed byte toarrayif it is
				* not a fill byte
				*/
				if (*nxt_byte < 0xff)
				{
					bit_array[*qm_bytes] = (*nxt_byte);

					/*
					* one more QM compressed byte for current channel
					*/
					(*qm_bytes)++;
				}

				/* write stacked fill byte to array */
				bit_array[*qm_bytes] = 0xff;

				/* one more QM compressed byte for current channel */
				(*qm_bytes)++;

				(*stk)--; /* decrement stack counter */

				/*
				* pretend handing over X'FF byte as if it were next
				* compressed byte
				*/
				*nxt_byte = 0xff;
			}
		}
	}

	/*
	* write next compressed byte to array if it is not a fill
	* byte
	*/
	if (*nxt_byte < 0xff)
	{
		//BufferPutByte( bitbuf_var, (*nxt_byte));
		bit_array[*qm_bytes] = (*nxt_byte);

		/* one more QM compressed byte for current channel */
		(*qm_bytes)++;
	}

	/* assign value of temporary byte to next compressed byte */
	*nxt_byte = (byte)temp;
}

/*--------------------------------------------------------------------
* renorm_enc()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
renorm_enc( unsigned char *bit_array, 
						byte *nxt_byte,	 /* carry bits */
						CDC_REG *reg,
						rsize_t *stk, 
						ulong *qm_bytes /* number of written bytes	*/
			 )
{
	do
	{
		/* double probability interval register */
		reg->a <<= 1;

		reg->c <<= 1; /* double code register */

		reg->cnt--; /* decrement remaining SLLs */

		/* at least 8 shift lefts have been done */
		if (reg->cnt == 0)
		{
			/* remove byte of compressed data from code register */
			byte_out( bit_array, nxt_byte, reg, stk, qm_bytes);

			reg->cnt = 8; /* reset countdown */
		}
	}
	/* repeat renormalization until size of augend is at least 0.75 */
	while ( reg->a < 0x8000);
}

/*--------------------------------------------------------------------
* BA_encode()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
BA_encode( unsigned char *bit_array, /* array for compressed data	*/
					byte *nxt_byte,  /* carry bits */
					cbool bit_in,	/* current bit to be compressed	*/
					CDC_REG *reg, 
					CTX_IDX *ctx, /* current context	*/
					rsize_t *stk, 
					ulong *qm_bytes /* number of written bytes	*/
			)
{
	/* calculate current MPS sub-interval */
	reg->a -= ctx->q_val;

	/* encode more probable symbol */
	if (ctx->mps == bit_in)
	{
		/* augend needs to be renormalized */
		if (reg->a < 0x8000)
		{
			/*
			* MPS sub-interval smaller than LPS probability estimate
			*/
			if (reg->a < ctx->q_val)
			{
				/* add MPS sub-interval to code register */
				reg->c += reg->a;

				/* expand augend to LPS probability estimate */
				reg->a = ctx->q_val;
			}

			est_q_mps( ctx);
		}
		else return;
	}
	/* encode less probable symbol */
	else
	{
		/*
		* size of augend is at least equal to LPS probability estimate
		*/
		if (reg->a >= ctx->q_val)
		{
			/* add MPS sub-interval to code register */
			reg->c += reg->a;

			/* shrink augend to LPS probability estimate */
			reg->a = ctx->q_val;
		}

		est_q_lps( ctx);
	}

	renorm_enc( bit_array, nxt_byte, reg, stk, qm_bytes);
}

/*--------------------------------------------------------------------
* init_enc()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
init_enc( CDC_REG *reg, rsize_t *stk)
{
	*stk = 0; /* clear stack counter */

	/*
	* precision of probability interval register assumed to be not
	* limited to 16 bits;
	* initialize augend with smallest 17 bit number (1.5)
	*/
	reg->a = 0x10000;

	/*
	* clear code register;
	* upper four bits of code register guaranteed to be 0 at any time
	*/
	reg->c = 0;

	/*
	* initialize SLL countdown on renormalization;
	* 3 spacer bits + 8 output bits shall be filled in code register
	* before first byte can be removed
	*/
	reg->cnt = 11;
}

/*--------------------------------------------------------------------
* flush()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
flush( unsigned char *bit_array, 
			byte *nxt_byte, 
			CDC_REG *reg, 
			rsize_t *stk, 
			ulong *qm_bytes  /* number of written bytes	*/
			)
{
	uint
		/* temporary variable for comparison with code register */
		temp;

	/*
	* set as many low order bits of code register as possible to 0
	* without pointing outside of final interval
	*
	* assign 32 bit code register plus 16 bit augend minus 1 to temp
	* thus leaving high order 16 bits unchanged in case of
	* Cx + A <= 0x10000 (= 1.5)
	*/
	temp = reg->c + reg->a - 1;

	temp &= 0xffff0000; /* clear low order 16 bits of temp */

	/*
	* first operation has had no effect on high order 16 bits of temp,
	* i.e. value of Cx is at most 0.75
	*/
	if (temp < reg->c)
	{
		/*
		* add 0.75 to value of temp to make sure it is at least equal
		* to value of code register
		*/
		temp += 0x8000;
	}

	/*
	* at least first 15 bits of low order 16 bits of code register are
	* now set to 0
	*
	* align output byte by shifting it (SLL countdown) times
	*/
	reg->c = temp << reg->cnt;

	/* remove byte from code register */
	byte_out( bit_array, nxt_byte, reg, stk, qm_bytes);

	/* shift code register by 8 bits to align 2nd output byte */
	reg->c <<= 8;

	/* remove byte from code register */
	byte_out( bit_array, nxt_byte, reg, stk, qm_bytes);

	/* remaining low order bits in code register are 0-bits */

	/*
	* write last saved compressed byte to bit stream since occurence
	* of carry-over is now impossible
	*/
	//BufferPutByte( bitbuf_var, (*nxt_byte));
	bit_array[*qm_bytes] = *nxt_byte;

	/* one more QM compressed byte for current channel */
	(*qm_bytes)++;


	/* check for trailing zero bytes */
	while (bit_array[*qm_bytes - 1] == 0x00)
	{
		/* subtract each zero byte from final channel byte count */
		(*qm_bytes)--;
	}
}

/*--------------------------------------------------------------------
* G_encode()
*
* for brief description see: BP.h
*-------------------------------------------------------------------*/
void
G_encode( uint *b)
{
	/*
	* let *b be a number with n binary places;
	* compare *b to its variant without LSB ((n - 1) binary places)
	* whereas opposite bit values yield TRUE
	*/
	*b ^= (*b >> 1);
}

/*--------------------------------------------------------------------
* map()
*
* for brief description see: BP.h
*-------------------------------------------------------------------*/
int map( int p)
{
	if (p >= 0) p <<= 1;
	else p = ((-p) << 1) - 1; /* shift a non-negative integer number	*/
	return p;
}
