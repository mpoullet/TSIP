/*****************************************************
 * File..: tiles.c
 * Desc..: functions for tile operations
 * Author: T. Strutz. PVL 2014 Team Darmstadt
 * Date..: 01.07.2014
 * changes
 * 27.08.2014 Strutz: determine max values for new tile
 * 16.11.2015 Strutz: tiles must not be squared anymore
 ******************************************************/
#include "header.h"

/*------------------------------------------------------------------
* PicData* getTile(PicData *tmp)
*
* Separates the picture in tiles
* Return Value: vector of tiles 
* Jan Bauer, 25.05.2014 - Team Darmstadt
 * 16.11.2015 Strutz: tiles must not be squared anymore
*-----------------------------------------------------------------*/
PicData* getTile( PicData *picdata, uint tileWidth, uint tileHeight, 
								 uint *xposFullPic, 
								 uint *yposFullPic, uint i)
{
	int deltaX, deltaY;
	uint pos, c, x, y;
	uint yStart, yEnde;
	uint xStart, xEnde;
	/* Jedes Einzelene Quadrat wird zun�chst mit dieser Hilfsstruktur konstruiert	*/
	PicData *newPic; 
	/* Die Werte f�r die jeweiligen Quadrate (von wo bis wo geht jede Kachel)		*/
			
	yStart = *yposFullPic; /* Startpunkt f�r Kachelsetzten Y-Koordinate			*/
	yEnde = *yposFullPic + tileHeight; /* Endpunkt f�r Kachel setzten Y-Koordinate*/

	/*Beim �berschreiten der Bildma�e wird die Position auf den gr�sstm�glichen Wert
	des Bildes gesetzt */
	if ( yEnde > picdata->height) yEnde = picdata->height;

	xStart = *xposFullPic;
	xEnde = xStart + tileWidth;
	*xposFullPic += tileWidth;

	/*Beim �berschreiten der Bildma�e wird die Position auf den gr�sstm�glichen Wert
	des Bildes gesetzt */
	if (xEnde >= picdata->width) // Strutz '=' required if tileWidth is multiple of width*/
	{
		xEnde = picdata->width;
		*xposFullPic = 0;
		*yposFullPic += tileHeight;
	}
			
	/*Gr��e der Kachel Bestimmen (Breite) -> Wegen Randst�cken*/
	deltaX = xEnde - xStart; 
	/*Gr��e der Kachel Bestimmen (H�he)*/
	deltaY = yEnde - yStart;
	
	newPic = CreatePicData( deltaX, deltaY, picdata->channel, picdata->bitperchannel, 
		picdata->bitperchan);
	for ( c = 0; c < picdata->channel; c++)
	{
		newPic->maxVal_orig[c] = 0;
		pos = 0;
		for ( y = yStart; y < yEnde; y++) /* Zeilen weise durchlaufen */ 
		{
			for ( x = xStart; x < xEnde; x++) /* Spalten (einzelner Pixel) */
			{	
				/* Farbe aus Orginalbild an der Position X/Y auslesen
				und an den PixelIndex der Kachel schreiben */
				newPic->data[c][pos] = *GetXYValue( picdata, c, x, y); 
				if (newPic->maxVal_orig[c] < newPic->data[c][pos]) /* determine maximal value	*/
						newPic->maxVal_orig[c] = newPic->data[c][pos];
				pos++;	/* N�chster Pixel */	
			}	
		}
	}
	newPic->space = picdata->space;
	newPic->transparency = picdata->transparency;
	newPic->transparency_colour = picdata->transparency_colour;
	newPic->x_tiles = picdata->x_tiles;
	newPic->y_tiles = picdata->y_tiles;
	fprintf( stderr, "\r tile %d", i+1); 
	fprintf( stderr, ", width: %d", newPic->width); 
	fprintf( stderr, ", height: %d", newPic->height);
	
	return newPic;
}


/*------------------------------------------------------------------
* getNumberOfTiles()
*-----------------------------------------------------------------*/
unsigned int getNumberOfTiles( PicData* pD, unsigned long tileWidth, 
															unsigned long tileHeight)
{
	pD->x_tiles = pD->width / tileWidth;
	if (pD->width % tileWidth !=0 )	pD->x_tiles++;

	pD->y_tiles = pD->height / tileHeight;
	if (pD->height % tileHeight !=0)	pD->y_tiles++;

	return ( pD->x_tiles * pD->y_tiles);
}

