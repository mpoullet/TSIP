/*****************************************************
* Datei: ubwt.h
* Autor: Thomas Schmiedel
* Datum: 02.11.2010
* Desc.: Header für die Burrows-Wheeler-Transformation
* Vers.: 1.0
******************************************************/

#ifndef _BWT_H_
#define _BWT_H_

#include "tstypes.h"

/* Hin- und Rücktransformation */
uint BWTEncode(uint *, unsigned long);
void BWTDecode(uint *, unsigned long, unsigned long);

#endif

