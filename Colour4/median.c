
/*****************************************************************
*
* File....:	median.c
* Function:	subroutine of XREC, do median filtering
* Author..:	Tilo Strutz
* Date....:	29.11.2006
* 15.02.2007 make full median
* 28.06.2007 one-dimensional median operators imedian, imedian5
*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
/*
#include "../MACYD/types.h"
#include "../Main/my_macros.h"
#include "../MatrixUtils/matrix_utils.h"
*/
#include "image.h"

#ifdef MEMWATCH
#include <memwatch.h>
#endif

/*
 * Algorithm from N. Wirth's book, implementation by N. Devillard.
 * This code in public domain.
 */
typedef int elem_type ;
#define ELEM_SWAP(a,b) { register elem_type t=(a);(a)=(b);(b)=t; }

/*---------------------------------------------------------------------------
   Function :   kth_smallest()
   In       :   array of elements, # of elements in the array, rank k
   Out      :   one element
   Job      :   find the kth smallest element in the array
   Notice   :   use the median() macro defined below to get the median. 

                Reference:

                  Author: Wirth, Niklaus 
                   Title: Algorithms + data structures = programs 
               Publisher: Englewood Cliffs: Prentice-Hall, 1976 
    Physical description: 366 p. 
                  Series: Prentice-Hall Series in Automatic Computation 

 ---------------------------------------------------------------------------*/

elem_type kth_smallest(elem_type a[], int n, int k)
{
	register int i,j,l,m;
	register elem_type x;

	l = 0; 
	m = n - 1;
	while (l < m)
	{
		x = a[k];
		i = l;
		j = m;
		do
		{
			while (a[i] < x) i++;
			while (x < a[j]) j--;
			if (i <= j)
			{
				ELEM_SWAP(a[i],a[j]);
				i++; j--;
			}
		} while (i <= j);
		if (j < k) l = i;
		if (k < i) m = j;
	}
	return a[k];
}

#define median(a,n) kth_smallest(a,n,(((n)&1)?((n)/2):(((n)/2)-1)))


/*
Median Finding on a 3-by-3 Grid
by Alan Paeth
from "Graphics Gems", Academic Press, 1990
*/

#define s2(a,b) {register int t; if ((t=b-a)<0) {a+=t; b-=t;}}
#define mn3(a,b,c) s2(a,b); s2(a,c);
#define mx3(a,b,c) s2(b,c); s2(a,c);
#define mnmx3(a,b,c) mx3(a,b,c); s2(a,b);
#define mnmx4(a,b,c,d) s2(a,b); s2(c,d); s2(a,c); s2(b,d);
#define mnmx5(a,b,c,d,e) s2(a,b); s2(c,d); mn3(a,c,e); mx3(b,d,e);
#define mnmx6(a,b,c,d,e,f) s2(a,d); s2(b,e); s2(c,f);\
                            mn3(a,b,c); mx3(d,e,f);
int med3x3( int *b1, int *b2, int *b3)
/*
 * Find median on a 3x3 input box of integers.
 * b1, b2, b3 are pointers to the left-hand edge of three
 * parallel scan-lines to form a 3x3 spatial median.
 * Rewriting b2 and b3 as b1 yields code which forms median
 * on input presented as a linear array of nine elements.
 */
{
    register int r1, r2, r3, r4, r5, r6;
		
    r1 = *b1; b1++; 
		r2 = *b1; b1++; 
		r3 = *b1;
    r4 = *b2; b2++; 
		r5 = *b2; b2++; 
		r6 = *b2;
    mnmx6(r1, r2, r3, r4, r5, r6);
		
    r1 = *b3; b3++;
    mnmx5(r1, r2, r3, r4, r5);
		
    r1 = *b3; b3++;
    mnmx4(r1, r2, r3, r4);
		
    r1 = *b3;
    mnmx3(r1, r2, r3);
    return(r2);
}

int med3x3u( unsigned int *b1, unsigned int *b2, unsigned int *b3)
/*
 * Find median on a 3x3 input box of integers.
 * b1, b2, b3 are pointers to the left-hand edge of three
 * parallel scan-lines to form a 3x3 spatial median.
 * Rewriting b2 and b3 as b1 yields code which forms median
 * on input presented as a linear array of nine elements.
 */
{
    register int r1, r2, r3, r4, r5, r6;
		
    r1 = *b1; b1++; 
		r2 = *b1; b1++; 
		r3 = *b1;
    r4 = *b2; b2++; 
		r5 = *b2; b2++; 
		r6 = *b2;
    mnmx6(r1, r2, r3, r4, r5, r6);
		
    r1 = *b3; b3++;
    mnmx5(r1, r2, r3, r4, r5);
		
    r1 = *b3; b3++;
    mnmx4(r1, r2, r3, r4);
		
    r1 = *b3;
    mnmx3(r1, r2, r3);
    return(r2);
}


/* t2(i,j) transposes elements in A[] such that A[i] <= A[j] */

#define t2(i, j) s2(A[i-1], A[j-1])


int median25(int A[25])
    {
/*
 * median25(A) partitions the array A[0..24] such that element
 * A[12] is the median and subarrays A[0..11] and A[13..24]
 * are partitions containing elements of smaller and larger
 * value (rank), respectively.
 *
 * The exchange table lists element indices on the range 1..25,
 * this accounts for the "-1" offsets in the macro t2 and in
 * the final return value used to adjust subscripts to C-code
 * conventions (array indices begin at zero).
 */
    t2( 1, 2); t2( 4, 5); t2( 3, 5); t2( 3, 4); t2( 7, 8);
    t2( 6, 8); t2( 6, 7); t2(10,11); t2( 9,11); t2( 9,10);
    t2(13,14); t2(12,14); t2(12,13); t2(16,17); t2(15,17);
    t2(15,16); t2(19,20); t2(18,20); t2(18,19); t2(22,23);
    t2(21,23); t2(21,22); t2(24,25); t2( 3, 6); t2( 4, 7);
    t2( 1, 7); t2( 1, 4); t2( 5, 8); t2( 2, 8); t2( 2, 5);
    t2(12,15); t2( 9,15); t2( 9,12); t2(13,16); t2(10,16);
    t2(10,13); t2(14,17); t2(11,17); t2(11,14); t2(21,24);
    t2(18,24); t2(18,21); t2(22,25); t2(19,25); t2(19,22);
    t2(20,23); t2( 9,18); t2(10,19); t2( 1,19); t2( 1,10);
    t2(11,20); t2( 2,20); t2( 2,11); t2(12,21); t2( 3,21);
    t2( 3,12); t2(13,22); t2( 4,22); t2( 4,13); t2(14,23);
    t2( 5,23); t2( 5,14); t2(15,24); t2( 6,24); t2( 6,15);
    t2(16,25); t2( 7,25); t2( 7,16); t2( 8,17); t2( 8,20);
    t2(14,22); t2(16,24); t2( 8,14); t2( 8,16); t2( 2,10);
    t2( 4,12); t2( 6,18); t2(12,18); t2(10,18); t2( 5,11);
    t2( 7,13); t2( 8,15); t2( 5, 7); t2( 5, 8); t2(13,15);
    t2(11,15); t2( 7, 8); t2(11,13); t2( 7,11); t2( 7,18);
    t2(13,18); t2( 8,18); t2( 8,11); t2(13,19); t2( 8,13);
    t2(11,19); t2(13,21); t2(11,21); t2(11,13);
    return(A[13-1]);
    }



/*----------------------------------------------------
 * imedian( v1, v2, v3)
 *----------------------------------------------------*/
int imedian( int v1, int v2, int v3)
{
		if (v1 > v2)
		{
			if ( v2 > v3) return v2;
			else
			{
			 if (v3> v1) return v1; 
			 else return v3;
			}
		}
		else
		{
			if ( v1 > v3) return v1;
			else
			{
			 if (v3> v2) return v2; 
			 else return v3;
			}
		}
}

/*----------------------------------------------------
 * imedian5( v1, v2, v3, v4, v5)
 *----------------------------------------------------*/
int imedian5( int v1, int v2, int v3, int v4, int v5)
{
	register int r1;
	
    r1 = v1; 
				
		mnmx4(r1, v2, v3, v4);
		/* r1 contains now min and max is in r4 
		 * take these values away and insert 5th value
		 */ 
		r1 = v5;
		mnmx3(r1, v2, v3);
		
    return( v2);
}

/*----------------------------------------------------
 * imedian7( v1, v2, v3, v4, v5, v6, v7)
 *----------------------------------------------------*/
int imedian7( int v1, int v2, int v3, int v4, 
							int v5, int v6, int v7)
{
	register int r1;
	
    r1 = v1; 
				
		mnmx5(r1, v2, v3, v4, v5);

		r1 = v6;
		mnmx4(r1, v2, v3, v4);
		/* r1 contains now min and max is in r4 
		 * take these values away and insert 5th value
		 */ 
		r1 = v7;
		mnmx3(r1, v2, v3);
		
    return(v2);
}

/*----------------------------------------------------
 * dmedian( v1, v2, v3)
 *----------------------------------------------------*/
double dmedian( double v1, double v2, double v3)
{
		if (v1 > v2)
		{
			if ( v2 > v3) return v2;
			else
			{
			 if (v3> v1) return v1; 
			 else return v3;
			}
		}
		else
		{
			if ( v1 > v3) return v1;
			else
			{
			 if (v3> v2) return v2; 
			 else return v3;
			}
		}
}

