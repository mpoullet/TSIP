/*******************************************************************
 *
 * Datei...: prediction.h
 * Funktion: Prototyping der Prädiktions-Methoden.
 * Autor...: Thomas Schatton
 * Datum...: 26.09.2010
 *
 ********************************************************************/

#ifndef _PREDICTION_H_
#define _PREDICTION_H_

#include "types.h"
#include "bild.h"
#include "parameter.h"
#include "seiteninfo.h"
#include "auswahl.h"
#include "predictor.h"
#include "errordb.h"

/* Bild-Prädiktion */
int predict(BILD **out, BILD *img, PARAMETER *param, 
            SEITENINFO **sInfo, ERRORDB **db, BILD *psImg);

/* Bild-Rekonstruktion */
int reconstruct(BILD **out, BILD *img, PARAMETER *param, 
                SEITENINFO **sInfo, ERRORDB **db);

/* Kontextbasierte Prädiktion/Rekonstruktion */
int contextMode(AUSWAHL *a, PREDICTOR_S *pred);

/* Blockbasierte Prädiktion/Rekonstruktion */
int blockMode( AUSWAHL *a, PREDICTOR_S *pred, BOOL readSInfo, 
               BILD *psImg);

int errorDbMode(AUSWAHL *a, PREDICTOR_S *pred, BILD *psImg);
int selectionTraining( BILD *img, ERRORDB **db);

#endif