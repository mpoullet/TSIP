/*****************************************************
* File...: read_header.c
* Desc...: writes the header info for *.tsip
* Authors: Tilo Strutz
* Data...: 27.03.2014
* 01.07.2014 new: operations for global header
* 04.07.2014 support of transparency
* 04.09.2014 transmit colour space and pred.mode also for HXC2
* 01.10.2014 derive index_sorting from RCT mode
* 23.01.2015 wA, wB
* 17.03.2015 differential decoding of HXC2 tolerance
* 24.04.2015 bugfix HXC2_TOL_EXCEPTION
* 03.09.2015 no ostBWT parameters in case of HXC3 but tolerances
* 03.11.2015 new: SCF
 * 10.11.2015 new: SCF_directional
 * 30.11.2015 new: SCF_prediction, SCF_stage2tuning, SCF_colourMAP
 * 26.08.2016 new: SCF_maxNumPatterns(Idx) has to be transmitted
 * 01.01.2016 bugfixes: transparency, channel == 4
 * 15.08.2017 new 	LSpred_searchSpace;	LSpred_numOfPreds, useCorrelation_flag
******************************************************/
#include <stdlib.h>
#include "header.h"
#include "bitbuf.h"
#include "fibocode.h"
#include "colour4.h"
#include "tile.h" /* for GlobalHeader structur etc.	*/
#include "hxc.h"	/* for MAX_PATTERN_SIZE	*/
#include "hxc3.h"	/* for PATTERN_SIZE3	*/

/*------------------------------------------------------------
 * read header info using  mit Fibonacci code
 *------------------------------------------------ */
PicHeader *LoadHeader( BitBuffer *bitbuf)
{
	unsigned int chan, val, i;
	PicHeader *ph = NULL;

	ph = CreateHeader();

	/* general parameters	*/
	ph->channel 	  = GetFiboFromStream( bitbuf);	
	ph->width 		  = GetFiboFromStream( bitbuf);
	ph->height 		  = GetFiboFromStream( bitbuf);
	ph->transparency = GetFiboFromStream( bitbuf);
	//if (ph->transparency == 3)
	//{
	//	/* this colour is in principle always maxVal+1, isn't it?	*/
	//	ph->transparency_colour = GetFiboFromStream( bitbuf);
	//}

	ph->bitperchan[0] = GetFiboFromStream( bitbuf);	
	ph->maxVal_rct[0] = GetFiboFromStream( bitbuf);	

	for (chan = 1; chan < ph->channel; chan++)
	{
		ph->bitperchan[chan] = GetFiboFromStream( bitbuf);	
		//ph->maxVal_orig[chan] = GetFiboFromStream( bitbuf);	
		ph->maxVal_rct[chan] = GetFiboFromStream( bitbuf);	
	}
	for (chan = ph->channel; chan < 4; chan++)
	{	/* reset values for remaining possible channels	*/
		ph->bitperchan[chan] = 0;
		ph->maxVal_orig[chan] = 0;
		ph->maxVal_rct[chan] = 0;
		ph->maxVal_pred[chan] = 0;
	}
	//ph->histMode			= (HistModeType)GetFiboFromStream( bitbuf);
	ph->histMode			= GetFiboFromStream( bitbuf);
	//ph->coding_method  = (CompMode)GetFiboFromStream( bitbuf);	
	ph->coding_method  = GetFiboFromStream( bitbuf);	
	ph->colour_space     = GetFiboFromStream( bitbuf);	
	
	ph->palette_sorting = 0; /* dummy value	*/
	ph->indexed_adj = 0;		/* dummy value	*/
	ph->indexed_arith = 0;	/* dummy value	*/
	ph->predMode	    = NONE;		/* dummy value	*/

	if (ph->colour_space == INDEXED_GBR )
	{
		ph->palette_sorting  = 0;	
	}
	else if (ph->colour_space == INDEXED_LUM)
	{
		ph->palette_sorting  = 1;	
	}

	if (ph->coding_method == TSIP)
	{
		//ph->segmentWidth    = GetFiboFromStream( bitbuf);		
		//ph->segmentHeight   = GetFiboFromStream( bitbuf);		
		if (ph->colour_space == INDEXED_GBR || ph->colour_space == INDEXED_LUM)
		{
			ph->indexed_adj  = GetFiboFromStream( bitbuf);	
			ph->indexed_arith  = GetFiboFromStream( bitbuf);	
		}

		ph->predMode = (PredictionType)GetFiboFromStream( bitbuf);	
		if (ph->predMode == LMSPRED || ph->predMode == LSBLEND || ph->predMode == LSPRED)
		{
			ph->LSpred_searchSpace = (unsigned char)GetFiboFromStream( bitbuf);	
			ph->LSpred_numOfPreds  = (unsigned char)GetFiboFromStream( bitbuf);	
		}
		if (ph->predMode == LSBLEND )
		{
			ph->useCorrelation_flag = (unsigned char)BufferGetBit( bitbuf);
		}
		if (ph->predMode == LSBLEND || ph->predMode == LMSPRED)
		{
			ph->useColPrediction_flag = (unsigned char)BufferGetBit( bitbuf);
			ph->useColPredictionP_flag = (unsigned char)BufferGetBit( bitbuf);
		}
		ph->postbwt	= (PostBWT)GetFiboFromStream( bitbuf);	
		if (ph->postbwt != HXC3) 
		{
			ph->pbwtparam     = GetFiboFromStream( bitbuf);	
			if (ph->postbwt == IFC)
			{
				ph->maxIFCCount = GetFiboFromStream( bitbuf);	
			}
			else
			{
				ph->maxIFCCount = 0; /* dummy initialisation	*/
			}
		}
		else
		{
			ph->pbwtparam = 0; /* dummy initialisation	*/
			ph->maxIFCCount = 0; /* dummy initialisation	*/
			ph->HXC2tolerance[0][0] = GetFiboFromStream( bitbuf);
			for ( i = 1; i < PATTERN_SIZE3; i++)
			{
				val = GetFiboFromStream( bitbuf);
				if (val == HXC2_TOL_EXCEPTION) 
				{
					ph->HXC2tolerance[0][i] = GetFiboFromStream( bitbuf);
				}
				else ph->HXC2tolerance[0][i] = ph->HXC2tolerance[0][i-1] + val; 
			}

			if (ph->channel > 1)
			{
				ph->HXC2tolerance[1][0] = GetFiboFromStream( bitbuf);
				for ( i = 1; i < PATTERN_SIZE3; i++)
				{
					val = GetFiboFromStream( bitbuf);
					if (val == HXC2_TOL_EXCEPTION) 
					{
						ph->HXC2tolerance[1][i] = GetFiboFromStream( bitbuf);
					}
					else ph->HXC2tolerance[1][i] = ph->HXC2tolerance[1][i-1] + val; 
				}
			}
			if (ph->channel > 2)
			{
				ph->HXC2tolerance[2][0] = GetFiboFromStream( bitbuf);
				for ( i = 1; i < PATTERN_SIZE3; i++)
				{
					val = GetFiboFromStream( bitbuf);
					if (val == HXC2_TOL_EXCEPTION) 
					{
						ph->HXC2tolerance[2][i] = GetFiboFromStream( bitbuf);
					}
					else ph->HXC2tolerance[2][i] = ph->HXC2tolerance[2][i-1] + val; 
				}
			}
			if (ph->channel == 4)
			{
				ph->HXC2tolerance[3][0] = GetFiboFromStream( bitbuf);
				ph->HXC2tolerance[3][0] = GetFiboFromStream( bitbuf);
				for ( i = 1; i < PATTERN_SIZE3; i++)
				{
					val = GetFiboFromStream( bitbuf);
					if (val == HXC2_TOL_EXCEPTION) 
					{
						ph->HXC2tolerance[3][i] = GetFiboFromStream( bitbuf);
					}
					else ph->HXC2tolerance[3][i] = ph->HXC2tolerance[3][i-1] + val; 
				}
			}
			ph->HXC2increaseFac		= GetFiboFromStream( bitbuf);
		}

		ph->interleaving_mode	= (InterleavingType)GetFiboFromStream( bitbuf);	
		//ph->interleaving_mode	= GetFiboFromStream( bitbuf);	
		ph->entropy_coding_method = GetFiboFromStream( bitbuf);	
		ph->separate_coding_flag = GetFiboFromStream( bitbuf);	
		ph->skip_precoding	= GetFiboFromStream( bitbuf);	
		ph->skip_rlc1				= GetFiboFromStream( bitbuf);
		ph->skip_postBWT				= GetFiboFromStream( bitbuf);
		ph->use_RLC2zero		= GetFiboFromStream( bitbuf);
	}
	else if (ph->coding_method == HXC)
	{
		ph->predMode	    = (PredictionType)GetFiboFromStream( bitbuf);	
		ph->HXCtolerance		= GetFiboFromStream( bitbuf);
		ph->HXCtoleranceOffset1		= GetFiboFromStream( bitbuf);
		ph->HXCtoleranceOffset2		= GetFiboFromStream( bitbuf);
		ph->HXCincreaseFac		= GetFiboFromStream( bitbuf);
		ph->interleaving_mode = Y_U_V; /* dummy for safety reasons */
	}
	else if (ph->coding_method == HXC2 || ph->coding_method == SCF)
	{
		ph->predMode = (PredictionType)GetFiboFromStream( bitbuf);	
		ph->pattern_size = GetFiboFromStream( bitbuf);

		if (ph->coding_method == SCF)
		{
			ph->SCF_prediction = BufferGetBit( bitbuf);
			ph->SCF_directional = BufferGetBit( bitbuf);
			ph->SCF_stage2tuning = BufferGetBit( bitbuf);
			ph->SCF_colourMAP = BufferGetBit( bitbuf);
			ph->SCF_maxNumPatternsIdx = GetFiboFromStream( bitbuf);
		}

		ph->HXC2tolerance[0][0] = GetFiboFromStream( bitbuf);
		for ( i = 1; i < MAX_PATTERN_SIZE; i++)
		{
			val = GetFiboFromStream( bitbuf);
			if (val == HXC2_TOL_EXCEPTION) 
			{
				ph->HXC2tolerance[0][i] = GetFiboFromStream( bitbuf);
			}
			else ph->HXC2tolerance[0][i] = ph->HXC2tolerance[0][i-1] + val; 
		}

		if (ph->channel > 1)
		{
			ph->HXC2tolerance[1][0] = GetFiboFromStream( bitbuf);
			for ( i = 1; i < MAX_PATTERN_SIZE; i++)
			{
				val = GetFiboFromStream( bitbuf);
				if (val == HXC2_TOL_EXCEPTION) 
				{
					ph->HXC2tolerance[1][i] = GetFiboFromStream( bitbuf);
				}
				else ph->HXC2tolerance[1][i] = ph->HXC2tolerance[1][i-1] + val; 
			}
		}
		if (ph->channel > 2)
		{
			ph->HXC2tolerance[2][0] = GetFiboFromStream( bitbuf);
			for ( i = 1; i < MAX_PATTERN_SIZE; i++)
			{
				val = GetFiboFromStream( bitbuf);
				if (val == HXC2_TOL_EXCEPTION) 
				{
					ph->HXC2tolerance[2][i] = GetFiboFromStream( bitbuf);
				}
				else ph->HXC2tolerance[2][i] = ph->HXC2tolerance[2][i-1] + val; 
			}
		}
		if (ph->channel == 4)
		{
			ph->HXC2tolerance[3][0] = GetFiboFromStream( bitbuf);
			for ( i = 1; i < MAX_PATTERN_SIZE; i++)
			{
				val = GetFiboFromStream( bitbuf);
				if (val == HXC2_TOL_EXCEPTION) 
				{
					ph->HXC2tolerance[3][i] = GetFiboFromStream( bitbuf);
				}
				else ph->HXC2tolerance[3][i] = ph->HXC2tolerance[3][i-1] + val; 
			}
		}
		ph->HXC2increaseFac		= GetFiboFromStream( bitbuf);
	}
	else if (ph->coding_method == BPC)
	{
		ph->s_flg	   = (unsigned char)GetFiboFromStream( bitbuf);	
		ph->use_colFlag	 = (unsigned char)GetFiboFromStream( bitbuf);	
		ph->use_predFlag = (unsigned char)GetFiboFromStream( bitbuf);	
		ph->predMode = (PredictionType)GetFiboFromStream( bitbuf);	
		for (chan = 0; chan < ph->channel; chan++)
		{
			ph->offset[chan] = GetFiboFromStream( bitbuf);
		}
		ph->interleaving_mode = Y_U_V; /* dummy for safety reasons */
	}
	else if (ph->coding_method == CoBaLP2)
	{
		ph->wA  = (unsigned char)GetFiboFromStream( bitbuf);	
		ph->wB  = (unsigned char)GetFiboFromStream( bitbuf);	
	}
	if (ph->predMode != NONE)
	{
		/* read maxVals after prediction	*/
		for (chan = 0; chan < ph->channel; chan++)
		{
			ph->maxVal_pred[chan] = GetFiboFromStream( bitbuf);
		}
	}
	else	/* copy from RCT	*/
	{
		for (chan = 0; chan < ph->channel; chan++)
		{
			ph->maxVal_pred[chan] = ph->maxVal_rct[chan];
		}
	}

	return ph;
}

// Geändert von Team Darmstadt
/*------------------------------------------------------------
 * read header for global information 
 *------------------------------------------------ */
GlobalPicHeader *LoadGlobalHeader( BitBuffer *bitbuf)
{
	unsigned int chan;
	GlobalPicHeader *gph = NULL;

	gph = CreateGlobalHeader();
	gph->numberOfTiles  = GetFiboFromStream( bitbuf);	

	if (gph->numberOfTiles > 1)
	{
		/* general parameters for global pic	*/
		gph->channel 	  = GetFiboFromStream( bitbuf);	
		gph->width 		  = GetFiboFromStream( bitbuf);
		gph->height 		  = GetFiboFromStream( bitbuf);

		gph->bitperchannel = GetFiboFromStream( bitbuf);
		gph->bitperchan[0] = GetFiboFromStream( bitbuf);
		

		for (chan = 1; chan < gph->channel; chan++)
		{
			gph->bitperchan[chan] = GetFiboFromStream( bitbuf);	
			
		}
		for (chan = gph->channel; chan < 4; chan++)
		{
			gph->bitperchan[chan] = 0;
		}
	}
	return gph;
}
// Ende der Änderungen
