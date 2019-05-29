/*******************************************************************
 *
 * File....: tmatching.h
 * Function: Definition of predictor variables and prototyping of
 *           functions for the template matching predictor.
 * Author..: Veronika Geneva
 *           Thomas Schatton (compatibility adjustments)
 * Date....: 01/13/2011
 *
 ********************************************************************/

#ifndef _TMATCHING_H_
#define _TMATCHING_H_

#include <math.h>
#include <conio.h>
#include "..\Typen\types.h"
#include "..\Module\auswahl.h"

/* structures */

/* CONTEXT_LIST */
typedef struct context_list
{	
		float distance;					/* linear distance from 
										         * current pixel*/
		int data_offset;				/* offset in pixels */
		int x;							/* horizontal distnace from 
										     * current pixel*/
		int y;							/* vertical distnace from 
		 								     * current pixel*/
		float powered;
		struct context_list *pointer;	/* pointer to next element */
} context_list;

/* WINDOW */
typedef struct 
{
		int l_offset;	/* number of pixels to the left of the context*/
		int r_offset;	/* number of pixels to the right of the context*/
		int h_offset;	/* number of pixels above the context */
} window;

/* CONTEXT_DIM */
typedef struct 
{
	int side;		/* Dimensions of the context - number of pixels */ 
	int up;			/* aside and above the current	*/
} context_dim;



/* initialization of predictor variables */
int tmatchingInitialize(AUSWAHL *a, float s); 
/* free predictor variables */
void tmatchingFree();
/* calculate prediction value */
int tmatchingXHat(AUSWAHL *a);

/* other functions */
int predict_template( 
  context_list *context_l, AUSWAHL *a, int position, window *win_p,
  context_dim *con_dim);
void define_window( 
  int position, BILD *data, context_dim *con_dim, window *win_p);
void define_context( 
  BILD *data, context_list *context_l, /*window *win_p,*/ char mode);
void sort_list( context_list *list);
void take_context( 
  context_list *context_base, context_list *context_l, 
  context_dim *con_dim_p);
float compare_context( 
  AUSWAHL *a, context_list *temp_context, int position,
	int distance);



#endif