/*****************************************************
* File...: write_header.c
* Desc...: writes the header info for *.tsip
* Authors: Tilo Strutz
* Data...: 27.03.2014
* 01.07.2014 new: operations for global header
* 04.07.2014 support of transparency
* 05.07.2014 writeGlobalHeader w/p gph structure
* 04.09.2014 transmit colour space and pred.mode also for HXC2
* 01.10.2014 derive index_sorting from RCT mode
* 23.01.2015 wA, wB
* 17.03.2015 differential coding of HXC2 tolerance
* 24.04.2015 bugfix HXC2_TOL_EXCEPTION
* 03.09.2015 no postBWT parameters in case of HXC3, but tolerances
* 21.09.2015 HXC2 pattern_size must be transmit
* 03.11.2015 new: SCF
 * 10.11.2015 new: SCF_directional
 * 30.11.2015 new: SCF_prediction, SCF_stage2tuning, SCF_colourMAP
 * 26.08.2016 new: SCF_maxNumPatterns(Idx) has to be transmitted
 * 15.08.2017 new 	LSpred_searchSpace;	LSpred_numOfPreds
******************************************************/
#include <stdlib.h>
#include "header.h"
#include "bitbuf.h"
#include "fibocode.h"
#include "colour4.h"
#include "tile.h" /* for CGlobalHeader structur etc.	*/
#include "hxc.h"	/* for MAX_PATTERN_SIZE	*/
#include "hxc3.h"	/* for PATTERN_SIZE3	*/

/*------------------------------------------------------------
 * write header info using  mit Fibonacci code
 *------------------------------------------------ */
void WriteHeader( PicHeader *ph, BitBuffer *bitbuf)
{	
	unsigned int chan, i;

	/* general parameters	*/
	WriteFiboToStream( bitbuf, ph->channel);
	WriteFiboToStream( bitbuf, ph->width);
	WriteFiboToStream( bitbuf, ph->height);	
	WriteFiboToStream( bitbuf, ph->transparency); /* */

	WriteFiboToStream( bitbuf, ph->bitperchan[0]);
	WriteFiboToStream( bitbuf, ph->maxVal_rct[0]);

	for (chan = 1; chan < ph->channel; chan++)
	{
		WriteFiboToStream( bitbuf, ph->bitperchan[chan]);
		//WriteFiboToStream( bitbuf, ph->maxVal_orig[chan]);	
		WriteFiboToStream( bitbuf, ph->maxVal_rct[chan]);	
	}
	WriteFiboToStream( bitbuf, ph->histMode);
	WriteFiboToStream( bitbuf, ph->coding_method);
	WriteFiboToStream( bitbuf, ph->colour_space);

	if (ph->coding_method == TSIP)
	{
		//WriteFiboToStream( bitbuf, ph->segmentWidth);		
		//WriteFiboToStream( bitbuf, ph->segmentHeight);		
		if (ph->colour_space == INDEXED_GBR || ph->colour_space == INDEXED_LUM)
		{
			WriteFiboToStream( bitbuf, ph->indexed_adj);
			WriteFiboToStream( bitbuf, ph->indexed_arith);
		}
		WriteFiboToStream( bitbuf, ph->predMode);
		if (ph->predMode == LMSPRED || ph->predMode == LSBLEND || ph->predMode == LSPRED)
		{
			WriteFiboToStream( bitbuf, ph->LSpred_searchSpace);
			WriteFiboToStream( bitbuf, ph->LSpred_numOfPreds);
		}
		if (ph->predMode == LSBLEND)
		{
			BufferPutBit( bitbuf, (cbool)ph->useCorrelation_flag);
		}
		if (ph->predMode == LSBLEND || ph->predMode == LMSPRED)
		{
			BufferPutBit( bitbuf, (cbool)ph->useColPrediction_flag);
			BufferPutBit( bitbuf, (cbool)ph->useColPredictionP_flag);
		}
		WriteFiboToStream( bitbuf, ph->postbwt);
		if (ph->postbwt != HXC3) 
		{
			WriteFiboToStream( bitbuf, ph->pbwtparam); /* for MTF and IFC	*/
			if (ph->postbwt == IFC)
			{
				WriteFiboToStream( bitbuf, ph->maxIFCCount);
			}
		}
		else
		{
			WriteFiboToStream( bitbuf, ph->HXC2tolerance[0][0]);
			for ( i = 1; i < PATTERN_SIZE3; i++)
			{
				if (ph->HXC2tolerance[0][i] < ph->HXC2tolerance[0][i-1])
				{
					/* exception	*/
					WriteFiboToStream( bitbuf, HXC2_TOL_EXCEPTION);
					WriteFiboToStream( bitbuf, ph->HXC2tolerance[0][i]);
				}
				else WriteFiboToStream( bitbuf, ph->HXC2tolerance[0][i] - ph->HXC2tolerance[0][i-1]);
			}

			if (ph->channel > 1)
			{
				WriteFiboToStream( bitbuf, ph->HXC2tolerance[1][0]);
				for ( i = 1; i < PATTERN_SIZE3; i++)
				{
					if (ph->HXC2tolerance[1][i] < ph->HXC2tolerance[1][i-1])
					{
						/* exception	*/
						WriteFiboToStream( bitbuf, HXC2_TOL_EXCEPTION);
						WriteFiboToStream( bitbuf, ph->HXC2tolerance[1][i]);
					}
					else WriteFiboToStream( bitbuf, ph->HXC2tolerance[1][i] - ph->HXC2tolerance[1][i-1]);
				}
			}
			if (ph->channel > 2)
			{
				WriteFiboToStream( bitbuf, ph->HXC2tolerance[2][0]);
				for ( i = 1; i < PATTERN_SIZE3; i++)
				{
					if (ph->HXC2tolerance[2][i] < ph->HXC2tolerance[2][i-1])
					{
						/* exception	*/
						WriteFiboToStream( bitbuf, HXC2_TOL_EXCEPTION);
						WriteFiboToStream( bitbuf, ph->HXC2tolerance[2][i]);
					}
					else WriteFiboToStream( bitbuf, ph->HXC2tolerance[2][i] - ph->HXC2tolerance[2][i-1]);
				}
			}
			if (ph->channel == 4)
			{
				WriteFiboToStream( bitbuf, ph->HXC2tolerance[3][0]);
				for ( i = 1; i < PATTERN_SIZE3; i++)
				{
					if (ph->HXC2tolerance[3][i] < ph->HXC2tolerance[3][i-1])
					{
						/* exception	*/
						WriteFiboToStream( bitbuf, HXC2_TOL_EXCEPTION);
						WriteFiboToStream( bitbuf, ph->HXC2tolerance[3][i]);
					}
					else WriteFiboToStream( bitbuf, ph->HXC2tolerance[3][i] - ph->HXC2tolerance[3][i-1]);
				}
			}
			WriteFiboToStream( bitbuf, ph->HXC2increaseFac);
		}
		WriteFiboToStream( bitbuf, ph->interleaving_mode);
		WriteFiboToStream( bitbuf, ph->entropy_coding_method);
		WriteFiboToStream( bitbuf, ph->separate_coding_flag);
		WriteFiboToStream( bitbuf, ph->skip_precoding);
		WriteFiboToStream( bitbuf, ph->skip_rlc1);
		WriteFiboToStream( bitbuf, ph->skip_postBWT);
		WriteFiboToStream( bitbuf, ph->use_RLC2zero);
	}
	else if (ph->coding_method == HXC)
	{
		WriteFiboToStream( bitbuf, ph->predMode);
		WriteFiboToStream( bitbuf, ph->HXCtolerance);
		WriteFiboToStream( bitbuf, ph->HXCtoleranceOffset1);
		WriteFiboToStream( bitbuf, ph->HXCtoleranceOffset2);
		WriteFiboToStream( bitbuf, ph->HXCincreaseFac);
	}

	else if (ph->coding_method == HXC2 || ph->coding_method == SCF)
	{
		WriteFiboToStream( bitbuf, ph->predMode);
		WriteFiboToStream( bitbuf, ph->pattern_size);

		if (ph->coding_method == SCF)
		{
			BufferPutBit( bitbuf, (cbool)ph->SCF_prediction);
			BufferPutBit( bitbuf, (cbool)ph->SCF_directional);
			BufferPutBit( bitbuf, (cbool)ph->SCF_stage2tuning);
			BufferPutBit( bitbuf, (cbool)ph->SCF_colourMAP);
			WriteFiboToStream( bitbuf, ph->SCF_maxNumPatternsIdx);
		}
		WriteFiboToStream( bitbuf, ph->HXC2tolerance[0][0]);
		for ( i = 1; i < MAX_PATTERN_SIZE; i++)
		{
			if (ph->HXC2tolerance[0][i] < ph->HXC2tolerance[0][i-1])
			{
				/* exception	*/
				WriteFiboToStream( bitbuf, HXC2_TOL_EXCEPTION);
				WriteFiboToStream( bitbuf, ph->HXC2tolerance[0][i]);
			}
			else WriteFiboToStream( bitbuf, ph->HXC2tolerance[0][i] - ph->HXC2tolerance[0][i-1]);
		}

		if (ph->channel > 1)
		{
			WriteFiboToStream( bitbuf, ph->HXC2tolerance[1][0]);
			for ( i = 1; i < MAX_PATTERN_SIZE; i++)
			{
				if (ph->HXC2tolerance[1][i] < ph->HXC2tolerance[1][i-1])
				{
					/* exception	*/
					WriteFiboToStream( bitbuf, HXC2_TOL_EXCEPTION);
					WriteFiboToStream( bitbuf, ph->HXC2tolerance[1][i]);
				}
				else WriteFiboToStream( bitbuf, ph->HXC2tolerance[1][i] - ph->HXC2tolerance[1][i-1]);
			}
		}
		if (ph->channel > 2)
		{
			WriteFiboToStream( bitbuf, ph->HXC2tolerance[2][0]);
			for ( i = 1; i < MAX_PATTERN_SIZE; i++)
			{
				if (ph->HXC2tolerance[2][i] < ph->HXC2tolerance[2][i-1])
				{
					/* exception	*/
					WriteFiboToStream( bitbuf, HXC2_TOL_EXCEPTION);
					WriteFiboToStream( bitbuf, ph->HXC2tolerance[2][i]);
				}
				else WriteFiboToStream( bitbuf, ph->HXC2tolerance[2][i] - ph->HXC2tolerance[2][i-1]);
			}
		}
		if (ph->channel == 4)
		{
			WriteFiboToStream( bitbuf, ph->HXC2tolerance[3][0]);
			for ( i = 1; i < MAX_PATTERN_SIZE; i++)
			{
				if (ph->HXC2tolerance[3][i] < ph->HXC2tolerance[3][i-1])
				{
					/* exception	*/
					WriteFiboToStream( bitbuf, HXC2_TOL_EXCEPTION);
					WriteFiboToStream( bitbuf, ph->HXC2tolerance[3][i]);
				}
				else WriteFiboToStream( bitbuf, ph->HXC2tolerance[3][i] - ph->HXC2tolerance[3][i-1]);
			}
		}
		WriteFiboToStream( bitbuf, ph->HXC2increaseFac);
	}
	else if (ph->coding_method == BPC)
	{
		WriteFiboToStream( bitbuf, ph->s_flg);
		WriteFiboToStream( bitbuf, ph->use_colFlag);
		WriteFiboToStream( bitbuf, ph->use_predFlag);
		WriteFiboToStream( bitbuf, ph->predMode);
		for (chan = 0; chan < ph->channel; chan++)
		{
			WriteFiboToStream( bitbuf, ph->offset[chan]);
		}
	}
	else if (ph->coding_method == CoBaLP2)
	{
		WriteFiboToStream( bitbuf, ph->wA);
		WriteFiboToStream( bitbuf, ph->wB);
	}
	if (ph->predMode != NONE)
	{
		/* write maxVals after prediction	*/
		for (chan = 0; chan < ph->channel; chan++)
		{
			WriteFiboToStream( bitbuf, ph->maxVal_pred[chan]);	
		}
	}
}


/* Headerstrkutur allokieren */
PicHeader *CreateHeader()
{
	PicHeader *ph = NULL;
	ALLOC( ph, 1u, PicHeader);	
	return ph;
}


/* ...und löschen */
void DeleteHeader(PicHeader *ph)
{		
	FREE(ph);
}

// Geändert von Team Darmstadt
/*------------------------------------------------------------
 * write header for global information 
 *------------------------------------------------ */
void writeGlobalHeader( PicData *pd,
												unsigned int numberOfTiles, 
												wchar_t *outFileName)
{	
	unsigned int chan;
	FILE *h_file = NULL;
	BitBuffer *bitbuf  = NULL;  

	/* open file for writing	*/
	h_file = _wfopen( outFileName, L"wb");
	if (h_file == NULL)
	{
		throw 14;
	}
	bitbuf = BufferOpenWrite( h_file);


	/* flag that a global header is used, 
	 * other wise this is the number of channels
	 */
	WriteFiboToStream( bitbuf, numberOfTiles);

	if (numberOfTiles > 1)
	{
		/* parameters for global pic */
		WriteFiboToStream( bitbuf, pd->channel);
		WriteFiboToStream( bitbuf, pd->width);
		WriteFiboToStream( bitbuf, pd->height);
		WriteFiboToStream( bitbuf, pd->bitperchannel);

		WriteFiboToStream( bitbuf, pd->bitperchan[0]);

		for (chan = 1; chan < pd->channel; chan++)
		{
			WriteFiboToStream( bitbuf, pd->bitperchan[chan]);
		}
	}
	BufferClose( bitbuf);

	fclose( h_file);
}


/*------------------------------------------------------------
 * CreateGlobalHeader() 
 *------------------------------------------------ */
GlobalPicHeader *CreateGlobalHeader()
{
	GlobalPicHeader *gph = NULL;
	ALLOC( gph, 1u, GlobalPicHeader); /*Speicher für den globalen Header reservieren.*/	
	return gph;
}

void DeleteGlobalHeader( GlobalPicHeader *gph)
{		
	FREE(gph); /*Freigeben des für den globalen Header reservierten Speichers. */
}
// Ende der Änderungen
