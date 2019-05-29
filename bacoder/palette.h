/*****************************************************
* Datei: palette.h
* Autor: Tilo Strutz
* Datum: 27.03.2014
* Desc.: prototyping for palette routines
******************************************************/

#ifndef _PALETTE_H_
#define _PALETTE_H_

void read_palette( BitBuffer *bitbuf, 
									unsigned int numColours, 
									unsigned char *palette);
void write_palette( BitBuffer *bitbuf, 
									 unsigned int numColours, 
									 unsigned char *palette);

void read_palette_2( BitBuffer * bitbuf, unsigned int numColours, 
										 unsigned char *palette);

void write_palette_2( BitBuffer * bitbuf, unsigned int numColours, 
										 unsigned char *palette);


unsigned long GetIndexedColours_2(PicData * pd, unsigned char *palette);

#endif

