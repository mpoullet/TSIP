/*****************************************************************
 *
 * File...:	colour4m_i.c
 * PVrpose:	implements inverse colour transforms and related
 * Author.:	Tilo Strutz
 * Date...: 16.09.2011
 *  changes:
 * 5.10.2011 Strutz: im->bit_depth_max-1 for addval
 * 27.03.2012 Strutz: new 108 colour spaces
 * 26.09.2013 new space: PEI09
 * 29.11.2013 Leipnitz: A7_4toRGB with Modulo-Operation
 * 16.12.2013 Leipnitz: Modulo-Operation for all colour spaces
 * 07.1.2014 Leipnitz: - Ax_2, Ax_3, Ax_5, Ax_8, Ax_9, B2_1, B2_3, B3_2, B5_2
 *											  changed shift to (addval2 - 1u) because of negation
 *											- swapped U and V 
 * 08.01.2015 Strutz: #ifdef MODULO_RCT for def-based compilation
 *****************************************************************/
#include <stdlib.h>
#include "colour4.h"

#ifdef MODULO_RCT

/*---------------------------------------------------------------
 *	A1_1toRGB() a1=a2=0
 *---------------------------------------------------------------*/
void A1_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A1_1) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		g_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_2toRGB()   a1=a2=0 R<=>G
 *---------------------------------------------------------------*/
void A2_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A2_2) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		r_ptr[i] = Yr;
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_3toRGB()   a1=a2=0 B<=>G
 *---------------------------------------------------------------*/
void A3_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A3_3) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		b_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_4toRGB() a1=a2=0 e=1/4
 *---------------------------------------------------------------*/
void A1_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A1_4) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_5toRGB()   a1=a2=0 e=1/4 R<=>G
 *---------------------------------------------------------------*/
void A2_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A2_5) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-R -(G-R)/4 = B -(G+3R)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr;
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_6toRGB()   a1=a2=0 e=1/4 B<=>G
 *---------------------------------------------------------------*/
void A3_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A3_6) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* G-B -(R-B)/4 = G -(R+3B)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_7toRGB() a1=a2=0 e=1/4 B<=>R
 *---------------------------------------------------------------*/
void A1_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A1_7) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* R-G -(B-G)/4 = R -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr;
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_8toRGB() a1=a2=0 e=1/4 R=>G=>B=>R
 *---------------------------------------------------------------*/
void A3_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A3_8) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = Ur + (Vr >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr;
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_9toRGB() a1=a2=0 e=1/4  R=>B=>G=>R
 *---------------------------------------------------------------*/
void A2_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A2_9) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);			/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr;
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_10toRGB() a1=a2=0 e=1/2
 *---------------------------------------------------------------*/
void A1_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A1_10) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);		/* B-G -(R-G)/2 = B -(R+G)/2	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_11toRGB() a1=a2=0 e=1/2 B<=>G
 *---------------------------------------------------------------*/
void A3_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A3_11) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);			/* G-B -(R-B)/2 = G -(R+B)/2	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_12toRGB() a1=a2=0 e=1/2  R<=>B
 *---------------------------------------------------------------*/
void A1_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A1_12) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);	/* R-G -(B-G)/2 = R -(B+G)/2	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr;
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

//##################################

/*---------------------------------------------------------------
 *	A2_1toRGB() a1=1 a2=0
 *---------------------------------------------------------------*/
void A2_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A2_1) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		g_ptr[i] = Yr - Vr;
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_2toRGB() a1=1 a2=0, R<=>G
 *---------------------------------------------------------------*/
void A1_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A1_2) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		r_ptr[i] = Yr - Vr;
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_3toRGB() a1=1 a2=0, B<=>G
 *---------------------------------------------------------------*/
void A2_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A2_3) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = -(signed)b_ptr[i] + (addval2 - 1u);  /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		b_ptr[i] = Yr - Vr;
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_4toRGB() a1=1 a2=0, e=1/4
 *---------------------------------------------------------------*/
void A2_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A2_4) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max);
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - Vr;
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_5toRGB() a1=1 a2=0, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A1_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A1_5) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max);
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - Vr;
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_6toRGB() a1=1 a2=0, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A2_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A2_6) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - Vr;
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_7toRGB() a1=1 a2=0, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A3_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A3_7) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - Vr;
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_8toRGB() a1=1 a2=0, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A1_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A1_8) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;  
		Vr = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = Ur + (Vr >> 2);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - Vr;
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);

	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_9toRGB() a1=1 a2=0, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A3_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A3_9) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - Vr;
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_10toRGB() a1=1 a2=0, e=1/2
 *---------------------------------------------------------------*/
void A2_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A2_10) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - Vr;
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_11toRGB() a1=1 a2=0, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void A2_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A2_11) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - Vr;
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_12toRGB() a1=1 a2=0, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void A3_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A3_12) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max);
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - Vr;
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}


//##################################

/*---------------------------------------------------------------
 *	A3_1toRGB() a1=0 a2=1
 *---------------------------------------------------------------*/
void A3_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A3_1) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		g_ptr[i] = Yr - Ur;
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_2toRGB() a1=0 a2=1, R<=>G
 *---------------------------------------------------------------*/
void A3_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A3_2) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		r_ptr[i] = Yr - Ur;
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_3toRGB() a1=0 a2=1, B<=>G
 *---------------------------------------------------------------*/
void A1_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A1_3) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		b_ptr[i] = Yr - Ur;
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_4toRGB() a1=0 a2=1, e=1/4
 *---------------------------------------------------------------*/
void A3_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A3_4) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - Ur;
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_5toRGB() a1=0 a2=1, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A3_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A3_5) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - Ur;
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_6toRGB() a1=0 a2=1, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A1_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A1_6) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - Ur;
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_7toRGB() a1=0 a2=1, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A2_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A2_7) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - Ur;
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_8toRGB() a1=0 a2=1, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A2_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A2_8) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - Ur;
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_9toRGB() a1=0 a2=1, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A1_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A1_9) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - Ur;
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A3_10toRGB() a1=0 a2=1, e=1/2
 *---------------------------------------------------------------*/
void A3_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A3_10) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - Ur;
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A1_11toRGB() a1=0 a2=1, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void A1_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A1_11) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - Ur;
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A2_12toRGB() a1=0 a2=1, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void A2_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A2_12) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - Ur;
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
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
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_1) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		g_ptr[i] = Yr - (Vr>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_2toRGB() a1=1/2 a2=0, R<=>G
 *---------------------------------------------------------------*/
void A4_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_2) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		r_ptr[i] = Yr - (Vr>>1);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_3toRGB() a1=1/2 a2=0, B<=>G
 *---------------------------------------------------------------*/
void A6_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_3) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		b_ptr[i] = Yr - (Vr>>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_4toRGB() a1=1/2 a2=0, e=1/4
 *---------------------------------------------------------------*/
void A4_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_4) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (Vr>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_5toRGB() a1=1/2 a2=0, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A4_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_5) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - (Vr>>1);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_6toRGB() a1=1/2 a2=0, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A6_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_6) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max);
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - (Vr>>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_7toRGB() a1=1/2 a2=0, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A5_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_7) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max);
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (Vr>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_8toRGB() a1=1/2 a2=0, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A5_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_8) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - (Vr>>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_9toRGB() a1=1/2 a2=0, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A6_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_9) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/ 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - (Vr>>1);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	_A4_10toRGB() a1=1/2 a2=0, e=1/2
 *---------------------------------------------------------------*/
void _A4_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_10) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (Vr>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_10toRGB() implemented as C4 ##########
 *---------------------------------------------------------------*/
void A4_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Y, Cg, Co, t, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_10) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Y = r_ptr[i]; /* first element	*/
		Co = g_ptr[i] - addval2;
		Cg = b_ptr[i] - addval2;
		t = Y;
		b_ptr[i] = (unsigned int)(Cg + t);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(t - (Co>>1));
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Co + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	_A6_11toRGB() a1=1/2 a2=0, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void _A6_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_11) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - (Vr>>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_11toRGB()  B<=>G implemented as C5  #########
 *---------------------------------------------------------------*/
void A6_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Y, Cg, Co, t, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_11) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Y = r_ptr[i]; /* first element	*/
		Co = g_ptr[i] - addval2;
		Cg = b_ptr[i] - addval2;
		t = Y;
		g_ptr[i] = (unsigned int)(Cg + t);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(t - (Co>>1));
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Co + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	_A5_12toRGB() a1=1/2 a2=0, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void _A5_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_12) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (Vr>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}


/*---------------------------------------------------------------
 *	A5_12toRGB()  implemented as C3 ##############
 *---------------------------------------------------------------*/
void A5_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Y, Cg, Co, t, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_12) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Y = r_ptr[i]; /* first element	*/
		Cg = g_ptr[i] - addval2;
		Co = b_ptr[i] - addval2;
		t = Y;
		r_ptr[i] = (unsigned int)(Cg + t);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(t - (Co>>1));
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Co + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}



//##################################

/*---------------------------------------------------------------
 *	A5_1toRGB() a2=1/2 a1=0
 *---------------------------------------------------------------*/
void A5_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_1) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		g_ptr[i] = Yr - (Ur>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_2toRGB() a2=1/2 a1=0, R<=>G
 *---------------------------------------------------------------*/
void A6_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_2) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		r_ptr[i] = Yr - (Ur>>1);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_3toRGB() a2=1/2 a1=0, B<=>G
 *---------------------------------------------------------------*/
void A5_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_3) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		b_ptr[i] = Yr - (Ur>>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_4toRGB() a2=1/2 a1=0, e=1/4
 *---------------------------------------------------------------*/
void A5_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_4) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (Ur>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_5toRGB() a2=1/2 a1=0, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A6_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_5) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - (Ur>>1);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_6toRGB() a2=1/2 a1=0, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A5_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_6) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - (Ur>>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_7toRGB() a2=1/2 a1=0, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A4_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_7) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (Ur>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_8toRGB() a2=1/2 a1=0, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A6_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_8) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = Ur + (Vr >> 2);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - (Ur>>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_9toRGB() a2=1/2 a1=0, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A4_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_9) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		//Ur = Ur + ((-Vr) >> 2);					
		r_ptr[i] = Yr - (Ur>>1);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		//b_ptr[i] = (unsigned int)(r_ptr[i]-Vr);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_10toRGB() a2=1/2 a1=0, e=1/2
 *---------------------------------------------------------------*/
void A5_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_10) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (Ur>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_11toRGB() a2=1/2 a1=0, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void A5_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_11) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - (Ur>>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_12toRGB() a2=1/2 a1=0, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void A4_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_12) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (Ur>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}



//##################################

/*---------------------------------------------------------------
 *	A6_1toRGB() a1=a2=1/2
 *---------------------------------------------------------------*/
void A6_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_1) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		g_ptr[i] = Yr - ((Vr+Ur)>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_2toRGB() a1=a2=1/2, R<=>G
 *---------------------------------------------------------------*/
void A5_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_2) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		r_ptr[i] = Yr - ((Vr+Ur)>>1);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_3toRGB() a1=a2=1/2, B<=>G
 *---------------------------------------------------------------*/
void A4_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_3) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		b_ptr[i] = Yr - ((Vr+Ur)>>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_4toRGB() a1=a2=1/2, e=1/4
 *---------------------------------------------------------------*/
void A6_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_4) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - ((Vr+Ur)>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_5toRGB() a1=a2=1/2, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A5_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_5) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - ((Vr+Ur)>>1);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_6toRGB() a1=1/2 a2=0, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A4_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_6) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - ((Vr+Ur)>>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_7toRGB() a1=a2=1/2, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A6_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_7) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - ((Vr+Ur)>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_8toRGB() a1=a2=1/2, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A4_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_8) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = Ur + (Vr >> 2);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - ((Vr+Ur)>>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A5_9toRGB() a1=a2=1/2, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A5_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A5_9) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - ((Vr+Ur)>>1);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}


/*---------------------------------------------------------------
 *	A6_10toRGB() a1=a2=1/2, e=1/2
 *---------------------------------------------------------------*/
void A6_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_10) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - ((Vr+Ur)>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A4_11toRGB() a1=a2=1/2, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void A4_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A4_11) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - ((Vr+Ur)>>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A6_12toRGB() a1=a2=1/2, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void A6_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A6_12) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - ((Vr+Ur)>>1);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}



//##################################

/*---------------------------------------------------------------
 *	A7_1toRGB() a1=a2=1/4  == YUUr
 *---------------------------------------------------------------*/
void A7_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_1) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u);	/*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		g_ptr[i] = Yr - ((Vr+Ur)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = Vr + (int)g_ptr[i];
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = Ur + (int)g_ptr[i];
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_2toRGB() a1=a2=1/4, R<=>G
 *---------------------------------------------------------------*/
void A8_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_2) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		r_ptr[i] = Yr - ((Vr+Ur)>>2);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_3toRGB() a1=a2=1/4, B<=>G
 *---------------------------------------------------------------*/
void A9_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_3) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		b_ptr[i] = Yr - ((Vr+Ur)>>2);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_4toRGB() a1=a2=1/4, e=1/4
 *---------------------------------------------------------------*/
void A7_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_4) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/
	for (i = 0; i < im->size; i++)
	{
		/*Yr = r_ptr[i]; /* first element	*/
		/*Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);					
		g_ptr[i] = Yr - ((Vr+Ur)>>2);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);*/
		Yr = r_ptr[i];
        Vr = g_ptr[i]-addval2;
        Ur = b_ptr[i]-addval2;
        Ur = Ur+(Vr>>2);
        Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
        g_ptr[i] = Yr-((Vr+Ur)>>2);
        g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (int)(Vr+g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (int)(Ur+g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
   	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_5toRGB() a1=a2=1/4, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A8_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_5) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - ((Vr+Ur)>>2);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_6toRGB() a1=a2=1/4, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A9_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_6) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - ((Vr+Ur)>>2);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_7toRGB() a1=a2=1/4, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A7_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_7) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - ((Vr+Ur)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_8toRGB() a1=a2=1/4, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A9_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_8) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = Ur + (Vr >> 2);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - ((Vr+Ur)>>2);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_9toRGB() a1=a2=1/4, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A8_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_9) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - ((Vr+Ur)>>2);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_10toRGB() a1=a2=1/4, e=1/2
 *---------------------------------------------------------------*/
void A7_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_10) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - ((Vr+Ur)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_11toRGB() a1=a2=1/4, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void A9_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_11) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - ((Vr+Ur)>>2);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_12toRGB() a1=a2=1/4, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void A7_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_12) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - ((Vr+Ur)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}



//##################################

/*---------------------------------------------------------------
 *	A8_1toRGB() a1=1/2 a2=1/4
 *---------------------------------------------------------------*/
void A8_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_1) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		g_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_2toRGB() a1=1/2 a2=1/4, R<=>G
 *---------------------------------------------------------------*/
void A7_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_2) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		r_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_3toRGB() a1=1/2 a2=1/4, B<=>G
 *---------------------------------------------------------------*/
void A8_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_3) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		b_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;

}

/*---------------------------------------------------------------
 *	A8_4toRGB() a1=1/2 a2=1/4, e=1/4
 *---------------------------------------------------------------*/
void A8_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_4) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_5toRGB() a1=1/2 a2=1/4, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A7_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_5) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_6toRGB() a1=1/2 a2=1/4, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A8_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_6) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_7toRGB() a1=1/2 a2=1/4, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A9_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_7) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_8toRGB() a1=1/2 a2=1/4, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A7_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_8) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_9toRGB() a1=1/2 a2=1/4, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A9_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_9) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_10toRGB() a1=1/2 a2=1/4, e=1/2
 *---------------------------------------------------------------*/
void A8_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
						 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_10) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_11toRGB() a1=1/2 a2=1/4, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void A8_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_11) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_12toRGB() a1=1/2 a2=1/4, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void A9_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_12) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (((Vr<<1)+Ur)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}


//##################################

/*---------------------------------------------------------------
 *	A9_1toRGB() a1=1/4 a2=1/2
 *---------------------------------------------------------------*/
void A9_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_1) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		g_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_2toRGB() a1=1/4 a2=1/2, R<=>G
 *---------------------------------------------------------------*/
void A9_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_2) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		r_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_3toRGB() a1=1/4 a2=1/2, B<=>G
 *---------------------------------------------------------------*/
void A7_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_3) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		b_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_4toRGB() a1=1/4 a2=1/2, e=1/4
 *---------------------------------------------------------------*/
void A9_4toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_4) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A9_5toRGB() a1=1/4 a2=1/2, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void A9_5toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_5) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_6toRGB() a1=1/4 a2=1/2, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void A7_6toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_6) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_7toRGB() a1=1/4 a2=1/2, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void A8_7toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_7) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_8toRGB() a1=1/4 a2=1/2, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void A8_8toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_8) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = Ur + (Vr >> 2);	
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_9toRGB() a1=1/4 a2=1/2, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void A7_9toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_9) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 2);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	_A9_10toRGB() a1=1/4 a2=1/2, e=1/2
 *---------------------------------------------------------------*/
void _A9_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
						 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_10) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}


/*---------------------------------------------------------------
 *	A9_10toRGB() implemented as C1 ##########
 *---------------------------------------------------------------*/
void A9_10toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Y, Cg, Co, t, addval2;

	if (im->colour_channels != 3u || im->colour_space != A9_10) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Y = r_ptr[i]; /* first element	*/
		Cg = g_ptr[i] - addval2;
		Co = b_ptr[i] - addval2;
		t = Y - (Cg >> 1);
		t = ModuloRange(t,-addval2,addval2 - 1u,addval);
		b_ptr[i] = (unsigned int)(Cg + t);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(t - (Co>>1));
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Co + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	_A7_11toRGB() a1=1/4 a2=1/2, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void _A7_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_11) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		b_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A7_11toRGB() implemented as C2 ####################
 *---------------------------------------------------------------*/
void A7_11toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Y, Cg, Co, t, addval2;

	if (im->colour_channels != 3u || im->colour_space != A7_11) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Y = r_ptr[i]; /* first element	*/
		Co = g_ptr[i] - addval2;
		Cg = b_ptr[i] - addval2;
		t = Y - (Cg >> 1);
		t = ModuloRange(t,-addval2,addval2 - 1u,addval);
		g_ptr[i] = (unsigned int)(Cg + t);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(t - (Co>>1));
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Co + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	_A8_12toRGB() a1=1/2 a2=1/4, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void _A8_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_12) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i] - addval2;
		Ur = Ur + (Vr >> 1);
		Ur = ModuloRange( Ur, -addval2, addval2 - 1u, addval);
		g_ptr[i] = Yr - (((Ur<<1)+Vr)>>2);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	A8_12toRGB() implemented as C3 ##########
 *---------------------------------------------------------------*/
void A8_12toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Y, Cg, Co, t, addval2;

	if (im->colour_channels != 3u || im->colour_space != A8_12) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max);
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Y = r_ptr[i]; /* first element	*/
		Cg = g_ptr[i] - addval2;
		Co = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		t = Y - (Cg >> 1);
		t = ModuloRange(t,-addval2,addval2 - 1u,addval);
		r_ptr[i] = (unsigned int)(Cg + t);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(t - (Co>>1));
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Co + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; /* Y keeps its range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B1_1toRGB() 
 *---------------------------------------------------------------*/
void B1_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != B1_1) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Ur = b_ptr[i];
		g_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur);
	}

	/* decrease dynamic range */
	/*/*im->bit_depth[1]--; im->bit_depth_max--;*/
	/*im->bit_depth[2]--; */
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B1_2toRGB() 
 *---------------------------------------------------------------*/
void B1_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != B1_2) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i]; /* first element	*/
		Yr = g_ptr[i];
		Vr = b_ptr[i] - addval2;
		g_ptr[i] = Yr;
		b_ptr[i] = (unsigned int)(Vr + g_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur);
	}

	/* decrease dynamic range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	//im->bit_depth[1]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B2_1toRGB() 
 *---------------------------------------------------------------*/
void B2_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != B2_1) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Ur = b_ptr[i];
		r_ptr[i] = Yr;
		g_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Ur);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; im->bit_depth_max--;*/
	//im->bit_depth[2]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B2_3toRGB() 
 *---------------------------------------------------------------*/
void B2_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != B2_3) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i];
		Vr = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		r_ptr[i] = Yr;
		g_ptr[i] = (unsigned int)(Ur);
		b_ptr[i] = (unsigned int)(Vr + r_ptr[i]);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	//im->bit_depth[1]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B3_2toRGB() 
 *---------------------------------------------------------------*/
void B3_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != B3_2) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i]; /* first element	*/
		Vr = -(signed)g_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		Yr = b_ptr[i];
		b_ptr[i] = Yr;
		g_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Ur);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; im->bit_depth_max--;*/
	//im->bit_depth[2]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B3_3toRGB() 
 *---------------------------------------------------------------*/
void B3_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != B3_3) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i]; /* first element	*/
		Vr = g_ptr[i] - addval2;
		Yr = b_ptr[i];
		b_ptr[i] = Yr;
		r_ptr[i] = (unsigned int)(Vr + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Ur);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; im->bit_depth_max--;*/
	//im->bit_depth[2]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B4_1toRGB() 
 *---------------------------------------------------------------*/
void B4_1toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != B4_1) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i];
		g_ptr[i] = Yr - (Ur >>1);
		b_ptr[i] = (unsigned int)(Vr);
		r_ptr[i] = (unsigned int)(Ur + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; im->bit_depth_max--;*/
	//im->bit_depth[2]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B5_2toRGB() 
 *---------------------------------------------------------------*/
void B5_2toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != B5_2) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr = r_ptr[i]; /* first element	*/
		Yr = g_ptr[i] ;
		Ur = -(signed)b_ptr[i] + (addval2 - 1u); /* Angleichung der Wertebereiche von Ur und Vr (-128...127)*/
		b_ptr[i] = Yr - (Ur >>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Vr);
		g_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[2]--; im->bit_depth_max--; */
	//im->bit_depth[1]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	B6_3toRGB() 
 *---------------------------------------------------------------*/
void B6_3toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Vr, Ur, addval2;

	if (im->colour_channels != 3u || im->colour_space != B6_3) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max-1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Ur = g_ptr[i] - addval2;
		Vr = b_ptr[i];
		b_ptr[i] = Yr - (Ur >>1);
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
		g_ptr[i] = (unsigned int)(Vr);
		r_ptr[i] = (unsigned int)(Ur + b_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; im->bit_depth_max--;*/
	//im->bit_depth[2]--; 
	im->colour_space = RGBc;
}

/*---------------------------------------------------------------
 *	PEI09toRGB() 
 *---------------------------------------------------------------*/
void PEI09toRGB( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	unsigned int i, addval;
	int  Yr, Cb, Cr, addval2;

	if (im->colour_channels != 3u || im->colour_space != PEI09) return;
	
	/* move to non-negative range */
	addval  = 1u << (im->bit_depth_max); 
	addval2 = 1u << (im->bit_depth_max - 1u); /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i]; /* first element	*/
		Cb = g_ptr[i] - addval2;
		Cr = b_ptr[i] - addval2;
		g_ptr[i] = Yr - ((86*Cr + 29*Cb) >> 8);
		g_ptr[i] = ModuloRange( g_ptr[i], 0, addval - 1u, addval);
		r_ptr[i] = (unsigned int)(Cr + g_ptr[i]);
		r_ptr[i] = ModuloRange( r_ptr[i], 0, addval - 1u, addval);
		b_ptr[i] = (unsigned int)(Cb + ((87*r_ptr[i] + 169*g_ptr[i]) >> 8));
		b_ptr[i] = ModuloRange( b_ptr[i], 0, addval - 1u, addval);
	}

	/* decrease dynamic range */
	/*im->bit_depth[1]--; im->bit_depth_max--;*/
	/*im->bit_depth[2]--;*/ 
	im->colour_space = RGBc;
}
#endif /* MODULO_RCT	*/