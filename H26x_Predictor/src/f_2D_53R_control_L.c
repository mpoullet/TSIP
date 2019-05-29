/*****************************************************************
 *
 * File....: f_2D_53R_control_L.c
 * Function: forward integer 2D-5/3-Wavelet-Transform
 * Author..: Tilo Strutz
 * Date....: 17.10.2011
 *  changes
 *	18.10.2011 Strutz: no float rounding
 *****************************************************************/

#include "iwt.h"



#define QDEBUG
//
#undef QDEBUG

/*--------------------------------------------------------------*
 * f_2D_53R_2d_L()	DWT eines 2D-Signals
 *--------------------------------------------------------------*/
/*--------------------------------------------------------------*
 * f_2D_53R_2d_L()	DWT eines 2D-Signals
 *--------------------------------------------------------------*/
void
f_2D_53R_2d_L ( long lptr[], long aptr[],
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

    /* Positions of subband signals
     *  AA AD
     *  DD DD
     */

    /* copy data into extended array, x_off=y_off=1	*/
    for ( y = 0, py = 0, apy = awidth; ( unsigned ) y < ll_height; y++, py += width, apy += awidth )
    {
        for ( x = 0, pos = py, apos = apy + 1; ( unsigned ) x < ll_width; x++, pos++, apos++ )
        {
            aptr[apos] = lptr[pos];
        }
    }

    /* symmetric extension of border pixels
     * for simplicity all pixels are duplicated even it
     * would be only necessary to take every second
     */
    symmetric_ext_L ( aptr, ll_awidth, ll_aheight, awidth );


    /*******************************************************************
     * compute D1 signal
     * NW   N  NO
     * W   1.0 S
     * SW   S  SO
     */
    for ( y = 0, apy = awidth; ( unsigned ) y < ll_height;
            y += 2, apy += awidth2 )
    {
        /* even rows	*/
        for ( x = 0, apos = apy + 1; ( unsigned ) x < ll_width; x += 2, apos += 2 )
        {
            /* separable transform
             *  0.25  -0.5  0.25
             * -0.5    1   -0.5
             *  0.25  -0.5  0.25
             */
            /**/
            /*  A/2 - B/4 = (2*A + B) / 4	*/
            xHat = (
                       ( ( aptr[apos + S] + aptr[apos + N] +
                           aptr[apos + W] + aptr[apos + O] ) << 1 ) -
                       ( aptr[apos + SW] + aptr[apos + NO] +
                         aptr[apos + NW] + aptr[apos + SO] )
                       + 2 )	>> 2;
            /*	*/
            /* ok
            			xHat = (long)ROUND(
            							(aptr[apos+S] + aptr[apos+N] +
            							 aptr[apos+W] + aptr[apos+O]) * 0.5F -
            						  (aptr[apos+SW] + aptr[apos+NO] +
            							 aptr[apos+NW] + aptr[apos+SO]) * 0.25F
            							 );
            */
            aptr[apos] = aptr[apos] - xHat;
        }
    }
    symmetric_ext_L ( aptr, ll_awidth, ll_aheight, awidth );

    /*******************************************************************
     * compute D2 signal like AD in separable filtering
     * D2 is in same rows as D1
     *       -0.5
     *  0.25  1.0  0.25
     *       -0.5
     */
    for ( y = 0, apy = awidth; ( unsigned ) y < ll_height; y += 2, apy += awidth2 )
    {
        /* even rows	*/
        for ( x = 0, apos = apy + 2; ( unsigned ) x < ll_width; x += 2, apos += 2 )
        {
            /*
            					aptr[apos] = aptr[apos] +	(long)ROUND(
            						  (aptr[apos+W] + aptr[apos+O]) * 0.25F
            						- (aptr[apos+N] + aptr[apos+S]) * 0.5F
            						);
            */
            /*  A/2 - B/4 = (2*A + B) / 4	*/
            aptr[apos] = aptr[apos] + (	(
                                            ( aptr[apos + W] + aptr[apos + O] ) -
                                            ( ( aptr[apos + N] + aptr[apos + S] ) << 1 )
                                            + 2 ) >> 2 );
        }
    }

    /**********************************************************
     * symmetric extension for DA
     */
    symmetric_ext_L ( aptr, ll_awidth, ll_aheight, awidth );


    /* compute DA
     *        0.25
     *  -0.5   1.0 -0.5
     *        0.25
     */
    for ( y = 0, apy = awidth2; ( unsigned ) y < ll_height; y += 2, apy += awidth2 )
    {
        /* odd rows	*/
        for ( x = 0, apos = apy + 1; ( unsigned ) x < ll_width; x += 2, apos += 2 )
        {
            /*
            			aptr[apos] = aptr[apos] + (long)ROUND(
            						  (aptr[apos+N] + aptr[apos+S]) * 0.25F
            						- (aptr[apos+W] + aptr[apos+O]) * 0.5F
            						);
            */
            /*  A/2 - B/4 = (2*A + B) / 4	*/
            aptr[apos] = aptr[apos] + (	(
                                            ( aptr[apos + N] + aptr[apos + S] ) -
                                            ( ( aptr[apos + W] + aptr[apos + O] ) << 1 )
                                            + 2 ) >> 2 );
        }
    }

    /*
     * symmetric extension of borders
     */
    symmetric_ext_L ( aptr, ll_awidth, ll_aheight, awidth );

    /* compute AA2  (Update 2)
     *  -0.0625 0.25 -0.0625
     *   0.25   1.0   0.25
     *  -0.0625 0.25 -0.0625
     */
    for ( y = 0, apy = awidth2; ( unsigned ) y < ll_height; y += 2, apy += awidth2 )
    {
        /* even rows	*/
        for ( x = 0, apos = apy + 2; ( unsigned ) x < ll_width; x += 2, apos += 2 )
        {
            /*
            			aptr[apos] = aptr[apos] + (long)ROUND(
            						  (aptr[apos+N] + aptr[apos+S] +
            						   aptr[apos+W] + aptr[apos+O] ) * 0.25F
            						- (aptr[apos+NW] + aptr[apos+NO] +
            							 aptr[apos+SO] + aptr[apos+SW] ) * 0.0625F
            														);
            */
            /*  A/4 - B/16 = (4*A + B) / 16	*/
            aptr[apos] = aptr[apos] + ( (
                                            ( ( aptr[apos + N] + aptr[apos + S] +
                                                aptr[apos + W] + aptr[apos + O] ) << 2 ) -
                                            ( aptr[apos + NW] + aptr[apos + NO] +
                                              aptr[apos + SO] + aptr[apos + SW] )
                                            + 8 ) >> 4 );

        }
    }

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
            lptr[pos] = aptr[apos] ;
        }
    }

    /* D3=DA band */
    for ( y = 0, apy = awidth2, py = 0; ( unsigned ) y < ( ll_height ) / 2;
            y++, apy += awidth2, py += width )
    {
        /* even rows	*/
        for ( x = 0, apos = apy + 1, pos = py + ( ll_width ) / 2;
                ( unsigned ) x < ( ll_width + 1 ) / 2; x++, apos += 2, pos++ )
        {
            lptr[pos] = aptr[apos];
        }
    }

    /* D1=DD band */

    for ( y = 0, apy = awidth, py = ( ll_height ) / 2 * width;
            ( unsigned ) y < ( ll_height + 1 ) / 2; y++, apy += awidth2, py += width )
    {
        /* even rows	*/
        for ( x = 0, apos = apy + 1, pos = py + ll_width / 2;
                ( unsigned ) x < ( ll_width + 1 ) / 2; x++, apos += 2, pos++ )
        {
            lptr[pos] = aptr[apos] ;
        }
    }

    /* D2=AD band */
    for ( y = 0, apy = awidth, py = ( ll_height ) / 2 * width;
            ( unsigned ) y < ( ll_height + 1 ) / 2; y++, apy += awidth2, py += width )
    {
        /* odd rows */
        for ( x = 0, apos = apy + 2, pos = py;
                ( unsigned ) x < ll_width / 2; x++, apos += 2, pos++ )
        {
            lptr[pos] = aptr[apos] ;
        }
    }
}

/*--------------------------------------------------------------*
 * f_2D_53R_control_L()	loop trough subbands
 *--------------------------------------------------------------*/
int f_2D_53R_control_L ( IMAGEc *im, DWT_PARAM *dwt, long lptr[] )
{
    char *rtn = "f_2D_53R_control_L";
    register int i;
    long		*aptr;

    /* create array for 2D-Transformation with 1-pixel border */
    aptr = ( long* ) malloc ( sizeof ( long ) * ( im->width + 2 ) * ( im->height + 2 ) );
    if ( aptr == NULL )
    {
        fprintf ( stderr, "\n %s: Unable to allocate vector!\n", rtn );
        return ( 2 );
    }

    /** forward 2D-transformation **/

    for ( i = 0; i < dwt->num_sb; i++ )
    {
        f_2D_53R_2d_L ( lptr, aptr, im,
                        ( unsigned ) dwt->p_lx[i], ( unsigned ) dwt->p_ly[i] );
    }
    free ( aptr );
    return 0;
}
