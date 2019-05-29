/*****************************************************
* Datei: umtf.h
* Autor: Thomas Schmiedel
* Datum: 02.11.2010
* Desc.: Header für Move To Front Codierung
* Vers.: 1.0
******************************************************/

#ifndef _MTF_H_
#define _MTF_H_

#include "tstypes.h"

/* MTF anwenden und rückwandeln */
void MTFEncode(uint *, uint, uint);
void MTFDecode(uint *, uint, uint);

/* Diagnose */
void PrintLookUp(uint *, uint);

#endif

