/*******************************************************************
 *
 * File....: arithmetic.h
 * Function: Arithmetic coding - Functions declarations and variables
 * Author..: Dávid Bankó
 * Date....: 25.07.2012.
 *
 ********************************************************************/

#ifndef _ARITHMETICB_H_
#define _ARITHMETICB_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fibocode.h"
#include "tstypes.h"
#include "bitbuf.h"
#include "ginterface.h"

/* Constants */
#define MODEL_NUMBER 81
#ifndef ARITH_B
	#define ARITH_B 30
	#define	Q2	((unsigned long) 1 << (ARITH_B-1))
	#define	Q1	((unsigned long) 1 << (ARITH_B-2))
#endif
/* Values of auto-method evaluation */
#define ZEROS_UPPER	0.9
#define ZEROS_RUN	0.75
#define ZRUN_RUN	0.35
#define ZEROS_SYM	0.4
#define ZRUN_SYM	0.4
#define AVGF_LOW	3.5
#define AVGF_HIGH	4.5

/* Moving to static variables, defined a the start of coding/decoding */
#define MAX_N_BITS	((unsigned long)1 << 31)

typedef struct
{
	unsigned long *H;
	unsigned int K;
} acModellB;

/* Structure for series analysis, contains all the important properties */
typedef struct
{
	unsigned int max_symbol;		// Biggest symbol
	unsigned int second;			// Second biggest symbol - method selection (predict properties before shifting)
	unsigned int used_symbols;		// Number of different occured symbols
	unsigned int hist_start;		// The previous symbols of the first zero in the histogram
	unsigned int *histogram;		// The whole histogram
	unsigned int *symbols;			// The compressed occured symbol list
	unsigned int runs;				// Number of run-markers
	unsigned int follow;			// Biggest symbols following the run-marker
	float avgF;
	unsigned int zrun;				// Longest run of zeros in the histogram
} sStats;

/* Functions */
int SelectMethod( sStats *_statistics, char _series);

/* Analysis */
void Analysis( sStats *_statistics, unsigned int *_array, unsigned int _length);

/* Misc functions */
void start_modelB( acModellB *_model, uint _max);
void update_modelB( unsigned long *_freqArray, int _max, int _symbol);
void Shift( unsigned int *_array, unsigned int _length);
void UndoShift( unsigned int *_array, unsigned int _length);

/* Encoding functions */
void Encode_coreB( acModellB* _model, unsigned int _symbol, BitBuffer *_bitbuf);
void Finish_EncodeB( BitBuffer *_bitbuf);
void AC_Adv( unsigned int *_array, unsigned int _length, BitBuffer *_bitbuf, sStats *_statistics);
void AC_Marker( unsigned int *_array, unsigned int _length, BitBuffer *_bitbuf, sStats *_statistics);
void AC_Mixed( unsigned int *_array, unsigned int _length, BitBuffer *_bitbuf, sStats *_statistics);
void AC_Encode( unsigned int *_arrays, unsigned int _length, BitBuffer *_bitbuf, char _series);
void AC_adj( PicData* pd, uint numColours, uint* arr, BitBuffer* out);

/* Decoding Function */
unsigned int Decode_coreB( acModellB *_model, unsigned int _index, BitBuffer *_bitbuf, unsigned int _diff);
unsigned int Get_Est_Symbol( acModellB *_model, unsigned int _diff);
void ADC_Adv( unsigned int *_array, unsigned int _length, BitBuffer *_bitbuf);
void ADC_Marker( unsigned int *_array, unsigned int _length, BitBuffer *_bitbuf);
void ADC_Mixed( unsigned int *_array, unsigned int _length, BitBuffer *_bitbuf);
void AC_Decode( unsigned int *_arrays, unsigned int _length, BitBuffer *_bitbuf);
void ADC_adj( PicData* pd, unsigned int numColours, uint* arr, BitBuffer* in);

#endif