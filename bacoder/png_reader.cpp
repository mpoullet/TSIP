/*****************************************************
* File..: png_reader.cpp
* Desc..: opens a PNG-Image
* Author: Alexander Leipnitz
* Date..: 08.08.2012
* 28.10.2015 Strutz: bugfix memory leaks row_pointers[] 
******************************************************/

#include "ginterface.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "png.h"

/*
* colour_spaces (copy from png.h):
*
* PNG_COLOR_TYPE_GRAY				0
* PNG_COLOR_MASK_PALETTE    1
* PNG_COLOR_MASK_COLOR      2
* PNG_COLOR_MASK_ALPHA      4
* PNG_COLOR_TYPE_PALETTE		(PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_PALETTE)
* PNG_COLOR_TYPE_RGB        (PNG_COLOR_MASK_COLOR)
* PNG_COLOR_TYPE_RGB_ALPHA  (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA)
* PNG_COLOR_TYPE_GRAY_ALPHA (PNG_COLOR_MASK_ALPHA)
* PNG_COLOR_TYPE_RGBA  PNG_COLOR_TYPE_RGB_ALPHA
* PNG_COLOR_TYPE_GA  PNG_COLOR_TYPE_GRAY_ALPHA
*/

/*--------------------------------------------------
 * New method for opening PNG images using libpng.
 *--------------------------------------------------*/
PicData* GetPicDataFromPNG_fast( FILE *in)
{
	uint width, height;
	int bit_depth, color_type, colour_channels;
	unsigned int maxR = 0, maxG = 0, maxB = 0, maxA = 0;
	unsigned int tmp[] = {8u, 8u, 8u, 8u};
	unsigned long pos = 0;
	unsigned char sig[8];
	PicData *result = NULL;
	int x, y;
	png_byte* row;
	png_byte* ptr;
	png_uint_32		rowbytes;
	png_bytepp		row_pointers = NULL;

	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;

	fread( sig, 1, 8, in);
	if (!png_check_sig( sig, 8))
		return NULL;

	png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); /* allocate and initialize a png_struct structure for reading PNG file */
	info_ptr = png_create_info_struct( png_ptr); /* allocate and initialize a png_info structure */

	png_init_io( png_ptr, in);  /* initialize input of the PNG file */

	png_set_sig_bytes( png_ptr, 8);  /* number of bytes read from PNG file */

	png_read_info( png_ptr, info_ptr); /* read the PNG image information */
	png_get_IHDR( png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL); /* get PNG_IHDR chunk information from png_info structure */

	if (bit_depth == 16)
		png_set_strip_16( png_ptr); /* strip 16 bit PNG file to 8 bit depth */
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand( png_ptr); /* paletted images are expanded to RGB */
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand( png_ptr); /* grayscale images of bit-depth less than 8 are expanded to 8-bit images*/
	if (png_get_valid( png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_expand( png_ptr); /* tRNS chunks are expanded to alpha channels */
	if (color_type == PNG_COLOR_TYPE_RGB && bit_depth < 8)
		png_set_expand( png_ptr);

	png_read_update_info( png_ptr, info_ptr); /* update png_info structure */

	colour_channels =  (int)png_get_channels( png_ptr, info_ptr);  /* get number of color channels in image*/

	/* allocate PicData with the number of colour-channels */
	if (colour_channels == 1)
	{
		tmp[1] = tmp[2] = tmp[3] = 0;
		result = CreatePicData( (uint)width, (uint)height, 1u, 8u, tmp);
	}
	else if (colour_channels == 2)
	{
		tmp[2] = tmp[3] = 0;
		result = CreatePicData( (uint)width, (uint)height, 2u, 8u, tmp);
	}
	else if (colour_channels == 3)
	{
		tmp[3] = 0;
		result = CreatePicData( (uint)width, (uint)height, 3u, 8u, tmp);
	}
	else if (colour_channels == 4)
	{
		result = CreatePicData( (uint)width, (uint)height, 4u, 8u, tmp);
	}

		//pRowbytes = 
	rowbytes = png_get_rowbytes( png_ptr, info_ptr);

		/* Allokieren des Speichers*/
  row_pointers = (png_bytep*) malloc( sizeof(png_bytep) * height);
	if (row_pointers == NULL) 
	{
			png_destroy_read_struct( &png_ptr, &info_ptr, NULL);
			return NULL;
	}
  for ( y = 0; y < (signed)height; y++)
	{
		row_pointers[y] = (png_byte*)malloc( rowbytes);
	}

	png_read_image( png_ptr, row_pointers); /* read the entire image into memory */

	for ( y = 0; y < (int)height; y++)
	{
		row = row_pointers[y];
		for ( x = 0; x < (int)width; x++)
		{
			ptr = &(row[x * colour_channels]);
			result->data[0][pos] = ptr[0];
			if (maxR < result->data[0][pos]) 
			{
				maxR = result->data[0][pos];	/* get maxvalues	*/
			}
			if (colour_channels == 2)
			{
				result->data[1][pos] = ptr[1];
				if (maxG < result->data[1][pos]) maxG = result->data[1][pos];
			}
			else if (colour_channels == 3)
			{
				result->data[1][pos] = ptr[1];
				result->data[2][pos] = ptr[2];
				if (maxG < result->data[1][pos]) maxG = result->data[1][pos];
				if (maxB < result->data[2][pos]) maxB = result->data[2][pos];
			}
			else if (colour_channels == 4)
			{
				result->data[1][pos] = ptr[1];
				result->data[2][pos] = ptr[2];
				result->data[3][pos] = ptr[3];
				if (maxG < result->data[1][pos]) maxG = result->data[1][pos];
				if (maxB < result->data[2][pos]) maxB = result->data[2][pos];
				if (maxA < result->data[3][pos]) maxA = result->data[3][pos];
			}
			pos++; /* next pixel	*/
		}
		
	}	

	/* check identity of channels ==> GREY image?	*/
	if (result->channel >=3 )
	{
		int identity;
		long r, g, b;

		identity = 1;
		for (pos = 0; pos < (unsigned)result->height * result->width; pos++)
		{
			r = result->data[0][pos];
			g = result->data[1][pos];
			b = result->data[2][pos];

			if (identity && ((r != g) || (r != b) || (b != g)) )
			{
					identity = 0; /* it is not a greyscale image	*/
					break;
			}
		}
		if (identity) /* is a greyscale image */
		{
			if (result->channel == 3) result->channel = 1;
			else if (result->channel == 4) result->channel = 2;
		}
	}
	
	result->maxVal_orig[0] = result->maxVal_pred[0] = result->maxVal_rct[0] = maxR;
	result->maxVal_orig[1] = result->maxVal_pred[1] = result->maxVal_rct[1] = maxG;
	result->maxVal_orig[2] = result->maxVal_pred[2] = result->maxVal_rct[2] = maxB;
	result->maxVal_orig[3] = result->maxVal_pred[3] = result->maxVal_rct[3] = maxA;

	png_destroy_read_struct( &png_ptr, &info_ptr, NULL);
	png_read_end( png_ptr, NULL);


	// Strutz
  for ( y = 0; y < (signed)height; y++)
	{
		FREE( row_pointers[y]);
	}
	FREE( row_pointers);
	
	return result;
}