/**********************************************************************
 * File...: tour_manip.c
 * Purpose: manipulation of order
 * Author.: Tilo Strutz
 * Date:..: 18.05.2015
 * changes:
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <string.h>


/*----------------------------------------------------
 * get_seg_length()
 *----------------------------------------------------*/
int get_seg_length( int N, int first, int last)
{
	if (first <= last)
	{
		return last - first + 1;
	}
	else
	{
		return last+(N-first+1);
	}
}

/*----------------------------------------------------
 * put_segment()
 * include a segment between two nodes
 *----------------------------------------------------*/
void put_segment(/*int N,*/ int *vector, int pos, int *segment, 
								 int len_seg, int reverse_flag)
{
	int i;

	//assert( (pos + len_seg) <= N);
	{
		if (reverse_flag == 1)
		{
			for ( i = 0; i < len_seg; i++)
			{
				vector[i+pos] = segment[len_seg-i-1];
			}
		}
		else
		{
			for (i = 0; i < len_seg; i++)
			{
				vector[i+pos] = segment[i];
			}
		}
	}
}

/*----------------------------------------------------
 * get_segment()
 * get a partial vector
 *----------------------------------------------------*/
void get_segment( /*int N,*/ int *vector, int pos, 
								 int *seg, int len_seg)
{
	int i;

	//assert( (pos + len_seg) <= N);
	{
		// copy part of vector starting at first index
		for (i = 0; i < len_seg; i++)
		{
			seg[i] = vector[pos + i];
		}
	}
	return;
}


/*----------------------------------------------------
 * randperm()
 * simulation of Matlab's randperm() function
 *----------------------------------------------------*/
void randperm( int N, int *randPerm)
{
	int idx1 = 0;
	int idx2 = 0;
	int temp = 0;
	int i;

	//init
	for (i = 0; i < N; i++)
	{
		randPerm[i] = i;
	}
	//mischen N mal
	for (i = 0; i < N; i++)
	{
		idx1 = rand() % N;
		idx2 = rand() % N;
		temp = randPerm[idx1];
		randPerm[idx1] = randPerm[idx2];
		randPerm[idx2] = temp;
	}
	return;
}
