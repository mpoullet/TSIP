/*************************************************************
* Datei: ginterface.h
* Desc.: Header für Bildstruktur und Farbraumtransformationen
* Autor: Thomas Schmiedel
* Datum: 01.11.2010, 24.01.2011 (Strutz), 02.05.2011
* 27.05.2011 Strutz: new colour transforms
* 03.05.2011 Strutz: new colour transform rYUV
* 23.01.2012 Strutz: new prediction mode: LeftNeighbour
* 02.08.2013 YY|uuvv interleaving
* 24.01.2014 differentiation between C_RESET and P_RESET
* 12.03.2014 keep original max values and current (RCT, Prediction)
* 04.07.2014 support of transparency
* 23.01.2015 wA, wB
* 28.07.2015 new: numOfConstantLines[4]
**************************************************************/

#ifndef _GINT_H_
#define _GINT_H_

#include "tstypes.h"

/* Makros für das Lesen und Schreiben von PPMs/PGMs */
#define BUFSIZE 500

#define GETLINE(_dest, _file)				\
do											\
{											\
	fgets(_dest, (BUFSIZE-2), _file);			\
} while (_dest[0] == '#' || _dest[0] == '\n')
/* BUFSIZE minus 2, because p_buf=&buf[2] in *LoadPPM()*/


/* Bildstruktur,
**data ist ein Array von Zeigern auf uint,
welche wiederum Arrays auf die sequenziellen
Farbwerte darstellen,
die Adressierung erfolgt also z.B.:
data[Kanal][Sequenzieller Farbwert] = 0xFFu */
typedef struct
{
	uint width,
		 height,
		 size, //pixels,
		 tileNum,
		 x_tiles, y_tiles,	/* number of horizontal and vertical tiles of original*/
		 channel,
		 channel_alloc,	 /* number of allocated channels, can be larger, 
										  * if alpha channel == 255 is present	*/
		 bitperchannel,
		 bitperchan[4],	/* Strutz 24.01.2011 */
		 //maxVal[3],	/* Strutz 08.07.2011, removed 29.3.14 */
		 maxVal_rct[4],	/* Strutz 29.03.2014 */
		 maxVal_pred[4],
		 maxVal_orig[4],	/* Strutz 12.03.2014 */
		 offset[4],
		 **data;
		int space;		 
		int transparency;	/* 0.. no, 1... binary, 2 ... alpha channel, 3... binary + Grey */
		unsigned int transparency_colour; /*	used for transparency=1	*/

	/* CoBaLP2	*/
	unsigned char wA, wB; /* 10x abs(weight) for predictor2	*/
	unsigned int numOfConstantLines[4]; /* counter of constant rows and lines	*/
} PicData;

/* Allokieren und Löschen der Bildstruktur */
PicData *CreatePicData(uint, uint, uint, uint, uint*);
void DeletePicData( PicData *);

/* Zeiger auf Farbwert nach X,Y-Koordinaten holen */
uint *GetXYValue( PicData *, uint, uint, uint);

/* Laden und Speichern von PPMs/PGMs */
//PicData *LoadPPM(char *);
PicData *LoadPPM( FILE *h_file);
void WritePPM( PicData *, FILE *h_file);

/* Farbraumtransformationen */
void TransformRGB2YrUrVr( PicData *pd);


#endif

