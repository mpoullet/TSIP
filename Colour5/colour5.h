/*****************************************************************
 *
 * File...:	colour5.h
 * Purpose:	definitions for colour transforms and related
 * Author.:	Tilo Strutz
 * Date...: 09.06.2016
 * changes:
 * 09.06.2016 is intended to replace colour4.h unifícation w/o
 *            speed optimisation
 *****************************************************************/
#ifndef COLOUR_H
#define COLOUR_H

//
#include "image.h"

#define GREY	0
#define RGBc	1

#define	A1_1	2	/*	G	R-G	B-G						A2	4	*/	
#define	A1_2	3	/*	G	G-R	B-R	*/			
#define	A1_3	4	/*	G	R-B	G-B	*/			
#define	A1_4	5	/*	G	R-G	B-(R+3G)/4		D1	20	*/	
#define	A1_5	6	/*	G	G-R	B-(G+3R)/4		D3	22	*/	
#define	A1_6	7	/*	G	R-B	G-(R+3B)/4	*/			
#define	A1_7	8	/*	G	B-G	R-(B+3G)/4		D4	23	*/	
#define	A1_8	9	/*	G	G-B	R-(G+3B)/4		D6	25	*/	
#define	A1_9	10	/*	G	B-R	G-(B+3R)/4	*/			
#define	A1_10	11	/*	G	R-G	B-(R+G)/2		D2	21	*/	
#define	A1_11	12	/*	G	R-B	G-(R+B)/2		C2	12	*/	
#define	A1_12	13	/*	G	B-G	R-(B+G)/2		D5	24	*/	
										
#define	A2_1	14	/*	R	R-G	B-G	*/			
#define	A2_2	15	/*	R	G-R	B-R					A6	8	*/	
#define	A2_3	16	/*	R	R-B	G-B	*/			
#define	A2_4	17	/*	R	R-G	B-(R+3G)/4	D9	28	*/	
#define	A2_5	18	/*	R	G-R	B-(G+3R)/4	D7	26	*/	
#define	A2_6	19	/*	R	R-B	G-(R+3B)/4	D15	52	*/	
#define	A2_7	20	/*	R	B-G	R-(B+3G)/4	*/			
#define	A2_8	21	/*	R	G-B	R-(G+3B)/4	*/			
#define	A2_9	22	/*	R	B-R	G-(B+3R)/4	D13	50	*/	
#define	A2_10	23	/*	R	R-G	B-(R+G)/2		D8	27	*/	
#define	A2_11	24	/*	R	R-B	G-(R+B)/2		D14	51	*/	
#define	A2_12	25	/*	R	B-G	R-(B+G)/2		C6	16	*/	
										
#define	A3_1	26	/*	B	R-G	B-G	*/			
#define	A3_2	27	/*	B	G-R	B-R	*/			
#define	A3_3	28	/*	B	R-B	G-B					A7	9	*/	
#define	A3_4	29	/*	B	R-G	B-(R+3G)/4	*/			
#define	A3_5	30	/*	B	G-R	B-(G+3R)/4	*/			
#define	A3_6	31	/*	B	R-B	G-(R+3B)/4	D10	29	*/	
#define	A3_7	32	/*	B	B-G	R-(B+3G)/4	D18	55	*/	
#define	A3_8	33	/*	B	G-B	R-(G+3B)/4	D16	53	*/	
#define	A3_9	34	/*	B	B-R	G-(B+3R)/4	D12	31	*/	
#define	A3_10	35	/*	B	R-G	B-(R+G)/2		C7	17	*/	
#define	A3_11	36	/*	B	R-B	G-(R+B)/2		D11	30	*/	
#define	A3_12	37	/*	B	B-G	R-(B+G)/2		D17	54	*/	
										
#define	A4_1	38	/*	(G+R)/2	R-G	B-G	*/			
#define	A4_2	39	/*	(G+R)/2	G-R	B-R	*/			
#define	A4_3	40	/*	(G+R)/2	R-B	G-B	*/			
#define	A4_4	41	/*	(G+R)/2	R-G	B-(R+3G)/4	*/			
#define	A4_5	42	/*	(G+R)/2	G-R	B-(G+3R)/4	*/			
#define	A4_6	43	/*	(G+R)/2	R-B	G-(R+3B)/4	*/			
#define	A4_7	44	/*	(G+R)/2	B-G	R-(B+3G)/4	*/			
#define	A4_8	45	/*	(G+R)/2	G-B	R-(G+3B)/4	*/			
#define	A4_9	46	/*	(G+R)/2	B-R	G-(B+3R)/4	*/			
#define	A4_10	47	/*	(G+R)/2	R-G	B-(R+G)/2	*/			
#define	A4_11	48	/*	(G+R)/2	R-B	G-(R+B)/2	*/			
#define	A4_12	49	/*	(G+R)/2	B-G	R-(B+G)/2	*/			
										
#define	A5_1	50	/*	(G+B)/2	R-G	B-G	*/			
#define	A5_2	51	/*	(G+B)/2	G-R	B-R	*/			
#define	A5_3	52	/*	(G+B)/2	R-B	G-B	*/			
#define	A5_4	53	/*	(G+B)/2	R-G	B-(R+3G)/4	*/			
#define	A5_5	54	/*	(G+B)/2	G-R	B-(G+3R)/4	*/			
#define	A5_6	55	/*	(G+B)/2	R-B	G-(R+3B)/4	*/			
#define	A5_7	56	/*	(G+B)/2	B-G	R-(B+3G)/4	*/			
#define	A5_8	57	/*	(G+B)/2	G-B	R-(G+3B)/4	*/			
#define	A5_9	58	/*	(G+B)/2	B-R	G-(B+3R)/4	*/			
#define	A5_10	59	/*	(G+B)/2	R-G	B-(R+G)/2	*/			
#define	A5_11	60	/*	(G+B)/2	R-B	G-(R+B)/2	*/			
#define	A5_12	61	/*	(G+B)/2	B-G	R-(B+G)/2	*/			
										
#define	A6_1	62	/*	(R+B)/2	R-G	B-G	*/			
#define	A6_2	63	/*	(R+B)/2	G-R	B-R	*/			
#define	A6_3	64	/*	(R+B)/2	R-B	G-B	*/			
#define	A6_4	65	/*	(R+B)/2	R-G	B-(R+3G)/4	*/			
#define	A6_5	66	/*	(R+B)/2	G-R	B-(G+3R)/4	*/			
#define	A6_6	67	/*	(R+B)/2	R-B	G-(R+3B)/4	*/			
#define	A6_7	68	/*	(R+B)/2	B-G	R-(B+3G)/4	*/			
#define	A6_8	69	/*	(R+B)/2	G-B	R-(G+3B)/4	*/			
#define	A6_9	70	/*	(R+B)/2	B-R	G-(B+3R)/4	*/			
#define	A6_10	71	/*	(R+B)/2	R-G	B-(R+G)/2	*/			
#define	A6_11	72	/*	(R+B)/2	R-B	G-(R+B)/2	*/			
#define	A6_12	73	/*	(R+B)/2	B-G	R-(B+G)/2	*/			
										
#define	A7_1	74	/*	(R+2G+B)/4	R-G	B-G	*/	/*	YUVr	*/
#define	A7_2	75	/*	(R+2G+B)/4	G-R	B-R	*/			
#define	A7_3	76	/*	(R+2G+B)/4	R-B	G-B	*/			
#define	A7_4	77	/*	(R+2G+B)/4	R-G	B-(R+3G)/4	E1	32	*/	
#define	A7_5	78	/*	(R+2G+B)/4	G-R	B-(G+3R)/4	E3	34	*/	
#define	A7_6	79	/*	(R+2G+B)/4	R-B	G-(R+3B)/4	*/			
#define	A7_7	80	/*	(R+2G+B)/4	B-G	R-(B+3G)/4	E4	35	*/	
#define	A7_8	81	/*	(R+2G+B)/4	G-B	R-(G+3B)/4	E6	37	*/	
#define	A7_9	82	/*	(R+2G+B)/4	B-R	G-(B+3R)/4	*/			
#define	A7_10	83	/*	(R+2G+B)/4	R-G	B-(R+G)/2		E2	33	*/	
#define	A7_11	84	/*	(R+2G+B)/4	R-B	G-(R+B)/2	    YcgCoR*/			
#define	A7_12	85	/*	(R+2G+B)/4	B-G	R-(B+G)/2		E5	36	*/	
										
#define	A8_1	86	/*	(2R+G+B)/4	R-G	B-G	*/			
#define	A8_2	87	/*	(2R+G+B)/4	G-R	B-R					A4	6	*/	
#define	A8_3	88	/*	(2R+G+B)/4	R-B	G-B	*/			
#define	A8_4	89	/*	(2R+G+B)/4	R-G	B-(R+3G)/4	E9	40*/		
#define	A8_5	90	/*	(2R+G+B)/4	G-R	B-(G+3R)/4	E7	38	*/	
#define	A8_6	91	/*	(2R+G+B)/4	R-B	G-(R+3B)/4	E15	58	*/			
#define	A8_7	92	/*	(2R+G+B)/4	B-G	R-(B+3G)/4	*/			
#define	A8_8	93	/*	(2R+G+B)/4	G-B	R-(G+3B)/4	*/			
#define	A8_9	94	/*	(2R+G+B)/4	B-R	G-(B+3R)/4	E13	56	*/			
#define	A8_10	95	/*	(2R+G+B)/4	R-G	B-(R+G)/2		E8	39	*/	
#define	A8_11	96	/*	(2R+G+B)/4	R-B	G-(R+B)/2		E14	57	*/			
#define	A8_12	97	/*	(2R+G+B)/4	B-G	R-(B+G)/2		C4	14	*/	
										
#define	A9_1	98	/*	(R+G+2B)/4	R-G	B-G	*/			
#define	A9_2	99	/*	(R+G+2B)/4	G-R	B-R	*/			
#define	A9_3	100	/*	(R+G+2B)/4	R-B	G-B					A5	7	*/	
#define	A9_4	101	/*	(R+G+2B)/4	R-G	B-(R+3G)/4	*/			
#define	A9_5	102	/*	(R+G+2B)/4	G-R	B-(G+3R)/4	*/			
#define	A9_6	103	/*	(R+G+2B)/4	R-B	G-(R+3B)/4	E10	41	*/			
#define	A9_7	104	/*	(R+G+2B)/4	B-G	R-(B+3G)/4	E18	61	*/			
#define	A9_8	105	/*	(R+G+2B)/4	G-B	R-(G+3B)/4	E16	59	*/			
#define	A9_9	106	/*	(R+G+2B)/4	B-R	G-(B+3R)/4	E12	43	*/			
#define	A9_10	107	/*	(R+G+2B)/4	R-G	B-(R+G)/2		C5	15	*/		
#define	A9_11	108	/*	(R+G+2B)/4	R-B	G-(R+B)/2		E11	42	*/			
#define	A9_12	109	/*	(R+G+2B)/4	B-G	R-(B+G)/2		E17	60	*/

#define	B1_1	110	/*	G R-G B	*/
#define	B1_2	111	/*	G B-G R	*/
#define	B2_1	112	/*	R G-R B	*/
#define	B2_3	113	/*	R B-R G	*/
#define	B3_2	114	/*	B G-B R	*/
#define	B3_3	115	/*	B R-B G	*/

#define	B4_1	116	/*	(G+R)/2  R-G  B	*/
#define	B5_2	117	/*	(B+G)/2  G-B  R	*/
#define	B6_3	118	/*	(B+R)/2  R-B  G	*/

#define	PEI09	119	/*	Pei, ICIP 2009 */
#define	A7_1s	120	/*	smoothed A7_1 */
#define	A7_1m	121	/*	median A7_1 */

//#define INDEXED_COL	120			/* colour map	used for header information in codec_e.c*/
#define INDEXED_GBR	122			/* colour map	used for header information in codec_e.c*/
#define INDEXED_LUM	123			/* colour map	used for header information in codec_e.c*/


unsigned char col2idx[108] = {	
	0,  0, /* dummy for Grey and RGB */
  0, 1, 0, 18, 19, 0, 0, 0, 0,  9,  0,  0, /* 2 - 13	*/
	1, 0, 0, 19, 18, 0, 0, 0, 0, 10,  0,  0, /* 14 - 25	*/
	2, 2, 0, 20, 20, 0, 0, 0, 0, 11,  0,  0, /* 26 - 37	*/
	3, 3, 0, 21, 21, 0, 0, 0, 0, 12,  0,  0, /* 38 - 49	*/
	4, 5, 0, 22, 23, 0, 0, 0, 0, 13,  0,  0, /* 50 - 61	*/
	5, 4, 0, 23, 22, 0, 0, 0, 0, 14,  0,  0, /* 62 - 73	*/
	6, 7, 0, 24, 25, 0, 0, 0, 0, 15,  0,  0, /* 74 - 85	*/
	7, 6, 0, 25, 90, 0, 0, 0, 0, 16,  0,  0, /* 86 - 97	*/
	8, 8, 0, 26,102, 0, 0, 0, 0, 17,  0,  0	/* 98 - 109	*/
}
/*|  |_Ax_2|   |_Ax_5           |_Ax_10
	|        |_Ay_4
  |_Ay_1
 */
/* shift values for 9 x 3 colour spaces
 * other variants stem from permutations of R, G, and B
 */
unsigned char shiftTab[27][3] = {	
	{ 9, 9, 9},  /* G						R-G		B-G				A1_1	*/
	{ 0, 9, 9},  /* R						R-G		B-G				A2_1	*/
	{ 9, 0, 9},  /* B						R-G		B-G				A3_1	*/
	{ 1, 9, 9},  /* (G+R)/2			R-G		B-G				A4_1	*/
	{ 9, 1, 9},  /* (G+B)/2			R-G		B-G				A5_1	*/
	{ 1, 1, 9},  /* (B+R)/2			R-G		B-G				A6_1	*/
	{ 2, 2, 9},  /* (R+2G+B)/4	R-G		B-G				A7_1	*/
	{ 1, 2, 9},  /* (2R+G+B)/4	R-G		B-G				A8_1	*/
	{ 2, 1, 9},  /* (R+G+2B)/4	R-G		B-G				A9_1	*/

	{ 9, 9, 1},  /* G						R-G		B-(R+G)/2	A1_10	*/
	{ 0, 9, 1},  /* R						R-G		B-(R+G)/2	A2_10	*/
	{ 9, 0, 1},  /* B						R-G		B-(R+G)/2	A3_10	*/
	{ 1, 9, 1},  /* (G+R)/2			R-G		B-(R+G)/2	A4_10	*/
	{ 9, 1, 1},  /* (G+B)/2			R-G		B-(R+G)/2	A5_10	*/
	{ 1, 1, 1},  /* (B+R)/2			R-G		B-(R+G)/2	A6_10	*/
	{ 2, 2, 1},  /* (R+2G+B)/4	R-G		B-(R+G)/2	A7_10	*/
	{ 1, 2, 1},  /* (2R+G+B)/4	R-G		B-(R+G)/2	A8_10	*/
	{ 2, 1, 1},  /* (R+G+2B)/4	R-G		B-(R+G)/2	A9_10	*/

	{ 9, 9, 2},  /* G						R-G		B-(R+3G)/4	A1_4	*/
	{ 0, 9, 2},  /* R						R-G		B-(R+3G)/4	A2_4	*/
	{ 9, 0, 2},  /* B						R-G		B-(R+3G)/4	A3_4	*/
	{ 1, 9, 2},  /* (G+R)/2			R-G		B-(R+3G)/4	A4_4	*/
	{ 9, 1, 2},  /* (G+B)/2			R-G		B-(R+3G)/4	A5_4	*/
	{ 1, 1, 2},  /* (B+R)/2			R-G		B-(R+3G)/4	A6_4	*/
	{ 2, 2, 2},  /* (R+2G+B)/4	R-G		B-(R+3G)/4	A7_4	*/
	{ 1, 2, 2},  /* (2R+G+B)/4	R-G		B-(R+3G)/4	A8_4	*/
	{ 2, 1, 2},  /* (R+G+2B)/4	R-G		B-(R+3G)/4	A9_4	*/
};

/* proto typing	*/
void RGBtoAxy( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr,
							  unsigned int col);

void RGBtoAxy_off( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr,
							  unsigned int col);



void RGBtoB1_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void RGBtoB1_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void RGBtoB2_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void RGBtoB2_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void RGBtoB3_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void RGBtoB3_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void RGBtoB4_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void RGBtoB5_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void RGBtoB6_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);

void RGBtoPEI09( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void RGBtoA7_1s( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void RGBtoA7_1m( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);



void A1_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A1_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A1_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A1_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A1_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A1_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A1_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A1_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A1_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A1_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A1_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A1_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);

void A2_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A2_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A2_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A2_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A2_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A2_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A2_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A2_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A2_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A2_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A2_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A2_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);

void A3_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A3_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A3_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A3_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A3_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A3_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A3_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A3_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A3_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A3_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A3_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A3_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);

void A4_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A4_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A4_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A4_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A4_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A4_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A4_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A4_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A4_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A4_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A4_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A4_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);

void A5_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A5_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A5_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A5_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A5_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A5_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A5_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A5_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A5_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A5_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A5_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A5_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);

void A6_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A6_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A6_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A6_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A6_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A6_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A6_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A6_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A6_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A6_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A6_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A6_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);

void A7_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);

void A8_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A8_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A8_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A8_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A8_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A8_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A8_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A8_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A8_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A8_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A8_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A8_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);

void A9_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A9_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A9_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A9_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A9_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A9_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A9_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A9_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A9_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A9_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A9_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A9_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);

void B1_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void B1_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void B2_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void B2_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void B3_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void B3_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void B4_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void B5_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void B6_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);

void PEI09toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_1stoRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);
void A7_1mtoRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr);

unsigned long estimate_noise( IMAGEc *im, int *ptr);

unsigned char find_colourspace4( IMAGEc *im, unsigned int *r_ptr, 
										 unsigned int *g_ptr, unsigned int *b_ptr,
										 unsigned long set_size,
										 unsigned int *Yi, unsigned int *UVj,
										 double entropy[],
										 unsigned int y_start,
										 unsigned int y_end,
										 unsigned int x_start,
										 unsigned int x_end,
										 unsigned char pred_flag
										 );
#endif
