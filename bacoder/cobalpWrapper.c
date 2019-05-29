/*****************************************************************
*
* File....:	cobalbWrapper.c
* Purpose.:	interface to CoBaLP2 encoding and decoding machine
* Author..:	Tilo Strutz
* Date....:	10.09.2013
* 23.09.2013 hand over of entropies
 * 19.03.2014 compression of constant lines toggled by external option
* 23.01.2015 wA, wB
 * 27.07.2018 added num_contexts in structure + handover
 * 16.08.2018 added num_contextsLS in structure + handover
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "autoconfig.h"
#include "ginterface.h"
#include "bitbuf.h"
#include "stats.h" /* for INTERMEDIATE_VALS *iv	*/
#include "types_CoBaLP.h"
#include "codec_CoBaLP.h"

int encoder( IMAGE *im, long *lptr, BitBuffer *bitbuf, 
						PARAM_STRUC	*param, float wA, float wB, float *entropy);
int decoder( IMAGE *im, BitBuffer *bitbuf, long *lptr, PARAM_STRUC	*param,
						float wA, float wB);


/*---------------------------------------------------------------
*	cobalbWrapper()
*---------------------------------------------------------------*/
int cobalbWrapper( PicData *pd, BitBuffer *bitbuf, 
								 int encoder_flag, unsigned char sT, unsigned char sA, 
								 unsigned char sB, unsigned char aR, unsigned char aL, 
								 float wA, float wB, 
								 unsigned char *CoBaLP2_constLines,
								 unsigned int numOfConstantLines[],
								 INTERMEDIATE_VALS *iv)
{
	long *lptr = NULL;
	unsigned int comp, count;
	unsigned long pos;
	IMAGE *im, im_obj;
	PARAM_STRUC	*param, param_obj;

	im = &im_obj;
	param = &param_obj;

	if (encoder_flag)
	{
		param->compact_flag = 0; /* compaction is done outside	*/
		/* these parameters should be made available to the GUI	*/
		param->exhaustive_flag = 0;
		param->pred_mode = 7;
		param->paeth_mode = 1;
		param->constant_lines_mode = *CoBaLP2_constLines;
		param->rotate_flag = 0;

		if (sA) param->pred_mode &= ~0x01;
		if (sB) param->pred_mode &= ~0x02;
		if (sT) param->pred_mode &= ~0x04;
		if (aR) param->pred_mode |= 0x10;
		if (aL) param->pred_mode |= 0x08;

		//param->pred_mode |= 0x08; /* MED */
	}
	count = 0;
	im->width = pd->width;
	im->height = pd->height;
	im->size = pd->size;

	/* copy data into long array	*/
	ALLOC( lptr, pd->size, long);
	for (comp = 0; comp < pd->channel; comp++)
	{					
		if (encoder_flag)
		{
			for (pos = 0; pos < im->size; pos++)	/* all 	*/
			{
				lptr[pos] = pd->data[comp][pos];
			}

			im->bit_depth = pd->bitperchan[comp];
			im->max_value = pd->maxVal_rct[comp];
			im->min_value = 0; /* dummy value, will be determined in next routine	*/
			param->num_contexts = iv->num_contexts;
			param->num_contextsLS = iv->num_contextsLS;
			encoder( im, lptr, bitbuf, param, wA, wB, &(iv->entropy[comp]));
			iv->num_contexts = param->num_contexts;
			iv->num_contextsLS = param->num_contextsLS;

			/* count how often constant line mode is not used	*/
			numOfConstantLines[comp] = param->numOfConstantLines;
			if (param->constant_lines_mode == 0)	count++;
			param->constant_lines_mode = *CoBaLP2_constLines; 
			/* reset mode for next channel, otherwise it wont be checked, 
			 * whether constant lines are present
			 */
		}
		else
		{
			decoder( im, bitbuf, lptr, param, wA, wB);
			for (pos = 0; pos < im->size; pos++)	/* all 	*/
			{
				pd->data[comp][pos] = lptr[pos];
			}
			fprintf( stderr, "\n");
		}
	}
	/* if constant lines mode is not used in all components, then
	 * indicate the reset of the coding parameter (for logging)
	 */
	if (encoder_flag && *CoBaLP2_constLines)
	{
		if (count == pd->channel) *CoBaLP2_constLines = 3;
	}

	FREE( lptr);
	return 0;
}
