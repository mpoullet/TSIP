/*****************************************************************
 *
 * File....:	init.c
 * Function:	initialisierung von JPEG-LS+
 * Author..:	Tilo Strutz
 * Date....: 	14.02.2011
 * 11.04.2014 bugfix in init_contextsR()() division by zero if maxVal = 0
 *****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "loco.h"
#include "tstypes.h"

extern char *q9R;

/*---------------------------------------------------------------
 *	init_constantsR()
 *---------------------------------------------------------------*/
void init_constantsR( IMAGE_LOCO *im, OPTION *op, CON *pc)
{
  int bpp;
  
    pc->MAXVAL = im->max_value;
    pc->NEAR   = op->nearlossless;
    pc->NEARm2p1 = pc->NEAR * 2 + 1;
    pc->RANGE   = (pc->MAXVAL + 2 * pc->NEAR) / (pc->NEARm2p1) + 1;
    pc->RANGE_R   =  pc->RANGE * pc->NEARm2p1;
    pc->RANGEd2   =  pc->RANGE / 2;
/*    pc->RANGEp1d2 = (pc->RANGE + 1) / 2; */
    pc->RANGEm1d2 = (pc->RANGE - 1) / 2;

    /* Bittiefe fuer RANGE	*/
    pc->qbpp = 0;
    bpp = pc->RANGE - 1;
    while (bpp >> pc->qbpp) { pc->qbpp++;}
    if (!pc->qbpp) pc->qbpp = 1;
    
    bpp = max( 2, im->bit_depth);
    if (bpp <  8)    	pc->LIMIT  = 2 * (bpp + 8);
    else	    	pc->LIMIT  = 4 * bpp;
    pc->LIMITq = pc->LIMIT - pc->qbpp - 1;
    
    pc->MAX_C =  127;
    pc->MIN_C = -128;
    
    /* Begrenzung des Adaptationsgedaechtnisses abhaengig von 
     * der Groesze des Bildes
     */
    if (op->reset_flag == 0)
    	 pc->RESET = (im->width + im->height + 16) / 32 + 32;
    else pc->RESET = op->reset;
}

/*---------------------------------------------------------------
 *	init_contextsR()
 *---------------------------------------------------------------*/
void init_contextsR( OPTION *op, CON *pc)
{
  int i, t1, t2, t3, fac, setA;

    /* Initialize the contexts. All uninitialized values are 0. */
    setA = max( 2, (pc->RANGE + 32) / 64);
    for ( i = 0; i < NUM_OF_CONTEXTS+2; i++)
    {
     	Ar[i] = setA;
     	Nr[i] = 1;
     	Br[i] = Cr[i] = 0;
    }

    /* Adaptive Initialisierung der Kontextschwellen	*/
    if (!op->T_flag)
    {
      if (pc->MAXVAL > 4095)   	fac = (4096 + 128) / 256;
      else			fac = (pc->MAXVAL + 128) / 256;
    
      if (fac)
      {
    		t1 = fac * (S1 - 2) + 2;
    		t2 = fac * (S2 - 3) + 3;
    		t3 = fac * (S3 - 4) + 4;
      }
      else
      {
				/* pc->MAXVAL < 128; use RANGE as MAXVAL can be zero	*/
				t1 = S1 / (256 / pc->RANGE);	if (t1 < 2) t1 = 2;
				t2 = S2 / (256 / pc->RANGE);	if (t2 < 3) t2 = 3;
				t3 = S3 / (256 / pc->RANGE);	if (t3 < 4) t3 = 4;
      }
      t1 += pc->NEAR * 3;
      t2 += pc->NEAR * 5;
      t3 += pc->NEAR * 7;
      op->T1 = t1;
      op->T2 = t2;
      op->T3 = t3;
    }
    else
    {
    	t1 = op->T1;
    	t2 = op->T2;
    	t3 = op->T3;
    }

//    CALLOC( q9R, pc->MAXVAL * 2 + 1, 1, char);
		ALLOC( q9R, pc->MAXVAL * 2 + 1, char);

    if (pc->NEAR)
    {
    	for ( i = -pc->MAXVAL; i <= pc->MAXVAL; i++)
    	{
    	    if (i <= -t3)      		q9R[i+pc->MAXVAL] = -4;
    	    else if (i <= -t2) 		q9R[i+pc->MAXVAL] = -3;
    	    else if (i <= -t1) 		q9R[i+pc->MAXVAL] = -2;
    	    else if (i <= -pc->NEAR-1)	q9R[i+pc->MAXVAL] = -1;
    	    else if (i <=  pc->NEAR)	q9R[i+pc->MAXVAL] = 0;
    	    else if (i <  t1)		q9R[i+pc->MAXVAL] = 1;
    	    else if (i <  t2)		q9R[i+pc->MAXVAL] = 2;
    	    else if (i <  t3)		q9R[i+pc->MAXVAL] = 3;
    	    else			q9R[i+pc->MAXVAL] = 4;
    	}
    }
    else
    {
    	for ( i = -pc->MAXVAL; i <= pc->MAXVAL; i++)
    	{
    	    if (i <= -t3)	q9R[i+pc->MAXVAL] = -4;
    	    else if (i <= -t2)	q9R[i+pc->MAXVAL] = -3;
    	    else if (i <= -t1)	q9R[i+pc->MAXVAL] = -2;
    	    else if (i <= -1)	q9R[i+pc->MAXVAL] = -1;
    	    else if (i ==  0)	q9R[i+pc->MAXVAL] = 0;
    	    else if (i <  t1)	q9R[i+pc->MAXVAL] = 1;
    	    else if (i <  t2)	q9R[i+pc->MAXVAL] = 2;
    	    else if (i <  t3)	q9R[i+pc->MAXVAL] = 3;
    	    else		q9R[i+pc->MAXVAL] = 4;
    	}
    }

//    fprintf( stderr,"\n\t T1=%d, T2=%d, T3=%d", t1, t2, t3);    
//    fprintf( stderr,"\t LIMIT=%d", pc->LIMITq);    
//    fprintf( stderr,"\t RESET=%d", pc->RESET);    

}

/*---------------------------------------------------------------
 *	free_qR()
 *---------------------------------------------------------------*/
void free_qR( void)
{
    free( q9R);
}
