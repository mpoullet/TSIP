/*****************************************************
* Datei: uwfc.h
* Autor: Thomas Schmiedel
* Datum: 02.11.2010
* Desc.: Header für Weighted Frequency Count
* changes:
* 23.01.2012 separate reset_val an max_weigth for counts rescaling
******************************************************/

#ifndef _WFC_H_
#define _WFC_H_

#include "tstypes.h"

/* WFC anwenden und rückwandeln */
void WFCEncode(uint *, uint, uint reset_val, uint max_weight);
void WFCDecode(uint *, uint, uint reset_val, uint max_weight);

/* Diagnose */
void PrintWeights(uint *, uint *, uint);

#endif

