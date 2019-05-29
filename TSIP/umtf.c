/*****************************************************
* Datei: umtf.c
* Autor: Thomas Schmiedel
* Datum: 02.11.2010
* Desc.: Implementiert die Move To Front Codierung
* Vers.: 1.0
******************************************************/


#include "umtf.h"
#include <string.h>


/* Modifizierte Move To Front Codierung,
gefundene Listensymbole werden auf einen bestimmten 
prozentualen Anteil der Liste verschoben */
void MTFEncode(uint *arr, uint len, uint percent)
{	
	uint i, val, k, index, limit, maxval;
	uint *lookup = NULL;
	
	/* Faktor XX% -> 0.XX für Neupositionierung 
	eines gefundenen Wertes in der Liste */ 
	double mul = 0.01 * (double)percent;	
	
	/* Maximalen Wert im Array ermitteln */
	for (i = 0, maxval = 0; i < len; i++)
	{
		if (arr[i] > maxval) maxval = arr[i];
	}
	
	/* lookup enthält alle möglichen Symbole
	der Nachricht in aufsteigender Reihenfolge */
	ALLOC(lookup, maxval + 1u, uint);	
	for (i = 0; i <= maxval; i++)
	{
		lookup[i] = i;
	}
	/* MTF anwenden */
	for (i = 0; i < len; i++)
	{		
		/* Aktuellen Wert holen */
		val = arr[i];	
		/* Welchen Index hat dieser in der Liste? */
		for (index = 0;
			 lookup[index] != val;
			 index++);
		/* Index speichern */
		arr[i] = index;	
		/* An welche Position wird der gefundene
		Wert in der Liste verschoben? 
		percent = 50 führt zu mul = 0.5,
		die Position des gefundenen Symbols wird
		also halbiert */
		limit  = (uint)(mul * (double)index);	
		/* Werte zwischen "limit" und "index" 
		einen Index nach oben verschieben */	
		for (k = index; k > limit; k--)
		{
			lookup[k] = lookup[k - 1u];
		}
		/* Den aktuellen Wert an die
		freigewordene Stelle schreiben */
		lookup[limit] = val;		
	}	
	FREE(lookup);
}


/* Rückwandlung der MTF */
void MTFDecode(uint *arr, uint len, uint percent)
{	
	uint i, k, index, limit, maxval;
	uint *lookup = NULL;
	
	/* Faktor XX% -> 0.XX für Neupositionierung 
	eines gefundenen Wertes in der Liste */ 
	double mul = 0.01 * (double)percent;	
	
	/* Maximalen Wert im Array ermitteln */
	for (i = 0, maxval = 0; i < len; i++)
	{
		if (arr[i] > maxval) maxval = arr[i];
	}
	
	/* lookup enthält alle möglichen Symbole
	der Nachricht in aufsteigender Reihenfolge */	
	ALLOC(lookup, maxval + 1u, uint);	
	for (i = 0; i <= maxval; i++)
	{
		lookup[i] = i;
	}
	/* Rückwandlung */
	for (i = 0; i < len; i++)
	{
		/* Index holen */
		index = arr[i];
		/* Zeichen an dieser Stelle ausgeben */
		arr[i] = lookup[index];	
		/* An welche Position wird der gefundene
		Wert in der Liste verschoben? 
		percent = 50 führt zu mul = 0.5,
		die Position des gefundenen Symbols wird
		also halbiert */
		limit  = (uint)(mul * (double)index);
		/* Werte zwischen "limit" und "index" 
		einen Index nach oben verschieben */	
		for (k = index; k > limit; k--)
		{
			lookup[k] = lookup[k - 1u];
		}
		/* Den aktuellen Wert an die
		freigewordene Stelle schreiben */
		lookup[limit] = arr[i];		
	}	
	FREE(lookup);	
}


/* Lookup Table nach stdout ausgeben */
void PrintLookUp(uint *lu, uint len)
{
	uint i;
	for (i = 0; i < len; i++)
	{
		printf("%i ", lu[i]);
	}
	printf("\n");
}


