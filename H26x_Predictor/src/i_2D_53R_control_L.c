/*****************************************************************
 *
 * File....: i_2D_53R_control.c
 * Function: inverse integer 2D-5/3-Wavelet-Transform w/o float
 * Author..: Tilo Strutz
 * Date....: 18.10.2011
 *
 *****************************************************************/

#include "iwt.h"
//#include "defines.h"

#define QDEBUG
#undef QDEBUG

/*--------------------------------------------------------------*
 * symmetric_ext_L()	symmetric extension of border values
 *--------------------------------------------------------------*/
void symmetric_ext_L ( long aptr[], int ll_awidth, int ll_aheight,
                       int awidth )
{
    int x, y;
    int awidth2;
    unsigned long apos, apy;

    awidth2 = awidth + awidth;

    /*
     * symmetric extension of top border
     */
    apy = 0;
    {
        for ( x = 1, apos = apy + 1; x < ll_awidth - 1; x++, apos++ )
        {
            aptr[apos] = aptr[apos + awidth2];
        }
    }
    /* symmetric extension of bottom border */
    apy = awidth * ( ll_aheight - 1 );
    {
        for ( x = 1, apos = apy + 1; x < ll_awidth - 1; x++, apos++ )
        {
            aptr[apos] = aptr[apos - awidth2];
        }
    }

    /* symmetric extension of left border */
    for ( y = 0, apy = 0; y < ll_aheight; y++, apy += awidth )
        /* symmetric extension of right border */
    {
        apos = apy;
        aptr[apos] = aptr[apos + 2];
        apos = apy + ll_awidth - 1; /* last column	*/
        aptr[apos] = aptr[apos - 2];
    }
}

/*--------------------------------------------------------------*
 * i_2D_53R_2d_L()	2D transform
 *--------------------------------------------------------------*/
void
i_2D_53R_2d_L ( long lptr[], long aptr[],
                IMAGEc *im, unsigned int ll_width, unsigned int ll_height )
{
    register 	int	x, y;

    int N, S, O, W, NO, NW, SO, SW;
    int 	width, awidth, awidth2, aheight, ll_awidth, ll_aheight;
    long xHat;
    unsigned long pos, apos, py, apy;

    width = im->width;
    awidth = im->width + 2;
    aheight = im->height + 2;
    awidth2 = awidth + awidth;
    ll_awidth = ll_width + 2;
    ll_aheight = ll_height + 2;

    N = -awidth;
    S = +awidth;
    W = -1;
    O = +1;
    NO = +1 - awidth;
    SO = +1 + awidth;
    NW = -1 - awidth;
    SW = -1 + awidth;

    /**********************************************
     * copy transformed data back
     *
     */
    /* AA band */
    for ( y = 0, apy = awidth2, py = 0; ( unsigned ) y < ll_height / 2;
            y++, apy += awidth2, py += width )
    {
        /* odd rows	*/
        for ( x = 0, apos = apy + 2, pos = py; ( unsigned ) x < ( ll_width ) / 2;
                x++, apos += 2, pos++ )
        {
            aptr[apos] = lptr[pos];
        }
    }

    /* AD band */
    for ( y = 0, apy = awidth2, py = 0; ( unsigned ) y < ( ll_height ) / 2;
            y++, apy += awidth2, py += width )
    {
        /* even rows	*/
        for ( x = 0, apos = apy + 1, pos = py + ( ll_width ) / 2;
                ( unsigned ) x < ( ll_width + 1 ) / 2; x++, apos += 2, pos++ )
        {
            aptr[apos] = lptr[pos] ;
        }
    }

    /* D1 band */
    for ( y = 0, apy = awidth, py = ( ll_height ) / 2 * width;
            ( unsigned ) y < ( ll_height + 1 ) / 2; y++, apy += awidth2, py += width )
    {
        /* even rows	*/
        for ( x = 0, apos = apy + 1, pos = py + ll_width / 2;
                ( unsigned ) x < ( ll_width + 1 ) / 2; x++, apos += 2, pos++ )
        {
            aptr[apos] = lptr[pos] ;
        }
    }

    /* D2 band */
    for ( y = 0, apy = awidth, py = ( ll_height ) / 2 * width;
            ( unsigned ) y < ( ll_height + 1 ) / 2; y++, apy += awidth2, py += width )
    {
        /* odd rows */
        for ( x = 0, apos = apy + 2, pos = py;
                ( unsigned ) x < ll_width / 2; x++, apos += 2, pos++ )
        {
            aptr[apos] = lptr[pos] ;
        }
    }


    /***********************************************************
     * symmetric extension of borders,
     */
    symmetric_ext_L ( aptr, ll_awidth, ll_aheight, awidth );

    /*----------------------------------------------*/

    /* compute AA2  (Update 2)
     *  -0.0625 0.25 -0.0625
     *   0.25   1.0   0.25
     *  -0.0625 0.25 -0.0625
     */
    for ( y = 2, apy = awidth2; y < ll_aheight - 1; y += 2, apy += awidth2 )
    {
        /* even rows	*/
        for ( x = 2, apos = apy + 2; x < ll_awidth - 1; x += 2, apos += 2 )
        {
            /*  A/4 - B/16 = (4*A + B) / 16	*/
            aptr[apos] = aptr[apos] - ( (
                                            ( ( aptr[apos + N] + aptr[apos + S] +
                                                aptr[apos + W] + aptr[apos + O] ) << 2 ) -
                                            ( aptr[apos + NW] + aptr[apos + NO] +
                                              aptr[apos + SO] + aptr[apos + SW] )
                                            + 8 ) >> 4 );
            /*
            			aptr[apos] = aptr[apos] - ROUND(
            						(aptr[apos+N] + aptr[apos+S] +
            						 aptr[apos+W] + aptr[apos+O]) * 0.25F -
            						(aptr[apos+NW] + aptr[apos+NO] +
            						 aptr[apos+SO] + aptr[apos+SW]) * 0.0625F );
            */
        }
    }


    /**********************************************************
     * symmetric extension of top border for AD
     */
    symmetric_ext_L ( aptr, ll_awidth, ll_aheight, awidth );

    /*----------------------------------------------*/

    /* compute AD
     *       -0.25
     *   0.5   1.0 0.5
     *       -0.25
     */
    for ( y = 1, apy = awidth2; ( unsigned ) y < ll_height; y += 2, apy += awidth2 )
    {
        /* odd rows	*/
        for ( x = 1, apos = apy + 1; x < ll_awidth - 1; x += 2, apos += 2 )
        {
            /*  A/2 - B/4 = (2*A + B) / 4	*/
            aptr[apos] = aptr[apos] - (	(
                                            ( aptr[apos + N] + aptr[apos + S] ) -
                                            ( ( aptr[apos + W] + aptr[apos + O] ) << 1 )
                                            + 2 ) >> 2 );
            /*
            			aptr[apos] = aptr[apos] +
            						ROUND( (aptr[apos+W] + aptr[apos+O]) * 0.5F +
            								  -(aptr[apos+N] + aptr[apos+S]) * 0.25F );
            */
        }
    }

    /***************************************************************
     * symmetric extension of borders, A1 Update
     */
    symmetric_ext_L ( aptr, ll_awidth, ll_aheight, awidth );

    /*----------------------------------------------*/



    /*******************************************************************
     * compute D2 signal
     *       -0.5
     *  0.25  1.0  0.25
     *       -0.5
     */
    for ( y = 1, apy = awidth; y < ll_aheight - 1; y += 2, apy += awidth2 )
    {
        /* even rows	*/
        for ( x = 2, apos = apy + 2; x < ll_awidth - 1; x += 2, apos += 2 )
        {
            /*  A/2 - B/4 = (2*A + B) / 4	*/
            aptr[apos] = aptr[apos] - (	(
                                            ( aptr[apos + W] + aptr[apos + O] ) -
                                            ( ( aptr[apos + N] + aptr[apos + S] ) << 1 )
                                            + 2 ) >> 2 );
            /*
            			aptr[apos] = aptr[apos] +
            						ROUND(  (aptr[apos+N] + aptr[apos+S]) * 0.5F +
            								   -(aptr[apos+W] + aptr[apos+O]) * 0.25F );
            */
        }
    }

    /*  D1 signal *************************************
     * symmetric extension of borders
     */
    symmetric_ext_L ( aptr, ll_awidth, ll_aheight, awidth );

    /*******************************************************************
     * compute D1 signal
     * NW   N  NO
     * W   1.0 S
     * SW   S  SO
     */
    for ( y = 0, apy = awidth; ( unsigned ) y < ll_height; y += 2, apy += awidth2 )
    {
        /* even rows	*/
        for ( x = 1, apos = apy + 1; x < ll_awidth - 1; x += 2, apos += 2 )
        {
            /*  A/2 - B/4 = (2*A + B) / 4	*/
            xHat = (
                       ( ( aptr[apos + S] + aptr[apos + N] +
                           aptr[apos + W] + aptr[apos + O] ) << 1 ) -
                       ( aptr[apos + SW] + aptr[apos + NO] +
                         aptr[apos + NW] + aptr[apos + SO] )
                       + 2 )	>> 2;
            aptr[apos] = aptr[apos] + xHat;
//			aptr[apos] = aptr[apos] + ROUND( xHat[0]);
        }
    }



    /* copy data into original array	*/
    for ( y = 0, py = 0, apy = awidth; ( unsigned ) y < ll_height; y++, py += width, apy += awidth )
    {
        for ( x = 0, pos = py, apos = apy + 1; ( unsigned ) x < ll_width; x++, pos++, apos++ )
        {
            lptr[pos] = aptr[apos];
        }
    }
}
/*--------------------------------------------------------------*
 * i_2D_53R_control_L()	loop trough subbands
 *--------------------------------------------------------------*/
int i_2D_53R_control_L ( IMAGEc *im, DWT_PARAM *dwt, long lptr[] )
{
    char		*rtn = "i_2D_53R_control_L";
    register int 	i;
    long		*aptr;


    /* create array for 2D-Transformation with 1-pixel border */
    aptr = ( long* ) malloc ( sizeof ( long ) * ( im->width + 2 ) * ( im->height + 2 ) );
    if ( aptr == NULL )
    {
        fprintf ( stderr, "\n %s: Unable to allocate vector!\n", rtn );
        return ( 2 );
    }
#ifdef QDEBUG
    for ( i = 0; ( unsigned ) i <  ( im->width + 2 ) * ( im->height + 2 ); i++ )
        aptr[i] = -99;
#endif

    /** forward 2D-transformation **/

    for ( i = dwt->num_sb - 1; i >= 0; i-- )
    {
        i_2D_53R_2d_L ( lptr, aptr, im,
                        ( unsigned ) dwt->p_lx[i], ( unsigned ) dwt->p_ly[i] );

    }
    free ( aptr );
    return 0;
}

