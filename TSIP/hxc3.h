/*****************************************************
* File..: hxc3.h
* Desc..: declarations for H(X|C)3 coder
* Author: T. Strutz
* Date..: 02.09.2015
******************************************************/
#include <stdlib.h>
//#include "hxc.h"
#include "ginterface.h"
#include "bitbuf.h"
#include "arithmetic.h"
#include "autoConfig.h"

#ifdef _DEBUG
  #define H_DEBUG
#endif

#define MAX_DEBUG_POS 21100
#define HXC3_EST_INF

/*     
 * D C B A X
 */
#define PATTERN_SIZE3 4 /* A B C D */	/* do not change !!!*/

#define HXC3_MAX_MAXVAL 2000	/* square must be allocatable	*/
#define MAX_HIST_SIZE3 30000 /* maximal size of stacked histogram in HXC2	*/


/* compact histogram in list structure */
typedef struct HIST_LIST_ELEM3
{
	HIST_LIST_ELEM3 *next;
	long value;	/* signal value, in maximum four components, for example RGB-A	*/
	unsigned long count;	/* count of this value */
}HIST_LIST_ELEM3;


/* structure of a single element on pattern list	*/
typedef struct PATTERN_LIST_ELEM3
{
	PATTERN_LIST_ELEM3 *next, *prev; /* connection to predecessor and successor	*/
	unsigned long pos;	/* position of pattern in original array	*/
	HIST_LIST_ELEM3 *top_hist_elem;	/* pointer to top element of histogram list */
	/* it is a conditional histogram, which values occur how often in connection 
	   with this pattern */
	unsigned int increment_value;	/* start with high number, then decrement */
	unsigned int hist_size;	/* number of elements in histogram	*/
} PATTERN_LIST_ELEM3;


/* head of pattern list	*/
typedef struct 
{
	unsigned long pattern_list_size;
	PATTERN_LIST_ELEM3 top;
}PATTERN_LIST3;


/* structure of context tree	*/
//typedef struct C_TREE_NODE3
//{
//	unsigned long count;
//	unsigned long val;
//	C_TREE_NODE3 *next_item; /* next node at same level	*/
//	C_TREE_NODE3 *next_level; /* node at next level	*/
//	unsigned long lfdNr;
//} C_TREE_NODE3;



typedef struct /*structure for exchanging information */ 
{
	PATTERN_LIST_ELEM3 **similar_patterns; /* array of pointer to similar patterns	*/
	unsigned int *similar_size; /* array of number of similar pattern items	*/
	unsigned int bestSimilarity;
	unsigned long K_;	/* size of histogram, number of bins	*/
}EXCHANGE_STRUC3;


void HXC3_coding( unsigned int *lptr, unsigned long arr_length,
									long maxVal,
								 BitBuffer *bitbuf, 
								 AutoConfig *aC, 
								 unsigned int chan,
								 int encoder_flag);


void get_distribution3( unsigned int *lptr, unsigned long pos, long maxVal,
													long *t_actual, long *t_plist, 
													PATTERN_LIST3 *pat_list,
													EXCHANGE_STRUC3 *exchange, 
													HIST_LIST_ELEM3 *merged_stacked_histogram, 
													long *tolerance,
													unsigned int max_similar_pattern,
													unsigned int increaseFac,
													unsigned int adressing_mode);
void update_pattern_list3( EXCHANGE_STRUC3 *exchange,
													long X,
													int included_flag, unsigned int maxCount,
													unsigned int maxCountFac, unsigned long pos);


void	create_pattern_list_element3( 
				PATTERN_LIST_ELEM3 *pat_list_top, 
				unsigned long pos,
				unsigned int increment_value);

void free_stacked_hist3( HIST_LIST_ELEM3 *stacked_histogram);

void free_pattern_list3( PATTERN_LIST_ELEM3 *pat_list_top);


HIST_LIST_ELEM3 * create_histogram_list_element3( 
				long X, 
				unsigned int increment_value);

void	create_cumulative_histogram( unsigned long *H, 
					unsigned long K);
//void modify_distribution3( HIST_LIST_ELEM3 *top_hist_elem, 
//												 unsigned long *H);
void merge_stacked_histogram3( HIST_LIST_ELEM3 *top_hist_elem, 
									HIST_LIST_ELEM3 *merged_stacked_histogram,
									unsigned long *K_,
									unsigned int increaseFac,
									unsigned long pos);	/* just for debugging	*/
int X_in_merged_hist3( HIST_LIST_ELEM3 *merged_stacked_histogram, 
										 long X,
										 unsigned long *symbol, unsigned long *H, unsigned long *K);
void get_merged_hist3( HIST_LIST_ELEM3 *merged_stacked_histogram, 
										 unsigned long *H, unsigned long *K);

void get_X_from_merged_hist3( HIST_LIST_ELEM3 *merged_stacked_histogram, 
								long *X,  unsigned long symbol);
void integrate_X3( HIST_LIST_ELEM3 *palette, long X, 
								 unsigned long *numOfPaletteItems);
unsigned long X_in_palette3( HIST_LIST_ELEM3 *palette, long X, 
								  unsigned long *H);
void get_X_from_palette3( HIST_LIST_ELEM3 *palette, unsigned long symbol, 
								 long *X);

void copy_pattern3( unsigned int *lptr, unsigned long pos, long *templ);


long *lvector( long N);
void free_lvector( long *v[]);
