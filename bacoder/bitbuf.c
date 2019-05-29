/*********************************************************************
* File   	: bitbuf.c
* Purpose	: to write compressed image data to a file;
*		  	  to read this data likewise
* Author 	: Thomas Schmiedel
* Dates  	: 02.08.2010, 20.04.2011 Strutz
* 05.05.2011 Strutz rice_coding added
*	05.05.2011 Strutz global variable numBytes
* 08.11.2013 bugfix, numBytes++ appears too often
*	18.10.2013 Roeppischer: changed file access option from
*		   	  'wb' to 'wb+' to have read access when writing;
*	18.10.2013 Roeppischer: catched EOF exception when
*		   	  reading in new byte from file
* Last Modif: 01.02.2014
*********************************************************************/

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bitbuf.h"

unsigned long numBytes;	/* access to the number of written bytes	*/

/*** Öffne Bitstrom zum Lesen ***/
BitBuffer *BufferOpenRead_old( char *filename)
{
	/* Buffer-Objekt erzeugen */
	BitBuffer *result = (BitBuffer *)malloc( sizeof(BitBuffer));
	if (result == NULL) return NULL;

	/* Stream öffnen */
	result->file = fopen(filename, "rb");
	if (result->file == NULL)
	{
		free( result);
		return NULL;
	}

	/* Attribute festlegen und Buffer setzen */
	result->iswrite = FALSE;
	result->buffer  = 0x00u;
	result->bitpos  = 8u;

	return result;
}

/*** Öffne Bitstrom zum Lesen ***/
BitBuffer *BufferOpenRead( FILE *h_file)
{
	/* Buffer-Objekt erzeugen */
	BitBuffer *result = (BitBuffer *)malloc( sizeof(BitBuffer));
	if (result == NULL) return NULL;

	/* Stream öffnen */
	result->file = h_file;

	/* Attribute festlegen und Buffer setzen */
	result->iswrite = FALSE;
	result->buffer  = 0x00u;
	result->bitpos  = 8u; /* enforces reading a new byte	at start */

	return result;
}


/*** Öffne Bitstrom zum Schreiben ***/
BitBuffer *BufferOpenWrite( FILE *h_file)
{
	/* Buffer-Objekt anlegen */
	BitBuffer *result = (BitBuffer *)malloc( sizeof(BitBuffer));
	if (result == NULL) return NULL;

	/* Stream öffnen */
	result->file = h_file;

	/* Atrribute und Buffer setzen */
	result->iswrite = TRUE;
	result->buffer  = 0x00u;
	result->bitpos  = 0u;

	numBytes = 0; /* reset	*/
	return result;
}


/*** close bitstream, only for encoder (flushing) ***/
void BufferClose( BitBuffer *bitbuf)
{
	/* Stream überhaupt offen? */
	if (bitbuf == NULL || bitbuf->file == NULL) return;

	/* Falls im Schreibmodus und noch Restdaten im Buffer */
	if (bitbuf->iswrite == TRUE && bitbuf->bitpos != 0)
	{
		putc( bitbuf->buffer, bitbuf->file);
		numBytes++;
	}

	/* Schließen und freigeben */
	//fclose( bitbuf->file);
	free( bitbuf);
}


/*** Bit schreiben ***/
void BufferPutBit( BitBuffer *bitbuf, cbool abit)
{
	/* Offener Schreibstream? */
	if (bitbuf == NULL ||
		bitbuf->iswrite == FALSE ||
		bitbuf->file == NULL)
		return;

	/* Setze Bit im Buffer an der Stelle bitpos */
	if (abit == TRUE)
	{
		bitbuf->buffer |= (1u << bitbuf->bitpos);
	}
	else
	{
		bitbuf->buffer &= ~(1u << bitbuf->bitpos);
	}
	bitbuf->bitpos++;

	/* Wenn Buffer voll, schreibe des Byte in Datei */
	if (bitbuf->bitpos == 8u)
	{
		putc( (int)bitbuf->buffer, bitbuf->file);
		bitbuf->bitpos = 0;
		// memset( &bitbuf->buffer, 0x00, sizeof(byte));
		bitbuf->buffer = 0;
		numBytes++;	/* increment byte counter	*/
	}
}


/*** Hole Bit ***/
cbool BufferGetBit( BitBuffer *bitbuf)
{
	cbool result;
	int temp;

	/* Korrekter Lese-Stream? */
	if (bitbuf == NULL ||
		bitbuf->iswrite == TRUE ||
		bitbuf->file == NULL)
		return FALSE;

	/* Buffer leer?, dann Byte holen */
	if (bitbuf->bitpos == 8u)
	{
		temp = fgetc( bitbuf->file);

		/* return 0-bit if decoder asks too much */
		if(temp == EOF) return false;

		bitbuf->buffer = (byte)abs( temp);
		bitbuf->bitpos = 0;
	}

	/* Hole aktuelles Bit */
	if (bitbuf->buffer & (1u << bitbuf->bitpos))
	{
		result = TRUE;
	}
	else
	{
		result = FALSE;
	}
	bitbuf->bitpos++;

	return result;
}

void BufferReadAlign( BitBuffer *bitbuf)
{
	/* Korrekter Lese-Stream? */
	if (bitbuf == NULL ||
		bitbuf->iswrite == TRUE )
		return;

	/* buffer not empty?, skip remaining bits  */
	if (bitbuf->bitpos != 8u)
	{
		bitbuf->bitpos = 8;
	}
}

/*** Byte in Stream schreiben ***/
void BufferPutByte( BitBuffer *bitbuf, byte abyte)
{
	uint i;

	/* Korrekter Schreib-Stream? */
	if (bitbuf == NULL ||
		bitbuf->iswrite == FALSE ||
		bitbuf->file == NULL)
		return;

	/* Byte bitweise schreiben */
	for (i = 0; i < 8u; i++)
		BufferPutBit( bitbuf, (abyte >> i) & 1u);
}

/*** Holt Byte aus dem Strom ***/
byte BufferGetByte( BitBuffer *bitbuf)
{
	uint i;
	byte result = 0x00u;

	/* Korrekter Lese-Stream? */
	if (bitbuf == NULL ||
		bitbuf->iswrite == TRUE ||
		bitbuf->file == NULL)
		return 0x00u;

	/* Byte bitweise zusammenbauen */
	for (i = 0; i < 8u; i++)
		result |= (BufferGetBit(bitbuf) << i);

	return result;
}

void BufferPutWord( BitBuffer *bitbuf, uword wrd)
{
	uint i;

	/* Korrekter Schreib-Stream? */
	if (bitbuf == NULL ||
		bitbuf->iswrite == FALSE ||
		bitbuf->file == NULL)
		return;

	/* Byte bitweise schreiben */
	for (i = 0; i < 16u; i++)
		BufferPutBit(bitbuf, (wrd >> i) & 1u);
}

uword BufferGetWord(BitBuffer *bitbuf)
{
	uint i;
	uword result = 0x0000u;

	/* Korrekter Lese-Stream? */
	if (bitbuf == NULL ||
		bitbuf->iswrite == TRUE ||
		bitbuf->file == NULL)
		return 0x0000u;

	/* Byte bitweise zusammenbauen */
	for (i = 0; i < 16u; i++)
		result |= (BufferGetBit(bitbuf) << i);

	return result;
}

/*----------------------------------------------------------------
* WriteValToStream( )
*   write binary number into stream
*----------------------------------------------------------------*/
void WriteValToStream( BitBuffer *bitbuf, uint number, uint num_bits)
{
	uint i;

	/* start with least significant bit	*/
	for (i = 0; i < num_bits; i++)
	{
		BufferPutBit( bitbuf, number & 0x01);
		number >>= 1; /* next bit	*/
	}
	/* if number > 0, then num_bits was too small	*/
	assert( number == 0);
}


/*----------------------------------------------------------------
* ReadValFromStream( )
*   write binary number into stream
*----------------------------------------------------------------*/
unsigned int ReadValFromStream( BitBuffer *bitbuf,
															 unsigned int num_bits)
{
	unsigned int i;
	unsigned int result = 0x00u;

	/* Korrekter Lese-Stream? */
	if (bitbuf == NULL ||
		bitbuf->iswrite == TRUE ||
		bitbuf->file == NULL)
		return 0x00u;

	/* create value */
	for (i = 0; i < num_bits; i++)
	{
		result |= ( BufferGetBit( bitbuf) << i );
	}

	return result;
}


/*---------------------------------------------------------------
*	rice_encoding()
*---------------------------------------------------------------*/
void rice_encoding( BitBuffer *bitbuf, unsigned long value, int k)
{
	register unsigned long mask, remainder;

	remainder = value >> k; /* get most significant bits */
	{
		/* unary coding of 'remainder' */
		while (remainder) { remainder--; BufferPutBit( bitbuf, 0);}
		BufferPutBit( bitbuf, 1);	/* terminating '1' */

		/* send k least significant bits	of 'value' */
		for ( mask = (1 << k) >> 1; mask; mask >>= 1)
		{
			/* use mask for indication of bit position */
			if (value & mask)	BufferPutBit( bitbuf, 1);
			else 		BufferPutBit( bitbuf, 0);
		}
	}
}

/*---------------------------------------------------------------
*	rice_decoding()
*---------------------------------------------------------------*/
unsigned long rice_decoding( BitBuffer *bitbuf, int k)
{
	register unsigned long r, mask;
	unsigned long 	 value;

	/* read zeros (unary code with terminating '1' */
	for (r = 0; !BufferGetBit( bitbuf); r++)
		;

	/* read k least significant bits of 'value' */
	for (value = 0, mask = (1 << k) >> 1; mask; mask >>= 1)
	{
		if (BufferGetBit( bitbuf)) value |= mask;
	}

	value += (r << k); /* combine with most significant bits */

	return value;
}
