/*****************************************************************
 *
 * File....:	image.h
 * Purpose.:	declaration of IMAG structure
 * Date....: 	04.10.2011
 * changes
 *****************************************************************/
#ifndef IMAG_H
#define IMAG_H


typedef struct {
	unsigned int
		width,		/* Bildbreite 	*/
		height;		/* Bildhoehe 	*/
	unsigned long
		size;		/* Anzahl der Bildpunkte	*/
	int	bit_depth[3];	/* Bittiefe der Daten	*/
	int	bit_depth_max;	/* maximum bit depth	*/
	unsigned long	max_value[3]; /* groester Wert im Bild	*/
	//int	sign_flag;	/* 1=>Daten sind vorzeichenbehaftet*/
	//short	dc_shift;	/* ergibt sich aus Bittiefe	*/
	int current_channel;
	unsigned int colour_channels;
	unsigned int colour_space;
} IMAGEc;


/*------------------------------------------------------------------*/
#endif

