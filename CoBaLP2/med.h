/************************************************
 *
 * File..:	med.h
 * Descr.:	Definitions for MED prediction
 * Author:	Tilo Strutz
 * Datum: 	19.04.2013
 *
 ************************************************/
#ifndef MED_H
#define MED_H

#include "predictor2.h"
//#include "codec_CoBaLP.h" /* for CLIP() macro	*/

float med_predictor_( TEMPLATE *templ, long min_value, long max_value);

#endif
