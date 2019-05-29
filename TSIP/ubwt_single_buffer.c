/*****************************************************
* Datei: ubwt.c
* Autor: Thomas Schmiedel
* Datum: 02.11.2010
* Desc.: Implementiert Burrows-Wheeler-Transformation
* Vers.: 1.0
******************************************************/


#include "ubwt.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>


/* blocklen ist die Länge Originalnachricht,
blockptr zeigt auf das erste Zeichen der Originalnachricht,
beide müssen in comparesym bekannt sein, deshalb global */
uint blocklen  = 0, 
	 *blockptr = NULL;


/* Vergleichsfunktion für zwei beliebige Rotationen
der Originalnachricht.
Beide werden mit dem ersten Zeichen (ihrer Rotation) beginnend
verglichen. Sind sie gleich, wird das nächst-rechte
Zeichen herangezogen usw.*/
int comparesym(const void *a, const void *b)
{	 
	static uint addval;
	/* Der Vergleich beginnt beim jeweils
	ersten Zeichen beider Zeichenketten (addval = 0),
	und solange diese gleich sind, wird addval erhöht.
	addval ist also nach der for-Schleife der Index 
	des ersten Zeichens, in welchem sich beide
	Rotationen unterscheiden,
	Modulo blocklen verhindert Zugriffsverletzungen 
	und macht die Rotation komplett.
	*(uint*)a ist nun der Startindex einer Rotation
	in der Originalnachricht, *(uint*)b der einer
	anderen Rotation */ 
	for (addval = 0;
		 addval < blocklen &&
		 blockptr[(*(uint*)a + addval) % blocklen] == blockptr[(*(uint*)b + addval) % blocklen];
		 addval++);	
	/* addval ist nun am Index, wo sich beide Zeichenketten
	unterscheiden, die Differenz der Zeichen an dieser
	Stelle wird Quicksort zurückgegeben */
	return blockptr[(*(uint*)a + addval) % blocklen] - blockptr[(*(uint*)b + addval) % blocklen];
}


/* Wendet die BWT auf ein uint-Array an
und liefert den Index der Originalzeile
im Rotatationblock zurück */
uint BWTEncode(uint *arr, uint len)
{
	uint i, original;
	uint *lineindex = NULL,
		 *tmp = NULL;	
	
	/* Siehe oben, globale Var. für 
	Compare-Funktion */
	blocklen = len;
	blockptr = arr;	
	
	/* lineindex ist ein Array, welches
	die Indizes der Startzeichen
	der einzelnen Rotationen enthält,
	die Zeichen selbst sind in der
	Originalnachricht */
	ALLOC(lineindex, len, uint);
	for (i = 0; i < len; i++)
	{
		lineindex[i] = i;
	}	
	
	/* lineindex, also die Rotationen sortieren */	
	qsort( lineindex, len, sizeof(uint), comparesym);
	
	/* Index der Originalnachricht
	im Roationsblock finden */
	for (original = 0;		 
		 lineindex[original] != 0;
		 original++);	
		 
	ALLOC(tmp, len, uint);
	memcpy(tmp, arr, len * sizeof(uint));
	/* Jeweils die letzten Zeichen jeder
	Rotation in arr übernehmen */
	for (i = 0; i < len; i++)
	{
		arr[i] = tmp[(lineindex[i] + len - 1u) % len]; 
	}			
	FREE(tmp);
	FREE(lineindex);
	/* Index der Originalnachricht im Block zurückliefern */	
	return original;
}


/* Vergleicht zwei Zeichen miteinander */
int comparerev(const void *a, const void *b)
{
	return *(uint*)a - *(uint*)b;
}


/* Rücktransformation der BWT */
void BWTDecode(uint *arr, uint len, uint original)
{
	uint *sorted = NULL,
		 *base   = NULL,
		 *rank   = NULL,
		 *tmp    = NULL;	 
	uint i, jumper, maxval;
	
	/* Größten Wert herausfinden */
	for (i = 0, maxval = 0; i < len; i++)
	{
		if (arr[i] > maxval) maxval = arr[i];	
	}	
		
	/* Kopie der Nachricht anlegen und sortieren,
	daraus resultiert die F-Spalte */
	ALLOC(sorted, len, uint);
	memcpy(sorted, arr, len * sizeof(uint));	
	qsort(sorted, len, sizeof(uint), comparerev);
	
	/* base zeigt auf das erste Vorkommen eines 
	Symbols in der sorted-Liste,
	rank ist die Anzahl, wie oft ein Symbol bis zum
	aktuellen Index in der L-Spalte (=arr) schon
	aufgetreten ist */
	ALLOC(base, maxval + 1u, uint);	
	ALLOC(rank, len, uint);
		
	/* rank ermitteln */
	memset(base, 0, (maxval + 1u) * sizeof(uint));	
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
	memset(base, 0xFF, (maxval + 1u) * sizeof(uint));
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
	FREE(sorted);	
	
	ALLOC(tmp, len, uint);
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
	memcpy(arr, tmp, len * sizeof(uint));
	FREE(tmp);	
	
	FREE(base);
	FREE(rank);		
}

