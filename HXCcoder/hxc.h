/*****************************************************
* File..: hxc.h
* Desc..: declarations for H(X|C) coder
* Author: T. Strutz
* Date..: 04.09.2013
 * 31.01.2014 new Parameters for HXC in aC->
 * 04.02.2014 definition removed, no Colour triple
 * 13.03.2014 new HXC_MAX_MAXVAL
 * 11.05.2014 fac in create_cumulative_histogram()
 * 09.07.2014 restore increaseValue management from version 1.25e
 * 20.08.2014 prototyping and structures for HXC2
* 24.04.2015 bugfix HXC2_TOL_EXCEPTION
* 29.09.2015 new:  MAX_TOL_A_B
******************************************************/
#include <stdlib.h>
#include "ginterface.h"
#include "bitbuf.h"
#include "arithmetic.h"
#include "autoConfig.h"


#ifndef HXC_H

#define HXC_H


#ifdef _DEBUG
//#define H_DEBUG
#endif

/* this tolerance is not allowed as it is used for signaling of
 * negative differences during sending the header infoermation
 */
#define HXC2_TOL_EXCEPTION	33 
/* maximum tolerance for positions A and B	*/
#define MAX_TOL_A_B					15

#define HXC2_EST_INF /* for debugging purposes	*/

/*     F
 *   C B D
 * E A X
 */
#define MAX_PATTERN_SIZE 6 /* A B C D E F */	/* do not change !!!*/
// obsolete #define NUM_OF_COMPONENTS 3	/* R G B */			/* do not change !!!*/


#define HXC_MAX_MAXVAL 2000	/* square must be allocatable	*/
//#define HXC_MAX_MAXVAL 1	/* use always linear addressing	*/
#define MAX_HIST_SIZE 30000 /* maximal size of stacked histogram in HXC2	*/

/* scales the counts in stage 2, can be beneficial, 
 * when a count becomes zero do to downscaling
 */
#define MODE1_INCREMENT 50



/* compact histogram in list structure */
typedef struct HIST_LIST_ELEM
{
	HIST_LIST_ELEM *next;
	long value;	/* signal value, in maximum three components, for example RGB	*/
	unsigned long count;	/* count of this value */
}HIST_LIST_ELEM;

/* compact histogram in list structure */
typedef struct HIST_LIST_ELEM2
{
	HIST_LIST_ELEM2 *next;
	long value[4];	/* signal value, in maximum four components, for example RGB-A	*/
	unsigned long count;	/* count of this value */
}HIST_LIST_ELEM2;

/* structure of a single element on pattern list	*/
typedef struct PATTERN_LIST_ELEM
{
	PATTERN_LIST_ELEM *next, *prev; /* connection to predecessor and successor	*/
	unsigned int r;	/* position of pattern in original image	*/
	unsigned int c;	/* position of pattern in original image	*/
	HIST_LIST_ELEM *top_hist_elem;	/* pointer to top element of histogram list */
	/* it is a conditional histogram, which values occur how often in connection 
	   with this pattern */
	unsigned int increment_value;	/* start with high number, then decrement */
} PATTERN_LIST_ELEM;

/* structure of a single element on pattern list	*/
typedef struct PATTERN_LIST_ELEM2
{
	PATTERN_LIST_ELEM2 *next, *prev; /* connection to predecessor and successor	*/
	unsigned int r;	/* position of pattern in original image	*/
	unsigned int c;	/* position of pattern in original image	*/
	HIST_LIST_ELEM2 *top_hist_elem;	/* pointer to top element of histogram list */
	/* it is a conditional histogram, which values occur how often in connection 
	   with this pattern */
	//unsigned int increment_value;	/* start with high number, then decrement */
	unsigned int hist_size;	/* number of elements in histogram	*/
	unsigned int rescaling_count;	/* number how often this pattern has been seen	*/
} PATTERN_LIST_ELEM2;

/* head of pattern list	*/
typedef struct 
{
	unsigned long pattern_list_size;
	PATTERN_LIST_ELEM top;
}PATTERN_LIST;

/* head of pattern list	*/
typedef struct 
{
	unsigned long pattern_list_size;
	PATTERN_LIST_ELEM2 top;
}PATTERN_LIST2;


/* new format for storing a palette */ 
typedef struct /* 15.05.2014 Batev: */ 
{
	unsigned long Red;
	unsigned long Green;
	unsigned long Blue;
	signed long *simIndicesTol;
	signed long *simIndicesOff1;
	signed long *simIndicesOff2;
}PALETTE_ELEM;



/* structure of context tree	*/
typedef struct C_TREE_NODE
{
	unsigned long count;
	unsigned long val;
	C_TREE_NODE *next_item; /* next node at same level	*/
	C_TREE_NODE *next_level; /* node at next level	*/
	unsigned long lfdNr;
} C_TREE_NODE;

/* structure of YUV context tree	*/
typedef struct C_TREE_NODE_2
{
	unsigned long count;
	unsigned long val[4];
	C_TREE_NODE_2 *next_item; /* next node at same level	*/
	C_TREE_NODE_2 *next_level; /* node at next level	*/
	unsigned long lfdNr;
	HIST_LIST_ELEM2 *top_hist_elem;	/* pointer to top element of histogram list */
	/* this is only active at lowest level F containing the distribution of X of
	 * the entire pattern A..F
	 */
	unsigned int hist_size;	/* number of elements in histogram	*/
} C_TREE_NODE_2;



typedef struct /*structure for exchanging information */ 
{
	PATTERN_LIST_ELEM2 **similar_patterns; /* array of pointer to similar patterns	*/
	unsigned int *similar_size; /* array of number of similar pattern items	*/
	unsigned int bestSimilarity;
	unsigned long K_;	/* size of histogram, number of bins	*/
}EXCHANGE_STRUC;

void HXC_coding( PicData *pd, BitBuffer *bitbuf, 
								 AutoConfig *aC, 
								 int encoder_flag,
								 unsigned int comp);

void HXC2_coding( PicData *pd, BitBuffer *bitbuf, 
								 AutoConfig *aC, 
								 int encoder_flag);

void HXC_codingIdx( PicData *pd,		/* structure with image data	*/
								BitBuffer *bitbuf, /* interface for file input and output*/
								AutoConfig *aC,  /* parameter of codec */
								int encoder_flag,	/* 1 ==> encoder; 0=> decoder	*/
								unsigned int comp,  /* colour component	*/
								//unsigned int numColors,  /* Image features Added 15.05.2014 */ 
								unsigned char *palette);	/* Original palette Added 15.05.2014 */

void get_distribution( PicData *pd, unsigned int r, unsigned int c,
											long **templ, long **templ2, 
											PATTERN_LIST *pat_list, PATTERN_LIST_ELEM **similar_elements,
											unsigned long *H, unsigned long K_,
											unsigned int tolerance[],
											unsigned int max_similar_pattern,
											unsigned int HXCincreaseFac,
											unsigned int comp); 

#ifndef OLD_HXC2
void get_distribution2( PicData *pd, unsigned int r, unsigned int c,
													long **t_actual, long **t_plist, 
													PATTERN_LIST2 *pat_list,
													EXCHANGE_STRUC *exchange, 
													HIST_LIST_ELEM2 *merged_stacked_histogram, 
													long **tolerance,
													unsigned int max_similar_pattern,
													// unsigned int increaseFac,
													unsigned int adressing_mode,
													unsigned int pattern_size,
													int no_new_pattern_flag);
void update_pattern_list2( EXCHANGE_STRUC *exchange,
													long *X, unsigned int numOfElements,
													int included_flag, unsigned int maxCount);
#else
void get_distribution2( PicData *pd, unsigned int r, unsigned int c,
													long **t_actual, long **t_plist, 
													PATTERN_LIST2 *pat_list, PATTERN_LIST_ELEM2 **similar_patterns,
													HIST_LIST_ELEM2 *merged_stacked_histogram, unsigned long *K_,
													long **tolerance,
													unsigned int max_similar_pattern,
													unsigned int increaseFac);

void update_pattern_list2( PATTERN_LIST_ELEM2 **similar_patterns,
													long *X, unsigned int numOfElements,
													int included_flag, unsigned int maxCount);
#endif

void get_distribution_Idx( PicData *pd, unsigned int r, unsigned int c,
											long *templ, long *templ2, 
											PATTERN_LIST *pat_list, PATTERN_LIST_ELEM **similar_elements,
											unsigned long *H, unsigned long K_,
											unsigned int tolerance[],
											unsigned int max_similar_pattern,
											unsigned int HXCincreaseFac,
											unsigned int comp,
                      PALETTE_ELEM *palette);

void	create_pattern_list_element( 
				PATTERN_LIST_ELEM *parent_elem, 
				unsigned int r, 
				unsigned int c,
				unsigned int increment_value);

void	create_pattern_list_element2( 
				PATTERN_LIST_ELEM2 *pat_list_top, 
				unsigned int r, 
				unsigned int c, 
				unsigned int numOfElements);

void free_stacked_hist( HIST_LIST_ELEM2 *stacked_histogram);

void free_pattern_list( PATTERN_LIST_ELEM *pat_list_top);
void free_pattern_list2( PATTERN_LIST_ELEM2 *pat_list_top);

void update_pattern_list( PATTERN_LIST_ELEM **similar_elements,
													long X);

HIST_LIST_ELEM * create_histogram_list_element( long X,
				unsigned int increment_value);
HIST_LIST_ELEM2 * create_histogram_list_element2( 
				long X[4], unsigned int numOfElements, unsigned int count);

void	create_cumulative_histogram( unsigned long *H, 
					unsigned long K);
void modify_distribution( HIST_LIST_ELEM *top_hist_elem, 
												 unsigned long *H);
void merge_stacked_histogram( HIST_LIST_ELEM2 *top_hist_elem, 
									HIST_LIST_ELEM2 *merged_stacked_histogram,
									unsigned long *K_,
									unsigned int numOfElements);
int X_in_merged_hist( HIST_LIST_ELEM2 *merged_stacked_histogram, 
										 long *X, unsigned int numOfElements, 
										 unsigned long *symbol, unsigned long *H, 
										 unsigned long *K,
										 unsigned int increaseFac);
void get_merged_hist( HIST_LIST_ELEM2 *merged_stacked_histogram, 
										 unsigned long *H, unsigned long *K,
										 unsigned int increaseFac);

void get_X_from_merged_hist( HIST_LIST_ELEM2 *merged_stacked_histogram, 
								long *X, unsigned int numOfElements, unsigned long symbol);
void integrate_X( HIST_LIST_ELEM2 *palette, long *X, unsigned int numOfElements, 
								 unsigned long *numOfPaletteItems);
unsigned long X_in_palette( HIST_LIST_ELEM2 *palette, long *X, 
								 unsigned int numOfElements);
void palette_to_histogram( HIST_LIST_ELEM2 *palette, 
														unsigned long *H, long *Xhat,
								 unsigned int numOfElements, long *predTol, int flag);
void get_X_from_palette( HIST_LIST_ELEM2 *palette, unsigned long symbol, 
								 long *X, unsigned int numOfElements);

void copy_pattern( PicData *pd, unsigned int r, unsigned int c, 
									long **templ, unsigned int comp);

void copy_pattern2( PicData *pd, unsigned int r, unsigned int c, 
									long **templ);
void copy_patternIdx( PicData *pd, unsigned int r, unsigned int c, 
									long *templ, unsigned int comp);  /* Batev: Indexed Mode */

void autoTolerances( PicData *pd, 
										unsigned int range,
										AutoConfig *aC, 
										unsigned int comp);

/* 15.05.2014 Batev */

int BuildPalette(unsigned char *palette,       /* original palette */
				  unsigned int maxVal,      /*  */
				  PALETTE_ELEM *palettePointer, /* pointer to array for the new palette */
				  unsigned long max_sim_idx);    /* max number of possible similar indices */

void CollectSimilarPaletteElements( unsigned int maxVal, /*  */
																	 PALETTE_ELEM *palettePointer, /* pointer to array for the new palette */
																	 unsigned int tol, unsigned int Off1, unsigned int Off2, /* tolerances */
																	unsigned int max_sim_idx);
void getHXC2_tolerances( PicData *pd, AutoConfig *aC);
int getHXC2_tolerances_tree( PicData *pd, AutoConfig *aC, ImageFeatures *iF);



long **imatrix( long N, long M);
void free_imatrix( long **m[]);

/* LOCO functions	*/
#define NUM_OF_LOCO_CONTEXTS	365	/* (9 * ( 9 *  4 +  4) +  4)  + 1 */
															    /* (9 * ( 9 * Q1 + Q2) + Q3)  + 1 */
#define La lineL[comp][ r    & 0x1][c-1]
#define Lb lineL[comp][(r+1) & 0x1][c]
#define Lc lineL[comp][(r+1) & 0x1][c-1]
#define Ld lineL[comp][(r+1) & 0x1][c+1]
#define Lx lineL[comp][ r    & 0x1][c]


#define LOCO_S1 3	/* Default-Schwellen fuer Kontextquantisierung	*/
#define LOCO_S2 7
#define LOCO_S3 21

void vec_pred_init( unsigned int width, unsigned int numOfElements, 
									 long *low_lim, long *up_lim, long *range,
									 unsigned int *maxVal, long *data);
void vec_pred_update( int r, int c, long *X, long *Xhat, unsigned int numOfElements, 
										 int *sign, int *state, long *low_lim, long *up_lim,
										 long *range);
void vec_pred( unsigned int width, int r, int c, unsigned int *maxVal,
								unsigned int numOfElements, long *Xhat, 
								int *state, int *sign, unsigned int *k);
void vec_pred_finish( unsigned int numOfElements, long *data);


#endif /* HXC_H ----------------------------------*/