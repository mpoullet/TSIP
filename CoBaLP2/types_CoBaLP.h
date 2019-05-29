/*****************************************************************
 *
 * Datei:	types_CoBaLP.h
 * Funktion:	Strukturtyp-Definitionen
 * Datum: 	15.06.2001
 * 26.04.2013 inclusion of min_value
 * 11.04.2014 unsigned long for max_value
 *****************************************************************/
#ifndef TYPES_H
#define TYPES_H

typedef struct {
	unsigned int
		width,		/* Bildbreite 	*/
		height;		/* Bildhoehe 	*/
	unsigned long
		size;		/* Anzahl der Bildpunkte	*/
	int	bit_depth;	/* Bittiefe der Daten	*/
	unsigned long	max_value;	/* groester Wert im Bild	*/
	unsigned long	min_value;	/* smallest value in image	*/
	/* if larger values must be supported, 
	 * then transmission of header info must be changed
	 */
} IMAGE;

/*------------------------------------------------------------------*/
#endif

