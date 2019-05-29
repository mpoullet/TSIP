/*****************************************************************
 *
 * Datei...:	f_5_3R_control.c
 * Funktion:	Festkomma-5/3-Wavelet-Transformation 2D
 * Autor...:	Tilo Strutz
 * Datum...: 	13.04.2000
 *
 *****************************************************************/

#include "iwt.h"

//#define QDEBUG


/*--------------------------------------------------------------*
 * f_5_3R_1d()
 *--------------------------------------------------------------*/
void f_5_3R_1d ( long vector[], long as[], long ds[], int vlh )
{
    register int		i;
    register long	*lp, *lpp1, *lpp2, *lambda, *gamma;

    // Ruf mit: f_5_3R_1d( vector, lp, &lp[xvlh], xvlh);

    lambda = as;	/* low- pass coefficients */
    gamma  = ds;	/* high-pass coefficients */

    /* assuming symmetric extension calculate left border  */
    *gamma  = vector[1] - ( ( vector[0] + vector[2] ) >> 1 );
    *lambda = vector[0] + ( ( *gamma + 1 ) >> 1 );

    lp   = vector + 2;
    lpp1 = vector + 3;
    lpp2 = vector + 4;

    gamma ++;
    lambda++;

    for ( i = ( vlh - 2 ); i > 0;
            i--, lp += 2, lpp1 += 2, lpp2 += 2, gamma++, lambda++ )
    {
        *gamma  = *lpp1 - ( ( *lp          + *lpp2     ) >> 1 );
        *lambda = *lp   + ( ( * ( gamma - 1 ) + *gamma + 2 ) >> 2 );
    }

    /* only even vec_len is processed currently */
    *gamma  = *lpp1 - *lp;
    *lambda = *lp   + ( ( * ( gamma - 1 ) + *gamma + 2 ) >> 2 );
}


/*--------------------------------------------------------------*
 * f_5_3R_1d_y()
 *--------------------------------------------------------------*/
void f_5_3R_1d_y ( long vector[], long as[], long ds[], int vlh,
                   unsigned int width )
{
    register int		i;
    register long	*lp, *lpp1, *lpp2, *lambda, *gamma;

    lambda = as;	/* low- pass coefficients */
    gamma  = ds;	/* high-pass coefficients */

    /* assuming symmetric extension calculate left border  */
    *gamma  = ( vector[1] ) - ( ( vector[0] + vector[2] ) >> 1 );
    *lambda = ( vector[0] ) + ( ( *gamma + 1 ) >> 1 );

    lp   = vector + 2;
    lpp1 = vector + 3;
    lpp2 = vector + 4;

    gamma  += width;
    lambda += width;

    for ( i = ( vlh - 2 ); i > 0;
            i--, lp += 2, lpp1 += 2, lpp2 += 2, gamma += width, lambda += width )
    {
        *gamma  = *lpp1 - ( ( *lp              + *lpp2     ) >> 1 );
        *lambda = *lp   + ( ( * ( gamma - width ) + *gamma + 2 ) >> 2 );
    }

    /* only even vec_len is processed currently */
    *gamma  = *lpp1 -   *lp;
    *lambda = *lp   + ( ( * ( gamma - width ) + *gamma + 2 ) >> 2 );
}

/*--------------------------------------------------------------*
 * f_5_3R_1d_u()
 *--------------------------------------------------------------*/
void f_5_3R_1d_u ( long vector[], long as[], long ds[], int vlh )
{
    register int		i;
    register long	*lp, *lpp1, *lpp2, *lambda, *gamma;

    lambda = as;	/* low- pass coefficients */
    gamma  = ds;	/* high-pass coefficients */

    /* assuming symmetric extension calculate left border  */
    *gamma  = vector[1] - ( ( vector[0] + vector[2] ) >> 1 );
    *lambda = vector[0] + ( ( *gamma + 1 ) >> 1 );

    lp   = vector + 2;
    lpp1 = vector + 3;
    lpp2 = vector + 4;

    gamma ++;
    lambda++;

    for ( i = ( vlh - 2 ); i > 0;
            i--, lp += 2, lpp1 += 2, lpp2 += 2, gamma++, lambda++ )
    {
        *gamma  = *lpp1 - ( ( *lp          + *lpp2 ) >> 1 );
        *lambda = *lp   + ( ( * ( gamma - 1 ) + *gamma + 2 ) >> 2 );
    }

    *lambda = *lp + ( ( * ( gamma - 1 ) + 1 ) >> 1 );
}


/*--------------------------------------------------------------*
 * f_5_3R_1d_uy()
 *--------------------------------------------------------------*/
void f_5_3R_1d_uy ( long vector[], long as[], long ds[], int vlh,
                    unsigned int width )
{
    register int		i;
    register long	*lp, *lpp1, *lpp2, *lambda, *gamma;

    lambda = as;	/* low- pass coefficients */
    gamma  = ds;	/* high-pass coefficients */

    /* assuming symmetric extension calculate left border  */
    *gamma  = vector[1] - ( ( vector[0] + vector[2] ) >> 1 );
    *lambda = vector[0] + ( ( *gamma + 1 ) >> 1 );

    lp   = vector + 2;
    lpp1 = vector + 3;
    lpp2 = vector + 4;

    gamma  += width;
    lambda += width;

    for ( i = ( vlh - 2 ); i > 0; i--, lp += 2, lpp1 += 2, lpp2 += 2,
            gamma += width, lambda += width )
    {
        *gamma  = *lpp1 - ( ( *lp              + *lpp2     ) >> 1 );
        *lambda = *lp   + ( ( * ( gamma - width ) + *gamma + 2 ) >> 2 );
    }
    *lambda = *lp + ( ( * ( gamma - width ) + 1 ) >> 1 );
}


/*--------------------------------------------------------------*
 * f_5_3R_2d()	DWT eines 2D-Signals
 *--------------------------------------------------------------*/
void
f_5_3R_2d ( long lptr[], long vector[],
            IMAGEc *im, unsigned int ll_width, unsigned int ll_height,
            DWT_PARAM *dwt )
{
    register 	int	x, y;
    register	long	*lp, *lp1;

    int 		xg_flag, yg_flag;
    int		xvlh, yvlh;
    long		*lpx, *lpx2;
    unsigned int 	width, ll_width_long;

    width = im->width;

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

    /*
    * alle Zeilen
    */
    lp = lptr;
    ll_width_long = sizeof ( long ) * ll_width;

    if ( ll_width > 2 )
    {

        if ( xg_flag )
        {
            for ( y = ll_height; y > 0; y--, lp += width )
            {
                ( void* ) memcpy ( vector, lp, ll_width_long );
                f_5_3R_1d ( vector, lp, &lp[xvlh], xvlh );
            }
        }
        else
        {
            for ( y = ll_height; y > 0; y--, lp += width )
            {
                ( void* ) memcpy ( vector, lp, ll_width_long );
                f_5_3R_1d_u ( vector, lp, &lp[xvlh], xvlh );
            }
        }
    }
    else if ( ll_width == 2 )
    {
        for ( y = ll_height; y > 0; y--, lp += width )
        {
            ( void* ) memcpy ( vector, lp, ll_width_long );

            lp[1] = vector[1] - ( vector[0] );
            lp[0] = vector[0] + ( ( lp[1] + 1 ) >> 1 );
        }
    }
    else
        fprintf ( stderr, "\n too less columns in this LL-band (%d)",
                  ll_width );

    /*
    * alle Spalten
    */
    if ( ll_height > 2 && !dwt->h_only_flag )
    {
        lpx = lptr;
        lpx2 = lpx + yvlh * width;
        if ( yg_flag )
        {
            for ( x = ll_width; x > 0; x--, lpx++, lpx2++ )
            {
                lp  = lpx;
                lp1 = vector;
                for ( y = ll_height; y > 0; y--, lp += width, lp1++ )
                    * lp1 = *lp;

                /* ergebnis wird gleich vertikal einsortiert */
                f_5_3R_1d_y ( vector, lpx, lpx2, yvlh, width );
            }
        }
        else
        {
            for ( x = ll_width; x > 0; x--, lpx++, lpx2++ )
            {
                lp  = lpx;
                lp1 = vector;
                for ( y = ll_height; y > 0; y--, lp += width, lp1++ ) 	* lp1 = *lp;

                /* ergebnis wird gleich vertikal einsortiert */
                f_5_3R_1d_uy ( vector, lpx, lpx2, yvlh, width );
            }
        }
    }
    else if ( ll_height == 2 && !dwt->h_only_flag )
    {
        lp = lptr;

        for ( y = ll_height; y > 0; y--, lp ++ )
        {
            vector[0] = lp[0];
            vector[1] = lp[width];

            lp[width] = vector[1] - ( vector[0] );
            lp[0] = vector[0] + ( ( lp[width] + 1 ) >> 1 );
        }
    }
    else
    {
        fprintf ( stderr, "\n too less rows in this LL-band (%d)",
                  ll_height );
    }
}


/*--------------------------------------------------------------*
 * f_5_3R_control()	loop trough subbands
 *--------------------------------------------------------------*/
int f_5_3R_control ( IMAGEc *im, long lptr[], DWT_PARAM *dwt )
{
    char *rtn = "f_5_3R_control";
    register int i;
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

    for ( i = 0; i < dwt->num_sb; i++ )
    {
        f_5_3R_2d ( lptr, vector, im,
                    ( unsigned ) dwt->p_lx[i], ( unsigned ) dwt->p_ly[i], dwt );

#ifdef QDEBUG
        {
            unsigned int ll_width, ll_height, x, y, x_off, y_off, width;
            unsigned long pos2, py2;
            int cnt = 0;
            double energy = 0., mean = 0., var;

            ll_width = dwt->p_lx[i];
            ll_height = dwt->p_ly[i];
            width = im->width;

            x_off = ( ll_width + 1 ) / 2;
            y_off = ( ll_height + 1 ) / 2;

            for ( y = 1,  py2 = y_off * width;
                    ( unsigned ) y < ll_height; y += 2,  py2 += width )
            {
                for ( x = 1, pos2 = py2 + x_off; ( unsigned ) x < ll_width;
                        x += 2, pos2++ )
                {
                    mean += lptr[pos2];
                    energy += lptr[pos2] * lptr[pos2];
                    cnt++;
                }
            }
            mean = mean / cnt;
            var = energy / cnt - ( mean * mean );
            fprintf ( stderr, "\n D1 %d: %f", ll_width, var );
        }
#endif

#ifdef QDEBUG
        {
            unsigned int ll_width, ll_height, x, y, x_off, y_off, width;
            unsigned long pos2, py2;
            float mean;

            ll_width = dwt->p_lx[i];
            ll_height = dwt->p_ly[i];
            width = im->width;

            /* DD */
            mean = 0;
            x_off = ( ll_width + 1 ) / 2;
            y_off = ( ll_height + 1 ) / 2;
            for ( y = 1,  py2 = y_off * width;
                    ( unsigned ) y < ll_height; y += 2,  py2 += width )
            {
                for ( x = 1, pos2 = py2 + x_off; ( unsigned ) x < ll_width;
                        x += 2, pos2++ )
                {
                    mean += lptr[pos2];
                }
            }
            mean = mean / ( signed ) ( ( ll_width / 2 ) * ( ll_height / 2 ) );
            fprintf ( stderr, "\n DD: %7.3f", mean );

            /* DA */
            mean = 0;
            x_off = ( ll_width + 1 ) / 2;
            y_off = 0;
            for ( y = 0,  py2 = y_off * width;
                    ( unsigned ) y < ll_height; y += 2,  py2 += width )
            {
                for ( x = 1, pos2 = py2 + x_off; ( unsigned ) x < ll_width;
                        x += 2, pos2++ )
                {
                    mean += lptr[pos2];
                }
            }
            mean = mean / ( signed ) ( ( ( ll_width ) / 2 ) * ( ( ll_height + 1 ) / 2 ) );
            fprintf ( stderr, ", DA: %7.3f", mean );

            /* AD */
            mean = 0;
            x_off = 0;
            y_off = ( ll_height + 1 ) / 2;
            for ( y = 1,  py2 = y_off * width;
                    ( unsigned ) y < ll_height; y += 2,  py2 += width )
            {
                for ( x = 0, pos2 = py2 + x_off; ( unsigned ) x < ll_width;
                        x += 2, pos2++ )
                {
                    mean += lptr[pos2];
                }
            }
            mean = mean / ( signed ) ( ( ( ll_width  + 1 ) / 2 ) * ( ( ll_height ) / 2 ) );
            fprintf ( stderr, ", AD: %7.3f", mean );

            /* AA */
            mean = 0;
            x_off = 0;
            y_off = 0;
            for ( y = 0,  py2 = y_off * width;
                    ( unsigned ) y < ll_height; y += 2,  py2 += width )
            {
                for ( x = 0, pos2 = py2 + x_off; ( unsigned ) x < ll_width;
                        x += 2, pos2++ )
                {
                    mean += lptr[pos2];
                }
            }
            mean = mean / ( signed ) ( ( ( ll_width  + 1 ) / 2 ) * ( ( ll_height + 1 ) / 2 ) );
            fprintf ( stderr, ", AA: %7.3f", mean );
        }
#endif
    }

    free ( vector );
    free ( as );
    free ( ds );
    return 0;
}
