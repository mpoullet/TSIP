/*****************************************************
* Datei: bitbuf.h
* Autor: Thomas Schmiedel
* Datum: 02.08.2010
* Desc.: Header für Binärzugriffs-Interface
* Vers.: 1.0
* 05.05.2011 Strutz new functions for rice coding
* 05.05.2011 Strutz global variable numBytes
* 01.07.2014 new: BufferReadAlign()
******************************************************/

#ifndef _BITBUFFER_H_
#define _BITBUFFER_H_

#include <stdio.h>
#include "tstypes.h"

extern unsigned long numBytes;	/* access to the number of written bytes	*/

/*** Das Stream-Objekt ***/
typedef struct
{
    FILE  *file;
    cbool iswrite;
    byte  buffer;
    uint  bitpos;
} BitBuffer;

/*** Stream öffnen und schließen, r, w ***/
BitBuffer *BufferOpenRead( FILE * h_file);
BitBuffer *BufferOpenWrite(FILE * h_file);
void BufferClose(BitBuffer *);  

/*** Input und Output ***/
void  BufferPutBit(BitBuffer *, cbool);
cbool BufferGetBit(BitBuffer *);
void  BufferPutByte(BitBuffer *, byte);
byte  BufferGetByte(BitBuffer *);
void  BufferPutWord(BitBuffer *, uword);
uword BufferGetWord(BitBuffer *);

void WriteValToStream( BitBuffer *bitbuf, 
											unsigned int number, 
											unsigned int num_bits);
unsigned int ReadValFromStream( BitBuffer *bitbuf, 
															 unsigned int num_bits);
void rice_encoding( BitBuffer *bitbuf, unsigned long value, int k);
unsigned long rice_decoding( BitBuffer *bitbuf, int k);
void BufferReadAlign( BitBuffer *bitbuf);

#endif

