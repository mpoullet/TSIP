/*****************************************************
* File...: uwfc.c
* Descr..: Implementiert Weighted Frequency Count
* Author.: Thomas Schmiedel
* Date...: 02.11.2010, 
* 5.5.2011 (strutz) additional criterion for resetting
* 23.01.2012 separate reset_val an max_weigth for counts rescaling
******************************************************/


#include "uwfc.h"
#include <stdlib.h>
#include <string.h>


/*---------------------------------------------------------
 * WFCEncode()
 *	incremental frequency count
 *------------------------------------------------------*/
void WFCEncode(uint *arr, uint len, uint reset_val, 
							 uint max_weight)
{		
	unsigned int reset;
	uint maxval, i, k, val, index, border, 
		 *weights = NULL,
		 *indices = NULL;
		 
	/* Maximalwert ermitteln */
	for (i = 0, maxval = 0; i < len; i++)
	{
		if (arr[i] > maxval) maxval = arr[i];
	}	
	ALLOC( weights, maxval + 1u, uint);
	ALLOC( indices, maxval + 1u, uint);
	memset( weights, 0, (maxval + 1u) * sizeof(uint));	
	/* Indexliste erstellen, analog zu MTF */
	for (i = 0; i <= maxval; i++)
	{
		indices[i] = i;
	}
	
	FILE *out = fopen( "wfc.txt", "w");
	fprintf( out, "max_count:%6d\t reset: %6d\n", max_weight, reset_val);

	/* Nachricht durchgehen */
	//reset_val = limit * 16; 
	/* reset also if a sufficient number of symbols was processed	*/
	reset = reset_val;
	for (i = 0; i < len; i++)
	{		
		/* Index des aktuellen Nachrichtensymbols finden */
		val = arr[i];
		for (index = 0; 
			 indices[index] != val;
			 index++);
		arr[i] = index;	

		fprintf( out, "%6d\t %6d\n", val, index);

		/* Gewicht dieses Wertes erhöhen */	
		weights[val]++;			
		
		/* Symbol anhand seines Gewichtes vorn in der Liste einsortieren,
		dabei den Index der Liste suchen, für dessen Wert das Gewicht erstmals
		kleiner oder gleich dem gerade aktualisierten Gewicht ist */
		for (border = 0; 
			 border < index &&
			 weights[indices[border]] > weights[val];
			 border++);		
			
		/* Liste updaten, alle Elemente größer "border"
		eine Stelle nach oben verschieben und beim Index "Border"
		das aktuelle Zeichen setzen */
		for (k = index; k > border; k--)
		{
			indices[k] = indices[k - 1u];
		}
		indices[border] = val;		
				
		/* Ist das höchste Gewicht über "limit",
		dann werden alle Gewichte halbiert */
//orig		if (weights[indices[0]] >= limit)
		reset--;
		if (!reset || weights[indices[0]] > max_weight)
		{					
			reset = reset_val;
			for (k = 0; k <= maxval; k++)
			{
				weights[k] >>= 1u;
			}					
		}						
	}	
	FREE(weights);
	FREE(indices);

	fclose(out);
}


/*---------------------------------------------------------
 * WFCDecode()
 *	incremental frequency count
 *------------------------------------------------------*/
void WFCDecode( uint *arr, uint len, uint reset_val, 
							 uint max_weight)
{	
	unsigned int reset;
	uint maxval, i, k, val, index, border, 
		 *weights = NULL,
		 *indices = NULL;
		 
	/* Maximalwert ermitteln */
	for (i = 0, maxval = 0; i < len; i++)
	{
		if (arr[i] > maxval) maxval = arr[i];
	}	
	ALLOC(weights, maxval + 1u, uint);
	ALLOC(indices, maxval + 1u, uint);
	memset(weights, 0, (maxval + 1u) * sizeof(uint));	
	/* Liste initialisieren */
	for (i = 0; i <= maxval; i++)
	{
		indices[i] = i;
	}
	
	/* Nachricht rekonstruieren */
	//reset_val = limit * 16;
	reset = reset_val;
	for ( i = 0; i < len; i++)
	{		
		/* Index in Realwert übersetzen */
		index = arr[i];

		arr[i] = indices[index];
		val = arr[i];	
		/* Gewicht des gefundenen Wertes erhöhen */			
		weights[val]++;			
		
		/* Symbol anhand seines Gewichtes vorn in der Liste einsortieren,
		dabei den Index der Liste suchen, für dessen Wert das Gewicht erstmals
		kleiner oder gleich dem gerade aktualisierten Gewicht ist */
		for ( border = 0; 
			 border < index &&
			 weights[indices[border]] > weights[val];
			 border++);		
			
		/* Liste updaten, alle Elemente größer "border"
		eine Stelle nach oben verschieben und beim Index "Border"
		das aktuelle Zeichen setzen */
		for ( k = index; k > border; k--)
		{
			indices[k] = indices[k - 1u];
		}
		indices[border] = val;		
		
		/* scale counts, if either one single symbol has more than max_weight 
		 * or reset symbols have been processed
		 */
		reset--;
		if (!reset || weights[indices[0]] > max_weight)
		{					
			reset = reset_val;
			for (k = 0; k <= maxval; k++)
			{
				weights[k] >>= 1u;
			}					
		}					
	}	
	FREE( weights);
	FREE( indices);
}


/* Symbole und zugehörige Gewichte nach stdout ausgeben */
void PrintWeights(uint *weights, uint *indices, uint len)
{
	uint i;
	printf("V: ");
	for (i = 0; i < len; i++)
	{
		printf("%i ", indices[i]);
	}
	printf("\nW: ");
	for (i = 0; i < len; i++)
	{
		printf("%i ", weights[indices[i]]);
	}	
	printf("\n\n");
}


