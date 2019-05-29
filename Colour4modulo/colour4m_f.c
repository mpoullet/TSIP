/*****************************************************************
 *
 * File...:	colour4m_f.c
 * PVrpose:	implements forward colour transforms and related
 * Author.:	Tilo Strutz
 * Date...: 17.09.2011
 * changes:
 * 5.10.2011 Strutz: im->bit_depth_max for addval
 * 27.03.2012 Strutz: new 108 colour spaces
 * 20.04.2012 Strutz: new 118 colour spaces
 * 26.09.2013 new space: PEI09
 * 29.11.2013 Leipnitz: RGBtoA7_4 with Modulo-Operation
 * 16.12.2013 Leipnitz: Modulo-Operation for all colour spaces + 8 bit limit for Y, U and V
 * 6.1.2014 Leipnitz: output number of Modulo-Operation
 * 07.1.2014 Leipnitz: - Ax_2, Ax_3, Ax_5, Ax_8, Ax_9, B2_1, B2_3, B3_2, B5_2
 *										  changed shift to (addval2 - 1u) because of negation
 *											- swapped U and V 
 * 08.01.2015 Strutz: #ifdef MODULO_RCT for def-based compilation
*****************************************************************/
#include <stdlib.h>
#include "colour4.h"
#include "Form1.h"

long numOfModulo=0;

#ifdef MODULO_RCT

/*---------------------------------------------------------------
 *   Modulo-Operation()
 *---------------------------------------------------------------*/
int ModuloRange( int e, int l, int u, unsigned int r)
{
		if (e > u) 
			{e -= r;}
		else if (e < l) 
			{e += r;}
		else {e = e;}
		return e;
}
/*#define MODULO_RANGE( e, l, u, r) 		\
(	(e) < (l) ? (e) + (r) : 		\
		(e) > (u) ? (e) - (r) : (e))
*/
/*---------------------------------------------------------------
 *	RGBtoA1_1()
 *---------------------------------------------------------------*/
void RGBtoA1_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;           	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Vr2 != Vr || Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A1_1;
}

/*---------------------------------------------------------------
 *	RGBtoA2_2() R<=>G
 *---------------------------------------------------------------*/
void RGBtoA2_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int  Vr, Ur, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		// Yr = r_ptr[i];									/* R	*/
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);   
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Vr2 != Vr || Ur2 != Ur) 
			(numOfModulo++);
		//r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A2_2;
}

/*---------------------------------------------------------------
 *	RGBtoA3_3()   a1=a2=0 B<=>G
 *---------------------------------------------------------------*/
void RGBtoA3_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = b_ptr[i];									/* B	*/
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Vr2 != Vr || Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = -Ur + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A3_3;
}

/*---------------------------------------------------------------
 *	RGBtoA1_4() a1=a2=0 e=1/4
 *---------------------------------------------------------------*/
void RGBtoA1_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/ 

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Vr2 != Vr || Ur2 != Ur) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/ 
	im->colour_space = A1_4;
}

/*---------------------------------------------------------------
*	RGBtoA2_5()   a1=a2=0 e=1/4 R<=>G
*---------------------------------------------------------------*/
void RGBtoA2_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i];									/* R	*/
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Vr2 != Vr || Ur2 != Ur) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-R -(G-R)/4 = B -(G+3R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/ 
	im->colour_space = A2_5;
}

/*---------------------------------------------------------------
*	RGBtoA3_6()  a1=a2=0 e=1/4 B<=>G
*---------------------------------------------------------------*/
void RGBtoA3_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = b_ptr[i];									/* B	*/
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Vr2 != Vr || Ur2 != Ur) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-B -(R-B)/4 = G -(R+3B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/ 
	im->colour_space = A3_6;
}

/*---------------------------------------------------------------
*	RGBtoA1_7()  a1=a2=0 e=1/4 B<=>R
*---------------------------------------------------------------*/
void RGBtoA1_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Vr2 != Vr || Ur2 != Ur) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-G -(B-G)/4 = R -(R+3G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
	im->colour_space = A1_7;
}

/*---------------------------------------------------------------
 *	RGBtoA3_8()  a1=a2=0 e=1/4 R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA3_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = b_ptr[i];									/* B	*/
		Vr2 = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Vr2 != Vr || Ur2 != Ur)
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/ 
	im->colour_space = A3_8;
}

/*---------------------------------------------------------------
 *	RGBtoA2_9() a1=a2=0 e=1/4  R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA2_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i];									/* R	*/
		Vr2 = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Vr2 != Vr || Ur2 != Ur)
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/ 
	im->colour_space = A2_9;
}

/*---------------------------------------------------------------
 *	RGBtoA1_10() a1=a2=0 e=1/2
 *---------------------------------------------------------------*/
void RGBtoA1_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Vr2 != Vr || Ur2 != Ur)
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
	im->colour_space = A1_10;
}


/*---------------------------------------------------------------
 *	RGBtoA3_11() a1=a2=0 e=1/2 B<=>G
 *---------------------------------------------------------------*/
void RGBtoA3_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr = b_ptr[i];									/* B	*/
				if (Vr2 != Vr || Ur2 != Ur)
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* G-B -(R-B)/2 = G -(R+B)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
	im->colour_space = A3_11;
}


/*---------------------------------------------------------------
 *	RGBtoA1_12()  a1=a2=0 e=1/2  R<=>B
 *---------------------------------------------------------------*/
void RGBtoA1_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Vr2 != Vr || Ur2 != Ur)
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* R-G -(B-G)/2 = R -(B+G)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
	im->colour_space = A1_12;
}

/*---------------------------------------------------------------
 *	RGBtoA2_1()  a1=1, 
 *---------------------------------------------------------------*/
void RGBtoA2_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + Vr;							/* G + R-G = R	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A2_1;
}

/*---------------------------------------------------------------
 *	RGBtoA1_2()  a1=1, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA1_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + Vr;							/* R + G-R = G	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A1_2;
}

/*---------------------------------------------------------------
 *	RGBtoA2_3()  a1=1, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA2_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + Vr;							/* B + R-B = R	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = -Ur + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A2_3;
}

/*---------------------------------------------------------------
 *	RGBtoA2_4()  a1=1, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA2_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + Vr;							/* G + R-G = R	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A2_4;
}

/*---------------------------------------------------------------
 *	RGBtoA1_5()  a1=1, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA1_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + Vr;							/* R + G-R = G	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A1_5;
}

/*---------------------------------------------------------------
 *	RGBtoA2_6()  a1=1, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA2_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + Vr;							/* B + R-B = R	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A2_6;
}

/*---------------------------------------------------------------
 *	RGBtoA3_7()  a1=1, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA3_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + Vr;							/* G + B-G = B	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A3_7;
}

/*---------------------------------------------------------------
 *	RGBtoA1_8()  a1=1, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA1_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + Vr;							/* B + G-B = G	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A1_8;
}
/*---------------------------------------------------------------
 *	RGBtoA3_9()  a1=1, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA3_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + Vr;							/* R + B-R = B	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A3_9;
}

/*---------------------------------------------------------------
 *	RGBtoA2_10()  a1=1, e=1/2
 *---------------------------------------------------------------*/
void RGBtoA2_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;   
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + Vr;							/* G + R-G = R	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A2_10;
}
/*---------------------------------------------------------------
 *	RGBtoA2_11()  a1=1, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA2_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + Vr;							/* B + R-B = R	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A2_11;
}

/*---------------------------------------------------------------
 *	RGBtoA3_12()  a1=1, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA3_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + Vr;							/* G + B-G = B	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A3_12;
}

//######################


/*---------------------------------------------------------------
 *	RGBtoA3_1()  a2=1, 
 *---------------------------------------------------------------*/
void RGBtoA3_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + Ur;							/* G + B-G = B	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A3_1;
}

/*---------------------------------------------------------------
 *	RGBtoA3_2()  a2=1, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA3_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + Ur;							/* R + B-R = B	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A3_2;
}

/*---------------------------------------------------------------
 *	RGBtoA1_3()  a2=1, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA1_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + Ur;							/* B + G-B = G	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = -Ur + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A1_3;
}

/*---------------------------------------------------------------
 *	RGBtoA3_4()  a2=1, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA3_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + Ur;							/* G + B-G = B	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A3_4;
}

/*---------------------------------------------------------------
 *	RGBtoA3_5()  a2=1, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA3_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + Ur;							/* R + B-R = B	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A3_5;
}

/*---------------------------------------------------------------
 *	RGBtoA1_6()  a2=1, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA1_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + Ur;							/* B + G-B = G	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A1_6;
}

/*---------------------------------------------------------------
 *	RGBtoA2_7()  a2=1, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA2_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + Ur;							/* G + R-G = R	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A2_7;
}

/*---------------------------------------------------------------
 *	RGBtoA2_8()  a2=1, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA2_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + Ur;							/* B + R-B = R	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A2_8;
}
/*---------------------------------------------------------------
 *	RGBtoA1_9()  a2=1, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA1_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + Ur;							/* R + G-R = G	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A1_9;
}

/*---------------------------------------------------------------
 *	RGBtoA3_10()  a2=1, e=1/2
 *---------------------------------------------------------------*/
void RGBtoA3_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + Ur;							/* G + B-G = B	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A3_10;
}

/*---------------------------------------------------------------
 *	RGBtoA1_11()  a2=1, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA1_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + Ur;							/* B + G-B = G	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A1_11;
}

/*---------------------------------------------------------------
 *	RGBtoA2_12()  a2=1, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA2_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + Ur;							/* G + R-G = R	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A2_12;
}
//##############################


/*---------------------------------------------------------------
 *	RGBtoA4_1()  a1=1/2, 
 *---------------------------------------------------------------*/
void RGBtoA4_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (Vr>>1);				/* G + (R-G)/2 = (G+R)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A4_1;
}

/*---------------------------------------------------------------
 *	RGBtoA4_2()  a1=1/2, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA4_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + (Vr>>1);				/* R + (G-R)/2 = (G+R)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A4_2;
}

/*---------------------------------------------------------------
 *	RGBtoA6_3()  a1=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA6_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (Vr>>1);				/* B + (R-B)/2 = (R+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = -Ur + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A6_3;
}

/*---------------------------------------------------------------
 *	RGBtoA4_4()  a1=1/2, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA4_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (Vr>>1);				/* G + (R-G)/2 = (R+G)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A4_4;
}

/*---------------------------------------------------------------
 *	RGBtoA4_5()  a1=1/2, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA4_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + (Vr>>1);				/* R + (G-R)/2 = (G+R)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A4_5;
}

/*---------------------------------------------------------------
 *	RGBtoA6_6()  a1=1/2, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA6_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (Vr>>1);				/* B + (R-B) = (R+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A6_6;
}

/*---------------------------------------------------------------
 *	RGBtoA5_7()  a1=1/2, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA5_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (Vr>>1);				/* G + (B-G) = (B+G)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A5_7;
}

/*---------------------------------------------------------------
 *	RGBtoA5_8()  a1=1/2, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA5_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (Vr>>1);				/* B + (G-B)/2 = (G+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A5_8;
}
/*---------------------------------------------------------------
 *	RGBtoA6_9()  a1=1/2, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA6_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + (Vr>>1);			  /* R + (B-R)/2 = (B+R)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A6_9;
}

/*---------------------------------------------------------------
 *	_RGBtoA4_10()  a1=1/2, e=1/2
 *---------------------------------------------------------------*/
void _RGBtoA4_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (Vr>>1);				/* G + (R-G)/2 = (R+G)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A4_10;
}

/*---------------------------------------------------------------
 *	RGBtoA4_10() implemented as C4
 *---------------------------------------------------------------*/
void RGBtoA4_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Y, Cg, Co, t, Cg2, Co2, t2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Co2 = r_ptr[i] - g_ptr[i];	/* R-G	*/
		Co = ModuloRange(Co2,-addval2,addval2 - 1u,addval);
		t2  = g_ptr[i] + (Co >> 1);/* (G+R)/2	*/
		t  = ModuloRange(t2,0,addval-1u,addval);
		Cg2 = b_ptr[i] - t;				/* B - (G+R)/2	*/
		Cg = ModuloRange(Cg2,-addval2,addval2 - 1u,addval);
		Y  = t ;									/* (G+R)/2 	*/
				if (Co2 != Co || t2 != t || Cg2 != Cg ) 
			(numOfModulo++);
		r_ptr[i] = (int)Y;
		g_ptr[i] = (int)(Co + addval2);
		b_ptr[i] = (int)(Cg + addval2);
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/

	im->colour_space = A4_10;
}


/*---------------------------------------------------------------
 *	_RGBtoA6_11()  a1=1/2, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void _RGBtoA6_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								 unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (Vr>>1);				/* B + (R-B)/2 = (R+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A6_11;
}

/*---------------------------------------------------------------
 *	RGBtoA6_11()  A6_11 implemented as C5  ####################
 *---------------------------------------------------------------*/
void RGBtoA6_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								 unsigned int *b_ptr)
{
	int Y, Cg, Co, Cg2, Co2,t2, t, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Co2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Co = ModuloRange(Co2,-addval2,addval2 - 1u,addval);
		t2  = b_ptr[i] + (Co >> 1);	/* (B+R)/2	*/
		t  = ModuloRange(t2,0,addval-1u,addval);
		Cg2 = g_ptr[i] - t;					/* G - (R+B)/2	*/
		Cg = ModuloRange(Cg2,-addval2,addval2 - 1u,addval);
		Y  = t ;										/* (R+B)/2 */
				if (Co2 != Co || t2 != t || Cg2 != Cg ) 
			(numOfModulo++);
		r_ptr[i] = (int)Y;
		g_ptr[i] = (int)(Co + addval2);
		b_ptr[i] = (int)(Cg + addval2);
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/

	im->colour_space = A6_11;
}
/*---------------------------------------------------------------
 *	_RGBtoA5_12()  a1=1/2, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void _RGBtoA5_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (Vr>>1);				/* G + (B-G)/2 = (B+G)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A5_12;
}

/*---------------------------------------------------------------
 *	RGBtoA5_12() implemented as C3 ####################
 *---------------------------------------------------------------*/
void RGBtoA5_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Y, Cg, Co, Co2, Cg2, t2, t, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Co2 = b_ptr[i] - g_ptr[i];	/* B-G	*/
		Co = ModuloRange(Co2,-addval2,addval2 - 1u,addval);
		t2  = g_ptr[i] + (Co >> 1);/* (G+B)/2	*/
		t  = ModuloRange(t2,0,addval-1u,addval);
		Cg2 = r_ptr[i] - t;				/* R - (G+B)/2	*/
		Cg = ModuloRange(Cg2,-addval2,addval2 - 1u,addval);
		Y  = t ;									/* (G+B)/2 	*/
				if (Co2 != Co || t2 != t || Cg2 != Cg ) 
			(numOfModulo++);
		r_ptr[i] = (int)Y;
		g_ptr[i] = (int)(Cg + addval2);
		b_ptr[i] = (int)(Co + addval2);
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/

	im->colour_space = A5_12;
}

//##############################


/*---------------------------------------------------------------
 *	RGBtoA5_1()  a2=1/2, 
 *---------------------------------------------------------------*/
void RGBtoA5_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (Ur>>1);				/* G + (B-G)/2 = (G+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A5_1;
}

/*---------------------------------------------------------------
 *	RGBtoA6_2()  a2=1/2, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA6_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + (Ur>>1);				/* R + (B-R)/2 = (B+R)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A6_2;
}

/*---------------------------------------------------------------
 *	RGBtoA5_3()  a2=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA5_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (Ur>>1);				/* B + (G-B)/2 = (G+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = -Ur + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A5_3;
}

/*---------------------------------------------------------------
 *	RGBtoA5_4()  a2=1/2, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA5_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (Ur>>1);				/* G + (B-G)/2 = (B+G)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A5_4;
}

/*---------------------------------------------------------------
 *	RGBtoA6_5()  a1=1/2, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA6_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + (Ur>>1);				/* R + (B-R)/2 = (B+R)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A6_5;
}

/*---------------------------------------------------------------
 *	RGBtoA5_6()  a2=1/2, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA5_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (Ur>>1);				/* B + (G-B) = (G+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A5_6;
}

/*---------------------------------------------------------------
 *	RGBtoA4_7()  a2=1/2, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA4_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (Ur>>1);				/* G + (R-G) = (R+G)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A4_7;
}

/*---------------------------------------------------------------
 *	RGBtoA6_8()  a2=1/2, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA6_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (Ur>>1);				/* B + (R-B)/2 = (R+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A6_8;
}
/*---------------------------------------------------------------
 *	RGBtoA4_9()  a2=1/2, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA4_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		//Vr = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Ur2 = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + (Ur>>1);			  /* R + (G-R)/2 = (G+R)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		//Ur = Ur - ((-Vr) >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur)
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A4_9;
}

/*---------------------------------------------------------------
 *	RGBtoA5_10()  a2=1/2, e=1/2
 *---------------------------------------------------------------*/
void RGBtoA5_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (Ur>>1);				/* G + (B-G)/2 = (G+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A5_10;
}

/*---------------------------------------------------------------
 *	RGBtoA5_11()  a2=1/2, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA5_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (Ur>>1);				/* B + (G-B)/2 = (G+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A5_11;
}

/*---------------------------------------------------------------
 *	RGBtoA4_12()  a2=1/2, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA4_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (Ur>>1);				/* G + (R-G)/2 = (R+G)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A4_12;
}

//##############################


/*---------------------------------------------------------------
 *	RGBtoA6_1()  a1=a2=1/2, 
 *---------------------------------------------------------------*/
void RGBtoA6_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + ((Vr+Ur)>>1);		/* G + (R-G+B-G)/2 = (R+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A6_1;
}

/*---------------------------------------------------------------
 *	RGBtoA5_2()  a1=a2=1/2, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA5_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + ((Vr+Ur)>>1);		/* R + (G-R+B-R)/2 = (B+G)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A5_2;
}

/*---------------------------------------------------------------
 *	RGBtoA4_3()  a1=a2=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA4_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + ((Vr+Ur)>>1);		/* B + (R-B+G-B)/2 = (R+G)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = -Ur + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A4_3;
}

/*---------------------------------------------------------------
 *	RGBtoA6_4()  a1=a2=1/2, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA6_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + ((Vr+Ur)>>1);		/* G + (R-G+B-G)/2 = (B+R)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A6_4;
}

/*---------------------------------------------------------------
 *	RGBtoA5_5()  a1=a2=1/2, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA5_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + ((Vr+Ur)>>1);		/* R + (G-R+B-R)/2 = (B+G)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A5_5;
}

/*---------------------------------------------------------------
 *	RGBtoA4_6()  a1=a2=1/2, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA4_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + ((Vr+Ur)>>1);		/* B + (R-B+G-B) = (G+R)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A4_6;
}

/*---------------------------------------------------------------
 *	RGBtoA6_7()  a1=a2=1/2, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA6_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + ((Vr+Ur)>>1);		/* G + (B-G+R-G) = (R+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A6_7;
}

/*---------------------------------------------------------------
 *	RGBtoA4_8()  a1=a2=1/2, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA4_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + ((Vr+Ur)>>1);		/* B + (G-B+R-B)/2 = (R+G)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A4_8;
}
/*---------------------------------------------------------------
 *	RGBtoA5_9()  a1=a2=1/2, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA5_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + ((Vr+Ur)>>1);		/* R + (B-R+G-R)/2 = (G+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A5_9;
}

/*---------------------------------------------------------------
 *	RGBtoA6_10()  a1=a2=1/2, e=1/2
 *---------------------------------------------------------------*/
void RGBtoA6_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + ((Vr+Ur)>>1);		/* G + (R-G+B-G)/2 = (R+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A6_10;
}

/*---------------------------------------------------------------
 *	RGBtoA4_11()  a1=a2=1/2, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA4_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + ((Vr+Ur)>>1);		/* B + (R-B-G-B)/2 = (G+R)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A4_11;
}

/*---------------------------------------------------------------
 *	RGBtoA6_12()  a1=a2=1/2, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA6_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + ((Vr+Ur)>>1);		/* G + (B-G+R-G)/2 = (R+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A6_12;
}


//##############################


/*---------------------------------------------------------------
 *	RGBtoA7_1()  a1=a2=1/4,  == YUUr
 *---------------------------------------------------------------*/
void RGBtoA7_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval  =  1u <<  im->bit_depth_max; 
	addval2 =  1u << (im->bit_depth_max - 1u);         /*Hälfte von addval*/
	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + ((Vr+Ur)>>2);	/* G + (R-G+B-G)/4 = (R+2G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/ 
 
	im->colour_space = A7_1;
}

/*---------------------------------------------------------------
 *	RGBtoA8_2()  a1=a2=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA8_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + ((Vr+Ur)>>2);		/* R + (G-R+B-R)/4 = (2R+B+G)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A8_2;
}

/*---------------------------------------------------------------
 *	RGBtoA9_3()  a1=a2=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA9_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + ((Vr+Ur)>>2);		/* B + (R-B+G-B)/4 = (R+G+2B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = -Ur + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A9_3;
}

/*---------------------------------------------------------------
 *	RGBtoA7_4()  a1=a2=1/4, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA7_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		/*Vr = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		/*Ur = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		/*Yr = g_ptr[i] + ((Vr+Ur)>>2);		/* G + (R-G+B-G)/4 = (B+2G+R)/4	*/
		/*Ur = Ur - (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		/*r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;*/
		Vr2 = r_ptr[i]- (int)g_ptr[i];							/* R-G	*/
    Vr  = ModuloRange(Vr2,-addval2,addval2 - 1u,addval);
		Ur2 = b_ptr[i]-(int)g_ptr[i];							/* B-G	*/
		Ur  = ModuloRange(Ur2,-addval2,addval2 - 1u,addval);
		Yr2 = g_ptr[i]+((Vr+Ur)>>2);								/* G + (R-G+B-G)/4 = (B+2G+R)/4	*/
		Yr  = ModuloRange(Yr2,0,addval-1u,addval);
		if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr>>2);										/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		if (Ur != Ur2 ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}

		
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
		im->colour_space = A7_4;
}

/*---------------------------------------------------------------
 *	RGBtoA8_5()  a1=a2=1/4, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA8_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, Yr2, addval2;
	unsigned int i, addval,numOfModulo=0;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/ 	

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + ((Vr+Ur)>>2);		/* R + (G-R+B-R)/4 = (2R+B+G)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur != Ur2 ) 
			(numOfModulo++);

		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A8_5;
}

/*---------------------------------------------------------------
 *	RGBtoA9_6()  a1=a2=1/4, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA9_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, Ur2, Yr2, addval2;
	unsigned int i,numOfModulo=0, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + ((Vr+Ur)>>2);		/* B + (R-B+G-B)/4 = (G+R+2B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
								if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
					if (Ur != Ur2 ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A9_6;
}

/*---------------------------------------------------------------
 *	RGBtoA7_7()  a1=a2=1/4, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA7_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + ((Vr+Ur)>>2);		/* G + (B-G+R-G)/4 = (B+2G+R)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-G -(B-G)/4 = R -(B+3G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A7_7;
}

/*---------------------------------------------------------------
 *	RGBtoA9_8()  a1=a2=1/4, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA9_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + ((Vr+Ur)>>2);		/* B + (G-B+R-B)/4 = (R+G+2B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
				if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A9_8;
}
/*---------------------------------------------------------------
 *	RGBtoA8_9()  a1=a2=1/4, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA8_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + ((Vr+Ur)>>2);		/* R + (B-R+G-R)/4 = (2R+G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
				if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A8_9;
}

/*---------------------------------------------------------------
 *	RGBtoA7_10()  a1=a2=1/4, e=1/2
 *---------------------------------------------------------------*/
void RGBtoA7_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{

	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + ((Vr+Ur)>>2);		/* G + (R-G+B-G)/4 = (R+2G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A7_10;
}

/*---------------------------------------------------------------
 *	RGBtoA9_11()  a1=a2=1/4, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA9_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + ((Vr+Ur)>>2);		/* B + (R-B-G-B)/4 = (G+R+2B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A9_11;
}

/*---------------------------------------------------------------
 *	RGBtoA7_12()  a1=a2=1/4, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA7_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + ((Vr+Ur)>>2);		/* G + (B-G+R-G)/4 = (R+2G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A7_12;
}



//##############################


/*---------------------------------------------------------------
 *	RGBtoA8_1()  a1=1/2 a2=1/4, 
 *---------------------------------------------------------------*/
void RGBtoA8_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (((Vr<<1)+Ur)>>2);/* G + (R-G+R-G+B-G)/4 = (2R+G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A8_1;
}

/*---------------------------------------------------------------
 *	RGBtoA7_2()  a1=1/2 a2=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA7_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + (((Vr<<1)+Ur)>>2);/* R + (G-R+G-R+B-R)/4 = (R+2G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A7_2;
}

/*---------------------------------------------------------------
 *	RGBtoA8_3()  a1=1/2 a2=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA8_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (((Vr<<1)+Ur)>>2);/* B + (R-B+R-B+G-B)/4 = (2R+G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = -Ur + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A8_3;
}

/*---------------------------------------------------------------
 *	RGBtoA8_4()  a1=1/2 a2=1/4, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA8_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (((Vr<<1)+Ur)>>2);/* G + (R-G+R-G+B-G)/4 = (2R+G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A8_4;
}

/*---------------------------------------------------------------
 *	RGBtoA7_5() a1=1/2 a2=1/4, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA7_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + (((Vr<<1)+Ur)>>2);/* R + (G-R+G-R+B-R)/4 = (R+2G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A7_5;
}

/*---------------------------------------------------------------
 *	RGBtoA8_6()  a1=1/2 a2=1/4, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA8_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (((Vr<<1)+Ur)>>2);/* B + (R-B+R-B+G-B)/4 = (2R+G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A8_6;
}

/*---------------------------------------------------------------
 *	RGBtoA9_7()  a1=1/2 a2=1/4, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA9_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (((Vr<<1)+Ur)>>2);/* G + (B-G+B-G+R-G)/4 = (R+G+2B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A9_7;
}

/*---------------------------------------------------------------
 *	RGBtoA7_8()  a1=1/2 a2=1/4, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA7_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (((Vr<<1)+Ur)>>2);/* B + (G-B+G-B+R-B)/4 = (R+2G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A7_8;
}
/*---------------------------------------------------------------
 *	RGBtoA9_9()  a1=1/2 a2=1/4, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA9_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + (((Vr<<1)+Ur)>>2);/* R + (B-R+B-R+G-R)/4 = (R+G+2B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A9_9;
}

/*---------------------------------------------------------------
 *	RGBtoA8_10()  a1=1/2 a2=1/4, e=1/2
 *---------------------------------------------------------------*/
void RGBtoA8_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (((Vr<<1)+Ur)>>2);/* G + (R-G+R-G+B-G)/4 = (2R+G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A8_10;
}

/*---------------------------------------------------------------
 *	RGBtoA8_11()  a1=1/2 a2=1/4, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA8_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (((Vr<<1)+Ur)>>2);/* B + (R-B+R-B+G-B)/4 = (2R+G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A8_11;
}

/*---------------------------------------------------------------
 *	RGBtoA9_12()  a1=1/2 a2=1/4, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA9_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (((Vr<<1)+Ur)>>2);/* G + (B-G+B-G+R-G)/4 = (R+G+2B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A9_12;
}



//##############################


/*---------------------------------------------------------------
 *	RGBtoA9_1()  a1=1/4 a2=1/2, 
 *---------------------------------------------------------------*/
void RGBtoA9_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (((Ur<<1)+Vr)>>2);/* G + (R-G+B-G+B-G)/4 = (R+G+2B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A9_1;
}

/*---------------------------------------------------------------
 *	RGBtoA9_2()  a1=1/4 a2=1/2, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA9_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + (((Ur<<1)+Vr)>>2);/* R + (G-R+B-R+B-R)/4 = (R+G+2B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A9_2;
}

/*---------------------------------------------------------------
 *	RGBtoA7_3()  a1=1/4 a2=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA7_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (((Ur<<1)+Vr)>>2);/* B + (R-B+G-B+G-B)/4 = (R+2G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = -Ur + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A7_3;
}

/*---------------------------------------------------------------
 *	RGBtoA9_4()  a1=1/4 a2=1/2, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA9_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (((Ur<<1)+Vr)>>2);/* G + (R-G+B-G+B-G)/4 = (R+G+2B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A9_4;
}

/*---------------------------------------------------------------
 *	RGBtoA9_5() a1=1/4 a2=1/2, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA9_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + (((Ur<<1)+Vr)>>2);/* R + (G-R+B-R+B-R)/4 = (R+G+2B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A9_5;
}

/*---------------------------------------------------------------
 *	RGBtoA7_6()  a1=1/4 a2=1/2, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA7_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (((Ur<<1)+Vr)>>2);/* B + (R-B+G-B+G-B)/4 = (R+2G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A7_6;
}

/*---------------------------------------------------------------
 *	RGBtoA8_7()  a1=1/4 a2=1/2, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA8_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (((Ur<<1)+Vr)>>2);/* G + (B-G+R-G+R-G)/4 = (2R+G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A8_7;
}

/*---------------------------------------------------------------
 *	RGBtoA8_8()  a1=1/4 a2=1/2, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA8_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (((Ur<<1)+Vr)>>2);/* B + (G-B+R-B+R-B)/4 = (2R+G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps i
	ts range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A8_8;
}

/*---------------------------------------------------------------
 *	RGBtoA7_9()  a1=1/4 a2=1/2, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA7_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = r_ptr[i] + (((Ur<<1)+Vr)>>2);/* R + (B-R+G-R+G-R)/4 = (R+2G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A7_9;
}

/*---------------------------------------------------------------
 *	_RGBtoA9_10()  a1=1/4 a2=1/2, e=1/2
 *---------------------------------------------------------------*/
void _RGBtoA9_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (((Ur<<1)+Vr)>>2);/* G + (R-G+B-G+B-G)/4 = (R+G+2B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
			(numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
						if (Ur2 != Ur) 
			(numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A9_10;
}

/*---------------------------------------------------------------
 *	RGBtoA9_10() implemented as C1 ##########
 *---------------------------------------------------------------*/
void RGBtoA9_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Y, Cg, Co, t, Y2, Cg2, Co2, t2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Co2 = r_ptr[i] - g_ptr[i];  /* R-G	*/
		Co = ModuloRange(Co2,-addval2,addval2 - 1u,addval);
		t2  = g_ptr[i] + (Co >> 1);  /* (G+R)/2	*/
		t  = ModuloRange(t2,0,addval-1u,addval);
		Cg2 = b_ptr[i] - t;					/* B - (G+R)/2	*/
		Cg = ModuloRange(Cg2,-addval2,addval2 - 1u,addval);
		Y2  = t + (Cg >> 1);					/* (G+R)/2 + B/2 - (G+R)/4 = ((R+G+2B)/4	*/
		Y  = ModuloRange(Y2,0,addval-1u,addval);
						if (Co2 != Co || t2 != t || Cg2 != Cg || Y2 != Y ) 
			(numOfModulo++);
		r_ptr[i] = (int)Y;
		g_ptr[i] = (int)(Cg + addval2);
		b_ptr[i] = (int)(Co + addval2);
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/

 
	im->colour_space = A9_10;
}

/*---------------------------------------------------------------
 *	_RGBtoA7_11()  a1=1/4 a2=1/2, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void _RGBtoA7_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = b_ptr[i] + (((Ur<<1)+Vr)>>2);/* B + (R-B+G-B+G-B)/4 = (R+2G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
						if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
           (numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
								if (Ur2 != Ur) 
           (numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A7_11;
}

/*---------------------------------------------------------------
 *	RGBtoA7_11()  A7_11 implemented as C2  ####################
 *---------------------------------------------------------------*/
void RGBtoA7_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Y, Cg, Co, t, Y2, Cg2, Co2, t2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Co2 = r_ptr[i] - b_ptr[i];		/* R-B	*/
		Co = ModuloRange(Co2,-addval2,addval2 - 1u,addval);
		t2  = b_ptr[i] + (Co >> 1);	/* (B+R)/2	*/
		t  = ModuloRange(t2,0,addval-1u,addval);
		Cg2 = g_ptr[i] - t;					/* G - (R+B)/2	*/
		Cg = ModuloRange(Cg2,-addval2,addval2 - 1u,addval);
		Y2  = t + (Cg >> 1);					/* (R+B)/2 + G/2 - (R+B)/4 = (R+2G+B)/4	*/
		Y  = ModuloRange(Y2,0,addval-1u,addval);
								if (Co2 != Co || t2 != t || Cg2 != Cg || Y2 != Y ) 
           (numOfModulo++);
		r_ptr[i] = (int)Y;
		g_ptr[i] = (int)(Co + addval2);
		b_ptr[i] = (int)(Cg + addval2);
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/

	im->colour_space = A7_11;
}

/*---------------------------------------------------------------
 *	_RGBtoA8_12()  a1=1/4 a2=1/2, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void _RGBtoA8_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Vr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
		Ur2 = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Yr2 = g_ptr[i] + (((Ur<<1)+Vr)>>2);/* G + (B-G+R-G+R-G)/4 = (2R+G+B)/4	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
								if (Vr2 != Vr || Ur2 != Ur || Yr2 != Yr ) 
           (numOfModulo++);
		Ur2 = Ur - (Vr >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
								if (Ur2 != Ur) 
           (numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr + addval2;
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 
	im->colour_space = A8_12;
}


/*---------------------------------------------------------------
 *	RGBtoA8_12() implemented as C3 ####################
 *---------------------------------------------------------------*/
void RGBtoA8_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Y, Cg, Co, t, Y2, Cg2, Co2, t2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;   
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Co2 = g_ptr[i] - b_ptr[i];		/* G-B	*/
		Co = ModuloRange(Co2,-addval2,addval2 - 1u,addval);
		t2  = b_ptr[i] + (Co >> 1);  /* (B+G)/2	*/
		t  = ModuloRange(t2,0,addval-1u,addval);
		Cg2 = r_ptr[i] - t;					/* R-(B+G)/2	*/
		Cg = ModuloRange(Cg2,-addval2,addval2 - 1u,addval);
		Y2  = t + (Cg >> 1);					/* (B+G)/2 + R/2 -(B+G)/4= (2R+G+B)/4	*/
		Y  = ModuloRange(Y2,0,addval-1u,addval);
								if (Co2 != Co || t2 != t || Cg2 != Cg || Y2 != Y) 
           (numOfModulo++);
		r_ptr[i] = (int)Y;
		g_ptr[i] = (int)(Cg + addval2);
		b_ptr[i] = (int)(-Co +(addval2 - 1u)) ; /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/*im->bit_depth[1]++; /* Y keeps its range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/

	im->colour_space = A8_12;
}



//#####################


/*---------------------------------------------------------------
 *	RGBtoB1_1()
 *---------------------------------------------------------------*/
void RGBtoB1_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Vr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
								if (Vr2 != Vr)
           (numOfModulo++);
		Ur = b_ptr[i];  /* B	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Ur;
	}
	
	/* r,b keep their range */
	/*im->bit_depth[1]++; im->bit_depth_max++;*/
	//im->bit_depth[2]++; 
	im->colour_space = B1_1;
}

/*---------------------------------------------------------------
 *	RGBtoB1_2()
 *---------------------------------------------------------------*/
void RGBtoB1_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Vr2 = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
								if (Vr2 != Vr)
           (numOfModulo++);
		Ur = r_ptr[i];  /* R	*/
		r_ptr[i] = Ur;
		g_ptr[i] = Yr;
		b_ptr[i] = Vr + addval2;
	}
	
	/* r,g keep their range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
 	//im->bit_depth[1]++; 

	im->colour_space = B1_2;
}


/*---------------------------------------------------------------
 *	RGBtoB2_1()
 *---------------------------------------------------------------*/
void RGBtoB2_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i];									/* R	*/
		Vr2 = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
								if (Vr2 != Vr)
           (numOfModulo++);
		Ur = b_ptr[i];  /* B	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Ur;
	}
	
	/* r,b keep their range */
	/*im->bit_depth[1]++; im->bit_depth_max++;*/
 	//im->bit_depth[2]++; 

	im->colour_space = B2_1;
}

/*---------------------------------------------------------------
 *	RGBtoB2_3()
 *---------------------------------------------------------------*/
void RGBtoB2_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i];									/* R	*/
		Vr2 = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
								if (Vr2 != Vr)
           (numOfModulo++);
		Ur = g_ptr[i];  /* G	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur;
		b_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/* r,g keep their range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
	//im->bit_depth[1]++; 
 
	im->colour_space = B2_3;
}


/*---------------------------------------------------------------
 *	RGBtoB3_2()
 *---------------------------------------------------------------*/
void RGBtoB3_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = b_ptr[i];									/* b	*/
		Vr2 = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
								if (Vr2 != Vr) 
           (numOfModulo++);
		Ur = r_ptr[i];  /* R	*/
		r_ptr[i] = Ur;
		g_ptr[i] = -Vr + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
		b_ptr[i] = Yr;
	}
	
	/* r,b keep their range */
	/*im->bit_depth[1]++; im->bit_depth_max++;*/
	//im->bit_depth[2]++; 
 
	im->colour_space = B3_2;
}

/*---------------------------------------------------------------
 *	RGBtoB3_3()
 *---------------------------------------------------------------*/
void RGBtoB3_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Vr2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Yr = b_ptr[i];									/* b	*/
		Vr2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = ModuloRange( Vr2, -addval2, addval2 - 1u, addval);
								if (Vr2 != Vr) 
           (numOfModulo++);
		Ur = g_ptr[i];  /* G	*/
		r_ptr[i] = Ur;
		g_ptr[i] = Vr + addval2;
		b_ptr[i] = Yr;
	}
	
	/* r,b keep their range */
	/*im->bit_depth[1]++; im->bit_depth_max++;*/
	//im->bit_depth[2]++; 
 
	im->colour_space = B3_3;
}


/*---------------------------------------------------------------
 *	RGBtoB4_1()
 *---------------------------------------------------------------*/
void RGBtoB4_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Ur2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Vr = b_ptr[i];									/* B	*/
		Yr2 = g_ptr[i] + (Ur >> 1);			/* (G+R)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
								if (Yr2 != Yr || Ur2 != Ur)
           (numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr;
	}
	
	/* r,b keep their range */
	/*im->bit_depth[1]++; im->bit_depth_max++;*/
	//im->bit_depth[2]++; 
 
	im->colour_space = B4_1;
}

/*---------------------------------------------------------------
 *	RGBtoB5_2()
 *---------------------------------------------------------------*/
void RGBtoB5_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Ur2 = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Vr = r_ptr[i];									/* G	*/
		Yr2 = b_ptr[i] + (Ur >> 1);			/* (G+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
								if (Ur2 != Ur || Yr2 != Yr ) 
           (numOfModulo++);
		r_ptr[i] = Vr;
		g_ptr[i] = Yr;
		b_ptr[i] = -Ur + (addval2 - 1u); /* neuer Wertebereich nach Negierung: -127..128 -> Verschiebung um 127 in pos. Bereich*/
	}
	
	/* r,g keep their range */
	/*im->bit_depth[2]++; im->bit_depth_max++;*/
	//im->bit_depth[1]++; 
 
	im->colour_space = B5_2;
}

/*---------------------------------------------------------------
 *	RGBtoB6_3()
 *---------------------------------------------------------------*/
void RGBtoB6_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Vr, Ur, Yr2, Ur2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Ur2 = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Ur = ModuloRange( Ur2, -addval2, addval2 - 1u, addval);
		Vr = g_ptr[i];									/* G	*/
		Yr2 = b_ptr[i] + (Ur >> 1);			/* (R+B)/2	*/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
								if (Ur2 != Ur || Yr2 != Yr ) 
           (numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval2;
		b_ptr[i] = Vr;
	}
	
	/* r,b keep their range */
	/*im->bit_depth[1]++; im->bit_depth_max++;*/
 	//im->bit_depth[2]++; 

	im->colour_space = B6_3;
}

/*---------------------------------------------------------------
 *	RGBtoPEI09() nach Pei ICIP'09
 *---------------------------------------------------------------*/
void RGBtoPEI09( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Yr2, Cb, Cb2, Cr, Cr2, addval2;
	unsigned int i, addval;

	if (im->colour_channels != 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval   = 1u <<  im->bit_depth_max;    	
	addval2  = 1u << (im->bit_depth_max - 1u);   /*Hälfte von addval*/

	for (i = 0; i < im->size; i++)
	{
		Cr2 = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Cr = ModuloRange(Cr2,-addval2,addval2 - 1u,addval);
		Cb2 = b_ptr[i] - ((87*r_ptr[i] + 169*g_ptr[i]) >> 8);	
		Cb = ModuloRange(Cb2,-addval2,addval2 - 1u,addval);
		Yr2 = g_ptr[i] + ((86*Cr + 29*Cb) >> 8);			/**/
		Yr = ModuloRange(Yr2,0,addval-1u,addval);
								if (Cr2 != Cr || Cb2 != Cb || Yr2 != Yr ) 
           (numOfModulo++);
		r_ptr[i] = Yr;
		g_ptr[i] = Cb + addval2;
		b_ptr[i] = Cr + addval2;
	}
	
	/* r,b keep their range */
	/*im->bit_depth[1]++; im->bit_depth_max++;
 	im->bit_depth[2]++; */

	im->colour_space = PEI09;
}

#endif /* MODULO_RCT	*/