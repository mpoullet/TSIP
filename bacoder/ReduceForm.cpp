/*****************************************************
 * Datei...: ReduceForm.cpp
 * Funktion: Code zur Farbquantisierung
 * Autor...: Nico Hücker
 * Date....: 21.06.2012
 ******************************************************/

#include "stdafx.h"
#include "ReduceForm.h"
#include "stats.h"

using namespace bacoder;

/*
 * Starten der Farbreduzierung durch Button-Click
 * mit Übergabe: -Bilddatenstruktur
 *				 -eingegebener Wert für max Abstand
 *				 -farbAnzahl-Array für vorher/nachher Vergleich
 */
System::Void ReduceForm::button1_Click(System::Object^  sender, System::EventArgs^  e) {
	ReduceColours( picdata, Convert::ToUInt32(ReduceForm::textBox1->Text), colourReduction);
	
	e=e; sender=sender;
}

/* 
 * Methode, die die Farbanzahl der Bilddatenstruktur anhand eines
 * maximal erlaubten Abstands im RGB-Farbraum reduziert
 * und deren Anzahl davor und danach Zählt.
 * Häufig vorkommende Farben werden behalten.
 */
unsigned long ReduceColours( PicData *pd, unsigned long distance, unsigned long *colourReduction)
{	
	unsigned long numColours;		/* Anzahl der Palettenfarben */
	unsigned char *palette;			/* Pallete-Array für Farbwerte */

	/* Variablen und Array für Farbhäufigkeiten */
	unsigned long maxCount, maxCountIndex = 0;
	unsigned long *idx_ptr, *indexAnzahl;	// Strutz
	unsigned long colourDistance;	/* Abstand der Farben (Berechnung) */
	unsigned int distance2;
	long i, j, idx;					/* Zähler für Schleifen */
	
	

	distance2 = distance * distance;	/* Quadrieren - spart sqrt() */

	{
		double colEntropy;
		numColours = GetNumColours( pd, &maxCount, &colEntropy);			/* Farben in pd-Struktur Zählen */
	}
	ALLOC( palette, (3*numColours), unsigned char);		/* Speicher für Palette allokieren */

	/*
	 * Bildfarben aus pd in Farbpalette extrahieren und zählen
	 */
	numColours = GetIndexedColours(pd, palette); 
	colourReduction[0] = numColours;					/* Farbanzahl für spätere Ausgabe speichern */
	colourReduction[1] = 0;
	/*
	 * 1. Array für Häufigkeit der Palettenfarben in pd anlegen
	 * 2. Alle Elemente mit 0 initialisieren
	 * 3. Farbhäufigkeit über alle Pixel in pd zählen
	 */

	ALLOC( indexAnzahl, numColours, unsigned long);
	ALLOC( idx_ptr, numColours, unsigned long);		//Strutz

	/* initialise arrays	*/
	for( i = 0; (unsigned)i < numColours; i++)
	{
		indexAnzahl[i] = 0;
		idx_ptr[i] = i;
	}
	/* count colurs	*/
	for( i = 0; (unsigned)i < pd->size; i++)
	{
		indexAnzahl[pd->data[0][i]]++;
	}

	/* sorting of values in ascending order	*/
	heap_sort_ul( numColours, indexAnzahl, idx_ptr);

	/*
	 * 1. Häufigste (noch nicht verwendete) Farbe suchen
	 * 2. Alle Palettenfarben die im Farbraum näher als die eingegebene 
	 *    Distanz an der Gewählten Farbe liegen mit dieser überschreiben
	 * 3. Gewählte Farbe und überschriebene Farbe(n) im Häufigkeits-Array
	 *    auf 0 setzen, damit diese nicht erneut ausgewählt werden
	 * 4. Diesen Vorgang wiederholen bis alle Farben ein Mal gewählt
	 *    oder überschrieben wurden und somit alle Farben im Farbraum
	 *    weiter als die eingegebene Distanz auseinander liegen
	 */
	for( j = numColours-1; j > 0; j--)	/* all colours starting from most frequent */
	{		/* do not use j==0, as it is compared finally to i=j-1	*/
		if (indexAnzahl[ j ]) /* check, whether colour was overwritten in previous loops	*/
		{
			colourReduction[1]++;	/* increment number of remaining colours */
			maxCountIndex = idx_ptr[j];	/* index of remaining palette entry with maximum count	*/
			/* compare to all remaining colours in descending order of ther counts*/
			for( i = j-1; i >=0; i--)	
			{	
				idx = idx_ptr[i];		/* index of palette entry with next maximum count	*/
				if (indexAnzahl[i] != 0)  /* check, whether colour was overwritten in previous loops	*/
				{
					register unsigned long idx1_3, idx2_3;
					/* compute Euclidean distance of two colours */
					idx1_3 = 3 * maxCountIndex;
					idx2_3 = 3 * idx;
					colourDistance = (
							(palette[idx1_3] - palette[idx2_3]) * 
							(palette[idx1_3] - palette[idx2_3]) +
							(palette[idx1_3 + 1] - palette[idx2_3 + 1]) * 
							(palette[idx1_3 + 1] - palette[idx2_3 + 1]) +
							(palette[idx1_3 + 2] - palette[idx2_3 + 2]) * 
							(palette[idx1_3 + 2] - palette[idx2_3 + 2]));
					//colourDistance = max (
					//		abs(palette[idx1_3] - palette[idx2_3]), max (
					//			abs(palette[idx1_3 + 1] - palette[idx2_3 + 1]), 
					//			abs(palette[idx1_3 + 2] - palette[idx2_3 + 2]) )
					//			);

					/* if distance is smaller than allowed distance, 
					 * then overwrite palette entry	with values of colour with higher count
					 */
					//if (colourDistance < distance)	
					if (colourDistance < distance2)	
					{	
						palette[idx2_3]   = palette[idx1_3];
						palette[idx2_3+1] = palette[idx1_3+1];
						palette[idx2_3+2] = palette[idx1_3+2];
						indexAnzahl[i] = 0;	/* colour has been overwritten, do not compare again */
					}
				} /*if (indexAnzahl[i] != 0)	*/
			}	/*	for( i 	*/
		}	/*	if (indexAnzahl[ j ])	*/
	}/*	for( j 	*/
	
	/*
	 * restore image data from palette
	 */
	Indexed2RGBColours( pd, palette, numColours);

	FREE( palette );		/* Palette-Array wieder frei geben    */
	FREE( indexAnzahl );	/* Farbanzahl-Array wieder frei geben */
	FREE( idx_ptr );	/* */

	return 0;
}