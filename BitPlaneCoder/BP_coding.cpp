/*********************************************************************
* File	 	: BP_coding.cpp
* Purpose	: encoding and decoding of bitplanes
* Author 	: Tilo Strutz
* Date	 	: 25.03.2014
* 01.04.2014 stop encoding as soon the buffer tends to overflow
*********************************************************************/
//#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> //MAXINT
#include <assert.h>

#include "bitbuf.h"
#include "BP.h"
#include "ginterface.h"
#include "qm_coder.h"
#include "tstypes.h"

/*--------------------------------------------------------------------
* BP_coding()
*
* for brief description see: BP.h
*-------------------------------------------------------------------*/
void
BP_coding( BitBuffer *bitbuf, cbool g_flg, cbool p_flg, cbool c_flg, 
					PicData *pd, int encoder_flag, unsigned char significance_flag)
{
	//int d1, d2, d3, d4, d5, d6, d7, d8;

	int
		/*
		* offset in reference to beginning of next channel footer part
		*/
		firstBP[3], /* bit plane from which coding starts */

		s_PelVal = 0, /* signed pixel value */
		z; /* control variable for bit planes */

	unsigned char *bit_array = NULL;
	unsigned char *significance_map = NULL;

	unsigned int
		bit_array_size,
		*PelVal; /* pointer to (color) value of pel */
	unsigned long
		py, pos,
		j,
		byte_pos,		/* index into bit_array	*/
		/* counter of QM compressed bytes per bit plane */
		// *qm_bytes = (ulong *)malloc( sizeof( ulong)),
		qm_bytes = 0,
		/* number of QM compressed bytes per channel */
		num_bytes[3] = {0, 0, 0},
		qm_bits = 0; /* QM coded bits per bit plane */

	rsize_t
		chan, /* control variable for image channels */
		ctxID, /* context instance identifier */
		/* bit mask to decompose number of bytes per channel */
		//mask = 0xff,  /* encoder	*/
		my_stk, /* local stack variable */
		num_cBits, /* number of context bits */
		num_ctx, /* number of context instances */
		//sl, /* encoder: shift logical value in order to compute offset */
		*stk, /* pointer to stack variable */
		x, /* control variable for horizontal pels */
		y; /* control variable for vertical pels */

	byte
		/*
		* allocate memory for first compressed byte that may later be
		* modified by carry-over
		*/
		// *nxt_byte = (byte *)malloc( sizeof( byte)); /* encoder	*/
		nxt_byte;

	cbool
		bit_in, /* value of bit to be encoded */
		bit_out, /* binary arithmetically decoded bit */
		//s_flg = 0, /* decode via QM (0) or read in unencoded data (1) */
		/*
		* data bit from auxiliary bit buffer that must be written to
		* bit stream
		*/

		/*
		* read unencoded bit plane data from bit stream (1) or not (0)
		*/
		u_flg;


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
	for ( j = 0; j < pd->channel; j++) 
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


	if (encoder_flag)
	{
		/* inverse mapping or GRAY decoding required */
		if (p_flg || g_flg || c_flg)
		{
			/* for all image channels */
			for ( chan = 0; chan < pd->channel; chan++)
			{
				/* for all vertical pels */
				for ( y = 0; y < pd->height; y++)
				{
					/* for all horizontal pels */
					for ( x = 0; x < pd->width; x++)
					{
						/*
						* get address of (color) value of current channel
						* at sequentialized two-dimensional position
						*/
						PelVal = GetXYValue( pd, chan, x, y);

						if ( (p_flg || c_flg) && pd->offset[chan] != 0)
						{
							/* subtract offset */
							s_PelVal = *PelVal - pd->offset[chan];

							/* get back unsigned pixel value */
							*PelVal = map( s_PelVal);
						}

						/* map pixel value to GRAY code */
						if (g_flg && !p_flg) G_encode( PelVal);
					}
				}
			}
		}
	}/* if (encoder_flag)*/

	/* set fixed number of context bits */
	num_cBits = 13;
	if (significance_flag) num_cBits++;

	/* a total of (2 to power of num_cBits) contexts */
	num_ctx = 1 << num_cBits;

	/* allocate memory for context instances */
	ALLOC( ctx_state, num_ctx, CTX_IDX);

	/* allocate memory for a single bit plane with 10% overhead */
	bit_array_size = (pd->size + pd->size/10)>>3;
	ALLOC( bit_array, bit_array_size, unsigned char);

	/* allocate memory for significance map with border of 1*/
	CALLOC_( significance_map, (pd->width+2) * (pd->height + 2), unsigned char);
	if (ERRCODE)
		return;

	//d1 = - 1 - (signed)pd->width;
	//d2 =     - (signed)pd->width;
	//d3 = + 1 - (signed)pd->width;
	//d4 = - 1;
	//d5 = + 1;
	//d6 = - 1 + (signed)pd->width;
	//d7 =     + (signed)pd->width;
	//d8 = + 1 + (signed)pd->width;

	/* for all image channels */
	for ( chan = 0; chan < pd->channel; chan++)
	{
		fprintf( stderr, "\n channel: %d", chan);

		/* reset 'uncoded' flag */
		u_flg = FALSE;

		/* for all bit planes */
		for ( z = firstBP[chan]; z >= 0; z--)
		{
#ifdef _DEBUG
				printf( "\nchan %d, plane %d", chan, z);
#endif
			if( encoder_flag)
			{
				/*
				* initial value of compressed byte that will be be overwritten
				* at first chance in byte_out()
				*/
				nxt_byte = 0xff;
				/* initialize binary arithmetic encoder */
				init_enc( reg, stk);
			}
			else
			{
				/* initialize QM byte count */
				u_flg = BufferGetBit( bitbuf); /* uncoded ?	*/
				if (!u_flg) 
				{
					int c;
					c = BufferGetBit( bitbuf);
					c = (c << 1) + BufferGetBit( bitbuf);
					if ( c == 0)
					{
						qm_bytes = BufferGetByte( bitbuf);
					}
					else if ( c == 1)
					{
						qm_bytes = BufferGetWord( bitbuf);
					}
					else if ( c == 2)
					{
						qm_bytes = BufferGetByte( bitbuf);
						qm_bytes = (qm_bytes << 16) + BufferGetWord( bitbuf);
					}
					else if ( c == 2)
					{
						qm_bytes = BufferGetWord( bitbuf);
						qm_bytes = (qm_bytes << 16) + BufferGetWord( bitbuf);
					}
					num_bytes[chan] += qm_bytes;

					/* fill array with compressed data (+ trailing zeros)	*/
					for ( j = 0; j < qm_bytes; j++)
					{
						bit_array[j] = BufferGetByte( bitbuf);
					}
					/* reset remaining array	*/
					memset( &(bit_array[qm_bytes]), 0, (bit_array_size-qm_bytes) * sizeof( unsigned char));

					/* (re-)initialize binary arithmetic decoder */
					byte_pos = 0;
					init_dec( bit_array, reg, &byte_pos);
				}
				else
				{
					/* read uncoded bits	*/
					qm_bytes = 0;
					num_bytes[chan] += (pd->size + 7) >> 3;
				}
			}

			ctx->mps = 0; /* reset sense of MPS */
			/* reset index of LPS probability estimate */
			ctx->q_idx = 0;
			/* reset LPS probability estimate */
			ctx->q_val = 0x5a1d;

			/*
			* advance pointer to context instance with each step
			*/
			for ( j = 0; j < num_ctx; j++)
			{
				memcpy( &(ctx_state[j]), ctx, sizeof( CTX_IDX));
			}

			if (encoder_flag)
			{
				fprintf( stderr, "\n   bitplane %d: ", z);
				/*
				 * reset number of QM compressed bytes before bit plane
				 * encoding
				 */
				qm_bytes = 0;
			} /* if encoder_flag*/

			/* for all vertical pels */
			for ( y = 0, py = pd->width; y < pd->height; y++, py += pd->width)
			{
#ifdef _DEBUG
				//printf( "\n row %d\n", y);
#endif
				/* for all horizontal pels */
				for ( x = 0; x < pd->width; x++)
				{
					/* get address of pel value */
					PelVal = GetXYValue( pd, chan, x, y);

					if (significance_flag)
					{
						pos = x+1 + py;
						/* check whether there was already a 1-bit	*/
						{
							/* identify context instance */
							ctxID = mod_con( (int)firstBP[chan], z, pd,
								chan, x, y);

							ctxID = (ctxID << 1) + significance_map[pos];
						}
					}
					else
					{
						/* identify context instance */
						ctxID = mod_con( (int)firstBP[chan], z, pd,
							chan, x, y);
					}

					/* assign context instance via ID */
					ctx = &(ctx_state[ctxID]);

					if (encoder_flag)
					{
						/*
						* first align, then extract bit in current plane
						*/
						bit_in = (cbool)((*PelVal >> z) & 1);

						if (significance_flag)
						{
							pos = x+1 + py;
							if ( !significance_map[pos]) /* not yet significant	*/
							{
								if (bit_in)
								{
									significance_map[pos]++; /* set to significant	*/
								}
							}
						}

#ifdef _DEBUG
						//printf( " %d(%d),", ctxID, bit_in);
#endif
						/* encode via QM */
						assert( bit_array_size > qm_bytes);
						/* encode binary arithmetically */
						BA_encode( bit_array, &nxt_byte, bit_in, reg,
								ctx, stk, &qm_bytes);
						if (qm_bytes > bit_array_size -4 )
						{
							/* stop encoding, 
							 * bit plane must be transmitted unencoded
							 */
							y = pd->height;
							break;
						}
					} /* if (encoder_flag)	*/
					else /* do the decoding	*/
					{
						/* decode via QM */
						if (!u_flg)
						{
							/* binary arithmetic decoding via QM */
							bit_out = BA_decode( bit_array, reg, ctx, &byte_pos);

							if (significance_flag)
							{
								pos = x+1 + py;
								if ( !significance_map[pos]) /* not yet significant	*/
								{
									if (bit_out)
									{
										significance_map[pos]++; /* set to significant	*/
									}
								}
							}
						}
						/* stop QM decoding */
						else
						{
							/* read in unencoded data bits from stream */
							bit_out = BufferGetBit( bitbuf);
						}
#ifdef _DEBUG
							//printf( " %d(%d),", ctxID, bit_out);
#endif

						/*
						* only decoded 1-bit requires inclusion;
						* first align, then include 1-bit in binary
						* representation of (color) value
						*/
						if (bit_out) *PelVal |= (bit_out << z);
					}/* else of if (encode_flag)*/
				}
			} /* for y	*/

			if (encoder_flag)
			{
				/*
				* end of bit plane: compare sizes of auxiliary buffers if
				* QMcoder has been deployed
				*/
				/* terminate binary arithmetic coding */
				flush( bit_array, &nxt_byte, reg, stk, &qm_bytes);
				/*
				 * compute number of QM coded bits for current bit plane
				 */
				qm_bits = qm_bytes << 3;

				/* QMcoder procuced more bits than it was fed */
				if (qm_bits > pd->size)
				{
					/*
					* LIMITING CASE: negative result -> QMcoder
					* ultimately failed to compress bit plane
					*/

					/* write info bit (uncoded!) to final bit stream */
					BufferPutBit( bitbuf, 1);

					/*
					* transfer all raw data for current bit plane
					* to bit stream
					*/
					for ( j = 0; j < pd->size; j++)
					{
						/* get bit from raw bit plane */
						bit_in = (cbool)((pd->data[chan][j] >> z) & 1);

						/* write bit to final bit stream */
						BufferPutBit( bitbuf, bit_in);
					}
					/* update channel byte count */
					num_bytes[chan] += (pd->size+7) >> 3;

					fprintf( stderr, " %d bytes(plain)", (pd->size+7) >> 3);
				}
				else
				{
					/*
					* LIMITING CASE: positive result -> QMcoder
					* narrowly succeded in compressing the bit plane
					*/
					/* update channel byte count */
					num_bytes[chan] += qm_bits >> 3;

					/* write info bit (qm_coded!) to final bit stream */
					BufferPutBit( bitbuf, 0);
					assert( qm_bytes < (1<<31));
					if (qm_bytes < 256)
					{
						BufferPutBit( bitbuf, 0);
						BufferPutBit( bitbuf, 0);
						BufferPutByte( bitbuf, (unsigned char)(qm_bytes & 0xff)); /* number of written bytes	*/
					}
					else if (qm_bytes < (1<<16))
					{
						BufferPutBit( bitbuf, 0);
						BufferPutBit( bitbuf, 1);
						BufferPutWord( bitbuf, (unsigned short)qm_bytes); /* number of written bytes	*/
					}
					else if (qm_bytes < (1<<24))
					{
						BufferPutBit( bitbuf, 1);
						BufferPutBit( bitbuf, 0);
						BufferPutByte( bitbuf, (unsigned char)(qm_bytes >> 16)); /* number of written bytes	*/
						BufferPutWord( bitbuf, (unsigned short)(qm_bytes & 0xffff)); /* number of written bytes	*/
					}
					else
					{
						BufferPutBit( bitbuf, 1);
						BufferPutBit( bitbuf, 1);
						BufferPutWord( bitbuf, (unsigned short)(qm_bytes >> 16)); /* number of written bytes	*/
						BufferPutWord( bitbuf, (unsigned short)(qm_bytes & 0xffff)); /* number of written bytes	*/
					}

					/*
					 * transfer all QM bytes for current bit plane
					 * to bit stream
					 */
					for ( j = 0; j < qm_bytes; j++)
					{
						unsigned char byte;
						byte = bit_array[j];
						BufferPutByte( bitbuf, byte);
					}
					fprintf( stderr, " %d bytes(qm)", qm_bytes);
				} /* if ( qm_bits > pd->size) */
			} /* if encoder_flag	*/
			else
			{
				if (u_flg)	fprintf( stderr, " %d bytes(plain)", (pd->size+7) >> 3);
				else 				fprintf( stderr, " %d bytes(qm)", qm_bytes);
			}
		} /* for z (bitplane)	*/

		fprintf( stderr, "\n  %d bytes per channel", num_bytes[chan]);
	} /* for ( chan = 0;*/

	if (encoder_flag)
	{
		/* empty bit buffer */
		// flushing is be done in codec_e.c, Encode()
		//if ((*bitbuf).bitpos != 0)
		//	putc( (int)(*bitbuf).buffer, (*bitbuf).file);
	}
	else
	{
		/* inverse mapping or GRAY decoding required */
		if (p_flg || g_flg || c_flg)
		{
			for ( chan = 0; chan < pd->channel; chan++)
			{
				for ( y = 0; y < pd->height; y++)
				{
					for ( x = 0; x < pd->width; x++)
					{
						/*
						* get address of (color) value of current channel
						* at sequentialized two-dimensional position
						*/
						PelVal = GetXYValue( pd, chan, x, y);

						/* get back plain binary code */
						if (g_flg && !p_flg) G_decode( PelVal);

						/* prediction took place */
						if ( (p_flg || c_flg) && pd->offset[chan] != 0)
						{
							/* prepare for signed pixel value */
							s_PelVal = *PelVal;

							s_PelVal = undo_map( s_PelVal);

							/* add offset */
							*PelVal = s_PelVal + pd->offset[chan];
						}
					}
				}
			}
		}
	} /* else of if encode_flag	*/

	/* deallocate memory for all context instances */
	FREE( ctx_state);
	FREE( bit_array);
	FREE( significance_map);
}