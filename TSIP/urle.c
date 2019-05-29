/*****************************************************
* Datei: urle.c
* Desc.: Implementiert die Lauflängencodierung
* Autor: Thomas Schmiedel
* Datum: 02.11.2010, , 5.5.2011 Strutz
******************************************************/
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "urle.h"


/* Lauflängencodierung, "marker" darf nicht
in der Nachricht vorkommen */
unsigned long RLEEncode( uint *arr, unsigned long len, uint marker, uint max_run)
{
	unsigned int *tmp = NULL;
	unsigned long i, rle, pos;
#ifdef DEBUG_RLC
	FILE *out=NULL;
	out = fopen( "rlc_encode.txt", "a");
	fprintf( out, "################ marker: %d\n\n", marker);
#endif
	ALLOC( tmp, len, uint);
	/* Zeichen der Nachricht durchgehen */	
	for (i = 0, pos = 0; i < len; )
	{
		/* Lauflänge zählen */
		for (rle = 1u;			 
			 i + rle < len &&	
			 rle < UINT_MAX && 		
			 //Strutz: limitation of run
			 (rle < max_run) &&  //this is surprisingly worse than  '< marker'
//			 rle < marker &&  this requires deeper investigations
// for some images it is better to concentrate the max. run length 
// to a certain value. Why?
			 arr[i + rle] == arr[i];
			 rle++);
		if (rle >= 4u) /* RLE verwenden */
		{
			/* Markierung, Symbol und Anzahl schreiben */
			tmp[pos++] = marker;			
			tmp[pos++] = arr[i]; /* symbol before run length: important for BWT */
			tmp[pos++] = rle - 4u;
			/* Position in der Nachricht um "rle" weiterrücken */		
			i += rle;
#ifdef DEBUG_RLC
			fprintf( out, "%4d\n", rle);
#endif
		}
		else
		{
			/* Symbol(e) unverändert übernehmen */
			do
			{
				tmp[pos++] = arr[i];
				rle--;
				i++; /* Ein Symbol weiter, bitte */
			}while(rle);
		}	
	}
	/* tmp in arr rückkopieren */
	memcpy( arr, tmp, pos * sizeof(uint));
	FREE( tmp);
#ifdef DEBUG_RLC
			fclose(out);
#endif
	return pos;	
}


/* RLE Dekodierung, "marker" muss mit dem der Codierung
identisch sein. "maxout" ist die maximale Länge, die das Array
nach der RLE-Decodierung einnehmen darf */
unsigned long RLEDecode( uint *arr, unsigned long len, uint marker, uint maxout)
{
	uint i, k, rle, pos,
		 *tmp = NULL;
#ifdef DEBUG_RLC
	FILE *out=NULL;
	out = fopen( "rlc_decode.txt", "a");
	fprintf( out, "################ marker: %d\n\n", marker);
#endif

	ALLOC( tmp, maxout, uint);
	for (i = 0, pos = 0; i < len; )
	{		
		/* Wenn aktuelles Zeichen = "marker", dann... */
		if (arr[i] == marker)
		{
			/* Lauflänge rekonstruieren */
			rle = arr[i + 2u] + 4u;
#ifdef DEBUG_RLC
			fprintf( out, "%4d\n", rle);
#endif
#ifdef Q_DEBUG
	if (rle >= 30)
	{
			pos = pos;
			//fclose(out);
	}
#endif
			/* ...und das Symbol rle-mal ausgeben */
			for (k = 0; k < rle; k++)
			{
				tmp[pos++] = arr[i + 1u];
#ifdef Q_DEBUG
	if (pos >= maxout-10)
	{
			pos = pos;
//			fclose(out);
	}
#endif
				assert( pos <= maxout);
			}
			i += 3u; /* Es wurden gerade 3 Zeichen gelesen */	
		}
		else
		{
			/* Symbol unverändert übernehmen */
			tmp[pos++] = arr[i];
#ifdef Q_DEBUG
	if (pos >= maxout-10)
	{
			pos = pos;
//			fclose(out);
	}
#endif
			i++; /* Nächstes Symbol bitte */
		}
	}	
	/* tmp in arr rückkopieren */
	memcpy( arr, tmp, pos * sizeof(uint));
	FREE( tmp);
#ifdef DEBUG_RLC
	fclose(out);
#endif
	return pos;
}


/*---------------------------------------------------------
 * RLC2_Encode() by David Banko
 * for zeros only
 *-------------------------------------------------------*/
unsigned long RLC2zero_Encode(uint *arr, unsigned long len, uint marker, uint max_run)
{
	unsigned int i, rle, pos, *tmp = NULL;
#ifdef DEBUG_RLC
	FILE *out=NULL;
	out = fopen( "rlc_encode.txt", "a");
	fprintf( out, "################ marker: %d\n\n", marker);
#endif
	ALLOC( tmp, len, uint);
	/* Zeichen der Nachricht durchgehen */	
	for (i = 0, pos = 0; i < len; )
	{
		/* Lauflänge zählen */
		for (rle = 1u;
			 arr[i] == 0 && // Added condition
			 i + rle < len &&	
			 rle < UINT_MAX && 		
			 //Strutz: limitation of run
			 (rle < max_run) && 
			 // Strutz arr[i + rle] == arr[i];
			 arr[i + rle] == 0;
			 rle++);
		if (rle >= 4u) /* RLE verwenden */
		{
			/* Markierung, Symbol und Anzahl schreiben */
			tmp[pos++] = marker;			
			//tmp[pos++] = arr[i]; /* no need, since we only mark runs of zeros */
			tmp[pos++] = rle - 4u;
			/* Position in der Nachricht um "rle" weiterrücken */		
			i += rle;
#ifdef DEBUG_RLC
			fprintf( out, "%4d\n", rle);
#endif
		}
		else
		{
			/* Symbol(e) unverändert übernehmen */
			do
			{
				tmp[pos++] = arr[i];
				rle--;
				i++; /* Ein Symbol weiter, bitte */
			}while(rle);
		}	
	}
	/* tmp in arr rückkopieren */
	memcpy( arr, tmp, pos * sizeof(uint));
	FREE( tmp);
#ifdef DEBUG_RLC
			fclose(out);
#endif
	return pos;	
}


/*---------------------------------------------------------
 * RLC2_Decode() by David Banko
 * for zeros only
 *-------------------------------------------------------*/
unsigned long RLC2zero_Decode(uint *arr, unsigned long len, 
															uint marker, uint maxout)
{
	uint *tmp = NULL;
	unsigned long i, k, rle, pos;

#ifdef DEBUG_RLC
	FILE *out=NULL;
	out = fopen( "rlc_decode.txt", "a");
	fprintf( out, "################ marker: %d\n\n", marker);
#endif

	ALLOC( tmp, maxout, uint);
	for (i = 0, pos = 0; i < len; )
	{		
		/* Wenn aktuelles Zeichen = "marker", dann... */
		if (arr[i] == marker)
		{
			/* Lauflänge rekonstruieren */
			rle = arr[i + 1u] + 4u; // the next number is already the run-length
#ifdef DEBUG_RLC
			fprintf( out, "%4d\n", rle);
#endif
#ifdef Q_DEBUG
	if (rle >= 30)
	{
			pos = pos;
			//fclose(out);
	}
#endif
			/* ...und das Symbol rle-mal ausgeben */
			for (k = 0; k < rle; k++)
			{
				tmp[pos++] = 0; // The symbol is always zero
#ifdef Q_DEBUG
	if (pos >= maxout-10)
	{
			pos = pos;
//			fclose(out);
	}
#endif
				assert( pos <= maxout);
			}
			i += 2u; /* 2 numbers have been read */	
		}
		else
		{
			/* Symbol unverändert übernehmen */
			tmp[pos++] = arr[i];
#ifdef Q_DEBUG
	if (pos >= maxout-10)
	{
			pos = pos;
//			fclose(out);
	}
#endif
			i++; /* Nächstes Symbol bitte */
		}
	}	
	/* tmp in arr rückkopieren */
	memcpy( arr, tmp, pos * sizeof(uint));
	FREE( tmp);
#ifdef DEBUG_RLC
	fclose(out);
#endif
	return pos;
}
