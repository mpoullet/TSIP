/*****************************************************
 * File..: tile.h
 * Descr.: Header for tiling of images
 * Author: Tilo  Strutz
 * Data..: 16.11.2015
 ******************************************************/

#ifndef _TILE_H_
#define _TILE_H_

/* GlobalPicHeader enthält die notwendigen Bildinformationen des 
Ausgangs-Bildes, um damit bei der Rekonstruktion des Bildes aus 
Kacheln, die PicData Struktur mit den Ausgangs-Bildinformationen
mittels der Funktion CreatePicData erstellen zu können */
typedef struct
{
	/* general parameters	*/
	unsigned int
		//---------------
		//tileDiv,
		numberOfTiles,
		//---------------
		 channel,
		 width,
		 height,
		 bitperchannel,
		 bitperchan[4];	 
} GlobalPicHeader;

void writeGlobalHeader( PicData* pD,
															unsigned int numberOfTiles, 
															wchar_t *outFileName);

unsigned int getNumberOfTiles( PicData* pD, unsigned long tileWidth, 
															unsigned long tileHeight);
PicData* getTile( PicData *tmp, uint tileWidth, uint tileHeight, uint *xposFullPic, 
								 uint *yposFullPic, uint i);
void  getTileSize( PicData *pd, unsigned long *tileWidth,
								 unsigned long *tileHeight, 
								 unsigned long *maxTileWidth, 
								 unsigned long *maxTileHeight, 
								 unsigned int numColours);

GlobalPicHeader *CreateGlobalHeader();
void DeleteGlobalHeader( GlobalPicHeader *gph);
//void WriteGlobalHeader( GlobalPicHeader *gph, BitBuffer *bitbuf);
GlobalPicHeader *LoadGlobalHeader(BitBuffer *);


#endif

