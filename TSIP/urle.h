/*****************************************************
* Datei: urle.h
* Desc.: Header für die Lauflängencodierung
* Autor: Thomas Schmiedel
* Datum: 02.11.2010, 17.3.2011 Strutz
******************************************************/

#ifndef _RLE_H_
#define _RLE_H_

#include "tstypes.h"

/* Codierung und Decodierung */
unsigned long RLEEncode(uint *, unsigned long len, uint, uint);
unsigned long RLEDecode(uint *, unsigned long len, uint, uint);
unsigned long RLC2zero_Encode(uint *, unsigned long len, uint, uint);
unsigned long RLC2zero_Decode(uint *, unsigned long len, uint, uint);

#endif

