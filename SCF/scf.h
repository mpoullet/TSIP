/*****************************************************
* File..: scf.h
* Desc..: declarations for screen content coder
* Author: T. Strutz
* Date..: 03.11.2015
* changes
* 07.12.2015 palette_to_histogram_SCF new parameter Hperr, MAX_MAP_ERR
******************************************************/
#include <stdlib.h>
#include "ginterface.h"
#include "bitbuf.h"
#include "arithmetic.h"
#include "autoConfig.h"
#include "hxc.h"
#include "header.h"

#ifdef _DEBUG
#define H_DEBUG
#define FIRSTCOL 184
#define LASTCOL 184
#endif

/* scales the counts in stage 2, can be beneficial, 
 * when a count becomes zero do to downscaling
 */
//#define MODE1_INCREMENT_SCF 10
//#define MODE1_INCREMENT_SCF 50
//
#define MODE1_INCREMENT_SCF 15
//#define MODE1_INCREMENT_SCF 1  
//#define MODE1_INCREMENT_SCF 100

/* maximal absolute estimation error for stage2tuning		*/
#define MAX_MAP_ERR 128

/* structure of a single element on pattern list	*/
typedef struct PATTERN_LIST_ELEM_SCF
{
	//PATTERN_LIST_ELEM_SCF *next, *prev; /* connection to predecessor and successor	*/
	PATTERN_LIST_ELEM_SCF *next; /* connection to predecessor 	*/
	unsigned int r;	/* position of pattern in original image	*/
	unsigned int c;	/* position of pattern in original image	*/
	HIST_LIST_ELEM2 *top_hist_elem;	/* pointer to top element of histogram list */
	/* it is a conditional histogram, which values occur how often in connection 
	   with this pattern */
	//unsigned int increment_value;	/* start with high number, then decrement */
	unsigned int hist_size;	/* number of elements in histogram	*/
	unsigned int rescaling_count;	/* number how often this pattern has been seen	*/
	unsigned long access_count;	/* number of often this pattern has been scanned	*/
	unsigned long usage_count;	/* number of often this pattern has been useful	*/
	int bias[4];	/* bias for MAP prediction	*/
} PATTERN_LIST_ELEM_SCF;

/* head of pattern list	*/
typedef struct 
{
	unsigned long pattern_list_size;
	PATTERN_LIST_ELEM_SCF top;
}PATTERN_LIST_SCF;

typedef struct /*structure for exchanging information */ 
{
	PATTERN_LIST_ELEM_SCF **similar_patterns; /* array of pointer to similar patterns	*/
	unsigned int *simCount; /* array of number of similar pattern items	*/
	//unsigned int *similarity; /* array of number of similar pattern items	*/
	//unsigned int bestSimilarity; /* similarity considering the distance between pattern elements	*/
	unsigned int bestSimCount; /*number of similar positions <= pattern_size	*/
	//unsigned int *sad; /* best SAD	*/
	//unsigned int bestSAD; /* best SAD	*/
	unsigned long K_;	/* size of histogram, number of bins	*/
}EXCHANGE_STRUC_SCF;


/* unin for removing pattern list elements	*/
typedef struct 
{
	int patternlist_flag; /* is set if something has to be removed	*/
	unsigned long  *addressListSize; /* pointer to pattern_list_size variable	*/
	PATTERN_LIST_ELEM_SCF *PreviousElementAddr, *CurrentElementAddr;
}PATTERN_REMOVE_SCF;


/* maximal number of items in similar pattern list */
//#define MAX_SIMILAR_PATTERN 500
/* interestingly a lower number of used similar patterns leads to a better distribution 
 * in the merged histogramm, 
 */
//#define MAX_SIMILAR_PATTERN 50 
#define MAX_SIMILAR_PATTERN 25 
#define PAT_RGB
#define PAT_QUANT_M 256 /* be cautious! an array of */
#define PAT_QUANT_D 256 /* PAT_QUANT_M * PAT_QUANT_D 
											 * will be allocated */
/* be cautious! an array of QUANT_FEAT_1 * QUANT_FEAT_2 * ...* QUANT_FEAT_6
 * will be allocated 
 */
//#define QUANT_FEAT_0 32 
//#define QUANT_FEAT_1 32
//#define QUANT_FEAT_2 4
//#define QUANT_FEAT_3 4
//#define QUANT_FEAT_4 4 
//#define QUANT_FEAT_5 4
//#define QUANT_FEAT_0 256 
//#define QUANT_FEAT_1 256
//#define QUANT_FEAT_2 2
//#define QUANT_FEAT_3 2
//#define QUANT_FEAT_4 2 
//#define QUANT_FEAT_5 2
#define QUANT_FEAT_0 128 
#define QUANT_FEAT_1 128
#define QUANT_FEAT_2 8
#define QUANT_FEAT_3 8
#define QUANT_FEAT_4 2 
#define QUANT_FEAT_5 2

#define SCF_NOTSET 0
#define SCF_HORIZONTAL 1
#define SCF_VERTICAL 2
#define SCF_SYNTHETIC 3
#define SCF_PHOTO2 4 /* activated, when higher than SCF_COLNUM_THRESH2_X */
#define SCF_PHOTO  5 /* activated, when higher than SCF_COLNUM_THRESH1_X */
#define SCF_COLOURBASED 7
/* SCF_PHOTO influences:
 * - tolerance values; will be set higher, when C,D or E,F position
 * - count of ESC symbol dependent on ..
     stage2_symbol0_offset
 */

// colour thresholds for quadtree decomposition SCF_PHOTO
/* 2.12b */
// nicht so gut
//#define SCF_COLNUM_THRESH1_64_DEFAULT 1300
//#define SCF_COLNUM_THRESH1_32_DEFAULT 520
//#define SCF_COLNUM_THRESH1_16_DEFAULT 200
//#define SCF_COLNUM_THRESH1_8_DEFAULT	60
//#define SCF_COLNUM_THRESH1_4_DEFAULT	16
// okay
#define SCF_COLNUM_THRESH1_64_DEFAULT 1200
#define SCF_COLNUM_THRESH1_32_DEFAULT 490
#define SCF_COLNUM_THRESH1_16_DEFAULT 180
#define SCF_COLNUM_THRESH1_8_DEFAULT	55
#define SCF_COLNUM_THRESH1_4_DEFAULT	16
//#define SCF_COLNUM_THRESH1_64_DEFAULT 1024
//#define SCF_COLNUM_THRESH1_32_DEFAULT 432
//#define SCF_COLNUM_THRESH1_16_DEFAULT 144
//#define SCF_COLNUM_THRESH1_8_DEFAULT	48
//#define SCF_COLNUM_THRESH1_4_DEFAULT	15

// colour thresholds for quadtree decomposition SCF_PHOTO2
#define SCF_COLNUM_THRESH2_64_DEFAULT 950
#define SCF_COLNUM_THRESH2_32_DEFAULT 300
#define SCF_COLNUM_THRESH2_16_DEFAULT 90
#define SCF_COLNUM_THRESH2_8_DEFAULT	30
#define SCF_COLNUM_THRESH2_4_DEFAULT	12
//
/*  2.12c  verschlechtert*/
//#define SCF_COLNUM_THRESH1_64_DEFAULT 972
//#define SCF_COLNUM_THRESH1_32_DEFAULT 324
//#define SCF_COLNUM_THRESH1_16_DEFAULT 108
//#define SCF_COLNUM_THRESH1_8_DEFAULT	36
//#define SCF_COLNUM_THRESH1_4_DEFAULT	12

/* compact histogram in list structure */
typedef struct QUADTREE_ELEM
{
	// unsigned char *arr;
	unsigned int quad_width;	/* 	*/
	unsigned int threshold;	/* max number of colours */
	unsigned int threshold2;	/* max number of colours */
	unsigned int width;	/* */
	unsigned int height;	/* */
	unsigned long size;	/* */
}QUADTREE_ELEM;

int quadtree( PicData *pd, 
							QUADTREE_ELEM *quadstruc);
void quadtree_structure( PicData *pd, unsigned char *mod_arr,
												int encoder_flag, 
												acModell *M, acState *acSt, BitBuffer *bitbuf);

//PM_MA improvement 01 count number of colours, also pass PicHeader structure
void SCF_coding( PicData *pd,		/* structure with image data	*/
								 BitBuffer *bitbuf, /* interface for file input and output*/
								 AutoConfig *aC,  /* parameter of codec */
								 int encoder_flag,	/* 1 ==> encoder; 0=> decoder	*/
								unsigned int *quad_bytes, PicHeader *ph);
unsigned char getBlockPredMode( PicData *pd, 
													 unsigned int y_start, unsigned int y_end,
													 unsigned int x_start, unsigned int x_end);
unsigned long getBlockNumColours( PicData *pd, 
														 unsigned int y_start, unsigned int y_end,
														 unsigned int x_start, unsigned int x_end);
unsigned long getBlockNumPatterns( PicData *pd, 
														 unsigned int y_start, unsigned int y_end,
														 unsigned int x_start, unsigned int x_end);
unsigned long getBlockNumPatterns2( PicData *pd, 
														 unsigned int y_start, unsigned int y_end,
														 unsigned int x_start, unsigned int x_end);
unsigned char get_distribution_SCF( PicData *pd, unsigned int r, 
									 unsigned int c,
							long **t_actual, long **t_plist, 
							PATTERN_LIST_SCF *pat_list, EXCHANGE_STRUC_SCF *exchange,
							HIST_LIST_ELEM2 *merged_stacked_histogram,
							long **tolerance,
							unsigned int pattern_size,
							int no_new_pattern_flag, long *bias,
							unsigned long *patt_arr,
							int increaseTolMode, unsigned int maxNumPatterns,
							PATTERN_REMOVE_SCF *patternRemove);
int X_in_merged_hist_SCF( HIST_LIST_ELEM2 *merged_stacked_histogram, 
										 long *X, unsigned int numOfElements, 
										 unsigned long *symbol, 
										 unsigned long *H, unsigned long *K,
										 unsigned int increaseFac,
										 long *Xhat);
void get_merged_hist_SCF( HIST_LIST_ELEM2 *merged_stacked_histogram, 
										 unsigned long *H, unsigned long *K,
										 unsigned int increaseFac,
										 long *Xhat, unsigned int numOfElements);
void modify_merged_hist( HIST_LIST_ELEM2 *merged_stacked_histogram, 
												long *X, unsigned int numOfElements, 
												unsigned int Hit_count);
void	create_pattern_list_element_SCF( 
																	 PATTERN_LIST_ELEM_SCF *pat_list_top, 
																	 unsigned int r, 
																	 unsigned int c, 
																	 unsigned int numOfElements);
void get_exchange_weights( EXCHANGE_STRUC_SCF *exchange,
													unsigned int numOfElements);
void update_pattern_list_SCF( EXCHANGE_STRUC_SCF *exchange,
													long *X, 
													unsigned int numOfElements,
													int X_in_merged_hist_flag, 
													unsigned int maxCountESC,
													unsigned int rescaling_threshold, long *Xhat_MAP);
void free_pattern_list_SCF( PATTERN_LIST_ELEM_SCF *pat_list_top);
void palette_to_histogram_SCF( HIST_LIST_ELEM2 *palette, 
														unsigned long *H, long *Xhat_Merged, long *Xhat_MAP,
								 unsigned int numOfElements, 
								 int size_flag, long diffMaxCalculated);
void greyPalette_to_histogram_SCF( HIST_LIST_ELEM2 *palette, 
														unsigned long *H, 
														long *Xhat_MAP,
								 int size_flag, int meanAbs);
int getSCF_tolerances_tree( PicData *pd,	AutoConfig *aC, ImageFeatures *iF);
void getSCF_tolerances_HXY( PicData *pd,	AutoConfig *aC, ImageFeatures *iF);

void rotateImage( PicData *pd, unsigned int rotateMode, int encodeDecodeFlag);

#ifdef _DEBUG
extern unsigned int r_glob; /* defined in scf.c	*/
#endif