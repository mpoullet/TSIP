/*****************************************************
* File...: codec.c
* Desc...: implementation of encoder
* Authors: Thomas Schmiedel, Tilo Strutz
* Data...: 12.11.2010, 19.05.2011 (Strutz)
* 27.05.2011 Strutz: new colour transforms
* 03.06.2011 Strutz: new parameters for rYUV
* 04.07.2011 include T4
* 08.07.2011 determine max. values of each channel
*						after colout transform (finally removed)
*           same offset also for UV (required for O1O2O3)
* 23.01.2012 new: left neighbour prediction
* 03.02.2012 new: aC-> ans iF-> structures
* 10.07.2012 write/read_palette_2() new version
* 30.07.2012 adjacency mapping included
* 01.08.2012 IFCencode instead of WFCEncode
* 02.08.2013 YY|uuvv interleaving
* 02.09.2013 new HXC coding
* 07.11.2013 histogram permutation
* 18.12.2013 bugfix: debug output of histogram modification
 * 10.02.2014 new HXCauto adaptive setting of parameters
 * 28.02.2014 transmit  three bit depth, one per channel
 * 10.03.2014 integrate new bit plane coder
 * 12.03.2014 bugfix in WriteHeader() bit plane coder
 * 12.03.2014 compact_histogram() w/o returning a maxvalue
 * 25.03.2014 call of BP_coding() instead off BP_encode()
 * 25.03.2014 bugfix: update of maxVals after prediction
* 04.07.2014 support of transparency
* 08.07.2014 bugfix: fake num of channels in presence of alpha 
* 08.07.2014 bugfix: fake maxVal_rct before predcition  
* 10.07.2014 correction in HXCauto mode: HXCtoleranceOffset
* 14.07.2014 HXC for palette images according to IBatev
* 20.08.2014 new: HXC2
* 08.09.2014 no separate alpha channel coding, when HXC2 is used
* 16.11.2014 bugfix: go not switch to Y_U_V if RGBc
 * 23.01.2015 syn2
* 23.01.2015 wA, wB; limits for transparency coding changed
* 19.05.2015 write compact/permuted histogram: adaptive rice parameter
* 28.08.2015 pred_flag for compact_histogram()
* 22.09.2015 getHXC2_tolerances_tree() nur wenn numCol < 3000
 * 16.10.2015 re-determine HXC2 pattern_size based on chosen tolerances
* 03.11.2015 new: SCF
 * 10.11.2015 new: SCF_directional
 * 30.11.2015 new: SCF_prediction, SCF_stage2tuning, SCF_colourMAP
 * 26.08.2016 new: SCF_maxNumPatterns(Idx) has to be transmitted
 * 15.08.2017 new 	LSpred_searchSpace;	LSpred_numOfPreds
******************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <assert.h>
#include "arithmeticB.h"
#include "adjmap.h"
#include "autoConfig.h"
#include "BP.h"
#include "codec.h"
#include "bitbuf.h"
#include "colour4.h"
#include "fibocode.h"
#include "histMode.h"
#include "header.h"
#include "huff.h"
#include "hxc.h"
#include "scf.h"
#include "ifc.h"
#include "imageFeatures.h"
#include "leftneighbour.h"
#include "med_proto.h"
#include "paeth.h"
#include "palette.h"
#include "stats.h"
#include "externpred.h"
#include "leastSquaresPred.h"


#ifdef _DEBUG
	#define Q_DEBUG
#endif

#ifdef Q_DEBUG
FILE *debug_out; /* used in several prediction routines	*/
#endif
//#undef Q_DEBUG

/* in rice_e.c */
void	locoEncoder( PicData *orig, PicData *pd, 
									int T4_flag, int locoImproveFlag, BitBuffer *bitbuf);

int cobalbWrapper( PicData *pd, BitBuffer *bitbuf, 
								 int encoder_flag, unsigned char, unsigned char,
								 unsigned char, unsigned char, unsigned char, float wA, float wB, 
								 unsigned char *cL_flag, 
								 unsigned int numOfConstantLines[],
								 INTERMEDIATE_VALS *iv);

void TSIP_enc( PicData *pd, BitBuffer *bitbuf,
							AutoConfig *aC,	INTERMEDIATE_VALS *iv);

//void contextQuant( PicData *pd, unsigned int numColours,  
//							 AutoConfig *aC, int exhaustive_flag);

#define PrintOUT 1
//
#undef PrintOUT
#define dPrintOUT 1
#undef dPrintOUT

#define MTFOUT 1
#undef MTFOUT
#define PALETTE
#undef PALETTE 



/*--------------------------------------------------------
* Haupt-Kompressionsroutine,
* pd -> Befüllte Bildstruktur,
* file -> Ausgabedatei,
* blocksize -> Kantenlänge eines Quadrates
* angegeben in Pixel. Das Bild wird blockartig
* von links oben nach rechts unten bearbeitet.
* Die Blöcke rechts und unten am Rand fallen evtl.
* kleiner aus. Je größer blocksize, desto besser
* die Kompressionsergebnisse aber länger die Laufzeit.
* cspace -> Gewünschte Farbraumtransformation,
* pbwt -> Gewünschtes PostBWT-Verfahren (MTF/IFC),
* pbwtparam -> Parameter für das gewählte
* PostBWT-Verfahren.
*--------------------------------------------------------*/
void EncodePic( PicData *pd, 
							 FILE *h_file,
							 void *extparam,
							 AutoConfig *aC,
							 ImageFeatures *iF,
							 INTERMEDIATE_VALS *iv,
							 BYTES_STRUC *bytes)
{
	unsigned char *palette = NULL; /* colour palette for indexed images	*/
	unsigned int *idx2val0=NULL;	/* for decoder	*/
	unsigned int *idx2val1=NULL;	/* required for histogram modification	*/
	unsigned int *idx2val2=NULL;
	unsigned int idx2val_len[3]; /* length of arrays	*/
	unsigned int comp, bufpos;
	unsigned int chan, i;
	unsigned long pos, numBytes_old;
	PicHeader *ph 	   = NULL;
	BitBuffer *bitbuf  = NULL;
	IMAGEc image, *im;

	im = &image;
	bufpos = 0;

#ifdef Q_DEBUG_X
	debug_out = fopen( "pred_log.txt", "wt");
#endif


	/* determination of maxvals */
	pd->maxVal_orig[0] = pd->maxVal_orig[1] = pd->maxVal_orig[2] = pd->maxVal_orig[3] = 0;
	for ( chan = 0; chan < pd->channel; chan++)
	{
		for ( pos = 0; pos < pd->size; pos++)
		{
			if (pd->maxVal_orig[chan] < pd->data[chan][pos]) pd->maxVal_orig[chan] = pd->data[chan][pos];
		}
	}

	/* fix dependencies	*/
	if ( aC->coding_method == HXC2 || aC->coding_method == SCF)
	{
		aC->indexed_flag = 0; /* already be done in check_dependenciies()	*/
	}
#ifdef _DEBUGxx
	if ( pd->channel >= 3)		
	{
				FILE *out=NULL;
				char filename[512];


				sprintf( filename, "r.pgm");
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

				sprintf( filename, "g.pgm");
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

				sprintf( filename, "b.pgm");
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
#endif
	/*
	* do the conversion of colour space
	*/ 
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

	switch (aC->rct_type_used)
	{
		case GREY: break;
		case RGBc: break;
		case A1_1:  RGBtoA1_1( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_2:  RGBtoA1_2( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_3:  RGBtoA1_3( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_4:  RGBtoA1_4( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_5:  RGBtoA1_5( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_6:  RGBtoA1_6( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_7:  RGBtoA1_7( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_8:  RGBtoA1_8( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_9:  RGBtoA1_9( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_10: RGBtoA1_10( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_11: RGBtoA1_11( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A1_12: RGBtoA1_12( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A2_1:  RGBtoA2_1( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_2:  RGBtoA2_2( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_3:  RGBtoA2_3( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_4:  RGBtoA2_4( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_5:  RGBtoA2_5( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_6:  RGBtoA2_6( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_7:  RGBtoA2_7( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_8:  RGBtoA2_8( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_9:  RGBtoA2_9( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_10: RGBtoA2_10( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_11: RGBtoA2_11( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A2_12: RGBtoA2_12( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A3_1:  RGBtoA3_1( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_2:  RGBtoA3_2( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_3:  RGBtoA3_3( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_4:  RGBtoA3_4( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_5:  RGBtoA3_5( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_6:  RGBtoA3_6( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_7:  RGBtoA3_7( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_8:  RGBtoA3_8( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_9:  RGBtoA3_9( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_10: RGBtoA3_10( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_11: RGBtoA3_11( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A3_12: RGBtoA3_12( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A4_1:  RGBtoA4_1( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_2:  RGBtoA4_2( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_3:  RGBtoA4_3( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_4:  RGBtoA4_4( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_5:  RGBtoA4_5( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_6:  RGBtoA4_6( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_7:  RGBtoA4_7( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_8:  RGBtoA4_8( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_9:  RGBtoA4_9( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_10: RGBtoA4_10( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_11: RGBtoA4_11( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A4_12: RGBtoA4_12( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A5_1:  RGBtoA5_1( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_2:  RGBtoA5_2( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_3:  RGBtoA5_3( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_4:  RGBtoA5_4( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_5:  RGBtoA5_5( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_6:  RGBtoA5_6( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_7:  RGBtoA5_7( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_8:  RGBtoA5_8( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_9:  RGBtoA5_9( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_10: RGBtoA5_10( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_11: RGBtoA5_11( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A5_12: RGBtoA5_12( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A6_1:  RGBtoA6_1( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_2:  RGBtoA6_2( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_3:  RGBtoA6_3( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_4:  RGBtoA6_4( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_5:  RGBtoA6_5( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_6:  RGBtoA6_6( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_7:  RGBtoA6_7( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_8:  RGBtoA6_8( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_9:  RGBtoA6_9( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_10: RGBtoA6_10( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_11: RGBtoA6_11( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A6_12: RGBtoA6_12( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A7_1:  RGBtoA7_1( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_2:  RGBtoA7_2( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_3:  RGBtoA7_3( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_4:  RGBtoA7_4( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_5:  RGBtoA7_5( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_6:  RGBtoA7_6( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_7:  RGBtoA7_7( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_8:  RGBtoA7_8( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_9:  RGBtoA7_9( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_10: RGBtoA7_10( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_11: RGBtoA7_11( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_12: RGBtoA7_12( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A8_1:  RGBtoA8_1( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_2:  RGBtoA8_2( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_3:  RGBtoA8_3( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_4:  RGBtoA8_4( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_5:  RGBtoA8_5( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_6:  RGBtoA8_6( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_7:  RGBtoA8_7( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_8:  RGBtoA8_8( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_9:  RGBtoA8_9( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_10: RGBtoA8_10( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_11: RGBtoA8_11( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A8_12: RGBtoA8_12( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case A9_1:  RGBtoA9_1( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_2:  RGBtoA9_2( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_3:  RGBtoA9_3( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_4:  RGBtoA9_4( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_5:  RGBtoA9_5( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_6:  RGBtoA9_6( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_7:  RGBtoA9_7( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_8:  RGBtoA9_8( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_9:  RGBtoA9_9( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_10: RGBtoA9_10( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_11: RGBtoA9_11( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A9_12: RGBtoA9_12( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case B1_1:  RGBtoB1_1( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B1_2:  RGBtoB1_2( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B2_1:  RGBtoB2_1( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B2_3:  RGBtoB2_3( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B3_2:  RGBtoB3_2( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B3_3:  RGBtoB3_3( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B4_1:  RGBtoB4_1( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B5_2:  RGBtoB5_2( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case B6_3:  RGBtoB6_3( im, pd->data[0], pd->data[1], pd->data[2]); break;

		case PEI09:  RGBtoPEI09( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_1s:  RGBtoA7_1s( im, pd->data[0], pd->data[1], pd->data[2]); break;
		case A7_1m:  RGBtoA7_1m( im, pd->data[0], pd->data[1], pd->data[2]); break;

		default: break;
	}

#ifdef _DEBUGxx
			if ( pd->channel >= 3)		
			{
				FILE *out=NULL;
				char filename[512];


				sprintf( filename, "y.pgm");
				out = fopen( filename, "wt");
				fprintf( out,"P2\n");
				fprintf( out,"# created by %s\n", thisVersion);
				fprintf( out,"%d %d\n", pd->width, pd->height);
				fprintf( out,"%d\n", 511);

				for ( i = 0; i < pd->size; i++) 
				{
					if ((i % pd->width) == 0)		fprintf( out, "\n");
					fprintf( out,"%5d ", pd->data[0][i]);
				}
				fprintf( out,"\n");
				fclose( out);

				sprintf( filename, "u.pgm");
				out = fopen( filename, "wt");
				fprintf( out,"P2\n");
				fprintf( out,"# created by %s\n", thisVersion);
				fprintf( out,"%d %d\n", pd->width, pd->height);
				fprintf( out,"%d\n", 511);

				for ( i = 0; i < pd->size; i++) 
				{
					if ((i % pd->width) == 0)		fprintf( out, "\n");
					fprintf( out,"%5d ", pd->data[1][i]);
				}
				fprintf( out,"\n");
				fclose( out);

				sprintf( filename, "v.pgm");
				out = fopen( filename, "wt");
				fprintf( out,"P2\n");
				fprintf( out,"# created by %s\n", thisVersion);
				fprintf( out,"%d %d\n", pd->width, pd->height);
				fprintf( out,"%d\n", 511);

				for ( i = 0; i < pd->size; i++) 
				{
					if ((i % pd->width) == 0)		fprintf( out, "\n");
					fprintf( out,"%5d ", pd->data[2][i]);
				}
				fprintf( out,"\n");
				fclose( out);


			}
#endif


	pd->space = im->colour_space;

	if ( pd->space == GREY || pd->space == RGBc || aC->indexed_flag)
	{
		/* copy max values if no colour transform was applied	*/
		for (chan = 0; chan < pd->channel; chan++)
		{
			pd->maxVal_rct[chan] = pd->maxVal_orig[chan];
		}
		/* different interleaving modes are allowed for RGB	*/
		if (pd->space != RGBc) aC->interleaving_mode = Y_U_V;

		//for (chan = 0; chan < pd->channel; chan++)
		//{	/* reset of offsets	*/
		//	pd->offset[chan] = 0; 
		//}
	}
	else
	{	/* only if colour transform was performed	*/
		/* re-determine max values	*/
		unsigned int mVal;
		pd->maxVal_rct[0] = 0;	/* init	*/
		pd->maxVal_rct[1] = 0;
		pd->maxVal_rct[2] = 0;
		/* must be kept: pd->maxVal_rct[3] */
		for (chan = 0; chan < pd->channel; chan++)
		{
			mVal = pd->data[chan][0];
			for (pos = 1; pos < pd->size; pos++)
			{
				if (mVal < pd->data[chan][pos]) 
					mVal = pd->data[chan][pos];
			}
			pd->maxVal_rct[chan] = mVal;
		}
		/* the colour transform routines use
		 * addval = (1u << im->bit_depth_max) - 1u;
		 * as offset. If bitdepth was changed, this offset was added
		 * offset must be updated in pd and be transmitted to the decoder
		 * ==> use fixed offset only for U and V
		 * offsets will later be overwritten, if prediction is activated
		 */
		for (chan = 0; chan < pd->channel; chan++)
		{
			if (pd->bitperchan[chan] != (unsigned)im->bit_depth[chan])
			{
				/* offset is only added if bit depth had to be increased	*/
				pd->offset[chan] = (1u << pd->bitperchannel) - 1u;
			}
			else
			{
				pd->offset[chan] = 0; 
			}
		}
	}

	/* copy changed parameters back */
	pd->bitperchan[0] = im->bit_depth[0];
	pd->bitperchan[1] = im->bit_depth[1];
	pd->bitperchan[2] = im->bit_depth[2];

	if (pd->channel <= 2)
	{ /* do not include alpha channel	*/
		pd->bitperchannel = pd->bitperchan[0]; 
	}
	else
	{
		pd->bitperchannel = max( pd->bitperchan[0], 
												max( pd->bitperchan[1],pd->bitperchan[2]));
	}
	/*
	 * create indexed image with colour palette ######################################
	 */
	if (aC->indexed_flag)
	{
		assert( pd->channel >= 3);
		/* colour palette is compact by itself */
		if (aC->histMode == COMPACTION)
		{
			aC->histMode = NoMOD; /* essential, otherwise reconstruction might be lossy! */
		}
		/* separate colour palettte from indices	*/
		ALLOC( palette, iF->numColours * 3, unsigned char);	

		if (aC->palette_sorting == 0)
		{
			i = GetIndexedColours( pd, palette);
			pd->space = INDEXED_GBR;
		}
		else
		{
			i = GetIndexedColours_2( pd, palette);
			pd->space = INDEXED_LUM;
		}
		/* fake of number of channels for indexed mode	*/
		if (pd->transparency == 2) 
		{
			unsigned int *ptr = NULL, tmp;
			pd->channel = 2; 
			/* copy transparence data into second array  via exchange of pointers */
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
		else pd->channel = 1; /* component [0] contains now the indices	*/

		if (i != iF->numColours)
		{	/* something wrong	*/
			EXIT_FAILURE;
			return;
		}

		/* set required bits	*/
		pd->bitperchannel = (unsigned int)ceil( log( (double)iF->numColours) / log(2.));
		pd->bitperchan[0] = pd->bitperchannel;
		pd->bitperchan[3] = 0;
		pd->bitperchan[2] = 0;
		if (pd->transparency == 0) pd->bitperchan[1]= 0;

		/* re-set rct max values */
		if (pd->transparency == 1) pd->maxVal_rct[0] = iF->numColours; 
		/* maxVal used for binary transparency indication	*/
		else pd->maxVal_rct[0] = iF->numColours - 1;
		pd->maxVal_rct[3] = 0;	/* reset	*/
		pd->maxVal_rct[2] = 0;
		if (pd->transparency == 0) pd->maxVal_rct[1] = 0;


		/* transmit palette after header */

		/* re-determination of some image features ?	*/
		//iF->noise = estimateNoise( pd, iF); /* get also constance	*/
		{
			long t;
			/* if HXCauto mode is enabled then set HXC parameters automatically	*/
			t = iF->constance + 2 * iF->syn - 1 * iF->noise[0];

#ifdef OBSOLETE
			if (aC->HXC2auto && t > 250 && iF->colperpix < 0.004)
			{
				aC->coding_method = HXC2;
				aC->predMode = NONE;
				if (t >= 565)
				{
					//if (iF->numColours > 256) aC->HXCincreaseFac = 1;
					//else if (iF->numColours > 128) aC->HXCincreaseFac = 2;
					//else aC->HXC2increaseFac = 3;
					for ( comp = 0; comp < pd->channel; comp++)
					{
						for ( i = 0; i < PATTERN_SIZE; i++)
						{
							aC->HXC2tolerance[comp][i] = 0;
						}
					}
				}
				else if (t > 510)
				{
					//aC->HXC2increaseFac = 2;
					for ( comp = 0; comp < pd->channel; comp++)
					{
						for ( i = 0; i < 4; i++)
						{
							aC->HXC2tolerance[comp][i] = 0;
						}
					}
					for ( comp = 0; comp < pd->channel; comp++)
					{
						aC->HXC2tolerance[comp][4] = 1;
						aC->HXC2tolerance[comp][5] = 1;
					}
				}
				else if (t > 350)
				{
					//aC->HXC2increaseFac = 1;
					for ( comp = 0; comp < pd->channel; comp++)
					{
						aC->HXC2tolerance[comp][0] = 0;
						aC->HXC2tolerance[comp][1] = 0;
					}
					for ( comp = 0; comp < pd->channel; comp++)
					{
						for ( i = 2; i < PATTERN_SIZE; i++)
						{
							aC->HXC2tolerance[comp][i] = 1;
						}
					}
				}
				else 
				{
					//aC->HXC2increaseFac = 0;
					for ( comp = 0; comp < pd->channel; comp++)
					{
						aC->HXC2tolerance[comp][0] = 1;
						aC->HXC2tolerance[comp][1] = 1;
					}
					for ( comp = 0; comp < pd->channel; comp++)
					{
						for ( i = 2; i < PATTERN_SIZE; i++)
						{
							aC->HXC2tolerance[comp][i] = 2;
						}
					}
				}
			}
#endif
			if ( aC->coding_method==HXC && aC->HXCauto && t > 250 && iF->colperpix < 0.004)
			{
				aC->coding_method = HXC;
				aC->predMode = NONE;
				if (t >= 565)
				{
					if (iF->numColours > 256) aC->HXCincreaseFac = 1;
					else if (iF->numColours > 128) aC->HXCincreaseFac = 2;
					else aC->HXCincreaseFac = 3;
					aC->HXCtolerance = 0;
					aC->HXCtoleranceOffset1 = 0;
					aC->HXCtoleranceOffset2 = 0;
				}
				else if (t > 510)
				{
					aC->HXCincreaseFac = 2;
					aC->HXCtolerance = 0;
					aC->HXCtoleranceOffset1 = 0;
					aC->HXCtoleranceOffset2 = 1;
				}
				else if (t > 350)
				{
					aC->HXCincreaseFac = 1;
					aC->HXCtolerance = 0;
					aC->HXCtoleranceOffset1 = 1;
					aC->HXCtoleranceOffset2 = 1;
				}
				else 
				{
					aC->HXCincreaseFac = 0;
					aC->HXCtolerance = 1;
					//aC->HXCtoleranceOffset1 = iF->numColours/4; not good for Philips_screw_head_1.ppm
					//aC->HXCtoleranceOffset2 = iF->numColours;
					aC->HXCtoleranceOffset1 = 2;
					aC->HXCtoleranceOffset2 = 2;
				}
			}
		}
		if (aC->indexed_adj)
		{
			/* do the adjacency mapping of indexes	*/

#ifdef _DEBUG
			{
				int val_old, val;
				FILE *out=NULL;
				char filename[512];


				sprintf( filename, "indices.pgm");
				out = fopen( filename, "wt");
				fprintf( out,"P2\n");
				fprintf( out,"# created by %s\n", thisVersion);
				fprintf( out,"%d %d\n", pd->width, pd->height);
				fprintf( out,"%d\n", iF->numColours);

				for ( i = 0; i < pd->size; i++) 
				{
					if ((i % pd->width) == 0)		fprintf( out, "\n");
					fprintf( out,"%5d ", pd->data[0][i]);
				}
				fprintf( out,"\n");
				fclose( out);


				sprintf( filename, "palette.txt");
				out = fopen( filename, "wt");
				fprintf( out,"# created by TSIP_v1.25\n");
				fprintf( out,"# index |  R   G   B\n");

				val_old = 0;
				for ( i = 0; i < iF->numColours; i++) 
				{
					val =(palette[i*3+2]<<16) + (palette[i*3+1]<<8)+ palette[i*3];
					fprintf( out,"%5d: %5d %5d %5d\n", i, 
						palette[i*3+1], palette[i*3+2], palette[i*3]
					);
					val_old = val;
				}
				fprintf( out,"\n");

				fclose( out);
			}
#endif
			Adjmap_perform_map2( pd, palette, iF->numColours);

#ifdef _DEBUG
			{
				FILE *out=NULL;
				char filename[512];


				sprintf( filename, "indicesMapped.pgm");
				out = fopen( filename, "wt");
				fprintf( out,"P2\n");
				fprintf( out,"# created by TSIP_v1.25\n");
				fprintf( out,"%d %d\n", pd->width, pd->height);
				fprintf( out,"%d\n", iF->numColours);

				for ( i = 0; i < pd->size; i++) 
				{
					if ((i % pd->width) == 0)		fprintf( out, "\n");
					fprintf( out,"%5d ", pd->data[0][i]);
				}
				fprintf( out,"\n");
				fclose( out);
			}
#endif
		}
	} /* if (aC->indexed_flag) */

	/*
	 * do the histogram compaction
	 * is also required in PERMUTAION mode
	 * transmit permutation arrays after header ########################
	 */
	if (aC->histMode == COMPACTION )
	{
		unsigned int hist_len;

		hist_len = (1u << pd->bitperchannel); /* maximum number of histogram bins	*/

		/* create histogramm  */
		ALLOC( idx2val0, hist_len, unsigned int);
		ALLOC( idx2val1, hist_len, unsigned int);
		ALLOC( idx2val2, hist_len, unsigned int);

		{
			int pred_flag = 0;
			if (aC->predMode != NONE  || 
					aC->coding_method == LOCO || aC->coding_method == CoBaLP2) pred_flag = 1;
			idx2val_len[0] = compact_histogram( pd, 0, idx2val0, pred_flag);
			idx2val_len[1] = idx2val_len[2] = 0; /* init	*/

			if ( pd->channel >= 3)
			{
				idx2val_len[1] = compact_histogram( pd, 1, idx2val1, pred_flag);
				idx2val_len[2] = compact_histogram( pd, 2, idx2val2, pred_flag);
			}
		}

		if (idx2val_len[0] == 0 && idx2val_len[1] == 0 && idx2val_len[2] == 0)
		{
			/* compaction is not possible	*/
			aC->histMode = C_RESET;	/* switch off histogram modus; saves side information */
			FREE( idx2val0);
			FREE( idx2val1);
			FREE( idx2val2);
		}

		/* update maxvals	if compaction is performed */
		/* after histogramm modification, the offset has no meaning any more*/
		if (idx2val_len[0])
		{
			pd->maxVal_rct[0] = idx2val_len[0] - 1;
			pd->offset[0] = 0;
		}
		if (idx2val_len[1])
		{
			pd->maxVal_rct[1] = idx2val_len[1] - 1;
			pd->offset[1] = 0;
		}
		if (idx2val_len[2])
		{
			pd->maxVal_rct[2] = idx2val_len[2] - 1;
			pd->offset[2] = 0;
		}

		{
			unsigned int syn[4];
			iF->syn2 = HowSynthetic( pd, syn);
			/* currently there seem to be problems, since syn2 can be larger than syn;
			* has to be checked
			*/
		}
	}
	else	if (aC->histMode == PERMUTATION)
	{
		// unsigned int maxVal;
		unsigned int hist_len;
		unsigned int *val2idx0 = NULL;
		unsigned int *val2idx1 = NULL;
		unsigned int *val2idx2 = NULL;

		/* do the compaction first w/o modification of signal	*/
		hist_len = (1u << pd->bitperchannel);

		/* create histogramm  */
		ALLOC( idx2val0, hist_len, unsigned int);
		ALLOC( idx2val1, hist_len, unsigned int);
		ALLOC( idx2val2, hist_len, unsigned int);

		ALLOC( val2idx0, hist_len, unsigned int);
		ALLOC( val2idx1, hist_len, unsigned int);
		ALLOC( val2idx2, hist_len, unsigned int);

		/* compaction first, permutation afterwards	*/
		idx2val_len[0] = compact_histogram_X( pd, 0, idx2val0, val2idx0);
		idx2val_len[1] = idx2val_len[2] = 0; /* init	*/

		if ( pd->channel >= 3)
		{
			idx2val_len[1] = compact_histogram_X( pd, 1, idx2val1, val2idx1);
			idx2val_len[2] = compact_histogram_X( pd, 2, idx2val2, val2idx2);
		}
	//	maxVal = hist_len -1; //(1u << pd->bitperchannel) - 1;

		idx2val_len[0] = permute_histogram( pd, 0, idx2val_len[0], idx2val0, val2idx0);

		if ( pd->channel >= 3)
		{
			idx2val_len[1] = permute_histogram( pd, 1, idx2val_len[1], idx2val1, val2idx1);
			idx2val_len[2] = permute_histogram( pd, 2, idx2val_len[2], idx2val2, val2idx2);
		}
		pd->bitperchannel = max( pd->bitperchan[0], max( pd->bitperchan[1], pd->bitperchan[2]));

		if (idx2val_len[0] == 0 && idx2val_len[1] == 0 && idx2val_len[2] == 0)
		{
			/* compaction is not possible	*/
			aC->histMode = P_RESET;	/* switch off histogram modus; saves side information */
		}
		FREE( val2idx0);
		FREE( val2idx1);
		FREE( val2idx2);

		/* update maxvals	if compaction is performed */
		/* after histogramm modification, the offset has no meaning any more*/
		if (idx2val_len[0])
		{
			pd->maxVal_rct[0] = idx2val_len[0] - 1;
			pd->offset[0] = 0;
		}
		if (idx2val_len[1])
		{
			pd->maxVal_rct[1] = idx2val_len[1] - 1;
			pd->offset[1] = 0;
		}
		if (idx2val_len[2])
		{
			pd->maxVal_rct[2] = idx2val_len[2] - 1;
			pd->offset[2] = 0;
		}
		{
			unsigned int syn[4];
			iF->syn2 = HowSynthetic( pd, syn);
		}
	} /* if (aC->histMode == PERMUTATION)*/
	else if (aC->histMode ==PERMUTE_2OPT)
	{
		unsigned int hist_len;
		unsigned int *val2idx0 = NULL;
		unsigned int *val2idx1 = NULL;
		unsigned int *val2idx2 = NULL;

		/* do the compaction first w/o modification of signal	*/
		hist_len = (1u << pd->bitperchannel);

		/* create histogramm  */
		ALLOC( idx2val0, hist_len, unsigned int);
		ALLOC( idx2val1, hist_len, unsigned int);
		ALLOC( idx2val2, hist_len, unsigned int);

		ALLOC( val2idx0, hist_len, unsigned int);
		ALLOC( val2idx1, hist_len, unsigned int);
		ALLOC( val2idx2, hist_len, unsigned int);

		/* compaction first, then permutation	*/
		idx2val_len[0] = compact_histogram_X( pd, 0, idx2val0, val2idx0);
		idx2val_len[1] = idx2val_len[2] = 0; /* init	*/

		if ( pd->channel >= 3)
		{
			idx2val_len[1] = compact_histogram_X( pd, 1, idx2val1, val2idx1);
			idx2val_len[2] = compact_histogram_X( pd, 2, idx2val2, val2idx2);
		}
	//	maxVal = hist_len -1; //(1u << pd->bitperchannel) - 1;

		idx2val_len[0] = permute_histogram_kopt( pd, 0, idx2val_len[0], idx2val0, val2idx0);

		if ( pd->channel >= 3)
		{
			idx2val_len[1] = permute_histogram_kopt( pd, 1, idx2val_len[1], idx2val1, val2idx1);
			idx2val_len[2] = permute_histogram_kopt( pd, 2, idx2val_len[2], idx2val2, val2idx2);
		}
		pd->bitperchannel = max( pd->bitperchan[0], max( pd->bitperchan[1], pd->bitperchan[2]));

		if (idx2val_len[0] == 0 && idx2val_len[1] == 0 && idx2val_len[2] == 0)
		{
			/* compaction is not possible	*/
			aC->histMode = T_RESET;	/* switch off histogram modus; saves side information */
		}
		FREE( val2idx0);
		FREE( val2idx1);
		FREE( val2idx2);

		/* update maxvals	if compaction is performed */
		/* after histogramm modification, the offset has no meaning any more*/
		if (idx2val_len[0])
		{
			pd->maxVal_rct[0] = idx2val_len[0] - 1;
			pd->offset[0] = 0;
		}
		if (idx2val_len[1])
		{
			pd->maxVal_rct[1] = idx2val_len[1] - 1;
			pd->offset[1] = 0;
		}
		if (idx2val_len[2])
		{
			pd->maxVal_rct[2] = idx2val_len[2] - 1;
			pd->offset[2] = 0;
		}
		{
			unsigned int syn[4];
			iF->syn2 = HowSynthetic( pd, syn);
		}
	}
	else iF->syn2 = iF->syn;

#ifdef Q_DEBUG_XX
		if (aC->histMode != NoMOD && aC->histMode != C_RESET && aC->histMode != P_RESET )
	 {
			FILE *out = NULL;

			out = fopen( "idx2val_enc.txt", "wt");
			fprintf(out, " debugging of histogram modification\n");
			fprintf(out, "\n\n len0: %d\n", idx2val_len[0]);
			for (i=0; i < idx2val_len[0]; i++)
			{
				fprintf(out, "%3d\n", idx2val0[i]);
			}
			if ( pd->channel >= 3)
			{
				fprintf(out, "\n\n len1: %d\n", idx2val_len[1]);
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


#ifdef Q_DEBUG_NIXX
		{
			unsigned int j;
			FILE *out = NULL;

			if (aC->histMode == PERMUTATION)
			{
				out = fopen( "image0_perm.pgm", "wt");
				fprintf( out, "P2\n");
				fprintf( out, "%d %d %d \n", pd->width, pd->height, idx2val_len[0]);
			}
			else 
			{
				out = fopen( "image0_orig.pgm", "wt");
				fprintf( out, "P2\n");
				fprintf( out, "%d %d %d \n", pd->width, pd->height, 1<<pd->bitperchan[0]);
			}
			for ( i = 0; i < pd->height; i++)
			{
				for ( j = 0; j < pd->width; j++)
				{
					fprintf( out, " %3d", pd->data[0][i*pd->width + j]);
				}
				fprintf( out, "\n");
			}
			fclose( out);
		}
#endif


	/* if components are jointly treated, then the range (=> offset)
	 * should be the same supporting the RLC
	 * this is only required if prediction offset is dependent on maxVals
	 * in the case that the offsets are dependent on bit depth this equalisation 
	 * seems to obsolete
	 */
#ifdef UNCLEAR
	if (aC->coding_method == TSIP && aC->interleaving_mode != Y_U_V)
	{
		uint maxV;
		maxV = _GETMAX( pd->maxVal_rct[1], pd->maxVal_rct[2]);
		if (aC->interleaving_mode == YY_uvuv || aC->interleaving_mode == YY_uuvv)
		{
			/* joint processing of U and V	*/
			pd->maxVal_rct[1] = pd->maxVal_rct[2] = maxV;
		}
		else
		{
			/* joint processing of all components	*/
			maxV = _GETMAX( pd->maxVal_rct[0], maxV);
			pd->maxVal_rct[0] = pd->maxVal_rct[1] = pd->maxVal_rct[2] = maxV;
		}
	}
#endif

#ifdef Q_DEBUG_X
		{
			FILE *out=NULL;
			char filename[512];


			sprintf( filename, "rgb.ppm");
			out = fopen( filename, "wt");
			if (pd->channel ==1)		fprintf( out, "P2\n");
			else		fprintf( out, "P3\n");
			fprintf( out,"# created by %s\n", thisVersion);
			fprintf( out,"%d %d\n", pd->width, pd->height);
			fprintf( out,"%d\n", 255);

			for ( i = 0; i < pd->size; i++) 
			{
				if ((i % pd->width) == 0)		fprintf( out, "\n");
				fprintf( out,"%5d ", pd->data[0][i]);
				if (pd->channel >= 3)
				{
					fprintf( out,"%5d ", pd->data[1][i]);
					fprintf( out,"%5d  ", pd->data[2][i]);
				}
			}
			fprintf( out,"\n");

			fclose( out);
		}
#endif



	/*---------------------------------------------------------------------
	 * do the prediction if required
	 *--------------------------------------------------------------------*/
	/* copy the maximum values per channel	
	 * also required for alpha channel coding
	 */
	for (chan = 0; chan < 4; chan++)
	{
		unsigned int val;
		pd->maxVal_pred[chan] = pd->maxVal_rct[chan];
		val = pd->maxVal_pred[chan];
		//BITDEPTH( val, pd->bitperchan[chan]);
	}
		pd->bitperchannel = max( pd->bitperchan[0], max( pd->bitperchan[1], pd->bitperchan[2]));

	if (aC->coding_method == LOCO || aC->coding_method == CoBaLP2) /* no prediction */
	{
		aC->skip_precoding = 1; /* ????? , should be set already */
		aC->predMode = NONE;
	}	
	else 
	{ 
		int tmpA = NULL, tmpB = NULL, tmpC = NULL;

		/* the prediction routines add an offset value to the prediction error
		* in order to avoid negative values
		* if Y and UV are jointly stuffed into the RLC1, they should have the 
		* same offset value. So we have to fake the bit depth temporarily
		*/
		if (aC->coding_method == TSIP && pd->channel >= 3)
		{
			if (aC->interleaving_mode == YuvYuv || aC->interleaving_mode == YYuuvv)
			{
				tmpA = pd->bitperchan[0];
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
		if (aC->predMode != NONE) fprintf( stderr, "\n                do the prediction ...     ");
		{
			unsigned int numChan, maxU=255, maxV=255;
			/* fake the number of channel ignoring alpha	*/
			numChan = pd->channel;
			if ( pd->channel == 4) pd->channel = 3;
			else if ( pd->channel == 2) pd->channel = 1;

			/* fake maximum values enforcing same offset for U and V	*/
			if (pd->channel >= 3)
			{
				maxU = pd->maxVal_rct[1];
				maxV = pd->maxVal_rct[2];
				pd->maxVal_rct[1] = pd->maxVal_rct[2] = max( maxU, maxV);
			}

			switch (aC->predMode)
			{
				case LEFTNEIGHBOUR:	LEFTNEIGHBOURprediction( pd); break;
				case PAETH:		PaethFilter( pd); break;
				case LMSPRED:	if (aC->useColPrediction_flag) 
											{
												if (aC->useColPredictionP_flag)
													lmsPredictionColP( pd, aC->LSpred_numOfPreds, aC->LSpred_searchSpace, 1);
												else
													lmsPredictionCol( pd, aC->LSpred_numOfPreds, aC->LSpred_searchSpace, 1);
											}
											else	lmsPrediction( pd, aC->LSpred_numOfPreds, aC->LSpred_searchSpace, 1);
											break;
				case LSBLEND:	lsZeroOrderPrediction( pd, aC->LSpred_numOfPreds, aC->LSpred_searchSpace, 
												aC->useCorrelation_flag, 1);  break;
				case LSPRED:	lsHighOrderPrediction( pd, aC->LSpred_numOfPreds, aC->LSpred_searchSpace, 1);  break;
				case MED:			MEDprediction( pd, 1);  break;
				case MEDPAETH:		MEDPAETHprediction( pd, 1);  break;
				case ADAPT:		ExternalPred( pd, extparam);	break;
				case H26x:		predictPicture( pd, TRUE, aC->H26x_Set); break;
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

		/* update max values	*/
		if (aC->predMode != NONE)
		{
			/* re-determine max values	*/
			unsigned int ii, mVal;
			pd->maxVal_pred[0] = 0;	/* init	*/
			pd->maxVal_pred[1] = 0;
			pd->maxVal_pred[2] = 0;
			for (chan = 0; chan < pd->channel; chan++)
			{
				mVal = pd->data[chan][0];
				for (ii = 1; ii < pd->size; ii++)
				{
					if (mVal < pd->data[chan][ii]) mVal = pd->data[chan][ii];
				}
				pd->maxVal_pred[chan] = mVal;
			}
		}

#ifdef Q_DEBUG
		if (aC->predMode != NONE)
		{
			FILE *out=NULL;
			char filename[512];


			sprintf( filename, "pre_err.ppm");
			out = fopen( filename, "wt");
			if (pd->channel ==1)		fprintf( out, "P2\n");
			else		fprintf( out, "P3\n");
			fprintf( out,"# created by TSIP_v2_16\n");
			fprintf( out,"%d %d\n", pd->width, pd->height);
			fprintf( out,"%d\n", 511);

			for ( i = 0; i < pd->size; i++) 
			{
				if ((i % pd->width) == 0)		fprintf( out, "\n");
				fprintf( out,"%5d ", pd->data[0][i]);
				if (pd->channel >= 3)
				{
					fprintf( out,"%5d ", pd->data[1][i]);
					fprintf( out,"%5d  ", pd->data[2][i]);
				}
			}
			fprintf( out,"\n");

			fclose( out);
		}
#endif

		/* undo the bit-depth fake	*/
		if (aC->coding_method == TSIP && pd->channel >= 3)
		{
			if (aC->interleaving_mode == YuvYuv || aC->interleaving_mode == YYuuvv)
			{
				/* restore original value	*/
				pd->bitperchan[0] = tmpA;
			}
			if (tmpC != tmpB)
			{
				pd->bitperchan[1] = tmpB;
				pd->bitperchan[2] = tmpC;
			}
		}

		if(aC->predMode == H26x)
		{
			// has been moved out of the main prediction routine because 
			// interleaving keeps overwriting the bitdepth.
			for (i = 0; i < pd->channel; i++)
			{
				if (aC->H26x_Set->newBitdepth[i] > pd->bitperchan[i])
				{
					pd->maxVal_rct[i] = (1 << aC->H26x_Set->newBitdepth[i]) - 1;
					pd->bitperchan[i] = aC->H26x_Set->newBitdepth[i];
				}
			}
			if (pd->channel >= 3)
				pd->bitperchannel = max(pd->bitperchan[0],pd->bitperchan[1]);
			else
				pd->bitperchannel = pd->bitperchan[0];
		}
	} /* 	if (aC->coding_method == LOCO || aC->coding_method == CoBaLP2 */


	/* determine entropy of components */
	iv->entropy[0] = (float)GetEntropy( pd->data[0], pd->size);
	if (pd->channel >= 3) 
	{
		iv->entropy[1] = (float)GetEntropy( pd->data[1], pd->size);
		iv->entropy[2] = (float)GetEntropy( pd->data[2], pd->size);
	}
	else
	{
		iv->entropy[1] = 0;
		iv->entropy[2] = 0;
	}

	/* automatic settings of HXC2 tolerances	*/
	//if ( aC->HXC2auto && aC->histMode != NoMOD)
	//{
	//	/* new computation if values are compacted */
	//	for( comp = 0; comp < pd->channel; comp++)
	//	{
	//		autoTolerances( pd, pd->maxVal_pred[comp]+1, aC, comp);
	//	}
	//}

	if (aC->coding_method == HXC2 || aC->coding_method == SCF)
	{
		if (aC->HXC2auto)
		{
			/* check tree-based approach first */
			if (iF->maxDepth + iF->numColours < 10000)
			{
#ifdef _DEBUG
				/* compare both calculations */
				//getHXC2_tolerances( pd, aC);
#endif
				if (aC->coding_method == HXC2) getHXC2_tolerances_tree( pd, aC, iF);
				if (aC->coding_method == SCF) 
				{
					getSCF_tolerances_HXY( pd, aC, iF); /* testing old tolerances*/
					//getSCF_tolerances_tree( pd, aC, iF);
				}
			}
			else
			{
				/* if possible stack overflow is detected than use other method	*/
				if (aC->coding_method == HXC2) 				getHXC2_tolerances( pd, aC);
				if (aC->coding_method == SCF) getSCF_tolerances_HXY( pd, aC, iF); /* */
			}
		}
		/* determine pattern_size based on tolerances	*/
		{
			unsigned int cnt;

			aC->pattern_size = 6;
			cnt = 0;
			for ( comp = 0; comp < pd->channel; comp++)
			{
				if (aC->HXC2tolerance[comp][4] >= pd->maxVal_pred[comp]  &&
						aC->HXC2tolerance[comp][5] >= pd->maxVal_pred[comp]) 
				{
					cnt++;
				}
			}
			if (cnt == pd->channel)
			{
				aC->pattern_size = 4;
				cnt = 0;
				for ( comp = 0; comp < pd->channel; comp++)
				{
					if (aC->HXC2tolerance[comp][2]>= pd->maxVal_pred[comp]  &&
							aC->HXC2tolerance[comp][3]>= pd->maxVal_pred[comp]) 
					{
						cnt++;
					}
				}
				if (cnt == pd->channel)
				{
					aC->pattern_size = 2;
				}
			} /* if (!flag) */
		} /* end of Block	*/
	}

	if (aC->coding_method == SCF)
	{
		if (aC->HXC2auto)
		{
			/* check tree-based approach first */
			if (iF->maxDepth + iF->numColours < 10000)
			{
				// getSCF_tolerances_tree( pd, aC, iF); /*takes only a subset of pixels into account */
			}
			else
			{
				/* if possible stack overflow is detected than use other method	*/
				// getSCF_tolerances( pd, aC); /*takes only a subset of pixels into account */
			}
		}
		/* determine pattern_size based on tolerances	*/
		{
			unsigned int cnt;

			aC->pattern_size = 6;
			cnt = 0;
			for ( comp = 0; comp < pd->channel; comp++)
			{
				if (aC->HXC2tolerance[comp][4] >= pd->maxVal_pred[comp]  &&
						aC->HXC2tolerance[comp][5] >= pd->maxVal_pred[comp]) 
				{
					cnt++;
				}
			}
			if (cnt == pd->channel)
			{
				aC->pattern_size = 4;
				cnt = 0;
				for ( comp = 0; comp < pd->channel; comp++)
				{
					if (aC->HXC2tolerance[comp][2]>= pd->maxVal_pred[comp]  &&
							aC->HXC2tolerance[comp][3]>= pd->maxVal_pred[comp]) 
					{
						cnt++;
					}
				}
				if (cnt == pd->channel)
				{
					aC->pattern_size = 2;
				}
			} /* if (!flag) */
		} /* end of Block	*/
	}

	if (1 && aC->coding_method == CoBaLP2)
	{
		double val;
		if (iF->syn > 100)
		{
			pd->wA = pd->wB = 0;
		}
		else 	
		{
			val = (100 - iF->syn) * 0.03; 
			pd->wA = pd->wB = (unsigned char)floor( val + 0.5);
		}
	}
	else
	{
		pd->wA = pd->wB = 0;
	}

	

	/*----------------------------------------------------------
	* Header befüllen und speichern
	*/
	bitbuf = BufferOpenWrite( h_file);
	ph = CreateHeader();
		ph->width  		  = pd->width;
		ph->height 		  = pd->height;
	ph->channel 	  = pd->channel;
	ph->transparency  = pd->transparency;
	ph->transparency_colour = pd->transparency_colour;

	/* ph->bitperchannel = pd->bitperchannel; ALeip */
	ph->bitperchan[0] = pd->bitperchan[0]; /* ALeip */
	ph->bitperchan[1] = pd->bitperchan[1]; /* ALeip */
	ph->bitperchan[2] = pd->bitperchan[2]; /* ALeip */
	ph->bitperchan[3] = pd->bitperchan[3]; /* ALeip */
	//ph->segmentWidth 	  = aC->segmentWidth;		
	//ph->segmentHeight 	= aC->segmentHeight;		
	ph->colour_space	= (uint)pd->space;
	if (ph->colour_space == INDEXED_GBR || ph->colour_space == INDEXED_LUM)
	{
		ph->indexed_adj = aC->indexed_adj;
		ph->indexed_arith = aC->indexed_arith;
	}
	ph->palette_sorting  = aC->palette_sorting;	
	ph->predMode		  = aC->predMode;
	ph->LSpred_searchSpace = aC->LSpred_searchSpace;
	ph->LSpred_numOfPreds  = aC->LSpred_numOfPreds;
	ph->useCorrelation_flag = aC->useCorrelation_flag;
	ph->useColPrediction_flag = aC->useColPrediction_flag;
	ph->postbwt		  = aC->postbwt;
	if (aC->postbwt == MTF)
		ph->pbwtparam	  = aC->MTF_val;
	else
		ph->pbwtparam	  = aC->IFCresetVal;
	ph->maxIFCCount	 = aC->IFCmaxCount;
	ph->skip_precoding	  = aC->skip_precoding;
	ph->interleaving_mode	  = aC->interleaving_mode;
	ph->coding_method	  = aC->coding_method; /* TSIP, LOCO, BPC, CoBaLP2, HXC */
	ph->entropy_coding_method	  = aC->entropy_coding_method;
	ph->separate_coding_flag	  = aC->separate_coding_flag;
	ph->skip_rlc1		= aC->skip_rlc1;
	ph->skip_postBWT	  = aC->skip_postBWT;
	ph->use_RLC2zero	= aC->use_RLC2zero;

	ph->histMode		= aC->histMode;

	ph->HXCtolerance = aC->HXCtolerance;
	ph->HXCtoleranceOffset1 = aC->HXCtoleranceOffset1;
	ph->HXCtoleranceOffset2 = aC->HXCtoleranceOffset2;
	ph->HXCincreaseFac = aC->HXCincreaseFac;
	ph->HXC2increaseFac = aC->HXC2increaseFac;
	ph->SCF_directional = aC->SCF_directional;
	ph->SCF_prediction = aC->SCF_prediction;
	ph->SCF_stage2tuning = aC->SCF_stage2tuning;
	ph->SCF_colourMAP = aC->SCF_colourMAP;
	ph->SCF_maxNumPatternsIdx = aC->SCF_maxNumPatternsIdx;

	ph->pattern_size = aC->pattern_size;

	{
		int ii, com;
		for( com= 0; com < 4; com++)
		{
			for( ii= 0; ii < 6; ii++)
			{
				ph->HXC2tolerance[com][ii] = aC->HXC2tolerance[com][ii];
			}
		}
	}

	ph->wA = pd->wA; /* CoBaLP2 init	*/
	ph->wB = pd->wB;
	
	ph->s_flg = aC->s_flg;	/* BPC*/
	ph->use_colFlag = aC->use_colFlag;	/* BPC*/
	ph->use_predFlag = aC->use_predFlag;	/* BPC*/

	for (chan = 0; chan < ph->channel; chan++)
	{
		//ph->maxVal_orig[chan]	= pd->maxVal_orig[chan];
		ph->maxVal_rct[chan]  	= pd->maxVal_rct[chan];
		ph->maxVal_pred[chan]  	= pd->maxVal_pred[chan];
		ph->offset[chan]  	= pd->offset[chan];
	}

	WriteHeader( ph, bitbuf);

	bytes->num_header_bytes = numBytes;
	numBytes_old = numBytes;

	/* Externer Header für Prädiktion */
	if (aC->predMode == ADAPT)
	{
		unsigned int numChan;
		/* fake the number of channela ignoring alpha	*/
		numChan = pd->channel;
		if ( pd->channel == 4) pd->channel = 3;
		else if ( pd->channel == 2) pd->channel = 1;

		WriteExtHeader( bitbuf, pd->channel);
		pd->channel = numChan;
	}	
	bytes->num_extPred_bytes = numBytes - numBytes_old;
	numBytes_old = numBytes;

	if (aC->predMode == H26x)
	{
		writeH26xSettings( aC->H26x_Set->config, bitbuf);
		evaluateLog( aC->H26x_Set, bitbuf, numBytes);
	}
	bytes->num_H26xPred_bytes = numBytes - numBytes_old;
	numBytes_old = numBytes;

	/*
	 * write LUT for histogram reconstruction into the bitstream #######
	 */
	if (aC->histMode == COMPACTION)
	{
		int q, k;

		/* write information to stream if required
		 * one bit more because if len =256 then 8 bits are not enough
		 */
		WriteValToStream( bitbuf, idx2val_len[0], pd->bitperchan[0] + 1);
		if (idx2val_len[0])
		{
			int N, A;
			rice_encoding( bitbuf, idx2val0[0], pd->bitperchan[0]-1);
			/* the higher the cpmpaction, the higher the differences, the higher k must be	*/
			q = (int)(1 << pd->bitperchan[0]) / idx2val_len[0];
			//k = 0;
			//while(q){k++; q = q >> 1;};	/*determine k	*/
			N = 1;
			A = q;
			for ( i = 1; i < idx2val_len[0]; i++)
			{
				int diff;
				{
					register int nst = N, At = A;
					for ( k = 0; nst < At; nst <<= 1, k++);
				}
				/* in compaction mode, entries are still in ascending order
				* ==> transmission of differences is more efficient
				*/
				diff = idx2val0[i]-idx2val0[i-1];
				rice_encoding( bitbuf, diff, k);
				N++;
				A += diff;
			}
		}

		if (pd->channel >= 3)
		{
			WriteValToStream( bitbuf, idx2val_len[1], pd->bitperchan[1] + 1);
			if (idx2val_len[1])
			{
				int N, A;
				rice_encoding( bitbuf, idx2val1[0], pd->bitperchan[1]-1);
				q = (int)floor( (double)(1u << pd->bitperchan[1]) / idx2val_len[1]);
				N = 1;
				A = q;
				//k = 0;
				//while(q){k++; q = q >> 1;};	/*determine k	*/
				for ( i = 1; i < idx2val_len[1]; i++)
				{
					int diff;
					{
						register int nst = N, At = A;
						for ( k = 0; nst < At; nst <<= 1, k++);
					}
					diff = idx2val1[i] - idx2val1[i-1];
					rice_encoding( bitbuf, diff, k);
					N++;
					A += diff;
				}
			}
			WriteValToStream( bitbuf, idx2val_len[2], pd->bitperchan[2] + 1);
			if (idx2val_len[2])
			{
				int N, A;
				rice_encoding( bitbuf, idx2val2[0], pd->bitperchan[2]-1);
				q = (int)floor( (double)(1u << pd->bitperchan[2]) / idx2val_len[2]);
				N = 1;
				A = q;
				//k = 0;
				//while(q){k++; q = q >> 1;};	/*determine k	*/
				for ( i = 1; i < idx2val_len[2]; i++)
				{
					int diff;
					{
						register int nst = N, At = A;
						for ( k = 0; nst < At; nst <<= 1, k++);
					}
					diff = idx2val2[i] - idx2val2[i-1];
					rice_encoding( bitbuf, diff, k);
					N++;
					A += diff;
				}
			}
		}
		FREE( idx2val0);
		FREE( idx2val1);
		FREE( idx2val2);
	}
	else if (aC->histMode == PERMUTATION || aC->histMode == PERMUTE_2OPT)
	{
		int q, k;

		/* write information to stream if required
		* one bit more because if len =256 then 8 bits are not enough
		*/
		WriteValToStream( bitbuf, idx2val_len[0], pd->bitperchan[0] +1);
		if (idx2val_len[0])
		{
			int N, A;
			// WriteFiboToStream( bitbuf, idx2val0[0]);
			rice_encoding( bitbuf, idx2val0[0], pd->bitperchan[0]-1);
			/* the higher the compaction, the higher the differences, the higher k must be	*/
			q = (int)(1 << pd->bitperchan[0]) / idx2val_len[0];
			//k = 0;
			//while(q){k++; q = q >> 1;};	/*determine k	*/
			N = 1;
			A = q;
			for ( i = 1; i < idx2val_len[0]; i++)
			{
				int absDiff;
				{
					register int nst = N, At = A;
					for ( k = 0; nst < At; nst <<= 1, k++);
				}
				/* in permutation mode, entries are not in ascending order
				* ==> transmission of differences  plus sign
				*/
				absDiff =  abs( (int)idx2val0[i]-(int)idx2val0[i-1]);
				rice_encoding( bitbuf, absDiff, k);
				N++;
				A += absDiff;
				if (idx2val0[i] > idx2val0[i-1])
				{
					BufferPutBit( bitbuf, 0);
				}
				else
				{
					BufferPutBit( bitbuf, 1);
				}
			}
		}

		if ( pd->channel >= 3)
		{
			WriteValToStream( bitbuf, idx2val_len[1], pd->bitperchan[1]+1);
			if (idx2val_len[1])
			{
				int N, A;
				rice_encoding( bitbuf, idx2val1[0], pd->bitperchan[1]-1);
				q = (int)floor( (double)(1u << pd->bitperchan[1]) / idx2val_len[1]);
				N = 1;
				A = q;
				//k = 0;
				//while(q){k++; q = q >> 1;};	/*determine k	*/
				for ( i = 1; i < idx2val_len[1]; i++)
				{
					int absDiff;
					{
						register int nst = N, At = A;
						for ( k = 0; nst < At; nst <<= 1, k++);
					}
					absDiff = abs( (int)idx2val1[i]-(int)idx2val1[i-1]);
					rice_encoding( bitbuf, absDiff, k);
					N++;
					A += absDiff;
					if (idx2val1[i] > idx2val1[i-1])
					{
						BufferPutBit( bitbuf, 0);
					}
					else
					{
						BufferPutBit( bitbuf, 1);
					}
				}
			}
			WriteValToStream( bitbuf, idx2val_len[2], pd->bitperchan[2]+1);
			if (idx2val_len[2])
			{
				int N, A;
				rice_encoding( bitbuf, idx2val2[0], pd->bitperchan[2]-1);
				q = (int)floor( (double)(1u << pd->bitperchan[2]) / idx2val_len[2]);
				N = 1;
				A = q;
				//k = 0;
				//while(q){k++; q = q >> 1;};	/*determine k	*/
				for ( i = 1; i < idx2val_len[2]; i++)
				{
					int absDiff;
					{
						register int nst = N, At = A;
						for ( k = 0; nst < At; nst <<= 1, k++);
					}
					absDiff = abs( (int)idx2val2[i]-(int)idx2val2[i-1]);
					rice_encoding( bitbuf, absDiff, k);
					N++;
					A += absDiff;
					if (idx2val2[i] > idx2val2[i-1])
					{
						BufferPutBit( bitbuf, 0);
					}
					else
					{
						BufferPutBit( bitbuf, 1);
					}
				}
			}
		}
		FREE( idx2val0);
		FREE( idx2val1);
		FREE( idx2val2);
	}
	bytes->num_histMode_bytes = numBytes - numBytes_old;
	numBytes_old = numBytes;


	/* 
	 * transmit palette ########################################
	 */
	if (pd->space == INDEXED_GBR || pd->space == INDEXED_LUM)
	{
		WriteFiboToStream( bitbuf, iF->numColours); /* is already transmitted as maxVal_rct */

		/* threshold must be refined by more tests	*/
		if (aC->palette_sorting == 0)
		{
			/* expects special order of colours 
			* => monotonic increase of green component etc.
			*/
			write_palette( bitbuf, iF->numColours, palette);
		}
		else
		{
			/* ensures non-negative differences by several runs through data	*/
			write_palette_2( bitbuf, iF->numColours, palette); /* version from PVL KTKI10 Team Wulffpack	*/
		}

#ifdef Q_DEBUG_XX
		{
			int val_old, val;
			FILE *out=NULL;
			char filename[512];


			sprintf( filename, "indices_%d_en.pgm", pd->tileNum);
			out = fopen( filename, "wt");
			fprintf( out,"P2\n");
			fprintf( out,"# created by %s\n", thisVersion);
			fprintf( out,"%d %d\n", pd->width, pd->height);
			fprintf( out,"%d", pd->maxVal_pred[0]);

			for ( i = 0; i < pd->size; i++) 
			{
				if ((i % pd->width) == 0)		fprintf( out, "\n");
				fprintf( out,"%5d ", pd->data[0][i]);
			}
			fprintf( out,"\n");
			fclose( out);

			sprintf( filename, "palette_%d_en.txt", pd->tileNum);
			out = fopen( filename, "wt");
			fprintf( out,"# created by %s\n", thisVersion);
			fprintf( out,"# index |  R   G   B\n");

			val_old = 0;
			for ( i = 0; i < iF->numColours; i++) 
			{
				val =(palette[i*3+2]<<16) + (palette[i*3+1]<<8)+ palette[i*3];
				fprintf( out,"%5d: %5d %5d %5d\n", i, 
					palette[i*3+1], palette[i*3+2], palette[i*3]
				);
				val_old = val;
			}
			fprintf( out,"\n");

			fprintf( out,"\n Coding of palette took %d bytes\n", numBytes - numBytes_old); 
			fclose( out);
		}
#endif


	} /* if (pd->space == INDEXED_GBR || */

	bytes->num_palette_bytes = numBytes - numBytes_old;
	numBytes_old = numBytes;

	/* reset intermediate values, for evaluation only */
	for (comp = 0; comp < 3; comp++)
	{						
		iv->data_len[comp] = 0;
		iv->RLC1_len[comp] = 0;
		iv->RLC1_marker[comp] = 0;
		iv->RLC2_len[comp] = 0;
		iv->RLC2_marker[comp] = 0;
	}

	fprintf( stderr, "\r do the encoding ...         ");
	{
		unsigned int tmp_channel = 1;

		bytes->num_quad_bytes = 0; // reset for all but SCF
		//if ( ph->coding_method != HXC2) use always separate method for encoding the alpha channel	*/
		{
			/* do some modification to prevent the encoding of the alpha channel
			 * alpha channel will be encoded afterwards using BPC or HXC
			 */
			tmp_channel = pd->channel;
			if (pd->channel == 2) pd->channel = 1;
			else if (pd->channel == 4) pd->channel = 3;
		}
		/*
		If adjacency mapping with arithmetic coding is ticked,
		only the arithmetic coding is applied without any
		pre-coding stage. Also this variant of arithmetic coding
		uses more models than the 'standard' one.
		*/
		if (aC->indexed_flag && aC->indexed_adj && aC->indexed_arith)
		{
			AC_adj( pd, iF->numColours, pd->data[0], bitbuf);
		}
		else if (aC->coding_method == LOCO) /* LOCO encoding mode */
		{
			fprintf( stderr, " LOCO ");
			BufferPutBit( bitbuf, (unsigned char)aC->T4_flag);
			BufferPutBit( bitbuf, (unsigned char)aC->locoImproveFlag);
			locoEncoder( pd, pd, aC->T4_flag, aC->locoImproveFlag, bitbuf);
		}
		else if (aC->coding_method == BPC) /* bit plane coding	*/
		{
			fprintf( stderr, " BPC ");
			cbool g_flg = true;	/* convert binary to gray code	*/
			cbool p_flg;	/* prediction ?	*/
			cbool c_flg;	/* colour transform ?	*/
			if ( aC->predMode == NONE) p_flg = false;
			else		 p_flg = true;
			if (pd->space == GREY || pd->space == RGBc || 
					pd->space == INDEXED_GBR || pd->space == INDEXED_LUM)
							c_flg = false;
			else		 c_flg = true;

			if (!aC->use_colFlag) c_flg = 0;	/* ignore	*/
			if (!aC->use_predFlag) p_flg = 0;	/* ignore	*/
			BP_coding( bitbuf, g_flg, p_flg, c_flg, pd, 1, aC->s_flg); /* encoding	*/
		}
		else if (aC->coding_method == HXC) /* list coding based on H(X|C)	*/
		{
			fprintf( stderr, " HXC ");
			//if (pd->space == INDEXED_COL && aC->predMode == NONE) /* Batev: Indexed mode */
			if (aC->indexed_flag && aC->predMode == NONE) /* Batev: Indexed mode */
			{
				HXC_codingIdx( pd, bitbuf, aC, 1, 0u, /* iF->numColours, */ palette);
			}
			else													/* Batev: RGB or YUV color space mode */
			{
				HXC_coding( pd, bitbuf, aC, 1, 0u);
				if (pd->channel >= 3)
				{
					HXC_coding( pd, bitbuf, aC, 1, 1u);
					HXC_coding( pd, bitbuf, aC, 1, 2u);
				}
			}
		}
		else if (aC->coding_method == HXC2) /* list coding based on H(X|C)	*/
		{
			fprintf( stderr, " HXC2");
			HXC2_coding( pd, bitbuf, aC, 1);
		}
		else if (aC->coding_method == SCF) /* list coding based on H(X|C)	*/
		{
			fprintf( stderr, " SCF");
			//SCF_coding( pd, bitbuf, aC, 1, &(bytes->num_quad_bytes));
			//PM_MA improvement 01 count number of colours, also pass header in case of roatation. (Decoder must know if it should rotate image. Also in case of 90 or 270 degree
			//rotation encoder must change x and y orientation of image
			SCF_coding( pd, bitbuf, aC, 1, &(bytes->num_quad_bytes), ph);
			
		}
		else if (aC->coding_method == CoBaLP2) /* CoBaLP2 encoding mode */
		{
			int err;
			float wA, wB;
			fprintf( stderr, " CoBaLP2 ");
			wA = pd->wA *0.1F;
			wB = pd->wB *0.1F;
			err = cobalbWrapper( pd, bitbuf, 1, aC->CoBaLP2_sT, aC->CoBaLP2_sA, 
				aC->CoBaLP2_sB, aC->CoBaLP2_aR, aC->CoBaLP2_aL, wA, wB, &(aC->CoBaLP2_constLines), 
				pd->numOfConstantLines, iv);
		}
		else	/* TSIP mode */
		{
			fprintf( stderr, " TSIP ");
			TSIP_enc( pd, bitbuf, aC, iv);
		} /* if TSIP */

		//if ( ph->coding_method != HXC2)
		{
			/* undo the fake of channels	*/
			pd->channel =	tmp_channel;
		}


		/* substract possible  quad_bytes from SCF method	*/
		bytes->num_data_bytes = numBytes - numBytes_old - bytes->num_quad_bytes;
		numBytes_old = numBytes;

#ifdef Q_DEBUG_XX
	if (pd->transparency )	
	{
		FILE *out=NULL;
		char filename[512];


		if (pd->channel <= 2) chan = 1;
		else chan = 3;

		sprintf( filename, "alpha_%d.pgm", pd->tileNum);
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

		/* encoding of the alpha channel if required	*/
		if (pd->transparency == 2 /* && aC->coding_method != HXC2 */ )
		{
			unsigned int *ptr_tmp;
			unsigned long maxCount, numColours; 
			unsigned long tmp_offset, tmp_maxVal_pred; 

			if (pd->space == GREY || pd->space == INDEXED_GBR || pd->space == INDEXED_LUM)
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

			/* get number of different alpha values	*/
			{
				double colEntropy;
				numColours = GetNumColours( pd, &maxCount, &colEntropy);
			}
			//if (numColours >= 200)
			//{
			//	int err;
			//	//locoEncoder( pd, pd, 1, bitbuf);
			//	err = cobalbWrapper( pd, bitbuf, 1, 1, 0, 
			//		0, 0, &(aC->CoBaLP2_constLines), iv);
			//}
			//else 
			if (numColours > 20 && numColours < 70)
			{
				AutoConfig aC2;
				aC2.HXCtolerance = 0;
				aC2.HXCtoleranceOffset1 = 0;
				aC2.HXCtoleranceOffset2 = 0;
				aC2.HXCincreaseFac = 2;

				BufferPutBit( bitbuf, 1);
				HXC_coding( pd, bitbuf, &aC2, 1, 0);
			}
			else	
			{
				BufferPutBit( bitbuf, 0);
				/* no Gray code, not prediction, no colour transform, significance_flag=1	*/
				BP_coding( bitbuf, 0, 0, 0, pd, 1, 1); /* encoding	*/
			}
			pd->data[0] = ptr_tmp;
			pd->channel = tmp_channel;
			pd->offset[0] = tmp_offset;
			pd->maxVal_pred[0] = tmp_maxVal_pred;
		}
	}

	//PM_MA delete just here, it's needed in SCF
	DeleteHeader( ph);

	if (aC->indexed_flag)
	{
		/* undo the fake of number of channels for indexed mode	*/
		pd->channel = 3; /* restore original value, required for FREE */
		if (pd->transparency == 2)pd->channel = 4;
	}

	if (aC->indexed_flag) FREE( palette);

	BufferClose( bitbuf);
	bytes->num_alpha_bytes = numBytes - numBytes_old;

#ifdef Q_DEBUG_X
	fclose( debug_out);
#endif
}
