/*****************************************************
* Datei: ubwt.c
* Autor: Thomas Schmiedel
* Datum: 15.02.2011
* Desc.: Implementiert Burrows-Wheeler-Transformation
* Vers.: 1.0
* Chnge: Speed optimisation of BWTEncode()
* 10.3.2011 Strutz
******************************************************/


#include "ubwt.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>


/* blocklen ist die Länge Originalnachricht,
blockptr zeigt auf das erste Zeichen der Originalnachricht,
beide müssen in comparesym bekannt sein, deshalb global */
unsigned long blocklen  = 0;
unsigned int 	 *blockptr = NULL;


/* Vergleichsfunktion für zwei beliebige Rotationen
der Originalnachricht.
Beide werden mit dem ersten Zeichen (ihrer Rotation) beginnend
verglichen. Sind sie gleich, wird das nächst-rechte
Zeichen herangezogen usw.*/
int comparesym( const void *a, const void *b)
{	 
	register unsigned long count;
  register unsigned int *pa, *pb;

	pa = &blockptr[*(uint *)a];
	pb = &blockptr[*(uint *)b];
	count = blocklen;
	while( (*pa == *pb) && count)
	{
		 pa++;
		 pb++;
		 count--;
	}
	if (*pa > *pb)	return 1;
	else if (*pa < *pb)	return -1;
	return 0; /* happens only if all symbols in blockptr are equal */
/*
  register unsigned int posa, posb;

	posa = *(uint *)a;
	posb = *(uint *)b;
	addval = 0; /* merely a counter *
	while( (blockptr[posa] == blockptr[posb]) 
					&& (addval < blocklen))
	{
		 addval++;
		 posa++;
		 posb++;
	}
	if (blockptr[posa] > blockptr[posb])	return 1;
	else if (blockptr[posa] < blockptr[posb])	return -1;
	return 0; /* happens only if all symbols in blockptr are equal *
*/

/*
	register uint posa, posb;
	
	posa = *(uint *)a;
	posb = *(uint *)b;
	for (addval = 0;
		 blockptr[posa + addval] == blockptr[posb + addval] && addval < blocklen;
		 addval++);	
	return blockptr[posa + addval] - blockptr[posb + addval];
*/
}


/* Wendet die BWT auf ein uint-Array an
und liefert den Index der Originalzeile
im Rotatationblock zurück */
uint BWTEncode( uint *arr, unsigned long len)
{
	uint i, original;
	unsigned int 	 *tmp = NULL;	
	unsigned long *lineindex = NULL;
		 
	ALLOC( tmp, 2u * len, unsigned int);
	if (ERRCODE == MALLOC_FAILED) return 66; // suppresses warning in code analysis, but makes not very useful

	memcpy( tmp, arr, len * sizeof(uint));
	memcpy( &tmp[len], arr, len * sizeof(uint));
	
	/* lineindex ist ein Array, welches
	die Indizes der Startzeichen
	der einzelnen Rotationen enthält,
	die Zeichen selbst sind in der
	Originalnachricht */
	ALLOC( lineindex, len, unsigned long);
	for (i = 0; i < len; i++)
	{
		lineindex[i] = i;
	}	
	
	/* Siehe oben, globale Var. für 
	Compare-Funktion */
	blocklen = len;
	blockptr = tmp;	
	
	fprintf( stderr, "\n start qsort()");
	/* lineindex, also die Rotationen sortieren */	
	qsort( lineindex, len, sizeof(unsigned long), comparesym);
	
	/* Index der Originalnachricht
	im Rotationsblock finden */
	for (original = 0;		 
		 lineindex[original] != 0;
		 original++);			 
	
	/* Jeweils die letzten Zeichen jeder
	Rotation in arr übernehmen */
	for (i = 0; i < len; i++)
	{
		arr[i] = tmp[lineindex[i] + len - 1u]; 
	}			
	FREE( tmp);
	FREE( lineindex);
	/* Index der Originalnachricht im Block zurückliefern */	
	return original;
}


/* Vergleicht zwei Zeichen miteinander */
int comparerev(const void *a, const void *b)
{
	return *(uint*)a - *(uint*)b;
}


/* Rücktransformation der BWT */
void BWTDecode(uint *arr, unsigned long len, unsigned long original)
{
	uint *sorted = NULL,
		 *base   = NULL,
		 *rank   = NULL,
		 *tmp    = NULL;	 
	unsigned long i, jumper, maxval;
	
	/* Größten Wert herausfinden */
	for (i = 0, maxval = 0; i < len; i++)
	{
		if (arr[i] > maxval) maxval = arr[i];	
	}	
		
	/* Kopie der Nachricht anlegen und sortieren,
	daraus resultiert die F-Spalte */
	ALLOC( sorted, len, uint);
	memcpy( sorted, arr, len * sizeof(uint));	
	qsort( sorted, len, sizeof(uint), comparerev);
	
	/* base zeigt auf das erste Vorkommen eines 
	Symbols in der sorted-Liste,
	rank ist die Anzahl, wie oft ein Symbol bis zum
	aktuellen Index in der L-Spalte (=arr) schon
	aufgetreten ist */
	ALLOC( base, maxval + 1u, uint);	
	ALLOC( rank, len, uint);
		
	/* rank ermitteln */
	memset( base, 0, (maxval + 1u) * sizeof(uint));	
	for (i = 0; i < len; i++)
	{
		/* base zählt erstmal das Auftreten der
		einzelnen Symbole, rank erhält diese Zahl 
		für die aktuelle Position und das aktuelle Symbol */
		rank[i] = base[arr[i]];
		base[arr[i]]++;	
	}	
	
	/* base ermitteln, dafür zuerst jeden Wert
	mit UINT_MAX initialisieren */
	memset( base, 0xFF, (maxval + 1u) * sizeof(uint));
	for (i = 0; i < len; i++)
	{
		/* Ist es das erste Symbol seiner Art, 
		dann beträgt base für dieses Zeichen noch
		UINT_MAX. In diesem Fall wird der aktuelle Index
		als Erstindex für alle Symbole dieses
		Typs übernommen */
		if (base[sorted[i]] == UINT_MAX)
		{
			base[sorted[i]] = i;		
		}	
	}		
	FREE( sorted);	
	
	ALLOC( tmp, len, uint);
	/* Bei der Rekonstruktion wird beim
	mitgelieferten Index begonnen,
	außerdem wird die Nachricht von hinten
	rekonstruiert */	
	for (i = len, jumper = original; i > 0; i--)
	{		
		/* Symbol holen, welches in der L-Spalte
		am index "jumper" steht */
		tmp[i - 1u] = arr[jumper];	
		/* jumper ist nun der Index, wo das 
		gefundene Symbol der L-Spalte in der
		F-Spalte steht und den gleichen Rang besitzt */
		jumper = base[arr[jumper]] + rank[jumper];			
	}		
	/* tmp auf arr rückkopieren */
	memcpy( arr, tmp, len * sizeof(uint));
	FREE( tmp);	
	
	FREE( base);
	FREE( rank);		
}

