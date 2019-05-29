/*****************************************************************
 *
 * Datei:	decoder.c
 * Funktion:	decoding
 * Autor:	Tilo Strutz
 * Datum: 	08.04.2013
 * changes
 * 08.04.2013 separation from common file encoder.c
 * 15.04.2013 complete new determination of thresholds + model_AC
 * 19.04.2013 addition of MED predictor
 * 27.04.2013 inclusion of PAETH mode
 * 29.04.2013 output of ASCII-pgm, if maxval>255
 * 28.05.2013 Ra_shift, scaling of Ra in kn[][]
 * 20.06.2013 Ra_shift replaced by * Ra_fac_numerator/Ra_fac_denominator 
 * 22.06.2013 update_weights_e() reset = image_width /4
 * 03.07.2013 bugfix: last model must at least contain +-1 
 * 25.07.2013 bugfix: clean_predictorTM() only if activated
 *									determination of bitdepth before LUT reading
 * 10.09.2013 unsigned short num_contexts
 * 18.12.2013 bugfix, return w/o processing if component is empty
 * 11.04.2014 read max and min vals in dependence on bit depth
 * 09.01.2015 bugfix, im->max_value == 0, 
 *              return after reading complete header
 * 23.01.2015 wA, wB
 * 13.08.2018 include predictLS()
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types_CoBaLP.h"
#include "codec_CoBaLP.h"
#include "contexts.h"
#include "arithmetic.h"
#include "bitbuf.h"
#include "predictor2.h"
#include "predictorTM.h"
#include "meta_predictor.h"
#ifdef STAND_ALONE_COBALP
#include "compaction.h"
#endif

void
decode_( IMAGE *im, IMAGE *imExt, long *lptr, FILE *in, 
				unsigned short num_contexts, 
				unsigned short num_contextsLS, 
				BitBuffer *bitbuf,
			unsigned char *const_rows, long *row_val, 
			unsigned char *const_cols, long *col_val, 
			FILE *out, FILE *outr, FILE *outx, FILE *outc,
					 int pred_mode, int paeth_mode, float wA, float wB);

#ifdef STAND_ALONE_COBALP
void write_pgm_header( IMAGE *im, FILE *out);
void write_asc_pgm_header( IMAGE *im, FILE *out);
#endif

int verbose( char *log_name, IMAGE *im, IMAGE *imExt, long *lptr,
						META_PRED *p_meta, PRED2 *p_pred2, 
					unsigned char *const_rows, long *row_val, 
					unsigned char *const_cols, long *col_val, 
					acModell *M, acModell *Mext, unsigned int *th_AC, 
					unsigned short num_contexts,
					unsigned short used_AC_contexts,
					unsigned long *count_px,
					unsigned long *count_cx,
					unsigned long *count_cxESC,
					long	*sign_count,
					char *rtn, unsigned long file_size,
					PREDE *p_predE,
					double H);


/* global arrays from contexts.c	*/
extern double kn[MAXNUM_OF_PRED_CONTEXTS][NUM_KNOT_ELEM];	/* defined in contexts.s	*/
extern double knLS[MAXNUM_OF_PRED_CONTEXTS][NUM_KNOT_ELEM_LS];	/* defined in contextsLS.c	*/
//extern int Ra_shift;
extern int Ra_fac_numerator;
extern int Ra_fac_denominator;
//extern int glob_sign;

/*---------------------------------------------------------------
*	put_pgm_value_1()
*---------------------------------------------------------------*/
void put_pgm_value_1( unsigned long val, FILE *out)
{
	putc( (int)val, out);
}

/*---------------------------------------------------------------
*	put_pgm_value_asc()
*---------------------------------------------------------------*/
void put_pgm_value_asc( unsigned long val, FILE *out)
{
	fprintf( out, " %3d", val);
}

/*---------------------------------------------------------------
*	put_pgm_value_2()
*---------------------------------------------------------------*/
//void put_pgm_value_2( unsigned long val, FILE *out)
//{
//	putc( (int)(val / 256), out);
//	putc( (int)(val & 0xff), out);
//}

/*---------------------------------------------------------------
*	decoder()
*---------------------------------------------------------------*/
#ifdef STANDALONE_COBALP
int decoder( IMAGE *im, FILE *in, FILE *out, 
						FILE *outr, FILE *outx, FILE *outc, PARAM_STRUC	*param)
#else
int decoder( IMAGE *im, BitBuffer *bitbuf, long *imptr, PARAM_STRUC	*param,
						float wA, float wB)
#endif
{
	int err = 0;
	int r, c, rk, rkd, rka;
	long *lptr = NULL, *row_val=NULL, *col_val=NULL;
	unsigned char *const_rows=NULL;
	unsigned char *const_cols=NULL;
	unsigned int true_max_value, true_min_value;
	unsigned short num_contexts, num_contextsLS, k;
	unsigned long py, pos, posExt;
#ifdef STANDALONE_COBALP
	unsigned char ch1, ch2, ch3, ch4, ch5; 
	BitBuffer *bitbuf  = NULL;
	unsigned short new_max_val = 0;	/* for histogramm compaction	*/
	unsigned int *idx2val = NULL; /* LUT for undoing the compaction at decoder*/
#endif
	IMAGE imExtended, *imExt = NULL;

	imExt = &imExtended;

	/*----------------------------------------------------------
	 * read header info 
	 */
#ifdef STAND_ALONE_COBALP
	bitbuf = BufferReadInit( in);
	ch1 = BufferGetByte( bitbuf);
	ch2 = BufferGetByte( bitbuf);
	ch3 = BufferGetByte( bitbuf);
	ch4 = BufferGetByte( bitbuf);
	ch5 = BufferGetByte( bitbuf);
	if ( ch1 != 'C' || ch2 != 'o' || ch3 != 'B' || ch4 != '0' || ch5 != '2')
	{
		fprintf( stderr, "\n input file is not a CoBALP file! Abort!\n");
		goto end;
	}
	ch4 = BufferGetByte( bitbuf); /* '\0' */
#endif

	im->width = BufferGetByte( bitbuf) * 256;
	im->width += BufferGetByte( bitbuf);
	im->height = BufferGetByte( bitbuf) * 256;
	im->height += BufferGetByte( bitbuf);

	im->size = im->width * im->height;

	if (im->size == 0)
	{
		err = 2;
		fprintf(stderr, "\n!!! Theres is something wrong in reading this file!!!!");
		fprintf(stderr, "\n!!! size of component is zero!!!!");
		return err;
	}

	im->bit_depth = BufferGetByte( bitbuf);
	im->max_value = BufferGetByte( bitbuf);
	im->min_value = BufferGetByte( bitbuf);
	{
		int	bits, shift = 8;
		bits = (signed)im->bit_depth - 8;
		while (bits > 0)
		{
			im->max_value = im->max_value + (BufferGetByte( bitbuf) << shift);
			im->min_value = im->min_value + (BufferGetByte( bitbuf) << shift);
			bits -= 8;
			shift += 8;
		}
	}
	//im->max_value = BufferGetByte( bitbuf) * 256;
	//im->max_value += BufferGetByte( bitbuf);
	//im->min_value = BufferGetByte( bitbuf) * 256;
	//im->min_value += BufferGetByte( bitbuf);

	im->bit_depth = 0;
	{
		unsigned int domain;
		domain = im->max_value;
		while (domain)
		{
			domain >>= 1; im->bit_depth++;
		}
	}

	param->pred_mode = rice_decoding( bitbuf, 4);	/* get prediction mode	*/
	param->paeth_mode = BufferGetBit( bitbuf);	/* get PAETH flag	*/
	param->compact_flag = BufferGetBit( bitbuf);	/* get compact flag	*/
	param->rotate_flag = BufferGetBit( bitbuf);	/* get  flag	*/

	/* new for constant components	*/
	if (im->max_value == im->min_value)
	{
		if (im->max_value == 0)
		{
			memset( imptr, im->max_value, im->size * sizeof(long));	
		}
		else
		{
			for (pos = 0; pos < im->size; pos++)
			{
				imptr[pos] =im->max_value;
			}

		}
		return 0;
	}


	true_max_value = im->max_value;
	true_min_value = im->min_value;

#ifdef STAND_ALONE_COBALP
	if (param->compact_flag)
	{
		new_max_val = BufferGetByte( bitbuf) * 256;
		new_max_val += BufferGetByte( bitbuf);
		/* LUT for mapping values	back */
		CALLOC( idx2val, new_max_val + 1, unsigned int);
		read_compaction_LUT( im, idx2val, new_max_val, bitbuf);

		/* use new range	*/
		im->max_value = new_max_val;
		im->min_value = 0;

#ifdef _DEBUG
		if(1)
		{
			FILE *out;
			out = fopen( "idx2val_d.txt", "wt");
			for (pos =0; pos <= new_max_val; pos++)
			{
				fprintf( out, "%4d %4d\n", pos, idx2val[pos]);
			}
			fclose( out);
		}
#endif
	}
#endif


	/* memory for image data	*/
	imExt->bit_depth = im->bit_depth;
	imExt->max_value = im->max_value;
	imExt->min_value = im->min_value;
	imExt->height = im->height + OFF_TOP;
	imExt->width = im->width + OFF_LEFT + OFF_RIGHT;
	imExt->size = imExt->height * imExt->width;
	CALLOC( lptr, imExt->size, long);

	/* 
	 * get constant rows or columns
	 */
	if (1)
	{
		int signal_flag;

		/* rows first	*/

		/* memory for signalling constant rows	*/
		CALLOC( const_rows, im->height, unsigned char);
		CALLOC( row_val, im->height, long);
		/* reset constant rows	*/
		//memset( const_rows, 0, sizeof(unsigned char));

		signal_flag = BufferGetBit( bitbuf);
		if (signal_flag)
		{	/* yes, there are constant rows	*/
			r = BufferGetWord( bitbuf);	/* row number	*/
			while ( (unsigned)r < im->height)
			{
				const_rows[r] = 1;
				if (im->max_value > 255)
				{
					row_val[r] = (long)BufferGetWord( bitbuf);	/* row value	*/
				}
				else
				{
					row_val[r] = (long)BufferGetByte( bitbuf);	/* row value	*/
				}
				r = BufferGetWord( bitbuf);	/* next row number	*/
			}
		}

		/* now columns	*/

		/* memory for signalling constant cols	*/
		CALLOC( const_cols, im->width, unsigned char);
		CALLOC( col_val, im->width, long);

		signal_flag = BufferGetBit( bitbuf);
		if (signal_flag)
		{	/* yes, there are constant cols	*/
			c = BufferGetWord( bitbuf);	/* row number	*/
			while ( (unsigned)c < im->width)
			{
				const_cols[c] = 1;
				if (im->max_value > 255)
				{
					col_val[c] = (long)BufferGetWord( bitbuf);	/* column value	*/
				}
				else
				{
					col_val[c] = (long)BufferGetByte( bitbuf);	/* column value	*/
				}
				c = BufferGetWord( bitbuf);	/* next row number	*/
			}
		}
	}

	/*
	 * Initialisation of proper writing method (1/2 byte)
	 */
#ifdef STAND_ALONE_COBALP
	if (true_max_value > 255)
	{
		unsigned tmp;
		//put_pgm_value = put_pgm_value_2;
		put_pgm_value = put_pgm_value_asc;
		tmp = im->max_value;
		im->max_value = true_max_value;
		if (param->rotate_flag)
		{
			unsigned int tmp;
			tmp = im->width;
			im->width = im->height;
			im->height = tmp;
			write_asc_pgm_header( im, out);
			tmp = im->width;
			im->width = im->height;
			im->height = tmp;
		}
		else
		{
			write_asc_pgm_header( im, out);
		}
		if (outr != NULL) write_asc_pgm_header( im, outr);
		im->max_value = tmp;
	}
	else
	{
		unsigned tmp;
		put_pgm_value = put_pgm_value_1;
		tmp = im->max_value;
		im->max_value = true_max_value;
		if (param->rotate_flag)
		{
			unsigned int tmp;
			tmp = im->width;
			im->width = im->height;
			im->height = tmp;
			write_pgm_header( im, out);
			tmp = im->width;
			im->width = im->height;
			im->height = tmp;
		}
		else
		{
			write_pgm_header( im, out);
		}
		if (outr != NULL) write_pgm_header( im, outr);
		im->max_value = tmp;
	}
#endif

	/* number of contexts, reserve 3 contexts for: 1st row, 1st col, RLC mode
	 * must be same as in encoder mode !!
	 */

	/* read in the context vectors required for prediction AND coding	*/
	{
		num_contexts = (unsigned short)rice_decoding( bitbuf, 8);
		//Ra_shift = rice_decoding( bitbuf, 3);
		Ra_fac_denominator = rice_decoding( bitbuf, 8);
		Ra_fac_numerator = rice_decoding( bitbuf, 8);
		rk = 6; /* coding parameter for rice coding	*/
		rka = 6; /* coding parameter for rice coding	*/
		rkd = 5; /* coding parameter for rice coding	*/
		for ( k = 0; k < num_contexts; k++)
		{
			unsigned long val;
			int i, b, abs0;

			/* first element is sorted in ascending order	*/
			if (k == 0)
			{
				val = rice_decoding( bitbuf, rk);
				kn[k][0] = (double)val;
				if (val)
				{
					cbool sign;
					sign = BufferGetBit( bitbuf); /* get sign	*/
					if (sign) kn[k][0] = - kn[k][0];	/* assign to knot	*/
				}
			}
			else
			{
				unsigned long diff;
				diff = rice_decoding( bitbuf, rkd);
				kn[k][0] = diff + kn[k-1][0];
				/* adapt coding parameter	*/
				b = 0;
				while (diff){ b++; diff >>= 1;}
				if ( b > 2*rkd)
					rkd = (rkd + b) >> 1;	/* fast adaptation	*/
				else
					rkd = (rkd * 7 + b) >> 3;	/* slower adaptation	*/
			}
			abs0 = abs( (int)kn[k][0]);
	#ifdef PCX_INCLUDE_RA
			for ( i = 1; i < NUM_KNOT_ELEM-1; i++)
	#else
			for ( i = 1; i < NUM_KNOT_ELEM; i++)
	#endif
			{
				val = rice_decoding( bitbuf, rk);
				//if (abs0 > 1023) kn[k][i] = (double)(val << 3);
				//else if (abs0 > 511) kn[k][i] = (double)(val << 2);
				//else if (abs0 > 255) kn[k][i] = (double)(val << 1);
				//else	
					kn[k][i] = (double)val;
				if (val)
				{
					cbool sign;
					sign = BufferGetBit( bitbuf); /* get sign	*/
					if (sign) kn[k][i] = -kn[k][i];	/* assign to knot	*/
				}
				/* adapt coding parameter	*/
				b = 0;
				while (val){ b++; val >>= 1;}
				if ( b > 2*rk)
					rk = (rk + b) >> 1;	/* fast adaptation	*/
				else
					rk = (rk * 7 + b) >> 3;	/* slower adaptation	*/
			}
	#ifdef PCX_INCLUDE_RA
			i = NUM_KNOT_ELEM - 1; /* last element = Ra, always positiv	*/
			{
				val = rice_decoding( bitbuf, rka);
				/* is multiple of 4, see contexts.c	*/
				// kn[k][i] = (double)(val << 2);
				kn[k][i] = (double)val *((Ra_fac_numerator >> 2) + 1);
				/* adapt coding parameter	*/
				b = 0;
				while (val){ b++; val >>= 1;}
				if ( b > 2*rka)
					rka = (rka + b) >> 1;	/* fast adaptation	*/
				else
					rka = (rka * 7 + b) >> 3;	/* slower adaptation	*/
			}
	#endif
		} /* for k */
	} /* read knots contexts */

	if ( 0|| param->pred_mode & 0x08 )
	{
		/* receive LS px contexts	*/
		num_contextsLS = (unsigned short)rice_decoding( bitbuf, 8);
		rk = 6; /* coding parameter for rice coding	*/
		rka = 6; /* coding parameter for rice coding	*/
		rkd = 5; /* coding parameter for rice coding	*/
		for ( k = 0; k < num_contextsLS; k++)
		{
			unsigned long val;
			int i, b, abs0;

			/* first element is sorted in ascending order	*/
			if (k == 0)
			{
				val = rice_decoding( bitbuf, rk);
				knLS[k][0] = (double)val;
				if (val)
				{
					cbool sign;
					sign = BufferGetBit( bitbuf); /* get sign	*/
					if (sign) knLS[k][0] = - knLS[k][0];	/* assign to knot	*/
				}
			}
			else
			{
				unsigned long diff;
				diff = rice_decoding( bitbuf, rkd);
				knLS[k][0] = diff + knLS[k-1][0];
				/* adapt coding parameter	*/
				b = 0;
				while (diff){ b++; diff >>= 1;}
				if ( b > 2*rkd)
					rkd = (rkd + b) >> 1;	/* fast adaptation	*/
				else
					rkd = (rkd * 7 + b) >> 3;	/* slower adaptation	*/
			}
			abs0 = abs( (int)knLS[k][0]);
			for ( i = 1; i < NUM_KNOT_ELEM_LS; i++)
			{
				val = rice_decoding( bitbuf, rk);
				knLS[k][i] = (double)val;
				if (val)
				{
					cbool sign;
					sign = BufferGetBit( bitbuf); /* get sign	*/
					if (sign) knLS[k][i] = -knLS[k][i];	/* assign to knot	*/
				}
				/* adapt coding parameter	*/
				b = 0;
				while (val){ b++; val >>= 1;}
				if ( b > 2*rk)
					rk = (rk + b) >> 1;	/* fast adaptation	*/
				else
					rk = (rk * 7 + b) >> 3;	/* slower adaptation	*/
			}
		} /* for k */
	}	
	else num_contextsLS = 0;
// !!!
//num_contexts = num_contextsLS; //wenn px mit pxLS in meta_predictor überschrieben ist
// !!

	/* fill extended data which is not available at decoder side
	 * must be same in encoder !!
	 */
	{
		long val;
		if (im->max_value < 256)
		{
			val = (im->max_value + im->min_value) / 2;
		}
		else
		{
			//val = im->min_value * 2;
			val = im->min_value + ((im->max_value + im->min_value) >> 3);
		}

		/* remove extended data which is not available at decoder side	*/
		for (r = 0, py = 0; r < OFF_TOP; r++, py += imExt->width)	/* all rows	*/
		{
			for (c = 0, pos = py; c < (signed)imExt->width; c++, pos++)	/* all columns	*/
			{
				lptr[pos] = val;
			}
		}

		r =  OFF_TOP; py = OFF_TOP * imExt->width;
		{
			for (c = 0, pos = py; c < (signed)OFF_LEFT; c++, pos++)	/* all columns	*/
			{
				lptr[pos] = val;
			}
		}
	}

#ifdef STAND_ALONE_COBALP
	decode_( im, imExt, lptr, in, num_contexts, num_contextsLS, bitbuf, 
			const_rows, row_val, const_cols, col_val, 
			out, outr, outx, outc, param->pred_mode, param->paeth_mode);
	/* undo the ghistogramm compaction	*/
	if (param->compact_flag)
	{
		restore_histogram( imExt, lptr, idx2val);
		if(0)
		{
			FILE *out;
			out = fopen( "idx2val_d.txt", "wt");
			for (pos =0; pos <= new_max_val; pos++)
			{
				fprintf( out, "%4d %4d\n", pos, idx2val[pos]);
			}
			fclose( out);
		}
	  FREE( idx2val);
	}
#else
	decode_( im, imExt, lptr, NULL, num_contexts, num_contextsLS, bitbuf, 
			const_rows, row_val, const_cols, col_val, 
			NULL, NULL, NULL, NULL, param->pred_mode, param->paeth_mode,
			wA, wB);
#endif


	/* write reconstructed image data to file	*/
	pos = 0;
	if (param->rotate_flag)
	{
		for (c = 0, py=OFF_LEFT; c < (signed)im->width; c++, py++)	/* all columns	*/
		{
			for (r = 0, posExt = py + OFF_TOP*imExt->width; r < (signed)im->height; 
				r++, posExt+=imExt->width)	/* all rows	*/
			{
#ifdef STAND_ALONE_COBALP
				put_pgm_value( lptr[pos], out);
#else
				imptr[pos++] = lptr[posExt];
#endif
			}
#ifdef STAND_ALONE_COBALP
			if (im->max_value > 255) /* ASCII output is used	*/
			{
				fprintf( out, "\n");
			}
#endif
		}
	}
	else
	{
		for (r = 0, py = OFF_TOP*imExt->width; r < (signed)im->height; 
			r++, py+=imExt->width)	/* all rows	*/
		{
			for (c = 0, posExt = py+OFF_LEFT; c < (signed)im->width; c++, posExt++)	/* all columns	*/
			{
#ifdef STAND_ALONE_COBALP
				put_pgm_value( lptr[posExt], out);
#else
				imptr[pos++] = lptr[posExt];
#endif
			}
#ifdef STAND_ALONE_COBALP
			if (im->max_value > 255) /* ASCII output is used	*/
			{
				fprintf( out, "\n");
			}
#endif
		}
	}

#ifdef STAND_ALONE_COBALP
end:
	BufferClose(bitbuf);
#endif
	FREE( lptr);
	FREE( const_rows);
	FREE( const_cols);
	FREE( row_val);
	FREE( col_val);

	return 0;
}

/*---------------------------------------------------------------
 *	decode_()
 *---------------------------------------------------------------*/
void decode_( IMAGE *im, IMAGE *imExt, long *lptr, FILE *in,
						 unsigned short num_contexts, 
						 unsigned short num_contextsLS, 
					 BitBuffer *bitbuf, 
					 unsigned char *const_rows, long *row_val, 
					 unsigned char *const_cols, long *col_val, 
					 FILE *out, FILE *outr, FILE *outx, FILE *outc,
					 int pred_mode, int paeth_mode,
					 float wA, float wB)
{
#ifdef STAND_ALONE_COBALP
	char	*rtn="code_";
	FILE *outlog = NULL;
#else
	in = NULL; /* avoid warnings	*/
	out = NULL; /* avoid warnings	*/
	outr = NULL;
	outx = NULL;
	outc = NULL;
#endif
	int 	err = 0, r, c;
	long	sign_count[MAXNUM_OF_PRED_CONTEXTS];/* accumulates signs (+1,-1) */
	long	*eptr = NULL;	/*  prediction error	*/
	long	*eptrLS = NULL;	/*  prediction error	*/
	long	index; /* prediction error + offset	*/
	long 	RANGEd2, RANGEm1d2, RANGE, MAXVAL, RANGEd4;
	unsigned char *paeth_flags = NULL;
	unsigned short k, i;	/* prediction context	*/
	unsigned short cx, used_AC_contexts=0;	/* arithmetic-coding context	*/
	unsigned short	*px_ptr = NULL;	/* prediction context	array	*/
	unsigned short	*pxLS_ptr = NULL;	/* prediction context	array	*/
	unsigned short	*pxExt_ptr = NULL;	/* extended prediction context	array	*/
	unsigned short	*predMult = NULL;
	unsigned int th_AC[NUM_AC_CONTEXTS];
	unsigned int model_AC[NUM_AC_CONTEXTS];
	//unsigned long count_px[MAXNUM_OF_PRED_CONTEXTS];
	//unsigned long count_pxLS[MAXNUM_OF_PRED_CONTEXTS];
	unsigned long *count_px = NULL; // [MAXNUM_OF_PRED_CONTEXTS];
	unsigned long *count_pxLS = NULL; // [MAXNUM_OF_PRED_CONTEXTS];
	unsigned long count_cx[NUM_AC_CONTEXTS];	/* for statistics	*/
	unsigned long count_cxESC[NUM_AC_CONTEXTS];
	unsigned long abs_err;	/* estimate of prediction-error magnitude	*/
	//unsigned long sum_abserr[MAXNUM_OF_PRED_CONTEXTS];	/* sums */
	unsigned long *sum_abserr = NULL; // [MAXNUM_OF_PRED_CONTEXTS];	/* sums */
	unsigned long num_abs_scaled_errors;
	unsigned long py, pos,pyExt, posExt;
	PRED2 pred2_obj, *p_pred2; /* variables for predictor2 A, B	*/
	PREDE predE_obj, *p_predE; /* variables for predictor2 A, B	*/
	META_PRED	meta_pred_obj, *p_meta=NULL; /* variables for combining predictors	*/
	TEMPLATE templ_obj, *p_templ=NULL;	/* template for contexts and lin. predictors	*/
	TEMPLATE_E templE_obj, *p_templE=NULL;	
	acModell M[NUM_AC_CONTEXTS];	/* normal AC context	*/
	acModell Mext[NUM_AC_CONTEXTS]; /* extended context for large values	*/
	acState acState_obj, *acSt;
	LEAST_SQUARES ls_obj, *p_ls;

	p_ls = &ls_obj;
	acSt = &acState_obj;
	p_pred2 = &pred2_obj;
	p_predE = &predE_obj;
	p_meta = &meta_pred_obj;
	p_templ = &templ_obj;
	p_templE = &templE_obj;

	/* im->max_value is always the max Value from the original image
	 * as this value is also copied in the prediction-error-image file
	 */
	MAXVAL = im->max_value;
	RANGE = MAXVAL + 1;
	RANGEd2 = RANGE / 2;
	RANGEd4 = RANGE / 4;  /* for improved AC */
	RANGEm1d2 = (RANGE - 1) / 2;

	CALLOC( count_px, MAXNUM_OF_PRED_CONTEXTS, unsigned long);
	CALLOC( sum_abserr, MAXNUM_OF_PRED_CONTEXTS, unsigned long);
	CALLOC( count_pxLS, MAXNUM_OF_PRED_CONTEXTS, unsigned long);


	/* array for absolute prediction error	*/
	CALLOC( eptr, im->size, long);
	/* array for prediction context	*/
	CALLOC( px_ptr, im->size, unsigned short);
	if (pred_mode & 0x8)
	{
		CALLOC( eptrLS, im->size, long);
		CALLOC( pxLS_ptr, im->size, unsigned short);
		CALLOC( predMult, num_contextsLS, unsigned short);
	}

	if (paeth_mode) CALLOC( paeth_flags, RANGE, unsigned char);

	/* initialize weights, mean values, and learning rates	*/
	init_pred2Weights( p_pred2, im->max_value, wA, wB);
	init_pred2Weights_R( p_pred2, im->max_value);
	/* initialize combination of predictors	*/
	err = init_meta_prediction( p_meta, pred_mode);
	if (err) goto end;

	if (pred_mode & 0x4) 
		init_predictorTM( imExt, p_ls); /* initialise offset array for TM predictor	*/
	if (pred_mode & 0x8) 
		init_pred2Weights_L_d( bitbuf, num_contextsLS, p_pred2);

	for ( k = 0; k < MAXNUM_OF_PRED_CONTEXTS; k++)
	{
		sign_count[k] = 0;	/* observe sign of prediction error	*/
		count_px[k] = 0;	/* reset context counter	*/
		count_pxLS[k] = 0;	/* reset context counter	*/
		sum_abserr[k] = 0;	/* reset context counter	*/
	}

  /* read prediction coefficients	*/
	if ( pred_mode & 0x08 && 0)
	{
		/* read  predictor selection	*/
		for ( k = 0; k < num_contextsLS; k++)
		{
			predMult[k] =	(unsigned short)rice_decoding( bitbuf, 3);
		}
		//int rkVal, rk[LS_TEMPLATE_SIZE] = { 9,9,8,8,8,8};
		//int rkSum[LS_TEMPLATE_SIZE] = { 9,9,8,8,8,8};
		//unsigned int absMean;
		//unsigned int absSum[LS_TEMPLATE_SIZE] = { 0,0,0,0,0,0};
		for ( k = 0; k < num_contextsLS; k++)
		{
			float *wL = p_pred2->weights_L[k];
			long *lwL = p_pred2->lweights_L[k];
			for (i = 0; i < LS_TEMPLATE_SIZE; i++)
			{
				//lwL[i] = rice_decoding( bitbuf, rk[i]); /* is only nominator	*/
				lwL[i] = rice_decoding( bitbuf, 8); /* is only nominator	*/
				if (BufferGetBit( bitbuf))
				{
					lwL[i] = -lwL[i];
				}
				wL[i] = ( (float)lwL[i]  / 1024.F);
				
				//absSum[i] += abs( lwL[i]);
				//absMean = (absSum[i] + ((k + 1)>>1) )/ (k + 1); /* average over all contexts	*/
				//rkVal = 0;
				//while( absMean) { absMean >>= 1; rkVal++;}
				//rkSum[i] += rkVal;
				//rk[i] = (rkSum[i] + ((k + 2)>>1) )/ (k + 2);
			}
		}
	}

	/* read the thresholds for AC contexts	*/
	{
		int rk;
		unsigned long last_val;

#ifndef FIXED_AC_CONTEXTS
		used_AC_contexts = (unsigned short)rice_decoding( bitbuf, 4);
		/* decoder mode, get thresholds	*/
		rk = 0; last_val = 0;
		// for ( k = 0; k < used_AC_contexts; k++) last upper threshold is not required
		for ( k = 0; k < used_AC_contexts-1; k++)
		{
			unsigned long diff;
			diff = rice_decoding( bitbuf, rk);
			th_AC[k] = diff + last_val;
			last_val = th_AC[k];
			rk = 1;
			while( diff) { diff >>= 1; rk++;}
		}
		th_AC[used_AC_contexts-1] = 0;
#else
		used_AC_contexts = 8;
		/* use fixed thresholds as in WuM97 */
		th_AC[0] = 5;
		th_AC[1] = 15;
		th_AC[2] = 25;
		th_AC[3] = 42;
		th_AC[4] = 60;
		th_AC[5] = 85;
		th_AC[6] = 140;
		th_AC[7] = 256;
#endif

#ifndef FULL_ALPHABET		/* transmit model deviation	*/
		/*  get AC model sizes	*/
		rk = 3; last_val = 0;
		assert(used_AC_contexts > 0);
		for ( k = 0; k < used_AC_contexts; k++)
		{
			unsigned long diff;
			diff = rice_decoding( bitbuf, rk);
			model_AC[k] = diff + last_val;
			last_val = model_AC[k];
			rk = 0;
			while( diff) { diff >>= 1; rk++;}
		}
#endif
	}
	if (model_AC[used_AC_contexts - 1] < 1)
	{
		//fprintf( stderr, "\n model_AC[used_AC_contexts - 1] = %d", model_AC[used_AC_contexts - 1]);
		/* at least one model must contain +-1 for proper use of M[cx] and Mext[cx]*/
		model_AC[used_AC_contexts - 1] = 1;
	}

#ifdef STAND_ALONE_COBALP
	if (outx != NULL) 
	{
		unsigned short tmp;
		tmp = im->max_value;
		im->max_value = num_contexts + 3;
		write_asc_pgm_header( im, outx);
		im->max_value = tmp;
	}
	if (outc != NULL) 
	{
		unsigned short tmp;
		tmp = im->max_value;
		im->max_value = used_AC_contexts;
		write_asc_pgm_header( im, outc);
		im->max_value = tmp;
	}
#endif

	/*##################################################################
	 */

	/* initialize arithmetic coding	*/
	for ( k = 0; k < used_AC_contexts; k++) 
	{
		count_cx[k] = 0;	/* for statistics	*/
		count_cxESC[k] = 0;
#ifdef FULL_ALPHABET
		/* there must be at least 4 bins: (ESC, -1, 0, +1) in one model*/
		if ( (k == used_AC_contexts-1) && RANGE <3)
		{
			start_model( &M[k], 4); /* +1 because of the sign flipping -128 => + 128	*/
			start_model( &Mext[k], 4);
		}
		else
		{
			start_model( &M[k], RANGE+1); /* +1 because of the sign flipping -128 => + 128	*/
			start_model( &Mext[k], RANGE+1);
		}
#else
		start_model( &M[k], (model_AC[k] + 1) * 2);
	#ifdef NOMext
			Mext[k].H = M[k].H;
			Mext[k].K = M[k].K;
	#else
			start_model( &Mext[k], (model_AC[k] + 1) * 2);
	#endif
#endif
	}
	
	start_decoding( acSt, bitbuf);

	init_weights_e( p_predE);
	num_abs_scaled_errors = (RANGEd2 + 1) * TH_FAC;/* plus one as abs(-128) is 127+1 !!*/
	/* extetended array for prediction contexts, required for predictorCM() */
	CALLOC( pxExt_ptr, imExt->size, unsigned short)

	/* -------------------------------------------------------------
	 * prediction and coding loop
	 */
	for (r = 0, py = 0, pyExt = OFF_TOP * imExt->width; r < (signed)im->height; 
		r++, py+=im->width, pyExt+=imExt->width)	/* all rows	*/
	{
		for (c = 0, pos = py, posExt = pyExt + OFF_LEFT; c < (signed)im->width; 
			c++, pos++, posExt++)	/* all columns	*/
		{
			long pred_err;
			long Rx_old;
			unsigned long sym;
			//unsigned short pxLS;

			/* copy neighbour values to template,
			 * must always be perform, because it must fill the borders for next lines
			 */
			set_template( lptr, p_templ, posExt, r, c, imExt->width);

			/* check whether entire row or col is constant	*/
			if (!const_rows[r] && !const_cols[c])
			{
				long off;

				/************************************************************/
				/* px required for AC context	*/

				/* do the prediction using all predictors
				 * result is in p_meta->xHat
				 */
				px_ptr[pos] = meta_predict( im, imExt, lptr, pxExt_ptr, posExt,
								r, c,	p_templ, p_meta, p_pred2, 
								pred_mode, num_contexts, num_contextsLS, 
								count_px, count_pxLS, pxLS_ptr, predMult, eptrLS, p_ls);
				//pxLS_ptr[pos] = pxLS;

				if (paeth_mode)
				{
					if ( !paeth_flags[p_meta->xHat] )
					{
						long p, da, db, dc;
						/* use PAETH predictor	*/
						/* plane extrapolation  p = A + B - C */	
						p = p_templ->R[0] + p_templ->R[1] - p_templ->R[2]; 	
						/* differences */
						da = abs(p - p_templ->R[0]); /* Ra */
						db = abs(p - p_templ->R[1]); /* Rb */
						dc = abs(p - p_templ->R[2]); /* Rc */

						/* take value with shortest distance */
						if (da <= db && da <= dc) p_meta->xHat = p_templ->R[0];
						else if (db <= dc) p_meta->xHat =  p_templ->R[1];
						else p_meta->xHat =  p_templ->R[2];	
					}
					// lptr[posExt] is not known yet
					//paeth_flags[ lptr[posExt] ] = 1; 
				}

				/* determine context for AC	based on surrounding absolute errors
				 * required for both ecoder and decoder
				 */
				//cx = get_AC_context( im, eptr, pos, r, c, px_ptr, th_AC, used_AC_contexts,
				//			sum_abserr[ px_ptr[pos] ], count_px[ px_ptr[pos] ]);
				cx = get_AC_context( im, eptr, pos, r, c, px_ptr, th_AC, used_AC_contexts,
							sum_abserr[ px_ptr[pos] ], count_px[ px_ptr[pos] ], 
							p_predE, p_templE, num_abs_scaled_errors, &abs_err);

				if (r+c == 0) cx = used_AC_contexts - 1; /* error will be large	*/ 
				assert( cx < used_AC_contexts);

				off = M[cx].K / 2;
				/* do the arithmetic decoding	*/
				//sym = Decode_core( &M[cx], acSt, bitbuf);
				sym = decode_AC( &M[cx], acSt, bitbuf);

				/* Update Model */
				update_model( M[cx].H, M[cx].K, sym);

				if (sym == 0)	/* ESCAPE detected, read true value	*/
				{
					int stop;
					
					/* do the arithmetic decoding	*/
					index = 0;
					do
					{
						/* increase by absolute model size = off -1	*/
						index += off - 1;

						if (cx < used_AC_contexts - 1)
						{
							/* range cannot be narrowed, use next context	*/
							cx++;
							off = Mext[cx].K / 2;
							/* look for a suitable context	*/
						}
						stop = 1;
						//sym = Decode_core( &Mext[cx], acSt, bitbuf);
						sym = decode_AC( &Mext[cx], acSt, bitbuf);
						update_model( Mext[cx].H, Mext[cx].K, sym);
						if (sym == 0)
						{
							stop = 0;
						}
					} while (!stop);
					if ( ((signed)sym - off) < 0) 	index = -index;
					index += sym - off;
				}
				else index = sym - off;

				/* copy value	*/
				pred_err = index;

				if (count_px[px_ptr[pos]] > NUM_SKIPPED_STEPS) /* skip transient response	*/
				{ /* must be same as in prediction loop	*/
					sum_abserr[px_ptr[pos]] += abs( pred_err);
				}

				/* create non-symmetric distribution	*/
				//if (glob_sign < 0)  
				//	pred_err = -pred_err;
				//else 
#ifndef NOSignFlip
				if (sign_count[px_ptr[pos]] < 0)  pred_err = -pred_err;
#endif

				eptr[pos] = pred_err;

				/* reconstruction	*/
				Rx_old = eptr[pos] + p_meta->xHat;
				//Rx_old = pred_err + p_meta->xHat;
				
				/* correct range	*/
				lptr[posExt] = MODULO_RANGE( Rx_old, 0, MAXVAL, RANGE);
				if (paeth_mode)
				{
					/* this value belongs to the setof signal values	*/
					paeth_flags[ lptr[posExt] ] = 1; 
				}

				/* if Rx was outside RANGE than also pred_err
				 * must be corrected for updating process
				 */
				//if (Rx < Rx_old)			pred_err -= RANGE;
				//else if (Rx > Rx_old)	pred_err += RANGE;

				/* use the final (combined) estimate for prediction	*/
				meta_error( p_meta, lptr[posExt], RANGEd2, RANGEm1d2, RANGE);
				if (pred_mode & 0x08 )
					eptrLS[pos] = (long)p_meta->pred_err_L;

				/* update weights for abs_err prediction	*/
				update_weights_e( p_predE, px_ptr[pos], abs(eptr[pos]), /* p_templE,*/
					im->width >> 2);

				/* adaptation of weights for lin. pred. contexts,
				 * not for first row/col or constant lines
				 */
				 //if (px_ptr[pos] < num_contexts) 
				if (r > 0 || c > 0) /* not for top-left corner	*/
				{
					update_pred2Weights(p_pred2, px_ptr[pos],
						p_meta->pred_err_a, p_meta->pred_err_b, pred_mode);
					update_pred2Weights_R(p_pred2, px_ptr[pos],
						p_meta->pred_err_R, pred_mode, p_templ);
				}
				// printf("\n"); for (i = 0; i < NUM_OF_PRED; i++) 	printf("%.8f, ", p_pred2->lr_a[i]);
				//printf("%9.5f, ", p_meta->xHat_a_d);

				/* update values for predictor combination
				 * count_px[pos] will be incremented insidemeta_predict( )
				 */
				if (pred_mode & 0x08)
					update_meta_prediction(px_ptr[pos], num_contexts,
						p_meta, pred_mode, count_px, count_pxLS, pxLS_ptr[pos]);
				else
					update_meta_prediction(px_ptr[pos], num_contexts,
						p_meta, pred_mode, count_px, count_pxLS, 0);

			} /* !const_rows	*/
			else
			{
				/* fill row with constant value */
				if (const_rows[r]) lptr[posExt] = row_val[r];
				else							 lptr[posExt] = col_val[c];
				p_meta->pred_err_a = 0.0;
				p_meta->pred_err_b = 0.0;
				p_meta->pred_err_T = 0.0;
				p_meta->pred_err_L = 0.0;
				p_meta->pred_err = 0;
				p_meta->p_err = 0;
				eptr[pos] = 0;
				px_ptr[pos] = num_contexts;
				if (pred_mode & 0x08)
				{
					eptrLS[pos] = 0;
					pxLS_ptr[pos] = num_contextsLS;
				}
				cx = 0;
			} /* if const_row	*/

			/**************************************************************
			 * update processes	
			 */
			

			/* observe sign based on prediction context
			 */
			if (eptr[pos] < 0)
			{
				sign_count[px_ptr[pos]]--;
			}
			else if (eptr[pos] > 0)
			{
				sign_count[px_ptr[pos]]++;
			}

			/* scaling of sums	*/
			if ( (count_px[px_ptr[pos]] % 256) == 0)
			{
				sign_count[px_ptr[pos]] >>= 1;
			}

			if (outx != NULL)
			{
					fprintf( outx, " %3d", px_ptr[pos]);
			}
			if (outc != NULL) fprintf( outc, " %3d", cx);
		}	/* all columns	*/
		//printf("\n");
		if (outx != NULL) fprintf( outx, "\n");
		if (outc != NULL) fprintf( outc, "\n");
		fprintf( stderr, "\r%0.1f%%", 100. * (r+1) / im->height);
	}	/* all rows	*/

	finish_meta_prediction();

#ifdef STAND_ALONE_COBALP
	/* output of prediction error	*/
	if (outr != NULL) 
	{
		if (im->max_value <= 255)
		{
			/* binary mode	*/
			for ( pos = 0; pos < im->size; pos++)
			{
				//index = MODULO_RANGE( eptr[pos], -RANGEd2, RANGEm1d2, RANGE)
				//index += RANGEd2;
				index = eptr[pos] + RANGEd2;
				put_pgm_value( index, outr);
			}
		}
		else
		{	/* ascii mode	*/
			for (r = 0, py = 0; r < (signed)im->height; r++, py+=im->width)	/* all rows	*/
			{
				for (c = 0, pos = py; c < (signed)im->width; c++, pos++)	/* all columns	*/
				{
					index = eptr[pos] + RANGEd2;
					put_pgm_value( index, outr);
				}
				fprintf(outr, "\n");
			}
		}
	}

	if (coding_verbose)
	{
		char	log_name[512];

		sprintf( log_name, "log_CoBALP2_d.txt");
		err =  verbose( log_name, im, imExt, lptr, p_meta, p_pred2,
			const_rows, row_val, const_cols, col_val, M, Mext, th_AC,
			num_contexts, used_AC_contexts, count_px, count_cx, count_cxESC, sign_count, rtn,
			(unsigned long)(ftell( out)+1), p_predE, 0.0);

		if (err)
		{
			goto end;
		}
	}

	{
		//fprintf( stderr,"\n  Bitrate: %.4f", 8.0 * numBytes / im->size);
		fprintf( stderr,"\n  Bitrate: %.3f", 8.0 * ftell( in) / im->size);
	}
	fprintf( stderr,"\n  -----------------");
#endif

end:
	if (pred_mode & 0x4) clean_predictorTM( p_ls);
	/* free some allocated memory	*/
	FREE( eptr);
	FREE( eptrLS);
	FREE( px_ptr);
	FREE( pxLS_ptr);
	FREE( pxExt_ptr);
	FREE( count_pxLS)
	FREE( predMult);
	FREE( paeth_flags);
	FREE( count_px);
	FREE( sum_abserr);


	/* clear arithmetic coding	*/
	for ( k = 0; k < used_AC_contexts; k++)
	{
		free_model( &M[k]);
#ifndef NOMext
		free_model( &Mext[k]);
#endif
	}
	return;
}
