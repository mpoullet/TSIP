/*********************************************************************
* File	 	: BP_encode.cpp
* Purpose	: to decompose image into bit planes;
*		 	  to encode binary decisions
* Author 	: Mathias Roeppischer
* Date	 	: 06.12.2013
* Changes	: QM-coder (encoder part); - Roeppischer (18.10.2013)
* Last Modif: 01.02.2014
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
byte_out( BitBuffer *bitbuf_var, byte *nxt_byte, CDC_REG *reg,
		 rsize_t *stk, ulong *qm_bytes)
{
	uword
		temp; /* temporarily stores output byte and carry bit */

	/*
	* halve code register 19 times to align output bits with low
	* order bits of temp
	*/
	temp = (*reg).c >> 19;

	/*
	* clear high order 13 bits of code register because first 8 bits
	* of them were assigned to new byte and last 4 bits shall
	* always be 0 and carry bit shall not be set
	*/
	(*reg).c &= 0x7ffff;

	/* carry-over has occured */
	if (temp > 0xff)
	{
		/* add carry bit to next compressed byte */
		(*nxt_byte)++;

		/* next compressed byte was X'FE byte and is now fill byte */
		if (*nxt_byte == 0xff)
		{
			/* write X'FF byte to bit stream */
			BufferPutByte( bitbuf_var, 0xff);

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
				BufferPutByte( bitbuf_var, (*nxt_byte));

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
				* write next compressed byte to bit stream if it is
				* not a fill byte
				*/
				if (*nxt_byte < 0xff)
				{
					BufferPutByte( bitbuf_var, (*nxt_byte));

					/*
					* one more QM compressed byte for current channel
					*/
					(*qm_bytes)++;
				}

				/* write stacked fill byte to bit stream */
				BufferPutByte( bitbuf_var, 0xff);

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
	* write next compressed byte to bit stream if it is not a fill
	* byte
	*/
	if (*nxt_byte < 0xff)
	{
		BufferPutByte( bitbuf_var, (*nxt_byte));

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
renorm_enc( BitBuffer *bitbuf_var, byte *nxt_byte, CDC_REG *reg,
		   rsize_t *stk, ulong *qm_bytes)
{
	do
	{
		/* double probability interval register */
		(*reg).a <<= 1;

		(*reg).c <<= 1; /* double code register */

		(*reg).cnt--; /* decrement remaining SLLs */

		/* at least 8 shift lefts have been done */
		if ((*reg).cnt == 0)
		{
			/* remove byte of compressed data from code register */
			byte_out( bitbuf_var, nxt_byte, reg, stk, qm_bytes);

			(*reg).cnt = 8; /* reset countdown */
		}
	}
	/* repeat renormalization until size of augend is at least 0.75 */
	while ( (*reg).a < 0x8000);
}

/*--------------------------------------------------------------------
* BA_encode()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
BA_encode( BitBuffer *bitbuf_var, byte *nxt_byte, cbool bit_in,
		  CDC_REG *reg, CTX_IDX *ctx, rsize_t *stk, ulong *qm_bytes)
{
	/* calculate current MPS sub-interval */
	(*reg).a -= (*ctx).q_val;

	/* encode more probable symbol */
	if ((*ctx).mps == bit_in)
	{
		/* augend needs to be renormalized */
		if ((*reg).a < 0x8000)
		{
			/*
			* MPS sub-interval smaller than LPS probability estimate
			*/
			if ((*reg).a < (*ctx).q_val)
			{
				/* add MPS sub-interval to code register */
				(*reg).c += (*reg).a;

				/* expand augend to LPS probability estimate */
				(*reg).a = (*ctx).q_val;
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
		if ((*reg).a >= (*ctx).q_val)
		{
			/* add MPS sub-interval to code register */
			(*reg).c += (*reg).a;

			/* shrink augend to LPS probability estimate */
			(*reg).a = (*ctx).q_val;
		}

		est_q_lps( ctx);
	}

	renorm_enc( bitbuf_var, nxt_byte, reg, stk, qm_bytes);
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
	(*reg).a = 0x10000;

	/*
	* clear code register;
	* upper four bits of code register guaranteed to be 0 at any time
	*/
	(*reg).c = 0;

	/*
	* initialize SLL countdown on renormalization;
	* 3 spacer bits + 8 output bits shall be filled in code register
	* before first byte can be removed
	*/
	(*reg).cnt = 11;
}

/*--------------------------------------------------------------------
* flush()
*
* for brief description see: qm_coder.h
*-------------------------------------------------------------------*/
void
flush( unsigned char *bit_array, byte *nxt_byte, CDC_REG *reg, rsize_t
	  *stk, ulong *qm_bytes)
{
	uint
		/* temporary variable for comparison with code register */
		temp;

	byte
		last_byte;

	/*
	* set as many low order bits of code register as possible to 0
	* without pointing outside of final interval
	*
	* assign 32 bit code register plus 16 bit augend minus 1 to temp
	* thus leaving high order 16 bits unchanged in case of
	* Cx + A <= 0x10000 (= 1.5)
	*/
	temp = (*reg).c + (*reg).a - 1;

	temp &= 0xffff0000; /* clear low order 16 bits of temp */

	/*
	* first operation has had no effect on high order 16 bits of temp,
	* i.e. value of Cx is at most 0.75
	*/
	if (temp < (*reg).c)
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
	(*reg).c = temp << (*reg).cnt;

	/* remove byte from code register */
	byte_out( bitbuf_var, nxt_byte, reg, stk, qm_bytes);

	/* shift code register by 8 bits to align 2nd output byte */
	(*reg).c <<= 8;

	/* remove byte from code register */
	byte_out( bitbuf_var, nxt_byte, reg, stk, qm_bytes);

	/* remaining low order bits in code register are 0-bits */

	/*
	* write last saved compressed byte to bit stream since occurence
	* of carry-over is now impossible
	*/
	BufferPutByte( bitbuf_var, (*nxt_byte));

	/* one more QM compressed byte for current channel */
	(*qm_bytes)++;

	/* check for trailing zero bytes */
	fseek( (*bitbuf_var).file, -1, SEEK_CUR);

	/* read in last byte from bit stream */
	last_byte = (byte)fgetc((*bitbuf_var).file);

	/* for all trailing zero bytes... */
	while ( last_byte == 0x00)
	{
		/* ...move back internal file position indicator */
		fseek( (*bitbuf_var).file, -2, SEEK_CUR);
		
		/* read in next byte from bit stream */
		last_byte = (byte)fgetc((*bitbuf_var).file);

		/* subtract each zero byte from final channel byte count */
		(*qm_bytes)--;
	}
	
	/* adjust internal file position indicator correctly */
	fseek( (*bitbuf_var).file, 0, SEEK_CUR);
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
void
map( int *p)
{
	if (*p >= 0) *p <<= 1;
	//else *p = -( (*p << 1) + 1 );
	else *p = ((-(*p)) << 1) - 1; /* shift a non-negative integer number	*/
}

/*--------------------------------------------------------------------
* BP_encode()
*
* for brief description see: BP.h
*-------------------------------------------------------------------*/
void
BP_encode( BitBuffer *bitbuf, cbool g_flg, cbool p_flg, PicData *pd)
{
	ulong
		/* counter of QM compressed bytes per bit plane */
		//*qm_bytes = (ulong *)malloc( sizeof( ulong)),
		qm_bytes = 0,
		/* number of QM compressed bytes per channel */
		num_bytes[3] = {0, 0, 0},
		qm_bits; /* QM coded bits per bit plane */

	int
		s_PelVal = 0, /* signed pixel value */
		z; /* control variable for bit planes */

	uint
		*PelVal; /* pointer to (color) value of pel */

	rsize_t
		chan, /* control variable for image channels */
		ctxID, /* context instance identifier */
		firstBP[3], /* bit plane from which coding starts */
		j,
		/* bit mask to decompose number of bytes per channel */
		mask = 0xff,
		my_stk, /* local stack variable */
		num_cBits, /* number of context bits */
		num_ctx, /* number of context instances */
		/*
		* offset in reference to beginning of next channel footer part
		*/
		offset,
		sl, /* shift logical value in order to compute offset */
		*stk, /* pointer to stack variable */
		x, /* control variable for horizontal pels */
		y; /* control variable for vertical pels */

	char
		/* name of file containing raw bit plane data */
		*bp_data = "bit_plane_raw.bin",
		/* name of file containing compressed QM bytes */
		*bp_qm = "bit_plane_coded.bin";

	byte
		/* number of QM compressed bit planes per channel */
		num_bp[3] = {0, 0, 0},
		/*
		* allocate memory for first compressed byte that may later be
		* modified by carry-over
		*/
		// *nxt_byte = (byte *)malloc( sizeof( byte));
		nxt_byte;

	cbool
		bit_in, /* value of bit to be encoded */
		/*
		* data bit from auxiliary bit buffer that must be written to
		* bit stream
		*/
		tmp_bit,
		/*
		* write unencoded bit plane data to bit stream (1) or not (0)
		*/
		//*u_flg = (cbool *)malloc( sizeof( cbool));
		u_flg;

	BitBuffer
		*bitbuf_data = NULL, /* bit buffer for unencoded data bits */
		*bitbuf_qm = NULL; /* bit buffer for QM coded bits */

	CTX_IDX
		*ctx, /* pointer to context instance structure */
		*ctx_state = NULL, /* entity of context instances */
		my_ctx; /* local context structure variable */

	CDC_REG
		my_cdc_reg, /* local codec register structure variable */
		*reg; /* pointer to decoder register structure */

	/* initialize pointers to structs */
	ctx = &my_ctx;
	reg = &my_cdc_reg;
	stk = &my_stk;

	/* initialize MSBP for all channels */
	//for ( j = 0; j < 3; j++) firstBP[j] = (*pd).bitperchan[j] - 1;
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

	/* inverse mapping or GRAY decoding required */
	if (p_flg || g_flg)
	{
		/* for all image channels */
		for ( chan = 0; chan < (*pd).channel; chan++)
		{
			/* for all vertical pels */
			for ( y = 0; y < (*pd).height; y++)
			{
				/* for all horizontal pels */
				for ( x = 0; x < (*pd).width; x++)
				{
					/*
					* get address of (color) value of current channel
					* at sequentialized two-dimensional position
					*/
					PelVal = GetXYValue( pd, chan, x, y);

					if (p_flg)
					{
						/* subtract offset */
						s_PelVal = *PelVal - (
							((*pd).maxVal_pred[chan] + 1) >> 1 );

						map( &s_PelVal);

						/* get back unsigned pixel value */
						*PelVal = s_PelVal;
					}

					/* map pixel value to GRAY code */
					if (g_flg) G_encode( PelVal);
				}
			}
		}
	}

	/* set fixed number of context bits */
	num_cBits = 13;

	/* a total of (2 to power of num_cBits) contexts */
	num_ctx = 1 << num_cBits;

	/* allocate memory for context instances */
	ALLOC( ctx_state, num_ctx, CTX_IDX);

	/* for all image channels */
	for ( chan = 0; chan < (*pd).channel; chan++)
	{
		fprintf( stderr, "\n channel: %d", chan);
		/*
		* initial value of compressed byte that will be be overwritten
		* at first chance in byte_out()
		*/
		nxt_byte = 0xff;

		/* initialize binary arithmetic encoder */
		init_enc( reg, stk);

		/* reset 'uncoded' flag */
		u_flg = FALSE;

		/* for all bit planes */
		for ( z = firstBP[chan]; z > -1; z--)
		{
			fprintf( stderr, "\n   bitplane %d: ", z);

			/* (re)-initialize context while still encoding via QM */
			if (!u_flg)
			{
				/* open file stream to compressed QM bytes */
		 		bitbuf_data = BufferOpenWrite( bp_data);

				/* open file stream to unencoded data bits */
				bitbuf_qm = BufferOpenWrite( bp_qm);

				/*
				* reset number of QM compressed bytes before bit plane
				* encoding
				*/
				qm_bytes = 0;

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

			/* for all vertical pels */
			for ( y = 0; y < (*pd).height; y++)
			{
				/* for all horizontal pels */
				for ( x = 0; x < (*pd).width; x++)
				{
					/* get address of pel value */
					PelVal = GetXYValue( pd, chan, x, y);

					/*
					* first align, then extract bit in current plane
					*/
					bit_in = (cbool)((*PelVal >> z) & 1);

					/* encode via QM */
					if (!u_flg)
					{
						/* write data bit to auxiliary bit buffer */
						BufferPutBit( bitbuf_data, bit_in);

						/* identify context instance */
						ctxID = mod_con( (int)firstBP[chan], z, pd,
							chan, x, y);

						/* assign context instance via ID */
						ctx = &(ctx_state[ctxID]);

						/* encode binary arithmetically */
						BA_encode( bitbuf_qm, &nxt_byte, bit_in, reg,
							ctx, stk, &qm_bytes);
					}
					else
					{
						/*
						* transfer raw bit plane bits directly to bit
						* stream
						*/
						BufferPutBit( bitbuf, bit_in);
					}
				}
			} /* for y	*/

			/*
			* end of bit plane: compare sizes of auxiliary buffers if
			* QMcoder has been deployed
			*/
			if (!u_flg)
			{
				/*
				* compute number of QM coded bits for current bit
				* plane
				*/
				qm_bits = qm_bytes << 3;

				/* QMcoder procuced more bits than it was fed */
				if ( qm_bits > (*pd).size)
				{
					/* terminate binary arithmetic coding */
					flush( bitbuf_qm, &nxt_byte, reg, stk, &qm_bytes);

					/* compute number of coded bits once more */
					qm_bits = qm_bytes << 3;

					/* close stream to raw bit plane data */
					BufferClose( bitbuf_data);

					/* close stream to QM compressed bytes */
					BufferClose( bitbuf_qm);

					/*
					* LIMITING CASE: negative result -> QMcoder
					* ultimately failed to compress bit plane
					*/
					if ( qm_bits > (*pd).size)
					{
						/* re-open buffer to read in raw data */
						bitbuf_data = BufferOpenRead( bp_data);

						/*
						* transfer all raw data for current bit plane
						* to bit stream
						*/
						for ( j = 0; j < (*pd).size; j++)
						{
							/* get bit from raw bit plane data file */
							tmp_bit = BufferGetBit( bitbuf_data);

							/* write bit to final bit stream */
							BufferPutBit( bitbuf, tmp_bit);
						}

						/* close stream to raw bit plane data */
						BufferClose( bitbuf_data);
					}
					/*
					* LIMITING CASE: positive result -> QMcoder
					* narrowly succeded in compressing the bit plane
					*/
					else
					{
						/* update channel byte count */
						num_bytes[chan] += qm_bits >> 3;

						/* re-open buffer to read in QM bytes */
						bitbuf_qm = BufferOpenRead( bp_qm);

						/*
						* transfer all QM bytes for current bit plane
						* to bit stream
						*/
						for ( j = 0; j < qm_bits; j++)
						{
							/* get bit from QM bytes file */
							tmp_bit = BufferGetBit( bitbuf_qm);

							/*
							* write QM coded bit to final bit stream
							*/
							BufferPutBit( bitbuf, tmp_bit);
						}

						/* close stream to compressed QM bytes */
						BufferClose( bitbuf_qm);
					}

					/* signal uncoded bits that will follow up */
					u_flg = TRUE;
				}
				else
				{
					/*
					* increment counter of QM compressed bit planes
					*/
					num_bp[chan]++;

					/*
					* add number of compressed bytes to channel byte
					* count
					*/
					num_bytes[chan] += qm_bytes;

					/* close stream to raw bit plane data */
					BufferClose( bitbuf_data);

					/* close stream to QM compressed bytes */
					BufferClose( bitbuf_qm);

					/* re-open buffer to read in QM bytes */
					bitbuf_qm = BufferOpenRead( bp_qm);

					/*
					* transfer all QM encoded bits for current bit
					* plane to bit stream
					*/
					for ( j = 0; j < qm_bits; j++)
					{
						/* get bit from QM bytes file */
						tmp_bit = BufferGetBit( bitbuf_qm);

						/* write bit to final bit stream */
						BufferPutBit( bitbuf, tmp_bit);
					}

					/* close stream to compressed QM bytes */
					BufferClose( bitbuf_qm);
				}
			}
			fprintf( stderr, " %d bytes", qm_bytes);
		} /* for z (bitplane)	*/

		/*
		* entire channel has been compressed ->
		* terminate encoding now
		*/
		if (!u_flg)
		{
			/* save byte count for last channel */
			qm_bytes = num_bytes[chan];

			/* terminate binary arithmetic coding */
			flush( bitbuf, &nxt_byte, reg, stk, &qm_bytes);

			/*
			* update byte count for last channel since QM code
			* register has now been flushed
			*/
			num_bytes[chan] = qm_bytes;
		}	
	  fprintf( stderr, "\n  %d bytes per channel", num_bytes[chan]);
	}

	/* empty bit buffer */
	if ((*bitbuf).bitpos != 0)
		putc( (int)(*bitbuf).buffer, (*bitbuf).file);

	/*
	* start writing footer;
	* write number of QM compressed bit planes for first two channels
	*/
	putc( num_bp[0] + (num_bp[1] << 4), (*bitbuf).file);

	/*
	* write number of QM compressed bit planes for third channel
	*/
	putc( num_bp[2], (*bitbuf).file);

	/* write number of QM compressed bytes per channel */
	for ( j = 0; j < (*pd).channel; j++)
	{
		/* number of data bits for corresponding channel */
		offset = (*pd).size * (*pd).bitperchan[j];

		sl = 0; /* (re-)initialize shift value */

		/*
		* increment shift value until resulting power of two is
		* greater than number of data bits for current channel
		*/
		while ( (rsize_t)(1 << sl) < offset ) sl++;

		/* compute number of bytes that will be assigned to offset */
		offset = sl >> 3;

		/* round up number of bytes if needed */
		if ( (offset << 3) < sl) offset++;

		/* decompose number of bytes per channel */
		for ( sl = 0; sl < offset; sl++)
		{
			/* write from least to most significant byte */
			putc( ( ( num_bytes[j] >> (8 * sl) ) & mask ),
				(*bitbuf).file);
		}
	}

	/* no more data shall be written beyond this point */
	(*bitbuf).iswrite = FALSE;

	/* shrink file size in order to truncate trailing zero bytes */
	_chsize( _fileno( (*bitbuf).file), ftell( (*bitbuf).file));

	/* deallocate memory for QM byte buffer */
	// FREE( nxt_byte);

	/* deallocate memory for 'uncoded' flag */
	// FREE( u_flg);

	/*
	* deallocate memory for number of QM compressed bytes for last
	* channel
	*/
	// FREE( qm_bytes);

	/* deallocate memory for all context instances */
	FREE( ctx_state);
}