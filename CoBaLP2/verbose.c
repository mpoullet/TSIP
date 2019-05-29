/*****************************************************************
 *
 * File..:	contexts.c
 * Descr.:	Contexts for linear prediction based on VQ (LBG algorithm)
 * Author.:	Tilo Strutz
 * Date..: 	09.04.2013
 * changes:
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
#include "meta_predictor.h"
#include "contexts.h"

/* global arrays from contexts.c	*/
extern double kn[MAXNUM_OF_PRED_CONTEXTS][NUM_KNOT_ELEM];	/* defined in contexts.c	*/
extern long ew[MAXNUM_OF_PRED_CONTEXTS][11]; /* 10 neighbours, 1 standarddeviation of context	*/

/*---------------------------------------------------------------
 *	verbose()
 *---------------------------------------------------------------*/
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
					double H)
{
	FILE *outlog = NULL;
	int err = 0;
	unsigned short k;
	unsigned int i, r, c;

	if ( (outlog = fopen( log_name, "wt")) == NULL) 
	{
		fprintf(stderr, "\n %s: Could not open %s\n", rtn, log_name);
		perror(" ");
		err = 1;
		goto end;
	}

	fprintf( outlog, "\n constant rows");
	for (r = 0; r < (signed)im->height; r++)
	{
		if (const_rows[r])
		{
			fprintf( outlog, "\n r=%4d, flag: %d", r, const_rows[r]);
			if (row_val !=NULL) fprintf( outlog, ", val = %3d", row_val[r]);
			else fprintf( outlog, ", val = %3d", lptr[OFF_LEFT + (OFF_TOP + r) * imExt->width]);
		}
	}
	fprintf( outlog, "\n constant columns");
	for (c = 0; c < (signed)im->width; c++)
	{
		if (const_cols[c])
		{
			fprintf( outlog, "\n c=%4d, flag: %d", c, const_cols[c]);
			if (col_val != NULL) fprintf( outlog, ", val = %3d", col_val[c]);
			else fprintf( outlog, ", val = %3d", lptr[OFF_LEFT + c + (OFF_TOP ) * imExt->width]);
		}
	}
	fprintf( outlog,"\n  Entropy: %.3f", H);
	fprintf( outlog,"\n  -----------------");
	
	fprintf( outlog, "\n AC contexts");
	for ( k = 0; k < used_AC_contexts; k++)
	{
		fprintf( outlog, "\n%3d, Thresh:%4d", k, th_AC[k]);
		fprintf( outlog, ", Range:%4d", (&M[k])->K);
		fprintf( outlog, ", %8d times", count_cx[k]);
		fprintf( outlog, ",\t Range:%4d:", (&Mext[k])->K);
		fprintf( outlog, ", %8d times ESC", count_cxESC[k]);
	}

	//fprintf( outlog, "\n cxs  count2   sign_count2  quote");
	//for ( k = 0; k < 1<<12; k++)
	//{
	//	fprintf( outlog, "\n%3d, %5d times, sign_count2: %+4d, %3.0f%%", 
	//		k, cxs_count2[k], sign_count2[k], 100.* abs( sign_count2[k]) / cxs_count2[k]);
	//}

	fprintf( outlog, "\n\n weights for error-magnitude prediction");
	fprintf( outlog, "\ncxr   times weights   flag");
	//for ( k = 0; k < NUM_PX_CONTEXTS; k++)
	for ( k = 0; k < num_contexts; k++)
	{
		int j;
		fprintf( outlog, "\n%3d:", k );
		fprintf( outlog, " %6d", p_predE->cnt[k]);
		for ( j = 0; j < NUM_ESTIMATORS; j++)
		{
			fprintf( outlog, " %7.3f", p_predE->weights_e[k][j]);
		}
		fprintf( outlog, " %d", p_predE->weight_flag[k]);
	}

	fprintf( outlog, "\n\n contexts");
	for ( k = 0; k < num_contexts; k++)
	{
		int j;
		fprintf( outlog, "\n%3d:", k );
		for ( j = 0; j < NUM_KNOT_ELEM; j++)
		{
			fprintf( outlog, " %3.0f", kn[k][j]);
		}
//			fprintf( outlog, ", TM: %d", kn_TMflag[k]);
	}
	fprintf( outlog, "\n\nmean prediction erros A-B-R-TM-MED, use of CX");
	fprintf( outlog, ", abs prediction erros");
	fprintf( outlog, ", abs erros x count");
	for ( k = 0; k < num_contexts; k++)
	{
		if (count_px[k] > NUM_SKIPPED_STEPS)
		{
			fprintf( outlog, "\n %2d %12.9f", k, 
				p_meta->sum_err_a[k] / (count_px[k]-NUM_SKIPPED_STEPS) );
			fprintf( outlog, " %15.9f", 
				p_meta->sum_err_b[k] / (count_px[k]-NUM_SKIPPED_STEPS) );
			fprintf( outlog, " %15.9f", 
				p_meta->sum_err_R[k] / (count_px[k]-NUM_SKIPPED_STEPS) );
			fprintf( outlog, " %15.9f", 
				p_meta->sum_err_T[k] / (count_px[k]-NUM_SKIPPED_STEPS) );

			fprintf( outlog, ", %6d times, %12.6f", count_px[k],
				p_meta->sum_abserr_a[k] / (count_px[k]-NUM_SKIPPED_STEPS) );
			fprintf( outlog, ", %12.6f",
				p_meta->sum_abserr_b[k] / (count_px[k]-NUM_SKIPPED_STEPS));
			fprintf( outlog, ", %12.6f",
				p_meta->sum_abserr_R[k] / (count_px[k]-NUM_SKIPPED_STEPS));
			fprintf( outlog, "\t, %12.0f",
				1. * p_meta->sum_abserr[k] / (count_px[k]-NUM_SKIPPED_STEPS) * count_px[k]);
		}
		else
		{
			fprintf( outlog, "\n %2d %12.9f", k, 
				p_meta->sum_err_a[k] );
			fprintf( outlog, " %12.9f, %d times",
				p_meta->sum_err_b[k] , count_px[k]);
			fprintf( outlog, " %12.9f, %d times",
				p_meta->sum_err_R[k] , count_px[k]);
		}
	}
	fprintf( outlog, "\n total: %12.9f", p_meta->mean_tot / im->size);

	fprintf( outlog, "\ncoefficients at end:    meta weighting A-B-R-TM-MED");
	for ( k = 0; k < num_contexts; k++)
	{
		fprintf( outlog, "\n context: %3d, %d times", k, count_px[k]);
		for ( i = 0; i < NUM_OF_PRED; i++)
		{
			fprintf( outlog, "\n   %2d\t %f\t %f", i, 
				p_pred2->weights_a[k][i], p_pred2->weights_b[k][i]);
			if (i < TEMPLATE_SIZE)
				fprintf( outlog, "\t %f", p_pred2->weights_R[k][i]);
			else
				fprintf( outlog, "\t \t");
		}
		fprintf( outlog, "| \t %f\t %f\t %f\t %f", 
			p_meta->wwa[k], p_meta->wwb[k], p_meta->wwR[k], p_meta->wwTM[k]);
		fprintf( outlog, "|sign \t %4d", sign_count[k]);
	}

	{
		int j;

		fprintf( outlog, "\n# each col = one context");
		for (j = 1; j < (signed)im->max_value; j++)
		{
			fprintf( outlog, "\n%4d", j -im->max_value/2);
			for (k = 0; k < used_AC_contexts; k++)
			{
				int idx;
				idx = j - im->max_value/2 + M[k].K/2;
				if ( idx >= 0 && idx < (signed)M[k].K)
				{
					double val;

					val = (double)(M[k].H[idx+1] - M[k].H[idx]) / M[k].H[ M[k].K];
					if (val > 1)
					{
						val = val;
					}
					fprintf( outlog, " %5.3f", val);
				}
				else fprintf( outlog, "      ");
			}
		}
		fprintf( outlog, "\n");
	}
	fprintf( outlog, "\n  Bitrate: %.3f", 8.0 * file_size / im->size);
	fprintf( outlog, "\n  Size: %15d", file_size);



	fprintf( outlog, "\n\n# context count A-C C-B B-D   wa(A-C) wa(C-B) wa(B-D) wb(A-C) wb(C-B) wb(B-D)");
	for ( k = 0; k < num_contexts; k++)
	{
		fprintf( outlog, "\n  %3d %6d ", k, count_px[k]);
		for ( i = 0; i < 3; i++)
		{
			fprintf( outlog, " %3.0f", kn[k][i]);
		}
		for ( i = 0; i < 3; i++)
		{
			fprintf( outlog, " %6.3f", p_pred2->weights_a[k][i]);
		}
		for ( i = 0; i < 3; i++)
		{
			fprintf( outlog, " %6.3f", p_pred2->weights_b[k][i]);
		}
	}


	fclose( outlog);

end:
	return err;
}
