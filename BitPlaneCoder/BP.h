/*********************************************************************
* File	 	: BP.h
* Purpose	: to provide function prototypes for bit plane coding;
*		 	  to introduce new compiler directives
* Author 	: Mathias Roeppischer
* Date	 	: 06.12.2013
* Last Modif: 01.02.2014
* 31.03.2014 Strutz: mofified declaration of functions
* 19.05.2014 Strutz: s_flg
*********************************************************************/
#ifndef _BP_H
#define _BP_H

#include "bitbuf.h"
#include "ginterface.h"
#include "qm_coder.h"
#include "tstypes.h"

/*--------------------------------------------------------------------
* G_decode()
*
* maps GRAY code to plain binary representation of integer
*
* *g: GRAY coded binary number
*-------------------------------------------------------------------*/
void G_decode( uint *g);

/*--------------------------------------------------------------------
* G_encode()
*
* maps integer to GRAY coded binary representation
*
* *b: integer based on plain binary representation
*-------------------------------------------------------------------*/
void G_encode( uint *b);


/*--------------------------------------------------------------------
* BP_coding()
*
* decomposes image into bit planes to provide for binary arithmetic
* encoding
*
* bitbuf: compressed byte buffer
* g_flg : user requested GRAY encoding (1) or not (0)
* p_flg : prediction took place (1) or not (0)
* pd	: pointer to picture data structure
*-------------------------------------------------------------------*/
void
BP_coding( BitBuffer *bitbuf, cbool g_flg, cbool p_flg, cbool c_flg, 
					PicData *pd, int encoder_flag, unsigned char s_flg);


/*--------------------------------------------------------------------
* map()
*
* maps pixel value to positive integer if prediction has been applied
*
* *p: pixel value
*-------------------------------------------------------------------*/
int map( int p);

/*--------------------------------------------------------------------
* undo_map()
*
* remaps pixel value to signed integer as before bit plane coding
*
* *p: pixel value
*-------------------------------------------------------------------*/
int undo_map( int p);

#endif