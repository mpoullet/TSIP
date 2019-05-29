/*****************************************************************
 *
 * File...:	colour5_f.c
 * Purpose:	implements forward colour transforms and related
 * Author.:	Tilo Strutz
 * Date...: 09.06.2016
 * changes:
 * 09.06.2016 is intended to replace colour4_f.c unifícation w/o
 *            speed optimisation
 *****************************************************************/
#include <stdlib.h>
#include "colour5.h"
#include "tstypes.h" /* for ALLOC() and FREE()*/

#ifndef MODULO_RCT

int med3x3( int *b1, int *b2, int *b3);
int med3x3u( unsigned int *b1, unsigned int *b2, unsigned int *b3);

/*---------------------------------------------------------------
 *	RGBtoAx_y()
 *---------------------------------------------------------------*/
void RGBtoAx_y( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr, unsigned int col)
{
	int Y, U, V;
	unsigned char a1Shift, a2Shift, eShift, shiftIdx;
	unsigned char yIdx;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	yIdx = ((col - 2) / 12) ;

	/* for all Ay_uv colour spaces: get index to shiftTable	*/
	if (col < 108) shiftIdx = col2idx[col];
	else /* do something	*/

	permutation = (shiftIdx + 1) * 12
  a1shift = shiftTab[shiftIdx][0];
  a2shift = shiftTab[cshiftIdxol][1];
  eshift  = shiftTab[shiftIdx][2];


	for (i = 0; i < im->size; i++)
	{
		Y = g_ptr[i];									/* G	*/
		U = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		V = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur;
		b_ptr[i] = Vr;
	}

}
/*---------------------------------------------------------------
 *	RGBtoAx_y_off()
 * including offset for non-negativ result
 *---------------------------------------------------------------*/
void RGBtoAx_y_off( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr, unsigned int col)
{
	int Y, U, V;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	addval = (1u << im->bit_depth_max) - 1u; /* offset	*/

}

/*---------------------------------------------------------------
 *	RGBtoA1_1()
 *---------------------------------------------------------------*/
void RGBtoA1_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A1_1;
}

/*---------------------------------------------------------------
 *	RGBtoA2_2() R<=>G
 *---------------------------------------------------------------*/
void RGBtoA2_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int  Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		// Yr = r_ptr[i];									/* R	*/
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		//r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A2_2;
}

/*---------------------------------------------------------------
 *	RGBtoA3_3()   a1=a2=0 B<=>G
 *---------------------------------------------------------------*/
void RGBtoA3_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = b_ptr[i];									/* B	*/
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = -Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A3_3;
}

/*---------------------------------------------------------------
 *	RGBtoA1_4() a1=a2=0 e=1/4
 *---------------------------------------------------------------*/
void RGBtoA1_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Vr = Vr - (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++; 
	im->colour_space = A1_4;
}

/*---------------------------------------------------------------
*	RGBtoA2_5()   a1=a2=0 e=1/4 R<=>G
*---------------------------------------------------------------*/
void RGBtoA2_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i];									/* R	*/
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Vr = Vr - (Ur >> 2);						/* B-R -(G-R)/4 = B -(G+3R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++; 
	im->colour_space = A2_5;
}

/*---------------------------------------------------------------
*	RGBtoA3_6()  a1=a2=0 e=1/4 B<=>G
*---------------------------------------------------------------*/
void RGBtoA3_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = b_ptr[i];									/* B	*/
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Vr = Vr - (Ur >> 2);						/* G-B -(R-B)/4 = G -(R+3B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++; 
	im->colour_space = A3_6;
}

/*---------------------------------------------------------------
*	RGBtoA1_7()  a1=a2=0 e=1/4 B<=>R
*---------------------------------------------------------------*/
void RGBtoA1_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Vr = Vr - (Ur >> 2);						/* R-G -(B-G)/4 = R -(R+3G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
	im->colour_space = A1_7;
}

/*---------------------------------------------------------------
 *	RGBtoA3_8()  a1=a2=0 e=1/4 R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA3_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = b_ptr[i];									/* B	*/
		Ur = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Vr = Vr - (Ur >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = -Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++; 
	im->colour_space = A3_8;
}

/*---------------------------------------------------------------
 *	RGBtoA2_9() a1=a2=0 e=1/4  R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA2_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i];									/* R	*/
		Ur = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Vr = Vr - (Ur >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++; 
	im->colour_space = A2_9;
}

/*---------------------------------------------------------------
 *	RGBtoA1_10() a1=a2=0 e=1/2
 *---------------------------------------------------------------*/
void RGBtoA1_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Vr = Vr - (Ur >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
	im->colour_space = A1_10;
}


/*---------------------------------------------------------------
 *	RGBtoA3_11() a1=a2=0 e=1/2 B<=>G
 *---------------------------------------------------------------*/
void RGBtoA3_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i];									/* B	*/
		Vr = Vr - (Ur >> 1);						/* G-B -(R-B)/2 = G -(R+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
	im->colour_space = A3_11;
}


/*---------------------------------------------------------------
 *	RGBtoA1_12()  a1=a2=0 e=1/2  R<=>B
 *---------------------------------------------------------------*/
void RGBtoA1_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Vr = Vr - (Ur >> 1);						/* R-G -(B-G)/2 = R -(B+G)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
	im->colour_space = A1_12;
}

/*---------------------------------------------------------------
 *	RGBtoA2_1()  a1=1, 
 *---------------------------------------------------------------*/
void RGBtoA2_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + Ur;							/* G + R-G = R	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A2_1;
}

/*---------------------------------------------------------------
 *	RGBtoA1_2()  a1=1, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA1_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + Ur;							/* R + G-R = G	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A1_2;
}

/*---------------------------------------------------------------
 *	RGBtoA2_3()  a1=1, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA2_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + Ur;							/* B + R-B = R	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = -Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A2_3;
}

/*---------------------------------------------------------------
 *	RGBtoA2_4()  a1=1, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA2_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + Ur;							/* G + R-G = R	*/
		Vr = Vr - (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A2_4;
}

/*---------------------------------------------------------------
 *	RGBtoA1_5()  a1=1, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA1_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + Ur;							/* R + G-R = G	*/
		Vr = Vr - (Ur >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A1_5;
}

/*---------------------------------------------------------------
 *	RGBtoA2_6()  a1=1, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA2_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + Ur;							/* B + R-B = R	*/
		Vr = Vr - (Ur >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A2_6;
}

/*---------------------------------------------------------------
 *	RGBtoA3_7()  a1=1, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA3_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + Ur;							/* G + B-G = B	*/
		Vr = Vr - (Ur >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A3_7;
}

/*---------------------------------------------------------------
 *	RGBtoA1_8()  a1=1, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA1_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Yr = b_ptr[i] + Ur;							/* B + G-B = G	*/
		Vr = Vr - (Ur >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = -Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A1_8;
}
/*---------------------------------------------------------------
 *	RGBtoA3_9()  a1=1, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA3_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Yr = r_ptr[i] + Ur;							/* R + B-R = B	*/
		Vr = Vr - (Ur >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A3_9;
}

/*---------------------------------------------------------------
 *	RGBtoA2_10()  a1=1, e=1/2
 *---------------------------------------------------------------*/
void RGBtoA2_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + Ur;							/* G + R-G = R	*/
		Vr = Vr - (Ur >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A2_10;
}
/*---------------------------------------------------------------
 *	RGBtoA2_11()  a1=1, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA2_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + Ur;							/* B + R-B = R	*/
		Vr = Vr - (Ur >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A2_11;
}

/*---------------------------------------------------------------
 *	RGBtoA3_12()  a1=1, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA3_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + Ur;							/* G + B-G = B	*/
		Vr = Vr - (Ur >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A3_12;
}

//######################


/*---------------------------------------------------------------
 *	RGBtoA3_1()  a2=1, 
 *---------------------------------------------------------------*/
void RGBtoA3_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + Vr;							/* G + B-G = B	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A3_1;
}

/*---------------------------------------------------------------
 *	RGBtoA3_2()  a2=1, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA3_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + Vr;							/* R + B-R = B	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A3_2;
}

/*---------------------------------------------------------------
 *	RGBtoA1_3()  a2=1, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA1_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + Vr;							/* B + G-B = G	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = -Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A1_3;
}

/*---------------------------------------------------------------
 *	RGBtoA3_4()  a2=1, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA3_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + Vr;							/* G + B-G = B	*/
		Vr = Vr - (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A3_4;
}

/*---------------------------------------------------------------
 *	RGBtoA3_5()  a2=1, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA3_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + Vr;							/* R + B-R = B	*/
		Vr = Vr - (Ur >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A3_5;
}

/*---------------------------------------------------------------
 *	RGBtoA1_6()  a2=1, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA1_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + Vr;							/* B + G-B = G	*/
		Vr = Vr - (Ur >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A1_6;
}

/*---------------------------------------------------------------
 *	RGBtoA2_7()  a2=1, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA2_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + Vr;							/* G + R-G = R	*/
		Vr = Vr - (Ur >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A2_7;
}

/*---------------------------------------------------------------
 *	RGBtoA2_8()  a2=1, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA2_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Yr = b_ptr[i] + Vr;							/* B + R-B = R	*/
		Vr = Vr - (Ur >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = -Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A2_8;
}
/*---------------------------------------------------------------
 *	RGBtoA1_9()  a2=1, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA1_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Yr = r_ptr[i] + Vr;							/* R + G-R = G	*/
		Vr = Vr - (Ur >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A1_9;
}

/*---------------------------------------------------------------
 *	RGBtoA3_10()  a2=1, e=1/2
 *---------------------------------------------------------------*/
void RGBtoA3_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + Vr;							/* G + B-G = B	*/
		Vr = Vr - (Ur >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A3_10;
}

/*---------------------------------------------------------------
 *	RGBtoA1_11()  a2=1, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA1_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + Vr;							/* B + G-B = G	*/
		Vr = Vr - (Ur >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A1_11;
}

/*---------------------------------------------------------------
 *	RGBtoA2_12()  a2=1, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA2_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + Vr;							/* G + R-G = R	*/
		Vr = Vr - (Ur >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A2_12;
}
//##############################


/*---------------------------------------------------------------
 *	RGBtoA4_1()  a1=1/2, 
 *---------------------------------------------------------------*/
void RGBtoA4_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + (Ur>>1);				/* G + (R-G)/2 = (G+R)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A4_1;
}

/*---------------------------------------------------------------
 *	RGBtoA4_2()  a1=1/2, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA4_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + (Ur>>1);				/* R + (G-R)/2 = (G+R)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A4_2;
}

/*---------------------------------------------------------------
 *	RGBtoA6_3()  a1=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA6_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + (Ur>>1);				/* B + (R-B)/2 = (R+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = -Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A6_3;
}

/*---------------------------------------------------------------
 *	RGBtoA4_4()  a1=1/2, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA4_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + (Ur>>1);				/* G + (R-G)/2 = (R+G)/2	*/
		Vr = Vr - (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A4_4;
}

/*---------------------------------------------------------------
 *	RGBtoA4_5()  a1=1/2, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA4_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + (Ur>>1);				/* R + (G-R)/2 = (G+R)/2	*/
		Vr = Vr - (Ur >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A4_5;
}

/*---------------------------------------------------------------
 *	RGBtoA6_6()  a1=1/2, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA6_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + (Ur>>1);				/* B + (R-B) = (R+B)/2	*/
		Vr = Vr - (Ur >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A6_6;
}

/*---------------------------------------------------------------
 *	RGBtoA5_7()  a1=1/2, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA5_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + (Ur>>1);				/* G + (B-G) = (B+G)/2	*/
		Vr = Vr - (Ur >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A5_7;
}

/*---------------------------------------------------------------
 *	RGBtoA5_8()  a1=1/2, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA5_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Yr = b_ptr[i] + (Ur>>1);				/* B + (G-B)/2 = (G+B)/2	*/
		Vr = Vr - (Ur >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = -Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A5_8;
}
/*---------------------------------------------------------------
 *	RGBtoA6_9()  a1=1/2, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA6_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Yr = r_ptr[i] + (Ur>>1);			  /* R + (B-R)/2 = (B+R)/2	*/
		Vr = Vr - (Ur >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A6_9;
}

/*---------------------------------------------------------------
 *	_RGBtoA4_10()  a1=1/2, e=1/2
 *---------------------------------------------------------------*/
void _RGBtoA4_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + (Ur>>1);				/* G + (R-G)/2 = (R+G)/2	*/
		Vr = Vr - (Ur >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A4_10;
}

/*---------------------------------------------------------------
 *	RGBtoA4_10() implemented as C4
 *---------------------------------------------------------------*/
void RGBtoA4_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Y, Cg, Co, t;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Co = r_ptr[i] - g_ptr[i];	/* R-G	*/
		t  = g_ptr[i] + (Co >> 1);/* (G+R)/2	*/
		Cg = b_ptr[i] - t;				/* B - (G+R)/2	*/
		Y  = t ;									/* (G+R)/2 	*/
		r_ptr[i] = (int)Y;
		g_ptr[i] = (int)(Co + addval);
		b_ptr[i] = (int)(Cg + addval);
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;

	im->colour_space = A4_10;
}


/*---------------------------------------------------------------
 *	_RGBtoA6_11()  a1=1/2, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void _RGBtoA6_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								 unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + (Ur>>1);				/* B + (R-B)/2 = (R+B)/2	*/
		Vr = Vr - (Ur >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A6_11;
}

/*---------------------------------------------------------------
 *	RGBtoA6_11()  A6_11 implemented as C5  ####################
 *---------------------------------------------------------------*/
void RGBtoA6_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								 unsigned int *b_ptr)
{
	int Y, Cg, Co, t;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Co = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		t  = b_ptr[i] + (Co >> 1);	/* (B+R)/2	*/
		Cg = g_ptr[i] - t;					/* G - (R+B)/2	*/
		Y  = t ;										/* (R+B)/2 */
		r_ptr[i] = (int)Y;
		g_ptr[i] = (int)(Co + addval);
		b_ptr[i] = (int)(Cg + addval);
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;

	im->colour_space = A6_11;
}
/*---------------------------------------------------------------
 *	_RGBtoA5_12()  a1=1/2, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void _RGBtoA5_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + (Ur>>1);				/* G + (B-G)/2 = (B+G)/2	*/
		Vr = Vr - (Ur >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A5_12;
}

/*---------------------------------------------------------------
 *	RGBtoA5_12() implemented as C3 ####################
 *---------------------------------------------------------------*/
void RGBtoA5_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Y, Cg, Co, t;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Co = b_ptr[i] - g_ptr[i];	/* B-G	*/
		t  = g_ptr[i] + (Co >> 1);/* (G+B)/2	*/
		Cg = r_ptr[i] - t;				/* R - (G+B)/2	*/
		Y  = t ;									/* (G+B)/2 	*/
		r_ptr[i] = (int)Y;
		g_ptr[i] = (int)(Cg + addval);
		b_ptr[i] = (int)(Co + addval);
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;

	im->colour_space = A5_12;
}

//##############################


/*---------------------------------------------------------------
 *	RGBtoA5_1()  a2=1/2, 
 *---------------------------------------------------------------*/
void RGBtoA5_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + (Vr>>1);				/* G + (B-G)/2 = (G+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A5_1;
}

/*---------------------------------------------------------------
 *	RGBtoA6_2()  a2=1/2, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA6_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + (Vr>>1);				/* R + (B-R)/2 = (B+R)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A6_2;
}

/*---------------------------------------------------------------
 *	RGBtoA5_3()  a2=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA5_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + (Vr>>1);				/* B + (G-B)/2 = (G+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = -Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A5_3;
}

/*---------------------------------------------------------------
 *	RGBtoA5_4()  a2=1/2, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA5_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + (Vr>>1);				/* G + (B-G)/2 = (B+G)/2	*/
		Vr = Vr - (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A5_4;
}

/*---------------------------------------------------------------
 *	RGBtoA6_5()  a1=1/2, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA6_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + (Vr>>1);				/* R + (B-R)/2 = (B+R)/2	*/
		Vr = Vr - (Ur >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A6_5;
}

/*---------------------------------------------------------------
 *	RGBtoA5_6()  a2=1/2, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA5_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + (Vr>>1);				/* B + (G-B) = (G+B)/2	*/
		Vr = Vr - (Ur >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A5_6;
}

/*---------------------------------------------------------------
 *	RGBtoA4_7()  a2=1/2, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA4_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + (Vr>>1);				/* G + (R-G) = (R+G)/2	*/
		Vr = Vr - (Ur >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A4_7;
}

/*---------------------------------------------------------------
 *	RGBtoA6_8()  a2=1/2, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA6_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Yr = b_ptr[i] + (Vr>>1);				/* B + (R-B)/2 = (R+B)/2	*/
		Vr = Vr - (Ur >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = -Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A6_8;
}
/*---------------------------------------------------------------
 *	RGBtoA4_9()  a2=1/2, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA4_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		//Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Yr = r_ptr[i] + (Vr>>1);			  /* R + (G-R)/2 = (G+R)/2	*/
		Vr = Vr - (Ur >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		//Vr = Vr - ((-Ur) >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A4_9;
}

/*---------------------------------------------------------------
 *	RGBtoA5_10()  a2=1/2, e=1/2
 *---------------------------------------------------------------*/
void RGBtoA5_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + (Vr>>1);				/* G + (B-G)/2 = (G+B)/2	*/
		Vr = Vr - (Ur >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A5_10;
}

/*---------------------------------------------------------------
 *	RGBtoA5_11()  a2=1/2, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA5_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + (Vr>>1);				/* B + (G-B)/2 = (G+B)/2	*/
		Vr = Vr - (Ur >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A5_11;
}

/*---------------------------------------------------------------
 *	RGBtoA4_12()  a2=1/2, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA4_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + (Vr>>1);				/* G + (R-G)/2 = (R+G)/2	*/
		Vr = Vr - (Ur >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A4_12;
}

//##############################


/*---------------------------------------------------------------
 *	RGBtoA6_1()  a1=a2=1/2, 
 *---------------------------------------------------------------*/
void RGBtoA6_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + ((Ur+Vr)>>1);		/* G + (R-G+B-G)/2 = (R+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A6_1;
}

/*---------------------------------------------------------------
 *	RGBtoA5_2()  a1=a2=1/2, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA5_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + ((Ur+Vr)>>1);		/* R + (G-R+B-R)/2 = (B+G)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A5_2;
}

/*---------------------------------------------------------------
 *	RGBtoA4_3()  a1=a2=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA4_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + ((Ur+Vr)>>1);		/* B + (R-B+G-B)/2 = (R+G)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = -Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A4_3;
}

/*---------------------------------------------------------------
 *	RGBtoA6_4()  a1=a2=1/2, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA6_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + ((Ur+Vr)>>1);		/* G + (R-G+B-G)/2 = (B+R)/2	*/
		Vr = Vr - (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A6_4;
}

/*---------------------------------------------------------------
 *	RGBtoA5_5()  a1=a2=1/2, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA5_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + ((Ur+Vr)>>1);		/* R + (G-R+B-R)/2 = (B+G)/2	*/
		Vr = Vr - (Ur >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A5_5;
}

/*---------------------------------------------------------------
 *	RGBtoA4_6()  a1=a2=1/2, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA4_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + ((Ur+Vr)>>1);		/* B + (R-B+G-B) = (G+R)/2	*/
		Vr = Vr - (Ur >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A4_6;
}

/*---------------------------------------------------------------
 *	RGBtoA6_7()  a1=a2=1/2, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA6_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + ((Ur+Vr)>>1);		/* G + (B-G+R-G) = (R+B)/2	*/
		Vr = Vr - (Ur >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A6_7;
}

/*---------------------------------------------------------------
 *	RGBtoA4_8()  a1=a2=1/2, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA4_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Yr = b_ptr[i] + ((Ur+Vr)>>1);		/* B + (G-B+R-B)/2 = (R+G)/2	*/
		Vr = Vr - (Ur >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = -Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A4_8;
}
/*---------------------------------------------------------------
 *	RGBtoA5_9()  a1=a2=1/2, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA5_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Yr = r_ptr[i] + ((Ur+Vr)>>1);		/* R + (B-R+G-R)/2 = (G+B)/2	*/
		Vr = Vr - (Ur >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A5_9;
}

/*---------------------------------------------------------------
 *	RGBtoA6_10()  a1=a2=1/2, e=1/2
 *---------------------------------------------------------------*/
void RGBtoA6_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + ((Ur+Vr)>>1);		/* G + (R-G+B-G)/2 = (R+B)/2	*/
		Vr = Vr - (Ur >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A6_10;
}

/*---------------------------------------------------------------
 *	RGBtoA4_11()  a1=a2=1/2, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA4_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + ((Ur+Vr)>>1);		/* B + (R-B-G-B)/2 = (G+R)/2	*/
		Vr = Vr - (Ur >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A4_11;
}

/*---------------------------------------------------------------
 *	RGBtoA6_12()  a1=a2=1/2, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA6_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + ((Ur+Vr)>>1);		/* G + (B-G+R-G)/2 = (R+B)/2	*/
		Vr = Vr - (Ur >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A6_12;
}


//##############################


/*---------------------------------------------------------------
 *	RGBtoA7_1()  a1=a2=1/4,  == YUVr
 *---------------------------------------------------------------*/
void RGBtoA7_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + ((Ur+Vr)>>2);		/* G + (R-G+B-G)/4 = (R+2G+B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_1;
}
/*---------------------------------------------------------------
 *	RGBtoA7_1m()  Median
 *---------------------------------------------------------------*/
void RGBtoA7_1m( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int *Y = NULL, *U = NULL, *V = NULL, d[8], tmpi;
	unsigned int i, x, y, addval, Yaddval;
	unsigned long py, pos;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	ALLOC( U, im->size, int);
	ALLOC( V, im->size, int);
	ALLOC( Y, im->size, int);

	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 
	Yaddval = 1u << (im->bit_depth_max - 1u); 

	d[0] = -1;
	d[1] = -(int)im->width;
	d[2] = -1-(int)im->width;
	d[3] = 1;
	d[4] = im->width;
	d[5] = 1+im->width;
	d[6] = -1+im->width;
	d[7] = +1-(int)im->width;

	/* top and bottom row	*/
	for (y = 0, py = 0; y < im->height; y+=im->height-1, py += im->size-im->width)
	{
		for (x = 0, pos = py ; x < im->width; x++, pos++)
		{
			U[pos] = r_ptr[pos] - (int)g_ptr[pos];	/* R-G	*/
			V[pos] = b_ptr[pos] - (int)g_ptr[pos];  /* B-G	*/
			Y[pos] = g_ptr[pos] + ((U[pos]+V[pos])>>2);		/* G + (R-G+B-G)/4 = (R+2G+B)/4	*/
		}
	}
	/* left and right col	*/
	for (y = 0, py = 0; y < im->height; y++, py += im->width)
	{
		for (x = 0, pos = py ; x < im->width; x+=im->width-1, pos+=im->width-1)
		{
			U[pos] = r_ptr[pos] - (int)g_ptr[pos];	/* R-G	*/
			V[pos] = b_ptr[pos] - (int)g_ptr[pos];  /* B-G	*/
			Y[pos] = g_ptr[pos] + ((U[pos]+V[pos])>>2);		/* G + (R-G+B-G)/4 = (R+2G+B)/4	*/
		}
	}
	/* main body	*/
	for (y = 1, py = im->width; y < im->height-1; y++, py += im->width)
	{
		for (x = 1, pos = py + 1; x < im->width-1; x++, pos++)
		{
			tmpi = (int)med3x3u( &(g_ptr[pos+d[2]]), &(g_ptr[pos+d[0]]), &(g_ptr[pos+d[6]]));	
			U[pos] = r_ptr[pos] - tmpi;
			V[pos] = b_ptr[pos] - tmpi; 
		}
	}


	for (y = 1, py = im->width; y < im->height-1; y++, py += im->width)
	{
		for (x = 1, pos = py + 1; x < im->width-1; x++, pos++)
		{
			Y[pos] = g_ptr[pos] + ((
				med3x3( &(U[pos+d[2]]), &(U[pos+d[0]]), &(U[pos+d[6]]) ) +
				med3x3( &(V[pos+d[2]]), &(V[pos+d[0]]), &(V[pos+d[6]]) )
				) >> 2);		
		}
	}

	/* copy back	*/
	for (i = 0; i < im->size; i++)
	{
		r_ptr[i] = Y[i] + Yaddval;
		g_ptr[i] = U[i] + addval;
		b_ptr[i] = V[i] + addval;
	}

	FREE( Y);
	FREE( U);
	FREE( V);
	/* increase dynamic range */
	im->bit_depth[0]++; /* Y also has increased range */
	im->bit_depth[1]++;
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_1m;
}
/*---------------------------------------------------------------
 *	RGBtoA7_1s() 
 *---------------------------------------------------------------*/
void RGBtoA7_1s( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int *Y = NULL, *U = NULL, *V = NULL, d[8], tmpi;
	unsigned int i, x, y, addval, Yaddval;
	unsigned long py, pos;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	ALLOC( U, im->size, int);
	ALLOC( V, im->size, int);
	ALLOC( Y, im->size, int);

	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 
	Yaddval = 1u << (im->bit_depth_max - 1u); 

	d[0] = -1;
	d[1] = -(int)im->width;
	d[2] = -1-(int)im->width;
	d[3] = 1;
	d[4] = im->width;
	d[5] = 1+im->width;
	d[6] = -1+im->width;
	d[7] = +1-(int)im->width;

	/* top and bottom row	*/
	for (y = 0, py = 0; y < im->height; y+=im->height-1, py += im->size-im->width)
	{
		for (x = 0, pos = py ; x < im->width; x++, pos++)
		{
			U[pos] = r_ptr[pos] - (int)g_ptr[pos];	/* R-G	*/
			V[pos] = b_ptr[pos] - (int)g_ptr[pos];  /* B-G	*/
			Y[pos] = g_ptr[pos] + ((U[pos]+V[pos])>>2);		/* G + (R-G+B-G)/4 = (R+2G+B)/4	*/
		}
	}
	/* left and right col	*/
	for (y = 0, py = 0; y < im->height; y++, py += im->width)
	{
		for (x = 0, pos = py ; x < im->width; x+=im->width-1, pos+=im->width-1)
		{
			U[pos] = r_ptr[pos] - (int)g_ptr[pos];	/* R-G	*/
			V[pos] = b_ptr[pos] - (int)g_ptr[pos];  /* B-G	*/
			Y[pos] = g_ptr[pos] + ((U[pos]+V[pos])>>2);		/* G + (R-G+B-G)/4 = (R+2G+B)/4	*/
		}
	}
	/* main body	*/
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
			U[pos] = r_ptr[pos] - tmpi;
			V[pos] = b_ptr[pos] - tmpi; 
		}
	}


	for (y = 1, py = im->width; y < im->height-1; y++, py += im->width)
	{
		for (x = 1, pos = py + 1; x < im->width-1; x++, pos++)
		{
			Y[pos] = g_ptr[pos] + ((
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

	/* copy back	*/
	for (i = 0; i < im->size; i++)
	{
		r_ptr[i] = Y[i] + Yaddval;
		g_ptr[i] = U[i] + addval;
		b_ptr[i] = V[i] + addval;
	}

	FREE( Y);
	FREE( U);
	FREE( V);
	/* increase dynamic range */
	im->bit_depth[0]++; /* Y also has increased range */
	im->bit_depth[1]++;
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_1s;
}

/*---------------------------------------------------------------
 *	RGBtoA8_2()  a1=a2=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA8_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + ((Ur+Vr)>>2);		/* R + (G-R+B-R)/4 = (2R+B+G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A8_2;
}

/*---------------------------------------------------------------
 *	RGBtoA9_3()  a1=a2=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA9_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + ((Ur+Vr)>>2);		/* B + (R-B+G-B)/4 = (R+G+2B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = -Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A9_3;
}

/*---------------------------------------------------------------
 *	RGBtoA7_4()  a1=a2=1/4, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA7_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + ((Ur+Vr)>>2);		/* G + (R-G+B-G)/4 = (B+2G+R)/4	*/
		Vr = Vr - (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_4;
}

/*---------------------------------------------------------------
 *	RGBtoA8_5()  a1=a2=1/4, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA8_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + ((Ur+Vr)>>2);		/* R + (G-R+B-R)/4 = (2R+B+G)/4	*/
		Vr = Vr - (Ur >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A8_5;
}

/*---------------------------------------------------------------
 *	RGBtoA9_6()  a1=a2=1/4, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA9_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + ((Ur+Vr)>>2);		/* B + (R-B+G-B)/4 = (G+R+2B)/4	*/
		Vr = Vr - (Ur >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A9_6;
}

/*---------------------------------------------------------------
 *	RGBtoA7_7()  a1=a2=1/4, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA7_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + ((Ur+Vr)>>2);		/* G + (B-G+R-G)/4 = (B+2G+R)/4	*/
		Vr = Vr - (Ur >> 2);						/* R-G -(B-G)/4 = R -(B+3G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_7;
}

/*---------------------------------------------------------------
 *	RGBtoA9_8()  a1=a2=1/4, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA9_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Yr = b_ptr[i] + ((Ur+Vr)>>2);		/* B + (G-B+R-B)/4 = (R+G+2B)/4	*/
		Vr = Vr - (Ur >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = -Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A9_8;
}
/*---------------------------------------------------------------
 *	RGBtoA8_9()  a1=a2=1/4, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA8_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Yr = r_ptr[i] + ((Ur+Vr)>>2);		/* R + (B-R+G-R)/4 = (2R+G+B)/4	*/
		Vr = Vr - (Ur >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A8_9;
}

/*---------------------------------------------------------------
 *	RGBtoA7_10()  a1=a2=1/4, e=1/2
 *---------------------------------------------------------------*/
void RGBtoA7_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + ((Ur+Vr)>>2);		/* G + (R-G+B-G)/4 = (R+2G+B)/4	*/
		Vr = Vr - (Ur >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_10;
}

/*---------------------------------------------------------------
 *	RGBtoA9_11()  a1=a2=1/4, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA9_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + ((Ur+Vr)>>2);		/* B + (R-B-G-B)/4 = (G+R+2B)/4	*/
		Vr = Vr - (Ur >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A9_11;
}

/*---------------------------------------------------------------
 *	RGBtoA7_12()  a1=a2=1/4, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA7_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + ((Ur+Vr)>>2);		/* G + (B-G+R-G)/4 = (R+2G+B)/4	*/
		Vr = Vr - (Ur >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_12;
}



//##############################


/*---------------------------------------------------------------
 *	RGBtoA8_1()  a1=1/2 a2=1/4, 
 *---------------------------------------------------------------*/
void RGBtoA8_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + (((Ur<<1)+Vr)>>2);/* G + (R-G+R-G+B-G)/4 = (2R+G+B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A8_1;
}

/*---------------------------------------------------------------
 *	RGBtoA7_2()  a1=1/2 a2=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA7_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + (((Ur<<1)+Vr)>>2);/* R + (G-R+G-R+B-R)/4 = (R+2G+B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_2;
}

/*---------------------------------------------------------------
 *	RGBtoA8_3()  a1=1/2 a2=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA8_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + (((Ur<<1)+Vr)>>2);/* B + (R-B+R-B+G-B)/4 = (2R+G+B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = -Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A8_3;
}

/*---------------------------------------------------------------
 *	RGBtoA8_4()  a1=1/2 a2=1/4, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA8_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + (((Ur<<1)+Vr)>>2);/* G + (R-G+R-G+B-G)/4 = (2R+G+B)/4	*/
		Vr = Vr - (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A8_4;
}

/*---------------------------------------------------------------
 *	RGBtoA7_5() a1=1/2 a2=1/4, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA7_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + (((Ur<<1)+Vr)>>2);/* R + (G-R+G-R+B-R)/4 = (R+2G+B)/4	*/
		Vr = Vr - (Ur >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_5;
}

/*---------------------------------------------------------------
 *	RGBtoA8_6()  a1=1/2 a2=1/4, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA8_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + (((Ur<<1)+Vr)>>2);/* B + (R-B+R-B+G-B)/4 = (2R+G+B)/4	*/
		Vr = Vr - (Ur >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A8_6;
}

/*---------------------------------------------------------------
 *	RGBtoA9_7()  a1=1/2 a2=1/4, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA9_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + (((Ur<<1)+Vr)>>2);/* G + (B-G+B-G+R-G)/4 = (R+G+2B)/4	*/
		Vr = Vr - (Ur >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A9_7;
}

/*---------------------------------------------------------------
 *	RGBtoA7_8()  a1=1/2 a2=1/4, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA7_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Yr = b_ptr[i] + (((Ur<<1)+Vr)>>2);/* B + (G-B+G-B+R-B)/4 = (R+2G+B)/4	*/
		Vr = Vr - (Ur >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = -Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_8;
}
/*---------------------------------------------------------------
 *	RGBtoA9_9()  a1=1/2 a2=1/4, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA9_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Yr = r_ptr[i] + (((Ur<<1)+Vr)>>2);/* R + (B-R+B-R+G-R)/4 = (R+G+2B)/4	*/
		Vr = Vr - (Ur >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A9_9;
}

/*---------------------------------------------------------------
 *	RGBtoA8_10()  a1=1/2 a2=1/4, e=1/2
 *---------------------------------------------------------------*/
void RGBtoA8_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + (((Ur<<1)+Vr)>>2);/* G + (R-G+R-G+B-G)/4 = (2R+G+B)/4	*/
		Vr = Vr - (Ur >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A8_10;
}

/*---------------------------------------------------------------
 *	RGBtoA8_11()  a1=1/2 a2=1/4, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA8_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + (((Ur<<1)+Vr)>>2);/* B + (R-B+R-B+G-B)/4 = (2R+G+B)/4	*/
		Vr = Vr - (Ur >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A8_11;
}

/*---------------------------------------------------------------
 *	RGBtoA9_12()  a1=1/2 a2=1/4, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA9_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + (((Ur<<1)+Vr)>>2);/* G + (B-G+B-G+R-G)/4 = (R+G+2B)/4	*/
		Vr = Vr - (Ur >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A9_12;
}



//##############################


/*---------------------------------------------------------------
 *	RGBtoA9_1()  a1=1/4 a2=1/2, 
 *---------------------------------------------------------------*/
void RGBtoA9_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + (((Vr<<1)+Ur)>>2);/* G + (R-G+B-G+B-G)/4 = (R+G+2B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A9_1;
}

/*---------------------------------------------------------------
 *	RGBtoA9_2()  a1=1/4 a2=1/2, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA9_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + (((Vr<<1)+Ur)>>2);/* R + (G-R+B-R+B-R)/4 = (R+G+2B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A9_2;
}

/*---------------------------------------------------------------
 *	RGBtoA7_3()  a1=1/4 a2=1/2, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA7_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + (((Vr<<1)+Ur)>>2);/* B + (R-B+G-B+G-B)/4 = (R+2G+B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = -Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_3;
}

/*---------------------------------------------------------------
 *	RGBtoA9_4()  a1=1/4 a2=1/2, e=1/4
 *---------------------------------------------------------------*/
void RGBtoA9_4( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + (((Vr<<1)+Ur)>>2);/* G + (R-G+B-G+B-G)/4 = (R+G+2B)/4	*/
		Vr = Vr - (Ur >> 2);						/* B-G -(R-G)/4 = B -(R+3G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A9_4;
}

/*---------------------------------------------------------------
 *	RGBtoA9_5() a1=1/4 a2=1/2, e=1/4, R<=>G
 *---------------------------------------------------------------*/
void RGBtoA9_5( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		Vr = b_ptr[i] - (int)r_ptr[i];  /* B-R	*/
		Yr = r_ptr[i] + (((Vr<<1)+Ur)>>2);/* R + (G-R+B-R+B-R)/4 = (R+G+2B)/4	*/
		Vr = Vr - (Ur >> 2);						/* B-R -(G-R)/4 = B -(3R+G)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A9_5;
}

/*---------------------------------------------------------------
 *	RGBtoA7_6()  a1=1/4 a2=1/2, e=1/4, B<=>G
 *---------------------------------------------------------------*/
void RGBtoA7_6( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + (((Vr<<1)+Ur)>>2);/* B + (R-B+G-B+G-B)/4 = (R+2G+B)/4	*/
		Vr = Vr - (Ur >> 2);						/* G-B -(R-B)/4 = G -(3B+R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_6;
}

/*---------------------------------------------------------------
 *	RGBtoA8_7()  a1=1/4 a2=1/2, e=1/4, B<=>R
 *---------------------------------------------------------------*/
void RGBtoA8_7( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + (((Vr<<1)+Ur)>>2);/* G + (B-G+R-G+R-G)/4 = (2R+G+B)/4	*/
		Vr = Vr - (Ur >> 2);						/* R-G -(B-G)/4 = R -(3G+B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A8_7;
}

/*---------------------------------------------------------------
 *	RGBtoA8_8()  a1=1/4 a2=1/2, e=1/4, R=>G=>B=>R
 *---------------------------------------------------------------*/
void RGBtoA8_8( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Vr = r_ptr[i] - (int)b_ptr[i];  /* R-B	*/
		Yr = b_ptr[i] + (((Vr<<1)+Ur)>>2);/* B + (G-B+R-B+R-B)/4 = (2R+G+B)/4	*/
		Vr = Vr - (Ur >> 2);						/* R-B -(G-B)/4 = R -(G+3B)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = -Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A8_8;
}

/*---------------------------------------------------------------
 *	RGBtoA7_9()  a1=1/4 a2=1/2, e=1/4, R=>B=>G=>R
 *---------------------------------------------------------------*/
void RGBtoA7_9( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
							 unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		Vr = g_ptr[i] - (int)r_ptr[i];  /* G-R	*/
		Yr = r_ptr[i] + (((Vr<<1)+Ur)>>2);/* R + (B-R+G-R+G-R)/4 = (R+2G+B)/4	*/
		Vr = Vr - (Ur >> 2);						/* G-R -(B-R)/4 = G -(B+3R)/4	*/
		r_ptr[i] = Yr;
		g_ptr[i] = -Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_9;
}

/*---------------------------------------------------------------
 *	_RGBtoA9_10()  a1=1/4 a2=1/2, e=1/2
 *---------------------------------------------------------------*/
void _RGBtoA9_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Vr = b_ptr[i] - (int)g_ptr[i];  /* B-G	*/
		Yr = g_ptr[i] + (((Vr<<1)+Ur)>>2);/* G + (R-G+B-G+B-G)/4 = (R+G+2B)/4	*/
		Vr = Vr - (Ur >> 1);						/* B-G -(R-G)/2 = B -(R+G)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A9_10;
}

/*---------------------------------------------------------------
 *	RGBtoA9_10() implemented as C1 ##########
 *---------------------------------------------------------------*/
void RGBtoA9_10( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Y, Cg, Co, t;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Co = r_ptr[i] - g_ptr[i];  /* R-G	*/
		t  = g_ptr[i] + (Co >> 1);  /* (G+R)/2	*/
		Cg = b_ptr[i] - t;					/* B - (G+R)/2	*/
		Y  = t + (Cg >> 1);					/* (G+R)/2 + B/2 - (G+R)/4 = ((R+G+2B)/4	*/
		r_ptr[i] = (int)Y;
		g_ptr[i] = (int)(Cg + addval);
		b_ptr[i] = (int)(Co + addval);
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;

 
	im->colour_space = A9_10;
}

/*---------------------------------------------------------------
 *	_RGBtoA7_11()  a1=1/4 a2=1/2, e=1/2, B<=>G
 *---------------------------------------------------------------*/
void _RGBtoA7_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Vr = g_ptr[i] - (int)b_ptr[i];  /* G-B	*/
		Yr = b_ptr[i] + (((Vr<<1)+Ur)>>2);/* B + (R-B+G-B+G-B)/4 = (R+2G+B)/4	*/
		Vr = Vr - (Ur >> 1);						/* G-B -(R-B)/2 = G -(B+R)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur + addval;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A7_11;
}

/*---------------------------------------------------------------
 *	RGBtoA7_11()  A7_11 implemented as C2  ####################
 *---------------------------------------------------------------*/
void RGBtoA7_11( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								 unsigned int *b_ptr)
{
	int Y, Cg, Co, t;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Co = r_ptr[i] - b_ptr[i];		/* R-B	*/
		t  = b_ptr[i] + (Co >> 1);	/* (B+R)/2	*/
		Cg = g_ptr[i] - t;					/* G - (R+B)/2	*/
		Y  = t + (Cg >> 1);					/* (R+B)/2 + G/2 - (R+B)/4 = (R+2G+B)/4	*/
		r_ptr[i] = (int)Y;
		g_ptr[i] = (int)(Co + addval);
		b_ptr[i] = (int)(Cg + addval);
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;

	im->colour_space = A7_11;
}

/*---------------------------------------------------------------
 *	_RGBtoA8_12()  a1=1/4 a2=1/2, e=1/2, B<=>R
 *---------------------------------------------------------------*/
void _RGBtoA8_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i] - (int)g_ptr[i];  /* R-G	*/
		Yr = g_ptr[i] + (((Vr<<1)+Ur)>>2);/* G + (B-G+R-G+R-G)/4 = (2R+G+B)/4	*/
		Vr = Vr - (Ur >> 1);						/* R-G -(B-G)/2 = R -(G+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Vr + addval;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;
 
	im->colour_space = A8_12;
}


/*---------------------------------------------------------------
 *	RGBtoA8_12() implemented as C3 ####################
 *---------------------------------------------------------------*/
void RGBtoA8_12( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, 
								unsigned int *b_ptr)
{
	int Y, Cg, Co, t;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Co = g_ptr[i] - b_ptr[i];		/* G-B	*/
		t  = b_ptr[i] + (Co >> 1);  /* (B+G)/2	*/
		Cg = r_ptr[i] - t;					/* R-(B+G)/2	*/
		Y  = t + (Cg >> 1);					/* (B+G)/2 + R/2 -(B+G)/4= (2R+G+B)/4	*/
		r_ptr[i] = (int)Y;
		g_ptr[i] = (int)(Cg + addval);
		b_ptr[i] = (int)(-Co + addval);
	}
	/* increase dynamic range */
	im->bit_depth[1]++; /* Y keeps its range */
	im->bit_depth[2]++; im->bit_depth_max++;

	im->colour_space = A8_12;
}



//#####################


/*---------------------------------------------------------------
 *	RGBtoB1_1()
 *---------------------------------------------------------------*/
void RGBtoB1_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Ur = b_ptr[i];  /* B	*/
		Vr = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	/* r,b keep their range */
	im->bit_depth[2]++; im->bit_depth_max++;
	//im->bit_depth[1]++; 

	im->colour_space = B1_1;
}

/*---------------------------------------------------------------
 *	RGBtoB1_2()
 *---------------------------------------------------------------*/
void RGBtoB1_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = g_ptr[i];									/* G	*/
		Ur = b_ptr[i] - (int)g_ptr[i];	/* B-G	*/
		Vr = r_ptr[i];  /* R	*/
		r_ptr[i] = Vr;
		g_ptr[i] = Yr;
		b_ptr[i] = Ur + addval;
	}
	/* increase dynamic range */
	/* r,g keep their range */
	im->bit_depth[2]++; im->bit_depth_max++;
	//im->bit_depth[1]++; 

	im->colour_space = B1_2;
}


/*---------------------------------------------------------------
 *	RGBtoB2_1()
 *---------------------------------------------------------------*/
void RGBtoB2_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i];									/* R	*/
		Ur = b_ptr[i];  /* B	*/
		Vr = g_ptr[i] - (int)r_ptr[i];	/* G-R	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur;
		b_ptr[i] = -Vr + addval;
	}
	/* increase dynamic range */
	/* r,b keep their range */
	im->bit_depth[2]++; im->bit_depth_max++;
	//im->bit_depth[1]++; 

	im->colour_space = B2_1;
}

/*---------------------------------------------------------------
 *	RGBtoB2_3()
 *---------------------------------------------------------------*/
void RGBtoB2_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = r_ptr[i];									/* R	*/
		Ur = g_ptr[i];  /* G	*/
		Vr = b_ptr[i] - (int)r_ptr[i];	/* B-R	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur;
		b_ptr[i] = -Vr + addval;
	}
	/* increase dynamic range */
	/* r,g keep their range */
	im->bit_depth[2]++; im->bit_depth_max++;
	//im->bit_depth[1]++; 
 
	im->colour_space = B2_3;
}


/*---------------------------------------------------------------
 *	RGBtoB3_2()
 *---------------------------------------------------------------*/
void RGBtoB3_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = b_ptr[i];									/* b	*/
		Ur = r_ptr[i];  /* R	*/
		Vr = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur;
		b_ptr[i] = -Vr + addval;
	}
	/* increase dynamic range */
	/* r,b keep their range */
	im->bit_depth[2]++; im->bit_depth_max++;
	//im->bit_depth[1]++; 
 
	im->colour_space = B3_2;
}

/*---------------------------------------------------------------
 *	RGBtoB3_3()
 *---------------------------------------------------------------*/
void RGBtoB3_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Yr = b_ptr[i];									/* b	*/
		Ur = g_ptr[i];  /* G	*/
		Vr = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	/* r,b keep their range */
	im->bit_depth[2]++; im->bit_depth_max++;
	//im->bit_depth[1]++; 
 
	im->colour_space = B3_3;
}


/*---------------------------------------------------------------
 *	RGBtoB4_1()
 *---------------------------------------------------------------*/
void RGBtoB4_1( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Vr = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Ur = b_ptr[i];									/* B	*/
		Yr = g_ptr[i] + (Vr >> 1);			/* (G+R)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	/* r,b keep their range */
	im->bit_depth[2]++; im->bit_depth_max++;
	//im->bit_depth[1]++; 
 
	im->colour_space = B4_1;
}

/*---------------------------------------------------------------
 *	RGBtoB5_2()
 *---------------------------------------------------------------*/
void RGBtoB5_2( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Vr = g_ptr[i] - (int)b_ptr[i];	/* G-B	*/
		Ur = r_ptr[i];									/* G	*/
		Yr = b_ptr[i] + (Vr >> 1);			/* (G+B)/2	*/
		r_ptr[i] = Ur;
		g_ptr[i] = Yr;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	/* r,g keep their range */
	im->bit_depth[2]++; im->bit_depth_max++;
	//im->bit_depth[1]++; 
 
	im->colour_space = B5_2;
}

/*---------------------------------------------------------------
 *	RGBtoB6_3()
 *---------------------------------------------------------------*/
void RGBtoB6_3( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Ur, Vr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Vr = r_ptr[i] - (int)b_ptr[i];	/* R-B	*/
		Ur = g_ptr[i];									/* G	*/
		Yr = b_ptr[i] + (Vr >> 1);			/* (R+B)/2	*/
		r_ptr[i] = Yr;
		g_ptr[i] = Ur;
		b_ptr[i] = Vr + addval;
	}
	/* increase dynamic range */
	/* r,b keep their range */
	im->bit_depth[2]++; im->bit_depth_max++;
	// im->bit_depth[1]++; 

	im->colour_space = B6_3;
}

/*---------------------------------------------------------------
 *	RGBtoPEI09() nach Pei ICIP'09
 *---------------------------------------------------------------*/
void RGBtoPEI09( IMAGEc *im, unsigned int *r_ptr, unsigned int *g_ptr, unsigned int *b_ptr)
{
	int Yr, Cb, Cr;
	unsigned int i, addval;

	if (im->colour_channels < 3u || im->colour_space != RGBc) return;
	
	/* Verschiebung in positiven Bereich */
	addval = (1u << im->bit_depth_max) - 1u; 

	for (i = 0; i < im->size; i++)
	{
		Cr = r_ptr[i] - (int)g_ptr[i];	/* R-G	*/
		Cb = b_ptr[i] - ((87*r_ptr[i] + 169*g_ptr[i]) >> 8);	
		Yr = g_ptr[i] + ((86*Cr + 29*Cb) >> 8);			/**/
		r_ptr[i] = Yr;
		g_ptr[i] = Cb + addval;
		b_ptr[i] = Cr + addval;
	}
	/* increase dynamic range */
	/* r,b keep their range */
	im->bit_depth[1]++; im->bit_depth_max++;
 	im->bit_depth[2]++; 

	im->colour_space = PEI09;
}

#endif /* no MODULO_RCT */