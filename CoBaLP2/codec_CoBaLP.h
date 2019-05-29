/************************************************
 *
 * File...:	codec_CoBaLP.h
 * Descr..:	Definitionen fuer Praediktion
 * Author.:	Tilo Strutz
 * Date...: 	16.10.2001
 * changes:
 * 08.01.2013 increase predcition order by Rk
 * 22.03.2013 reorganisation of #defines
 * 15.04.2013 new elements
 * 18.07.2013 new: constant_lines_mode
 * 13.06.2014 signum function included
 * 27.07.2018 added num_contexts in structure
 ************************************************/
#ifndef CODEC_H
#define CODEC_H

#include <stdio.h>
#include <malloc.h>

//#define NOSignFlip /* do not flip sign of e[n]	*/
//#define NOMext /* do not use Mext models for AC		*/
//#define FIXED_AC_CONTEXTS /* see get_AC_thresholds, fixed limits are used	*/
//#define FULL_ALPHABET  /* no tail truncation in AC	*/

extern unsigned int ERRCODE; /* in p_main.c */

#ifdef _DEBUG
	#define coding_verbose 1
#else
	#define coding_verbose 0
	//#define coding_verbose 1
#endif

/*
 * MACROS
 */


#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#ifndef sgnx
#define sgnx(x) ( ((x) < 0) ? (-1) : ((x) > 0) )
#endif

/*** Speicher belegen und Rückgabe prüfen ***/
#ifndef CALLOC
#define CALLOC(_ptr, _num, _type)								\
{																								\
	_ptr = (_type *)calloc((_num), sizeof(_type));\
	if (_ptr == NULL)															\
	{																							\
		fprintf(stderr, "calloc failed for %s(%d) %s\n", #_num, _num, #_type);	\
		ERRCODE = MALLOC_FAILED;										\
	}																							\
}
#endif

#ifndef ALLOC
#define ALLOC(_ptr, _num, _type)									\
{																									\
	_ptr = (_type *)malloc((_num) * sizeof(_type));	\
	if (_ptr == NULL)								\
	{																\
		fprintf(stderr, "malloc failed for %s %s\n", #_num, #_type);	\
		ERRCODE = MALLOC_FAILED;			\
	}																\
}
#endif

/*** Speicher freigeben ***/
#ifndef FREE
#define FREE(_ptr)								\
{																	\
	if (_ptr != NULL) free(_ptr);		\
	_ptr = NULL;										\
}
#endif

#define CLIP( a, b, c) 	(a) > (c) ? (c) : (a) < (b) ? (b) : (a);

#define MODULO_RANGE( e, l, u, r) \
	(e) < (l) ? (e) + (r) : 				\
	(e) > (u) ? (e) - (r) : (e);	


/* number of contexts, reserve 3 contexts for: 1st row, 1st col, RLC mode	*/
#define PCX_INCLUDE_RA


#define DIFF_TEXTURE	/* use differences for context determination	*/
//#undef DIFF_TEXTURE

#ifndef  DIFF_TEXTURE
	#undef PCX_INCLUDE_RA
#endif


//#define COMPUTE_NUMBER_OF_CONTEXTS( im)\
//			 (unsigned int)min( MAXNUM_OF_PRED_CONTEXTS-3, \
//											(im->width + im->height) / 20 );
//#define COMPUTE_NUMBER_OF_CONTEXTS( im)\
//			 (unsigned short)min( MAXNUM_OF_PRED_CONTEXTS-3, \
//											sqrt( (double)im->width * im->height) / 10 );
#define COMPUTE_NUMBER_OF_CONTEXTS( im)\
			 (unsigned short)min( MAXNUM_OF_PRED_CONTEXTS-3, \
											sqrt( (double)im->width * im->height) / 8 ); /* 2018*/
/*
 * Definitions
 */

#define LOG2	0.69314718	/* used for entropy calculation	*/

#define NUM_SKIPPED_STEPS 5 /* used for bias compensation	*/

#define NUM_KNOT_ELEM_LS 7	/* 	*/

#ifdef PCX_INCLUDE_RA
#define NUM_KNOT_ELEM 6	/* one for Ra	*/
#else
	#ifdef  DIFF_TEXTURE
		#define NUM_KNOT_ELEM 5	/* do not exceed  w/o changing contextExt()	*/
	#else
		#define NUM_KNOT_ELEM 6	/* do not exceed  w/o changing contextExt()	*/
	#endif
#endif
#define NUM_AC_CONTEXTS 40 /* max. number of contexts for arithmetic coding	*/

#define KNOT_STRING "# knots\n"

#define TH_FAC	10		/* factor for stretching the AC contexts	*/
/* offsets for extended image-data array	*/
#define OFF_TOP 3	/* three additional lines on top	*/
#define OFF_LEFT	4	/* four additional columns to the left	*/
#define OFF_RIGHT 3	/* three additional columns to the right	*/

/*** Typedefs für bitio.h ***/
typedef unsigned long ulong;
typedef unsigned int  uint;
typedef unsigned char byte;
typedef unsigned char cbool;
typedef unsigned short uword;

/*** Zur Verwendung mit bool ***/
#define TRUE 1u
#define FALSE 0

/*** Error Codes ***/
extern  uint ERRCODE;
#define PPM_OPEN_FAIL 1u
#define PPM_DAMAGED   2u
#define PPM_INVALID   3u
#define MALLOC_FAILED 4u
#define PRED_CRASHED  5u
#define CTX_NOT_SET   6u

#ifdef STAND_ALONE_COBALP
void(*put_pgm_value)();
long (*get_pgm_value)();
#endif


typedef struct {
	unsigned short num;
	unsigned long *distribution;
	double H,	/* single entropies	of states */
		Hcomb, /* entropies of merged distributions	*/
		Hburden;	/* difference between combined and single	*/
	unsigned long num_events; /* number of events in each state	*/
} CXE_STRUC;


typedef struct {
	int exhaustive_flag;
	int pred_mode;	/*  predictor selection	*/
	int paeth_mode;
	int compact_flag;
	int constant_lines_mode;
	int rotate_flag;
	unsigned int numOfConstantLines; /* count of rows and columns	*/
	unsigned short num_contexts;
	unsigned short num_contextsLS;
} PARAM_STRUC;


#endif
