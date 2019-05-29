/*****************************************************
 * File..: autoConfig.c
 * Desc..: automatic parameter-selection and functions
 * Author: T. Strutz
 * Date..: 02.02.2012
 * 08.02.2012 new element: indexed_flag
 * 20.02.2012 some changes in parameters
 * 09.07.2012 if INDEXED then skip RLC1
 * 10.07.2012 if LOCO then use also T4, max Segment size decreased
 * 02.08.2013 aC->skip_rlc1 = 1; only if number of colours is high enough
 * 27.08.2013 type CompMode for ac->compression_mode
 * 27.09.2013 pred_flag in find_colourspace()
 * 08.11.2013 bugfix with presetting histMode in indexed Mode
 * 31.01.2014 new Parameters for HXC in aC->
 * 31.01.2014 new classification of compression mode
 * 31.01.2014 autoConfig() w/o return value
 * 04.02.2014 bugfix find_colourspace only fpr channel == 3
 * 10.02.2014 new HXCauto 
 * 11.05.2014 set aC->rct_type_used according to aC->pred_mode
 * 30.06.2014 bugfix: rct_used = pred if CoBalp or LOCO
 * 20.07.2014 special initioalisation of TSIP
 * 20.07.2014  use HXC or CoBaLP2 instead of TSIP, when modalPart too high
 * 20.10.2014  bugfix, if GREY, then disable aC->indexed_flag
 * 22.10.2014 bugfix, tilesize must be computed outside this function
 *						and only once
 * 02.12.2014 new flags: 	aC->use_colFlag,	aC->use_predFlag 
 * 11.03.2015 bug fix: in	compare_aC()  ac->HXC2tolerances[][]
 * 29.07.2015 aC->rct_manual_flag set 
 * 22.09.2015 adaptive aC->pattern_size
 * 08.10.2015 replacing the classification via vector k-means clusteruíng
 *						 by a decision tree
 * 16.10.2015 initialise HXC2tolerances with doubled max value
 * 10.11.2015 new: SCF_directional, SCF_stage2tuning, SCF_colourMAP
 * 21.01.2016 bugfix: processing of tileHeight included
 * 29.04.2016 new: coding_method_isPassed
 * 04.05.2016 decision on pattern_size based on colEntropy
 * 22.08.2016 compare_aC() updated, new: SCF_maxNumPatterns, RCTpercentage
 * 26.08.2016 new: SCF_maxNumPatternsIdx, mapIdx2maxNumPatterns()
 * 14.11.2016 include locoImproveFlag
 * 15.08.2017 new 	LSpred_searchSpace;	LSpred_numOfPreds
 * 07.09.2017 new useColPredictionP_flag
 ******************************************************/
//#include "stdafx.h"
#include "ConfigForm.h"
#include "stats.h"
#include "image.h"
#include "autoConfig.h"
#include "colour4.h"
#include "codec.h"	/* for max()	*/
#include "hxc.h"	/* for MAX_PATTERN_SIZE	*/
#include "leastSquaresPred.h"

/*--------------------------------------------------------
* autoConfig()
* Auto Parameter settings in Config dialog
*-------------------------------------------------------*/
void autoConfig( PicData *pd, ImageFeatures *iF, 
							 AutoConfig *aC, double entropy[], 
							 int coding_method_isPassed)
{
	int comp, i;
	double relDepth;
	IMAGEc image, *im;
	CompressionSettings compSet = setLOCO;

	im = &image;
	aC->log_name = NULL; /* initialisation	*/
	aC->rct_manual_flag = 0;

	/* default settings	*/
	aC->CoBaLP2_aR = 0;
	aC->CoBaLP2_aL = 0;
	aC->CoBaLP2_constLines = 1;
	aC->CoBaLP2_sA = 0;
	aC->CoBaLP2_sB = 0;
	aC->CoBaLP2_sT = 1;

	aC->LSpred_searchSpace = LS_SEARCHSPACE; /* defaults	*/
	aC->LSpred_numOfPreds = NUMOF_LSPREDS;
	aC->useCorrelation_flag = 1;
	aC->useColPrediction_flag = 1;
	aC->useColPredictionP_flag = 1;

	if (coding_method_isPassed == 0) aC->coding_method = LOCO;
	aC->entropy_coding_method = 1;	/* arithmetic	 for TSIP		*/
	aC->H26x_Set = 0; /*?? */
	aC->histMode = NoMOD;
	aC->HXC2auto = 0;
	aC->HXC2increaseFac = 1;
	for ( comp = 0; comp < 4; comp++)
	{
		for( i = 0; i < MAX_PATTERN_SIZE; i++)
		{
			aC->HXC2tolerance[comp][i] = 0;
		}
	}
	aC->SCF_directional = 1;
	aC->SCF_prediction = 1;
	aC->SCF_stage2tuning = 1;
	/* MAP in stage 2 of SCF is not successful if there are to less colours	*/
	// content_1176x400 has 128 colours 
	// feature-screen-rea_760x712 has also 128 colours and benefits from tuning
	if (iF->numColours < 128) 
	{
		aC->SCF_stage2tuning = 0;
	}
	else 
	{
		if (iF->numColours <= 256 && iF->patternMeasure < 9000) aC->SCF_stage2tuning = 0;
		else  aC->SCF_stage2tuning = 1;
	}
	aC->SCF_colourMAP = 1;
	aC->pattern_size = MAX_PATTERN_SIZE;
	aC->SCF_maxNumPatternsIdx = 8;
	/* speed up processing for large images	*/
	if (pd->size > 2500*2000)
		aC->SCF_maxNumPatternsIdx = 1;
	else if (pd->size > 2000*2000)
		aC->SCF_maxNumPatternsIdx = 2;
	else if (pd->size > 2000*1500)
		aC->SCF_maxNumPatternsIdx = 3;
	else if (pd->size > 1500*1500)
		aC->SCF_maxNumPatternsIdx = 4;
	else if (pd->size > 1500*1000)
		aC->SCF_maxNumPatternsIdx = 5;
	else if (pd->size > 1000*1000)
		aC->SCF_maxNumPatternsIdx = 6;
	else if (pd->size > 800*800)
		aC->SCF_maxNumPatternsIdx = 7;
//aC->SCF_maxNumPatternsIdx = 0; //#################!!!!! TEst test

	aC->SCF_maxNumPatterns = mapIdx2maxNumPatterns( aC->SCF_maxNumPatternsIdx);
	  // will be overwritten again in CLI.cpp  for some reason!!!!
	aC->HXCauto = 0;
	aC->HXCincreaseFac = 2;
	aC->HXCtolerance = 0;
	aC->HXCtoleranceOffset1 = 1;
	aC->HXCtoleranceOffset2 = 1;

	aC->IFCmaxCount = (uint)floor(50u + 350 * ( 1. -
								exp( -0.001 * sqrt( (double)pd->width*pd->height) ) ));	/* for TSIP	*/
	aC->IFCresetVal = aC->IFCmaxCount * 16;	/* for TSIP	*/
	aC->indexed_adj = 0;
	aC->indexed_arith = 0;
	aC->indexed_flag = 0;
	aC->interleaving_mode = YY_uvuv;	/* for TSIP	*/
	aC->MTF_val = 0;			/* for TSIP	*/
	/* aC->num_HXC_patterns = ; not a compression parameter	*/
	aC->palette_sorting = 0; /* Idx_GBR */
	aC->postbwt = IFC;		/* for TSIP	*/
	aC->predMode = NONE;

	/*
	 * colour-space selection
	 */
	aC->RCTpercentage = 100; /* check full image	*/
	if (pd->channel >= 3)
	{
		unsigned int Yi, UVj;
		unsigned long set_size;

		set_size = (pd->size * aC->RCTpercentage) / 100;
		/* determine best colour space based on entropy */
		im->height = pd->height;
		im->width = pd->width;
		im->size = pd->size;
		im->bit_depth[0] = pd->bitperchan[0];
		im->bit_depth[1] = pd->bitperchan[1];
		im->bit_depth[2] = pd->bitperchan[2];
		im->bit_depth_max = pd->bitperchannel;

#ifdef RCT_VARIANCE_CRITERION
		aC->rct_type_nopr = find_colourspace5( im, pd->data[0], 
				pd->data[1], pd->data[2], set_size, &Yi, &UVj,
				entropy,
				0, im->height,
				0, im->width, 0); /* without prediction	*/
		aC->rct_type_pred = find_colourspace5( im, pd->data[0], 
				pd->data[1], pd->data[2], set_size, &Yi, &UVj,
				entropy,
				0, im->height,
				0, im->width, 1); /* with prediction	*/
#else
		aC->rct_type_nopr = find_colourspace4( im, pd->data[0], 
				pd->data[1], pd->data[2], set_size, &Yi, &UVj,
				entropy,
				0, im->height,
				0, im->width, 0); /* without prediction	*/
		//fprintf( stderr, "without prediction ...         ");
		//fflush( stderr);
		aC->rct_type_pred = find_colourspace4( im, pd->data[0], 
				pd->data[1], pd->data[2], set_size, &Yi, &UVj,
				entropy,
				0, im->height,
				0, im->width, 1); /* with prediction	*/
#endif
	}
	else
	{
		aC->rct_type_nopr = aC->rct_type_pred = GREY; /* no colour */
		aC->SCF_colourMAP = 0; /* forecast of predictor not required here, saves soem time	*/
	}
	aC->rct_type_used = aC->rct_type_nopr; /* assume that no prediction is used	*/

	aC->s_flg	= 1; /* significance flag for BPC	*/
	aC->use_colFlag	= 0;	/*  flag for BPC, for some reason not beneficial	*/
	aC->use_predFlag = 1; /* flag for BPC	*/
	aC->separate_coding_flag = 1;	/* for TSIP	*/
	aC->skip_postBWT = 0;		/* for TSIP	*/
	aC->skip_precoding = 0;	/* for TSIP	*/
	aC->skip_rlc1 = 0;		/* for TSIP	*/
	aC->T4_flag = 1;			/* for LOCO	*/
	aC->locoImproveFlag = 1;/* for LOCO	*/
	aC->use_RLC2zero = 1;	/* for TSIP	*/


	/*
	 * decision tree classification
	 */
  relDepth = (double)iF->numColours / iF->maxDepth;
	if (aC->rct_type_nopr == GREY)
	{
		if ( iF->numColours < 5)
		{
			compSet = setBPC_COMP;
		}
		else if (iF->relColourContrast > 78)
		{
			if (iF->noise[0] >= 25)
			{
				compSet = setSCF_10_40_60_1;
			}
			else if (iF->syn < 90)
			{
				compSet = setSCF_10_40_60_1;
			}
			else
			{
				compSet = setSCF_zero_3;
			}
		}
		else if (iF->jointEntropy[0] > 8.0 )
		{
			compSet = setCoBaLP2;
		}
		else if (iF->corrcoeff[0] < 50)
		{
			compSet = setTSIP;
		}
		else if (iF->corrcoeff[0] >= 90)
		{
			if (iF->syn_grad < 2)
			{
				compSet = setSCF_0_20_220; /* Julia_3	*/
			}
			else
			{
				compSet = setSCF_auto;
			}
		}
		else if (iF->colModalPart < 146)
		{
			compSet = setSCF_10_40_60_1;
		}
		else if (iF->corrcoeff[0] >= 94 )
		{
			compSet = setCoBaLP2_TM;
		}
		else if (iF->noise[0] > 25 )
		{
			compSet = setCoBaLP2_TM;
		}
		else
		{
			compSet = setSCF_auto;
		}


		/* to Do */
		//if ( relDepth <= 0.6)
		//{
		//	if (iF->numColours < 8 )
		//		compSet = setBPC;
		//	{
		//		if ( relDepth > 0.46 &&
		//				 relDepth < 0.57)
		//		{
		//			compSet = setBPC;
		//		}
		//		else
		//		{
		//			//compSet = setHXC2_auto;
		//			compSet = setSCF_auto;
		//		}
		//	}
		//}
		//else
		//{
		//	if (iF->numColours < 256 )
		//	{
		//		//compSet = setHXC2_auto;
		//			compSet = setSCF_auto;
		//	}
		//	else
		//	{
		//		compSet = setCoBaLP2_TM;
		//	}
		//}
	}
	else /* not GREY	*/
	{
		if ( relDepth <= 0.6 )
		{
			if (iF->numColours < 8 && iF->colEntropy < 2.0)
			{
				compSet = setBPC_Idx;
			}
			else
			{
				//if (iF->syn + iF->syn_grad > 2*190) compSet = setHXC2_Tol0;
				//else compSet = setSCF_auto;
				compSet = setSCF_auto; /* seems to be always better */
			}
		}
		else
		{
			if ( relDepth > 50)
			{
				if (iF->maxDepth < 510)
				{
					compSet = setCoBaLP2_TM;
				}
				else
				{
					if( iF->relColourContrast == 200)
					{
						if (iF->colourContrast < 13)
						{
							compSet = setCoBaLP2_TM;
						}
						else
						{
							if (iF->colourContrast < 20)
							{
								compSet = setTSIP_ADAPT;
							}
							else
							{
								//compSet = setHXC2_auto;
								compSet = setSCF_auto;
							}
						}
					}
					else
					{
						compSet = setCoBaLP2_TM;
					}
				}
			}
			else
			{
				if (iF->syn + iF->syn_grad < 2*95)
				{
					if (iF->relColourSaturation == 200)
					{
						if (iF->constance > 80)
						{
							compSet = setBPC;
						}
						else
						{
							compSet = setTSIP_Idx_10_3;
						}
					}
					else
					{
						if (iF->numColours < 2000)
						{
							compSet = setTSIP_Idx_80_5;
						}
						else
						{
							compSet = setCoBaLP2_TM;
						}
					}
				}
				else
				{
					if (iF->numColours > 50000)
					{
						compSet = setCoBaLP2_TM;
					}
					else
					{
						if (iF->syn > 20 || iF->syn_grad < 175 )
						{
							//compSet = setHXC2_auto;
							compSet = setSCF_auto; /* seams mostly to be better than HXC2 by now	*/
						}
						else /* for Xcode_shortcut.ppm	*/
						{
							compSet = setHXC2_Tol0;
						}
					}
				}
			}
		}
	}

	if (coding_method_isPassed)
	{
		if (aC->coding_method == SCF)
		{
			if ( (compSet != setSCF_auto)       && (compSet != setSCF_0_20_220) && 
				   (compSet != setSCF_10_40_60_1) && (compSet != setSCF_zero_3) )
			{
				compSet = setSCF_auto;
			}
		}
		else if (aC->coding_method == CoBaLP2)
		{
			if ( (compSet != setCoBaLP2)    && (compSet != setCoBaLP2_cl) && 
				   (compSet != setCoBaLP2_TM) && (compSet != setCoBaLP2_Idx) && 
					 (compSet != setCoBaLP2_Idx_PERMUT) )
			{
				compSet = setCoBaLP2_TM;
			}
		}
		else if (aC->coding_method == LOCO)
		{
			compSet = setLOCO;
		}
	}

	switch (compSet)
	{
		case setBPC:
			aC->coding_method = BPC;	/* BitPlane coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 0;
			aC->histMode = NoMOD;
			break;
		case setBPC_COMP:
			aC->coding_method = BPC;	/* BitPlane coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			break;
		case setBPC_PAETH:
			aC->coding_method = BPC;	/* TSIP coding	*/
			aC->rct_type_used = aC->rct_type_pred;
			aC->predMode = PAETH;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			break;
		case setBPC_ADAPT:
			aC->coding_method = BPC;	/* TSIP coding	*/
			aC->rct_type_used = aC->rct_type_pred;
			aC->predMode = ADAPT;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			break;
		case setBPC_Idx:
			aC->coding_method = BPC;	/* BitPlane coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 1;
			/* aC->palette_sorting = 1; /* is set below */
			aC->histMode = NoMOD; /* always in indexed mode */
			break;
		case setBPC_Idx_PERMUT:
			aC->coding_method = BPC;	/* BitPlane coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 1;
			/* aC->palette_sorting = 1; /* is set below */
			aC->histMode = PERMUTATION;
			break;
		case setHXC:
			aC->coding_method = HXC;	/* HXC coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 0;
			/* aC->palette_sorting = 1; /* is set below */
			aC->histMode = COMPACTION;
			aC->HXCauto = 1;
			break;
		case setHXC_Idx:
			aC->coding_method = HXC;	/* HXC coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 1;
			aC->palette_sorting = 0;
			aC->histMode = NoMOD; /* always in indexed mode */
			aC->HXCauto = 1;
			break;
		case setHXC2_auto:
			aC->coding_method = HXC2;	/* HXC2 coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			aC->HXC2auto = 1;
			break;
		case setSCF_auto:
			aC->coding_method = SCF;	/* SCF coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
// XXXX	aC->rct_type_used = aC->rct_type_pred; /* test only */
			aC->predMode = NONE;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			aC->HXC2auto = 1;
			break;
			
		case setSCF_0_20_220:
			aC->coding_method = SCF;	/* SCF coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			aC->HXC2auto = 0;
			aC->HXC2tolerance[0][0] = 0;
			aC->HXC2tolerance[0][1] = 0;
			aC->HXC2tolerance[0][2] = 20;
			aC->HXC2tolerance[0][3] = 20;
			aC->HXC2tolerance[0][4] = 220;
			aC->HXC2tolerance[0][5] = 220;
			aC->HXC2increaseFac = 1;
			break;
		case setSCF_10_40_60_1:
			aC->coding_method = SCF;	/* SCF coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			aC->HXC2auto = 0;
			aC->HXC2tolerance[0][0] = 10;
			aC->HXC2tolerance[0][1] = 10;
			aC->HXC2tolerance[0][2] = 30;
			aC->HXC2tolerance[0][3] = 30;
			aC->HXC2tolerance[0][4] = 60;
			aC->HXC2tolerance[0][5] = 60;
			aC->HXC2increaseFac = 1;
			break;
		case setSCF_zero_3:
			aC->coding_method = SCF;	/* SCF coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			aC->HXC2auto = 0;
			aC->HXC2tolerance[0][0] = 0;
			aC->HXC2tolerance[0][1] = 0;
			aC->HXC2tolerance[0][2] = 0;
			aC->HXC2tolerance[0][3] = 0;
			aC->HXC2tolerance[0][5] = 0;
			aC->HXC2tolerance[0][6] = 0;
			aC->HXC2increaseFac = 4;
			break;
		case setHXC2_Tol0:
			aC->coding_method = HXC2;	/* HXC2 coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			aC->HXC2auto = 0;  /* keep tolerances equal to zero	*/
			{
				int correlation = 0;

				for ( comp = 0; comp < (signed)pd->channel; comp++)
				{
					fprintf( stderr, "\n %d: ", comp);
					correlation += iF->corrcoeff[comp];
				}
				correlation = (int)floor( (double)correlation / pd->channel);
				if (correlation < 50) aC->HXC2increaseFac = 0;
				else if (correlation < 90) aC->HXC2increaseFac = 1;
				else if (correlation < 95) aC->HXC2increaseFac = 2;
				else aC->HXC2increaseFac = 3;
				if (iF->colModalPart > 150) aC->HXC2increaseFac++;
				if (iF->syn_grad == 200)	aC->HXC2increaseFac++;
			}
			break;
		case setTSIP_Idx:
			aC->coding_method = TSIP;	/* TSIP coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 1;
			aC->histMode = NoMOD; /* always in indexed mode */
			break;
		case setTSIP_Idx_80_5:
			aC->coding_method = TSIP;	/* TSIP coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 1;
			aC->histMode = NoMOD; /* always in indexed mode */
			aC->IFCmaxCount = 5;
			aC->IFCresetVal = 80;
			aC->skip_rlc1 = 1;	
			break;
		case setTSIP_Idx_10_3:
			aC->coding_method = TSIP;	/* TSIP coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 1;
			aC->histMode = NoMOD; /* always in indexed mode */
			aC->IFCmaxCount = 3;
			aC->IFCresetVal = 10;
			aC->skip_rlc1 = 1;	
			break;
		case setTSIP_Idx_10_3_skip0:
			aC->coding_method = TSIP;	/* TSIP coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 1;
			aC->histMode = NoMOD; /* always in indexed mode */
			aC->IFCmaxCount = 3;
			aC->IFCresetVal = 10;
			aC->skip_rlc1 = 0;	
			break;
			
		case setTSIP_10_3_skip0:
			aC->coding_method = TSIP;	/* TSIP coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			aC->IFCmaxCount = 3;
			aC->IFCresetVal = 10;
			aC->skip_rlc1 = 0;	
			break;
		case setTSIP:
			aC->coding_method = TSIP;	/* TSIP coding	*/
			aC->rct_type_used = aC->rct_type_nopr;
			aC->predMode = NONE;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			break;
		case setTSIP_ADAPT:
			aC->coding_method = TSIP;	/* TSIP coding	*/
			aC->rct_type_used = aC->rct_type_pred;
			aC->predMode = ADAPT;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			break;
		case setTSIP_MEDPAETH:
			aC->coding_method = TSIP;	/* TSIP coding	*/
			aC->rct_type_used = aC->rct_type_pred;
			aC->predMode = MEDPAETH;
			aC->indexed_flag = 0;
			aC->histMode = NoMOD;
			aC->skip_rlc1 = 1;		/* for TSIP	*/
			break;
		case setLOCO:
			aC->coding_method = LOCO;	/* LOCO coding	*/
			aC->rct_type_used = aC->rct_type_pred;
			aC->indexed_flag = 0;
			// aC->histMode = COMPACTION;
			aC->T4_flag = 0;
			break;
		case setLOCO_T4:
			aC->coding_method = LOCO;	/* LOCO coding	*/
			aC->rct_type_used = aC->rct_type_pred;
			aC->indexed_flag = 0;
			// aC->histMode = COMPACTION;
			aC->T4_flag = 1;
			break;
		case setCoBaLP2:
			aC->coding_method = CoBaLP2;	/* CoBaLP2 coding	*/
			aC->rct_type_used = aC->rct_type_pred;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			aC->CoBaLP2_constLines = 0;
			aC->CoBaLP2_sT = 1;	/* TM off	*/ 
			break;
		case setCoBaLP2_cl:
			aC->coding_method = CoBaLP2;	/* CoBaLP2 coding	*/
			aC->rct_type_used = aC->rct_type_pred;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			aC->CoBaLP2_constLines = 1;
			break;
		case setCoBaLP2_TM:
			aC->coding_method = CoBaLP2;	/* CoBaLP2 coding	*/
			aC->rct_type_used = aC->rct_type_pred;
			aC->indexed_flag = 0;
			aC->histMode = COMPACTION;
			aC->CoBaLP2_constLines = 1;
			aC->CoBaLP2_sT = 0;	/* TM on	*/ 
			break;
		case setCoBaLP2_Idx:
			aC->coding_method = CoBaLP2;	/* CoBaLP2 coding	*/
			aC->rct_type_used = aC->rct_type_pred;
			aC->indexed_flag = 1;
			aC->histMode = NoMOD; /* always in indexed mode */
			aC->CoBaLP2_constLines = 1;
			aC->CoBaLP2_sT = 0;	/* TM on	*/ 
			break;
		case setCoBaLP2_Idx_PERMUT:
			aC->coding_method = CoBaLP2;	/* CoBaLP2 coding	*/
			aC->rct_type_used = aC->rct_type_pred;
			aC->indexed_flag = 1;
			aC->histMode = PERMUTATION;
			aC->CoBaLP2_constLines = 1;
			aC->CoBaLP2_sT = 0;	/* TM on	*/ 
			break;
	}

	/* grey scale images should not use indexed colours	*/
	if (aC->rct_type_used == GREY) aC->indexed_flag = 0;
	else
	{
		if ( (aC->coding_method == HXC2 || aC->coding_method == SCF) && 
			    iF->numColours < 64)
		{
			aC->rct_type_used = RGBc;
			aC->rct_type_nopr = RGBc;
			aC->rct_type_pred = RGBc;
		}
	}


	/* use better palette sorting
	 * for indexed HXC, idx_GRB is always better	( already set above)
	 */
	if (aC->indexed_flag == 1 && compSet != setHXC_Idx )
	{
		if (iF->numColours < PALETTE_THRESHOLD)
		{
			aC->palette_sorting = 0; /* GRB sorting */
		}
		else
		{
			aC->palette_sorting = 1; /* luminance based	as default */
		}
	}

	/* get tolerances for RGB components	*/
	for ( comp = 0; comp < (signed)pd->channel; comp++)
	{
		pd->maxVal_pred[comp] = pd->maxVal_orig[comp];
	}


	/* reduce size of pattern for images with many colours
	 * should speed up the search	
	 */
	//if( iF->numColours > 150000)
	//{
	//	aC->pattern_size = 2;
	//}
	//else 	if( iF->numColours > 30000)
	//{
	//	aC->pattern_size = 4;
	//}
	//else 	
	//{
	//	aC->pattern_size = 6;
	//}
	if( iF->colEntropy > 13.0)
	{
		aC->pattern_size = 2;
	}
	else 	if( iF->colEntropy > 8.0)
	{ /* Bildschirmfoto-20_1280x800, NYT_Screen_Capture_984x936, Metria */
		aC->pattern_size = 4;
	}
	else 	
	{
		aC->pattern_size = 6;
	}
	for (comp = 0; comp < (signed)pd->channel; comp++)
	{
		for ( i = aC->pattern_size; i < MAX_PATTERN_SIZE; i++)
		{
			aC->HXC2tolerance[comp][i] = pd->maxVal_orig[comp] * 2; 
			/* factor 2: take possible RCT into account*/
		}
	}

	/* check increase factor	*/
	{
		double sum;
		unsigned long count;
		sum = 0;
		count = 0;
		for (comp = 0; comp < (signed)pd->channel; comp++)
		{
			for ( i = 0; i < (signed)aC->pattern_size; i++)
			{
				sum += aC->HXC2tolerance[comp][i];
				count++;
			}
		}
		sum /= count;
		if (sum < 0.4)	aC->HXC2increaseFac = 3;
		else if (sum < 0.8)	aC->HXC2increaseFac = 2;
		else 	aC->HXC2increaseFac = 1;
	}

	if (iF->numColours > 30000)
	{ /* too many colours for indexing	*/
		aC->indexed_flag = 0;
	}


	if (aC->coding_method == HXC2)
	{
		aC->indexed_flag = 0;
		if (aC->rct_type_used == INDEXED_GBR || 
				aC->rct_type_used == INDEXED_LUM)
		{
			aC->rct_type_pred = RGBc;
			aC->rct_type_nopr = RGBc;
			aC->rct_type_used = RGBc;
		}
	}

	if (aC->coding_method != HXC2 && aC->coding_method != SCF)
	{
		aC->HXC2auto = 0;	/* prevent change of settings */
	}
	if (aC->coding_method != HXC)
	{
		aC->HXCauto = 0;	/* prevent change of settings */
	}
//fprintf( stderr, "\n autoConfig() ready ");
	return;
} /* AUTO parameter settings */

/*--------------------------------------------------------
* compare_aC()
* ==()
* Overloads the compare operator of two AutoConfig structc
*-------------------------------------------------------*/
//bool operator== (AutoConfig aC1, AutoConfig aC2)
int compare_aC( AutoConfig *aC1, AutoConfig *aC2)
{
	/* single comparisons enable debugging	*/
	if ((int)aC1->postbwt != (int)aC2->postbwt) return 1;
	if (aC1->MTF_val != aC2->MTF_val) return 1;
	if (aC1->IFCresetVal != aC2->IFCresetVal) return 1;
	if (aC1->IFCmaxCount != aC2->IFCmaxCount ) return 1;
	if (aC1->rct_type_used != aC2->rct_type_used) return 1;
	if (aC1->RCTpercentage != aC2->RCTpercentage) return 1;
			// Strutz for some reason this comparison is erroneous
	if (aC1->indexed_flag != aC2->indexed_flag) return 1;
	if (aC1->indexed_adj != aC2->indexed_adj) return 1;
	if (aC1->indexed_arith != aC2->indexed_arith) return 1;
	if (aC1->palette_sorting != aC2->palette_sorting) return 1;
	if (aC1->skip_precoding != aC2->skip_precoding) return 1;
	if (aC1->skip_postBWT != aC2->skip_postBWT) return 1;
	if (aC1->skip_rlc1 != aC2->skip_rlc1) return 1;
	if (aC1->use_RLC2zero != aC2->use_RLC2zero) return 1;
	if (aC1->tileWidth != aC2->tileWidth) return 1;
	if (aC1->tileHeight != aC2->tileHeight) return 1;
	//if (aC1->segmentHeight != aC2->segmentHeight) return 1;
	//if (aC1->segmentWidth != aC2->segmentWidth) return 1;
	if (aC1->entropy_coding_method != aC2->entropy_coding_method) return 1;
	if (aC1->separate_coding_flag != aC2->separate_coding_flag) return 1;
	if ((int)aC1->coding_method != (int)aC2->coding_method) return 1;
	if ((int)aC1->interleaving_mode != (int)aC2->interleaving_mode) return 1;
	if (aC1->T4_flag != aC2->T4_flag) return 1;
	if ((int)aC1->predMode != (int)aC2->predMode) return 1;
	if ((int)aC1->histMode != (int)aC2->histMode) return 1;
	if (aC1->HXCauto != aC2->HXCauto)
	{
		return 1;
	}
	else
	{
		if (!aC1->HXCauto)
		{
			/* these values might be automatically set, when HXCauto is active
			 * that is why they should checked only if HXCauto is false
			 */
			if (aC1->HXCtolerance != aC2->HXCtolerance) return 1;
			if (aC1->HXCtoleranceOffset1 != aC2->HXCtoleranceOffset1) return 1;
			if (aC1->HXCtoleranceOffset2 != aC2->HXCtoleranceOffset2) return 1;
		}
	}
	if (aC1->HXC2auto != aC2->HXC2auto)
	{
		return 1;
	}
	else
	{
		if (!aC1->HXC2auto)
		{
			int i, comp, flag = 0;
			/* these values might be automatically set, when HXC2auto is active
			 * that is why they should checked only if HXC2auto is false
			 */
			for ( comp = 0; comp < 4; comp++)
			{
				for ( i = 0; i < MAX_PATTERN_SIZE; i++)
				{
					if (aC1->HXC2tolerance[comp][i] != aC2->HXC2tolerance[comp][i]) 
					{
						flag = 1; 
						break;
					}
				}
			}
			if (flag) return 1;
		}
	}
	if (aC1->HXCincreaseFac != aC2->HXCincreaseFac) return 1;
	if (aC1->HXC2increaseFac != aC2->HXC2increaseFac) return 1;

	if (aC1->SCF_colourMAP != aC2->SCF_colourMAP) return 1;
	if (aC1->SCF_prediction != aC2->SCF_prediction) return 1;
	if (aC1->SCF_directional != aC2->SCF_directional) return 1;
	if (aC1->SCF_stage2tuning != aC2->SCF_stage2tuning) return 1;
	if (aC1->SCF_maxNumPatterns != aC2->SCF_maxNumPatterns) return 1;

	if (aC1->CoBaLP2_sT != aC2->CoBaLP2_sT) return 1;
	if (aC1->CoBaLP2_sA != aC2->CoBaLP2_sA) return 1;
	if (aC1->CoBaLP2_sB != aC2->CoBaLP2_sB) return 1;
	if (aC1->CoBaLP2_aR != aC2->CoBaLP2_aR)	return 1;
	if (aC1->CoBaLP2_aL != aC2->CoBaLP2_aL)	return 1;
	if (aC1->CoBaLP2_constLines != aC2->CoBaLP2_constLines)	return 1;
	
	if (aC1->s_flg != aC2->s_flg)	return 1;
	return 0;
}
/*--------------------------------------------------------
* !=()
* Overloads the compare operator of two AutoConfig structc
*-------------------------------------------------------*/
//bool operator!= (AutoConfig aC1, AutoConfig aC2)
//{
//	return !( aC1 == aC2);
//}

/*--------------------------------------------------------
* mapIdx2maxNumPatterns()
* 
*-------------------------------------------------------*/
unsigned int mapIdx2maxNumPatterns( unsigned int Idx)
{
		switch (Idx)
		{
			case 0: return 10;
			case 1: return 50;
			case 2: return 100; break;
			case 3: return 200; break;
			case 4: return 400; break;
			case 5: return 800; break;
			case 6: return 1600; break;
			case 7: return 3200; break;
			case 8: 
			default: return 999999;
		}

}
