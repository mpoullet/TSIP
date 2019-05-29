/*****************************************************
* Datei: ginterface.c
* Desc.: Manipulation der Bildstruktur
* Autor: Thomas Schmiedel
* Datum: 01.11.2010, 24.01.2011 (Strutz), 
* 11.3.2011 load_PPM determination of bpc
* 23.3.2011 allow grey-scale images with space==GREY
* 27.05.2011 Strutz: new colour transforms
* 03.05.2011 Strutz: new colour transform rYUV
* 27.11.2012 LoadPPM() Übernahme von Bpc in alle bitperchan[]_Werte
* 02.04.2014 LoadPPM() support P3, check for identity ==> GREY
* 02.07.2014 CreatePicData() initialisation of tileNum
* 04.07.2014 support of transparency
* 02.09.2014 initialisation of maxVal_pred and maxVal_rct equal to maxVal_orig
* 28.07.2015 new: pd->numOfConstantLines[4]
******************************************************/
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "ginterface.h"
//#include "pdconvert.h" Strutz: for some reason, thsi wont be compiled	*/
#include "colour4.h"

extern char* const thisVersion; /* bacoder.cpp	*/

PicData *CopyPicData(PicData *);

/*-------------------------------------------------------------
 * Allokiert eine leere Zeichenfläche im Hauptspeicher
 *----------------------------------------------------------*/
PicData *CreatePicData( uint width, uint height, uint chan, 
											  uint bitperchannel, uint *bitperchan)
{
	uint i;
	PicData *pd = NULL;	
	ALLOC( pd, 1u, PicData);	
	
	/* Attribute setzen */
	pd->width   = width;
	pd->height  = height;	
	pd->size  = width * height;
	pd->channel = chan;
	pd->channel_alloc = chan;
	pd->tileNum = 0; /* default for entire image, is used as decision somewhere	*/
	pd->x_tiles = pd->y_tiles = 1;
	if (pd->channel > 2) pd->space = RGBc;
	else pd->space   = GREY;
	pd->bitperchannel = bitperchannel;
	pd->bitperchan[0] = bitperchan[0];
	pd->bitperchan[1] = bitperchan[1];
	pd->bitperchan[2] = bitperchan[2];
	pd->bitperchan[3] = bitperchan[3];
	pd->offset[0] = 0;
	pd->offset[1] = 0;
	pd->offset[2] = 0;
	pd->offset[3] = 0;

	pd->transparency = 0;	/* no transparency*/
	pd->transparency_colour =0;
		
	/* Speicher belegen,
	zuerst Kanäle erzeugen und dann pro Kanal 
	ein Array für die Farbwerte anfordern und 
	mit 0 füllen */
	ALLOC( pd->data, chan, uint*);
	for (i = 0; i < chan; i++)
	{
		ALLOC( pd->data[i], pd->size, uint);	
		memset( pd->data[i], 0, pd->size * sizeof(uint));
	}	
	pd->numOfConstantLines[0] = 0;
	pd->numOfConstantLines[1] = 0;
	pd->numOfConstantLines[2] = 0;
	pd->numOfConstantLines[3] = 0;
	
	return pd;
}


/*-------------------------------------------------------------
 * Bildstruktur freigeben 
 *----------------------------------------------------------*/
void DeletePicData( PicData *pd)
{
	uint i;
	for (i = 0; i < pd->channel_alloc; i++)
	{
		FREE( pd->data[i]);
	}	
	FREE( pd->data);
	FREE( pd);
}


/*-------------------------------------------------------
 * X,Y-Koordinaten in sequenziellen Wert umrechnen und
 * einen Pointer auf den Farbwert an dieser Stelle beschaffen
 *----------------------------------------------------------- */
uint *GetXYValue(PicData *pd, uint chan, uint x, uint y)
{
	uint *ptr = NULL;
	ptr = pd->data[chan] + y * pd->width + x;
	return ptr;
}



/*-------------------------------------------------------------
 * Lädt eine PPM/PGM-Datei und befüllt die Bildstruktur damit 
 *----------------------------------------------------------*/
PicData *LoadPPM( FILE *h_file)
{	 
	PicData *pd  = NULL;
	//FILE *infile = NULL;	
	char buf[BUFSIZE], *p_buf;
	int val, val1, val2, identity, format;
	unsigned int uival[5], cnt = 0;

	uint i, k, w, h, 
		 channel,
		 maxVal,
		 bpc;
	uint tmp[4];

	/* Datei öffnen und auf Fehler reagieren */
	//infile = fopen( file, "rb");
	//if (infile == NULL)
	//{
	//	fprintf(stderr, "failed to open file: %s\n", file);
	//	ERRCODE = PPM_OPEN_FAIL;
	//	return NULL;
	//}
		
	/* Erste Zeile lesen, P6 entspricht einer Pixmap (3 Kanäle),
	P5 einer Graymap (1 Kanal) */
	GETLINE( buf, h_file);
	if (strncmp( buf, "P6", 2) == 0)
	{
		channel = 3;
		format = 6;
	}
	else if(strncmp( buf, "P5", 2) == 0)
	{
		channel = 1;
		format = 5;
	}
	else if(strncmp( buf, "P3", 2) == 0)
	{
		channel = 3;
		format = 3;
	}
	else if(strncmp( buf, "P2", 2) == 0)
	{
		channel = 1;
		format = 2;
	}
	else
	{
		//fprintf( stderr, "%s is not a valid raw pixmap/graymap file\n", file);
		fprintf( stderr, "This is not a valid raw pixmap/graymap file\n");
		ERRCODE = PPM_INVALID;
		return NULL;
	}		
	
	/* Breite, Höhe und größen Farbwert holen */
	p_buf = &buf[2];
	while (cnt < 3)
	{
		val = sscanf( p_buf, "%u %u %u", &uival[cnt], &uival[cnt+1], &uival[cnt+2]);
		if ( val == EOF)
		{
  		GETLINE( p_buf, h_file);
			val = 0;
		}
		else if ( val < 3-(signed)cnt)
		{
  		GETLINE( p_buf, h_file);
		}
		cnt += val;
	}
	w = uival[0];
	h = uival[1];
	maxVal = uival[2];

	/* Bit per Channel berechnen, 
	gleichbedeutend mit Logarithmus Dualis */
	for (bpc = 1u;
		 bpc < 16u &&
		 (1u << bpc) <= maxVal; 
		 bpc++);	

	/* Zeichenfläche im Speicher anlegen */
	tmp[0] = tmp[1] = tmp[2] = bpc;
	tmp[3] = 0;
	pd = CreatePicData( w, h, channel, bpc, tmp);
	pd->maxVal_orig[0] = pd->maxVal_pred[0] = pd->maxVal_rct[0] = maxVal;
	if (channel == 3)
	{
		pd->maxVal_orig[1] = pd->maxVal_pred[1] = pd->maxVal_rct[1] = maxVal;
		pd->maxVal_orig[2] = pd->maxVal_pred[2] = pd->maxVal_rct[2] = maxVal;
	}
	identity = 1;
	if (format == 2)
	{
		/* Loop zum Kopieren der Bildinformationen */
		for (i = 0; i < pd->size; i++)
		{		
			fscanf( h_file, "%u ", &val); /* read ASCII text	*/
			pd->data[0][i] = (uint)val;
		}
	}
	else 	if (format == 3)
	{
		/* Loop zum Kopieren der Bildinformationen */
		for (i = 0; i < pd->size; i++)
		{		
			fscanf( h_file, "%u %u %u ", &val, &val1, &val2); /* read ASCII text	*/
			pd->data[0][i] = (uint)val;
			pd->data[1][i] = (uint)val1;
			pd->data[2][i] = (uint)val2;
			if ( (val != val1) || (val != val2) ) identity = 0;
		}
	}
	else
	{
		/* Loop zum Kopieren der Bildinformationen */
		for (i = 0; i < pd->size; i++)
		{		
			for (k = 0; k < channel; k++)
			{
				if (bpc <= 8u) /*Ein Byte pro Kanal in Datei verwendet */
				{
					pd->data[k][i] = (uint)abs( fgetc( h_file)); 		
				}
				else /*Zwei Bytes pro Kanal in Datei verwendet */
				{
					pd->data[k][i] = 0;
					pd->data[k][i] |= (byte)abs( fgetc( h_file)) << 8u;
					pd->data[k][i] |= (byte)abs( fgetc( h_file));			
				}
			
				/* Die Datei muss mindestens so viele Farbwerte anthalten,
				wie Höhe * Breite vorgibt */
				if (feof( h_file))
				{
					//fprintf( stderr, "%s is damaged\n", file);
					fprintf( stderr, "File is damaged\n");
					ERRCODE = PPM_DAMAGED;
					return NULL;
				}		
			}
			if ( channel == 3 && 
				  ((pd->data[0][i] != pd->data[1][i]) || 
					 (pd->data[0][i] != pd->data[2][i])) ) 
					 identity = 0;
		}	
	}
	
	/* Nun sollte die Datei am Ende sein, 
	ist sie es nicht, so stehen zuviele 
	Farbwerte in der Datei. Es kann aber
	trotzdem fortgefahren werden */
	//if (fgetc(infile) != EOF)
	//{
	//	fprintf( stderr, "%s is damaged, continuing anyway...\n", file);
	//}	
	
	if (channel == 3 && identity)
	{
		/* create a new image with only one channel	*/
		PicData *pd2 = NULL;
		/* fake the number of channels, so that only first channel is copied */
		pd->channel = 1; 
		pd2 = CopyPicData( pd);
		DeletePicData( pd);
		pd = pd2;
	}

	//fclose(infile);
	return pd;
}


/*-------------------------------------------------------------
 * Schreibt die Bildstruktur samt Inhalt als PPM/PGM 
 *----------------------------------------------------------*/
void WritePPM(PicData *pd, FILE *h_file)
{
	//FILE *outfile = NULL;
	uint i, k, val;

	/* Datei im Schreibmodus öffnen */
	//outfile = fopen(filename, "wb");
	//if (outfile == NULL)
	//{
	//	fprintf(stderr, "failed to open file: %s\n", filename);
	//	ERRCODE = PPM_OPEN_FAIL;
	//	return;
	//}
	
	/* Enthält das Bild nur einen Kanal,
	so wird es als PGM gespeichert */ 
	if (pd->channel == 1u)
	{
		fprintf( h_file, "P5\n");
	}
	else
	{
		fprintf( h_file, "P6\n");
	}
	
	/* Header schreiben */
	fprintf( h_file, "# Created by %s, HfTL\n", thisVersion);
	fprintf( h_file, "%u %u\n", pd->width, pd->height);
	fprintf( h_file, "%u\n", (1u << pd->bitperchannel) - 1u);
		
	/* Farbwerte speichern */	
	for (i = 0; i < pd->size; i++)
	{
		for (k = 0; k < pd->channel; k++)
		{
			val = pd->data[k][i];			
			if (pd->bitperchannel <= 8u) /* Nur ein Byte pro Kanal verwenden */
			{
				fputc( val, h_file);		
			}
			else /* 2 Byte pro Kanal, 16 Bit separat speichern */
			{
				fputc((val & 0xFF00u) >> 8u, h_file);
				fputc( val & 0x00FFu, h_file);	
			}		
		}	
	}		

	//fclose(outfile);
}


/*-------------------------------------------------------------
 * Farbraumtransformation RGB nach YUVr (verlustfrei),
 * der Wertebereich erweitert sich für Ur und Vr um jeweils 1 Bit
 *--------------------------------------------------------------*/
void TransformRGB2YrUrVr( PicData *pd)
{
	uint i, Yr, Ur, Vr, addval;
		
	if (pd->channel < 3u || pd->space != RGBc) return;	
	addval = (1u << pd->bitperchannel) - 1u; /* Verschiebung in positiven Bereich */
	
	for (i = 0; i < pd->size; i++)
	{				
		Yr = (pd->data[0][i] + (pd->data[1][i] << 1) + pd->data[2][i]) >> 2;
		if (Yr > 255)
		{
			i=i;
		}
		Ur =  pd->data[0][i] -  pd->data[1][i] + addval;
		Vr =  pd->data[2][i] -  pd->data[1][i] + addval;		
		pd->data[0][i] = Yr;
		pd->data[1][i] = Ur;
		pd->data[2][i] = Vr;	
	}
	/* Wertebereich erhöhen */	
	pd->bitperchannel++;
	pd->bitperchan[1]++; /* Y keeps its range */
	pd->bitperchan[2]++;
	pd->space = A7_1;
}

