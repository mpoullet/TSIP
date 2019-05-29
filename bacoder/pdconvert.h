/*****************************************************
* File..: pdconvert.h
* Author: Thomas Schmiedel, T.Strutz
* Date..: 
 * 27.07.2012 Toia-Cret new GetPicDataFromBitmap_fast()
******************************************************/

#ifndef _PDCONVERT_H_
#define _PDCONVERT_H_

using namespace System::Drawing;
#include "tstypes.h"
#include "ginterface.h"

/* Schnittstelle zwischen Bildstruktur und Bitmap-Objekt */
PicData *GetPicDataFromBitmap( Bitmap^);
PicData *GetPicDataFromBitmap_fast( Bitmap^);
Bitmap^ GetBitmapFromPicData( PicData *);
PicData *CopyPicData(PicData *);
void ConvertToGray( PicData *);

#endif
