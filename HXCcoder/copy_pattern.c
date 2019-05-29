/*****************************************************
* File...: copy_pattern.c
* Desc...: implementation for H(X|C) coder
* Author.: T. Strutz
* Date...: 02.09.2013
* changes: 20.08.2014 new copy_pattern2() RGB-A
******************************************************/
#include <stdlib.h>
#include <assert.h>
#include "hxc.h"
#include "ginterface.h"

/*--------------------------------------------------------
 * copy_pattern()
 *     F
 *   C B D
 * E A X
 *--------------------------------------------------------*/
void copy_pattern( PicData *pd, unsigned int r, unsigned int c, 
									long **templ, unsigned int comp)
{
	if (c > 0)
	{
		templ[0][0] = pd->data[comp][c-1 + r * pd->width];	/* A */
		if (r > 0)
		{
			templ[0][2] = pd->data[comp][c-1 + (r-1) * pd->width];	/* C */
		}
		else
		{
			templ[0][2] = templ[0][0];	/* C = A */
		}
		if (c > 1)
		{
			templ[0][4] = pd->data[comp][c-2 + r * pd->width];	/* E */
		}
		else
		{
			templ[0][4] = templ[0][2];	/* E = C */
		}
	}
	else
	{
		if (r > 0)
		{
			templ[0][0] = pd->data[comp][c + (r-1) * pd->width];	/* A = B */
			templ[0][2] = templ[0][0];	/* C = A = B */
			templ[0][4] = templ[0][0];	/* E = A = B */
		}
		else
		{
			templ[0][0] = 0;	/* A = 0 */
			templ[0][2] = 0;	/* C = 0 */
			templ[0][4] = 0;	/* E = 0 */
		}
	}


	if (r > 0)
	{
		templ[0][1] = pd->data[comp][c + (r-1) * pd->width];	/* B */
		if (c < pd->width-1)
		{
			templ[0][3] = pd->data[comp][c+1 + (r-1) * pd->width];	/* D */
		}
		else
		{
			templ[0][3] = templ[0][1]; /* D = B */
		}
		if (r > 1)
		{
			templ[0][5] = pd->data[comp][c + (r-2) * pd->width];	/* F */
		}
		else
		{
			templ[0][5] = pd->data[comp][c + (r-1) * pd->width];	/* F = B */
		}
	}
	else
	{
		if (c > 0)
		{
			templ[0][1] = templ[0][0];	/* B = A */
			templ[0][3] = templ[0][0];  /* D = A */
			templ[0][5] = templ[0][0];  /* F = A */
		}
		else
		{
			templ[0][1] = 0; /* B = 0 */
			templ[0][3] = 0; /* D = 0 */
			templ[0][5] = 0; /* F = 0 */
		}
	}
}

/*--------------------------------------------------------
 * copy_pattern2()   RGB-A
 *     F
 *   C B D
 * E A X
 * fixed order of A, B, ... F !!
 *--------------------------------------------------------*/
void copy_pattern2( PicData *pd, unsigned int r, unsigned int c, 
									long **templ)
{
	
	unsigned int comp;

	for( comp = 0; comp < pd->channel; comp++)
	{
		if (c > 0)
		{
			templ[0][comp] = pd->data[comp][c-1 + r * pd->width];	/* A */
			if (r > 0)
			{
				templ[2][comp] = pd->data[comp][c-1 + (r-1) * pd->width];	/* C */
			}
			else
			{
				templ[2][comp] = templ[0][comp];	/* C = A */
			}
			if (c > 1)
			{
				templ[4][comp] = pd->data[comp][c-2 + r * pd->width];	/* E */
			}
			else
			{
				templ[4][comp] = templ[2][comp];	/* E = C */
			}
		}
		else
		{
			if (r > 0)
			{
				templ[0][comp] = pd->data[comp][c + (r-1) * pd->width];	/* A = B */
				templ[2][comp] = templ[0][comp];	/* C = A = B */
				templ[4][comp] = templ[0][comp];	/* E = A = B */
			}
			else
			{
				templ[0][comp] = 0;	/* A = 0 */
				templ[2][comp] = 0;	/* C = 0 */
				templ[4][comp] = 0;	/* E = 0 */
			}
		}


		if (r > 0)
		{
			templ[1][comp] = pd->data[comp][c + (r-1) * pd->width];	/* B */
			if (c < pd->width-1)
			{
				templ[3][comp] = pd->data[comp][c+1 + (r-1) * pd->width];	/* D */
			}
			else
			{
				templ[3][comp] = templ[1][comp]; /* D = B */
			}
			if (r > 1)
			{
				templ[5][comp] = pd->data[comp][c + (r-2) * pd->width];	/* F */
			}
			else
			{
				templ[5][comp] = templ[1][comp];	/* F = B */
			}
		}
		else
		{
			if (c > 0)
			{
				templ[1][comp] = templ[0][comp];	/* B = A */
				templ[3][comp] = templ[0][comp];  /* D = A */
				templ[5][comp] = templ[0][comp];  /* F = A */
			}
			else
			{
				templ[1][comp] = 0; /* B = 0 */
				templ[3][comp] = 0; /* D = 0 */
				templ[5][comp] = 0; /* F = 0 */
			}
		}
	} /* for comp	*/
}

/*--------------------------------------------------------
 * copy_patternIdx()
 *     F
 *   C B D
 * E A X
 *--------------------------------------------------------*/
void copy_patternIdx( PicData *pd, unsigned int r, unsigned int c, 
									long *templ, unsigned int comp)
{
	if (c > 0)
	{
		templ[0] = pd->data[comp][c-1 + r * pd->width];	/* A */
		if (r > 0)
		{
			templ[2] = pd->data[comp][c-1 + (r-1) * pd->width];	/* C */
		}
		else
		{
			templ[2] = templ[0];	/* C = A */
		}
		if (c > 1)
		{
			templ[4] = pd->data[comp][c-2 + r * pd->width];	/* E */
		}
		else
		{
			templ[4] = templ[2];	/* E = C */
		}
	}
	else
	{
		if (r > 0)
		{
			templ[0] = pd->data[comp][c + (r-1) * pd->width];	/* A = B */
			templ[2] = templ[0];	/* C = A = B */
			templ[4] = templ[0];	/* E = A = B */
		}
		else
		{
			templ[0] = 0;	/* A = 0 */
			templ[2] = 0;	/* C = 0 */
			templ[4] = 0;	/* E = 0 */
		}
	}


	if (r > 0)
	{
		templ[1] = pd->data[comp][c + (r-1) * pd->width];	/* B */
		if (c < pd->width-1)
		{
			templ[3] = pd->data[comp][c+1 + (r-1) * pd->width];	/* D */
		}
		else
		{
			templ[3] = templ[1]; /* D = B */
		}
		if (r > 1)
		{
			templ[5] = pd->data[comp][c + (r-2) * pd->width];	/* F */
		}
		else
		{
			templ[5] = pd->data[comp][c + (r-1) * pd->width];	/* F = B */
		}
	}
	else
	{
		if (c > 0)
		{
			templ[1] = templ[0];	/* B = A */
			templ[3] = templ[0];  /* D = A */
			templ[5] = templ[0];  /* F = A */
		}
		else
		{
			templ[1] = 0; /* B = 0 */
			templ[3] = 0; /* D = 0 */
			templ[5] = 0; /* F = 0 */
		}
	}
}

