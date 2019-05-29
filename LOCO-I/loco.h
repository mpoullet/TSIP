/************************************************
 *
 * File....:	loco.h
 * Function:	Definitionen fuer Praediktion und Codierung
 * Data....: 	14.02.2011
 * 04.07.2011 include T4
 ************************************************/
#ifndef RICE_H
#define RICE_H
#

#include <stdio.h>
#include "bitbuf.h"



typedef struct {
	int	bpp_flag;	/* */
	int	rlc_flag;	/* */
	int	esc_flag;	/* */
	int	nearlossless;
	int	reset, reset_flag;
	unsigned int	T_flag, T4_flag;	
	long	T1, T2, T3, T4;	
} OPTION;


typedef struct {	/* Struktur fuer alle Konstanten	*/
	long	MAXVAL;	/* groeszter Wert im Bild	*/
	long	RANGE;	/* [...)-Wertebereich der quant. Grauwerte	*/
	long	RANGE_R;/* [...)-Wertebereich der rekonstr. Grauwerte
			    = RANGE * NEARm2p1	*/
	long	RANGEd2; /*  (RANGE ) / 2	*/
/*	long	RANGEp1d2; /*  (RANGE + 1 ) / 2	*/
	long	RANGEm1d2; /*  (RANGE - 1 ) / 2  should be identical to ceil(RANGE / 2) - 1;  */
	int	qbpp;	/* Bittiefe von RANGE	*/
	int	LIMIT;	/* fue RLC interupt-Codierung	*/
	int	LIMITq;	/* grenze fuer Rest-Codierung	*/
	int	NEAR;	/* Toleranz fuer near-lossless	*/
	int	NEARm2p1;/* NEAR * 2 + 1	*/
	int	RESET;	/* max. Zahl fuer Adaptation	*/
	int	MAX_C;	/* Begrenzer fuer C-Adaptation	*/
	int	MIN_C;	/* Begrenzer fuer C-Adaptation	*/
} CON;

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#define CLIP( a, b, c) 	(a) > (c) ? (c) : (a) < (b) ? (b) : (a);

#define MODULO_RANGE( e, l, u, r) 		\
	(e) < (l) ? (e) + (r) : 		\
		(e) > (u) ? (e) - (r) : (e);	


/* #define NUM_OF_CONTEXTS	365	/* (9 * ( 9 *  4 +  4) +  4)  + 1 */
															  /* (9 * ( 9 * Q1 + Q2) + Q3)  + 1 */
/* q4 corresponds to  new treshold T4 */
#define NUM_OF_CONTEXTS	730	/* ((9 * ( 9 * 4  +  4) +  4) * 2 +  1) + 1 */
														/* ((9 * ( 9 * Q1 + Q2) + Q3) * 2 + Q4) + 1 */

#define Ra liner[ r    & 0x1][c-1]
#define Rb liner[(r+1) & 0x1][c]
#define Rc liner[(r+1) & 0x1][c-1]
#define Rd liner[(r+1) & 0x1][c+1]
#define Rx liner[ r    & 0x1][c]


#define S1 3	/* Default-Schwellen fuer Kontextquantisierung	*/
#define S2 7
#define S3 21

/*---------------------
Liste der Praediktoren

0	A + C - B
6	A
8	B
-----------------------*/


extern long *liner[2];

extern long Nr[NUM_OF_CONTEXTS+2], Cr[NUM_OF_CONTEXTS+2]; 
extern long Ar[NUM_OF_CONTEXTS+2], Br[NUM_OF_CONTEXTS+2]; 


typedef struct { 
	short state; 
	short sgn; 
	short rlc_flag; 
	long xHat; 
} PRED_STRUC;

typedef struct {
	unsigned int
		width,		/* Bildbreite 	*/
		height;		/* Bildhoehe 	*/
	unsigned long
		size;		/* Anzahl der Bildpunkte	*/
	int	bit_depth;	/* Bittiefe der Daten	*/
	int	sign_flag;	/* 1=>Daten sind vorzeichenbehaftet*/
	long	dc_shift;	/* ergibt sich aus Bittiefe	*/
	long max_value;	/* groester Wert im Bild	*/
} IMAGE_LOCO;


void
free_qR( void),
init_contextsR( OPTION *op, CON *c),
init_constantsR( IMAGE_LOCO *im, OPTION *op, CON *pc),
encode_runlength( int Runcnt, int EOLine, BitBuffer *bitbuf),
rice_encode_R( unsigned long value, int k, int limit, int qbpp, 
						 BitBuffer *bitbuf),
rice_encode_escR( unsigned long value, int k, int limit, int qbpp,
						 BitBuffer *bitbuf),
predictor( IMAGE_LOCO *im, OPTION *op, CON *pc, PRED_STRUC *pp, 
						 int r, int c);

long 
encode_EndOfRun_nl( CON *pc, long x, long a, long b, 
									 BitBuffer *bitbuf);

#endif
