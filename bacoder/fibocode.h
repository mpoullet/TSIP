/*****************************************************
* Datei: fibocode.h
* Autor: Thomas Schmiedel
* Datum: 01.11.2010
* Desc.: Header für die Fibonacci-Codierung
* Vers.: 1.0
******************************************************/

#ifndef _FIBOCODE_H_
#define _FIBOCODE_H_

#include "tstypes.h"
#include "bitbuf.h"

/* Container für ein Fibonacci-Codemuster */
typedef struct
{
	cbool *code;
	uint  count;
} FiboCode;

/* Codierung und Decodierung */
FiboCode *WriteFibo(uint);
uint RestoreFiboNum(FiboCode *);
void DeleteFibo(FiboCode *);

/* Interface für bitbuf-Streamzugriff */
void WriteFiboToStream( BitBuffer *, uint);
uint GetFiboFromStream( BitBuffer *);

#endif

