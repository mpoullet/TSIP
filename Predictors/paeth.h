/*****************************************************
* Datei: paeth.h
* Autor: Thomas Schmiedel
* Datum: 02.11.2010
* Desc.: Header für Paeth-Vorfilter
* Vers.: 1.0
******************************************************/

#ifndef _PAETH_H_
#define _PAETH_H_

#include "tstypes.h"
#include "ginterface.h"

/* Schätzwert errechnen */
MYINLINE int GetPredVal(PicData *, uint, uint, uint);

/* Prädiktor auf Bild anwenden bzw. Rückwandeln */
void PaethFilter(PicData *);
void PaethInverse(PicData *);

#endif

