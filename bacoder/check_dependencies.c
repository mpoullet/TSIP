/*****************************************************
 * File..: check_dependencies.c
 * Desc..: checking of manual parameter-selection
 * Author: T. Strutz
 * Date..: 07.04.2014
 * changes:
 * 03.12.2014 use RCT for prediction in BPC mode
 * 13.01.2015 bugfix: -"- but not when indexed mod is activated
 * 29.07.2015 new flag: 	aC->rct_manual_flag  
 ******************************************************/
#include "autoConfig.h"
#include "imageFeatures.h"
#include "ginterface.h"
#include "colour4.h"

/*--------------------------------------------------------
 * check_dependencies()
 *-------------------------------------------------------*/
void check_dependencies( PicData *pd, ImageFeatures *iF, 
							 AutoConfig *aC)
{
	if (aC->coding_method != TSIP)
	{
		aC->interleaving_mode = Y_U_V; /* avoid change of MaxVal[] in codec_e.c	*/
		if (aC->coding_method == LOCO || aC->coding_method == CoBaLP2)  /* LOCO  */
		{
			aC->predMode = NONE;
		}
	}


	if (aC->rct_type_used == GREY) 
	{
		aC->indexed_flag = 0;
	}
	if (aC->coding_method == HXC2 || aC->coding_method == SCF)
	{
		aC->indexed_flag = 0;
		//if (aC->rct_type_used == INDEXED_GBR || aC->rct_type_used == INDEXED_LUM)
		//{
		//	aC->rct_type_pred = RGBc;
		//	aC->rct_type_nopr = RGBc;
		//	aC->rct_type_used = RGBc;
		//}
	}

	if (aC->indexed_flag)  /* Indexed colours  */
	{
		if (aC->palette_sorting) aC->rct_type_used = INDEXED_LUM;
		else aC->rct_type_used = INDEXED_GBR;

		/* if number of colours is too high, then prevent usage of indexed colours	*/
		if (aC->coding_method == CoBaLP2)
		{
			if (iF->numColours > 1024)
			{
				aC->rct_type_used = aC->rct_type_pred;
				aC->indexed_flag = 0;
			}
		}

		if (aC->coding_method == LOCO)
		{
			if (iF->numColours > 20000)
			{
				aC->rct_type_used = aC->rct_type_pred;
				aC->indexed_flag = 0;
				fprintf( stderr, "\n! Warning: Number of colours is too high   !!");
				fprintf( stderr, "\n! for indexed mode in combination with LOCO!!");
			}
		}
	}
	else
	{
		aC->indexed_adj = 0;	/* senseless without indexing mode	*/
		aC->indexed_arith = 0;
		if (pd->transparency == 1) pd->transparency = 2;
	}

	if (aC->indexed_adj == 0)
	{
		aC->indexed_arith = 0;/* only in combination with adjacency mapping	*/
	}

	if (aC->indexed_arith == 1)
	{
		aC->skip_precoding = 1; /* not used, when arithmetic coding is activated	*/
	}

	if (aC->histMode == PERMUTATION || aC->histMode == PERMUTE_2OPT)
	{
		if ( iF->numColours > 2048)
		{ /* avoid allocation problems */
			aC->histMode = COMPACTION;
			printf( "\n !###  Warning ###!");
			printf( "\n !###  too many colours for histogram permutation ###!\n"); 
			fprintf( stderr, "\n !###  Warning ###!");
			fprintf( stderr, "\n !###  too many colours for histogram permutation ###!\n");
		}
	}

	if (aC->predMode == ADAPT && 	pd->width * pd->height > 16000000)
	{
		aC->predMode = MED;
		printf( "\n !###  Warning ###!");
		printf( "\n !###  image is too large for ADAPT prediction ###!\n");
		fprintf( stderr, "\n !###  Warning ###!");
		fprintf( stderr, "\n !###  image is too large for ADAPT prediction ###!\n");
	}

		/* check parameter	*/
	/* Strutz: this is not allowed anymore, the tileWidth has to sty fixed here */
	//if (aC->coding_method == TSIP )
	//{
	//	//while (aC->segmentWidth * aC->segmentHeight > 1000000)
	//	while (aC->tileWidth * aC->tileWidth > 1000000)
	//	{
	//		// Strutz if (aC->interleaving_mode != Y_U_V) aC->interleaving_mode = Y_U_V;
	//		aC->tileWidth  = (aC->tileWidth  + 1) >> 1;
	//		//aC->segmentWidth  = (aC->segmentWidth  + 1) >> 1;
	//		//aC->segmentHeight = (aC->segmentHeight + 1) >> 1;
	//		printf( "\n !###  Warning ###!");
	//		printf( "\n !###  image is too large for fast BWT  ###!\n");
	//		fprintf( stderr, "\n !###  Warning ###!");
	//		fprintf( stderr, "\n !###  image is too large for fast BWT ###!\n");
	//	}
	//}

	if (aC->rct_type_used == RGBc && pd->channel == 1)
	{
		aC->rct_type_used = GREY;
	}


	if (aC->coding_method == BPC)
	{ /* use colour space for prediction as it works better
		 * if indexed mode is activated then keep it */
		if ( !aC->indexed_flag)
		{
			if (!aC->rct_manual_flag)
			{
				aC->rct_type_used = aC->rct_type_pred;
			} /* otherwise the rct was set in the GUI or via cli parameter	*/
		}
	}
}