/*****************************************************
 * File..: iwt.h
 * Desc..: header of the abstract interface to the transformations
 * Author: Michael Stegemann
 * Date..: 30.10.2012
 ******************************************************/

#ifndef IWT_H
#define IWT_H

#define MAX_NUM_SB 4

#include "stdafx.h"
#include <string.h>
#include "tstypes.h"
#include "image.h"

typedef struct
{
    int	h_only_flag;	/* skip vertical decomposition	*/
    int	p_lx[MAX_NUM_SB + 2];
    int	p_ly[MAX_NUM_SB + 2];
    int num_sb;		/* Anzahl der Zerlegungen*/
    int	filter;		/* Wavelet-Filter		*/
} DWT_PARAM;

/*  Steuerroutine für Blocknutzung */
void Lifting_5_3 ( long* data, uint width, uint height, bool seperateable , bool forward );

void symmetric_ext_L ( long aptr[], int ll_awidth, int ll_aheight, int awidth );

/* Prototypen für 5/3 Forward */
int  f_5_3R_control ( IMAGEc*, long [], DWT_PARAM * );
void f_5_3R_1d ( long vector[], long as[], long ds[], int vlh );
void f_5_3R_1d_y ( long vector[], long as[], long ds[], int vlh, unsigned int width );
void f_5_3R_1d_u ( long vector[], long as[], long ds[], int vlh );
void f_5_3R_1d_uy ( long vector[], long as[], long ds[], int vlh, unsigned int width );
void f_5_3R_2d ( long lptr[], long vector[], IMAGEc *im, unsigned int ll_width, unsigned int ll_height,
                 DWT_PARAM *dwt );


/* Prototypen für 5/3 Inverse */
int i_5_3R_control ( IMAGEc *im, long lptr[], DWT_PARAM *dwt );
void i_5_3R_1d ( long vector[], long as[], long ds[],
                 unsigned int vec_len, int vlh );
void i_5_3R_1d_y ( long vector[], long as[], long ds[],
                   unsigned int vec_len, int vlh, unsigned int width );
void i_5_3R_1d_u ( long vector[], long as[], long ds[],
                   unsigned int vec_len, int vlh );
void i_5_3R_1d_uy ( long vector[], long as[], long ds[],
                    unsigned int vec_len, int vlh, unsigned int width );
int i_5_3R_2d ( long lptr[], long vector[], long as[], long ds[],
                IMAGEc *im, unsigned int ll_width,
                unsigned int ll_height );

/* Prototypen für 2D 5/3 Forward */

void
f_2D_53R_2d_L ( long lptr[], long aptr[],
                IMAGEc *im, unsigned int ll_width, unsigned int ll_height );
int f_2D_53R_control_L ( IMAGEc *im, DWT_PARAM *dwt, long lptr[] );

/* Prototypen für 2D 5/3 Inverse */

void
i_2D_53R_2d_L ( long lptr[], long aptr[],
                IMAGEc *im, unsigned int ll_width, unsigned int ll_height );
int i_2D_53R_control_L ( IMAGEc *im, DWT_PARAM *dwt, long lptr[] );

/*------------------------------------------------------------------*/
#endif

