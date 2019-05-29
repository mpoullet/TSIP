/*******************************************************************
 *
 * File....: arithmetic.c
 * Function: Arithmetic coding - Coding functions
 * Author..: Dávid Bankó
 *
 ********************************************************************/

#include "extract.h"

static unsigned int index = 0;

void ExtractSeries( unsigned int *arr, unsigned int len, char s)
{
	unsigned int i;
	char name[20] = "Kep_";
	char tmp[10];
	FILE *out;

	if (s == 'R')
		strcat( name, "Run_");
	itoa( index, tmp, 10);
	strcat( name, tmp);
	out = fopen( name, "w");
	for ( i = 0; i < len; i++)
	{
		fprintf( out, "%d\n", arr[i]);
	}
	fclose( out);

	index++;
	return;
}