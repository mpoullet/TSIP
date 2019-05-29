/*******************************************************************
 *
 * File....: arithmetic.h
 * Function: Arithmetic coding - Functions declarations and variables
 * Author..: Strutz
 * Date....: 04.03.2013
 *
 * 10.04.2014 unsigned long K; support BACK_BITS
 ********************************************************************/

#ifndef _ARITHMETIC_H_
#define _ARITHMETIC_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bitbuf.h"


//extern unsigned long ac_diff; /* value for arithmetic decoding	*/

/* Constants */
//#define MODEL_NUMBER 81
#ifndef ARITH_B
#define ARITH_B 30
//#define ARITH_B 10
#define	Q2	((unsigned long) 1 << (ARITH_B-1))
#define	Q1	((unsigned long) 1 << (ARITH_B-2))

#define BACK_BITS
#undef BACK_BITS

#endif

/* Moving to static variables, defined a the start of coding/decoding */
//#define MAX_N_BITS	((unsigned long)1 << 31)

typedef struct
{
	unsigned long *H;
	unsigned long K;
} acModell;

typedef struct
{
	unsigned long low, range;
	unsigned long n_bits;
	unsigned long ac_diff, H_range;
	unsigned long pos;
} acState;

/* Misc functions */
void start_model( acModell *_model, unsigned long  _max);
void update_model( unsigned long *H, unsigned long K, unsigned long symbol);
void free_model( acModell *M);

/* Encoding functions */
void start_encoding( acState *acSt);
void encode_AC( acModell* _model, acState *acSt, unsigned long symbol, BitBuffer *_bitbuf);
void finish_encode( acState *acSt, BitBuffer *_bitbuf);

/* Decoding Function */
void start_decoding( acState *acSt, BitBuffer *bitbuf);
unsigned long decode_AC( acModell *M, acState *acSt, BitBuffer *in);
//unsigned long Get_Est_Symbol( acModell *_model);

#endif