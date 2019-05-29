/*****************************************************************
*
* Datei:	encoder.c
* Funktion:	Encoding
* Autor:	Tilo Strutz
* Datum: 	15.10.2001
* changes
* 04.09.2012 new adaptation CoBALP2
* 15.09.2012 new option -c, new output for contexts
* 08.01.2013 initialization of weights dependent on context
* 09.01.2013 include decode_mode
* 19.03.2013 restructering
* 20.03.2013 transmission of info about constant rows and cols
* 22.03.2013 reorganisation of #defines
* 22.03.2013 code_() Rx = lptr[pos] instead reading from file
* 22.03.2013 outlog only in _DEBUG mode
* 22.03.2013 rice coding of knot data
* 22.03.2013 RLC mode removed
* 25.03.2013 new: kn_TMflag for activation of template matching
* 02.04.2013 new arithmetic coding with different alphabets
* 03.04.2013 kn_TMflag deactivated
* 04.04.2013 new: pred_mode
* 04.04.2013 differential ricecoding for first knot elements
* 08.04.2013 decoder in separate file
* 15.04.2013 complete new determination of thresholds + model_AC
* 19.04.2013 addition of MED predictor
* 27.04.2013 inclusion of PAETH mode
* 29.04.2013 support of ASCII-pgm input
* 02.05.2013 get AC context based on mean abs error in prediction contexts
* 06.05.2013 call of HowSynthetic() switching off Paeth mode if too low
* 28.05.2013 Ra_shift, scaling of Ra in kn[][]
* 20.06.2013 Ra_shift replaced by * Ra_fac_numerator/Ra_fac_denominator 
* 22.06.2013 update_weights_e() reset = image_width /4
* 03.07.2013 bugfix: last model must at least contain +-1 
* 05.07.2013 bugfix: clean_predictorTM() only if TM was enabled
 * 18.07.2013 new: constant_lines_mode
 * 10.09.2013 unsigned short	min_val = (unsigned short)(~(1<<15))
 *						use image data array instead of file handle for input
 * 18.12.2013 bugfix, return w/o processing if component is empty
 * 11.04.2014 write max and min vals in dependence on bit depth
 * 09.01.2015 slightly changed output to console
* 23.01.2015 wA, wB
* 28.10.2015 bugfix memory leak array H
* 13.08.2018 include predictLS()
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types_CoBaLP.h"
#include "codec_CoBaLP.h"
#include "arithmetic.h"
#include "bitbuf.h"
#include "predictor2.h"
#include "predictorTM.h"
#include "med.h"
#include "meta_predictor.h"
#include "contexts.h"
#ifdef STAND_ALONE_COBALP
#include "compaction.h"
#endif

// #define CHECK 99

#define LOG_FLAG 0

//#define WATCH_WEIGHTS
int
fitting_lin( long**px_data, unsigned long num_of_experiments, 
						unsigned int co, double *wb,
						unsigned short num_conditions);

unsigned short HowSynthetic_( unsigned int hist_size, 
														unsigned long *histo /* histogram	*/);

unsigned short estimateNoise_CoBaLP( IMAGE *im, long *lptr);

// unsigned short estimateNoise_CoBaLP( IMAGE *im, long *lptr){return 0;}

#ifdef STAND_ALONE_COBALP
void
encode_( IMAGE *im, IMAGE *imExt, long *lptr, FILE *in, 
				unsigned short num_contexts, 
				BitBuffer *bitbuf,
				unsigned char *const_rows, 
				unsigned char *const_cols, 
				FILE *out, FILE *outr, FILE *outx, FILE *outc, 
				int pred_mode, int paeth_mode);

void write_pgm_header( IMAGE *im, FILE *out);
void write_asc_pgm_header( IMAGE *im, FILE *out);

void put_pgm_value_1( unsigned long val, FILE *out); /* in decoder.c	*/
//void put_pgm_value_2( unsigned long val, FILE *out);
void put_pgm_value_asc( unsigned long val, FILE *out);
#else
void
encode_( IMAGE *im, IMAGE *imExt, long *lptr, 
				PARAM_STRUC	*param, 
				BitBuffer *bitbuf,
				unsigned char *const_rows, 
				unsigned char *const_cols, 
				FILE *out, FILE *outr, FILE *outx, FILE *outc, 
				float wA, float wB, float *entropy);
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


unsigned short  get_AC_thresholds( IMAGE *im, CXE_STRUC * cx_struc, 
																unsigned int th_AC[], 
																unsigned int model_AC[],
																unsigned long num_abs_scaled_errors);

/* global arrays from contexts.c	*/
extern double kn[MAXNUM_OF_PRED_CONTEXTS][NUM_KNOT_ELEM];	/* defined in contexts.c	*/
extern double knLS[MAXNUM_OF_PRED_CONTEXTS][NUM_KNOT_ELEM_LS];	/* defined in contextsLS.c	*/
//extern int Ra_shift;
extern int Ra_fac_numerator;
extern int Ra_fac_denominator;
extern char basename[511];

/*---------------------------------------------------------------
*	encoder()
*---------------------------------------------------------------*/
#ifdef STANDALONE_COBALP
int encoder( IMAGE *im, FILE *in, FILE *out, FILE *outr, FILE *outx, 
						FILE *outc, PARAM_STRUC	*param)
#else
int encoder( IMAGE *im_orig, long *imptr, BitBuffer *bitbuf, 
						PARAM_STRUC	*param, float wA, float wB, float *entropy)
#endif
{
	int r, c, rk, rkd, rka;
	long *lptr = NULL;
	unsigned char *const_rows=NULL; /* arrays for signalling constant lines	*/
	unsigned char *const_cols=NULL;
	unsigned short k, synthetic, noise;
	unsigned long true_max_value, true_min_value;
	unsigned long numBytes_old;
	unsigned long py, pos, pyExt, posExt;
	/* for analysis of gaps in histogram	*/
	unsigned long *H = NULL;	/* histogram of prediction errors	*/
#ifdef STANDALONE_COBALP
	BitBuffer *bitbuf  = NULL;
	unsigned int new_max_val = 0;	/* for histogramm compaction	*/
	unsigned int *idx2val = NULL; /* LUT for undoing the compaction at decoder*/
#endif
	IMAGE im_obj, *im = NULL, imExtended, *imExt = NULL;
	TEMPLATE templ_obj, *p_templ=NULL; /* template for contexts and lin. predictors	*/

	im = &im_obj;
	imExt = &imExtended;
	p_templ = &templ_obj;

	/* copy values from original settings, 
	 * otherwise values will be overwritten in rotation mode
	 */
	im->width = im_orig->width;
	im->height = im_orig->height;
	im->size = im_orig->size;
	im->bit_depth = im_orig->bit_depth;
	im->max_value = im_orig->max_value;
	im->min_value = im_orig->min_value;

	if (0 && im->width > im->height)
	//if (0 && im->width < im->height)
	{
		unsigned int tmp;
		param->rotate_flag = 1;
		tmp = im->width;
		im->width = im->height;
		im->height = tmp;
	}
	else
	{
		param->rotate_flag = 0;
	}


	/* memory for image data with extended borders	*/
	imExt->bit_depth = 0;
	imExt->max_value = im->max_value;
	imExt->height = im->height + OFF_TOP;
	imExt->width = im->width + OFF_LEFT + OFF_RIGHT;
	imExt->size = imExt->height * imExt->width;

	CALLOC( lptr, imExt->size, long);
	CALLOC( H, im->max_value + 1, unsigned long);	/* for entropy calculation	*/

	/* read entire image into correct position
	* determine min and max value
	* extended borders
	*/
	pos = 0;
	if (param->rotate_flag)
	{
		unsigned short	max_val = 0;
		unsigned short	min_val = (unsigned short)(~(1<<15));

		for (c = 0, pyExt = OFF_LEFT; c < (signed)im->width; 
			c++, pyExt++)	/* all columns	*/
		{
			for (r = 0, posExt = pyExt + OFF_TOP*imExt->width; r < (signed)im->height; 
				r++, posExt+=imExt->width)	/* all rows	*/
			{

				/* TSIP lptr[posExt] = (long)get_pgm_value( in); */
				lptr[posExt] = imptr[pos++];
				if (r==0 && c== 0)
				{
					lptr[posExt-1] = lptr[posExt]; /* fake, only for initialisation	*/
				}
				if (max_val < (unsigned)lptr[posExt]) max_val = (unsigned short)lptr[posExt];
				if (min_val > (unsigned)lptr[posExt]) min_val = (unsigned short)lptr[posExt];
				H[lptr[posExt]]++;

				/* misuse of set_template() only to extend the image at borders
				* now contexts() also can access border pixels
				*/
				/* if (r > 2) remove these lines after determination of contexts
				* as the decoder do not have this information
				*/
				set_template( lptr, p_templ, posExt, r, c, imExt->width);
			}
		}
		/* correction of range	*/
		im->min_value = imExt->min_value = min_val;
		im->max_value = imExt->max_value = max_val;
	}
	else
	{
		unsigned long	max_val = 0;
		unsigned long	min_val = (unsigned long)(~(1<<31));

		for (r = 0, pyExt = OFF_TOP*imExt->width; r < (signed)im->height; 
			r++, pyExt+=imExt->width)	/* all rows	*/
		{
			for (c = 0, posExt = pyExt+OFF_LEFT; c < (signed)im->width; 
				c++, posExt++)	/* all columns	*/
			{

				/* TSIP lptr[posExt] = (long)get_pgm_value( in); */
				lptr[posExt] = imptr[pos++];

				if (r==0 && c== 0)
				{
					lptr[posExt-1] = lptr[posExt]; /* fake, only for initialisation	*/
				}
				if (max_val < (unsigned)lptr[posExt]) max_val = lptr[posExt]; //not necessary
				if (min_val > (unsigned)lptr[posExt]) min_val = lptr[posExt];
				H[lptr[posExt]]++;

				/* misuse of set_template() only to extend the image at borders
				* now contexts() also can access border pixels
				*/
				/* if (r > 2) remove these lines after determination of contexts
				* as the decoder do not have this information
				*/
				set_template( lptr, p_templ, posExt, r, c, imExt->width);
			}
		}
		/* correction of range	*/
		im->min_value = imExt->min_value = min_val;
		im->max_value = imExt->max_value = max_val; //not necessary
	}


	im->bit_depth = 0;
	{
		unsigned long domain;
		domain = im->max_value;
		while (domain)
		{
			domain >>= 1; im->bit_depth++;
		}
	}
	imExt->bit_depth = im->bit_depth;

	/* save true values, they might be overwritten in compaction mode	*/
  true_max_value = im->max_value;
  true_min_value = im->min_value;

	/*
	* check histogram compaction
	* get new max_value (min_value should be zero afterwards)
	* provide an array for the LUT
	*/
#ifdef STAND_ALONE_COBALP
	if (param->compact_flag)
	{
		/* LUT for mapping values	back */
		CALLOC( idx2val, im->max_value + 1, unsigned int);

		new_max_val = compact_histogram( imExt, lptr, 
					H, im->max_value + 1, idx2val);
		if (new_max_val == 0) 
		{
			/* no compaction has been performed	*/
			param->compact_flag = 0;
			FREE( idx2val); /* must not be transmitted in this case */
			fprintf( stderr, "\n Compaction mode is switched off");
			fprintf( stdout, "\n Compaction mode is switched off");
		}
		else
		{
			/* use new range	*/
			im->max_value = imExt->max_value = (unsigned short)new_max_val;
			im->min_value = imExt->min_value = 0;

			/* get histogramm of compacted image data	*/
			memset( H, 0, true_max_value + 1 * sizeof(unsigned int));

			for (c = 0, pyExt = OFF_LEFT; c < (signed)im->width; 
				c++, pyExt++)	/* all columns	*/
			{
				for (r = 0, posExt = pyExt + OFF_TOP*imExt->width; r < (signed)im->height; 
					r++, posExt+=imExt->width)	/* all rows	*/
				{
					H[lptr[posExt]]++;
				}
			}
		}
	}
#endif

	synthetic = HowSynthetic_( im->max_value + 1,	/* size of histogram	*/ 
			H /* histogram	*/);
	fprintf( stdout, " synthetic: %d", synthetic);
	noise = estimateNoise_CoBaLP( imExt, lptr);
	fprintf( stdout, "\t noise: %d", noise);
	//noise = plane_approx( imExt, lptr);
	//fprintf( stdout, "\t noise2: %d", noise);

	if (synthetic < 100) /* threshold is not sufficiently tested	*/
	{
		/* paeth mode puts a little burden on natural images	*/
		if (param->paeth_mode)
		{
			fprintf( stderr, "\n Paeth mode is switched off");
			fprintf( stdout, "\n Paeth mode is switched off");
			param->paeth_mode = 0;
		}
	}
	if (synthetic > 180) /* threshold is not sufficiently tested	*/
	{
		if (param->constant_lines_mode)
		{
			fprintf( stderr, "\n constant-lines mode is switched off");
			fprintf( stdout, "\n constant-lines mode is switched off");
			param->constant_lines_mode = 0;
		}
	}

#ifdef _DEBUG
	if (0) /*data	*/
	{
		FILE *outl = NULL;

		outl = fopen( "lptr_data.pgm", "wb");
		fprintf( outl, "P2\n");
		fprintf( outl, 
			"# Created by CoBALP2, (Leipzig University of Telecommunications)\n");
		fprintf( outl, 
			"# Compaction: %d\n", param->compact_flag);
		fprintf( outl, "%d %d\n", imExt->width, imExt->height);
		fprintf( outl, "%d\n", imExt->max_value);

		for (r = 0, py = 0; r < (signed)imExt->height; r++, py += imExt->width)	/* all rows	*/
		{
			for (c = 0, pos = py; c < (signed)imExt->width; c++, pos++)	/* all columns	*/
			{
				fprintf( outl, " %3d", lptr[pos]);
			}
			fprintf( outl, "\n");
		}
		fclose( outl);
	}
#endif

#ifdef STAND_ALONE_COBALP2
	/*----------------------------------------------------------
	* write header info 
	*/
	bitbuf = BufferWriteInit( out);
	BufferPutByte( bitbuf, 'C');
	BufferPutByte( bitbuf, 'o');
	BufferPutByte( bitbuf, 'B');
	BufferPutByte( bitbuf, '0');
	BufferPutByte( bitbuf, '2');
	BufferPutByte( bitbuf, '\0');
#endif

	numBytes_old = numBytes;

	BufferPutByte( bitbuf, (byte)(im->width / 256));
	BufferPutByte( bitbuf, (byte)(im->width % 256));
	BufferPutByte( bitbuf, (byte)(im->height / 256));
	BufferPutByte( bitbuf, (byte)(im->height % 256));
	//BufferPutByte( bitbuf, (byte)(true_max_value / 256));
	//BufferPutByte( bitbuf, (byte)(true_max_value % 256));
	//BufferPutByte( bitbuf, (byte)(true_min_value / 256));
	//BufferPutByte( bitbuf, (byte)(true_min_value % 256));
	BufferPutByte( bitbuf, (byte)im->bit_depth);
	BufferPutByte( bitbuf, (byte)(true_max_value & 0xff));
	BufferPutByte( bitbuf, (byte)(true_min_value & 0xff));
	{
		int	bits;
		unsigned long val1, val2;
		bits = (signed)im->bit_depth - 8;
		val1 = true_max_value >> 8;
		val2 = true_min_value >> 8;
		while (bits > 0)
		{
			BufferPutByte( bitbuf, (byte)(val1 & 0xff));
			BufferPutByte( bitbuf, (byte)(val2 & 0xff));
			bits -= 8;
			val1 >>= 8;
			val2 >>= 8;
		}
	}
	rice_encoding( bitbuf, param->pred_mode, 4);
	BufferPutBit( bitbuf, (cbool)param->paeth_mode);
	BufferPutBit( bitbuf, (cbool)param->compact_flag);
	BufferPutBit( bitbuf, (cbool)param->rotate_flag);

	printf( "\n overhead general: %d bytes", numBytes-numBytes_old);
	numBytes_old = numBytes;

	if (param->rotate_flag)
		printf( "\n image was internally rotated ");

	/* new for empty components	*/
	if (im->max_value == im->min_value)
	{
		param->numOfConstantLines = 0; /* reset value here as corresponding section ist not reached anymore */
		return 0;
	}
	/***************************************************************
	* send histogram compaction
	*/
#ifdef STAND_ALONE_COBALP2
	if (param->compact_flag)
	{
		BufferPutByte( bitbuf, (byte)(new_max_val / 256));
		BufferPutByte( bitbuf, (byte)(new_max_val % 256));

		write_compaction_LUT( im, idx2val, new_max_val, bitbuf);

#ifdef _DEBUG
		if(1)
		{
			FILE *out;
			out = fopen( "idx2val_e.txt", "wt");
			for (pos =0; pos <= new_max_val; pos++)
			{
				fprintf( out, "%4d %4d\n", pos, idx2val[pos]);
			}
			fclose( out);
		}
#endif

		FREE( idx2val);

		printf( "\n    overhead compaction mode: %d bytes", numBytes-numBytes_old);
		numBytes_old = numBytes;

		/*  must be upto date 
		 */
		im->bit_depth = 0;
		{
			unsigned int domain;
			domain = im->max_value;
			while (domain)
			{
				domain >>= 1; im->bit_depth++;
			}
		}
	}
#endif

	/* 
	* find and transmit constant rows or columns
	* for synthetic images the overhead of transmitting the constant lines
	* might be larger than the savings
	*/
	{
		int constant_row_flag=0, constant_col_flag=0, signal_flag, fac;

		param->numOfConstantLines = 0; /* reset counter for each component */

		/* memory for signalling constant rows	*/
		CALLOC( const_rows, im->height, unsigned char);

		/* rows first, exclude extended rows and cols	*/
		signal_flag = 1;
		if (param->constant_lines_mode)
		{
			for (r = 0, py = OFF_TOP * imExt->width; r < (signed)im->height; 
				r++, py+=imExt->width)	/* all rows	*/
			{
				constant_row_flag = 1; /* assume constant row	*/
				/* all columns	*/
				for (c = 1, pos = py+1+OFF_LEFT; c < (signed)im->width; c++, pos++)	
				{
					if (lptr[pos] != lptr[pos-1])
					{
						constant_row_flag = 0;	/* row is not constant	*/
						break;
					}
				}
				if (constant_row_flag == 1) /* row is constant	*/
				{
					param->numOfConstantLines++;
					const_rows[r] = 1; /* will be checked in prediction and coding loops	*/
					if (signal_flag)	/* first constant row */
					{
						BufferPutBit( bitbuf, 1);	/* yes, there are constant rows	*/
						signal_flag = 0;	/* this bit must not be sent again	*/
					}
					BufferPutWord( bitbuf, (uword)r);	/* row number	*/
					if (im->max_value > 255)
					{
						BufferPutWord( bitbuf, (unsigned short)lptr[py+OFF_LEFT]);	/* row value	*/
					}
					else
					{
						BufferPutByte( bitbuf, (unsigned char)lptr[py+OFF_LEFT]);	/* row value	*/
					}
				}
				else
				{
					const_rows[r] = 0; /* this row is not constant	*/
				}
			}
		}
		if (signal_flag)
		{ /* nothing was signalled yet	*/
			BufferPutBit( bitbuf, 0);	/* no, there aren't constant rows	*/
		}
		else
		{
			BufferPutWord( bitbuf, (uword)im->height);	/* termination of transmission	*/
		}

		/* ****************************************************
		* columns
		*/
		/* memory for signalling constant cols	*/
		CALLOC( const_cols, im->width, unsigned char);

		signal_flag = 1;
		if (param->constant_lines_mode)
		{
			for (c = 0, py = OFF_LEFT; c < (signed)im->width; c++, py++)	/* all columns	*/
			{
				constant_col_flag = 1;	/* assume constant column	*/
				fac = 1;	/* distance factor for row comparison	*/
				for (r = 1, pos = py+(OFF_TOP +1)* imExt->width; r < (signed)im->height; 
					r++, pos+=imExt->width)	/* all rows	*/
				{
					if (const_rows[r]) /* current row is constant	*/
					{
						fac++;	/* do not compare to this row	*/
						continue; /* ignore interruptions by constant rows */
					}
					if (lptr[pos] != lptr[pos - fac*imExt->width])
					{
						constant_col_flag = 0; /* column is not constant	*/
						break;
					}
					else fac = 1;	/* reset distance for comparison	*/
				}
				if (constant_col_flag == 1) /* column is constant	*/
				{
					param->numOfConstantLines++;
					const_cols[c] = 1; /* will be checked in prediction and coding loops	*/
					if (signal_flag)	/* first constant col */
					{
						BufferPutBit( bitbuf, 1);	/* yes, there are constant cols	*/
						signal_flag = 0;	/* this bit must not be sent again	*/
					}
					BufferPutWord( bitbuf, (uword)c);	/* row number	*/
					if (im->max_value > 255)
					{
						/* output of column value	*/
						BufferPutWord( bitbuf, (unsigned short)lptr[py+OFF_TOP * imExt->width]);
					}
					else
					{
						/* col value	*/
						BufferPutByte( bitbuf, (unsigned char)lptr[py+OFF_TOP * imExt->width]);	
					}
				}
				else
				{
					const_cols[c] = 0;
				}
			}
		}
		if (signal_flag)
		{ /* nothing was sign alled yet	*/
			BufferPutBit( bitbuf, 0);	/* no, there aren't constant cols	*/
		}
		else
		{
			BufferPutWord( bitbuf, (uword)im->width);	/* termination of transmission	*/
		}

		if (constant_row_flag == 0 && constant_col_flag == 0)
		{ /* reset parameter,  for logging	*/
			param->constant_lines_mode = 0;
		}
	}/* constant rows or cols	*/

	printf( "\n overhead constant lines: %d bytes", numBytes-numBytes_old);
	numBytes_old = numBytes;


	/**************************************************
	* determine prediction contextes,  differences
	*/
	/* number of prediction contexts, use same MACRO in decoder mode	*/
	param->num_contexts = COMPUTE_NUMBER_OF_CONTEXTS( im); 
	if (param->num_contexts < 2) param->num_contexts = 2;
	param->num_contextsLS = param->num_contexts;
	/*
	* determines vectors kn[][] and Ra_shift, both globally declared
	*/
	/* is required for prediction AND coding ! */
	contextsExt( lptr, imExt, &(param->num_contexts), const_rows, const_cols,
		param->exhaustive_flag);

	/* for predictLS()
	* determines vectors knLS[][] and Ra_shift, both globally declared
	*/
	if (0 || param->pred_mode & 0x08)
	{
		contextsExtLS( lptr, imExt, &(param->num_contextsLS), const_rows, const_cols,
			param->exhaustive_flag); 
	}
	else param->num_contextsLS = 0;


#ifdef DO_IT_LATER_IN_ENCODE_
	/* fill extended data which is not available at decoder side
	 * must be same in decoder !!
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

#endif

	/*  is required for prediction AND coding ! */
	{
		/* transmit context knots	*/
		rice_encoding( bitbuf, param->num_contexts, 8);
		//rice_encoding( bitbuf, Ra_shift, 3);
		rice_encoding( bitbuf, Ra_fac_denominator, 8);
		rice_encoding( bitbuf, Ra_fac_numerator, 8);
		rk = 6; /* coding parameter for rice coding	*/
		rka = 6; /* coding parameter for rice coding	*/
		rkd = 5; /* coding parameter for rice coding	*/
		for ( k = 0; k < param->num_contexts; k++)
		{
			int val, i, b, abs0;

			/* first element is sorted in ascending order	*/
			if (k == 0)
			{
				/* absolute value first	*/
				val = abs( (int)kn[k][0]);
				rice_encoding( bitbuf, val, rk);
				/* followed by sign	*/
				if ( kn[k][0] < 0) 	BufferPutBit( bitbuf, 1);
				else if ( kn[k][0] > 0) 	BufferPutBit( bitbuf, 0);
			}
			else /* differential coding, values are always non-negative	*/
			{
				int diff;
				diff = (int)(kn[k][0] - kn[k-1][0]);
				rice_encoding( bitbuf, diff, rkd);
				/* adapt coding parameter	*/
				b = 0;
				while (diff){ b++; diff >>= 1;}
				if ( b > 2*rkd)
					rkd = (rkd + b) >> 1;	/* fast adaptation	*/
				else
					rkd = (rkd * 7 + b) >> 3;	/* slower adaptation	*/
			}
			abs0 = abs( (int)kn[k][0]);
			/* remaining elements	*/
	#ifdef PCX_INCLUDE_RA
			for ( i = 1; i < NUM_KNOT_ELEM-1; i++)
	#else
			for ( i = 1; i < NUM_KNOT_ELEM; i++)
	#endif
			{
				val = abs( (int)kn[k][i]);
				//if (abs0 > 255) val >>= 1;
				//if (abs0 > 511) val >>= 1;
				//if (abs0 > 1023) val >>= 1;
				/* absolute value first	*/
				rice_encoding( bitbuf, val, rk);
				//if (abs0 > 1023) kn[k][i] = (double)(val << 3);
				//else if (abs0 > 511) kn[k][i] = (double)(val << 2);
				//else if (abs0 > 255) kn[k][i] = (double)(val << 1);

				/* followed by sign	*/
				if ( kn[k][i] < 0)
				{
					BufferPutBit( bitbuf, 1);
					//kn[k][i] = -val;
				}
				else if ( kn[k][i] > 0) 
				{
					BufferPutBit( bitbuf, 0);
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
				/* absolute value first	*/
				val = abs( (int)kn[k][i]);
				/* is multiple of 4, see contexts.c	*/
				//val >>= 2;
				val /= ((Ra_fac_numerator >> 2) + 1);

				rice_encoding( bitbuf, val, rka);
				/* adapt coding parameter	*/
				b = 0;
				while (val){ b++; val >>= 1;}
				if ( b > 2*rka)
					rka = (rka + b) >> 1;	/* fast adaptation	*/
				else
					rka = (rka * 7 + b) >> 3;	/* slower adaptation	*/
			}
	#endif
		}
	}
// !!!
//param->num_contexts = param->num_contextsLS; //wenn px mit pxLS in meta_predictor überschrieben ist
// !!

	if (0 || param->pred_mode & 0x08 )
	{
		/* transmit LS px contexts	*/
		rice_encoding( bitbuf, param->num_contextsLS, 8);
		rk = 6; /* coding parameter for rice coding	*/
		rka = 6; /* coding parameter for rice coding	*/
		rkd = 5; /* coding parameter for rice coding	*/
		for ( k = 0; k < param->num_contextsLS; k++)
		{
			int val, i, b, abs0;

			/* first element is sorted in ascending order	*/
			if (k == 0)
			{
				/* absolute value first	*/
				val = abs( (int)knLS[k][0]);
				rice_encoding( bitbuf, val, rk);
				/* followed by sign	*/
				if ( knLS[k][0] < 0) 	BufferPutBit( bitbuf, 1);
				else if ( knLS[k][0] > 0) 	BufferPutBit( bitbuf, 0);
			}
			else /* differential coding, values are always non-negative	*/
			{
				int diff;
				diff = (int)(knLS[k][0] - knLS[k-1][0]);
				rice_encoding( bitbuf, diff, rkd);
				/* adapt coding parameter	*/
				b = 0;
				while (diff){ b++; diff >>= 1;}
				if ( b > 2*rkd)
					rkd = (rkd + b) >> 1;	/* fast adaptation	*/
				else
					rkd = (rkd * 7 + b) >> 3;	/* slower adaptation	*/
			}
			abs0 = abs( (int)knLS[k][0]);
			/* remaining elements	*/
			for ( i = 1; i < NUM_KNOT_ELEM_LS; i++)
			{
				val = abs( (int)knLS[k][i]);
				/* absolute value first	*/
				rice_encoding( bitbuf, val, rk);

				/* followed by sign	*/
				if ( knLS[k][i] < 0)
				{
					BufferPutBit( bitbuf, 1);
				}
				else if ( knLS[k][i] > 0) 
				{
					BufferPutBit( bitbuf, 0);
				}
				/* adapt coding parameter	*/
				b = 0;
				while (val){ b++; val >>= 1;}
				if ( b > 2*rk)
					rk = (rk + b) >> 1;	/* fast adaptation	*/
				else
					rk = (rk * 7 + b) >> 3;	/* slower adaptation	*/
			}
		}
	}
#ifdef STAND_ALONE_COBALP
	if (outr != NULL) /* prediction errors	*/
	{
		unsigned int k, i;
		if (im->max_value > 255) fprintf( outr, "P2\n");
		else fprintf( outr, "P5\n");
		fprintf( outr, 
			"# Created by CoBALP2, (Leipzig University of Telecommunications)\n");
		fprintf( outr, "%s", KNOT_STRING);
		for ( k = 0; k < num_contexts; k++)
		{
			fprintf( outr, "#%d", k);
			for ( i = 0; i < NUM_KNOT_ELEM; i++)
			{
				fprintf( outr, " %.0f", kn[k][i]);
			}
			fprintf( outr, "\n");
		}
		fprintf( outr,"%d %d\n", im->width, im->height);
		fprintf( outr,"%d\n", im->max_value);
	}
	printf( "\n number of pred. contexts: %d", num_contexts);
	printf( "\n overhead pred. contexts: %d bytes", numBytes-numBytes_old);
	numBytes_old = numBytes;

	/* main encoding routine	*/
	encode_( im, imExt, lptr, in, num_contexts, bitbuf, 
		const_rows, const_cols,
		out, outr, outx, outc, param->pred_mode, param->paeth_mode);
	BufferClose( bitbuf);

#else
	encode_( im, imExt, lptr, 
		param, 
		bitbuf, 
		const_rows, const_cols,
		NULL, NULL, NULL, NULL,
		wA, wB, entropy);
#endif

	FREE( lptr);
	//FREE( lptr_);
	FREE( const_rows);
	FREE( const_cols);
	FREE( H);

	return 0;
}

/*---------------------------------------------------------------
*	encode_()
*---------------------------------------------------------------*/
#ifdef STAND_ALONE_COBALP
void encode_( IMAGE *im, IMAGE *imExt, long *lptr, FILE *in, 
						 unsigned short num_contexts,  BitBuffer *bitbuf, 
						 unsigned char *const_rows, 
						 unsigned char *const_cols, 
						 FILE *out, FILE *outr, FILE *outx, FILE *outc,
						 int pred_mode, int paeth_mode)
{
	char	*rtn="encode_";
	long	*row_val=NULL, *col_val=NULL; /* not used	in encoder */
#else
void encode_( IMAGE *im, IMAGE *imExt, long *lptr, 
							PARAM_STRUC	*param, 
						 BitBuffer *bitbuf, 
						 unsigned char *const_rows, 
						 unsigned char *const_cols, 
						 FILE *out, FILE *outr, FILE *outx, FILE *outc,
						 float wA, float wB, float *entropy)
{
	out = NULL; /* avoid warnings	*/
	outr = NULL; /* avoid warnings	*/
	outx = NULL; /* avoid warnings	*/
	outc = NULL; /* avoid warnings	*/
#endif
	int 	err = 0;
	long	sign_count[MAXNUM_OF_PRED_CONTEXTS];/* accumulates signs (+1,-1) */
	long	*eptr = NULL;	/*  prediction error	*/
	long	*eptrLS = NULL;	/*  prediction error of LS predictor	*/
	long	index; /* prediction error + offset	*/
	long 	RANGEd2, RANGEm1d2, RANGE, MAXVAL, RANGEd4;
	double H; /* entropy	*/

	unsigned char *paeth_flags = NULL;
	unsigned short k, i;	/* prediction context	*/
	//unsigned short cxr;	/*  context	*/
	unsigned short cx, used_AC_contexts = 0;	/* arithmetic-coding context	*/
	unsigned short	*px_ptr = NULL;	/* prediction context	array	*/
	unsigned short	*pxLS_ptr = NULL;	/* prediction context	array	*/
	unsigned short	*pxExt_ptr = NULL;	/* extended prediction context	array	*/
  unsigned short *predMult = NULL;
  unsigned int r, c;
	unsigned long numBytes_old;
	/* for AC context modelling	*/
	unsigned int th_AC[NUM_AC_CONTEXTS];
	unsigned int model_AC[NUM_AC_CONTEXTS];
	//unsigned long count_px[MAXNUM_OF_PRED_CONTEXTS];
	//unsigned long count_pxLS[MAXNUM_OF_PRED_CONTEXTS];
	unsigned long *count_px = NULL; // [MAXNUM_OF_PRED_CONTEXTS];
	unsigned long *count_pxLS = NULL; // [MAXNUM_OF_PRED_CONTEXTS];
	unsigned long count_cx[NUM_AC_CONTEXTS];	/* for statistics	*/
	unsigned long count_cxESC[NUM_AC_CONTEXTS];
	/* number of different scaled absolute pred.errors	*/
	unsigned long num_abs_scaled_errors;	
	unsigned long abs_err;	/* estimate of prediction-error magnitude	*/
	//unsigned long sum_abserr[MAXNUM_OF_PRED_CONTEXTS];	/* sums */
	unsigned long *sum_abserr = NULL; // [MAXNUM_OF_PRED_CONTEXTS];	/* sums */
	unsigned long py, pos, pyExt, posExt;
	/* for statistics	*/
	unsigned long *H_err = NULL;	/* histogram of prediction errors	*/
#ifdef FITTING
	unsigned long	***px_data = NULL;	/* prediction context	array	*/
#endif
	PRED2 pred2_obj, *p_pred2; /* variables for predictor2 A, B	*/
	PREDE predE_obj, *p_predE; /* variables for predictor2 A, B	*/
	META_PRED	meta_pred_obj, *p_meta=NULL; /* variables for combining predictors	*/
	/* template for contexts and lin. predictors	*/
	TEMPLATE templ_obj, *p_templ=NULL;	
	TEMPLATE_E templE_obj, *p_templE=NULL;	
	acModell M[NUM_AC_CONTEXTS];	/* normal AC context	*/
	acModell Mext[NUM_AC_CONTEXTS]; /* extended context for large values	*/
	acState acState_obj, *acSt;
	CXE_STRUC *cx_struc;
	LEAST_SQUARES ls_obj, *p_ls;

	p_ls = &ls_obj;
	acSt = &acState_obj;
	p_pred2 = &pred2_obj;
	p_meta = &meta_pred_obj;
	p_templ = &templ_obj;
	p_predE = &predE_obj;
	p_templE = &templE_obj;


	/* im->max_value is always the max Value from the original image
	* as this value is also copied in the prediction-error-image file
	*/
	MAXVAL = im->max_value;
	RANGE = MAXVAL + 1;
	RANGEd2 = RANGE / 2;
	RANGEd4 = RANGE / 4;  /* for improved AC */
	RANGEm1d2 = (RANGE - 1) / 2;

	CALLOC(count_px, MAXNUM_OF_PRED_CONTEXTS, unsigned long);
	CALLOC(count_pxLS, MAXNUM_OF_PRED_CONTEXTS, unsigned long);
	CALLOC(sum_abserr, MAXNUM_OF_PRED_CONTEXTS, unsigned long);


	/* array for absolute prediction error	*/
	CALLOC( eptr, im->size, long);
	/* array for prediction context	*/
	CALLOC( px_ptr, im->size, unsigned short);
	if (param->pred_mode & 0x08)
	{
		/* array for LS prediction error	*/
		CALLOC( eptrLS, im->size, long);
		CALLOC(pxLS_ptr, im->size, unsigned short);
		CALLOC(predMult, param->num_contextsLS, unsigned short);
	}
	/* initialize linera predictors A, B, R	*/
	init_pred2Weights( p_pred2, im->max_value, wA, wB);
	init_pred2Weights_R( p_pred2, im->max_value);
	/* initialize combination of predictors	*/
	err = init_meta_prediction( p_meta, param->pred_mode);
	if (err) goto end;

	if (param->pred_mode & 0x4) 
		init_predictorTM( imExt, p_ls); /* initialise offset array for TM predictor	*/
	if (param->pred_mode & 0x08 )
		init_pred2Weights_L( bitbuf, imExt, lptr, im, param->num_contextsLS,
												pxLS_ptr, p_pred2);

	for ( k = 0; k < MAXNUM_OF_PRED_CONTEXTS; k++)
	{
		count_px[k] = 0;	/* reset context counter	*/
		count_pxLS[k] = 0;	/* reset context counter	*/
		/* will be reset again before coding loop	*/
	}

	/*****************************************************************
	* prediction and analysis in encoder
	*/
	/* allocate memory for histogram of absolute prediction errors	*/
	num_abs_scaled_errors = (RANGEd2 + 1) * TH_FAC;/* plus one as abs(-128) is 127+1 !!*/
	CALLOC( cx_struc, num_abs_scaled_errors, CXE_STRUC);
	for ( k = 0; k < num_abs_scaled_errors; k++)
	{
		CALLOC( cx_struc[k].distribution, RANGE, unsigned long);
	}

	CALLOC( H_err, RANGE+1, unsigned long);	/* for entropy calculation	*/
	if (param->paeth_mode) CALLOC( paeth_flags, RANGE, unsigned char);


	init_weights_e( p_predE);
	/* extended array for prediction contexts, required for predictorCM() */
	CALLOC( pxExt_ptr, imExt->size, unsigned short)

	if (param->pred_mode & 0x08 && 0)
	{
		init_pred2Weights_Mult( imExt, lptr, im, param->num_contextsLS, predMult);

		/* save  predictor selection	*/
		for ( k = 0; k < param->num_contextsLS; k++)
		{
				rice_encoding( bitbuf, predMult[k], 3);
		}
		/* save prediction coefficients	*/
		if(0)
		{
			//int rkVal, rk[LS_TEMPLATE_SIZE] = { 9,9,8,8,8,8};
			//int rkSum[LS_TEMPLATE_SIZE] = { 9,9,8,8,8,8};
			//unsigned int absMean;
			//unsigned int absSum[LS_TEMPLATE_SIZE] = { 0,0,0,0,0,0};
			fprintf( stdout, "\nprediction coefficients");
			for ( k = 0; k < param->num_contextsLS; k++)
			{
				long *lwL = p_pred2->lweights_L[k];
				fprintf( stdout, "\n%d: ", k);
				for (i = 0; i < LS_TEMPLATE_SIZE; i++)
				{
					fprintf( stdout, "%5d, ", lwL[i]);
					rice_encoding( bitbuf, abs( lwL[i]), 8);
					//rice_encoding( bitbuf, abs( lwL[i]), rk[i]);
					if (lwL[i] < 0) 
					{
						BufferPutBit( bitbuf, true);
					}
					else 
					{
						BufferPutBit( bitbuf, false);
					}
					//absSum[i] += abs( lwL[i]);
					//absMean = (absSum[i] + ((k + 1)>>1) )/ (k + 1); /* average over all contexts	*/
					//rkVal = 0;
					//while( absMean) { absMean >>= 1; rkVal++;}
					//rkSum[i] += rkVal;
					//rk[i] = (rkSum[i] + ((k + 2)>>1) )/ (k + 2);
				}
			}
			fprintf( stdout, "\n");
		}
	}

	/* moved from encode() to here	*/
	/* fill extended data which is not available at decoder side
	 * must be same in decoder !!
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

	/********************************************************************************
	* do the prediction for entire image
	*/
	fprintf( stderr, "\n");
	for (r = 0, py = 0, pyExt = OFF_TOP * imExt->width; r < (signed)im->height; 
		r++, py+=im->width, pyExt+=imExt->width)	/* all rows	*/
	{
		for (c = 0, pos = py, posExt = pyExt + OFF_LEFT; c < (signed)im->width; 
			c++, pos++, posExt++)	/* all columns	*/
		{
			/* copy neighbour values to template,
			* must always be performed, because it must fill the borders for next lines
			*/
			set_template( lptr, p_templ, posExt, r, c, imExt->width);
#ifdef _DEBUG
if (r == 6 && c ==667)
{
	c=c;
}
#endif
			/* nothing to predict if row or col has constant value	*/
			if ( !const_rows[r] && !const_cols[c])
			{

				/* do the prediction using all predictors	*/
				px_ptr[pos] = meta_predict( im, imExt, lptr, pxExt_ptr, posExt,
					r, c,	p_templ, p_meta, p_pred2, 
					param->pred_mode, param->num_contexts, param->num_contextsLS,
					count_px, count_pxLS, pxLS_ptr, predMult, eptrLS, p_ls);

				if (param->paeth_mode)
				{
					if ( !paeth_flags[p_meta->xHat] )
					{
						long p, da, db, dc;

						/* use PAETH predictor	*/
						/* plane extrapolation  p = A + C-B */	
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
					paeth_flags[ lptr[posExt] ] = 1; 
					/* this value belongs to the set of signal values	*/
				}

#ifdef WATCH_WEIGHTS
				if (px_ptr[pos] >= num_contexts/2 -2 && px_ptr[pos] <= num_contexts/2 + 2)
				{
					int i;
					char filename[200];
					FILE *out;

					sprintf( filename, "weights_%03d.txt", px_ptr[pos]);
					out = fopen( filename, "at");
					fprintf( out, "\n");
					for ( i = 0; i < NUM_OF_PRED; i++)
					{
						fprintf(  out, " %7.4f", p_pred2->weights_b[px_ptr[pos]][i]);
					}
					fclose( out);

					//sprintf( filename, "data_%03d.txt", px_ptr[pos]);
					//out = fopen( filename, "at");
					//fprintf( out, "\n %3d,", lptr[posExt] - p_templ->Rb);
					//for ( i = 0; i < NUM_OF_PRED; i++)
					//{
					//	fprintf(  out, " %3d", p_pred2->difference_i_b[i]);
					//}
					//fclose( out);
				}
#endif


				/* use the final (combined) estimate for prediction	*/
				meta_error( p_meta, lptr[posExt], RANGEd2, 
					RANGEm1d2, RANGE);
				if (param->pred_mode & 0x08)
					eptrLS[pos] = (long)p_meta->pred_err_L;

//if (abs( p_meta->pred_err) > 5 + fabs(p_meta->pred_err_b))
//{
//	r = r;
//}
				eptr[pos] = p_meta->p_err; /* including modulo RANGE	*/

				/* distribution for entropy computation	*/
				H_err[eptr[pos] + RANGEd2]++; /* shift into non-negative range	*/

				/* get averaged prediction error in neighbourhood */
				//abs_err = get_averaged_error( im, eptr, pos, r, c, px_ptr,
				//	p_meta->sum_abserr[px_ptr[pos]], count_px[ px_ptr[pos]]);
				abs_err = get_averaged_error( im, eptr, pos, r, c, px_ptr,
					p_meta->sum_abserr[px_ptr[pos]], count_px[ px_ptr[pos]], 
					p_predE, p_templE, num_abs_scaled_errors);

				assert( (signed)abs_err < num_abs_scaled_errors);
				assert( eptr[pos] >= -RANGEd2);
				assert( eptr[pos] < RANGE);
				/* abserr defines the context for which the histogramm is built up	*/
				cx_struc[abs_err].distribution[eptr[pos] + RANGEd2]++;

				/* update weights for abs_err prediction	*/
				update_weights_e( p_predE, px_ptr[pos], abs(eptr[pos]), /* p_templE,*/
					im->width >> 2);

				/* adaptation of weights for lin. pred. contexts, */
				//if (px_ptr[pos] < num_contexts) /* not for first row/col because the predictors are not used there */
				if (r > 0 || c > 0) /* not for top-left corner	*/
				{
					update_pred2Weights( p_pred2, px_ptr[pos], 
						p_meta->pred_err_a, p_meta->pred_err_b, param->pred_mode);
					update_pred2Weights_R( p_pred2, px_ptr[pos], 
						p_meta->pred_err_R, param->pred_mode, p_templ);
				}
				// printf("\n");				for (i = 0; i < NUM_OF_PRED; i++)	printf("%.8f, ", p_pred2->lr_a[i]);
				// printf("%9.5f, ", p_meta->xHat_a_d);
				/* set learning rate	*
				if (0)
				{
					int i;
					double val;
					val = 0.0001 + 0.005 * exp( -(long)count_px[ px_ptr[pos] ] * 0.005);
					for ( i = 0; i < NUM_OF_PRED; i++)
					{
						p_pred2->lr[i] = (float)(val / (i+1));
					}
				}
				/* update values for predictor combination	*/
				if (param->pred_mode & 0x08)
					update_meta_prediction(px_ptr[pos], param->num_contexts,
						p_meta, param->pred_mode, count_px, count_pxLS, pxLS_ptr[pos]);
				else
					update_meta_prediction(px_ptr[pos], param->num_contexts,
						p_meta, param->pred_mode, count_px, count_pxLS, 0);
			}
			else
			{
				eptr[pos] = 0;
				/* simulate get_context()	*/
				px_ptr[pos] = param->num_contexts;
				H_err[RANGEd2]++;
			}
		} /* for c */
		//printf("\n");
		//fprintf( stderr, "\r%d%%  ", 100 * (r+1)  / im->height);
		fprintf( stderr, "\r%.2f%%  ", 100. * (r+1)  / im->height);
	}	/* for r	*/

	finish_meta_prediction( );
	FREE( pxExt_ptr);

	/* free some allocated memory	*/
	FREE( paeth_flags);

	if (coding_verbose && 0)
	{
		FILE *outL=fopen( "lptr.pgm", "w");

		fprintf( outL,"P2\n");
		fprintf( outL,"%d %d\n", imExt->width, imExt->height);
		fprintf( outL,"%d\n", imExt->max_value);
		for (r = 0, py = 0; r < (signed)imExt->height; 
			r++, py+=imExt->width)	/* all rows	*/
		{
			for (c = 0, pos = py; c < (signed)imExt->width; c++, pos++)	/* all columns	*/
			{
				fprintf( outL,"%3d ", lptr[pos]);
			}
			fprintf( outL,"\n");
		}
		fclose( outL);
	}
#ifdef _DEBUG
	if ((coding_verbose && 0) || 0)
	{
		int  ii;
		FILE *outL=fopen( "mu.txt", "w");

		fprintf( outL,"#learning rates \n");
		//for ( k = 0; k < MAXNUM_OF_PRED_CONTEXTS; k++)
		{
			//fprintf( out,"context %d\n", k);
			for ( ii = 0; ii < NUM_OF_PRED; ii++)
			{
				fprintf( outL, "%7.5f ", p_pred2->lr_a[ii]);
			}
			fprintf( outL,"\n");
			for ( ii = 0; ii < NUM_OF_PRED; ii++)
			{
				fprintf( outL,"%7.5f ", p_pred2->lr_b[ii]);
			}
			fprintf( outL,"\n");
		}
		fclose( outL);
	}
	if ((coding_verbose && 0) || 0)
	{
		FILE *outL=fopen( "err_2_17.pgm", "w");

		fprintf( outL,"P2\n");
		fprintf( outL,"%d %d\n", im->width, im->height);
		//fprintf( out,"%d\n", im->max_value);
		fprintf( outL,"%d\n", 255);
		for (r = 0, py = 0; r < (signed)im->height; 
			r++, py+=im->width)	/* all rows	*/
		{
			for (c = 0, pos = py; c < (signed)im->width; c++, pos++)	/* all columns	*/
			{
				//fprintf( outL,"%3d ", eptr[pos] + (signed)((im->max_value+1)>>1));
				fprintf( outL,"%3d ", eptr[pos] + 128);
			}
			fprintf( outL,"\n");
		}
		fclose( outL);
	}
	if ( (coding_verbose && 0) || 0)
	{
		FILE *outL=fopen( "px.pgm", "w");

		fprintf( outL,"P2\n");
		fprintf( outL,"%d %d\n", im->width, im->height);
		fprintf( outL,"%d\n", param->num_contexts+2);
		for (r = 0, py = 0; r < (signed)im->height; 
			r++, py+=im->width)	/* all rows	*/
		{
			for (c = 0, pos = py; c < (signed)im->width; c++, pos++)	/* all columns	*/
			{
				fprintf( outL,"%3d ", px_ptr[pos]);
			}
			fprintf( outL,"\n");
		}
		fclose( outL);
	
		outL=fopen( "pxLS.pgm", "w");

		fprintf( outL,"P2\n");
		fprintf( outL,"%d %d\n", im->width, im->height);
		fprintf( outL,"%d\n", param->num_contexts+2);
		for (r = 0, py = 0; r < (signed)im->height; 
			r++, py+=im->width)	/* all rows	*/
		{
			for (c = 0, pos = py; c < (signed)im->width; c++, pos++)	/* all columns	*/
			{
				fprintf( outL,"%3d ", pxLS_ptr[pos]);
			}
			fprintf( outL,"\n");
		}
		fclose( outL);
	}
#endif
	/* do some statistics	*/
	{
		double p;		/* entropy of prediction errors	*/ 
		long idx;
		unsigned long sum = 0;

		H = 0.0;
		for (idx = 0; idx <= RANGE; idx++)
		{
			if (H_err[idx])
			{
				sum += H_err[idx];
				p = (double)H_err[idx] / (double)im->size;
				H -= (p * log( p) / LOG2);
			}
		}
		assert( sum == im->size);
		fprintf( stderr,"\n  -----------------");
		fprintf( stderr,"\n  Mean   : %.4f", (double)p_meta->mean_tot / im->size);
		fprintf( stderr,"    Entropy: %.3f", H);
		fprintf( stderr,"\n  -----------------");
		printf( "  Entropy: %.3f", H);
#ifndef STAND_ALONE_COBALP
		*entropy = (float)H;
#endif
	}
#ifdef TAUGTNIX
	/********************************************************************************
	* do the prediction refinement for entire image
	*/
	/* array for absolute prediction error	*/
	CALLOC( e2ptr, im->size, long);
	/* reset histogramm	*/
	for ( c = 0; c <= (unsigned)RANGE; c++)
	{
		H_err[c] = 0;
	}

	for (r = 0, py = 0; r < im->height; r++, py+=im->width)	/* all rows	*/
	{
		for (c = 0, pos = py; c < im->width; c++, pos++)	/* all columns	*/
		{

			if (c > 1 && r > 1 && c < im->width - 2)
			{
				refine_residual( im, pos, 1, eptr, e2ptr, px_ptr);
				e2ptr[pos] = MODULO_RANGE( e2ptr[pos], -RANGEd2, RANGEm1d2, RANGE);
			}
			else
			{
				e2ptr[pos] = eptr[pos];
			}
			/* distribution for entropy computation	*/
			H_err[e2ptr[pos] + RANGEd2]++; /* shift into non-negative range	*/
		}
	}


	/* do some statistics	*/
	{
		double p, H = 0.0;		/* entropy of prediction errors	*/ 
		long ii;
		unsigned long sum = 0;

		for ( ii = 0; ii <= RANGE; ii++)
		{
			if (H_err[ii])
			{
				sum += H_err[ii];
				p = (double)H_err[ii] / (double)im->size;
				H -= (p * log( p) / LOG2);
			}
		}
		assert( sum == im->size);
		fprintf( stderr,"\n  -----------------");
		fprintf( stderr,"\n  Entropy: %.3f", H);
		fprintf( stderr,"\n  -----------------");
		printf( "  Entropy: %.3f", H);
	}
	{
		long *tmp;
		tmp = eptr;
		eptr = e2ptr;
		e2ptr = tmp;
	}
#endif

	/* use error distribution for determining the thresholds and
	* model sizes of AC contexts
	*/
	used_AC_contexts = get_AC_thresholds( im, cx_struc, th_AC, 
		model_AC, num_abs_scaled_errors);
	printf( "\n number of AC contexts: %d", used_AC_contexts);

	FREE( H_err);
	for ( k = 0; k < num_abs_scaled_errors; k++)
	{
		FREE( cx_struc[k].distribution);
	}
	FREE( cx_struc);

#ifdef STAND_ALONE_COBALP
	if (outx != NULL) /* output of prediction contexts	*/
	{
		unsigned short tmp;
		tmp = im->max_value;
		im->max_value = num_contexts + 3;
		write_asc_pgm_header( im, outx);
		im->max_value = tmp;
	}
	if (outc != NULL)  /* output of contexts for arithmetic coding	*/
	{
		unsigned short tmp;
		tmp = im->max_value;
		im->max_value = used_AC_contexts;
		write_asc_pgm_header( im, outc);
		im->max_value = tmp;
	}
#endif

	numBytes_old = numBytes;
	/* transmit parameters of AC models	*/
	{
		int rk;
		unsigned int last_val;

#ifndef FIXED_AC_CONTEXTS
		rice_encoding( bitbuf, used_AC_contexts, 4); /* factor value */
		/* transmit thresholds	*/
		rk = 0; last_val = 0;
		// for ( k = 0; k < used_AC_contexts; k++) last upper threshold is not required
		for ( k = 0; k < used_AC_contexts-1; k++)
		{
			unsigned long diff;
			assert( th_AC[k] >= last_val);
			diff = th_AC[k] - last_val;
			rice_encoding( bitbuf, diff, rk);
			last_val = th_AC[k];
			rk = 1;
			while( diff) { diff >>= 1; rk++;}
		}
		printf( "\n overhead AC contexts: %d bytes", numBytes-numBytes_old);
		numBytes_old = numBytes;
#endif

#ifndef FULL_ALPHABET		/* transmit model deviation	*/
		rk = 3; last_val = 0;
		for ( k = 0; k < used_AC_contexts; k++)
		{
			unsigned long diff;
			assert( model_AC[k] >= last_val);
			diff = model_AC[k] - last_val;
			rice_encoding( bitbuf, diff, rk);
			last_val = model_AC[k];
			rk = 0;
			while( diff) { diff >>= 1; rk++;}
		}
		printf( "\n overhead AC models: %d bytes", numBytes-numBytes_old);
		numBytes_old = numBytes;
#endif
	}
	if (model_AC[used_AC_contexts - 1] < 1)
	{
		//fprintf( stderr, "\n model_AC[used_AC_contexts - 1] = %d", model_AC[used_AC_contexts - 1]);
		/* at least one model must contain +-1 for proper use of M[cx] and Mext[cx]*/
		model_AC[used_AC_contexts - 1] = 1;
	}
#ifdef STAND_ALONE_COBALP /* numBytes includes all bytes of previous components*/
	fprintf( stderr, "\n overhead total: %d bytes\n", numBytes);
	printf( "\n overhead total: %d bytes\n", numBytes);
#endif

	/*##################################################################
	*
	*##################################################################
	*/
	/* reset values according to decoder	*/
	for ( k = 0; k < MAXNUM_OF_PRED_CONTEXTS; k++)
	{
		sign_count[k] = 0;	/* observe sign of prediction error	*/
		count_px[k] = 0;	/* reset context counter	*/
		sum_abserr[k] = 0;	/* reset context counter	*/
	}


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

	start_encoding( acSt); /* reset internal values	*/
	//glob_sign = 1; /* incase that this value is not set in get_averaged_error()	*/
	init_weights_e( p_predE);

	/* -------------------------------------------------------------
	* coding loop
	*/
	fprintf( stderr, "\n");
	for (r = 0, py = 0, pyExt = OFF_TOP * imExt->width; r < (signed)im->height; 
		r++, py+=im->width, pyExt+=imExt->width)	/* all rows	*/
	{
		for (c = 0, pos = py, posExt = pyExt + OFF_LEFT; c < (signed)im->width; 
			c++, pos++, posExt++)	/* all columns	*/
		{
			long pred_err;

			/* nothing to encode if row has constant value	*/
			if ( !const_rows[r] && !const_cols[c])
			{
				/* determine context for AC	based on surrounding absolute errors
				* required for both ecoder and decoder
				*/
				//cx = get_AC_context( im, eptr, pos, r, c, px_ptr, th_AC, used_AC_contexts,
				//			sum_abserr[ px_ptr[pos] ], count_px[ px_ptr[pos] ]);
				cx = get_AC_context( im, eptr, pos, r, c, px_ptr, th_AC, used_AC_contexts,
					sum_abserr[ px_ptr[pos] ], count_px[ px_ptr[pos] ], 
					p_predE, p_templE, num_abs_scaled_errors, &abs_err);

				if (r+c == 0) cx = used_AC_contexts - 1; /* error will be large, take widest model	*/ 
				assert( cx < used_AC_contexts);
				if (coding_verbose) count_cx[cx]++;	/* for statistics	*/

				// Rx = lptr[posExt];	/* read value from array	*/
				pred_err = eptr[pos];

				if (count_px[px_ptr[pos]] > NUM_SKIPPED_STEPS) /* skip transient response	*/
				{ /* must be same as in prediction loop	*/
					sum_abserr[px_ptr[pos]] += abs( pred_err);
				}

				/* create non-symmetric distribution	*/
				//if (glob_sign < 0)  
				//	pred_err = -pred_err;
				//else 
#ifndef NOSignFlip
				// if (sign_count[px_ptr[pos]] < 0)  pred_err = -pred_err;
				if (sign_count[px_ptr[pos]] < 0)  pred_err = -pred_err;
#endif
				/* ..............	*/
				{
					int stop;
					long symbol, off;

					off = M[cx].K / 2; /* get position of zero	*/

					if (off == 0)
					{
						off = off;
					}
					/*                   
					*                0  1          off
					* range in cx: ESC, -off+1 ... 0 ... + off-1 
					*
					*/
					symbol = pred_err + off; /* non-negative symbol	*/
					if (symbol > 0 && symbol < (signed)(M[cx].K))
					{
						/* do the arithmetic coding	*/
						//Encode_core( &M[cx], acSt, symbol, bitbuf);
						encode_AC( &M[cx], acSt, symbol, bitbuf);
						/* Update Model */
						update_model( M[cx].H, M[cx].K, symbol);
					}
					else
					{
						/* prediction error is outside model size
						* encode zero as symbol for exception handling
						*/
						/* do the arithmetic coding	of exception handling */
						//Encode_core( &M[cx], acSt, 0, bitbuf);
						encode_AC( &M[cx], acSt, 0, bitbuf);
						/* Update Model */
						update_model( M[cx].H, M[cx].K, 0);

						index = pred_err; /* copy since index will be changed	*/
						/* narrow the symbol and try again	*/
						if (coding_verbose && 0)
						{
							printf( "\n ");
						}
						do
						{
							if (coding_verbose)count_cxESC[cx]++;

							if (coding_verbose && 0)
							{
								printf( "%d not in cx=%d, ", index, cx);
							}
							stop = 1; /* assume that reduced symbol will fit next model	*/
							/* reduce by absolute model size = off -1	*/
							if (index < 0) index  = -( (-index) - (off - 1) ); 
							else index = index - (off - 1);
							/* use next context	with possibly greater model size */
							if (cx < used_AC_contexts - 1)
							{
								cx++; /* Mext[] will be never used !	*/
								off = Mext[cx].K / 2;
							}
							symbol = index + off; /* get symbol based on model	*/
							if (symbol > 0 && symbol < (signed)(Mext[cx].K))
							{
								/* do the arithmetic coding	*/
								//Encode_core( &Mext[cx], acSt, symbol, bitbuf);
								encode_AC( &Mext[cx], acSt, symbol, bitbuf);
								/* Update Model */
								update_model( Mext[cx].H, Mext[cx].K, symbol);
							}
							else
							{	/* encode zero as symbol for exception handling	*/
								stop = 0;
								//Encode_core( &Mext[cx], acSt, 0, bitbuf);
								encode_AC( &Mext[cx], acSt, 0, bitbuf);
								update_model( Mext[cx].H, Mext[cx].K, 0);
							}
						} while( !stop);
					}
				} 
				/* update weights for abs_err prediction	*/
				update_weights_e( p_predE, px_ptr[pos], abs(eptr[pos]), /* p_templE,*/
					im->width >> 2);
			} /* if !constant_row	*/
			else
			{
				/* everything had be done during prediction step	*/
				cx = 0;
			}

			/**************************************************************
			* update processes	
			*/


			/* increment counter of actual prediction context
			* in decoder this is done inside update_meta_prediction()*/
			count_px[px_ptr[pos]]++;	

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


			/* regulary re-scaling 	*/
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
		if (outx != NULL) fprintf( outx, "\n");
		if (outc != NULL) fprintf( outc, "\n");
		fprintf( stderr, "\r%d%%", 100 * (r+1) / im->height);
	}	/* all rows	*/

	finish_encode( acSt, bitbuf);

#ifdef STAND_ALONE_COBALP
	/*
	* Initialisation of proper reading method (1/2 byte)
	*/
	if (im->max_value > 255)
	{
		// put_pgm_value = put_pgm_value_2;
		put_pgm_value = put_pgm_value_asc;
	}
	else
	{
		put_pgm_value = put_pgm_value_1;
	}
	/* output of prediction error	*/
	if (outr != NULL) 
	{
		for (r = 0, py = 0; r < (signed)im->height; 
			r++, py+=im->width)	/* all rows	*/
		{
			for (c = 0, pos = py; c < (signed)im->width; 
				c++, pos++)	/* all columns	*/
			{
				index = MODULO_RANGE( eptr[pos], -RANGEd2, RANGEm1d2, RANGE)
					index += RANGEd2;
				put_pgm_value( index, outr);
			}
			if (im->max_value > 255) fprintf( outr, "\n"); /* ASCII mode	*/
		}
		fclose( outr);
		outr = NULL;
	}

	if (coding_verbose && 0)
	{
		int ii;
		FILE *out=NULL;

		out = fopen( "distribu.txt", "wt");
		fprintf( out, "# each col = one context");
		for (ii = 1; ii < RANGE; ii++)
		{
			fprintf( out, "\n%4d", ii -RANGEd2);
			for (k = 0; k < used_AC_contexts; k++)
			{
				int idx;
				idx = ii - RANGEd2 + M[k].K/2;
				if ( idx >= 0 && idx < (signed)M[k].K)
				{
					double val;

					val = (double)(M[k].H[idx+1] - M[k].H[idx]) / M[k].H[ M[k].K];
					if (val > 1)
					{
						val = val;
					}
					fprintf( out, " %5.3f", val);
				}
				else fprintf( out, "      ");
			}
		}
		fprintf( out, "\n");
		fclose( out);
	}

	if (coding_verbose || LOG_FLAG)
	{
		char	log_name[512];

		sprintf( log_name, "%s_log_CoBALP2_e.txt", basename);
		err =  verbose( log_name, im, imExt, lptr, p_meta, p_pred2,
			const_rows, row_val, const_cols, col_val, M, Mext, th_AC,
			num_contexts, used_AC_contexts, count_px, count_cx, count_cxESC, sign_count, rtn,
			(unsigned long)(ftell( out)+1), p_predE, H);

		if (err)
		{
			goto end;
		}
	}

	//printf( "  Bitrate: %.3f", 8.0 * (numBytes+1) / im->size);
	printf( "  Bitrate: %.3f", 8.0 * (ftell( out)+1) / im->size);
	printf( "  Size: %8d", ftell( out)+1 );
	/* plus 1, since Buffer is not closed yet	*/
	//fprintf( stderr, "\n  Bitrate: %.3f", 8.0 * (numBytes+1) / im->size);
	fprintf( stderr, "\n  Bitrate: %.3f", 8.0 * (ftell( out)+1) / im->size);
	fprintf( stderr,"\n  -----------------");
#endif

end:
	if (param->pred_mode & 0x4) clean_predictorTM( p_ls);
	FREE( H_err);
	FREE( eptr);
	FREE( eptrLS);
	FREE( px_ptr);
	FREE( pxLS_ptr);
	FREE( predMult);
	FREE(count_px);
	FREE(count_pxLS);
	FREE(sum_abserr);

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
