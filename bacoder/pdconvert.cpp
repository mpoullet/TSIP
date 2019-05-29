/*****************************************************
* Datei: pdconvert.cpp
* Desc.: Convertierungen zwischen PicData und Bitmap
* Autor: Thomas Schmiedel, T.Strutz
* Datum: 14.12.2010, 8.3.2011
* 23.3.2011 check, whether grey-scale images was loaded
 * 08.07.2011 copy also max values of each channel
 * 27.07.2012 Toia-Cret new GetPicDataFromBitmap_fast()
 * 27.11.2012 Strutz: bugfix shift calculation 
 * 12.03.2014 Strutz: assert( rgb < 256) 
 * 12.03.2014 initialise maxVal[] 
 * 01.07.2014 *CopyPicData( ) 'tileNum' added
* 04.07.2014 support of transparency
* 02.09.2014 initialisation of maxVal_pred and maxVal_rct equal to maxVal_orig
******************************************************/
#include "pdconvert.h"
#include "ginterface.h"
#include "colour4.h"
#include <string.h>
#include <assert.h>

/* Toia-Cret start*/
#using <System.Drawing.dll>

using namespace System::Drawing;
using namespace System::Drawing::Imaging;
/* Toia-Cret end*/

/*--------------------------------------------------
 * Bitmap in PicData konvertieren 
 *--------------------------------------------------*/
PicData *GetPicDataFromBitmap(Bitmap^ bmp)
{
	PicData *result = NULL;
	uint x, y, i;
	unsigned char identity;
	Color pc;
	uint tmp[] = {8u, 8u, 8u};  // Strutz 

	/* check identity of components, Grey scale ? */
	identity = 1;
	for (y = 0; y < (uint)bmp->Height; y++)
	{
		for (x = 0; x < (uint)bmp->Width; x++)
		{
			pc = bmp->GetPixel(x, y);
			if ( (pc.R != pc.G) || (pc.R != pc.B) || (pc.B != pc.G) )
			{
				identity = 0;
				break;
			}
		}
	}

	if (identity)
	{
		/* create empty image */
		result = CreatePicData( (uint)bmp->Width,
								 (uint)bmp->Height,
								 1u, 8u, tmp);
		/* copy intensities */
		i = 0;
		for (y = 0; y < (uint)bmp->Height; y++)
		{
			for (x = 0; x < (uint)bmp->Width; x++)
			{
				pc = bmp->GetPixel(x, y);
				result->data[0][i] = (uint)pc.R;
				i++;
			}
		}
	}
	else
	{
		/* create empty image */
		result = CreatePicData( (uint)bmp->Width,
								 (uint)bmp->Height,
								 3u, 8u, tmp);
		/* Farbwerte kopieren */
		i = 0;
		for (y = 0; y < (uint)bmp->Height; y++)
		{
			for (x = 0; x < (uint)bmp->Width; x++)
			{
				pc = bmp->GetPixel(x, y);
				result->data[0][i] = (uint)pc.R;
				result->data[1][i] = (uint)pc.G;
				result->data[2][i] = (uint)pc.B;
				i++;
			}
		}
	}
	return result;
}

/*----------------------------------------------------------
 * PicData in Bitmap konvertieren
 *----------------------------------------------------------*/
Bitmap^ GetBitmapFromPicData( PicData *pd)
{
	Bitmap^ bmp = gcnew Bitmap( pd->width, pd->height);
	Color pc;
	int flag = 1;
	uint x, y, shift;
	register unsigned int r, g, b, a;
	unsigned long pos;

	/* Bitmap kann nur 8Bit pro Kanal speichern,
	enthält PicData mehr, muss um Differenzbetrag
	geshiftet werden */
	if (pd->bitperchannel > 8)
	{
		shift = pd->bitperchannel - 8u;
	}
	else
	{
		shift = 0;
	}

	/* Farbwerte kopieren */
	pos = 0;
	if (pd->channel == 1u)
	{
		pd->space = GREY;
		for (y = 0; y < pd->height; y++)
		{
			for (x = 0; x < pd->width; x++)
			{
				/* Farbwerte auf 8 Bit verringern, falls nötig */
				r = pd->data[0][pos++] >> shift;

				/* In Bitmap schreiben */
				pc = pc.FromArgb( r, r, r);			
				bmp->SetPixel( x, y, pc);
				// if (maxR < r) maxR = r;	/* get maxvalues	*/
			}
		}
	}
	else if (pd->channel == 2u) /* Grey plus alpha channel	*/
	{
		for (y = 0; y < pd->height; y++)
		{
			for (x = 0; x < pd->width; x++)
			{
				/* Farbwerte auf 8 Bit verringern, falls nötig */
				
				r = pd->data[0][pos] >> shift;
				a = pd->data[1][pos++] >> shift;

				assert( r < 256);
				assert( a < 256);

				if (r > 255) r = 255;
				if (a > 255) a = 255;
				/* In Bitmap schreiben */
				pc = pc.FromArgb( a, r, r, r);			
				bmp->SetPixel( x, y, pc);
			}
		}
	}
	else if (pd->channel == 3u)
	{
		for (y = 0; y < pd->height; y++)
		{
			for (x = 0; x < pd->width; x++)
			{
				/* Farbwerte auf 8 Bit verringern, falls nötig */
				
				r = pd->data[0][pos] >> shift;
				g = pd->data[1][pos] >> shift;
				b = pd->data[2][pos++] >> shift;

				//assert( r < 256);
				//assert( g < 256);
				//assert( b < 256);
				if (r > 255)
				{
					r = 255;
					if (flag)
					{
						fprintf( stderr, "\n !!! reconstruction failed !!!\n");
						flag = 0;
					}
				}
				if (g > 255)
				{
					g = 255;
					if (flag)
					{
						fprintf( stderr, "\n !!! reconstruction failed !!!\n");
						flag = 0;
					}
				}
				if (b > 255)
				{
					b = 255;
					if (flag)
					{
						fprintf( stderr, "\n !!! reconstruction failed !!!\n");
						flag = 0;
					}
				}
				/* In Bitmap schreiben */
				pc = pc.FromArgb( r, g, b);			
				bmp->SetPixel( x, y, pc);
			}
		}
	}
	else if (pd->channel == 4u)
	{
		for (y = 0; y < pd->height; y++)
		{
			for (x = 0; x < pd->width; x++)
			{
				/* Farbwerte auf 8 Bit verringern, falls nötig */
				
				r = pd->data[0][pos] >> shift;
				g = pd->data[1][pos] >> shift;
				b = pd->data[2][pos] >> shift;
				a = pd->data[3][pos++] >> shift;

				assert( r < 256);
				assert( g < 256);
				assert( b < 256);
				assert( a < 256);
				if (r > 255) r = 255;
				if (g > 255) g = 255;
				if (b > 255) b = 255;
				if (a > 255) a = 255;
				/* In Bitmap schreiben */
				pc = pc.FromArgb( a, r, g, b);			
				bmp->SetPixel( x, y, pc);
			}
		}
	}
	return bmp;
}

/*--------------------------------------------------
 * New method for opening images using .NET classes.
 * If pixelformat is not supported, it passes the work
 * on to GetPicDataFromBitmap
 *--------------------------------------------------*/
PicData* GetPicDataFromBitmap_fast( Bitmap^ bmp)
{
	int bytespp;
	unsigned char identity = 1, r, g, b, a=0;
	unsigned int maxR=0, maxG=0, maxB=0, maxA=0;
	unsigned int tmp[] = {8u, 8u, 8u, 8u}; //Strutz, bitdepth
	unsigned int *rgbValues;
	unsigned long pos;
	PicData *result = NULL;

	if (bmp->PixelFormat != PixelFormat::Format24bppRgb &&
			bmp->PixelFormat != PixelFormat::Format32bppArgb &&
			bmp->PixelFormat != PixelFormat::Format32bppRgb)
	{
		//If format not supported, use normal function
		return GetPicDataFromBitmap( bmp);
	}

	if (bmp->PixelFormat == PixelFormat::Format24bppRgb)
	{
		bytespp = 3;
	}
	else if ( bmp->PixelFormat == PixelFormat::Format32bppRgb ||	/* Strutz bugfix */
						bmp->PixelFormat == PixelFormat::Format32bppArgb)
	{
		bytespp = 4;
	}
	else 		bytespp = 3;

	BitmapData^ bmpData = bmp->LockBits(Rectangle(0, 0, bmp->Width, bmp->Height),
		ImageLockMode::ReadOnly, bmp->PixelFormat);

	rgbValues = (unsigned int*)bmpData->Scan0.ToPointer();

	for (pos = 0; pos < (unsigned)bmp->Height * bmp->Width; pos++)
	{
		r = (rgbValues[pos] >> 16) & 0xFF;
		g = (rgbValues[pos] >> 8) & 0xFF;
		b = rgbValues[pos] & 0xFF;

		if (identity && ((r != g) || (r != b) || (b != g)) )
		{
				identity = 0; /* it is not a greyscale image	*/
				break;
		}
	}

	/* create a proper PicDate structure	*/
	if (identity) /* is a greyscale image */
	{
		if (bytespp == 3)
		{ /* no alpha channel	*/
			tmp[1] = tmp[2] = tmp[3] = 0;
			result = CreatePicData( (uint)bmp->Width,
			 (uint)bmp->Height, 1u, 8u, tmp);
		}
		else
		{ /*  alpha channel	*/
			tmp[2] = tmp[3] = 0;
			result = CreatePicData( (uint)bmp->Width,
			 (uint)bmp->Height, 2u, 8u, tmp);
		}
	}
	else
	{
		if (bytespp == 3)
		{ /* no alpha channel	*/
			tmp[3] = 0;
			result = CreatePicData( (uint)bmp->Width,
			 (uint)bmp->Height, 3u, 8u, tmp);
		}
		else
		{ /*  alpha channel	*/
			result = CreatePicData( (uint)bmp->Width,
			 (uint)bmp->Height, 4u, 8u, tmp);
		}
	}

	for (pos = 0; pos < (unsigned)bmp->Height * bmp->Width; pos++)
	{
		r = (rgbValues[pos] >> 16) & 0xFF;
		g = (rgbValues[pos] >> 8) & 0xFF;
		b = rgbValues[pos] & 0xFF;

		result->data[0][pos] = r;
		if (!identity)
		{
			result->data[1][pos] = g;
			result->data[2][pos] = b;
		}
		if (bytespp == 4) /* there was a fourth channel	*/
		{
			a = (rgbValues[pos] >> 24) & 0xFF;
			if (identity)	result->data[1][pos] = a;
			else					result->data[3][pos] = a;
		}
		if (maxR < r) maxR = r;	/* get maxvalues	*/
		if (maxG < g) maxG = g;
		if (maxB < b) maxB = b;
		if (maxA < a) maxA = a;
	}

	//result->maxVal[0] = maxR;
	//result->maxVal[1] = maxG;
	//result->maxVal[2] = maxB;
	result->maxVal_orig[0] = result->maxVal_pred[0] = result->maxVal_rct[0] = maxR;
	result->maxVal_orig[1] = result->maxVal_pred[1] = result->maxVal_rct[1] = maxG;
	result->maxVal_orig[2] = result->maxVal_pred[2] = result->maxVal_rct[2] = maxB;
	result->maxVal_orig[3] = result->maxVal_pred[3] = result->maxVal_rct[3] = maxA;
		
	bmp->UnlockBits( bmpData);
	return result;
}

/*------------------------------------------------------------
 * *CopyPicData()
 * PicData kopieren
 *------------------------------------------------------------*/
PicData *CopyPicData( PicData *pd)
{
	PicData *newpd = NULL;
	uint i;

	newpd = CreatePicData( pd->width, pd->height, pd->channel, 
		pd->bitperchannel, pd->bitperchan);

	for (i = 0; i < pd->channel; i++)
	{
		memcpy( newpd->data[i], pd->data[i], pd->size * sizeof(uint));
		newpd->offset[i] = pd->offset[i];
		newpd->maxVal_orig[i] = pd->maxVal_orig[i];
		newpd->maxVal_rct[i] = 0;
		newpd->maxVal_pred[i] = 0;
		newpd->numOfConstantLines[i] = pd->numOfConstantLines[i];
	}
	for (i = pd->channel; i < 4; i++)
	{
		newpd->maxVal_orig[i] = 0;
		newpd->maxVal_rct[i] = 0;
		newpd->maxVal_pred[i] = 0;
		newpd->offset[i] = 0;
	}
	newpd->tileNum = pd->tileNum;
	newpd->x_tiles = pd->x_tiles;
	newpd->y_tiles = pd->y_tiles;
	newpd->space = pd->space;
/*Anfang Team Hannover PNG-Transparenz*/
	newpd->transparency	= pd->transparency;
	if (newpd->transparency == 3)
	{
		newpd->transparency_colour = pd->transparency_colour;
	}
/*Ende Team Hannover PNG-Transparenz*/
	return newpd;
}


/* RGB-PicData in Graustufe verwandeln */
void ConvertToGray( PicData *pd)
{
	if (pd->channel < 3u) return;	
	TransformRGB2YrUrVr( pd);
	pd->bitperchannel--;
	pd->space = GREY;
	FREE( pd->data[1]);
	FREE( pd->data[2]);
	pd->channel = 1;
}

