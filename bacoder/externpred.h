/*****************************************************
* Datei: externpred.h
* Autor: Thomas Schmiedel
* Datum: 02.11.2010
* Desc.: DSchnittstelle f�r externe Pr�diktion
* Vers.: 1.0
******************************************************/


#ifndef _EXTERN_PRED_H_
#define _EXTERN_PRED_H_

#include "tstypes.h"
#include "bitbuf.h"
#include "ginterface.h"
// Strutz #include "externpred\prediction.h"
#include "prediction.h"
#include "glue.h" /* for CustomParameter	*/

/* Pr�diktion und Depr�diktion */
void ExternalPred(PicData *, void *);
void ExternalPredInverse(PicData *, void *);

/* Einen bestimtmen Kanal von PicData mit BILD �berschreiben */
void IncludeBILD(PicData *, uint, BILD *);

/* Bestimmten Kanal von PicData nach BILD konvertieren */
BILD *ConvertPD2BILD(PicData *, uint);

/* Header der externen Pr�diktion schreiben und lesen */
void WriteExtHeader(BitBuffer *, uint);
void ReadExtHeader(BitBuffer *, uint, CustomParam *);

#endif

