/*****************************************************
 * File..: 5_3_Lifting.c
 * Desc..: implements a abstract interface to the transformations
 * Author: Michael Stegemann
 * Date..: 30.10.2012
 ******************************************************/

#include "iwt.h"
#include "H26x_Prediction.h"


/*------------------------------------------------------------------
 * Lifting_5_3 (long* data, uint width, uint height, bool seperateable, bool forward)
 *
 * Allows external functions to use the transformations much easier
 *-----------------------------------------------------------------*/
void Lifting_5_3 ( long* data, uint width, uint height, bool seperateable, bool forward )
{
    int err = 0;

    /* Build temporary objects for the passed blocks */

    IMAGEc im;

    im.height = height;
    im.width = width;

    DWT_PARAM dwt;

    dwt.num_sb = log2 ( width );


    dwt.h_only_flag = 0;
    dwt.p_lx[0] = im.width;
    dwt.p_ly[0] = im.height;
    for ( int i = 1; i < dwt.num_sb + 1; i++ )
    {
        dwt.p_lx[i] = ( dwt.p_lx[i - 1] + 1 ) >> 1;
        dwt.p_ly[i] = ( dwt.p_ly[i - 1] + 0 ) >> 1;
    }

    if ( seperateable == true )
    {
        if ( forward == true )
        {
            err = f_5_3R_control ( &im, data, &dwt );
        }
        else
        {
            err = i_5_3R_control ( &im, data, &dwt );
        }
    }
    else
    {
        if ( forward == true )
        {
            err = f_2D_53R_control_L ( &im, &dwt, data );
        }
        else
        {
            err = i_2D_53R_control_L ( &im, &dwt, data );
        }
    }
}

