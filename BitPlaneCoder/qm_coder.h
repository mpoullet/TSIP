/*********************************************************************
* File	 	: qm_coder.h
* Purpose	: to make custom definitions;
*		      to declare structure types;
*		      to store estimator states;
*		      to provide function prototypes
* Changes	: 22.01.2014 Roeppischer: removed 'discard_0s' procedure
* Author 	: Mathias Roeppischer
* Date	 	: 04.11.2013
* Last Modif: 01.02.2014
* 31.03.2014 Strutz: modified declaration with *bit_array
*********************************************************************/

#ifndef QM_CODER_H
#define QM_CODER_H

#define STATES 113 /* number of states in MARKOV chain Qtbl[] */

#include "bitbuf.h"
#include "ginterface.h"

/*--------------------------------------------------------------------
* typedef struct CDC_REG
*
* Codec Registers;
* contains registers for both encoder and decoder
*
* a  : current probability interval (augend)
* c  : trailing bits of bitstream
* cnt: counter for remaining shift-left-logical operations on
*	   renormalization until one byte of data is removed from code
*	   register
*-------------------------------------------------------------------*/
typedef struct
{
	uint
		a,
		c;

	byte
		cnt;
}
CDC_REG;

/*--------------------------------------------------------------------
* typedef struct CTX_IDX
*
* Context Index;
* function of previous coding decisions
*
* q_idx: conditional probability estimate identifier used in
*		 encoding a binary decision
* q_val: fixed precision integer value equivalent to fractional
*		 value representing probability for less probable symbol
* mps  : sense of more probable symbol
*-------------------------------------------------------------------*/
typedef struct
{
	uword
		q_val;

	byte
		q_idx;

	cbool
		mps;
}
	CTX_IDX;

/*--------------------------------------------------------------------
* typedef struct FSM_EST
*
* Finite State Machine;
* consists of finite number of sequences of probability estimates
*
*
* q_idx		 : LPS probability estimate identifier
* q_val		 : hexadecimal probability LPS estimate
* nxt_idx_lps: next probability estimate identifier after LPS
*			   renormalization
* nxt_idx_mps: next probability estimate identifier after MPS
*			   renormalization
* swt_mps	 : conitional exchange indicator (MPS <> LPS);
*			   LPS-subinterval larger than MPS-subinterval (1) or
*			   not (0)
*-------------------------------------------------------------------*/
typedef struct
{
	byte
		q_idx;

	uword
		q_val;

	byte
		nxt_idx_lps,
		nxt_idx_mps;

	cbool
		swt_mps;
}
FSM_EST;

/*--------------------------------------------------------------------
* static const FSM_EST Qtbl[]
*
* Qe table;
* structure constant representing probability estimation state machine
* with HEX LPS probability estimates
*
* 'NT' marks nontransient state
*
* for parameters see: struct FSM_EST
*-------------------------------------------------------------------*/
static const FSM_EST Qtbl[STATES] =
{
	{  0, 0x5a1d,   1,   1, 1},
	{  1, 0x2586,  14,   2, 0},
	{  2, 0x1114,  16,   3, 0},
	{  3, 0x080b,  18,   4, 0},
	{  4, 0x03d8,  20,   5, 0},
	{  5, 0x01da,  23,   6, 0},
	{  6, 0x00e5,  25,   7, 0},
	{  7, 0x006f,  28,   8, 0},
	{  8, 0x0036,  30,   9, 0},
	{  9, 0x001a,  33,  10, 0},	/* NT */
	{ 10, 0x000d,  35,  11, 0},	/* NT */
	{ 11, 0x0006,   9,  12, 0},	/* NT */
	{ 12, 0x0003,  10,  13, 0},	/* NT */
	{ 13, 0x0001,  12,  13, 0},	/* NT */
	{ 14, 0x5a7f,  15,  15, 1},
	{ 15, 0x3f25,  36,  16, 0},
	{ 16, 0x2cf2,  38,  17, 0},
	{ 17, 0x207c,  39,  18, 0},
	{ 18, 0x17b9,  40,  19, 0},
	{ 19, 0x1182,  42,  20, 0},
	{ 20, 0x0cef,  43,  21, 0},
	{ 21, 0x09a1,  45,  22, 0},
	{ 22, 0x072f,  46,  23, 0},
	{ 23, 0x055c,  48,  24, 0},
	{ 24, 0x0406,  49,  25, 0},
	{ 25, 0x0303,  51,  26, 0},
	{ 26, 0x0240,  52,  27, 0},
	{ 27, 0x01b1,  54,  28, 0},
	{ 28, 0x0144,  56,  29, 0},
	{ 29, 0x00f5,  57,  30, 0},
	{ 30, 0x00b7,  59,  31, 0},
	{ 31, 0x008a,  60,  32, 0},
	{ 32, 0x0068,  62,  33, 0},	/* NT */
	{ 33, 0x004e,  63,  34, 0},	/* NT */
	{ 34, 0x003b,  32,  35, 0},	/* NT */
	{ 35, 0x002c,  33,   9, 0},	/* NT */
	{ 36, 0x5ae1,  37,  37, 1},
	{ 37, 0x484c,  64,  38, 0},
	{ 38, 0x3a0d,  65,  39, 0},
	{ 39, 0x2ef1,  67,  40, 0},
	{ 40, 0x261f,  68,  41, 0},
	{ 41, 0x1f33,  69,  42, 0},
	{ 42, 0x19a8,  70,  43, 0},
	{ 43, 0x1518,  72,  44, 0},
	{ 44, 0x1177,  73,  45, 0},
	{ 45, 0x0e74,  74,  46, 0},
	{ 46, 0x0bfb,  75,  47, 0},
	{ 47, 0x09f8,  77,  48, 0},
	{ 48, 0x0861,  78,  49, 0},	/* NT */
	{ 49, 0x0706,  79,  50, 0},	/* NT */
	{ 50, 0x05cd,  48,  51, 0},	/* NT */
	{ 51, 0x04de,  50,  52, 0},	/* NT */
	{ 52, 0x040f,  50,  53, 0},	/* NT */
	{ 53, 0x0363,  51,  54, 0},	/* NT */
	{ 54, 0x02d4,  52,  55, 0},	/* NT */
	{ 55, 0x025c,  53,  56, 0},	/* NT */
	{ 56, 0x01f8,  54,  57, 0},	/* NT */
	{ 57, 0x01a4,  55,  58, 0},	/* NT */
	{ 58, 0x0160,  56,  59, 0},	/* NT */
	{ 59, 0x0125,  57,  60, 0},	/* NT */
	{ 60, 0x00f6,  58,  61, 0},	/* NT */
	{ 61, 0x00cb,  59,  62, 0},	/* NT */
	{ 62, 0x00ab,  61,  63, 0},	/* NT */
	{ 63, 0x008f,  61,  32, 0},	/* NT */
	{ 64, 0x5b12,  65,  65, 1},
	{ 65, 0x4d04,  80,  66, 0},
	{ 66, 0x412c,  81,  67, 0},
	{ 67, 0x37d8,  82,  68, 0},
	{ 68, 0x2fe8,  83,  69, 0},
	{ 69, 0x293c,  84,  70, 0},
	{ 70, 0x2379,  86,  71, 0},
	{ 71, 0x1edf,  87,  72, 0},	/* NT */
	{ 72, 0x1aa9,  87,  73, 0},	/* NT */
	{ 73, 0x174e,  72,  74, 0},	/* NT */
	{ 74, 0x1424,  72,  75, 0},	/* NT */
	{ 75, 0x119c,  74,  76, 0},	/* NT */
	{ 76, 0x0f6b,  74,  77, 0},	/* NT */
	{ 77, 0x0d51,  75,  78, 0},	/* NT */
	{ 78, 0x0bb6,  77,  79, 0},	/* NT */
	{ 79, 0x0a40,  77,  48, 0},	/* NT */
	{ 80, 0x5832,  80,  81, 1},
	{ 81, 0x4d1c,  88,  82, 0},
	{ 82, 0x438e,  89,  83, 0},
	{ 83, 0x3bdd,  90,  84, 0},
	{ 84, 0x34ee,  91,  85, 0},
	{ 85, 0x2eae,  92,  86, 0},
	{ 86, 0x299a,  93,  87, 0},	/* NT */
	{ 87, 0x2516,  86,  71, 0},	/* NT */
	{ 88, 0x5570,  88,  89, 1},
	{ 89, 0x4ca9,  95,  90, 0},
	{ 90, 0x44d9,  96,  91, 0},
	{ 91, 0x3e22,  97,  92, 0},
	{ 92, 0x3824,  99,  93, 0},
	{ 93, 0x32b4,  99,  94, 0},	/* NT */
	{ 94, 0x2e17,  93,  86, 0},	/* NT */
	{ 95, 0x56a8,  95,  96, 1},
	{ 96, 0x4f46, 101,  97, 0},
	{ 97, 0x47e5, 102,  98, 0},
	{ 98, 0x41cf, 103,  99, 0},
	{ 99, 0x3c3d, 104, 100, 0},	/* NT */
	{100, 0x375e,  99,  93, 0},	/* NT */
	{101, 0x5231, 105, 102, 0},
	{102, 0x4c0f, 106, 103, 0},
	{103, 0x4639, 107, 104, 0},	/* NT */
	{104, 0x415e, 103,  99, 0},	/* NT */
	{105, 0x5627, 105, 106, 1},
	{106, 0x50e7, 108, 107, 0},
	{107, 0x4b85, 109, 103, 0},	/* NT */
	{108, 0x5597, 110, 109, 0},
	{109, 0x504f, 111, 107, 0},	/* NT */
	{110, 0x5a10, 110, 111, 1},
	{111, 0x5522, 112, 109, 0},	/* NT */
	{112, 0x59eb, 112, 111, 1},	/* NT */
};

/*--------------------------------------------------------------------
* BA_decode()
*
* determines whether LPS or MPS for given context index shall be
* decoded unless conditional exchange is needed
*
* bitbuf   : pointer to bit buffer of TSIPcoder
* ctx      : pointer to context instance structure
* *qm_bytes: pointer to QM byte counter
* reg      : pointer to encoder register
* *u_flg   : pointer to 'uncoded' flag 
*
* returns a binary digit (0 or 1)
*-------------------------------------------------------------------*/
cbool
BA_decode( unsigned char *bit_array, CDC_REG *reg, CTX_IDX
		  *ctx, ulong *byte_pos);

/*--------------------------------------------------------------------
* BA_encode()
*
* encodes binary value (0 or 1)
*
* bitbuf_var: pointer to variant of BitBuffer structure
* BitVal	: binary decision (0 or 1) to be coded
* ctx		: pointer to context instance structure
* *nxt_byte : pointer to byte buffer of QMcoder
* *qm_bytes	: pointer to QM byte counter
* reg		: pointer to encoder register
* stk		: pointer to stack counter
*-------------------------------------------------------------------*/
void
BA_encode( unsigned char *bit_array, byte *nxt_byte, cbool bit_in,
		  CDC_REG *reg, CTX_IDX *ctx, rsize_t *stk, ulong *qm_bytes);

/*--------------------------------------------------------------------
* byte_in()
*
* fetches one byte of data and compensates for stuffed 0 byte
* following X'FF (fill) byte
*
* bitbuf   : pointer to bit buffer of TSIPcoder
* *qm_bytes: pointer to QM byte counter
* reg      : pointer to encoder register
* *u_flg   : pointer to 'uncoded' flag
*-------------------------------------------------------------------*/
void byte_in( unsigned char *bit_array, CDC_REG *reg, 
				ulong	*byte_pos);

/*--------------------------------------------------------------------
* byte_out()
*
* removes one byte of compressed data from code register
*
* bitbuf_var: pointer to variant of BitBuffer structure
* *nxt_byte	: pointer to byte buffer of QMcoder
* *qm_bytes : pointer to QM byte counter
* reg		: pointer to encoder register
* stk		: pointer to stack counter
*-------------------------------------------------------------------*/
void
byte_out( unsigned char *bit_array, 
					byte *nxt_byte, /* carry bits */
					CDC_REG *reg,
					rsize_t *stk, 
					ulong *qm_bytes /* number of written bytes	*/
		 );

/*--------------------------------------------------------------------
* est_q_lps()
*
* gets new LPS probability estimate from probability estimation state
* table on LPS renormalization path
*
* ctx: pointer to context instance structure
*-------------------------------------------------------------------*/
void
est_q_lps( CTX_IDX *ctx);

/*--------------------------------------------------------------------
* est_q_mps()
*
* gets new LPS probability estimate from probability estimation state
* table on MPS renormalization path
*
* ctx: pointer to context instance structure
*-------------------------------------------------------------------*/
void
est_q_mps( CTX_IDX *ctx);

/*--------------------------------------------------------------------
* flush()
*
* terminates binary arithmetic coding and prepares entropy-coded
* segment for addition of marker prefix following arithmetically coded
* data
*
* bitbuf_var: pointer to variant of BitBuffer structure
* *nxt_byte	: pointer to byte buffer of QMcoder
* *qm_bytes : pointer to QM byte counter
* reg		: pointer to encoder register
* stk		: pointer to stack counter
*-------------------------------------------------------------------*/
void
flush( unsigned char *bit_array, byte *nxt_byte, CDC_REG *reg, rsize_t
	  *stk, ulong *qm_bytes);

/*--------------------------------------------------------------------
* init_dec()
*
* initializes arithmetic decoder
*
* bitbuf   : pointer to bit buffer of TSIPcoder
* *qm_bytes: pointer to QM bytes counter
* reg	   : pointer to encoder register
* *u_flg   : pointer to 'uncoded' flag
*-------------------------------------------------------------------*/
void
init_dec( unsigned char *bit_array, CDC_REG *reg, ulong *byte_pos);

/*--------------------------------------------------------------------
* init_enc()
*
* initializes arithmetic encoder
*
* reg: pointer to encoder register
* stk: pointer to stack counter
*-------------------------------------------------------------------*/
void init_enc( CDC_REG *reg, rsize_t *stk);

/*--------------------------------------------------------------------
* mod_con()
*
* initiates context modeling
*
* chan   : number of current channel
* firstBP: number of first bit plane to be encoded
* pd     : pointer to struct PicData
* x	     : horizontal pel position
* y      : vertical pel position
* z      : number of current bit plane
*
* returns context instance ID
*-------------------------------------------------------------------*/
rsize_t
mod_con( int firstBP, int z, PicData *pd, rsize_t chan, rsize_t x,
		rsize_t y);

/*--------------------------------------------------------------------
* renorm_dec()
*
* doubles augend and code register until size of augend is at least
* 0.75 when decoding
*
* bitbuf   : pointer to bit buffer of TSIPcoder
* *qm_bytes: pointer to QM byte counter
* reg      : pointer to encoder register
* *u_flg   : pointer to 'uncoded' flag
*-------------------------------------------------------------------*/
void
renorm_dec( unsigned char *bit_array, CDC_REG *reg, ulong *byte_pos);

/*--------------------------------------------------------------------
* renorm_enc()
*
* doubles augend and code register until size of augend is at least
* 0.75 when encoding
*
* bitbuf_var: pointer to variant of BitBuffer structure
* *nxt_byte	: pointer to byte buffer of QMcoder
* *qm_bytes : pointer to QM byte counter
* reg		: pointer to encoder register
* stk		: pointer to stack counter
*-------------------------------------------------------------------*/
void
renorm_enc( unsigned char *bit_array, 
						byte *nxt_byte, CDC_REG *reg,
						rsize_t *stk, ulong *qm_bytes);

#endif