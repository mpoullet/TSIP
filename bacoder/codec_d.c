/*****************************************************
* Datei: codec_d.c
* Desc.: implementation of decoder 
* Autor: Thomas Schmiedel
* Datum: 12.11.2010, 19.05.2011 (Strutz)
* 27.05.2011 Strutz: new colour transforms
* 23.01.2012 PicHeader:separate reset_val and maxCount
* 03.02.2012 ph->skip_precoding
* 10.07.2012 write/read_palette_2() new version
* 01.08.2012 IFCdecode instead of WFCDecode
* 05.09.2013 new HXC coding
* 07.11.2013 hostogram permutation
* 18.12.2013 bugfix: debug output of histogram modification
* 28.02.2014 transmit  three bit depth, one per channel
* 10.03.2014 integrate new bit plane decoder
* 12.03.2014 bugfix in LoadHeader() bit plane coder
* 04.07.2014 support of transparency
* 08.07.2014 bugfix: fake num of channels in presence of alpha 
* 08.07.2014 bugfix: fake maxVal_rct before predcition  
* 14.07.2014 HXC for palette images according to IBatev
* 20.08.2014 new: HXC2
* 08.09.2014 no separate alpha channel coding, when HXC2 is used
* 02.12.2014 bugfix, maxVals must be determined in reconstructed image
* 08.01.2015 bugfix, after undoing COMPACTION, update of maxVals only
*               if compaction was performed for the component
* 23.01.2015 wA, wB
* 19.05.2015 write compact/permuted histogram: adaptive rice parameter
* 03.11.2015 new: SCF
 * 10.11.2015 new: SCF_directional
 * 30.11.2015 new: SCF_predcition, SCF_stage2tuning, SCF_colourMAP
 * 26.08.2016 new: SCF_maxNumPatterns(Idx) has to be transmitted
 * 14.11.2016 include locoImproveFlag
******************************************************/
#include <limits.h>
#include <assert.h>
#include "adjmap.h"
#include "arithmeticB.h"
#include "BP.h"
#include "bitbuf.h"
#include "codec.h"
#include "colour4.h"
#include "fibocode.h"
#include "externpred.h"
//#include "glue.h" /* for CustomParameter	*/
#include "header.h"
#include "histMode.h"
#include "hxc.h"
#include "scf.h"
#include "ifc.h"
/* predictors	*/
#include "leftneighbour.h"
#include "med_proto.h"
#include "paeth.h"

#include "palette.h"
#include "stats.h"
#include "tile.h"
#include "leastSquaresPred.h"

extern CustomParam *cparam; /* in externpred.c */

#ifdef _DEBUG
#define Q_DEBUG
#endif

#ifdef Q_DEBUG_XX
extern FILE *debug_out;
#endif

/* in rice_d.c */
void locoDecoder( PicData *pd, 
								 int T4_flag, int locoImproveFlag, BitBuffer *bitbuf);

int cobalbWrapper( PicData *pd, BitBuffer *bitbuf, 
									int encoder_flag, unsigned char, unsigned char,
									unsigned char, unsigned char,  unsigned char aL, 
									float wA, float wB,
									unsigned char*, 
								  unsigned int numOfConstantLines[],
									INTERMEDIATE_VALS *iv);
void TSIP_dec( PicData *pd, PicHeader *ph, BitBuffer *bitbuf);

#define PrintOUT 1
//
#undef PrintOUT
#define dPrintOUT 1
#undef dPrintOUT

#define MTFOUT 1
#undef MTFOUT 

#define PALETTE
#undef PALETTE 

extern  char* const thisVersion; /* in bacoder.cpp	*/


/*------------------------------------------------------------------
* void ReconstructPic( PicData *rpd, PicData *tilepd, uint *posFullPic, uint *xposFullPic, uint *yposFullPic)
*
* Rekonstruktionsfunktion des Gesamtbildes aus Kacheln.
* Befüllt die Bildstruktur mit den Daten der Kacheln
* Geändert von Team Darmstadt
* Matthias Klenner, Jan Bauer, 14.06.2014 - Team Darmstadt
*-----------------------------------------------------------------*/
void ReconstructPic( PicData *rpd, PicData *tilepd, uint *posFullPic, 
										uint *xposFullPic, uint *yposFullPic)
{
	uint tStart, rStart;
	uint pos = 0;
	uint r; /*yPosition innerhalb der Kachel */
	uint c; /*xPosition innerhalb der Kachel */
	tStart = 0;
	rStart = 0;
	for ( uint ch = 0; ch < tilepd->channel; ch++) /* Für jede Farbe (Channel) */
	{
		pos=0; /* Pixelindex der Kachel */
		for ( c = tStart; c < tilepd->height; c++) /*Zeilen weise durchlaufen */
		{
			for ( r = rStart; r < tilepd->width; r++) /*Spalten (einzelner Pixel)*/
			{     
				/*Der Pixelindex des rekonstruierten Bildes wird entsprechend der
				Kachel, und der Position innerhalb der Kachel selbst, aktualisiert,
				damit die Daten der Kacheln an die richtige Position geschrieben 
				werden */
				// Strutz  *posFullPic = rpd->width * t + *yposFullPic * rpd->width + r + *xposFullPic;
				*posFullPic = rpd->width * (c + *yposFullPic) + r + *xposFullPic;
				/* Farbe aus der Kachel an der Position X/Y auslesen
				und an den PixelIndex des rekonstruierten Bildes schreiben */
				rpd->data[ch][*posFullPic] = tilepd->data[ch][pos]; 
				pos++;      /* Nächster Pixel */
			}     
			rStart = 0;
		}
	}
	/*aktualisieren der xPosition innerhalb des rekonstruierten Bildes
	nach jeder Kachel um die Breite der Kachel */ 
	*xposFullPic = *xposFullPic + tilepd->width; 

	/*Ist die neue xPosition innerhalb des Rekonstruierten Bildes 
	gleich der Breite des rekonstruierten Bildes, wird die yPosition
	um die Kachelhöhe erhöht und die xPosition auf 0 gesetzt, um 
	im rekonstruierten Bild eine "Kachel-Zeile" weiter unten 
	fortzufahren*/
	if ( *xposFullPic == rpd->width)
	{
		*xposFullPic = 0;
		*yposFullPic = *yposFullPic + tilepd->height;
	}
}
// Ende der Änderungen



/*--------------------------------------------------------
* Decoder-Funktion,
* befüllt die Bildstruktur aus einer Datei
*-------------------------------------------------------*/
PicData *DecodePic( FILE * h_file)
{
	int T4_flag, locoImproveFlag; /* for LOCO-I */

	unsigned char *palette = NULL; /* colour palette for indexed images	*/
	unsigned int i, chan;
	/* histogram modification	*/
	unsigned int *idx2val0 = NULL;	/* restore histogram	*/
	unsigned int *idx2val1 = NULL;
	unsigned int *idx2val2 = NULL;
	unsigned int idx2val_len[3];

	/* general	*/
	unsigned long numIdxColours=0;
	PicData *pd = NULL;
	BitBuffer *bitbuf = NULL;
	PicHeader *ph = NULL;
	IMAGEc image, *im;
	// Geändert von Team Darmstadt
	/*Variablen für das spätere Zusammensetzen aus Kacheln */
	uint posFullPic		= 0;
	uint yposFullPic	= 0;
	uint xposFullPic	= 0;
	uint currenttile = 0;
	GlobalPicHeader *gph = NULL;
	PicData *ReconstructedPic = NULL;
	// Ende der Änderungen

	im = &image;

#ifdef Q_DEBUG_XX
	debug_out = fopen( "pred_log.txt", "wt");
#endif

	/* open file and read header info */
	bitbuf = BufferOpenRead( h_file);
	gph = LoadGlobalHeader( bitbuf);

	/*if more than one tile then create space for total image */
	if (gph->numberOfTiles > 1)
	{ 
		ReconstructedPic = CreatePicData( gph->width, gph->height, gph->channel,
			gph->bitperchannel, gph->bitperchan);
	}
	/*falls das GPH-Flag nicht gesetzt ist, wird die Datei 
	wieder geschlossen und neu geöffnet, um die Position des Zeigers 
	innerhalb der Datei wieder auf den Anfang zu setzen */

	do
	{
		// Ende der Änderungen
		BufferReadAlign( bitbuf); /* skip remaining bytes as encoder flushes last bits */
		ph = LoadHeader( bitbuf);

		/* load header for ADAPT mode */
		if (ph->predMode == ADAPT)
		{
			cparam = new CustomParam;
			ReadExtHeader(bitbuf, ph->channel, cparam);
		}
		if (ph->predMode == H26x)
		{
			ALLOC( ph->H26x_Set, 1u, H26xSettings);
			ALLOC( ph->H26x_Set->config, 1u, H26xConfiguration);
			readH26xSettings( ph->H26x_Set->config, bitbuf);
			reconstructLog( ph->H26x_Set, bitbuf);
		}

		/* 
		 * allocate data array
		 */
		/* if indexed colours then allocate three channels	*/
		{
			int numChan;
			numChan =  ph->channel;
			if (ph->colour_space == INDEXED_GBR || ph->colour_space == INDEXED_LUM)
			{
				if (ph->transparency) numChan = 4; /* plus alpha channel	*/
				else									numChan = 3;
			}
			pd = CreatePicData( ph->width, ph->height, numChan, 
				max(ph->bitperchan[0], 
				max( ph->bitperchan[1], ph->bitperchan[2])), ph->bitperchan);
			pd->transparency = ph->transparency;
			// pd->transparency_colour = ph->transparency_colour;

			if (gph->numberOfTiles > 1)
			{
				pd->tileNum = currenttile + 1;
				fprintf( stderr, "\n decoding of tile %d/%d...\n",
					pd->tileNum, gph->numberOfTiles);
			}
			else pd->tileNum = 0;
		}


		/*-----------------------------------------------
		* copy values from file header into local structure
		*/
		for (chan = 0; chan < ph->channel; chan++)
		{
			pd->maxVal_rct[chan] = ph->maxVal_rct[chan];
			pd->maxVal_pred[chan] = ph->maxVal_pred[chan];
			pd->offset[chan] = ph->offset[chan];
			//pd->maxVal_orig[chan] = ph->maxVal_orig[chan];
		}
		for (chan = ph->channel; chan < 4; chan++) /* set unused values to zero */
		{
			pd->maxVal_rct[chan] = pd->maxVal_pred[chan] = 0;
			pd->maxVal_orig[chan] = 0;
		}
		if (pd->transparency == 2) /* transparency with alpha channel	*/
		{
			pd->maxVal_rct[3] = pd->maxVal_pred[3] = pd->maxVal_orig[3] = 255;
		}

		pd->space = ph->colour_space;

		/* fake number of channels	for indexed mode*/
		if (pd->space == INDEXED_GBR || pd->space == INDEXED_LUM)
		{
			if (ph->transparency == 2)	pd->channel = 2; /* plus alpha channel	*/
			else												pd->channel = 1;
		}


		/*
		* read LUT for histogram reconstruction #########################
		*/
		if (ph->histMode == COMPACTION)
		{
			int q, k, N, A;
			unsigned int hist_len;

			hist_len = (1u << pd->bitperchannel);

			/* create histogramm  */
			ALLOC( idx2val0, hist_len, unsigned int);

			idx2val_len[0] = ReadValFromStream( bitbuf, pd->bitperchan[0]+1);
			if (idx2val_len[0])
			{
				idx2val0[0] = rice_decoding( bitbuf, pd->bitperchan[0]-1);
				q = (int)floor( (double)(1u << pd->bitperchan[0]) / idx2val_len[0]);
				N = 1;
				A = q;
				//k = 0;
				//while(q){k++; q = q >> 1;};	/*determine k	*/
				for ( i = 1; i < idx2val_len[0]; i++)
				{
					{
						register int nst = N, At = A;
						for ( k = 0; nst < At; nst <<= 1, k++);
					}
					idx2val0[i] = idx2val0[i-1] + rice_decoding( bitbuf, k);
					N++;
					A += idx2val0[i] - idx2val0[i-1];
				}
			}
			if ( pd->channel >= 3)
			{
				ALLOC( idx2val1, hist_len, unsigned int);
				ALLOC( idx2val2, hist_len, unsigned int);

				idx2val_len[1] = ReadValFromStream( bitbuf, pd->bitperchan[1] + 1);
				if(idx2val_len[1])
				{
					idx2val1[0] = rice_decoding( bitbuf, pd->bitperchan[1]-1);
					q = (int)floor( (double)(1u << pd->bitperchan[1]) / idx2val_len[1]);
					N = 1;
					A = q;
					for ( i = 1; i < idx2val_len[1]; i++)
					{
						{
							register int nst = N, At = A;
							for ( k = 0; nst < At; nst <<= 1, k++);
						}
						idx2val1[i] = idx2val1[i-1] + rice_decoding( bitbuf, k);
						N++;
						A += idx2val1[i] - idx2val1[i-1];
					}
				}
				idx2val_len[2] = ReadValFromStream( bitbuf, pd->bitperchan[2] + 1);
				if (idx2val_len[2])
				{
					idx2val2[0] = rice_decoding( bitbuf, pd->bitperchan[2]-1);
					q = (int)floor( (double)(1u << pd->bitperchan[2]) / idx2val_len[2]);
					N = 1;
					A = q;
					for ( i = 1; i < idx2val_len[2]; i++)
					{
						{
							register int nst = N, At = A;
							for ( k = 0; nst < At; nst <<= 1, k++);
						}
						idx2val2[i] = idx2val2[i-1] + rice_decoding( bitbuf, k);
						N++;
						A += idx2val2[i] - idx2val2[i-1];
					}
				}
			} /* if ( pd->channel >= 3)*/
		}
		else if (ph->histMode == PERMUTATION || ph->histMode == PERMUTE_2OPT)
		{
			int q, k, N, A;
			unsigned int hist_len;

			hist_len = (1u << pd->bitperchannel);

			/* create histogramm  */
			ALLOC( idx2val0, hist_len, unsigned int);

			idx2val_len[0] = ReadValFromStream( bitbuf, pd->bitperchan[0]+1);
			if (idx2val_len[0])
			{
				idx2val0[0] = rice_decoding( bitbuf, pd->bitperchan[0]-1);
				q = (int)floor( (double)(1u << pd->bitperchan[0]) / idx2val_len[0]);
				N = 1;
				A = q;
				//k = 0;
				//while(q){k++; q = q >> 1;};	/*determine k	*/
				for ( i = 1; i < idx2val_len[0]; i++)
				{
					int abs_diff, sign;
					{
						register int nst = N, At = A;
						for ( k = 0; nst < At; nst <<= 1, k++);
					}
					abs_diff = rice_decoding( bitbuf, k);
					N++;
					A += abs_diff;
					sign = BufferGetBit( bitbuf);
					if (sign) idx2val0[i] = idx2val0[i-1] - abs_diff;
					else			idx2val0[i] = idx2val0[i-1] + abs_diff;
				}
			}
			if ( pd->channel >= 3)
			{
				ALLOC( idx2val1, hist_len, unsigned int);
				ALLOC( idx2val2, hist_len, unsigned int);

				idx2val_len[1] = ReadValFromStream( bitbuf, pd->bitperchan[1] + 1);
				if(idx2val_len[1])
				{
					idx2val1[0] = rice_decoding( bitbuf, pd->bitperchan[1]-1);
					q = (int)floor( (double)(1u << pd->bitperchan[1]) / idx2val_len[1]);
					N = 1;
					A = q;
					//k = 0;
					//while(q){k++; q = q >> 1;};	/*determine k	*/
					for ( i = 1; i < idx2val_len[1]; i++)
					{
						int abs_diff, sign;
						{
							register int nst = N, At = A;
							for ( k = 0; nst < At; nst <<= 1, k++);
						}
						abs_diff = rice_decoding( bitbuf, k);
						N++;
						A += abs_diff;
						sign = BufferGetBit( bitbuf);
						if (sign) idx2val1[i] = idx2val1[i-1] - abs_diff;
						else			idx2val1[i] = idx2val1[i-1] + abs_diff;
					}
				}
				idx2val_len[2] = ReadValFromStream( bitbuf, pd->bitperchan[2] + 1);
				if (idx2val_len[2])
				{
					idx2val2[0] = rice_decoding( bitbuf, pd->bitperchan[2]-1);
					q = (int)floor( (double)(1u << pd->bitperchan[2]) / idx2val_len[2]);
					N = 1;
					A = q;
					//k = 0;
					//while(q){k++; q = q >> 1;};	/*determine k	*/
					for ( i = 1; i < idx2val_len[2]; i++)
					{
						int abs_diff, sign;
						{
							register int nst = N, At = A;
							for ( k = 0; nst < At; nst <<= 1, k++);
						}
						abs_diff = rice_decoding( bitbuf, k);
						N++;
						A += abs_diff;
						sign = BufferGetBit( bitbuf);
						if (sign) idx2val2[i] = idx2val2[i-1] - abs_diff;
						else			idx2val2[i] = idx2val2[i-1] + abs_diff;
					}
				}
			} /* if ( pd->channel >= 3)*/
		} /* if (ph->histMode == PERMUTATION)*/


#ifdef Q_DEBUG
		if (ph->histMode != NoMOD && ph->histMode != C_RESET  && ph->histMode != P_RESET )
		{
			FILE *out = NULL;

			out = fopen( "idx2val_dec.txt", "wt");
			fprintf(out, " debugging of histogram modification\n");
			fprintf(out, "\n\n len2: %d\n", idx2val_len[0]);
			for (i=0; i < idx2val_len[0]; i++)
			{
				fprintf(out, "%3d\n", idx2val0[i]);
			}
			if ( pd->channel == 3)
			{
				fprintf(out, "\n\n len2: %d\n", idx2val_len[1]);
				for (i=0; i < idx2val_len[1]; i++)
				{
					fprintf(out, "%3d\n", idx2val1[i]);
				}
				fprintf(out, "\n\n len2: %d\n", idx2val_len[2]);
				for (i=0; i < idx2val_len[2]; i++)
				{
					fprintf(out, "%3d\n", idx2val2[i]);
				}
			}
			fclose( out);
		}
#endif


		if (ph->colour_space == INDEXED_GBR || ph->colour_space == INDEXED_LUM)
		{
			numIdxColours = GetFiboFromStream( bitbuf);
			/* read colour palettte */
			ALLOC( palette, numIdxColours * 3, unsigned char);	

			if (ph->palette_sorting == 0)
			{
				/* receive palette, COMPRESSION ??? 
				* at least for green component, since it is sorted
				*/
				read_palette( bitbuf, numIdxColours, palette);
			}
			else
			{
				read_palette_2( bitbuf, numIdxColours, palette); /* version from PVL KTKI10 Team Wulffpack	*/
			}

#ifdef PALETTE
			{
				FILE *out=NULL;
				char filename[512];


				sprintf( filename, "palette_dec.txt");
				out = fopen( filename, "wt");
				fprintf( out,"# created by TSIP_1.25\n");
				fprintf( out,"# index |  R   G   B\n");

				for ( i = 0; i < numIdxColours; i++) 
				{
					fprintf( out,"%5d: %5d %5d %5d\n", i, palette[i*3+1], palette[i*3+2], palette[i*3]);
				}
				fprintf( out,"\n");

				fclose( out);
			}
#endif

		} /* if (pd->space == INDEXED_COL)	*/


		/*------------------------------------------------------------
		* call of decoding routines
		*/
		{
			unsigned int tmp_channel= 1; /* suppress compiler warning	*/

			//if ( ph->coding_method != HXC2)
			{
				/* do some modification to prevent the decoding of the alpha channel
				 * alpha channel will be decoded afterwards using BPC or HXC
				 */
				tmp_channel = pd->channel;
				if (pd->channel == 2) pd->channel = 1;
				else if (pd->channel == 4) pd->channel = 3;
			}

			if ( (ph->colour_space == INDEXED_GBR || ph->colour_space == INDEXED_LUM) && 
						ph->indexed_adj && ph->indexed_arith)
			{
				/* special decoding for adjacency index arrays	*/
				ADC_adj( pd, numIdxColours, pd->data[0], bitbuf);
			}
			else if (ph->coding_method == LOCO) /* LOCO decoding mode */
			{
				T4_flag = BufferGetBit( bitbuf);
				locoImproveFlag = BufferGetBit( bitbuf);
				locoDecoder( pd, T4_flag, locoImproveFlag, bitbuf);
			}
			else if (ph->coding_method == HXC) /* HXC decoding mode */
			{
				AutoConfig aC;
				aC.HXCtolerance = ph->HXCtolerance;
				aC.HXCtoleranceOffset1 = ph->HXCtoleranceOffset1;
				aC.HXCtoleranceOffset2 = ph->HXCtoleranceOffset2;
				aC.HXCincreaseFac = ph->HXCincreaseFac;

				if ((ph->colour_space == INDEXED_GBR || ph->colour_space == INDEXED_LUM) && 
						ph->predMode == NONE) /* Batev: Indexed mode */
				{
					HXC_codingIdx( pd, bitbuf, &aC, 0, 0u, /*numIdxColours,*/ palette);
				}
				else													/* Batev: RGB or YUV color space mode */
				{
					HXC_coding( pd, bitbuf, &aC, 0, 0u);
					if (pd->channel >= 3)
					{
						HXC_coding( pd, bitbuf, &aC, 0, 1u);
						HXC_coding( pd, bitbuf, &aC, 0, 2u);
					}
				}
			}
			else if (ph->coding_method == HXC2) /* list coding based on H(X|C)	*/
			{
				AutoConfig aC;

				aC.HXC2increaseFac = ph->HXC2increaseFac;
				aC.pattern_size	= ph->pattern_size;

				{
					int ii;
						for( ii= 0; ii < MAX_PATTERN_SIZE; ii++)
						{ /* copy tolerances to all components (channels) 	*/
							aC.HXC2tolerance[0][ii] = ph->HXC2tolerance[0][ii];
							aC.HXC2tolerance[1][ii] = ph->HXC2tolerance[1][ii];
							aC.HXC2tolerance[2][ii] = ph->HXC2tolerance[2][ii];
							aC.HXC2tolerance[3][ii] = ph->HXC2tolerance[3][ii];
						}
				}

				fprintf( stderr, " HXC2\n");
				HXC2_coding( pd, bitbuf, &aC, 0);
			}
			else if (ph->coding_method == SCF) /* screen content coding	*/
			{
				unsigned int dummy; /* only relevant for encoder */
				AutoConfig aC;

				aC.HXC2increaseFac = ph->HXC2increaseFac;
				aC.pattern_size	= ph->pattern_size;
				aC.SCF_prediction	= ph->SCF_prediction;
				aC.SCF_directional	= ph->SCF_directional;
				aC.SCF_stage2tuning	= ph->SCF_stage2tuning;
				aC.SCF_colourMAP	= ph->SCF_colourMAP;
				aC.SCF_maxNumPatternsIdx	= ph->SCF_maxNumPatternsIdx;
				aC.SCF_maxNumPatterns = mapIdx2maxNumPatterns( aC.SCF_maxNumPatternsIdx);

				{
					int ii;
						for( ii= 0; ii < MAX_PATTERN_SIZE; ii++)
						{ /* copy tolerances to all components (channels) 	*/
							aC.HXC2tolerance[0][ii] = ph->HXC2tolerance[0][ii];
							aC.HXC2tolerance[1][ii] = ph->HXC2tolerance[1][ii];
							aC.HXC2tolerance[2][ii] = ph->HXC2tolerance[2][ii];
							aC.HXC2tolerance[3][ii] = ph->HXC2tolerance[3][ii];
						}
				}

				fprintf( stderr, " SCF\n");
				SCF_coding( pd, bitbuf, &aC, 0, &dummy, ph);
			}
			else if (ph->coding_method == CoBaLP2) /* CoBaLP2 decoding mode */
			{
				int err;
				float wA, wB;
				wA = ph->wA *0.1F;
				wB = ph->wB *0.1F;
				fprintf( stderr, " CoBaLP2\n");
				err = cobalbWrapper( pd, bitbuf, 0, 0, 0, 0, 0, 0, wA, wB, NULL, NULL, NULL);
			}
			else if (ph->coding_method == BPC)
			{
				cbool g_flg = true;	/* convert bnary to gray code	*/
				cbool p_flg;
				cbool c_flg;	/* colour transform ?	*/

				if ( ph->predMode == NONE) p_flg = false;
				else		 p_flg = true;
				if (pd->space == GREY || pd->space == RGBc || 
						pd->space == INDEXED_GBR || pd->space == INDEXED_LUM)
					c_flg = false;
				else	c_flg = true;

				if (!ph->use_colFlag) c_flg = 0;	/* ignore	*/
				if (!ph->use_predFlag) p_flg = 0;	/* ignore	*/
				fprintf( stderr, " BPC");
				BP_coding( bitbuf, g_flg, p_flg, c_flg, pd, 0, ph->s_flg);
			} /* if BPC*/
			else if (ph->coding_method == TSIP)
			{
				fprintf( stderr, " TSIP\n");
				TSIP_dec( pd, ph, bitbuf);
			} /* if coding_method == TSIP*/

			//if ( ph->coding_method != HXC2)
			{
				/* undo the fake of channels	*/
				pd->channel =	tmp_channel;
			}

			/* decoding of the alpha channel if required	*/
			if (pd->transparency == 2 /* && ph->coding_method != HXC2*/ )
			{
				unsigned int *ptr_tmp;
				unsigned long tmp_offset, tmp_maxVal_pred; 

				if (pd->space == GREY || pd->space == INDEXED_GBR || pd->space == INDEXED_LUM )
				{
					chan = 1;
				}
				else chan = 3; 

				/* move alpha channel virtually into first array	*/
				tmp_channel = pd->channel;
				pd->channel = 1;
				ptr_tmp = pd->data[0];
				pd->data[0] = pd->data[chan];
				tmp_offset = pd->offset[0];
				pd->offset[0] = pd->offset[chan];
				tmp_maxVal_pred = pd->maxVal_pred[0];
				pd->maxVal_pred[0] = pd->maxVal_pred[chan];

				/* get decision which method to use	*/
				if (BufferGetBit( bitbuf))
				{
					AutoConfig aC;
					aC.HXCtolerance = 0;
					aC.HXCtoleranceOffset1 = 0;
					aC.HXCtoleranceOffset2 = 0;
					aC.HXCincreaseFac = 2;
					HXC_coding( pd, bitbuf, &aC, 0, 0);
				}
				else	
				{
					/* no Gray code, not prediction, no colour transform, significance_flag=1	*/
					BP_coding( bitbuf, 0, 0, 0, pd, 0, 1); /* decoding	*/
				}

				/* move channel back	*/
				pd->data[0] = ptr_tmp;
				pd->channel = tmp_channel;
				pd->offset[0] = tmp_offset;
				pd->maxVal_pred[0] = tmp_maxVal_pred;

			} /* if (pd->transparency == 2)	*/

#ifdef Q_DEBUG
		if (pd->transparency && gph->numberOfTiles == 1)	
		{
			FILE *out=NULL;
			char filename[512];
			unsigned long pos;


			if (pd->channel <= 2) chan = 1;
			else chan = 3;

			sprintf( filename, "alpha_d1_%d.pgm", pd->tileNum);
			out = fopen( filename, "wb");
			fprintf( out,"P5\n");
			fprintf( out,"# created by %s\n", thisVersion);
			fprintf( out,"%d %d\n", pd->width, pd->height);
			fprintf( out,"%d\n", 255);

			for ( pos = 0; pos < pd->size; pos++) 
			{
				putc( pd->data[chan][pos], out);
			}
			fclose( out);

		}
	#endif

		} /* call of dcoding routines */

		/*---------------------------------------------------------------------
		* undo the prediction if required
		*--------------------------------------------------------------------*/
		if (ph->predMode != NONE)
		{
			int tmpA=0, tmpB=0, tmpC=0;
			/* if YUV are jointly stuffed into the RLC1, they should have the 
			* same offset value. So we have to fake the bit depth temporarily
			*/
			if (ph->coding_method == TSIP && ph->channel >= 3)
			{
				if (ph->interleaving_mode == YuvYuv || ph->interleaving_mode == YYuuvv)
				{
					tmpA = pd->bitperchan[0];
					//pd->bitperchan[0] = pd->bitperchan[1];
					pd->bitperchan[0] = pd->bitperchannel;
				}
				if (pd->bitperchan[1] != pd->bitperchan[2] )
				{
					tmpB = pd->bitperchan[1];
					tmpC = pd->bitperchan[2];
					pd->bitperchan[1] = pd->bitperchannel;
					pd->bitperchan[2] = pd->bitperchannel;
				}
				else
				{
					tmpB = tmpC = 0; /* must be equal for restoring the values	*/
				}
			}

#ifdef Q_DEBUG
			{
				FILE *out=NULL;
				char filename[512];


				sprintf( filename, "pre_err_dec.ppm");
				out = fopen( filename, "wt");
				fprintf( out,"P3\n");
				fprintf( out,"# created by TSIP_v2_16\n");
				fprintf( out,"%d %d\n", pd->width, pd->height);
				fprintf( out,"%d\n", 511);

				for ( i = 0; i < pd->size; i++) 
				{
					if ((i % pd->width) == 0)		fprintf( out, "\n");
					for (chan = 0; chan < pd->channel; chan++)
					{
					fprintf( out,"%5d ", pd->data[chan][i]);
					}
				}
				fprintf( out,"\n");

				fclose( out);
			}
#endif

			{
				unsigned int numChan, maxU=255, maxV=255;
				/* fake the number of channel ignoring alpha	*/
				numChan = pd->channel;
				if ( pd->channel == 4) pd->channel = 3;
				else if ( pd->channel == 2) pd->channel = 1;

				/* fake maximum values enfordcing same offset for U and V	*/
				if (pd->channel >= 3)
				{
					maxU = pd->maxVal_rct[1];
					maxV = pd->maxVal_rct[2];
					pd->maxVal_rct[1] = pd->maxVal_rct[2] = max( maxU, maxV);
				}

				switch (ph->predMode)
				{
					case LEFTNEIGHBOUR:	iLEFTNEIGHBOURprediction( pd); break;
					case PAETH:			PaethInverse( pd); break;
					case LMSPRED:		if (ph->useColPrediction_flag)
													{
														if (ph->useColPredictionP_flag)
															lmsPredictionColP( pd, ph->LSpred_numOfPreds, ph->LSpred_searchSpace, 0);
														else
															lmsPredictionColP( pd, ph->LSpred_numOfPreds, ph->LSpred_searchSpace, 0);
													}
													else	lmsPrediction( pd, ph->LSpred_numOfPreds, ph->LSpred_searchSpace, 0);
													break;
					case LSBLEND:		lsZeroOrderPrediction( pd, ph->LSpred_numOfPreds, ph->LSpred_searchSpace, 
														ph->useCorrelation_flag, 0);  break;
					case LSPRED:		lsHighOrderPrediction( pd, ph->LSpred_numOfPreds, ph->LSpred_searchSpace, 0);  break;
					case MED:				MEDprediction( pd, 0);  break;
					case MEDPAETH:	MEDPAETHprediction( pd, 0);  break;
					case ADAPT:			ExternalPredInverse( pd, cparam);			break;
					case H26x:			predictPicture( pd, FALSE, ph->H26x_Set); break;
					default: break;
				}
				pd->channel = numChan; /* restore	*/
				/* restore maximum values 	*/
				if (pd->channel >= 3)
				{
					pd->maxVal_rct[1] = maxU;
					pd->maxVal_rct[2] = maxV;
				}
			}

			/* undo the bit-depth fake	*/
			if (ph->coding_method == TSIP && ph->channel >= 3)
			{
				if (ph->interleaving_mode == YuvYuv || ph->interleaving_mode == YYuuvv)
				{
					/* restore original value	*/
					pd->bitperchan[0] = tmpA;
				}
				if (tmpC != tmpB )
				{
					pd->bitperchan[1] = tmpB;
					pd->bitperchan[2] = tmpC;
				}
			}

			if (ph->predMode == H26x)
			{
				// has been moved out of the main prediction routine because 
				// interleaving keeps overwriting the bitdepth.
				for (i = 0; i < pd->channel; i++)
				{
					if (ph->H26x_Set->newBitdepth[i] < pd->bitperchan[i])
					{
						pd->maxVal_rct[i] = (1 << ph->H26x_Set->newBitdepth[i]) - 1;
						pd->bitperchan[i] = ph->H26x_Set->newBitdepth[i];
					}
				}
				if (pd->channel >= 3)
					pd->bitperchannel = max( pd->bitperchan[0],pd->bitperchan[1]);
				else
					pd->bitperchannel = pd->bitperchan[0];
			}
		}

		/* undo the histogramm compaction	*/
		if (ph->histMode == COMPACTION)
		{
			if (idx2val_len[0]) restore_histogram_compact( pd, 0, idx2val0);
			pd->maxVal_rct[0] = idx2val0[ idx2val_len[0]-1];
			FREE( idx2val0);
			if (pd->channel >= 3)
			{
				if (idx2val_len[1]) 
				{
					restore_histogram_compact( pd, 1, idx2val1);
					pd->maxVal_rct[1] = idx2val1[ idx2val_len[1]-1];
				}
				if (idx2val_len[2]) 
				{
					restore_histogram_compact( pd, 2, idx2val2);
					pd->maxVal_rct[2] = idx2val2[ idx2val_len[2]-1];
				}
				FREE( idx2val1);
				FREE( idx2val2);
			}
		}
		else if (ph->histMode == PERMUTATION || ph->histMode == PERMUTE_2OPT)
		{
			/* pd->maxVal[] is determined inside restore...()	*/
			if (idx2val_len[0]) restore_histogram_permute( pd, 0, idx2val0);
			FREE( idx2val0);
			if (pd->channel >= 3)
			{
				if (idx2val_len[1]) restore_histogram_permute( pd, 1, idx2val1);
				if (idx2val_len[2]) restore_histogram_permute( pd, 2, idx2val2);
				FREE( idx2val1);
				FREE( idx2val2);
			}
		}

#ifdef _DEBUG_XX
		for ( chan = 0; chan < pd->channel; chan++)		
		{
					FILE *out=NULL;
					char filename[512];


					sprintf( filename, "restored_%d.pgm", chan);
					out = fopen( filename, "wt");
					fprintf( out,"P2\n");
					fprintf( out,"# created by %s\n", thisVersion);
					fprintf( out,"%d %d\n", pd->width, pd->height);
					fprintf( out,"%d\n", 255);

					for ( i = 0; i < pd->size; i++) 
					{
						if ((i % pd->width) == 0)		fprintf( out, "\n");
						fprintf( out,"%5d ", pd->data[chan][i]);
					}
					fprintf( out,"\n");
					fclose( out);
				}
#endif

		/* copy parameters in IMAGE structure */
		im->height = pd->height;
		im->width = pd->width;
		im->size = pd->size;
		im->colour_space = pd->space;
		im->colour_channels = pd->channel;
		im->bit_depth[0] = pd->bitperchan[0];
		im->bit_depth[1] = pd->bitperchan[1];
		im->bit_depth[2] = pd->bitperchan[2];
		im->bit_depth_max = pd->bitperchannel;
		im->max_value[0] = pd->maxVal_rct[0];
		im->max_value[1] = pd->maxVal_rct[1];
		im->max_value[2] = pd->maxVal_rct[2];

		switch(im->colour_space)
		{
		case GREY: break;
		case RGBc: break;
		case A1_1:  A1_1toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_2:  A1_2toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_3:  A1_3toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_4:  A1_4toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_5:  A1_5toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_6:  A1_6toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_7:  A1_7toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_8:  A1_8toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_9:  A1_9toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_10: A1_10toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_11: A1_11toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_12: A1_12toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A2_1:  A2_1toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_2:  A2_2toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_3:  A2_3toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_4:  A2_4toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_5:  A2_5toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_6:  A2_6toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_7:  A2_7toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_8:  A2_8toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_9:  A2_9toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_10: A2_10toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_11: A2_11toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_12: A2_12toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A3_1:  A3_1toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_2:  A3_2toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_3:  A3_3toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_4:  A3_4toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_5:  A3_5toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_6:  A3_6toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_7:  A3_7toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_8:  A3_8toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_9:  A3_9toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_10: A3_10toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_11: A3_11toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_12: A3_12toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A4_1:  A4_1toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_2:  A4_2toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_3:  A4_3toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_4:  A4_4toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_5:  A4_5toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_6:  A4_6toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_7:  A4_7toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_8:  A4_8toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_9:  A4_9toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_10: A4_10toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_11: A4_11toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_12: A4_12toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A5_1:  A5_1toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_2:  A5_2toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_3:  A5_3toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_4:  A5_4toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_5:  A5_5toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_6:  A5_6toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_7:  A5_7toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_8:  A5_8toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_9:  A5_9toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_10: A5_10toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_11: A5_11toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_12: A5_12toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A6_1:  A6_1toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_2:  A6_2toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_3:  A6_3toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_4:  A6_4toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_5:  A6_5toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_6:  A6_6toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_7:  A6_7toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_8:  A6_8toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_9:  A6_9toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_10: A6_10toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_11: A6_11toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_12: A6_12toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A7_1:  A7_1toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_2:  A7_2toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_3:  A7_3toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_4:  A7_4toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_5:  A7_5toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_6:  A7_6toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_7:  A7_7toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_8:  A7_8toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_9:  A7_9toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_10: A7_10toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_11: A7_11toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_12: A7_12toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A8_1:  A8_1toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_2:  A8_2toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_3:  A8_3toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_4:  A8_4toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_5:  A8_5toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_6:  A8_6toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_7:  A8_7toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_8:  A8_8toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_9:  A8_9toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_10: A8_10toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_11: A8_11toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_12: A8_12toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A9_1:  A9_1toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_2:  A9_2toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_3:  A9_3toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_4:  A9_4toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_5:  A9_5toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_6:  A9_6toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_7:  A9_7toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_8:  A9_8toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_9:  A9_9toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_10: A9_10toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_11: A9_11toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_12: A9_12toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case B1_1:  B1_1toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B1_2:  B1_2toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B2_1:  B2_1toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B2_3:  B2_3toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B3_2:  B3_2toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B3_3:  B3_3toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B4_1:  B4_1toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B5_2:  B5_2toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B6_3:  B6_3toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case PEI09:  PEI09toRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_1s:  A7_1stoRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_1m:  A7_1mtoRGB( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case INDEXED_GBR: 
		case INDEXED_LUM: 
			/* component [0] contains now the indices	*/
			if (ph->indexed_adj)
			{
				//Indexed2RGBColours_adj( pd, palette_adj, numIdxColours);
				Adjmap_perform_rebuild2( pd, palette, numIdxColours);
				//#ifdef Q_DEBUG
				{
					FILE *out=NULL;
					char filename[512];


					sprintf( filename, "indicesReMapped.pgm");
					out = fopen( filename, "wt");
					fprintf( out,"P2\n");
					fprintf( out,"# created by TSIP_v1.14\n");
					fprintf( out,"%d %d\n", pd->width, pd->height);
					fprintf( out,"%d\n", numIdxColours);

					for ( i = 0; i < pd->size; i++) 
					{
						if ((i % pd->width) == 0)		fprintf( out, "\n");
						fprintf( out,"%5d ", pd->data[0][i]);
					}
					fprintf( out,"\n");
					fclose( out);
				}
				//#endif
			}
			/* undo fake of channels	*/
			if (pd->transparency)
			{
				unsigned int *ptr = NULL, tmp;
				pd->channel = 4; 
				/* copy transparence data into fourth array  via exchange of pointers */
				ptr = pd->data[1];
				pd->data[1] = pd->data[3];
				pd->data[3] = ptr;
				tmp = pd->maxVal_pred[1];
				pd->maxVal_pred[1] = pd->maxVal_pred[3];
				pd->maxVal_pred[3] = tmp;
				tmp = pd->maxVal_rct[1];
				pd->maxVal_rct[1] = pd->maxVal_rct[3];
				pd->maxVal_rct[3] = tmp;
				tmp = pd->offset[1];
				pd->offset[1] = pd->offset[3];
				pd->offset[3] = tmp;
				tmp = pd->bitperchan[1];
				pd->bitperchan[1] = pd->bitperchan[3];
				pd->bitperchan[3] = tmp;
			}
			else			pd->channel = 3; 

			Indexed2RGBColours( pd, palette, numIdxColours);

			pd->bitperchannel = 8; /* if RGB is used	*/
			pd->bitperchan[0] = 8;
			pd->bitperchan[1] = 8;
			pd->bitperchan[2] = 8;
			if (pd->transparency)			pd->bitperchan[3] = 8; 
			else			pd->bitperchan[3] = 0; 

			/* resest to RGBC later, as used colour space (INDEXED_COL) is still required	*/

			FREE( palette);
			break;
		default: 
			break;/* do nothing	*/
		}
#ifdef _DEBUG
		if (0)
		{
				FILE *out=NULL;
				char filename[512];

				sprintf( filename, "rd.pgm");
				out = fopen( filename, "wt");
				fprintf( out,"P2\n");
				fprintf( out,"# created by %s\n", thisVersion);
				fprintf( out,"%d %d\n", pd->width, pd->height);
				fprintf( out,"%d\n", 255);

				for ( i = 0; i < pd->size; i++) 
				{
					if ((i % pd->width) == 0)		fprintf( out, "\n");
					fprintf( out,"%5d ", pd->data[0][i]);
				}
				fprintf( out,"\n");
				fclose( out);
				if (pd->channel >= 3)
				{
					sprintf( filename, "gd.pgm");
					out = fopen( filename, "wt");
					fprintf( out,"P2\n");
					fprintf( out,"# created by %s\n", thisVersion);
					fprintf( out,"%d %d\n", pd->width, pd->height);
					fprintf( out,"%d\n", 255);

					for ( i = 0; i < pd->size; i++) 
					{
						if ((i % pd->width) == 0)		fprintf( out, "\n");
						fprintf( out,"%5d ", pd->data[1][i]);
					}
					fprintf( out,"\n");
					fclose( out);

					sprintf( filename, "bd.pgm");
					out = fopen( filename, "wt");
					fprintf( out,"P2\n");
					fprintf( out,"# created by %s\n", thisVersion);
					fprintf( out,"%d %d\n", pd->width, pd->height);
					fprintf( out,"%d\n", 255);

					for ( i = 0; i < pd->size; i++) 
					{
						if ((i % pd->width) == 0)		fprintf( out, "\n");
						fprintf( out,"%5d ", pd->data[2][i]);
					}
					fprintf( out,"\n");
					fclose( out);
				}
		}
#endif
		/* copy changed parameters back */
		if ( pd->space != INDEXED_GBR && pd->space != INDEXED_LUM)
		{
			pd->bitperchan[0] = im->bit_depth[0];
			pd->bitperchan[1] = im->bit_depth[1];
			pd->bitperchan[2] = im->bit_depth[2];
			pd->bitperchannel = im->bit_depth_max;
			pd->space = im->colour_space;
		}
		else
		{
			pd->space = RGBc; /* reset back to RGB*/
		}

		if(0)
		{
			/* output image */
			unsigned int x, y;
			char filename[512];
			FILE *out=NULL;

			sprintf( filename, "tile_%d.ppm", currenttile);
			out = fopen( filename, "wb");
			if (pd->channel >= 3)		fprintf( out, "P3\n");
			else		fprintf( out, "P2\n");
			fprintf( out, "%d %d\n", pd->width, pd->height);
			fprintf( out, "255\n");
			for( y=0; y<pd->height; y++)
			{
				for( x=0; x<pd->width; x++)
				{
					fprintf( out, "%d ", *GetXYValue( pd, 0, x, y));
					if (pd->channel >= 3)	
					{
						fprintf( out, "%d ", *GetXYValue( pd, 1, x, y));
						fprintf( out, "%d  ", *GetXYValue( pd, 2, x, y));
					}
				}
				fprintf( out, "\n");
			}
			fclose( out);
		}

		if (ph->predMode == H26x)
		{
			FREE(ph->H26x_Set->config);
			FREE(ph->H26x_Set->log);
			FREE(ph->H26x_Set);
		}
		if (ph->predMode == ADAPT)
		{
			freeParameter( cparam->predparam); /* in ReadExtHeader allociated	*/
			delete cparam;	
		}

		DeleteHeader( ph);
		// Geändert von Team Darmstadt
		currenttile++;
		// ---

		/* set alpha channel if binary transparency in combination with greyscale image	*/
		//if ( pd->transparency == 3)
		//{
		//	unsigned long pos, maxVal;
		//	for ( pos = 0; pos < pd->size; pos++)
		//	{
		//		if ( pd->data[0][pos] == pd->transparency_colour)	
		//		{
		//			pd->data[0][pos] = 0; /* dummy value */
		//			pd->data[1][pos] = 0;
		//		}
		//		else																							
		//			pd->data[1][pos] = 255;
		//	}
		//	
		//	/* remove additional value		*/
		//	maxVal = pd->transparency_colour - 1;
		//	pd->bitperchan[0] = 0;
		//	while (maxVal) /* re determine bit depth	*/
		//	{
		//		maxVal >>=1;
		//		pd->bitperchan[0]++;
		//	}
		//	pd->bitperchannel = max( pd->bitperchan[0], pd->bitperchan[1]);

		//}

		if (gph->numberOfTiles != 1) /*falls gekachelt, mit Funktion zusammensetzen */
		{
			ReconstructPic( ReconstructedPic, pd, &posFullPic, &xposFullPic, &yposFullPic);
			DeletePicData( pd);
		}
		else
		{
			ReconstructedPic = pd; /*falls nicht gekachelt, ist pd das fertige Bild */
		}

#ifdef Q_DEBUG_XX
		if (pd->transparency && gph->numberOfTiles == 1)	
		{
			FILE *out=NULL;
			char filename[512];
			unsigned long pos;


			if (pd->channel <= 2) chan = 1;
			else chan = 3;

			sprintf( filename, "alpha_d_%d.pgm", pd->tileNum);
			out = fopen( filename, "wb");
			fprintf( out,"P5\n");
			fprintf( out,"# created by %s\n", thisVersion);
			fprintf( out,"%d %d\n", pd->width, pd->height);
			fprintf( out,"%d\n", 255);

			for ( pos = 0; pos < pd->size; pos++) 
			{
				putc( pd->data[chan][pos], out);
			}
			fclose( out);

		}
	#endif

	} while ( currenttile < gph->numberOfTiles); /*Solange bis alle Kacheln verarbeitet wurden*/
	// Ende der Änderungen
	FREE( bitbuf);
	DeleteGlobalHeader( gph);

#ifdef Q_DEBUG_XX
	fclose( debug_out);
#endif

	/* determine maxVals	*/
	{
			unsigned long pos;
		ReconstructedPic->maxVal_orig[0] = 0;
		ReconstructedPic->maxVal_orig[1] = 0;
		ReconstructedPic->maxVal_orig[2] = 0;
		ReconstructedPic->maxVal_orig[3] = 0;
		for ( chan = 0; chan < ReconstructedPic->channel; chan++)
		{
			for ( pos = 0; pos < ReconstructedPic->size; pos++)
			{
				if (ReconstructedPic->maxVal_orig[chan] < ReconstructedPic->data[chan][pos]) 
					ReconstructedPic->maxVal_orig[chan] = ReconstructedPic->data[chan][pos];
			}
		}
	}

	// Geändert von Team Darmstadt
	return ReconstructedPic; /*Rückgabe des Ausgabe-Bildes */
	// Ende der Änderungen
}

