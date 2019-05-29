/*****************************************************
* File..: scf.c
* Desc..: implementation of screen content coder
* Author: T. Strutz
* Date..: 03.11.2015
* changes:
* 09.11.2015 common structure for encoder and decoder
* 09.11.2015 new: pattern_flags
* 10.11.2015 new: SCF_directional
* 19.11.2015 function moved to hist_ops_SCF.c
 * 30.11.2015 new: SCF_prediction, stage2tuning, SCF_colourMAP
 * 04.12.2015 directional mode also if both top and left pattern are identical
******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "scf.h"
#include "types.h"
#include "codec.h"	/* for min( a, b);	*/
#include "ginterface.h"
#include "bitbuf.h"
#include "arithmetic.h"
#include "paeth.h"
#include "header.h"
//PM_MA new includes
#include "tile.h"
#include "stats.h"
#include "imageFeatures.h"
#include "pdconvert.h"
#using <System.Drawing.dll>

#ifdef _DEBUG
 //#define H_DEBUG
	unsigned int r_glob;
#endif

#ifdef HXC2_EST_INF
/* already in hxc2.c	*/
extern double inf_stage1, inf_stage2, inf_stage3;
extern unsigned long cnt_stage1, cnt_stage2, cnt_stage3;
unsigned long cnt_stage0;
#define invLOG2	1.442695041  /* 1 / ln(2.)  */
#endif

#define OUT_MODE
//
#undef OUT_MODE
#define MED_PRED
//#undef MED_PRED
/* Errechnet den Schätzwert für die Prädiktion */
//PM_MA from external PVL
//replace old GetMAPVal with new one and diagonal detection
/*
MYINLINE long GetMAPVal( PicData *pd, uint chan, uint x, uint y,
												long *A, long *B, long *C, long *D, 
												long *E, long *F, long *P)
{
	static long ca, cb, cc, Xhat;

	// C B
	// A X
	//

	if (x == 0)
	{
		ca = 0; 
	}
	else
	{
		ca = (int)*GetXYValue(pd, chan, x - 1u, y);
		if (x == 1) *E = ca;
		else		*E = (int)*GetXYValue(pd, chan, x - 2u, y);
	}
	if (y == 0)
	{
		cb = 0;
	}
	else
	{
		cb = (int)*GetXYValue(pd, chan, x, y - 1u);
		if (y == 1) *F = cb;
		else		*F = (int)*GetXYValue(pd, chan, x, y - 2u);
	}
	if (x == 0 || y == 0) cc = 0;
	else cc = (int)*GetXYValue(pd, chan, x - 1u, y - 1u);
	if ( (x == pd->width-1) || y == 0) *D = 0;
	else *D = (int)*GetXYValue(pd, chan, x + 1u, y - 1u);

	if (cc <= ca)
	{
		if (cc <= cb)
		{
			if (ca > cb)
			{
				Xhat = ca; // C <= B < A	
			}
			else
			{
				Xhat = cb; // C <= A <= B	
			}
		}
		else
		{
			// B < C <= A	
			Xhat = ca + cb - cc;
		}
	}
	else
	{
		if (cc >= cb)
		{
			if (ca < cb)
			{
				Xhat = ca; // A < B <= C	
			}
			else
			{
				Xhat = cb; // B <= A <= C	
			}
		}
		else
		{
			// A < C < B	
			Xhat = ca + cb - cc;
		}
	}
	*A = ca;
	*B = cb;
	*C = cc;
	*P = ca + cb - cc;
	return Xhat;	
}
*/

//new:
MYINLINE long GetMAPVal( PicData *pd, uint chan, uint x, uint y,
												long *A, long *B, long *C, long *D, 
												long *E, long *F, long *P)
{
	static long ca, cb, cc, cd, Xhat;
	//thresholds
	int t1 = 20;
	int t2 = 8;
	//to avoid range violations (will be added to possible negative values)

	// C B D
	// A X
	//

	if (x == 0)
	{
		ca = 0; 
	}
	else
	{
		ca = (int)*GetXYValue(pd, chan, x - 1u, y);
		if (x == 1) *E = ca;
		else		*E = (int)*GetXYValue(pd, chan, x - 2u, y);
	}
	if (y == 0)
	{
		cb = 0;
	}
	else
	{
		cb = (int)*GetXYValue(pd, chan, x, y - 1u);
		if (y == 1) *F = cb;
		else		*F = (int)*GetXYValue(pd, chan, x, y - 2u);
	}
	if (x == 0 || y == 0) cc = 0;
	else cc = (int)*GetXYValue(pd, chan, x - 1u, y - 1u);
	if ( (x == pd->width-1) || y == 0) {*D = 0; cd = *D;}
	else {*D = (int)*GetXYValue(pd, chan, x + 1u, y - 1u); cd = *D;}
	//fprintf(stderr, "\ncd = %d", cd);


	if (cc <= ca)
	{
		if (cc <= cb)
		{
			if (ca > cb)
			{
				Xhat = ca; // C <= B < A	
			}
			else
			{
				Xhat = cb; // C <= A <= B	
			}
		}
		else
		{
			// B < C <= A	
			Xhat = ca + cb - cc;
		}
	}
	else
	{
		if (cc >= cb)
		{
			if (ca < cb)
			{
				Xhat = ca; // A < B <= C	
			}
			else
			{
				Xhat = cb; // B <= A <= C	
			}
		}
		else
		{
			// A < C < B	
			Xhat = ca + cb - cc;
		}
	}

	//new introduced with PVL
	if(cc >= max(ca, cb))
	{
		//diagonal exists?
		if ((cc - max(ca,cb)) > t1 && (abs (cb-ca) <= t2 ))
		{
			//proposal, instead of using maxval, check value of cb in comparison to ca + cds
			//if(cb >= (ca + cd) && (ca + cd - cb) <= 255)
			{
				Xhat = ca + cd - cb;
				//Xhat = cb + cd - ca;
			}
		}
	}
	else if(cc <= min(ca, cb))
	{
		//diagonal exists?
		if ((min (ca, cb)-cc) > t1 &&(abs(cb-ca) <= t2 ))
		{
			//proposal, instead of using maxval, check value of cb in comparison to ca + cd
			//if(cb >= (ca + cd) && (ca + cd - cb) <= 255)
			{
				Xhat = ca + cd - cb;
				//Xhat = ca + cb - cc;
			}
		}
	}

	//check if values of Xhat are in range, if not (higher than 255 or lower than 255) correct values
	//PM_MA put values in range if they're out of it

	//wird deutlich schlechter!
	/*
	if(Xhat < 0)Xhat = 0;
	if(Xhat > 255) Xhat = 255;
	*/
	/*
	if ( Xhat > ( maxvalue / 2))
		Xhat = Xhat - ( maxvalue + 1);
	else
	{
		if ( Xhat < -(( maxvalue + 1) / 2))
			Xhat = Xhat + ( maxvalue + 1);
	}
	*/
	//avoid negative values --> falsch???
	//Xhat += ( maxvalue + 1) / 2; 
	//PM_MA Xhat auf 0 setzen, falls negativ
	//if(Xhat < 0)Xhat = 0;
	

	*A = ca;
	*B = cb;
	*C = cc;
	*D = cd;
	*P = ca + cb - cc;
	return Xhat;	
}

/*--------------------------------------------------------
* SCF_coding()
*--------------------------------------------------------*/
void SCF_coding( PicData *pd,		/* structure with image data	*/
								BitBuffer *bitbuf, /* interface for file input and output*/
								AutoConfig *aC,  /* parameter of codec */
								int encoder_flag,	/* 1 ==> encoder; 0=> decoder	*/
								unsigned int *quad_bytes, PicHeader *ph)
{
	int X_in_merged_hist_flag = 0; /* 1 ... current colour X is in the merged histogram	*/
	int no_new_pattern_flag;
	int stage = 0;
	long bias[4];
	long **t_actual;	/* template of actual pattern	*/
	long **t_plist	/* template from pattern	list */;
	long **tolerance;	/* tolerence in pattern comparison	*/
	long X[4]; /* colour vector at current position	*/
	long pred_err = 0; /*  prediction error */
//	long predTol[4]; /* average prediction error vector	*/
	long Xhat_MAP[4]; /* esimate from MED prediction	*/
	long Xhat_Merged[4]; /* esimate from merged_stacked_histogram	*/
	long Av[4], Bv[4];	/* colour vectors at A and B positions	*/
	int pred_mode;	/* prediction mode in MED prediction	*/
	int increaseTolMode;
	unsigned char *mode_arr=NULL; /* decision between prediction and colour based coding	*/
	/* for debugging:	*/
	unsigned char *stage_arr=NULL; /* pattern flags	*/
	unsigned char *flags_arr=NULL; /* pattern flags	*/
	unsigned int *inf_arr=NULL; /* spent bpp * 1000	*/

	unsigned char pattern_flags; /* represents best pattern found, not further used, is it?	*/
	unsigned int comp; /* number of channel or component	*/
	unsigned int r, c; /* row an d column	*/
	/* for re-scaling the count of the ESC symbol in pattern lists histograms */
	unsigned int maxCountESC;
	unsigned int increaseFac; /* scaling of counts in merged stacked histogram	*/
	/* for detection of vertical and horizontal structures	*/
	unsigned int verticalHit_count, horizontalHit_count;
	unsigned int verticalHitModificationFlag = 0;
	unsigned int horizontalHitModificationFlag = 0;
	unsigned long *patt_arr=NULL; /* saves pattern positions	*/
	/* used when updating the counts in the pattern-list histograms:	*/
	unsigned int rescaling_threshold;
	/* ?? */
	long stage2_symbol0_hitCount, stage2_symbol0_totCount, stage2_symbol0_offset = 0;
	long stage1_symbol0_hitCount, stage1_symbol0_totCount;
	unsigned long *(Hist[4]), K_Hist[4];	/* zero order histograms, sizes	*/
//	unsigned long countAbsPred_err=0, sumAbsPred_err=0, meanAbs = 0; /* for stage 2 grey images	*/
	unsigned long i;
	//PM_MA improvement 01 count number of colours added pos_new for rotation
	unsigned long pos; /* current position in image	*/
	unsigned long numLists;
	unsigned long symbol = 0; /* suppress warning, symbol to be encoded/decoded	*/
//	unsigned long sum_abs_err[4] , num_abs_err;

	/* stage 2 variables	*/
	long ABC=0, A=0, B=0, C=0, D=0, E=0, F=0, P=0, ABCD, min_abs_err;
	//long AE=0, BF=0;
	unsigned long bytes_old; /* temporary number of written bytes	*/
	unsigned long numOfPaletteItems; /* refers to palette	*/
//	unsigned long *HperrMAP=NULL, HperrResetCnt; /* histogram of absolute errors of	MAP */
//	unsigned long *HperrMRG=NULL; /* histogram of absolute errors of	Merged */

	/* stage 3 variables	*/
	//int colourMapSuccess = 0;
	//int colourMapFailure = 0;

	//PM_MA for improved stage 2 tuning
	unsigned int rotateMode = 0;
	unsigned int paletteSizeStage2Tuning = 32;
	unsigned int diffMaxCalculated = 11; //  ph->SCF_diffMaxCalculated;

	unsigned int rotationInfo_bool_01 = 0; //  ph->SCF_rotationInfo_bool_01;
	unsigned int rotationInfo_bool_02 = 0; //  ph->SCF_rotationInfo_bool_02;
	unsigned int paletteSizeStage2Tuning_bool_01 = 0; //  ph->SCF_paletteSizeStage2Tuning_bool_01;
	unsigned int paletteSizeStage2Tuning_bool_02 = 0; // ph->SCF_paletteSizeStage2Tuning_bool_02;
	//unsigned int diffMaxCalculated_bool_01 = ph->SCF_diffMaxCalculated_bool_01;
	//unsigned int diffMaxCalculated_bool_02 = ph->SCF_diffMaxCalculated_bool_02;
	//unsigned int diffMaxCalculated_bool_03 = ph->SCF_diffMaxCalculated_bool_03;

	if(rotationInfo_bool_01 == 0 && rotationInfo_bool_02 == 0){
		rotateMode = 0;
	}

	if(rotationInfo_bool_01 == 0 && rotationInfo_bool_02 == 1){
		rotateMode = 1;
	}

	if(rotationInfo_bool_01 == 1 && rotationInfo_bool_02 == 0){
		rotateMode = 2;
	}

	if(rotationInfo_bool_01 == 1 && rotationInfo_bool_02 == 1){
		rotateMode = 3;
	}

	if(paletteSizeStage2Tuning_bool_01 == 0 && paletteSizeStage2Tuning_bool_02 == 0){
		paletteSizeStage2Tuning = 64;
	}

	if(paletteSizeStage2Tuning_bool_01 == 0 && paletteSizeStage2Tuning_bool_02 == 1){
		paletteSizeStage2Tuning = 64;
	}

	if(paletteSizeStage2Tuning_bool_01 == 1 && paletteSizeStage2Tuning_bool_02 == 0){
		paletteSizeStage2Tuning = 128;
	}


	if(rotationInfo_bool_01 == 1 && rotationInfo_bool_02 == 1){
		rotateMode = 3;
	}

	/*
	if(diffMaxCalculated_bool_01 == 0 && diffMaxCalculated_bool_02 == 0 && diffMaxCalculated_bool_03 == 0)
	{
		diffMaxCalculated = 4;
	}

	if(diffMaxCalculated_bool_01 == 0 && diffMaxCalculated_bool_02 == 0 && diffMaxCalculated_bool_03 == 1)
	{
		diffMaxCalculated = 6;
	}

	if(diffMaxCalculated_bool_01 == 0 && diffMaxCalculated_bool_02 == 1 && diffMaxCalculated_bool_03 == 0)
	{
		diffMaxCalculated = 10;
	}

	if(diffMaxCalculated_bool_01 == 0 && diffMaxCalculated_bool_02 == 1 && diffMaxCalculated_bool_03 == 1)
	{
		diffMaxCalculated = 12;
	}

	if(diffMaxCalculated_bool_01 == 1 && diffMaxCalculated_bool_02 == 0 && diffMaxCalculated_bool_03 == 0)
	{
		diffMaxCalculated = 14;
	}
	*/

	//read from header and translate bits to concrete numbers
	//fprintf(stderr, "\nrotateMode = %d", rotateMode);
	//fprintf(stderr, "\nSCF_paletteSizeStage2Tuning = %d", paletteSizeStage2Tuning);
	//fprintf(stderr, "\nSCF_diffMaxCalculated = %d", diffMaxCalculated);

	//system("Pause");
	//exit(EXIT_SUCCESS);

	//simulation end

	//try relative value
	//paletteSizeStage2Tuning = 0.1 * numOfColoursTotal;


	//PM_MA improvement 01 count number of colours begin
	/*principle: using the tiles.c functionality to get tiles of the image to decide whether the image has to be rotated before starting the SCF. 
	  For each image 4 tiles are created. By default they have the size of 1/8 of the image. So there are 4 tiles:
	  1. the upper horizontal part of the image, representing an image not rotated (0 degree).
	  2. the bottom horizontal part of the image, representing an image which is rotated by 180 degree
	  3. the left vertical part of the image, representing an image which is rotated by 90 degree.
	  4. the right vertical part of the image, representing an image which is rotated by 270 degree.
	  Keep in mind that those are only representations and we want to count the colorus of each tile. The decision if an image should be rotated before start is being made based upon
	  following criterion: find out which of the tiles has the minimum number of colours. If e.g. the bottom horizontal tile has the lowest number of all 4 tiles, rotate image by 180 degree
	  before compressing.*/


	//for testing purposes bool if rotation is switched on or not
	bool roatateFlag = true;

	if(roatateFlag == true && encoder_flag == 1){
		
		rotateImage(pd, rotateMode, encoder_flag);

	}
	//PM_MA improvement 01 count number of colours end

	/* pointer to array of pointers to single list elements	*/
	PATTERN_LIST_SCF *pat_list; /* top of dictionary (pattern list) */
	HIST_LIST_ELEM2 merged_stacked_histogram;
	HIST_LIST_ELEM2 palette_obj, *palette;
	EXCHANGE_STRUC_SCF exchange;
	PATTERN_REMOVE_SCF patternRemove;

	acModell M;	/* normal AC context	*/
	acState acState_obj, *acSt;	/* state of arithmetic coder	*/

#ifdef OUT_MODE
	int mode_hxc2 = 0;
	FILE *mode_file=NULL;
	//mode_file = fopen( "mode_hxc2.pgm", "wb");
	// fprintf( mode_file, "P2\n%d %d\n2\n", pd->width, pd->height);
	mode_file = fopen( "estimate.ppm", "wt");
	fprintf( mode_file, "P3\n%d %d\n%d\n", pd->width, pd->height, 255);
#endif

	acSt = &acState_obj;
	palette = &palette_obj;

	/* prepare data for possible removal of pattern list elements	*/
	patternRemove.patternlist_flag = 0;
	patternRemove.CurrentElementAddr = NULL;
	patternRemove.PreviousElementAddr = NULL;

#ifdef HXC2_EST_INF
	inf_stage1 = inf_stage2 = inf_stage3 = 0.0;
	cnt_stage0 = cnt_stage1 = cnt_stage2 = cnt_stage3 = 0;
#endif

	/* default colour	*/
	palette->count = 0;
	palette->next = NULL;
	palette->value[0] = -1;
	palette->value[1] = -1;
	palette->value[2] = -1;
	palette->value[3] = -1;
	numOfPaletteItems = 1;

	assert( aC->HXC2increaseFac >= 0);


	/* start with empty pattern list, +1 because 0 has to be included */
#ifdef PAT_RGB
	numLists = QUANT_FEAT_1 * QUANT_FEAT_2 * QUANT_FEAT_3 *
						 QUANT_FEAT_4 * QUANT_FEAT_5 * QUANT_FEAT_0; 
#else
	numLists = PAT_QUANT_M * PAT_QUANT_D; 
#endif

	assert( numLists < 5000000);

	/* six features á PAT_QUANT different values	*/
	ALLOC( pat_list, numLists, PATTERN_LIST_SCF);
	if (ERRCODE == MALLOC_FAILED)
	{
		goto endSCF;
	}


	/* reset all allocated lists	*/
	for (i = 0; i < numLists; i++)
	{
		(pat_list[i]).pattern_list_size = 0;
		(pat_list[i]).top.next = NULL;
//		(pat_list[i]).top.prev = NULL;
		(pat_list[i]).top.top_hist_elem = NULL; /* empty histogramm list	*/
		(pat_list[i]).top.r = 0;
		(pat_list[i]).top.c = 0;
	}

	/* allocate zero order histograms for all channels	*/
	for (comp = 0; comp < pd->channel; comp++)
	{
		K_Hist[comp] = pd->maxVal_pred[comp] + 1;
		CALLOC( Hist[comp], K_Hist[comp]+1, unsigned long);
		if (Hist[comp] == NULL)
		{
			goto endSCF;
		}
	}

	//HperrResetCnt = 1; /* not zero because of division somewhere	*/
	//CALLOC( HperrMRG, MAX_MAP_ERR+1, unsigned long);
	//if (HperrMRG == NULL)
	//{
	//	goto endSCF;
	//}
	//CALLOC( HperrMAP, MAX_MAP_ERR+1, unsigned long);
	//if (HperrMAP == NULL)
	//{
	//	goto endSCF;
	//}

	/* allocate template for pattern (current + corresponding to pattern list	*/
	t_actual = imatrix( MAX_PATTERN_SIZE, 4);
	t_plist = imatrix( MAX_PATTERN_SIZE, 4);
	tolerance = imatrix( MAX_PATTERN_SIZE, 4);

	/* pre init
	*     F
	*   C B D
	* E A X
	*/
	/* tolerance defines the maximum difference between pixels of current template and
	* pixels of reference template. If difference is higher than tolerance, then
	* reference template is NOT regarded as match
	*/
	maxCountESC = 0; /* 	*/
	for (comp = 0; comp < pd->channel; comp++)
	{
		for (c = 0; c < MAX_PATTERN_SIZE; c++)
		{
			/* only the tolerance in first component are set, so far	
			* now lets check different tolerances for all components
			*/
			tolerance[c][comp] = aC->HXC2tolerance[comp][c];
			maxCountESC += tolerance[c][comp];
		}
	}

	/* the smaller maxCountESC the earlier the count of the ESC symbol will be down-scaled	
	* assumption: the smaller the tolerances the less influence should have the count of the ESC symbol
	*/
	maxCountESC = 8;

#ifdef H_DEBUG
	fprintf( stdout, "\ntolerances (%d,%d,%d),(%d,%d,%d),(%d,%d,%d),", 
		tolerance[0][0], tolerance[0][1], tolerance[0][2], 
		tolerance[1][0], tolerance[1][1], tolerance[1][2], 
		tolerance[2][0], tolerance[2][1], tolerance[2][2]); 
	fprintf( stdout, "(%d,%d,%d),(%d,%d,%d),(%d,%d,%d)",
		tolerance[3][0], tolerance[3][1], tolerance[3][2], 
		tolerance[4][0], tolerance[4][1], tolerance[4][2], 
		tolerance[5][0], tolerance[5][1], tolerance[5][2] 
	);

#endif


	/* allocate array for pointers, one more for list termination, two needed, Why??	*/
	/* Batev: One more for list termination, and the second one for the current pattern,  
	* if it is to be added, as well
	* the larger MAX_SIMILAR_PATTERN the slower the search
	*/
	exchange.similar_patterns = NULL; /* suppress compiler warnings	*/
	ALLOC( exchange.similar_patterns, MAX_SIMILAR_PATTERN+2, PATTERN_LIST_ELEM_SCF *);
	if (ERRCODE == MALLOC_FAILED)
	{
		goto endSCF;
	}
	exchange.simCount = NULL; /* suppress compiler warnings	*/
	ALLOC( exchange.simCount, MAX_SIMILAR_PATTERN+2, unsigned int);
	if (ERRCODE == MALLOC_FAILED)
	{
		goto endSCF;
	}
	//ALLOC( exchange.similarity, MAX_SIMILAR_PATTERN+2, unsigned int);
	//if (ERRCODE == MALLOC_FAILED)
	//{
	//	goto endSCF;
	//}
	//ALLOC( exchange.sad, MAX_SIMILAR_PATTERN+2, unsigned int);
	//if (ERRCODE == MALLOC_FAILED)
	//{
	//	goto endSCF;
	//}

	exchange.K_ = 0; /* used for merged histogram, number of elements	*/
	//exchange.bestSimilarity = 0; /* is also reset in get_distribution_SCF()	*/

	increaseFac = aC->HXC2increaseFac * 2 + 1; /* '+1' is save guard	*/

	/* prepare arithmetic coding	*/
	if (encoder_flag)
	{
		start_encoding( acSt); /* reset internal values	*/
	}
	else
	{
		start_decoding( acSt, bitbuf); /* reset internal values	*/
	}

	/* allocate model of arithmetic coding
	* use maximum possible number of symbols
	* this is theoretically the number of colours in the image
	* which is equal to the number of pixels in maximum
	*/
	{
		unsigned long num;
		num = pd->size + 1;
		start_model( &M, num);
		M.K = 1; /* we start with empty histogram	*/
	}

	//num_abs_err = 0;
	//for( comp = 0; comp < pd->channel; comp++)
	//{
	//	sum_abs_err[comp] = predTol[comp] = 0;
	//}

	/*************************
	* create quadtree structure
	* one flag, whether pixels can be predicted (horizontally or vertically)
	* if yes, binary decision on direction (horizontally or vertically)
	* if no, binary decision, whether pixel needs a special treatment
	* the latter depends on the number of different colours in the block
	* a) when the number is rather high, then (i) the number of new patters is probably 
	*    high disturbing the statistics and slowing down the processing
	*	b) when the number is very high, then additionally (ii) it contains likely 
	*    new colours (stage 3 coding)
	* how can this info be utilised?
	* - in both cases, the required 'symbol=0'-coding should be avoided
	* - the inclusion of new patterns could be modified
	* - the update of the patterns histos could be modified
	* - the pattern size could be modified
	* - a totally different coding method could be used
	*  => have not found a method towards bitrate reduction yet :-(
	*
	* quadtree overhead can be critical
	* > granularity should be limited for images with few colours
	************************/
	bytes_old = numBytes; 

	CALLOC( mode_arr, pd->size, unsigned char);
	if (aC->SCF_prediction)
	{
		quadtree_structure( pd, mode_arr, encoder_flag,  &M, acSt, bitbuf);
	}

	*quad_bytes = numBytes - bytes_old;
	bytes_old = numBytes;
#ifdef INF_ARR
	//if (encoder_flag)
	{
		CALLOC( inf_arr, pd->size, unsigned int);
		CALLOC( flags_arr, pd->size, unsigned char);
		CALLOC( stage_arr, pd->size, unsigned char);
	}
#endif

	CALLOC( patt_arr, pd->size, unsigned long);
	verticalHit_count = horizontalHit_count = 1; /* same pattern Xhat=A or = B? */
	stage1_symbol0_totCount = stage1_symbol0_hitCount = 1; /* for SCF_PHOTO	*/
	stage2_symbol0_totCount = stage2_symbol0_hitCount = 1; /* for SCF_PHOTO	*/

	/* downscaling the histograms of patterns
	 * if threshold is reached all counts of this pattern are divided by 2
	 * value was not intensively tested
	 */
	rescaling_threshold = 8 * pd->width;

	fprintf( stderr, "\n");
	/*************************************************************************
	* process image
	************************/
	for ( r = 0; r < pd->height; r++)
	//for ( r = pd->height; r >= 0; r--)
	{
#ifdef _DEBUG
			r_glob = r;
#endif
		bytes_old = numBytes; /* check bytes per row	*/
		for( c = 0; c < pd->width; c++)
		//for( c = pd->width; c >= 0; c--)
		{
			pos = c + r * pd->width;
#ifdef _DEBUG
			if (r==6 && c == 12)
			{
				c= c;
			}
#endif
			/* reset estimated value, will be evaluated for update process	*/
			for ( comp= 0; comp < pd->channel; comp++)
			{
				Xhat_MAP[comp] = -1;
			}
			/* check whether pixel can be copied from above	*/
			if (mode_arr[pos] == SCF_VERTICAL)
			{
				if ( !encoder_flag)
				{
					/* write decoded triple to image array	*/
					for ( comp= 0; comp < pd->channel; comp++)
					{
						pd->data[comp][pos] = pd->data[comp][pos - pd->width];
					}
				}
#ifdef HXC2_EST_INF
				cnt_stage0++;
#endif
				continue;
			}
			/* check whether pixel can be copied from the left	*/
			if (mode_arr[pos] == SCF_HORIZONTAL)
			{
				if ( !encoder_flag)
				{
					/* write decoded triple to image array	*/
					for ( comp= 0; comp < pd->channel; comp++)
					{
						pd->data[comp][pos] = pd->data[comp][pos-1];
					}
				}
#ifdef HXC2_EST_INF
				cnt_stage0++;
#endif
				continue;
			}

			/* start with normal HXC2 mode	*/
			copy_pattern2( pd, r, c, t_actual);	/* get current pattern from image	*/

#ifdef H_DEBUG
			if ( r >= FIRSTCOL && r <= LASTCOL)
			{
				fprintf( stdout, "\n\n#pos %d, %d:", r, c);
				fprintf( stdout, " actual pattern: (");
				for( i = 0; i < MAX_PATTERN_SIZE; i++)
				{
					for( comp = 0; comp < pd->channel; comp++)
					{
						fprintf( stdout, "%d,", t_actual[i][comp]);
					}
					fprintf( stdout, ";");
				}
				fprintf( stdout, ")");
				fflush( stdout);
			}
#endif

			/* initialise first element of merged histogram	*/
			merged_stacked_histogram.count = 0; /* ESC item	*/
			merged_stacked_histogram.next = NULL;
			merged_stacked_histogram.value[0] = -1; /* no RGBA / YUVY quadruple assigned*/
			merged_stacked_histogram.value[1] = -1;
			merged_stacked_histogram.value[2] = -1;
			merged_stacked_histogram.value[3] = -1;
			exchange.K_ = 0; /* merged_stacked_histogram is empty, do not count ESC item*/

			if ( 0 && mode_arr[pos] == SCF_PHOTO)
			{
				/*  avoiding creation of new patterns	*/
				no_new_pattern_flag = 1;
				/* seems not to be a good mode	*/
			}
			else		no_new_pattern_flag = 0;

if ( mode_arr[pos] != SCF_PHOTO)
{

			/* enable increasing of tolerances dependent on the PHOTO mode	*/
			if (mode_arr[pos] == SCF_PHOTO) increaseTolMode = 2;
			else if (mode_arr[pos] == SCF_PHOTO2) increaseTolMode = 1;
			else increaseTolMode = 0;

			/* search list for matching patterns,
			* collect information about count statistics, build distribution if possible
			* return flags of similiar positions (not furtehr used currentla, is it?
			*/
			pattern_flags = get_distribution_SCF( pd, r, c, t_actual, t_plist, pat_list, 
				&exchange, &merged_stacked_histogram, tolerance,
				aC->pattern_size, no_new_pattern_flag, bias,
				patt_arr, increaseTolMode, aC->SCF_maxNumPatterns, &patternRemove);

#ifdef INF_ARR
			flags_arr[pos] = pattern_flags;
#endif

} /* if (mode_arr[pos] != SCF_PHOTO) */

			/*********************************************************/
			/* get estimate of triple	*/
			for( comp = 0; comp < pd->channel; comp++)
			{
				/* use MAP	*/
				long val;

				val = GetMAPVal( pd, comp, c, r, &A, &B, &C, &D, &E, &F, &P); /* get Xhat as MED	*/
				val += bias[comp]; /* pattern dependent bias compensation */
				Xhat_MAP[comp] = max( 0, min( (signed)K_Hist[comp], val) ); /* check range	*/
			} /* for ( comp */

			stage = 1;

			if (encoder_flag)
			{
				/* get colour vector from current position	*/
				for( comp = 0; comp < pd->channel; comp++)
				{
					X[comp] = pd->data[comp][pos];
				}
			}

			/* number of items in merged histogram	(excluding ESC symbol)
			* same would be pattern_flags == 0
			*/
			if (exchange.K_ == 0 )
			{
				X_in_merged_hist_flag = 0;
				for( comp = 0; comp < pd->channel; comp++)
				{ /* no estimate available	*/
					Xhat_Merged[comp] = -1;
				}
#ifdef H_DEBUG
				if ( r >= FIRSTCOL && r <= LASTCOL)
				{
					fprintf( stdout, ", merged histogram is empty");
				}
#endif
				/* go directly to stage 2	(palette based coding) */
			}
			else
			{
if (mode_arr[pos] != SCF_PHOTO)
//if(true)
{

				/* merged histogram contains at least one symbol in addition to ESC symbol*/
				/* check, whether merged histogram should be modified
				 * assumption: in SCF_PHOTO blocks there is a higher probability that
				 * X is not in merged histogram => count of ESC should be increased
				 */
				if ( mode_arr[pos] == SCF_PHOTO)
				{ /* set the count of exception handling to a high number */
					int off;
					long total_cnt=0;
					HIST_LIST_ELEM2 *current_item;

					/* first, get total count of merged hist	*/
					current_item = merged_stacked_histogram.next; /* ignore ESC	*/
					while (current_item != NULL)
					{
						total_cnt += current_item->count;
						current_item = current_item->next;
					}
					/* probability of ESC is stage1_symbol0_hitCount / stage1_symbol0_totCount
					 * if it is close to one, the count of ESC should be much larger than
					 * sum of others
					 */
					/* if (stage1_symbol0_hitCount / stage1_symbol0_totCount == 0.5) then
					 * count should not be modified	
					 *  2* ( a/b -0.5) = 2 * (a - 0.5*b) / b
					 * if quotient a/b is < 0.5 (very unlikely), then offset is negative
					 * +1 is helpful in case of 
					 *     stage1_symbol0_hitCount = stage1_symbol0_totCount=1
					 */
					off = 1 + total_cnt * /*  2   * */
								(stage1_symbol0_hitCount - (stage1_symbol0_totCount>>1) ) /
										       stage1_symbol0_totCount;
					if ( (signed)merged_stacked_histogram.count + off >= 0)
					{ /* count must not be negative ! */
						merged_stacked_histogram.count += off;
					}
					else
					{ /* unlikely to be EsC symbol	*/
						merged_stacked_histogram.count = 0;
					}
					stage1_symbol0_totCount++;
					/* frequent rescaling of statistics	*/
					if (stage1_symbol0_totCount > 31)
					{
#ifdef _DEBUG_X
						fprintf( stderr, " offset1=%3d/%d\n", stage1_symbol0_hitCount, stage1_symbol0_totCount);
#endif
						stage1_symbol0_totCount >>= 1;
						stage1_symbol0_hitCount >>= 1;
					}
				}

				
		//else
				{/* skip if in Photo mode, however the coincidence of PHOTO and vertical
					 * or horizontal structures is very rare, so it saves mainly some time
					 */
					verticalHitModificationFlag = 0;
					horizontalHitModificationFlag = 0;
					if (aC->SCF_directional && c > 0 && r > 0)
					{
						if (patt_arr[pos] == patt_arr[pos-1] /*&& 
							  patt_arr[pos] != patt_arr[pos-pd->width]*/)
						{ /* constant only in horizontal direction	*/
							/* increase count of A in merged_stacked_histogram */
							/* get colour vector from current A position	*/
							for( comp = 0; comp < pd->channel; comp++)
							{
								Av[comp] = pd->data[comp][pos-1];
							}
							modify_merged_hist( &merged_stacked_histogram, Av, 
								pd->channel, horizontalHit_count);
							horizontalHitModificationFlag = 1; /* activate updating	*/
		#ifdef H_DEBUG
							if ( r >= FIRSTCOL && r <= LASTCOL)
							{
								fprintf( stdout, ", horizontalHit_count=%d", horizontalHit_count);
							}
		#endif
						}
						else if (/* patt_arr[pos] != patt_arr[pos-1] && */
							       patt_arr[pos] == patt_arr[pos-pd->width])
						{
							/* get colour vector from B position	*/
							for( comp = 0; comp < pd->channel; comp++)
							{
								Bv[comp] = pd->data[comp][pos-pd->width];
							}
							/* increase count of B in merged_stacked_histogram */
							modify_merged_hist( &merged_stacked_histogram, Bv, 
								pd->channel, verticalHit_count);
							verticalHitModificationFlag = 1; /* activate updating	*/
		#ifdef H_DEBUG
							if ( r >= FIRSTCOL && r <= LASTCOL)
							{
								fprintf( stdout, ", verticalHit_count=%d", verticalHit_count);
							}
		#endif
						}
					}
				} /* if ( mode_arr[pos] != SCF_PHOTO) */

					if (encoder_flag)
					{
#ifdef OUT_MODE
						mode_hxc2 = 0;
#endif

						/* there is at least one item in merged histogram */
						/* check, whether X is included in merged histogram	
						* created histogram and get symbol
						* get averaged X vector = Xhat_Merged
						*/
						X_in_merged_hist_flag = X_in_merged_hist_SCF( &merged_stacked_histogram, 
							X, pd->channel,	&symbol, M.H, &(M.K), increaseFac, Xhat_Merged);
					}
					else
					{
						// get_merged_hist( &merged_stacked_histogram, M.H, &(M.K), increaseFac);
						get_merged_hist_SCF( &merged_stacked_histogram, M.H, &(M.K), increaseFac,
							Xhat_Merged, pd->channel);
					}

					assert( M.K < MAX_HIST_SIZE);
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						HIST_LIST_ELEM2 *merged_hist_element;
						fprintf( stdout, "\n\t merged histo:");
						merged_hist_element = &merged_stacked_histogram;
						do
						{
							fprintf( stdout, " %d x (", merged_hist_element->count);
							for ( comp = 0; comp < pd->channel; comp++)
							{
								fprintf( stdout, "%d,", merged_hist_element->value[comp]);
							}
							fprintf( stdout, "),");
							merged_hist_element = merged_hist_element->next;
						} while(merged_hist_element != NULL);

					}
#endif
					create_cumulative_histogram( M.H, M.K);

					if (encoder_flag)
					{
						/* do the arithmetic encoding	*/
						encode_AC( &M, acSt, symbol, bitbuf);
#ifdef HXC2_EST_INF
						{
							double p;
							p = (double)(M.H[symbol+1] - M.H[symbol]) / M.H[M.K];
#ifdef INF_ARR
							inf_arr[pos] = (int)(-1000. * log( p) * invLOG2);
							stage_arr[pos] = 1;
#endif
							inf_stage1 -= log( p) * invLOG2;
							if (symbol) 
								cnt_stage1++;
						}
#endif
					}
					else
					{
						/* do the arithmetic decoding	*/
						symbol = decode_AC( &M, acSt, bitbuf);
						if (symbol != 0)
						{
							/* store decoded signal value	*/
							/* get X from merged histogram	*/
							get_X_from_merged_hist( &merged_stacked_histogram, X, pd->channel,
								symbol);
							X_in_merged_hist_flag = 1;
						}
						else
						{
							X_in_merged_hist_flag = 0;
						}
					}
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						if (X_in_merged_hist_flag) fprintf( stdout, "\n\t X is in merged histo");
						else fprintf( stdout, "\n\t X is not in merged histo");

						fprintf( stdout, "\n\t en/decode (merged)symbol=%d", symbol);
						fprintf( stdout, "\n\t\t using %d-distribution(cumul.): ", M.K);
						for (i = 1; i <= M.K; i++)
						{
							fprintf( stdout, "%d,", M.H[i]);
						}
					}
#endif


} /* if !SCF_PHOTO */
else
{  /* must be set when stage 1 is bypassed	*/
				X_in_merged_hist_flag = 0;
				for( comp = 0; comp < pd->channel; comp++)
				{ /* no estimate available	*/
					Xhat_Merged[comp] = -1;
				}
}

			} /* if (exchange.K_ > 0 )*/

			/****************************************************************************
			* stage 2 palette based coding
			*****************************************************************************/
			if (!X_in_merged_hist_flag) /* enter stage 2	*/
			{
				stage = 2;

				if ( mode_arr[pos] == SCF_PHOTO && exchange.K_ > 0)
				{ /* increase the count of exception handling
					 * only if also stage1_symbol0_totCount had been incremented
					 */
					stage1_symbol0_hitCount++;
				}

				/* big question for grey images: 
				 * Should the next grey-value be encoded in stage 2 or immediately
				 * in stage 3?
				 * advantage of stage 2: paltette is small in the beginning => few bits for few symbols
				 * advantage of stage 3: prediction error might be smaller than orig value
				 */
				//2.14d:				if (pd->channel > 1 || palette->count < (pd->maxVal_rct[0] >> 3) /* 0 => go directly to stage 3 */)
				// 2.14e				if (pd->channel > 1 || palette->count < (pd->maxVal_rct[0] >> 1) /* 0 => go directly to stage 3 */)
				// 2.14f				if (pd->channel > 1 || alette->count < (pd->maxVal_rct[0] >> 2) /* 0 => go directly to stage 3 */)
				// 2.14g				if (pd->channel > 1 || numOfPaletteItems < (pd->maxVal_rct[0] >> 2) /* 0 => go directly to stage 3 */)
				// 2.15
				//aveCosts2 = inf_stage2 / (cnt_stage2 + 1);
				//aveCosts3 = inf_stage3 / (cnt_stage3 + 1);
				if (pd->channel > 1 ||  numOfPaletteItems < (pd->maxVal_rct[0] >> 1)) /* 0 => go directly to stage 3 */
				{ 
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						fprintf( stdout, "\n\t X is not in merged histogram, enter stage 2");
					}
#endif
#ifdef OUT_MODE
					mode_hxc2 = 1;
#endif

					/* fake count of ESC sxmbol (must be undone afterwards	*/
					if ( mode_arr[pos] == SCF_PHOTO)
					{ /* increase the count of exception handling	*/
						stage2_symbol0_offset = (signed)numOfPaletteItems * /* 2 * */
							(stage2_symbol0_hitCount - (stage2_symbol0_totCount>>1) ) /
								stage2_symbol0_totCount;
						if ( ((signed)palette->count + stage2_symbol0_offset) < 0)
						{ /* avoid negative counts	*/
							stage2_symbol0_offset = -(signed)palette->count;
						}
						palette->count += stage2_symbol0_offset;

						stage2_symbol0_totCount++;
						if (stage2_symbol0_totCount > 31)
						{
#ifdef _DEBUG_X
fprintf( stderr, "   offset2=%3d/%d\n", stage2_symbol0_hitCount, stage2_symbol0_totCount);
#endif
							stage2_symbol0_totCount >>= 1;
							stage2_symbol0_hitCount >>= 1;
						}
					}

					/*********************************************************/

					if (pd->channel > 1  || 1)
					{
						/* prepare histogram based on palette and increase count of estimated
						* triple +- tolerances
						*/
						if (numOfPaletteItems < paletteSizeStage2Tuning) /* does not change performance very much	*/
						{ /* do not increase counts when size of palette is low */
								palette_to_histogram_SCF( palette, M.H, Xhat_Merged, Xhat_MAP, pd->channel, 0, diffMaxCalculated);
						}
						else
						{/* increase counts of elements which are equal to Xhat +- predTol*4 */
							//palette_to_histogram_SCF( palette, M.H, Xhat_MAP, pd->channel, predTol, 
							//	Hperr, HperrResetCnt, aC->SCF_stage2tuning);
							palette_to_histogram_SCF( palette, M.H, Xhat_Merged, Xhat_MAP, pd->channel,
								aC->SCF_stage2tuning, diffMaxCalculated);
						}
					}
					//else
					//{
					//	greyPalette_to_histogram_SCF( palette, M.H, Xhat_MAP,
					//		aC->SCF_stage2tuning && (numOfPaletteItems >= 11), meanAbs);
					//}

					if ( mode_arr[pos] == SCF_PHOTO)
					{ /* undo fake	*/
						palette->count -= stage2_symbol0_offset;
					}
					M.K = numOfPaletteItems;
					create_cumulative_histogram( M.H, M.K); /* prepare histogram */	

					//undo diffMaxIncrase

					if(mode_arr[pos] == SCF_SYNTHETIC)
					{
						//diffMaxCalculated -= 1;
						//fprintf(stderr, "\nSCF_SYNTHETIC: diffMaxCalculated increased is now %d", diffMaxCalculated);
					}

					if(mode_arr[pos] == SCF_PHOTO2)
					{
						//diffMaxCalculated -= 2;
						//fprintf(stderr, "\ndiffMaxCalculated decreased is now %d", diffMaxCalculated);
					}

					if(mode_arr[pos] == SCF_PHOTO)
					{
						//diffMaxCalculated -= 4;
						//fprintf(stderr, "\ndiffMaxCalculated increased is now %d", diffMaxCalculated);
					}


					if (encoder_flag)
					{
						/* check whether X is in palette, fill histogram
						* returns zero if not in palette
						*/
						symbol = X_in_palette( palette, X, pd->channel);
						encode_AC( &M, acSt, symbol, bitbuf);

#ifdef HXC2_EST_INF
						{
							double p;
							p = (double)(M.H[symbol+1] - M.H[symbol]) / M.H[M.K];
#ifdef INF_ARR
							inf_arr[pos] += (int)(-1000. * log( p) * invLOG2);
							stage_arr[pos] = 2;
#endif
							inf_stage2 -= log( p) * invLOG2;
							if (symbol)
								cnt_stage2++; 
							/* exclude symbol == 0 as this pixel is counted in the next stage*/
						}
#endif
					}
					else
					{
						symbol = decode_AC( &M, acSt, bitbuf);
						/* decoder must reconstruct X vector, if X is contained	*/
						if (symbol) get_X_from_palette( palette, symbol, X, pd->channel);
					}
						
					//if (pd->channel == 1)
					//{ /* update prediction error distribution for grey images	*/
					//	sumAbsPred_err += abs(X[0] - Xhat_MAP[0]);
					//	countAbsPred_err++;
					//	meanAbs = sumAbsPred_err / countAbsPred_err;
					//}
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						HIST_LIST_ELEM2 *current_hist_element;
						fprintf( stdout, "\n\t use palette histo:");
						current_hist_element = palette;
						while( current_hist_element != NULL) /* list is terminated with NULL	*/
						{
							fprintf( stdout, " %d x (", current_hist_element->count);
							for (comp=0; comp< pd->channel; comp++)
							{
								fprintf( stdout, " %d,", current_hist_element->value[comp]);
							}
							fprintf( stdout, ")");
							/* goto to the next histogram element	*/
							current_hist_element = current_hist_element->next;
						}
					}
#endif
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						fprintf( stdout, "\n\t encode (palette)symbol=%d", symbol);
						fprintf( stdout, "\n\t\t using cumul.%d-distribution: ", M.K);
						for (i = 1; i <= M.K; i++)
						{
							fprintf( stdout, "%d,", M.H[i]);
						}
					}
#endif
				}
				else /* pd->channel == 1; enter directly stage 3	*/
				{
					symbol = 0;
				}

				if (symbol)
				{
					/* nothing to do here */
				}
				else /* symbol == 0; enter stage 3	*/
				{
					/***********************************************************************
					* stage 3 separate coding of components
					************************************************************************/
#ifdef H_DEBUG
					if ( r >= FIRSTCOL && r <= LASTCOL)
					{
						fprintf( stdout, "\n\t ==> X is also not in palette yet");
					}
#endif
#ifdef OUT_MODE
					mode_hxc2 = 2;
#endif
					stage = 3;

					if ( mode_arr[pos] == SCF_PHOTO)
					{ /* esc symbol was  used, so increment the count	*/
						stage2_symbol0_hitCount++;
					}

					palette->count++; /* count of symbol 0 (X not in palette)	 */
					/* count has been increased check for rescaling
					 * adopted from hist_ops2.c, integrate_X() where other symbols are checked
					 */
					if (palette->count >  4 + ((numOfPaletteItems) >> 2) )
					{
						HIST_LIST_ELEM2 *current_item;
						current_item = palette; /* start with symbol ==0 item */
						while (current_item != NULL) 
						{
							current_item->count >>= 1;  /* scale down	*/
							current_item = current_item->next;
						}
					}

					/* X is not in palette, use separate coding	*/
					/* this implementation is bit consuming compared to the approach with 
					* indexed colours, since three values have to be encoded here
					* instead of only one symbol
					* after encoding R, the distribution of G should be dependent on R,
					* B should be dependent on R and G a.s.o.
					* however, this stage is only used one time for each colour
					*/
					//encode RGBA separately
					pred_mode = 0;
					for( comp = 0; comp < pd->channel; comp++)
					{
						/* maximum number of different values	*/
						M.K = K_Hist[comp];
						/* copy histogramm of pred errors of component into M	*/
						memcpy( M.H, Hist[comp], sizeof( unsigned long) * (K_Hist[comp] + 1) );
#ifdef H_DEBUG
						if ( r >= FIRSTCOL && r <= LASTCOL)
						{
							fprintf( stdout, "\n\t\t   using %d-distribution: ",  M.K);
							for (i = 1; i <= M.K; i++)
							{
								fprintf( stdout, "%d,", M.H[i]);
							}
						}
#endif

						create_cumulative_histogram( M.H, M.K);
						/* use MAP	*/
						{
							long clampVal1, clampVal2, val;
							clampVal1 = (M.K-1) >> 1;
							clampVal2 = M.K >> 1;

							/* bias not seems to improve things	*/ 
							val = GetMAPVal( pd, comp, c, r, &A, &B, &C, &D, &E, &F, &P);
							ABC = max( 0, min( (signed)M.K, val) );
							P = max( 0, min( (signed)M.K, P) );
							ABCD = (A + B + C + D + 2) >> 2;


							if (encoder_flag)
							{
								/* for comp==0: pred_mode == 0	*/
								//PM_MA improvement 02 reintroduce Xhat_MAP[comp] = ABC
								if (!pred_mode)	Xhat_MAP[comp] = ABC;
								else if (pred_mode == 1) Xhat_MAP[comp] = A;
								else if (pred_mode == 2) Xhat_MAP[comp] = B;
								else if (pred_mode == 3) Xhat_MAP[comp] = C;
								else if (pred_mode == 4) Xhat_MAP[comp] = D;
								else if (pred_mode == 5) Xhat_MAP[comp] = P;
								else if (pred_mode == 6) Xhat_MAP[comp] = ((A + B) >> 1);
								else if (pred_mode == 7) Xhat_MAP[comp] = ABCD;
									
								pred_err = X[comp] - Xhat_MAP[comp];

								//fprintf( stdout, "\n %d,%d: X=%d val=%d mode=%d", r, c, X[comp], val, pred_mode);

								/* clamp	*/
								if (pred_err > clampVal1) pred_err -= M.K;
								else if (pred_err < -clampVal2) pred_err += M.K;
								symbol = pred_err + clampVal2;
								encode_AC( &M, acSt, symbol, bitbuf);
#ifdef HXC2_EST_INF
								{
									double p;
									p = (double)(M.H[symbol+1] - M.H[symbol]) / M.H[M.K];
#ifdef INF_ARR
									inf_arr[pos] += (int)(-1000. * log( p) * invLOG2);
									stage_arr[pos] = 3;
#endif
									inf_stage3 -= log( p) * invLOG2;
									if (comp == 0) 
										cnt_stage3++; /* single count for pixel	*/
								}
#endif
							}
							else
							{ /* decoder	*/
								symbol = decode_AC( &M, acSt, bitbuf);
								pred_err = symbol - clampVal2;

								//PM_MA improvement 02 reintroduce Xhat_MAP[comp] = ABC
								if (!pred_mode)	Xhat_MAP[comp] = ABC;
								else if (pred_mode == 1) Xhat_MAP[comp] = A;
								else if (pred_mode == 2) Xhat_MAP[comp] = B;
								else if (pred_mode == 3) Xhat_MAP[comp] = C;
								else if (pred_mode == 4) Xhat_MAP[comp] = D;
								else if (pred_mode == 5) Xhat_MAP[comp] = P;
								else if (pred_mode == 6) Xhat_MAP[comp] = ((A + B) >> 1);
								else if (pred_mode == 7) Xhat_MAP[comp] = ABCD;

								X[comp] = pred_err + Xhat_MAP[comp];

								/* un-clamp	*/
								if (X[comp] >= (signed) M.K) X[comp] -= (signed)M.K;
								else if (X[comp] < 0) X[comp] += M.K;
							}

							/* check best mode for next component */
							if (aC->SCF_colourMAP)
							{
									/* include also the adaptive predictor !	*/
									min_abs_err = abs( X[comp] - ABC);
									pred_mode = 0; 
									if (min_abs_err > abs( X[comp] - A) )
									{
										pred_mode = 1;
										min_abs_err = abs( X[comp] - A);
									}
									if (min_abs_err > abs( X[comp] - B) )
									{
										pred_mode = 2;
										min_abs_err = abs( X[comp] - B);
									}
									if (min_abs_err > abs( X[comp] - C) )
									{
										pred_mode = 3;
										min_abs_err = abs( X[comp] - C);
									}
									if (min_abs_err > abs( X[comp] - D) )
									{
										pred_mode = 4;
										min_abs_err = abs( X[comp] - D);
									}
									if (min_abs_err > abs( X[comp] - P) )
									{
										pred_mode = 5;
										min_abs_err = abs( X[comp] - P);
									}
									if (min_abs_err > abs( X[comp] - ((A + B) >> 1)) )
									{
										pred_mode = 6;
										min_abs_err = abs( X[comp] - ((A + B) >> 1));
									}
									if (min_abs_err > abs( X[comp] - ABCD) )
									{
										pred_mode = 7;
										min_abs_err = abs( X[comp] - ABCD);
									}
							}
						}/* do prediction	*/
#ifdef H_DEBUG
						if ( r >= FIRSTCOL && r <= LASTCOL)
						{
							fprintf( stdout, "\n\t   prediction error=%d", pred_err);
							fprintf( stdout, "\t   symbol=%d", symbol);
						}
#endif

						/* update histogram for this component, since this element
						* can be also a part of  another RGB or YUV vector 
						*/
						Hist[comp][ symbol+1 ]++;	/* increment count of X element	*/
					} /* for comp */
				} /* if (symbol == 0)  end of stage 3*/
			} /* if (!X_in_merged_hist_flag) stage 2 + 3 */
			else
			{
				/* X is in merged histogramm */
			}
#ifdef OUT_MODE
			// fprintf( mode_file, "%d ", mode_hxc2);
			if (!X_in_merged_hist_flag)
			{
				fprintf( mode_file, "%3d %3d %3d ", Xhat[0], Xhat[1], Xhat[2]);
			}
			else
			{
				fprintf( mode_file, "%3d %3d %3d ", 0, 0, 0);
			}
#endif

			if (!X_in_merged_hist_flag /* || use_stage == 3 */)
			{
				/* integrate actual colour at top of palette or update frequency	*/
				integrate_X( palette, X, pd->channel, &numOfPaletteItems);
#ifdef H_DEBUG
				if ( r >= FIRSTCOL && r <= LASTCOL)
				{
					HIST_LIST_ELEM2 *current_hist_element;
					fprintf( stdout, "\n\t\t ==> new palette histo:");
					current_hist_element = palette;
					while( current_hist_element != NULL) /* list is terminated with NULL	*/
					{
						fprintf( stdout, " %d x (", current_hist_element->count);
						for (comp=0; comp< pd->channel; comp++)
						{
							fprintf( stdout, " %d,", current_hist_element->value[comp]);
						}
						fprintf( stdout, ")");
						/* goto to the next histogram element	*/
						current_hist_element = current_hist_element->next;
					}
				}
#endif
			}

			if (!encoder_flag)
			{ /* decoder must write X vector to image array	*/
				/* write decoded triple to image array	*/
				for ( comp= 0; comp < pd->channel; comp++)
				{
					pd->data[comp][pos] = X[comp];
				}
			}
#ifdef H_DEBUG
			if ( r >= FIRSTCOL && r <= LASTCOL)
			{
				fprintf( stdout, "\n\t X=(");
				for( comp = 0; comp < pd->channel; comp++)
				{
					fprintf( stdout, "%d,", pd->data[comp][c + r * pd->width]);
				}
				fprintf( stdout, ")");
			}
#endif

			/* include information about true signal value in list for all
			* patterns, which had been found to be similar
			*/
			//update_pattern_list2( &exchange, 
			//	X, pd->channel, X_in_merged_hist_flag, maxCountESC);
	
			//call update_pattern_list in case that "if mode_arr[pos] != SCF_PHOTO)" was true
			//so that memory is already allocated for later processing
			/* should be here because of decoder	*/
			update_pattern_list_SCF(&exchange, X, pd->channel, X_in_merged_hist_flag,
				maxCountESC, rescaling_threshold, Xhat_MAP);
				
			/* if !X_in_merged_hist_flag then  also ESC count is incremented */


			if (patternRemove.patternlist_flag)
			{ /* one element of last used pattern list must be removed	*/
				HIST_LIST_ELEM2 *hist_elem, *tmp;

				patternRemove.patternlist_flag = 0; /* reset for next round	*/
				/* low performer pattern must be excluded from this list */

/*				if (patternRemove.PreviousElementAddr->next != patternRemove.CurrentElementAddr)
				{
					int wert = 77;
				}*/
				patternRemove.PreviousElementAddr->next = patternRemove.CurrentElementAddr->next;
				/* remove histogramm items first	*/
				hist_elem = patternRemove.CurrentElementAddr->top_hist_elem;
				while (hist_elem != NULL)
				{
					tmp = hist_elem->next;
					free(hist_elem);
					hist_elem = tmp;
				}
				(*patternRemove.addressListSize)--; /* decrease size of corresponding list	*/
				free(patternRemove.CurrentElementAddr);
			}
			/* update counts for vertical an horizontal enforcement	*/
			if (horizontalHitModificationFlag)
			{
				int match_flag = 1;
				for( comp = 0; comp < pd->channel; comp++)
				{
					if (X[comp] != Av[comp])
					{
						match_flag = 0;
						break;
					}
				}
				if (match_flag)
				{
					horizontalHit_count++;
				}
				else
				{
					if (horizontalHit_count) horizontalHit_count >>= 1;
				}
#ifdef H_DEBUG
				if ( r >= FIRSTCOL && r <= LASTCOL)
				{
					fprintf( stdout, ", updated horizontalHit_count=%d", horizontalHit_count);
				}
#endif
			}
			if (verticalHitModificationFlag)
			{
				int match_flag = 1;
				for( comp = 0; comp < pd->channel; comp++)
				{
					if (X[comp] != Bv[comp])
					{
						match_flag = 0;
						break;
					}
				}
				if (match_flag)
				{
					verticalHit_count++;
				}
				else
				{
					if (verticalHit_count) verticalHit_count >>= 1;
				}
#ifdef H_DEBUG
				if ( r >= FIRSTCOL && r <= LASTCOL)
				{
					fprintf( stdout, ", updated verticalHit_count=%d", verticalHit_count);
				}
#endif
			}
			// free merged histogram
			free_stacked_hist( &merged_stacked_histogram);

		} /* for( c = 0 */

		/* show number of pattern in actual list */
		{
			unsigned long numPattern = 0;
			for (i = 0; i < numLists; i++)
			{
				numPattern += pat_list[i].pattern_list_size;
			}
			fprintf( stderr, "\r%3.2f%%  pattern: %d  ", 100.*(r+1) / pd->height, numPattern);
			//fprintf( stderr, " meanAbs: %d  ", meanAbs);
		}

		if (encoder_flag)
		{
			fprintf( stderr, "  Bytes: %d / %d  ", numBytes - bytes_old, numBytes);
#ifdef _DEBUG
			static long old_count = 0;
			fprintf( stdout, "\n r=%d:  Bytes: %d / %d   ", r, numBytes - bytes_old, numBytes);
			fprintf( stdout, ", counts %d / %d   ", 
				cnt_stage1 + cnt_stage2 + cnt_stage3 - old_count, pd->width);
			old_count = cnt_stage0 + cnt_stage1 + cnt_stage2 + cnt_stage3;
#endif
#ifdef OUT_MODE
			fprintf( mode_file, "\n");
#endif
		}
	} /* for( r = 0 */

	if (encoder_flag)
	{
		finish_encode( acSt, bitbuf);	/* finish arithmetic coding	*/
		fprintf( stderr, "\ntotal:  %d Bytes", numBytes);

		aC->num_HXC_patterns = 0;
		// for (i = 0; i <= pd->maxVal[comp]; i++ )
		for (i = 0; i < numLists; i++ )
		{
			aC->num_HXC_patterns += pat_list[i].pattern_list_size;
		}
	}

	//PM_MA improvement 01 tmp here: undo rotate BEGIN
	if(rotateMode != 0 && !encoder_flag){
		rotateImage(pd, rotateMode, encoder_flag);
	}

#ifdef H_DEBUG
	if ( r >= FIRSTCOL && r <= LASTCOL)
	{
		fprintf( stdout, "\n final zero order histograms\n");
		for( comp = 0; comp < pd->channel; comp++)
		{
			fprintf( stdout, "\n");
			for ( i = 0; i <= K_Hist[comp]; i++)
			{
				fprintf( stdout, "%d ", Hist[comp][i]);
			}
		}
		fprintf( stdout, "\n final palette\n");
		{
			HIST_LIST_ELEM2 *item;
			item = palette;
			do
			{
				fprintf( stdout, "%dx", item->count);
				fprintf( stdout, "(");
				for( comp = 0; comp < pd->channel; comp++)
				{
					fprintf( stdout, "%d,", item->value[comp]);
				}
				fprintf( stdout, ")");
				item = item->next;
			} while( item != NULL);
		}
	}
#endif

#ifdef INF_ARR
#ifdef HXC2_EST_INF
	{
		FILE *out=NULL;
		char filename[512];
		unsigned int i;


		sprintf( filename, "inf_arr.pgm");
		out = fopen( filename, "wt");
		fprintf( out,"P2\n");
		fprintf( out,"# created by %s\n", thisVersion);
		fprintf( out,"%d %d\n", pd->width, pd->height);
		fprintf( out,"%d", 24000);

		for ( i = 0; i < pd->size; i++) 
		{
			if ((i % pd->width) == 0)		fprintf( out, "\n");
			fprintf( out,"%5d ", inf_arr[i]);
		}
		fprintf( out,"\n");
		fclose( out);
	}
#endif 
	{
		FILE *out=NULL;
		char filename[512];
		unsigned int i;


		sprintf( filename, "flags_arr.pgm");
		out = fopen( filename, "wt");
		fprintf( out,"P2\n");
		fprintf( out,"# created by %s\n", thisVersion);
		fprintf( out,"%d %d\n", pd->width, pd->height);
		fprintf( out,"%d", 255); /* assume 24 Bits in maximum	*/

		for ( i = 0; i < pd->size; i++) 
		{
			if ((i % pd->width) == 0)		fprintf( out, "\n");
			fprintf( out,"%5d ", flags_arr[i]);
		}
		fprintf( out,"\n");
		fclose( out);
	}
	{
		FILE *out=NULL;
		char filename[512];
		unsigned int i;


		sprintf( filename, "stage_arr.pgm");
		out = fopen( filename, "wt");
		fprintf( out,"P2\n");
		fprintf( out,"# created by %s\n", thisVersion);
		fprintf( out,"%d %d\n", pd->width, pd->height);
		fprintf( out,"%d", 3); /* 	*/

		for ( i = 0; i < pd->size; i++) 
		{
			if ((i % pd->width) == 0)		fprintf( out, "\n");
			fprintf( out,"%5d ", stage_arr[i]);
		}
		fprintf( out,"\n");
		fclose( out);
	}
#endif 

endSCF:
	FREE( flags_arr);
	FREE( inf_arr);
	FREE( stage_arr);

	FREE( mode_arr);
	FREE( patt_arr);

	/* clear arithmetic coding	*/
	free_model( &M);
	FREE( exchange.similar_patterns);
	FREE( exchange.simCount);
	//FREE( exchange.similarity);
	//FREE( exchange.sad);
	free_imatrix( &t_actual);
	free_imatrix( &t_plist);
	free_imatrix( &tolerance);

	/* free all allocated patter list elements and the
	* corresponding histogram elements
	*/
	for (i = 0; i < numLists; i++ )
	{
		free_pattern_list_SCF( &(pat_list[i].top));
	}
	FREE( pat_list);
	//FREE( HperrMAP);
	//FREE( HperrMRG);

	/* free all allocated  histograms	 */
	for (comp = 0; comp < pd->channel; comp++)
	{
		FREE( Hist[comp]);
	}

	/* free palette	*/
	free_stacked_hist( palette);
#ifdef OUT_MODE
	fprintf( mode_file, "\n");
	fclose( mode_file);
#endif

}

