/*****************************************************************
 *
 * File...:	colour4_i.c
 * Purpose:	implements inverse colour transforms and related
 * Author.:	Tilo Strutz
 * Date...: 16.09.2011
 *  changes:
 * 5.10.2011 Strutz: im->bit_depth_max-1 for addval
 * 27.03.2012 Strutz: new 108 colour spaces
 * 26.09.2013 new space: PEI09
 * 18.12.2013 bugfix: the bit depth must be increased also 
 *                 in Bx_y spaces due to side effects 
 * 28.02.2014 bit depth must not be increased anu more 
 * 08.01.2015 Strutz: #ifdef MODULO_RCT for def-based compilation
 * 20.07.2015 smooth_RCT, A7_1s
 * 21.07.2015 median_RCT, A7_1m
 *****************************************************************/
#include <stdlib.h>
#include "colour4.h"
#include "tstypes.h" /* for ALLOC() and FREE()*/

#ifndef MODULO_RCT
int med3x3( int *b1, int *b2, int *b3);
int med3x3u( unsigned int *b1, unsigned int *b2, unsigned int *b3);

/*---------------------------------------------------------------
 *	A1_1toRGB() a1=a2=0
 *---------------------------------------------------------------*/
void A1_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A1_1) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		g_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_2toRGB()   a1=a2=0 R<=>G
 *---------------------------------------------------------------*/
void A2_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A2_2) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		r_ptr[i] = Yr;
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_3toRGB()   a1=a2=0 B<=>G
 *---------------------------------------------------------------*/
void A3_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A3_3) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = -(signed)b_ptr[i] + addval;
		b_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_4toRGB() a1=a2=0 e=1/4
 *---------------------------------------------------------------*/
void A1_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A1_4) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		g_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_5toRGB()   a1=a2=0 e=1/4 R<=>G
 *---------------------------------------------------------------*/
void A2_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A2_5) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-R -(G-R)/4 = B -(G+3R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_6toRGB()   a1=a2=0 e=1/4 B<=>G
 *---------------------------------------------------------------*/
void A3_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A3_6) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* G-B -(R-B)/4 = G -(R+3B)/4	*/
		b_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_7toRGB() a1=a2=0 e=1/4 B<=>R
 *---------------------------------------------------------------*/
void A1_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A1_7) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* R-G -(B-G)/4 = R -(R+3G)/4	*/
		g_ptr[i] = Yr;
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_8toRGB() a1=a2=0 e=1/4 R=>G=>B=>R
 *---------------------------------------------------------------*/
void A3_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A3_8) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = -(signed)b_ptr[i] + addval;
		Vr = Vr + (Ur >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		b_ptr[i] = Yr;
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_9toRGB() a1=a2=0 e=1/4  R=>B=>G=>R
 *---------------------------------------------------------------*/
void A2_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A2_9) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);			/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		r_ptr[i] = Yr;
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_10toRGB() a1=a2=0 e=1/2
 *---------------------------------------------------------------*/
void A1_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A1_10) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);		/* B-G -(R-G)/2 = B -(R+G)/2	*/
		g_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_11toRGB() a1=a2=0 e=1/2 B<=>G
 *---------------------------------------------------------------*/
void A3_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A3_11) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);			/* G-B -(R-B)/2 = G -(R+B)/2	*/
		b_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_12toRGB() a1=a2=0 e=1/2  R<=>B
 *---------------------------------------------------------------*/
void A1_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A1_12) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);	/* R-G -(B-G)/2 = R -(B+G)/2	*/
		g_ptr[i] = Yr;
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

//##################################

/*---------------------------------------------------------------
 *	A2_1toRGB() a1=1 a2=0
 *---------------------------------------------------------------*/
void A2_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A2_1) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		g_ptr[i] = Yr - Ur;
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_2toRGB() a1=1 a2=0, R<=>G
 *---------------------------------------------------------------*/
void A1_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A1_2) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		r_ptr[i] = Yr - Ur;
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_3toRGB() a1=1 a2=0, B<=>G
 *---------------------------------------------------------------*/
void A2_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A2_3) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = -(signed)b_ptr[i] + addval;
		b_ptr[i] = Yr - Ur;
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_4toRGB() a1=1 a2=0, e=1/4
 *---------------------------------------------------------------*/
void A2_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A2_4) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		g_ptr[i] = Yr - Ur;
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_5toRGB() a1=1 a2=0, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A1_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A1_5) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr - Ur;
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_6toRGB() a1=1 a2=0, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A2_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A2_6) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		b_ptr[i] = Yr - Ur;
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_7toRGB() a1=1 a2=0, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A3_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A3_7) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		g_ptr[i] = Yr - Ur;
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_8toRGB() a1=1 a2=0, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A1_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A1_8) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = -(signed)b_ptr[i] + addval;
		Vr = Vr + (Ur >> 2);					
		b_ptr[i] = Yr - Ur;
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_9toRGB() a1=1 a2=0, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A3_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A3_9) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		r_ptr[i] = Yr - Ur;
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_10toRGB() a1=1 a2=0, e=1/2
 *---------------------------------------------------------------*/
void A2_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A2_10) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - Ur;
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_11toRGB() a1=1 a2=0, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void A2_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A2_11) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		b_ptr[i] = Yr - Ur;
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_12toRGB() a1=1 a2=0, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void A3_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A3_12) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - Ur;
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}


//##################################

/*---------------------------------------------------------------
 *	A3_1toRGB() a1=0 a2=1
 *---------------------------------------------------------------*/
void A3_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A3_1) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		g_ptr[i] = Yr - Vr;
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_2toRGB() a1=0 a2=1, R<=>G
 *---------------------------------------------------------------*/
void A3_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A3_2) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		r_ptr[i] = Yr - Vr;
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_3toRGB() a1=0 a2=1, B<=>G
 *---------------------------------------------------------------*/
void A1_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A1_3) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = -(signed)b_ptr[i] + addval;
		b_ptr[i] = Yr - Vr;
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_4toRGB() a1=0 a2=1, e=1/4
 *---------------------------------------------------------------*/
void A3_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A3_4) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		g_ptr[i] = Yr - Vr;
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_5toRGB() a1=0 a2=1, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A3_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A3_5) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr - Vr;
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_6toRGB() a1=0 a2=1, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A1_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A1_6) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);			
		b_ptr[i] = Yr - Vr;
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_7toRGB() a1=0 a2=1, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A2_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A2_7) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		g_ptr[i] = Yr - Vr;
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_8toRGB() a1=0 a2=1, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A2_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A2_8) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = -(signed)b_ptr[i] + addval;
		Vr = Vr + (Ur >> 2);					
		b_ptr[i] = Yr - Vr;
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_9toRGB() a1=0 a2=1, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A1_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A1_9) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		r_ptr[i] = Yr - Vr;
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_10toRGB() a1=0 a2=1, e=1/2
 *---------------------------------------------------------------*/
void A3_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A3_10) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - Vr;
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_11toRGB() a1=0 a2=1, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void A1_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A1_11) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		b_ptr[i] = Yr - Vr;
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_12toRGB() a1=0 a2=1, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void A2_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A2_12) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - Vr;
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}
//######################


//##################################

/*---------------------------------------------------------------
 *	A4_1toRGB() a1=1/2 a2=0
 *---------------------------------------------------------------*/
void A4_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_1) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		g_ptr[i] = Yr - (Ur>>1);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_2toRGB() a1=1/2 a2=0, R<=>G
 *---------------------------------------------------------------*/
void A4_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_2) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		r_ptr[i] = Yr - (Ur>>1);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_3toRGB() a1=1/2 a2=0, B<=>G
 *---------------------------------------------------------------*/
void A6_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_3) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = -(signed)b_ptr[i] + addval;
		b_ptr[i] = Yr - (Ur>>1);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_4toRGB() a1=1/2 a2=0, e=1/4
 *---------------------------------------------------------------*/
void A4_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_4) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		g_ptr[i] = Yr - (Ur>>1);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_5toRGB() a1=1/2 a2=0, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A4_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_5) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr - (Ur>>1);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_6toRGB() a1=1/2 a2=0, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A6_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_6) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		b_ptr[i] = Yr - (Ur>>1);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_7toRGB() a1=1/2 a2=0, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A5_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_7) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		g_ptr[i] = Yr - (Ur>>1);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_8toRGB() a1=1/2 a2=0, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A5_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_8) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = -(signed)b_ptr[i] + addval;
		Vr = Vr + (Ur >> 2);					
		b_ptr[i] = Yr - (Ur>>1);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_9toRGB() a1=1/2 a2=0, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A6_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_9) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		r_ptr[i] = Yr - (Ur>>1);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	_A4_10toRGB() a1=1/2 a2=0, e=1/2
 *---------------------------------------------------------------*/
void _A4_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_10) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - (Ur>>1);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_10toRGB() implemented as C4 ##########
 *---------------------------------------------------------------*/
void A4_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Y, Cg, Co, t, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_10) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Y = r_ptr[i]; /* first element	*/
		Co = g_ptr[i] - addval;
		Cg = b_ptr[i] - addval;
		t = Y;
		b_ptr[i] = (unsigned int)(Cg + t);
		g_ptr[i] = (unsigned int)(t - (Co>>1));
		r_ptr[i] = (unsigned int)(Co + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	_A6_11toRGB() a1=1/2 a2=0, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void _A6_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_11) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		b_ptr[i] = Yr - (Ur>>1);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_11toRGB()  B<=>G implemented as C5  #########
 *---------------------------------------------------------------*/
void A6_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Y, Cg, Co, t, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_11) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Y = r_ptr[i]; /* first element	*/
		Co = g_ptr[i] - addval;
		Cg = b_ptr[i] - addval;
		t = Y;
		g_ptr[i] = (unsigned int)(Cg + t);
		b_ptr[i] = (unsigned int)(t - (Co>>1));
		r_ptr[i] = (unsigned int)(Co + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	_A5_12toRGB() a1=1/2 a2=0, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void _A5_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_12) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - (Ur>>1);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}


/*---------------------------------------------------------------
 *	A5_12toRGB()  implemented as C3 ##############
 *---------------------------------------------------------------*/
void A5_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Y, Cg, Co, t, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_12) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Y = r_ptr[i]; /* first element	*/
		Cg = g_ptr[i] - addval;
		Co = b_ptr[i] - addval;
		t = Y;
		r_ptr[i] = (unsigned int)(Cg + t);
		g_ptr[i] = (unsigned int)(t - (Co>>1));
		b_ptr[i] = (unsigned int)(Co + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}



//##################################

/*---------------------------------------------------------------
 *	A5_1toRGB() a2=1/2 a1=0
 *---------------------------------------------------------------*/
void A5_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_1) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		g_ptr[i] = Yr - (Vr>>1);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_2toRGB() a2=1/2 a1=0, R<=>G
 *---------------------------------------------------------------*/
void A6_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_2) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		r_ptr[i] = Yr - (Vr>>1);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_3toRGB() a2=1/2 a1=0, B<=>G
 *---------------------------------------------------------------*/
void A5_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_3) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = -(signed)b_ptr[i] + addval;
		b_ptr[i] = Yr - (Vr>>1);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_4toRGB() a2=1/2 a1=0, e=1/4
 *---------------------------------------------------------------*/
void A5_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_4) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		g_ptr[i] = Yr - (Vr>>1);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_5toRGB() a2=1/2 a1=0, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A6_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_5) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						
		r_ptr[i] = Yr - (Vr>>1);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_6toRGB() a2=1/2 a1=0, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A5_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_6) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		b_ptr[i] = Yr - (Vr>>1);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_7toRGB() a2=1/2 a1=0, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A4_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_7) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		g_ptr[i] = Yr - (Vr>>1);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_8toRGB() a2=1/2 a1=0, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A6_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_8) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = -(signed)b_ptr[i] + addval;
		Vr = Vr + (Ur >> 2);					
		b_ptr[i] = Yr - (Vr>>1);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_9toRGB() a2=1/2 a1=0, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A4_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_9) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		//Vr = Vr + ((-Ur) >> 2);					
		r_ptr[i] = Yr - (Vr>>1);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		//b_ptr[i] = (unsigned int)(r_ptr[i]-Ur);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_10toRGB() a2=1/2 a1=0, e=1/2
 *---------------------------------------------------------------*/
void A5_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_10) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - (Vr>>1);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_11toRGB() a2=1/2 a1=0, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void A5_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_11) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		b_ptr[i] = Yr - (Vr>>1);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_12toRGB() a2=1/2 a1=0, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void A4_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_12) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - (Vr>>1);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}



//##################################

/*---------------------------------------------------------------
 *	A6_1toRGB() a1=a2=1/2
 *---------------------------------------------------------------*/
void A6_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_1) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		g_ptr[i] = Yr - ((Ur+Vr)>>1);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_2toRGB() a1=a2=1/2, R<=>G
 *---------------------------------------------------------------*/
void A5_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_2) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		r_ptr[i] = Yr - ((Ur+Vr)>>1);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_3toRGB() a1=a2=1/2, B<=>G
 *---------------------------------------------------------------*/
void A4_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_3) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = -(signed)b_ptr[i] + addval;
		b_ptr[i] = Yr - ((Ur+Vr)>>1);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_4toRGB() a1=a2=1/2, e=1/4
 *---------------------------------------------------------------*/
void A6_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_4) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		g_ptr[i] = Yr - ((Ur+Vr)>>1);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_5toRGB() a1=a2=1/2, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A5_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_5) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						
		r_ptr[i] = Yr - ((Ur+Vr)>>1);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_6toRGB() aa1=2=1/2, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A4_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_6) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		b_ptr[i] = Yr - ((Ur+Vr)>>1);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_7toRGB() a1=a2=1/2, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A6_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_7) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		g_ptr[i] = Yr - ((Ur+Vr)>>1);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_8toRGB() a1=a2=1/2, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A4_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_8) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = -(signed)b_ptr[i] + addval;
		Vr = Vr + (Ur >> 2);					
		b_ptr[i] = Yr - ((Ur+Vr)>>1);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_9toRGB() a1=a2=1/2, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A5_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A5_9) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		r_ptr[i] = Yr - ((Ur+Vr)>>1);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_10toRGB() a1=a2=1/2, e=1/2
 *---------------------------------------------------------------*/
void A6_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_10) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - ((Ur+Vr)>>1);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_11toRGB() a1=a2=1/2, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void A4_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A4_11) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		b_ptr[i] = Yr - ((Ur+Vr)>>1);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_12toRGB() a1=a2=1/2, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void A6_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A6_12) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - ((Ur+Vr)>>1);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}



//##################################

/*---------------------------------------------------------------
 *	A7_1toRGB() a1=a2=1/4  == YUVr
 *---------------------------------------------------------------*/
void A7_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_1) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		g_ptr[i] = Yr - ((Ur+Vr)>>2);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_1mtoRGB() median
 *---------------------------------------------------------------*/
void A7_1mtoRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int *Y = NULL, *U = NULL, *V = NULL, d[8], tmpi;
	unsigned int i, x, y, addval, Yaddval;
	unsigned long py, pos;

	if (im->colour_channels < 3u || im->colour_space != A7_1m) return;
	
	ALLOC( U, im->size, int);
	ALLOC( V, im->size, int);
	ALLOC( Y, im->size, int);
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 
	Yaddval = 1u << (im->bit_depth_max - 2u); 

	d[0] = -1;
	d[1] = -(int)im->width;
	d[2] = -1-(int)im->width;
	d[3] = 1;
	d[4] = im->width;
	d[5] = 1+im->width;
	d[6] = -1+im->width;
	d[7] = +1-(int)im->width;

	/* copy forward	*/
	for (i = 0; i < im->size; i++)
	{
		Y[i] = r_ptr[i] - Yaddval;
		U[i] = g_ptr[i] - addval;
		V[i] = b_ptr[i] - addval;
	}


	/* top and bottom row	*/
	for (y = 0, py = 0; y < im->height; y+=im->height-1, py += im->size-im->width)
	{
		for (x = 0, pos = py ; x < im->width; x++, pos++)
		{
			g_ptr[pos] = Y[pos] - ((U[pos]+V[pos])>>2);		/* G + (R-G+B-G)/4 = (R+2G+B)/4	*/
			r_ptr[pos] = U[pos] + (int)g_ptr[pos];	/* R-G	*/
			b_ptr[pos] = V[pos] + (int)g_ptr[pos];  /* B-G	*/
		}
	}
	/* left and right col	*/
	for (y = 0, py = 0; y < im->height; y++, py += im->width)
	{
		for (x = 0, pos = py ; x < im->width; x+=im->width-1, pos+=im->width-1)
		{
			g_ptr[pos] = Y[pos] - ((U[pos]+V[pos])>>2);		/* G + (R-G+B-G)/4 = (R+2G+B)/4	*/
			r_ptr[pos] = U[pos] + (int)g_ptr[pos];	/* R-G	*/
			b_ptr[pos] = V[pos] + (int)g_ptr[pos];  /* B-G	*/
		}
	}

	/* main body	*/
	for (y = 1, py = im->width; y < im->height-1; y++, py += im->width)
	{
		for (x = 1, pos = py + 1; x < im->width-1; x++, pos++)
		{
			g_ptr[pos] = Y[pos] - ((
				med3x3( &(U[pos+d[2]]), &(U[pos+d[0]]), &(U[pos+d[6]]) ) +
				med3x3( &(V[pos+d[2]]), &(V[pos+d[0]]), &(V[pos+d[6]]) )
				) >> 2);		
		}
	}

	/* U, V	*/
	for (y = 1, py = im->width; y < im->height-1; y++, py += im->width)
	{
		for (x = 1, pos = py + 1; x < im->width-1; x++, pos++)
		{
			tmpi = (int)med3x3u( &(g_ptr[pos+d[2]]), &(g_ptr[pos+d[0]]), &(g_ptr[pos+d[6]]));	
			r_ptr[pos] = U[pos] + tmpi;
			b_ptr[pos] = V[pos] + tmpi; 
		}
	}

	FREE( Y);
	FREE( U);
	FREE( V);

	/* decrease dynamic range */
	im->bit_depth[0]--; /* Y also changes its range */
	im->bit_depth[1]--; 
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_1stoRGB() smoothed
 *---------------------------------------------------------------*/
void A7_1stoRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int *Y = NULL, *U = NULL, *V = NULL, d[8], tmpi;
	unsigned int i, x, y, addval, Yaddval;
	unsigned long py, pos;

	if (im->colour_channels < 3u || im->colour_space != A7_1s) return;
	
	ALLOC( U, im->size, int);
	ALLOC( V, im->size, int);
	ALLOC( Y, im->size, int);
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 
	Yaddval = 1u << (im->bit_depth_max - 2u); 

	d[0] = -1;
	d[1] = -(int)im->width;
	d[2] = -1-(int)im->width;
	d[3] = 1;
	d[4] = im->width;
	d[5] = 1+im->width;
	d[6] = -1+im->width;
	d[7] = +1-(int)im->width;

	/* copy forward	*/
	for (i = 0; i < im->size; i++)
	{
		Y[i] = r_ptr[i] - Yaddval;
		U[i] = g_ptr[i] - addval;
		V[i] = b_ptr[i] - addval;
	}


	/* top and bottom row	*/
	for (y = 0, py = 0; y < im->height; y+=im->height-1, py += im->size-im->width)
	{
		for (x = 0, pos = py ; x < im->width; x++, pos++)
		{
			g_ptr[pos] = Y[pos] - ((U[pos]+V[pos])>>2);		/* G + (R-G+B-G)/4 = (R+2G+B)/4	*/
			r_ptr[pos] = U[pos] + (int)g_ptr[pos];	/* R-G	*/
			b_ptr[pos] = V[pos] + (int)g_ptr[pos];  /* B-G	*/
		}
	}
	/* left and right col	*/
	for (y = 0, py = 0; y < im->height; y++, py += im->width)
	{
		for (x = 0, pos = py ; x < im->width; x+=im->width-1, pos+=im->width-1)
		{
			g_ptr[pos] = Y[pos] - ((U[pos]+V[pos])>>2);		/* G + (R-G+B-G)/4 = (R+2G+B)/4	*/
			r_ptr[pos] = U[pos] + (int)g_ptr[pos];	/* R-G	*/
			b_ptr[pos] = V[pos] + (int)g_ptr[pos];  /* B-G	*/
		}
	}

	/* main body	*/
	for (y = 1, py = im->width; y < im->height-1; y++, py += im->width)
	{
		for (x = 1, pos = py + 1; x < im->width-1; x++, pos++)
		{
			g_ptr[pos] = Y[pos] - ((
				(U[pos] << 2) +
				((U[pos+d[0]] + U[pos+d[1]]) << 1) + U[pos+d[2]] +
				((U[pos+d[3]] + U[pos+d[4]]) << 1) + U[pos+d[5]] +
				 U[pos+d[6]] + U[pos+d[7]] +
				(V[pos] << 2) +
				((V[pos+d[0]] + V[pos+d[1]]) << 1) + V[pos+d[2]] +
				((V[pos+d[3]] + V[pos+d[4]]) << 1) + V[pos+d[5]] +
				 V[pos+d[6]] + V[pos+d[7]]
				) >> 7);		
		}
	}

	/* U, V	*/
	for (y = 1, py = im->width; y < im->height-1; y++, py += im->width)
	{
		for (x = 1, pos = py + 1; x < im->width-1; x++, pos++)
		{
			tmpi = (int)(
				(g_ptr[pos] << 2) + 
				((g_ptr[pos+d[0]] + g_ptr[pos+d[1]]) << 1) + g_ptr[pos+d[2]] +
				((g_ptr[pos+d[3]] + g_ptr[pos+d[4]]) << 1) + g_ptr[pos+d[5]] +
				 g_ptr[pos+d[6]] + g_ptr[pos+d[7]]
								) >> 4;	
			r_ptr[pos] = U[pos] + tmpi;
			b_ptr[pos] = V[pos] + tmpi; 
		}
	}

	FREE( Y);
	FREE( U);
	FREE( V);

	/* decrease dynamic range */
	im->bit_depth[0]--; 
	im->bit_depth[1]--; 
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_2toRGB() a1=a2=1/4, R<=>G
 *---------------------------------------------------------------*/
void A8_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_2) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		r_ptr[i] = Yr - ((Ur+Vr)>>2);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_3toRGB() a1=a2=1/4, B<=>G
 *---------------------------------------------------------------*/
void A9_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_3) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = -(signed)b_ptr[i] + addval;
		b_ptr[i] = Yr - ((Ur+Vr)>>2);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_4toRGB() a1=a2=1/4, e=1/4
 *---------------------------------------------------------------*/
void A7_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_4) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		g_ptr[i] = Yr - ((Ur+Vr)>>2);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_5toRGB() a1=a2=1/4, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A8_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_5) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						
		r_ptr[i] = Yr - ((Ur+Vr)>>2);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_6toRGB() a1=a2=1/4, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A9_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_6) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		b_ptr[i] = Yr - ((Ur+Vr)>>2);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_7toRGB() a1=a2=1/4, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A7_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_7) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		g_ptr[i] = Yr - ((Ur+Vr)>>2);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_8toRGB() a1=a2=1/4, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A9_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_8) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = -(signed)b_ptr[i] + addval;
		Vr = Vr + (Ur >> 2);					
		b_ptr[i] = Yr - ((Ur+Vr)>>2);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_9toRGB() a1=a2=1/4, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A8_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_9) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		r_ptr[i] = Yr - ((Ur+Vr)>>2);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_10toRGB() a1=a2=1/4, e=1/2
 *---------------------------------------------------------------*/
void A7_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_10) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - ((Ur+Vr)>>2);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_11toRGB() a1=a2=1/4, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void A9_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_11) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		b_ptr[i] = Yr - ((Ur+Vr)>>2);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_12toRGB() a1=a2=1/4, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void A7_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_12) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - ((Ur+Vr)>>2);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}



//##################################

/*---------------------------------------------------------------
 *	A8_1toRGB() a1=1/2 a2=1/4
 *---------------------------------------------------------------*/
void A8_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_1) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		g_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_2toRGB() a1=1/2 a2=1/4, R<=>G
 *---------------------------------------------------------------*/
void A7_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_2) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		r_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_3toRGB() a1=1/2 a2=1/4, B<=>G
 *---------------------------------------------------------------*/
void A8_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_3) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = -(signed)b_ptr[i] + addval;
		b_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_4toRGB() a1=1/2 a2=1/4, e=1/4
 *---------------------------------------------------------------*/
void A8_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_4) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		g_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_5toRGB() a1=1/2 a2=1/4, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A7_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_5) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_6toRGB() a1=1/2 a2=1/4, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A8_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_6) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		b_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_7toRGB() a1=1/2 a2=1/4, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A9_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_7) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		g_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_8toRGB() a1=1/2 a2=1/4, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A7_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_8) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = -(signed)b_ptr[i] + addval;
		Vr = Vr + (Ur >> 2);					
		b_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_9toRGB() a1=1/2 a2=1/4, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A9_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_9) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		r_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_10toRGB() a1=1/2 a2=1/4, e=1/2
 *---------------------------------------------------------------*/
void A8_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
						 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_10) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_11toRGB() a1=1/2 a2=1/4, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void A8_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_11) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		b_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_12toRGB() a1=1/2 a2=1/4, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void A9_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_12) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}


//##################################

/*---------------------------------------------------------------
 *	A9_1toRGB() a1=1/4 a2=1/2
 *---------------------------------------------------------------*/
void A9_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_1) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		g_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_2toRGB() a1=1/4 a2=1/2, R<=>G
 *---------------------------------------------------------------*/
void A9_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_2) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		r_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_3toRGB() a1=1/4 a2=1/2, B<=>G
 *---------------------------------------------------------------*/
void A7_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_3) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = -(signed)b_ptr[i] + addval;
		b_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_4toRGB() a1=1/4 a2=1/2, e=1/4
 *---------------------------------------------------------------*/
void A9_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_4) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		g_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_5toRGB() a1=1/4 a2=1/2, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A9_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_5) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_6toRGB() a1=1/4 a2=1/2, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A7_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_6) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		b_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_7toRGB() a1=1/4 a2=1/2, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A8_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_7) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		g_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_8toRGB() a1=1/4 a2=1/2, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A8_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_8) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = -(signed)b_ptr[i] + addval;
		Vr = Vr + (Ur >> 2);					
		b_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_9toRGB() a1=1/4 a2=1/2, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A7_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_9) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = -(signed)g_ptr[i] + addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 2);					
		r_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	_A9_10toRGB() a1=1/4 a2=1/2, e=1/2
 *---------------------------------------------------------------*/
void _A9_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
						 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_10) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}


/*---------------------------------------------------------------
 *	A9_10toRGB() implemented as C1 ##########
 *---------------------------------------------------------------*/
void A9_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Y, Cg, Co, t, addval;

	if (im->colour_channels < 3u || im->colour_space != A9_10) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Y = r_ptr[i]; /* first element	*/
		Cg = g_ptr[i] - addval;
		Co = b_ptr[i] - addval;
		t = Y - (Cg >> 1);
		b_ptr[i] = (unsigned int)(Cg + t);
		g_ptr[i] = (unsigned int)(t - (Co>>1));
		r_ptr[i] = (unsigned int)(Co + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	_A7_11toRGB() a1=1/4 a2=1/2, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void _A7_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_11) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval;
		Vr = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		b_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_11toRGB() implemented as C2 ####################
 *---------------------------------------------------------------*/
void A7_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Y, Cg, Co, t, addval;

	if (im->colour_channels < 3u || im->colour_space != A7_11) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Y = r_ptr[i]; /* first element	*/
		Co = g_ptr[i] - addval;
		Cg = b_ptr[i] - addval;
		t = Y - (Cg >> 1);
		g_ptr[i] = (unsigned int)(Cg + t);
		b_ptr[i] = (unsigned int)(t - (Co>>1));
		r_ptr[i] = (unsigned int)(Co + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	_A8_12toRGB() a1=1/2 a2=1/4, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void _A8_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_12) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval;
		Ur = b_ptr[i] - addval;
		Vr = Vr + (Ur >> 1);					
		g_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_12toRGB() implemented as C3 ##########
 *---------------------------------------------------------------*/
void A8_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Y, Cg, Co, t, addval;

	if (im->colour_channels < 3u || im->colour_space != A8_12) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Y = r_ptr[i]; /* first element	*/
		Cg = g_ptr[i] - addval;
		Co = -(signed)b_ptr[i] + addval;
		t = Y - (Cg >> 1);
		r_ptr[i] = (unsigned int)(Cg + t);
		b_ptr[i] = (unsigned int)(t - (Co>>1));
		g_ptr[i] = (unsigned int)(Co + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; /* Y keeps its range */
	im->bit_depth[2]--; im->bit_depth_max--;
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B1_1toRGB() 
 *---------------------------------------------------------------*/
void B1_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != B1_1) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i];
		Vr = b_ptr[i] - addval;
		g_ptr[i] = Yr;
		b_ptr[i] = (unsigned int)(Ur);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[2]--; im->bit_depth_max--;
	//im->bit_depth[1]--; 
	//im->bit_depth[2]--;  ????????????????????????????
	/* must also be decremented due to side effects in implementation*/
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B1_2toRGB() 
 *---------------------------------------------------------------*/
void B1_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != B1_2) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Vr = r_ptr[i]; /* first element	*/
		Yr = g_ptr[i];
		Ur = b_ptr[i] - addval;
		g_ptr[i] = Yr;
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = (unsigned int)(Vr);
	}

	/* decrease dynamic range */
	im->bit_depth[2]--; im->bit_depth_max--;
	//im->bit_depth[1]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B2_1toRGB() 
 *---------------------------------------------------------------*/
void B2_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != B2_1) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i];
		Vr = -(signed)b_ptr[i] + addval;
		r_ptr[i] = Yr;
		b_ptr[i] = (unsigned int)(Ur);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[2]--; im->bit_depth_max--;
	//im->bit_depth[1]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B2_3toRGB() 
 *---------------------------------------------------------------*/
void B2_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != B2_3) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i];
		Vr = -(signed)b_ptr[i] + addval;
		r_ptr[i] = Yr;
		g_ptr[i] = (unsigned int)(Ur);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[2]--; im->bit_depth_max--;
	//im->bit_depth[1]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B3_2toRGB() 
 *---------------------------------------------------------------*/
void B3_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != B3_2) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i];
		Vr = -(signed)b_ptr[i] + addval;
		b_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Ur);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[2]--; im->bit_depth_max--;
	//im->bit_depth[1]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B3_3toRGB() 
 *---------------------------------------------------------------*/
void B3_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != B3_3) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i];
		Ur = g_ptr[i]; /* first element	*/
		Vr = b_ptr[i] - addval;
		b_ptr[i] = Yr;
		g_ptr[i] = (unsigned int)(Ur);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[2]--; im->bit_depth_max--;
	//im->bit_depth[1]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B4_1toRGB() 
 *---------------------------------------------------------------*/
void B4_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != B4_1) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i];
		Vr = b_ptr[i] - addval;
		g_ptr[i] = Yr - (Vr >>1);
		b_ptr[i] = (unsigned int)(Ur);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[2]--; im->bit_depth_max--;
	//im->bit_depth[1]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B5_2toRGB() 
 *---------------------------------------------------------------*/
void B5_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != B5_2) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i] ;
		Ur = r_ptr[i]; /* first element	*/
		Vr = (signed)b_ptr[i] - addval;
		b_ptr[i] = Yr - (Vr >>1);
		r_ptr[i] = (unsigned int)(Ur);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[2]--; im->bit_depth_max--;
	//im->bit_depth[1]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B6_3toRGB() 
 *---------------------------------------------------------------*/
void B6_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Ur, Vr, addval;

	if (im->colour_channels < 3u || im->colour_space != B6_3) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i];
		Vr = b_ptr[i] - addval;
		b_ptr[i] = Yr - (Vr >>1);
		g_ptr[i] = (unsigned int)(Ur);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
	}

	/* decrease dynamic range */
	im->bit_depth[2]--; im->bit_depth_max--;
	//im->bit_depth[1]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	PEI09toRGB() 
 *---------------------------------------------------------------*/
void PEI09toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i;
	int  Yr, Cb, Cr, addval;

	if (im->colour_channels < 3u || im->colour_space != PEI09) return;
	
	/* move to non-negative range */
	addval = (1u << (im->bit_depth_max-1)) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Cb = g_ptr[i] - addval;
		Cr = b_ptr[i] - addval;
		g_ptr[i] = Yr - ((86*Cr + 29*Cb) >> 8);
		r_ptr[i] = (unsigned int)(Cr + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Cb + ((87*r_ptr[i] + 169*g_ptr[i]) >> 8));
	}

	/* decrease dynamic range */
	im->bit_depth[1]--; im->bit_depth_max--;
	im->bit_depth[2]--; 
	im->colour_space = RGBc;
}

#endif /* no MODULO_RCT */