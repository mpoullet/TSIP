/*******************************************************************
 *
 * File....: tmatching.c
 * Function: Calculation of estimation value using a template
 *           matching algorithm
 * Author..: Veronika Geneva
 *           Thomas Schatton (compatibility adjustments)
 * Date....: 11/04/2010, 13.01.2011 (Strutz), 13.01.11 (Schatton)
 *
 ********************************************************************/

#include "tmatching.h"

/* variables */
int context_size = 8,   /* Win */
    offset       = 24;  /* Search Offset */

static int w_flag = 0, /* weighting */
           e_flag = 0, /* fast searching (skips pixels) */
           h_flag = 0; /* context shaping */

window     win, *win_p;	  /* Dimensions of the search area */

/* Lists containing the three different contexts and pointers 
 * to them */
context_list *context_base_n = NULL, *context_l_n = NULL, 
             *context_base_v = NULL, *context_l_v = NULL,
             *context_base_h = NULL, *context_l_h = NULL;

/* Dimensions of normal, horizontally and vertically stretched 
 * contexts */
context_dim con_n_dim, con_v_dim, con_h_dim;
/* Pointers to normal, horizontally and vertically stretched 
 * contexts */
context_dim *con_n_dim_p, *con_v_dim_p, *con_h_dim_p;

int   side = 0, up = 0; /* Size of the context as an offset to the 
				  	             * left and right and above the current 
          	  				   * pixel */

float   sigma = 0;		/* Strutz: sigma >0 activates the blending */
//ULONG hist_D[256];


/*------------------------------------------------------------------
 * tmatchingInitialize(AUSWAHL *a, float s)
 *
 * Allocation and initialization of contexts (s: sigma).
 * Source: Geneva\tmatching.c, lines 130 to 244: encode_template()
 *-----------------------------------------------------------------*/
int tmatchingInitialize(AUSWAHL *a, float s)
{
  /* initialize sigma (truncate to a [0..20] domain) */
  sigma = (s > 20.0f ? 20.0f : (s < 0.0f ? 0.0f : s));

	/* Initializing the pointers */
  con_n_dim_p = &con_n_dim;
  con_v_dim_p = &con_v_dim;
  con_h_dim_p = &con_h_dim;
  win_p = &win;
	
	/* Allocate memory for context list bases */
  TRY( ECALLOC( context_base_n, 256, context_list, 120)); /* Normal */
  TRY( ECALLOC( context_base_v, 256, context_list, 120)); /* Vertical */
  TRY( ECALLOC( context_base_h, 256, context_list, 120)); /* Horizontal */
  /* Allocate memory for context lists */
  TRY( ECALLOC( context_l_n, 256, context_list, 120));
  TRY( ECALLOC( context_l_v, 256, context_list, 120));
  TRY( ECALLOC( context_l_h, 256, context_list, 120));
	
	/* Define base context base arrays */
	define_context( a->src, context_base_n, /*win_p,*/ 'n');
	define_context( a->src, context_base_v, /*win_p,*/ 'v');
	define_context( a->src, context_base_h, /*win_p,*/ 'h');
	
	/* Take the first "context_size" elements */ 
	take_context( context_base_n, context_l_n, con_n_dim_p);
	take_context( context_base_v, context_l_v, con_v_dim_p);
	take_context( context_base_h, context_l_h, con_h_dim_p);
	
	/* If option for context shaping used take the maximun offset
	 * side offset from horizontal context
	 * and up offset from vertical offset 
	 */
	if (h_flag)
	{
		side = con_h_dim.side;
		up = con_v_dim.up;
	}
	else
	{
		side = con_n_dim.side;
		up = con_n_dim.up;
	}	

  return 0;
}

/*------------------------------------------------------------------
 * tmatchingXHat( AUSWAHL *a)
 *
 * Calculates the prediction value using template matching.
 * Source: Geneva\tmatching.c
 *-----------------------------------------------------------------*/
int tmatchingXHat(AUSWAHL *a)
{
  context_list *temp_context = NULL;
  int          i = a->r * a->src->width + a->c;

  /* first row */
  if (a->r == 0)
    return getPixel( a, 'a');

  /* If current position is closer to the image border then the
	 * context size to that side use upper pixel for prediction */
  if (((int)a->c < side) || ((int)(a->src->width - a->c) <= side))
    return getPixel( a, 'b');
  
  /* Check if window must be reshaped because of being 
	 * close to image border and if so reshape it */
  define_window( i, a->src, con_n_dim_p, win_p);
	
	/* Assign the address of the list to temporary variable */ 
	temp_context = context_l_n;

  /* Predict pixel */
	return predict_template( temp_context, a, i, win_p, con_n_dim_p );
}

/*---------------------------------------------------------------
 *	predict_template()
 * 
 * 	Scan the search area and find match for the context. 
 *  Modification: Edited to work with AUSWAHL struct.
 *---------------------------------------------------------------*/
int predict_template( context_list *context_l, AUSWAHL *a, 
					  int position, window *win_p, context_dim *con_dim )
{
	context_list *temp_context;
	int i = 0, k = 0;			 /* Counters */
	int jump = 0, distance = 0;	 /* Distance in pixels from the end of 
								                * a row to the begining of the 
								                * previuos row */
	float pixel = 0; /* reset Strutz: ! should be float!!*/
	int predict = 0;	 /* Pixel value */
	float coef_sum = 0;			 /* Sum of weighting coeficients */
		
	float MADs = 0; /* Strutz: */
	float error = 0, MD = 65000 /* assume maximal posssible distanz */;
	float MD_temp = 0;
	float weight = 0, sum_weight = 0;	/* Weight for prediction */		

	temp_context = context_l;
	
	i = 1;
	
	/* Pass trough the row to the left of the current pixel if there 
	 * is any*/
	if (win_p->l_offset != 0)
	{
		do
		{
			error = 0;
			while (temp_context)
			{
				/* Error sums the absolute difference between pixel 
				 * in the window corresponding to 
				 * pixel in the context*/
				if (w_flag)
					 error += temp_context->powered * 
            abs( (int)(getPixelByIndex( a, position - temp_context->data_offset - i)
               - getPixelByIndex( a, position - temp_context->data_offset)));                                                                
				else 
					 error += 
            abs( (int)(getPixelByIndex( a, position - temp_context->data_offset - i)
               - getPixelByIndex( a, position - temp_context->data_offset)));    
        
				/* Sum the coefficients used for weighting*/
				coef_sum += temp_context->powered;
				
				/* Move to the next pixel in the context */ 
				temp_context = temp_context->pointer;
			}

			/* If weighting is used normalize the SAD */
			if (w_flag && h_flag)
			{
				error = error / coef_sum;
				coef_sum = 0;
			}

			if ( sigma == 0)
			{
				if ( MD > error)
				{
					MD = error;

					/* Use only the best match to predict the pixel
					 * if sigma is set to 0 */
					pixel = (float)getPixelByIndex( a, position - i);
				}
			}
			else
			{
				/* Strutz: use MAD instaed of SAD */
				MADs = error / context_size / sigma;
				weight = (float)exp( -0.5 * MADs * MADs); 
				pixel += weight * getPixelByIndex( a, position - i);
				sum_weight += weight;
			}
			i++;
			
			/* Return the temp pointer to the first element of the 
			 * list */
			temp_context = context_l;
			
		}/* do
		/* Until search area border reached */
		while (i <= win_p->l_offset);

	} /*if (win_p->l_offset != 0)*/
	
	k = 0;
	
	/* Jump to the first element on the right in the row above the 
	 * current pixel */
  jump = 2 * con_dim->side + i + a->src->width - (win_p->l_offset + 
			   win_p->r_offset + 2 * con_dim->side + 1);
	 
	do
	{
		/* Keep track for the position. Here no changing of the 
		 * context shape and size 
		 * when approaching the end or beginning of a row is used*/
		
		i = 0;
		do
		{
			distance = i + jump;
			 /* Fensterinhalte vergleichen*/
			/* Strutz: can MD_temp be a float vaule??  Yes if positions are weighted */
			MD_temp = compare_context( a, temp_context, position, distance);

		  if ( sigma == 0)
			{
				if ( MD > MD_temp)
				{
					MD = MD_temp;

					/* Use only the best match to predict the pixel
					 * if sigma is set to 0 */
					pixel = (float)getPixelByIndex( a, position - distance);
				}
			}
			else
			{
				/* weighted averaging of values */
				/* Strutz: use MAD instead of SAD */
				MADs = MD_temp / context_size / sigma;
				weight = (float)exp( -0.5 * MADs * MADs); 
				pixel += weight * getPixelByIndex(a, position - distance);
				sum_weight += weight;
			}

			/* If flag for fast searching is used, jump over pixels,
			 * otherwise check every pixel
			 */
      i += e_flag ? 2 : 1;

			temp_context = context_l;
		}
		/* While the end of a row is reached */ 
		while (i < (win_p->l_offset + win_p->r_offset + 1) );
		
		/* If flag for fast searching is used jump over rows,
		 * otherwise check every row */
    k    += e_flag ? 2 : 1;
    jump += (e_flag ? 2 : 1) * a->src->width;
	}
  while (k < (win_p->h_offset) );/* do
	/* While top of the window is reached */

	/* Calculate the predicted pixel value */
  predict = (int)((sigma == 0) ? pixel : pixel / sum_weight);
	
  return predict;
}

/********************/
/* Geneva\context.c */
/********************/

/*---------------------------------------------------------------
 *	define_window()
 * 
 *	Define the search area dimensions based on the distace from
 *	the border of the image
 * 
 *  Modification: Compatibility to image structure.
 *---------------------------------------------------------------*/
void define_window( int position, BILD *data, context_dim *con_dim, 
		                window *win_p)
{
	/* If image border closer to pixel context than window offset 
	 * then change window size */
	/* Window height */
	if ( ( (int)(position / data->width) - con_dim->up) < offset )
		win_p->h_offset = (int)(position / data->width) - con_dim->up;
	else win_p->h_offset = offset;
	
	/* Window left offset */
	if ( ( (position % (int)data->width) - con_dim->side) < offset )
		win_p->l_offset = (position % data->width) - con_dim->side;
	else win_p->l_offset = offset;
	
	/* Window right offset */
	if ( ((int)data->width - (position % (int)data->width) - 1 
       - con_dim->side) < offset)
		win_p->r_offset = data->width - (position % data->width) - 1 - 
			con_dim->side;
	else win_p->r_offset = offset;	
}

/*---------------------------------------------------------------
 *	define_context()
 * 
 *	Define linked list of offsets in pixel count from the current 
 * 	pixel.
 * 	Sort the list in order of the linear distance from the 
 * 	current pixel.
 *
 *  Modification: Compatibility to image structure
 *---------------------------------------------------------------*/
void define_context( BILD *data, context_list *context_l, 
    //                 window *win_p, 
	char mode)
{
	int i = 0, j = 0, k;    /* Counters */
	int max_size = 220; /* Maximal size of context */

	/* Coeficients of horizontal and vertical stretching of the 
	 * context */
	float a = 1 , b = 1;
		
	/* Ratio between 'a' and 'b' define the shape of context */
  if (mode == 'v') a = 2;
  if (mode == 'h') b = 2;
	
	/* Link the list's elements */
	do
	{
	  context_l[i].pointer = &context_l[i+1];
		i++;
	}
	while (i < max_size);
	
	/* Assign 0 to the last pointer */	
	context_l[i-1].pointer = 0;
	
	/* Calculate linear distance and offset of every pixel to the 
	 * current pixel*/
	i = 1;
		
	do
	{
		/* Pick pixel from the left */
		context_l[j].data_offset = i;
		context_l[j].x = i;
		context_l[j].y = 0;
		context_l[j].distance = i * a;
		/* Calculate the weight of the pixel */
		context_l[j].powered = (float)pow( 0.8f, context_l[j].distance);

		j++;

		/* Pick pixel from above */
		context_l[j].data_offset = i * (data->width);
		context_l[j].x = 0;
		context_l[j].y = i;
		context_l[j].distance = i * b;
		/* Calculate the weight of the pixel */
		context_l[j].powered = (float)pow( 0.8f, context_l[j].distance);
		
    j++;
		k = 1;
		
		do
		{
			/* Pick pixel from the left and right in the row i 
			 * times above */
			context_l[j].data_offset = i * (data->width) - k;
			context_l[j].x = k;
			context_l[j].y = i;
			context_l[j].distance = (float)sqrt(k*k * a*a + i*i * b*b);
			/* Calculate the weight of the pixel */
			context_l[j].powered = (float)pow( 0.8f, context_l[j].distance);
			
      j++;
			
			context_l[j].data_offset = i * (data->width) + k;
			context_l[j].x = k;
			context_l[j].y = i;
			context_l[j].distance = (float)sqrt(k*k * a*a +	i*i * b*b);
			/* Calculate the weight of the pixel */
			context_l[j].powered = (float)pow( 0.8f, context_l[j].distance);
			
      j++;
			k++;
		}	
    while	(k <= 10);
		
		i++;
	} /* context_l[j].data_offset = i;*/
	while	(i <= 10);
	
	/* Sort pointers in context in order of the distance from 
	 * current pixel*/
	sort_list( context_l);	
}

/*---------------------------------------------------------------
 *	sort_list()
 * 
 *	Sort linked list in order of the distnce field.
 *---------------------------------------------------------------*/
void sort_list( context_list *list)
{
	context_list *thisp, *nextp, *aux;
	int          exchange;
	
	aux = list;
	
	do
	{
		/* Flag for exchage made */
		exchange = 0;
		
		/* Keep pointer for the list */
		thisp = aux;
		while ( (nextp = thisp->pointer) && nextp->pointer)
		{
			if (nextp->distance > nextp->pointer->distance)
			{
				/* Exchange */
				exchange = 1;
				thisp->pointer = nextp->pointer;
				nextp->pointer = thisp->pointer->pointer;
				thisp->pointer->pointer = nextp;
			}
			thisp = thisp->pointer;
		}
	}
	while (exchange);
	
	/* Return poiter for list */
	
	list = aux;
}

/*---------------------------------------------------------------
 *	take_context()
 * 
 * 	Copy the first "context_size" elements from the base array
 *---------------------------------------------------------------*/
void take_context( context_list *context_base, 
                   context_list *context_l, 
		               context_dim *con_dim_p)
{
	int i = 0;							/* Counter */
	int x = 0, y = 0;	/* Coordinates of pixels in the context 
                     * according to the current */	
	
  context_list *temp_p_1, *temp_p_2;	/* Temporary lists */

  /* Link the list */	
	do
	{
		context_l[i].pointer = &context_l[i+1];
		i++;
	}
	while (i < context_size);
	context_l[i - 1].pointer = 0;
		
	/* Assign temporary pointers */	
	temp_p_1 = context_l;
	temp_p_2 = context_base;
	
	/* Copy the first values */
	/* Offset from the current */
	temp_p_1->data_offset = temp_p_2->data_offset;		
	/* Coeficient for weighting */
	temp_p_1->powered = temp_p_2->powered;		
	/* Distance */
	temp_p_1->distance = temp_p_2->distance;	
	/* Coordinates */
	x = temp_p_2->x;					
	y = temp_p_2->y;

	do
	{
		/* Move the temporary pointers over the elements of 
		 * the lists */
		temp_p_1 = temp_p_1->pointer;
		temp_p_2 = temp_p_2->pointer;
		
		/* Take the maximum offset to the side and the top - x and y
		 * This will be used to determine the size - horizontal and
		 *  vertical of the context
		 */
		if (x < temp_p_2->x) x = temp_p_2->x;
		if (y < temp_p_2->y) y = temp_p_2->y;
		
		/* Copy values from the base array to the one with
		 * defined value of "context_size"
		 */
		/* Offset from the current */
		temp_p_1->data_offset = temp_p_2->data_offset;
		/* Euclidean distance */
		temp_p_1->distance = temp_p_2->distance;		
		/* Coeficient for weighting */
		temp_p_1->powered = temp_p_2->powered;	
	}
	while (temp_p_1->pointer); /* The last element of hte list has a 
	poiter value of "0" */
	
	/* Take the context dimensions */
	con_dim_p->side = x;	 
	con_dim_p->up = y;	
}


/*---------------------------------------------------------------
 *	compare_context()
 * 
 * 	Compare the context of the current pixel with the context 
 * 	of a pointed pixel
 *
 *  Modification: Compatibility to AUSWAHl structure.
 *---------------------------------------------------------------*/
float compare_context( AUSWAHL *a, context_list *temp_context, 
                       int position, int distance)
{
	float error = 0;
	float coef_sum = 0;
	
	while (temp_context)
	{
		/* If flag for weighting is used use coeficients for weghting
		 * othewise take just the SAD 
		 */
		if (w_flag)
		{
			error += temp_context->powered * 
               abs( (int)(getPixelByIndex( a, position - temp_context->data_offset - distance)
                  - getPixelByIndex( a, position - temp_context->data_offset)));
		}
		else 
		{
			error += abs( (int)(getPixelByIndex( a, position - temp_context->data_offset - distance)
                  - getPixelByIndex( a, position - temp_context->data_offset)));

//Strutz, Test SSE statt SAD schlechter für barbara.y, Warum????
//			float diff = data->pData[position - 
//			    temp_context->data_offset - distance ] -
//			    data->pData[position - temp_context->data_offset];
//			error += diff * diff;
//Strutz
//Strutz
// Multiplication => Autokorrelation taugt nicht als Prädiktor
//			error += data->pData[position - 
//			    temp_context->data_offset - distance ] *
//			    data->pData[position - temp_context->data_offset] ;
		}
		/* Sum the coeficients used for weighting*/
		coef_sum += temp_context->powered;
		
		/* Move to the next pixel of context */
		temp_context = temp_context->pointer;
	}
	
	/* If weighting is used normalize the SAD */
  return (w_flag && h_flag) ? (error / coef_sum) : error;
}