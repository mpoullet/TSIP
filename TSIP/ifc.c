/*****************************************************
* File...: ifc.c
* Descr..: incremental Frequency Count
* Author.: Tilo Strutz
* Date...: 01.08.2012 
* implementation is faster than in uwfc.c !
******************************************************/
#include <stdlib.h>
#include <string.h>
#include "ifc.h"

/*---------------------------------------------------------
 * IFCencode()
 *	incremental frequency count
 *------------------------------------------------------*/
void IFCencode(unsigned int *arr, unsigned int len, 
							 unsigned int reset_val, 
							 unsigned int max_count)
{		
	int stop;
	unsigned int counter;
	unsigned int maxval, i, k, sym, sym2, idx, idxm1;
	unsigned int  *counts = NULL;
	unsigned int  *sym2idx = NULL;
	unsigned int  *idx2sym = NULL;
		 
	/* Maximalwert ermitteln */
	for (i = 0, maxval = 0; i < len; i++)
	{
		if (arr[i] > maxval) maxval = arr[i];
	}	
	ALLOC( sym2idx, maxval + 1u, unsigned int);
	ALLOC( idx2sym, maxval + 1u, unsigned int);
	ALLOC( counts, maxval + 1u, unsigned int);

	if (ERRCODE == MALLOC_FAILED) return; // suppresses warning in code analysis, but makes not very useful

	memset( counts, 0, (maxval + 1u) * sizeof(unsigned int));	
	/* Indexliste erstellen, analog zu MTF */
	for (i = 0; i <= maxval; i++)
	{
		sym2idx[i] = i;
		idx2sym[i] = i;
	}
	counter = reset_val;

	//FILE *out = fopen( "ifc.txt", "w");
	//fprintf( out, "max_count:%6d\t reset: %6d\n", max_count, counter);

	
	/* for all symbols */
	for (i = 0; i < len; i++)
	{		
		sym = arr[i];/* read symbol from sequence	*/
		idx = sym2idx[sym];	/*  get index of symbol */
		arr[i] = idx;	/* replace symbol in sequence by index	*/

		//fprintf( out, "%6d\t %6d\n", sym, idx);

		counts[idx]++;	/* increment count of symbol via index	*/
		counter--;

		/* kind of adaptation of statistics	*/
		if (counter == 0 || (counts[idx] > max_count) )
		{
			for (k = 0; k <= maxval; k++)
			{
				counts[k] >>= 1;
			}
			counter = reset_val;
		}

		do /* kind of bubble sort for a single symbol	*/
		{
			stop = 1;
			/* if symbol has higher or same count than predecessor in list,
			 * then they must exchange their postions
			 * in case of same count, last occurring symbol is priorised
			 */
			idxm1 = idx - 1;
			if ( (idx > 0) && (counts[idx] >= counts[idxm1]) )
			{
				unsigned int t;

				stop = 0;	/* another comparison is needed	*/

				sym2 = idx2sym[idxm1]; /* neighbour symbol	*/

				/* exchange indices	*/
				sym2idx[sym]--;		/* sym2idx[sym] = idxm1; */
				sym2idx[sym2]++;	/* sym2idx[sym2] = idx;	*/

				/* exchange counts	*/
				t = counts[idx];
				counts[idx] = counts[idxm1];
				counts[idxm1] = t;

				/* exchange symbols	*/
				idx2sym[idx] = sym2;
				idx2sym[idxm1] = sym;
			}	
			/* get new index of symbol	*/
			idx = idxm1; /* = sym2idx[sym], required for do-loop	*/

			/* do as long as an exchange took place	*/
		}while( !stop);
	}	
	FREE(counts);
	FREE(idx2sym);
	FREE(sym2idx);

	//fclose(out);
}


/*---------------------------------------------------------
 * IFCdecode()
 *	incremental frequency count
 *------------------------------------------------------*/
void IFCdecode(unsigned int *arr, unsigned int len, 
							 unsigned int reset_val, 
							 unsigned int max_count)
{		
	int stop;
	unsigned int counter;
	unsigned int maxval, i, k, sym, sym2, idx, idxm1;
	unsigned int  *counts = NULL;
	unsigned int  *sym2idx = NULL;
	unsigned int  *idx2sym = NULL;
		 
	/* Maximalwert ermitteln */
	for (i = 0, maxval = 0; i < len; i++)
	{
		if (arr[i] > maxval) maxval = arr[i];
	}	
	ALLOC( sym2idx, maxval + 1u, unsigned int);
	ALLOC( idx2sym, maxval + 1u, unsigned int);
	ALLOC( counts, maxval + 1u, unsigned int);
	if (ERRCODE == MALLOC_FAILED) return; // suppresses warning in code analysis, but makes not very useful

	memset( counts, 0, (maxval + 1u) * sizeof(unsigned int));	
	/* Indexliste erstellen, analog zu MTF */
	for (i = 0; i <= maxval; i++)
	{
		sym2idx[i] = i;
		idx2sym[i] = i;
	}
	counter = reset_val;
	
	/* for all indexes */
	for (i = 0; i < len; i++)
	{		
		idx = arr[i];
		/*  get symbol of index*/
		sym = idx2sym[idx];
		/* replace index by symbol 	*/
		arr[i] = sym;

		counts[idx]++;
		counter--;

		if (counter == 0 || (counts[idx] > max_count) )
		{
			for (k = 0; k <= maxval; k++)
			{
				counts[k] >>= 1;
			}
			counter = reset_val;
		}
		do /* kind of bubble sort for a single symbol	*/
		{
			stop = 1;
			/* symbol has higher or same count then predecessor in list
			 * then they exchange their postions
			 * in case of same count, last occurring symbol is priorised
			 */
			idxm1 = idx - 1;
			if ( (idx > 0) && (counts[idx] >= counts[idxm1]) )
			{
				unsigned int t;

				stop = 0;

				sym2 = idx2sym[idxm1]; /* neighbour symbol	*/

				/* exchange indices	*/
				sym2idx[sym]--;		/* sym2idx[sym] = idxm1; */
				sym2idx[sym2]++;	/* sym2idx[sym2] = idx;	*/

				/* exchange counts	*/
				t = counts[idx];
				counts[idx] = counts[idxm1];
				counts[idxm1] = t;

				/* exchange symbols	*/
				idx2sym[idx] = sym2;
				idx2sym[idxm1] = sym;
			}	
			/* get new index of symbol	*/
			idx = idxm1; /* = sym2idx[sym]	*/

			/* do as long as an exchange took place	*/
		}while( !stop);
	}	
	FREE(counts);
	FREE(idx2sym);
	FREE(sym2idx);
}

