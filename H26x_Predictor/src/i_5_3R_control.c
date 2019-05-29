/*****************************************************************
 *
 * Datei:	i_5_3R_control.c
 * Funktion:	inverse Festkomma-5/3-Wavelet-Transformation 2D
 * Autor:	Tilo Strutz
 * Datum: 	13.04.2000
 *
 *****************************************************************/

#include "iwt.h"

/*--------------------------------------------------------------*
 * i_5_3R_1d()
 *--------------------------------------------------------------*/
void i_5_3R_1d ( long vector[], long as[], long ds[],
                 unsigned int vec_len, int vlh )
{
    register int		i;
    register long	*lp, *lpp1, *lpp2, *lambda, *gamma;

    lp   = &vector[vec_len - 2];
    lpp1 = lp + 1;

    lambda = &as[vlh - 1];	/* low- pass coefficients */
    gamma  = &ds[vlh - 1];	/* high-pass coefficients */

    *lp   = *lambda - ( ( * ( gamma - 1 ) + *gamma + 2 ) >> 2 );
    *lpp1 = *gamma  +   *lp;

    lpp2  = lp;
    lp   -= 2;
    lpp1 -= 2;

    gamma--;
    lambda--;

    for ( i = ( vlh - 2 ); i > 0; i--, lp -= 2, lpp1 -= 2, lpp2 -= 2,
            gamma--, lambda-- )
    {
        *lp   = *lambda - ( ( * ( gamma - 1 ) + *gamma + 2 ) >> 2 );
        *lpp1 = *gamma  + ( ( *lp	  + *lpp2     ) >> 1 );
    }

    *lp   = *lambda - ( ( *gamma + 1 ) >> 1 );
    *lpp1 = *gamma  + ( ( *lp + *lpp2 ) >> 1 );
}

/*--------------------------------------------------------------*
 * i_5_3R_1d_y()
 *--------------------------------------------------------------*/
void i_5_3R_1d_y ( long vector[], long as[], long ds[],
                   unsigned int vec_len, int vlh, unsigned int width )
{
    register int		i;
    register long	*lp, *lpp1, *lpp2, *lambda, *gamma;

    lp   = &vector[vec_len - 2];
    lpp1 = lp + 1;

    lambda = &as[ ( vlh - 1 ) * width];	/* low- pass coefficients */
    gamma  = &ds[ ( vlh - 1 ) * width];	/* high-pass coefficients */

    *lp   = *lambda - ( ( * ( gamma - width ) + *gamma + 2 ) >> 2 );
    *lpp1 = *gamma  + *lp;

    lpp2  = lp;
    lp   -= 2;
    lpp1 -= 2;

    gamma  -= width;
    lambda -= width;

    for ( i = ( vlh - 2 ); i > 0; i--, lp -= 2, lpp1 -= 2, lpp2 -= 2,
            gamma  -= width, lambda -= width )
    {
        *lp   = *lambda - ( ( * ( gamma - width ) + *gamma + 2 ) >> 2 );
        *lpp1 = *gamma  + ( ( *lp	      + *lpp2     ) >> 1 );
    }

    *lp   = *lambda - ( ( *gamma + 1 ) >> 1 );
    *lpp1 = *gamma  + ( ( *lp + *lpp2 ) >> 1 );
}


/*--------------------------------------------------------------*
 * i_5_3R_1d_u()
 *--------------------------------------------------------------*/
void i_5_3R_1d_u ( long vector[], long as[], long ds[],
                   unsigned int vec_len, int vlh )
{
    register int	i;
    register long	*lp, *lpp1, *lpp2, *lambda, *gamma;

    lp   = &vector[vec_len - 1];
    lpp1 = lp + 1;

    lambda = &as[vlh - 1];	/* even samples */
    gamma  = &ds[vlh - 1];	/* wavelet subset, odd samples */

    *lp   = *lambda - ( ( * ( gamma - 1 ) + 1 ) >> 1 );

    lpp2  = lp;
    lp   -= 2;
    lpp1 -= 2;

    gamma --;
    lambda--;

    for ( i = ( vlh - 2 ); i > 0;
            i--, lp -= 2, lpp1 -= 2, lpp2 -= 2, gamma--, lambda-- )
    {
        *lp   = *lambda - ( ( * ( gamma - 1 ) + *gamma + 2 ) >> 2 );
        *lpp1 = *gamma  + ( ( *  lp          + *lpp2     ) >> 1 );
    }

    *lp   = *lambda - ( ( *gamma + 1 ) >> 1 );
    *lpp1 = *gamma  + ( ( *lp + *lpp2 ) >> 1 );
}


/*--------------------------------------------------------------*
 * i_5_3R_1d_uy()
 *--------------------------------------------------------------*/
void i_5_3R_1d_uy ( long vector[], long as[], long ds[],
                    unsigned int vec_len, int vlh, unsigned int width )
{
    register int		i;
    register long	*lp, *lpp1, *lpp2, *lambda, *gamma;

    lp   = &vector[vec_len - 1];
    lpp1 = lp + 1;

    lambda = &as[ ( vlh - 1 ) * width];	/* low- pass coefficients */
    gamma  = &ds[ ( vlh - 1 ) * width];	/* high-pass coefficients */

    *lp   = *lambda - ( ( * ( gamma - width ) + 1 ) >> 1 );

    lpp2  = lp;
    lp   -= 2;
    lpp1 -= 2;

    gamma -= width;
    lambda -= width;

    for ( i = ( vlh - 2 ); i > 0; i--, lp -= 2, lpp1 -= 2, lpp2 -= 2,
            gamma -= width, lambda -= width )
    {
        *lp   = *lambda - ( ( * ( gamma - width ) + *gamma + 2 ) >> 2 );
        *lpp1 = *gamma  + ( ( *lp          + *lpp2     ) >> 1 );
    }

    *lp   = *lambda - ( ( *gamma + 1 ) >> 1 );
    *lpp1 = *gamma  + ( ( *lp + *lpp2 ) >> 1 );
}

/*--------------------------------------------------------------*
 * i_5_3R_2d()	IDWT eines 2D-Signals
 *--------------------------------------------------------------*/
int i_5_3R_2d ( long lptr[], long vector[], long as[], long ds[],
                IMAGEc *im, unsigned int ll_width,
                unsigned int ll_height )
{
    register unsigned int	x, y;
    int 		xg_flag, yg_flag;
    int		xvlh, yvlh;
    long		*lp, *lpx, *lpv;
    unsigned int 	width;
    unsigned long yvlh_width, ysizef_ll, xsizef_ll, xsizef_vlh;

    width = im->width;

    ysizef_ll = sizeof ( long ) * ll_height;
    xsizef_ll = sizeof ( long ) * ll_width;

    if ( ll_height & 0x01 )
    {
        yvlh = ( ll_height + 1 ) >> 1;
        yg_flag = 0;			/* ungerade	*/
    }
    else
    {
        yvlh = ll_height >> 1;
        yg_flag = 1;			/* gerade	*/
    }
    yvlh_width = yvlh * im->width;

    if ( ll_width & 0x01 )
    {
        xvlh = ( ll_width + 1 ) >> 1;
        xg_flag = 0;			/* ungerade	*/
    }
    else
    {
        xvlh = ll_width >> 1;
        xg_flag = 1;			/* gerade	*/
    }
    xsizef_vlh  = sizeof ( long ) * xvlh;

    /*
     * alle Spalten
     */
    if ( ll_height > 2 )
    {
        lpx = lptr;
        if ( yg_flag )
        {
            for ( x = ll_width; x > 0; x--, lpx++ )
            {
                lp = lpx;
                lpv = &lpx[yvlh_width];

                i_5_3R_1d_y ( vector, lp, lpv, ll_height, yvlh, width );

                lp = lpx;
                for ( y = 0; y < ll_height; y++, lp += width )
                    * lp = vector[y];
            }
        }
        else
        {
            for ( x = ll_width; x > 0; x--, lpx++ )
            {
                lp = lpx;
                lpv = &lpx[yvlh_width];

                i_5_3R_1d_uy ( vector, lp, lpv, ll_height, yvlh, width );

                lp = lpx;
                for ( y = 0; y < ll_height; y++, lp += width )
                    * lp = vector[y];
            }
        }
    }
    else if ( ll_height == 2 )
    {
        lp = lptr;

        for ( y = ll_height; y > 0; y--, lp ++ )
        {
            vector[0] = lp[0];	//equals a0
            vector[1] = lp[width]; // equals d0

            lp[0] = vector[0] - ( ( vector[1] + 1 ) >> 1 );
            lp[width] = vector[1] + lp[0];
        }
    }

    /*
     * alle Zeilen
     */
    if ( ll_width > 2 )
    {
        lp = lptr;
        if ( xg_flag )
        {
            for ( y = 0; y < ll_height; y++, lp += width )
            {
                ( void* ) memcpy ( as, lp, xsizef_vlh );
                ( void* ) memcpy ( ds, &lp[xvlh], xsizef_vlh );
                i_5_3R_1d ( lp, as, ds, ll_width, xvlh );
            }
        }
        else
        {
            for ( y = 0; y < ll_height; y++, lp += width )
            {
                ( void* ) memcpy ( as, lp, xsizef_vlh );
                ( void* ) memcpy ( ds, &lp[xvlh], xsizef_vlh );
                i_5_3R_1d_u ( lp, as, ds, ll_width, xvlh );
            }
        }
    }
    else if ( ll_width == 2 )
    {
        lp = lptr;

        for ( y = ll_height; y > 0; y--, lp += width )
        {
            vector[0] = lp[0];
            vector[1] = lp[1];

            lp[0] = vector[0] - ( ( vector[1] + 1 ) >> 1 );
            lp[1] = vector[1] + lp[0];
        }
    }
    return 0;
}

/*--------------------------------------------------------------*
 * i_5_3R_control()	loop trough subbands
 *--------------------------------------------------------------*/
int i_5_3R_control ( IMAGEc *im, long lptr[], DWT_PARAM *dwt )
{
    char		*rtn = "i_5_3R_control";
    register int	i;
    long		*vector, *as, *ds;
    unsigned int  max_dim;

    max_dim = im->width;
    if ( max_dim < im->height ) max_dim = im->height;

    /** create arrays for 1D-Transformation **/
    vector = ( long* ) malloc ( sizeof ( long ) * max_dim );
    if ( vector == NULL )
    {
        fprintf ( stderr, "\n %s: Unable to allocate vector!\n", rtn );
        return ( 2 );
    }
    as = ( long* ) malloc ( sizeof ( long ) * ( max_dim + 1 ) / 2 );
    if ( as == NULL )
    {
        fprintf ( stderr, "\n %s: Unable to allocate as!\n", rtn );
        return ( 2 );
    }
    ds = ( long* ) malloc ( sizeof ( long ) * max_dim / 2 );
    if ( ds == NULL )
    {
        fprintf ( stderr, "\n %s: Unable to allocate ds!\n", rtn );
        return ( 2 );
    }


    /** forward 2D-transformation **/

    for ( i = dwt->num_sb - 1; i >= 0; i-- )
    {
        i_5_3R_2d ( lptr, vector, as, ds, im,
                    ( unsigned ) dwt->p_lx[i], ( unsigned ) dwt->p_ly[i] );
    }

    free ( vector );
    free ( as );
    free ( ds );
    return 0;
}

