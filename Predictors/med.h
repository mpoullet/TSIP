/************************************************
 *
 * File..:	med.h
 * Descr.:	Definitionen fuer Praediktion und Codierung
 * Author:	Tilo Strutz
 * Datum: 	24.01.2011
 * 31.03.2014 larger type: long	max_value
 ************************************************/
#ifndef MED_H
#define MED_H

#include <stdio.h>



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


#define NUM_OF_CONTEXTS	365	/* 9 * ( 9 * 4 + 4) + 4)  + 1*/

/*
#define Ra line[ r    & 0x1][c-1]
#define Rb line[(r+1) & 0x1][c]
#define Rc line[(r+1) & 0x1][c-1]
#define Rd line[(r+1) & 0x1][c+1]
#define Rx line[ r    & 0x1][c]
*/

#define S1 3	/* Default-Schwellen fuer Kontextquantisierung	*/
#define S2 7
#define S3 21


typedef struct { 
	short state; 
	short sgn; 
	short rlc_flag; 
	long xHat; 
} PRED_STRUC;

#ifndef IMAGE_LOCO
typedef struct {
	unsigned int
		width,		/* Bildbreite 	*/
		height;		/* Bildhoehe 	*/
	unsigned long
		size;		/* Anzahl der Bildpunkte	*/
	int	bit_depth;	/* Bittiefe der Daten	*/
	int	sign_flag;	/* 1=>Daten sind vorzeichenbehaftet*/
	long	dc_shift;	/* ergibt sich aus Bittiefe	*/
	long	max_value;	/* groester Wert im Bild	*/
} IMAGE_LOCO;
#endif

typedef struct {
	int	bpp_flag;	/* */
	int	rlc_flag;	/* */
	int	esc_flag;	/* */
	int	nearlossless;
	int	reset, reset_flag;
	unsigned int	T1, T2, T3, T_flag;	
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


/*---------------------
Liste der Praediktoren

0	A + C - B
6	A
8	B
-----------------------*/


/* long *line[2]; */


extern long medN[];
extern long medC[];
extern long medA[];
extern long medB[];

/* proto typing */

void init_constants( IMAGE_LOCO *im, OPTION *op, CON *pc);
void init_contexts( OPTION *op, CON *pc);
void free_q( void);

#endif
