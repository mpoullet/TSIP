/*****************************************************
* File..: fileIO.cpp
* Desc..: implements external filereading and writing
* Author: Michael Stegemann
* Date..: 08.08.2012
* 27.08.2013 Strutz: some fixes in logResultsToFile()
*            type CompMode for ac->compression_mode
* 28.08.2013 dependency check between parameters
*	           some fixes in logResultsToFile()
*						 extension check in loadPicFile()
* 23.09.2013 logf output for CoBaLP2
* 29.10.2013 bugfix reading Format32bppArgb
* 10.01.2014 check numColours if histMode == PERMUTATION
* 10.01.2014 increased number of digits: %.8f", iF->colperpix
* 18.03.2014 Unicode filenames are translated to something feasible
* 01.07.2014 support of tiles
* 26.08.2014 check existence of path before Bitmap()
* 01.10.2014 write name of loaded image to stderr
* 20.11.2014 Leipnitz: added PNG input routine
 * 26.08.2015 int instead of cbool for modif_flag
 * 31.08.2015 log ouput modified: averaged values
 *            output of loaded file name for all extensions
 * 26.08.2016 log ouput added: joint entropies
 * 27.07.2018 logging of num_contexts in structure iv->
 * 16.08.2018 logging of num_contextsLS in structure iv->
 * 04.10.2018 inserted: fclose(h_file);
******************************************************/

#include <assert.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fileIO.h"
#include "hxc.h" /* only for HXC2_EST_INF	*/
#include "hxc3.h" /* only for PATTERN_SIZE3	*/

using namespace bacoder;
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Drawing::Imaging;
using namespace System::Threading;
using namespace System::Runtime::InteropServices;
using namespace System::IO;
using namespace System::Media;


#ifdef HXC2_EST_INF
extern double inf_stage1, inf_stage2, inf_stage3;
extern unsigned long cnt_stage0, cnt_stage1, cnt_stage2, cnt_stage3;
#endif
#ifdef HXC3_EST_INF
extern double inf3_stage1, inf3_stage2, inf3_stage3;
extern unsigned long cnt3_stage1, cnt3_stage2, cnt3_stage3;
#endif



void check_dependencies( PicData *pd, ImageFeatures *iF, 
							 AutoConfig *aC);

PicData* GetPicDataFromPNG_fast( FILE *in);

/* required for the handover of modified filename in case of 
 * Unicode names
 */
//extern wchar_t outputFileName[512]; /* in Form1.cpp declared	*/


/*---------------------------------------------------
 * isGoodCharacter()
 * return of 0 if character is not allowed in filenames
 *------------------------------------------------------*/
int isGoodCharacter( char c)
{
	if (c < 32) return 0;
	if (c > 122) return 0;
	if (c == '"' || c == '%' || c == '*') return 0;
	if (c == ':' || c == '<' || c == '>') return 0;
	if (c == '?' || c == 92 /* backslash*/ || c == '=' ) return 0;
	return 1;
}

/*---------------------------------------------------
 * check_filename()
 *------------------------------------------------------*/
int check_filename( String^ filename, /* string in arbitrary form	*/
									  String^% asciiName,  /* string reduced to ASCII form	*/
									 int console_flag)
{
	char *Cfilename;
	char *CfilenameChar = NULL;
	wchar_t *CfilenameWide = NULL;
	int is_ASCII_flag = 1;
	int len, lenW;
	errno_t err;
	size_t converted_len;

	/* convert managed String to ANSI string	*/
	CfilenameChar = (char *)(void *)Marshal::StringToHGlobalAnsi( filename);
	/* convert managed String to Unicode (wide character) string	*/
	CfilenameWide = (wchar_t *)(void *)Marshal::StringToHGlobalUni( filename);
	//   StringToHGlobalUAuto

	/* get length of string	*/
	lenW = wcslen( CfilenameWide);
	len = lenW + 2;

	/*----------------
	 * test compatibility
	 *---------------------*/
	/* allocate memory for string	*/
	ALLOC( Cfilename, len, char);
	assert( Cfilename != NULL);
	
	/* convert Unicode string to ansi string	*/
	err = wcstombs_s( &converted_len, Cfilename, len, CfilenameWide, _TRUNCATE);

	/* if filename contains non-ASCII charactres then replace it by something else	*/
	if ( err == 42)	/* Illegal byte sequence, CfilenameWide contains special characters	*/
	{
		int i, length;
		char c1, c2, buf[4];

		is_ASCII_flag = 0; /* is not an ASCII string	*/

		for ( i = 0; i < lenW; i++)
		{
			/* Convert a wide character to the corresponding multibyte character */
			length = wctomb( buf, CfilenameWide[i]);
			if (length < 0)
			{
				c1 = (char)(CfilenameWide[i] % 256);
				c2 = (char)(CfilenameWide[i] / 256);
				if ( isGoodCharacter(c1) )
				{
					Cfilename[i] = c1;
				}
				else if ( isGoodCharacter(c2))
				{
					Cfilename[i] = c2;
				}
				else if ( isGoodCharacter( c1 + 32) )
				{
					Cfilename[i] = c1 + 32;
				}
				else if ( isGoodCharacter(c2 + 32))
				{
					Cfilename[i] = c2 + 32;
				}
				else if ( isGoodCharacter( c1 - 90) )
				{
					Cfilename[i] = c1 -90;
				}
				else if ( isGoodCharacter(c2 - 90))
				{
					Cfilename[i] = c2 -90;
				}
				else
				{
					/* character is not okay	*/
					Cfilename[i] = '_';
				}
			}
			else
			{
				Cfilename[i] = buf[0];
			}
		}
		Cfilename[lenW] = '\0';

		// outName = filename;

		if (console_flag)
		{
			fprintf( stderr,"\n# Warning: Filename contains Unicode characters.");
			fprintf( stderr,"\n# Replaced by '.%s'!", strrchr( Cfilename, '\\'));
		}
		//else
		//{
		//	MessageBox::Show (
		//		"Filename contains Unicode characters.\nReplaced by '" + IO::Path::GetFileName( filename) + "'!",
		//		"Warning",
		//		MessageBoxButtons::OK, MessageBoxIcon::Information);
		//}

	}	/* if ( err == 42)*/


	/* convert wide string to managed string	*/
	asciiName = Marshal::PtrToStringAnsi( (IntPtr)Cfilename);
	Marshal::FreeHGlobal ( IntPtr ( CfilenameChar ));
	Marshal::FreeHGlobal ( IntPtr ( CfilenameWide ));
	free( Cfilename);

	return is_ASCII_flag;
}

/*--------------------------------------------------------
 *
 *----------------------------------------------------------*/
inline bool file_exists(wchar_t * name) {
  struct _stat buffer;   
  return ( _wstat(name, &buffer) == 0); 
}
/*---------------------------------------------------
 * loadPicFile()
 *------------------------------------------------------*/
PicData* loadPicFile( String ^filename, Form1 ^frame, int console_flag)
{
	PicData *tmp = NULL;
	wchar_t *wide_name, *ptr;
	FILE *h_file = NULL;
	if ( filename == "" ) return NULL;

	if ( frame != nullptr )
	{
		//frame->globFilename = filename;	/* name in global variable for later use */
		frame->inFilename = filename;	/* name in global variable for later use */
	}

	wide_name = (wchar_t *)(void *)Marshal::StringToHGlobalUni( filename);
	ptr = wcsrchr( wide_name, L'\\');
	if (ptr == NULL)
	{
		fwprintf( stderr, L"\nload: %s\n",  wide_name);
	}
	else
	{
		fwprintf( stderr, L"\nload: %s\n",  ptr);
	}

	try
	{
		/* Datei laden */
		if ( filename->ToLower()->EndsWith ( ".ppm" ) ||
			filename->ToLower()->EndsWith ( ".pnm" ) ||
			filename->ToLower()->EndsWith ( ".pgm" ) ||
			filename->ToLower()->EndsWith ( ".tsip" ) )
		{
			//h_file = _wfopen( wide_name, L"rb+");  why open for writing???
			h_file = _wfopen( wide_name, L"rb");
			if (h_file == NULL)
			{
				if (console_flag)
				{
					fprintf( stderr,"\n# Warning: cannot open file.");
					throw 13;
				}
				else
				{
					//MessageBox::Show (
					//	"Filename contains Unicode characters.\nReplaced by '" + IO::Path::GetFileName( filename) + "'!",
					//	"Warning",
					//	MessageBoxButtons::OK, MessageBoxIcon::Information);
					throw 13;
				}
			}
			/* Decoder oder PPM-Loader verwenden */
			if ( filename->ToLower()->EndsWith ( ".tsip" ) )
			{
				// tmp = DecodePic ( temp_name, ( void * ) ctx_default_file);
				tmp = DecodePic( h_file);
			}
			else
			{
				tmp = LoadPPM( h_file);
			}
			Marshal::FreeHGlobal ( IntPtr ( wide_name ));
			fclose( h_file);

			/* Bild für Form aufbereiten */
			if ( frame != nullptr )
			{
				frame->setImageOfPictureBox1( GetBitmapFromPicData ( tmp ));
			}
		}
		else if ( filename->ToLower()->EndsWith ( ".bmp" ) ||
			filename->ToLower()->EndsWith ( ".gif" ) ||
			filename->ToLower()->EndsWith ( ".jpg" )  ||
			/* Strutz images from Waterloo Greyset2 were imported with only 16 different grey values *
			filename->ToLower()->EndsWith ( ".tif" )  ||
			filename->ToLower()->EndsWith ( ".tiff" ) ||
			/**/
			filename->ToLower()->EndsWith ( ".jpeg" ) 
			)
			/* in Form.resx sind die Open-Filter deklariert	*/
		{
			/* if not supported format: OutOfMemoryException */

			/* check existence of path	*/
			wide_name = (wchar_t *)(void *)Marshal::StringToHGlobalUni( filename);
			if (!file_exists( wide_name))
			{
				if (console_flag)
				{
					fprintf( stderr,"\n# Warning: file does not exist. ###");
					throw 13;
				}
				else
				{
					MessageBox::Show( "File does not exist.\n",
						"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
					return NULL;
				}
			}
			Marshal::FreeHGlobal ( IntPtr ( wide_name ));

			/* Andere Dateitypen laden */
			Bitmap ^ tmpBitmap;
			tmpBitmap = gcnew Bitmap( Image::FromFile ( filename, false ));
			/* ...und Bildstruktur daraus erzeugen */
			/* slow: tmp = GetPicDataFromBitmap( formImage); */

			if ( frame != nullptr )
			{
				frame->setImageOfPictureBox1( tmpBitmap);
			}
			tmp = GetPicDataFromBitmap_fast( tmpBitmap);
			/* check for transparency is made in tsipWrapper1() separately for each tile	*/

#ifdef _DEBUG_M
		{
			FILE *out=NULL;
			char filename[512];
			long i;
			PicData *pd = tmp;


			sprintf( filename, "rgb.ppm");
			out = fopen( filename, "wt");
			if (pd->channel ==1)		fprintf( out, "P2\n");
			else		fprintf( out, "P3\n");
			fprintf( out,"# created by %s\n", thisVersion);
			fprintf( out,"%d %d\n", pd->width, pd->height);
			fprintf( out,"%d\n", 255);

			for ( i = 0; i < pd->size; i++) 
			{
				if ((i % pd->width) == 0)		fprintf( out, "\n");
				fprintf( out,"%5d ", pd->data[0][i]);
				if (pd->channel >= 3)
				{
					fprintf( out,"%5d ", pd->data[1][i]);
					fprintf( out,"%5d  ", pd->data[2][i]);
				}
			}
			fprintf( out,"\n");

			fclose( out);
		}
#endif
		}
		else if ( filename->ToLower()->EndsWith ( ".png" )) /*ALeip*/
		{
						/* if not supported format: OutOfMemoryException */

			wide_name = (wchar_t *)(void *)Marshal::StringToHGlobalUni( filename);
			h_file = _wfopen( wide_name, L"rb+");
			/* check existence of path	*/
			if (!file_exists( wide_name) || h_file == NULL)
			{
				if (console_flag)
				{
					fprintf( stderr,"\n# Warning: file does not exist. ###");
					throw 13;
				}
				else
				{
					MessageBox::Show( "File does not exist.\n",
						"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
					return NULL;
				}
			}
			tmp = GetPicDataFromPNG_fast( h_file);
			
			Marshal::FreeHGlobal ( IntPtr ( wide_name ));
			fclose( h_file);
			
			if ( frame != nullptr )
			{
				frame->setImageOfPictureBox1( GetBitmapFromPicData ( tmp ));
			}
		}
		else
		{
			MessageBox::Show( "Format is not supported.\n",
				"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
			return NULL;
		}

	} /* 	try	*/
	catch ( ... )
	{
		if (!console_flag)
			MessageBox::Show ( "Could not open image\n for some reason.",
				"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);

		//Werfen der Exception zur Verarbeitung an die höheren Ebenen. Mit Stacktrace.
		throw; 

		//return NULL;
	}
	return tmp;
}

/*---------------------------------------------------
 * savePicFile()
 *------------------------------------------------------*/
TimeSpan savePicFile ( String ^filename,
											PicData* tileData,
											AutoConfig* aC,
											ImageFeatures* iF,
											INTERMEDIATE_VALS* iv,
											CustomParam* cparam ,
											BYTES_STRUC *bytes,
											//System::String^ %outName,		/* pointer to a managed string	*/
											int console_flag)
{
	FILE * h_file = NULL;
	ImageFormat ^ imgf;
	DateTime start, end;
	TimeSpan span;
	//PicData *tmp = NULL;
	String^ ascii_name;
	String^%asciiName = ascii_name;
	wchar_t *outFileName;

	bool autoMode = false;

	try
	{ /* Strutz: this overwrites filename, which might be modified in GUI, problem	 !!! */
		outFileName = (wchar_t*)(void*)Marshal::StringToHGlobalUni( filename);

		/* Speichern */
		if ( filename->ToLower()->EndsWith( ".raw") )
		{
			unsigned int r, c, comp;
			/* open file for writing	*/
			h_file = _wfopen( outFileName, L"wb");
			if (h_file == NULL)
			{
				throw 14;
			}
			for ( comp = 0; comp < tileData->channel; comp++)
			{
				for ( r = 0; r < tileData->height; r++)
				{
					for ( c = 0; c < tileData->width; c++)
					{
						putc( tileData->data[comp][c + r * tileData->width], h_file);
					}
				}
			}

			fclose( h_file);
		}
		else if ( filename->ToLower()->EndsWith( ".ppm" ) ||
			        filename->ToLower()->EndsWith( ".pgm" ) ||
			        filename->ToLower()->EndsWith( ".pnm" ) )
		{
			//tmp = CopyPicData( tileData);
			/* Wenn PGM, dann konvertieren */
			if ( filename->ToLower()->EndsWith ( ".pgm" ) ) ConvertToGray( tileData);
			//wcscpy( outputFileName, outFileName);

			/* open file for writing	*/
			h_file = _wfopen( outFileName, L"wb");
			if (h_file == NULL)
			{
				throw 14;
			}

			/* PPM-Writer benutzen */
			WritePPM( tileData, h_file);
			fclose( h_file);

			//DeletePicData( tmp);
		}
		else if ( filename->ToLower()->EndsWith( ".tsip" ) )
		{
			int is_ASCII_flag /*, i*/;

			//tmp = CopyPicData( picdata);

			/* ----------------------------------------------------------
			 * check filename
			 * if Unicode character(s), then create something usful
			 */
			is_ASCII_flag = check_filename( filename, asciiName, console_flag);

			//wideFileName = (wchar_t *) (void *) Marshal::StringToHGlobalUni( filename);
			//wcscpy( outputFileName, wideFileName);
			//Marshal::FreeHGlobal ( IntPtr ( wideFileName ));

			start = DateTime::Now;

			/*-------------------------------
			 * check dependencies of some parameters
			 *---------------------------*/
			check_dependencies( tileData, iF, aC);

			/*----------------------------------------------
			* Coding
			*/
			/* open file for writing, 
			 * append as the global header (and other tiles) have already been written	*/
			h_file = _wfopen( outFileName, L"ab");
			if (h_file == NULL)
			{
				throw 14;
			}
			printf( "\n%s-----------", thisVersion);
			//printf( "\n image: %s", filename->ToCharArray));
			EncodePic( tileData, h_file, (void *)cparam, aC, iF, iv, bytes);
			fclose( h_file);

			end = DateTime::Now;
			span = end - start;

			/* copy parameter back, which have been modified in the encodeing routine
			 * just for logging
			 */
			//tileData->wA = tileData->wA;
			//tileData->wB = tileData->wB;
			//for ( i = 0; i < 4; i++)
			//{
			//	picdata->numOfConstantLines[i] = tmp->numOfConstantLines[i];
			//}

			//DeletePicData( tmp);
			fclose(h_file); 
			return span;

		}
		else
		{
			if ( filename->ToLower()->EndsWith( ".bmp" ) ) imgf = ImageFormat::Bmp;
			else if ( filename->ToLower()->EndsWith( ".gif" ) ) imgf = ImageFormat::Gif;
			else if ( filename->ToLower()->EndsWith( ".jpg" ) ) imgf = ImageFormat::Jpeg;
			else if ( filename->ToLower()->EndsWith( ".png" ) ) imgf = ImageFormat::Png;
			else if ( filename->ToLower()->EndsWith( ".tif" ) ) imgf = ImageFormat::Tiff;
			else
			{
				if ( !autoMode )
				{
					MessageBox::Show (
						"Unknown extension in filename: " + filename, "Error",
						MessageBoxButtons::OK, MessageBoxIcon::Information);
					return span;
				}
			}
			Bitmap ^ tmpBitmap;
			tmpBitmap = GetBitmapFromPicData( tileData);
			tmpBitmap->Save( filename, imgf);
		}
		Marshal::FreeHGlobal( IntPtr( outFileName));
	}
	catch ( ... )
	{
		if (console_flag)
		{
			fprintf( stderr, "\n\n!!!! Could not save image !!!");
		}
		else
		{
			/* Fehlerbearbeitung */
			MessageBox::Show ( "Could not save image!",
				"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
		}
		throw;
	}
	return span;
}


//PM_MA improvement 01 count number of colours
/*--------------------------------------------------------
 *	logResultsToFile()
 *-------------------------------------------------------*/
void logResultsToFile( char* logname,
											TimeSpan span,
											String ^ct_string,
											int modif_flag,
											FileInfo ^ fout,
											char* Cfilename,
											//wchar_t* outFileName,
											PicData* pd,
											AutoConfig* aC,
											ImageFeatures* iF,
											INTERMEDIATE_VALS* iv,
											CustomParam* cparam,
											BYTES_STRUC *bytes,
											int tileFlag)
{
	static unsigned long numBytes_old = 0;
	unsigned long numTileBytes = 0;
	char *namePtr = NULL;
	char *ptr = NULL;
	int i, len;
	FILE *logf;

	/* open logf file in append mode	*/
	logf = fopen( logname, "at");
	if ( logf != NULL )
	{
		fprintf( logf, "\n");
		namePtr = strrchr( thisVersion, ' ' ) + 1;
		if ( namePtr != NULL )
		{
			fprintf( logf, "%s", namePtr);
		}
		else
		{
			fprintf( logf, "???");
		}

		if ( !modif_flag ) fprintf( logf, "AUTO");	/* automatic settings	*/
		else fprintf( logf, "    ");


		/* ----------------------------------------------------------
		 * check filename
		 * if Unicode character(s), then create something usful
		 */
		//is_ASCII_flag = check_filename( inFileName, asciiName, 1);
		//Cfilename = (char *)(void *)Marshal::StringToHGlobalAnsi( asciiName);

		/* search last appereance of a folder separator	*/
		namePtr = strrchr( Cfilename, '\\');

		if ( namePtr != NULL ) /* separator found, cut path*/
		{
			do
			{	/* replace all spaces	*/
				ptr = strrchr( namePtr, ' ');
				if (ptr != NULL) ptr[0] = '_'; 
			}while ( ptr != NULL );
			namePtr++;
			fprintf( logf, " %s", namePtr);
			len = strlen( namePtr);
		}
		else	/* no path available	*/
		{
			fprintf( logf, " %s", Cfilename);
			len = strlen( Cfilename);
		}
		if ( pd->tileNum > 0)
		{
			fprintf( logf, "_tile_%02d_%dx%d", pd->tileNum, pd->x_tiles, pd->y_tiles);
			len += 12; /* corecction of space for formatting the output	*/
		}

		/* fill with spaces ==> formatting of logfile	*/
		for ( i = len; i < 50; i++ ) fprintf( logf, " ");

		/* features of image	*/
		fprintf( logf, " %5d %5d", pd->width, pd->height);
		fprintf( logf, " %6d", iF->numColours);
		fprintf( logf, " %6d", (int)( log( (double)iF->numColours) / log( 2.) ) );
		fprintf( logf, " %6.3f", iF->colEntropy);
		fprintf( logf, " %6ld", iF->patternMeasure);
		fprintf( logf, " %6ld", iF->maxDepth);
		//fprintf( logf, " %.8f", iF->colperpix);
		fprintf( logf, " %3d", (int)floor( 20 * log( 1 + 1. /iF->colperpix) - 13) );
		//fprintf( logf, " %3d", 
		//		min( 300, (int)(-50 * log( (double)iF->colperpix + 1 ) / log(10.) )) );
		fprintf( logf, " %3d", iF->syn);

		fprintf( logf, " %3d", iF->colModalPart);
		fprintf( logf, " %3d", iF->relColourSaturation);
		fprintf( logf, " %3d", iF->relColourContrast);
		fprintf( logf, " %3d", iF->colourSaturation);
		fprintf( logf, " %3d", iF->colourContrast);
		fprintf( logf, " %3d %3d %3d", iF->rgbStdDev[0], iF->rgbStdDev[1], iF->rgbStdDev[2]);

		if ( pd->channel >= 3)
		{
		//	if (iF->syn_grad[0] > 200)
		//	{
		//		if (iF->syn_grad[1] > 200)
		//		{
		//			if (iF->syn_grad[2] > 200) fprintf( logf, " %3d", 0);
		//			else fprintf( logf, " %3d", iF->syn_grad[2]);
		//		}
		//		else 
		//		{
		//			if (iF->syn_grad[2] > 200) fprintf( logf, " %3d", iF->syn_grad[1]);
		//			else fprintf( logf, " %3d", (int)floor( 0.5 * (iF->syn_grad[1] + iF->syn_grad[2]) + 0.5));
		//		}
		//	}
		//	else
		//	{
		//		if (iF->syn_grad[1] > 200)
		//		{
		//			if (iF->syn_grad[2] > 200) fprintf( logf, " %3d", iF->syn_grad[0]);
		//			else fprintf( logf, " %3d", (int)floor( 0.5 * (iF->syn_grad[0] + iF->syn_grad[2]) + 0.5));
		//		}
		//		else 
		//		{
		//			if (iF->syn_grad[2] > 200) fprintf( logf, " %3d", (int)floor( 0.5 * (iF->syn_grad[0] + iF->syn_grad[1]) + 0.5));
		//			else fprintf( logf, " %3d", (int)floor( 0.33333 * (iF->syn_grad[0] + iF->syn_grad[1] + iF->syn_grad[2]) + 0.5));
		//		}
		//	}
			fprintf( logf, " %3d", iF->syn_grad);

			fprintf( logf, " %3d", (int)iF->constance);
			//fprintf( logf, " %3d", (int)floor( 0.33333 * (iF->roughness[0] + iF->roughness[1] + iF->roughness[2]) + 0.5));
			fprintf( logf, " %3d", (int)floor( 0.33333 * (iF->corrcoeff[0] + iF->corrcoeff[1] + iF->corrcoeff[2]) + 0.5));
			fprintf( logf, " %3d", (int)floor( 0.33333 * (iF->monotony[0] + iF->monotony[1] + iF->monotony[2]) + 0.5));
			// fprintf( logf, " %3d", (int)floor( 0.33333 * (iF->constance[0] + iF->constance[1] + iF->constance[2]) + 0.5));
			fprintf( logf, " %3d", (int)floor( 0.33333 * (iF->noise[0] + iF->noise[1] + iF->noise[2]) + 0.5));
		}
		else
		{
			fprintf( logf, " %3d", iF->syn_grad);
			fprintf( logf, " %3d", iF->constance);
			fprintf( logf, " %3d %3d", iF->corrcoeff[0], iF->monotony[0]);
			fprintf( logf, " %3d", iF->noise[0]);
		}

		fprintf( logf, " %3d %3d", iF->corrcoeff[0], iF->monotony[0]);
		fprintf( logf, " %3d", iF->noise[0]);

		fprintf( logf, " %3d %3d", iF->corrcoeff[1], iF->monotony[1]);
		fprintf( logf, " %3d", iF->noise[1]);

		fprintf( logf, " %3d %3d", iF->corrcoeff[2], iF->monotony[2]);
		fprintf( logf, " %3d", iF->noise[2]);

		fprintf( logf, " %7.3lf %7.3lf %7.2lf", 
			iF->jointEntropy[0],
			iF->jointEntropy[1],
			iF->jointEntropy[2]);

		/* features of image	*/
		//fprintf( logf, " %3d", iF->HaarFeatures.entropy_AD1);
		//fprintf( logf, " %3d", iF->HaarFeatures.entropy_DA1);
		//fprintf( logf, " %3d", iF->HaarFeatures.entropy_DD1);
		//fprintf( logf, " %3d", iF->HaarFeatures.sum_AD1);
		//fprintf( logf, " %3d", iF->HaarFeatures.sum_DA1);
		//fprintf( logf, " %3d", iF->HaarFeatures.sum_DD1);

		//fprintf( logf, " %3d", 0);
		fprintf( logf, " trans_%d", pd->transparency);

		/* results, bytes	bitrate*/
		if (pd->tileNum < 2)
		{
			double bpp;

			bpp = 8.0 * (unsigned long)fout->Length / (pd->width * pd->height);
			fprintf( logf, " %9ld %6.3lf", (unsigned long)fout->Length, bpp);
			numBytes_old = (unsigned long)fout->Length;
			
			fprintf( stdout, " Tile %d: %6.3lf bpp\n", pd->tileNum, bpp);
		}
		else
		{
			double bpp;

			numTileBytes = (unsigned long)fout->Length - numBytes_old;
			numBytes_old = (unsigned long)fout->Length;
			/* only the bytes of the last tile	*/
			bpp = 8.0 * numTileBytes / (pd->width * pd->height);
			fprintf( logf, " %9ld %6.3lf", numTileBytes, bpp);
			
			fprintf( stdout, " Tile %d: %6.3lf bpp\n", pd->tileNum, bpp);
		}

		fprintf( logf, " %5.1f_s ", span.TotalSeconds);

		/* used colour space	*/
#ifdef MODULO_RCT
			fprintf( logf, "m");
#ifdef MODULO_SELECTION
			fprintf( logf, "S");
#endif
			fprintf( logf, "_");
#endif
		{
				char* str2 = (char*)(void*)Marshal::StringToHGlobalAnsi( ct_string);
				fprintf(logf, "%s", str2);
				Marshal::FreeHGlobal(IntPtr(str2));
		}
		if (aC->indexed_flag)
		{
			if ( aC->indexed_adj )
			{
				fprintf( logf, "adj");
				if ( aC->indexed_arith )
				{
					fprintf( logf, "AC");
				}
			}
		}

		/* parameters */
		switch ( aC->coding_method )
		{
		case LOCO:
			fprintf( logf, " LOCO   ");
			break;
		case TSIP:
			fprintf( logf, " TSIP   ");
			break;
		case BPC:
			fprintf( logf, " BPC    ");
			break;
		case HXC:
			if (aC->HXCauto) fprintf( logf, " HXCauto");
			else						 fprintf( logf, " HXC    ");
			break;
		case HXC2:
			if (aC->HXC2auto) fprintf( logf, " HXC2aut");
			else							fprintf( logf, " HXC2   ");
			break;
		case SCF:
			if (aC->HXC2auto) fprintf( logf, " SCFauto");
			else							fprintf( logf, " SCF    ");
			break;
		case CoBaLP2:
			fprintf( logf, " CoBaLP2");
			break;
		case MiXeD:
			fprintf( logf, " MiXeD  ");
			break;
		default:
			fprintf( logf, " ???    ");
			break;
		}

		//if ( aC->coding_method == MiXeD)
		if (tileFlag)
		{
			fprintf( logf, " TileSize: %dx%d", aC->tileWidth, aC->tileHeight);
			fprintf( logf, " %dx%d", pd->x_tiles, pd->y_tiles);
		}
		else
		{
			switch ( aC->histMode )
			{
			case NoMOD:
				fprintf( logf, "  NoMOD_ ");
				break;
			case COMPACTION:
				fprintf( logf, "  COMPACT");
				break;
			case PERMUTATION:
				fprintf( logf, "  PERMUT ");
				break;
			case PERMUTE_2OPT:
				fprintf( logf, "  PERM2OP");
				break;
			case C_RESET:
				fprintf( logf, "  C_RESET");
				break;
			case P_RESET:
				fprintf( logf, "  P_RESET");
				break;
			case T_RESET:
				fprintf( logf, "  T_RESET");
				break;
			default:
				fprintf( logf, "  ????   ");
				break;
			}

			if ( aC->coding_method != CoBaLP2 && aC->coding_method != LOCO)
			{
				switch ( aC->predMode )
				{
					case NONE:
						fprintf( logf, " noPred  ");
						break;
					case LEFTNEIGHBOUR:
						fprintf( logf, " LeftN   ");
						break;
					case PAETH:
						fprintf( logf, " PAETH   ");
						break;
					case MED:
						fprintf( logf, " MED     ");
						break;
					case MEDPAETH:
						fprintf( logf, " MEDPAETH");
						break;
					case ADAPT:
						fprintf( logf, " ADAPT   ");
						break;
					case LSBLEND:
						fprintf( logf, " LSBLEND");
						break;
					case LMSPRED:
						fprintf( logf, " LMSPRED ");
						break;
					case LSPRED:
						fprintf( logf, " LSPRED");
						break;
					case H26x:
						fprintf( logf, " H.26x   ");
						break;
					default:
						fprintf( logf, " ????    ");
						break;
				}
			}
			if ( aC->predMode == LMSPRED || aC->predMode == LSBLEND || aC->predMode == LSPRED)
			{
				fprintf( logf, " SS_%02d", aC->LSpred_searchSpace);
				fprintf( logf, " P_%02d", aC->LSpred_numOfPreds);
			}
			if ( aC->predMode == LSBLEND || aC->predMode == LMSPRED )
			{
				fprintf( logf, " Col_%d_%d", aC->useColPrediction_flag, aC->useColPredictionP_flag);
			}
			if ( aC->predMode == LSBLEND )
			{
				fprintf( logf, " C_%d", aC->useCorrelation_flag);
			}

		switch ( aC->coding_method )
		{
		case HXC:
			fprintf( logf, " Tol._%d", aC->HXCtolerance);
			fprintf( logf, " Off1_%d", aC->HXCtoleranceOffset1);
			fprintf( logf, " Off2_%d", aC->HXCtoleranceOffset2);
			fprintf( logf, " Incr_%d", aC->HXCincreaseFac);
			fprintf( logf, " Pat_%d", aC->num_HXC_patterns);
			break;
		case HXC2:
		case SCF:
			fprintf( logf, " %d_%d", aC->SCF_prediction, aC->SCF_directional);
			fprintf( logf, "_%d_%d", aC->SCF_stage2tuning, aC->SCF_colourMAP);
			fprintf( logf, " Tol.R");
			for( i = 0; i < 6; i+=2)
			{
				fprintf( logf, "_%d", aC->HXC2tolerance[0][i]);
				fprintf( logf, "_%d ", aC->HXC2tolerance[0][i+1]);
			}
			if ( pd->channel > 1)
			{
				fprintf( logf, " Tol.G");
				for( i = 0; i < 6; i+=2)
				{
					fprintf( logf, "_%d", aC->HXC2tolerance[1][i]);
					fprintf( logf, "_%d ", aC->HXC2tolerance[1][i+1]);
				}
			}
			if ( pd->channel > 2)
			{
				fprintf( logf, " Tol.B");
				for( i = 0; i < 6; i+=2)
				{
					fprintf( logf, "_%d", aC->HXC2tolerance[2][i]);
					fprintf( logf, "_%d ", aC->HXC2tolerance[2][i+1]);
				}
			}
			if ( pd->channel == 4)
			{
				fprintf( logf, " Tol.A");
				for( i = 0; i < 6; i+=2)
				{
					fprintf( logf, "_%d", aC->HXC2tolerance[3][i]);
					fprintf( logf, "_%d ", aC->HXC2tolerance[3][i+1]);
				}
			}
				
			fprintf( logf, " Incr_%d", aC->HXC2increaseFac);
			fprintf( logf, " Pat_%d", aC->num_HXC_patterns);
			if (aC->coding_method == SCF) 
				fprintf( logf, " max_%d", aC->SCF_maxNumPatterns);

#ifdef HXC2_EST_INF
			{ 
				double sum;
				fprintf( logf, " Inf1_%d", (int)floor( inf_stage1 * 0.125 + 0.5));
				fprintf( logf, " Inf2_%d", (int)floor( inf_stage2 * 0.125 + 0.5));
				fprintf( logf, " Inf3_%d", (int)floor( inf_stage3 * 0.125 + 0.5));
				sum = inf_stage1 + inf_stage2 + inf_stage3;
				//fprintf( logf, " Perc1_%d", (int)floor( inf_stage1 * 100. / sum + 0.5));
				//fprintf( logf, " Perc2_%d", (int)floor( inf_stage2 * 100. / sum + 0.5));
				//fprintf( logf, " Perc3_%d", (int)floor( inf_stage3 * 100. / sum + 0.5));
				if (aC->coding_method == SCF) fprintf( logf, " cnt0_%d", cnt_stage0);
				fprintf( logf, " cnt1_%d", cnt_stage1);
				fprintf( logf, " cnt2_%d", cnt_stage2);
				fprintf( logf, " cnt3_%d", cnt_stage3);
				if (cnt_stage1 == 0) cnt_stage1 = 1;
				if (cnt_stage2 == 0) cnt_stage2 = 1;
				if (cnt_stage3 == 0) cnt_stage3 = 1;
				fprintf( logf, " bps1_%.3f", inf_stage1 / cnt_stage1);
				fprintf( logf, " bps2_%.3f", inf_stage2 / cnt_stage2);
				fprintf( logf, " bps3_%.3f", inf_stage3 / cnt_stage3);
			}
#endif
			break;

		case CoBaLP2:
			if (aC->CoBaLP2_sA && aC->CoBaLP2_sB &&
				  aC->CoBaLP2_sT && !aC->CoBaLP2_aR && !aC->CoBaLP2_aL)
			{
				fprintf( logf, " noPred");
			}
			else
			{
				fprintf( logf, " P_");
				if (aC->CoBaLP2_sT) fprintf( logf, "sT");
				if (aC->CoBaLP2_sA) fprintf( logf, "sA");
				if (aC->CoBaLP2_sB) fprintf( logf, "sB");
				if (aC->CoBaLP2_aR) fprintf( logf, "aR");
				if (aC->CoBaLP2_aL) fprintf( logf, "aL");
			}
			fprintf( logf, " %d", aC->CoBaLP2_constLines);
			fprintf( logf, " %d", pd->numOfConstantLines[0]);
			fprintf( logf, "_%d", pd->numOfConstantLines[1]);
			fprintf( logf, "_%d", pd->numOfConstantLines[2]);
			fprintf( logf, " wA_%d", pd->wA); /* initial weight of predictor A x10	*/
			fprintf( logf, " wB_%d", pd->wB);
			fprintf( logf, " numPX_%d", iv->num_contexts );
			fprintf( logf, " numPxLS_%d", iv->num_contextsLS );
			break;

		case TSIP:
			int comp;
			float rlc1_fac[3];
			float rlc2_fac[3];

			/* parameters used for TSIP	*/

			/* TSIP-specific parameters	*/
			if ( aC->rct_type_used != GREY && !aC->indexed_flag )
			{
				/* interlacing	*/
				switch ( aC->interleaving_mode )
				{
				case Y_U_V:
					fprintf( logf, "   Y|U|V");
					break;
				case YY_uvuv:
					fprintf( logf, " YY|uvuv");
					break;
				case YY_uuvv:
					fprintf( logf, " YY|uuvv");
					break;
				case YuvYuv:
					fprintf( logf, "  YuvYuv");
					break;
				case YYuuvv:
					fprintf( logf, "  YYuuvv");
					break;
				default:
					fprintf( logf, "   ???? ");
					break;
				}
			}
			else  fprintf( logf, "     .");

			if ( aC->postbwt == HXC3)
			{
				fprintf( logf, " Tol1");
				for( i = 0; i < PATTERN_SIZE3; i++)
				{
					fprintf( logf, "_%d", aC->HXC2tolerance[0][i]);
				}
				if ( pd->channel > 1)
				{
					fprintf( logf, " Tol2");
					for( i = 0; i < PATTERN_SIZE3; i++)
					{
						fprintf( logf, "_%d", aC->HXC2tolerance[1][i]);
					}
				}
				if ( pd->channel > 2)
				{
					fprintf( logf, " Tol3");
					for( i = 0; i < PATTERN_SIZE3; i++)
					{
						fprintf( logf, "_%d", aC->HXC2tolerance[2][i]);
					}
				}					
				fprintf( logf, " Incr_%d", aC->HXC2increaseFac);
				fprintf( logf, " Pat_%d", aC->num_HXC_patterns);
			}

			if ( !aC->skip_precoding)
			{
				if ( aC->skip_rlc1 )	fprintf( logf, " 1");
				else	fprintf( logf, " 0");
				if ( aC->skip_postBWT )	fprintf( logf, " 1");
				else	fprintf( logf, " 0");
				if ( aC->skip_precoding )	fprintf( logf, " 1");
				else	fprintf( logf, " 0");
				if ( aC->use_RLC2zero )	fprintf( logf, "_1");
				else	fprintf( logf, "_0");
			}
			else
			{
				fprintf( logf, " . . . ");
			}

			if (aC->skip_postBWT)
			{
				fprintf( logf, "    .    .");
			}
			else if (aC->postbwt != HXC3)
			{
				if ( !aC->skip_precoding)
				{
					if (aC->postbwt == MTF)
					{
						fprintf( logf, " %4d", aC->MTF_val); /* either MTF value or first IFC value*/
						fprintf( logf, "   .");
					}
					else
					{
						fprintf( logf, " %4d", aC->IFCresetVal); /* either MTF value or first IFC value*/
						fprintf( logf, " %3d", aC->IFCmaxCount);
					}
				}
				else
				{
					fprintf( logf, "    .   .");
				}
			}

			fprintf( logf, " %6d", aC->tileWidth);
#ifdef HXC3_EST_INF
			if (aC->postbwt == HXC3)
			{ 
				double sum;
				fprintf( logf, " Inf1_%d", (int)floor( inf3_stage1 * 0.125 + 0.5));
				fprintf( logf, " Inf2_%d", (int)floor( inf3_stage2 * 0.125 + 0.5));
				fprintf( logf, " Inf3_%d", (int)floor( inf3_stage3 * 0.125 + 0.5));
				sum = inf3_stage1 + inf3_stage2 + inf3_stage3;
				fprintf( logf, " cnt1_%d", cnt3_stage1);
				fprintf( logf, " cnt2_%d", cnt3_stage2);
				fprintf( logf, " cnt3_%d", cnt3_stage3);
				fprintf( logf, " bps1_%.3f", inf3_stage1 / cnt3_stage1);
				fprintf( logf, " bps2_%.3f", inf3_stage2 / cnt3_stage2);
				fprintf( logf, " bps3_%.3f", inf3_stage3 / cnt3_stage3);
			}
#endif

			if ( aC->predMode == ADAPT )
			{
				/* block size	*/
				fprintf( logf, " %2d %2d", cparam->predparam->blockWidth, cparam->predparam->blockHeight);
				/* used predictors	*/
				fprintf( logf, " ");
				if ( cparam->predparam->predictors[0] ) fprintf( logf, "Med");
				if ( cparam->predparam->predictors[1] ) fprintf( logf, "_Paeth");
				if ( cparam->predparam->predictors[2] ) fprintf( logf, "_Lin");
				if ( cparam->predparam->predictors[3] ) fprintf( logf, "_GAP");
				if ( cparam->predparam->predictors[4] ) fprintf( logf, "_CoBaLP");
				if ( cparam->predparam->predictors[5] ) fprintf( logf, "_TM");
			}
			else
			{
				fprintf( logf, "  .");
				fprintf( logf, "  .");
				fprintf( logf, "       .");
			}

			for ( comp = 0; ( unsigned ) comp < 3; comp++ )
			{
				if ( iv->RLC1_len[comp] > 0 ) rlc1_fac[comp] = ( float ) iv->RLC1_len[comp] / iv->data_len[comp];
				else
				{
					rlc1_fac[comp] = 0;
				}
				if ( iv->RLC2_len[comp] > 0 ) rlc2_fac[comp] = ( float ) iv->RLC2_len[comp] / iv->RLC1_len[comp];
				else
				{
					rlc2_fac[comp] = 0;
				}
			}
			fprintf( logf, " %6.3f  %4d", rlc1_fac[0], iv->RLC1_marker[0]);
			fprintf( logf, " %6.3f  %4d", rlc1_fac[1], iv->RLC1_marker[1]);
			fprintf( logf, " %6.3f  %4d", rlc1_fac[2], iv->RLC1_marker[2]);
			fprintf( logf, " %6.3f  %4d", rlc2_fac[0], iv->RLC2_marker[0]);
			fprintf( logf, " %6.3f  %4d", rlc2_fac[1], iv->RLC2_marker[1]);
			fprintf( logf, " %6.3f  %4d", rlc2_fac[2], iv->RLC2_marker[2]);

			if (aC->indexed_arith)
			{
					fprintf( logf, "        ."); /* special encoding method is used	*/
					fprintf( logf, "         .");
			}
			else
			{
				if ( aC->entropy_coding_method == 1 )
				{
					fprintf( logf, "  Arithm.");
				}
				else
				{
					fprintf( logf, "  Huffman");
				}
				if ( aC->separate_coding_flag == 1 )
				{
					fprintf( logf, "  Separate");
				}
				else
				{
					fprintf( logf, "  Jointly ");
				}
			}

			break;


		case LOCO:
			fprintf( logf, " MED+Bias");
			if ( aC->T4_flag ) fprintf( logf, "+T4");
			if ( aC->locoImproveFlag ) fprintf( logf, "+k");
			else  fprintf( logf, "  ");
			if ( !aC->T4_flag ) fprintf( logf, "   ");

			break;

		case BPC:
			if ( aC->s_flg ) fprintf( logf, " s_1");
			else  fprintf( logf, " s_0");
			if ( aC->use_colFlag ) fprintf( logf, "/c_1");
			else  fprintf( logf, "/c_0");
			if ( aC->use_predFlag ) fprintf( logf, "/p_1");
			else  fprintf( logf, "/p_0");

			break;
		}

			fprintf( logf, " Bytes: data_%d ghead_%d head_%d histM_%d", bytes->num_data_bytes,
				bytes->num_globheader_bytes, bytes->num_header_bytes, bytes->num_histMode_bytes);
			fprintf( logf, " pal_%d quad_%d Alpha_%d ADAPT_%d H26x_%d", bytes->num_palette_bytes,
				bytes->num_quad_bytes, bytes->num_alpha_bytes, bytes->num_extPred_bytes, 
				bytes->num_H26xPred_bytes);

			fprintf( logf, " %6.3f", iv->entropy[0]);
			fprintf( logf, " %6.3f", iv->entropy[1]);
			fprintf( logf, " %6.3f", iv->entropy[2]);
		//else   is already above
		//{
		//	fprintf( logf, " %6d", aC->tileWidth);
		//}

		if (aC->coding_method != TSIP ) /* in TSIP mode this output has already been made */
		{
			if ( aC->predMode == ADAPT )
			{
				/* block size	*/
				fprintf( logf, " %2d %2d", cparam->predparam->blockWidth, cparam->predparam->blockHeight);
				/* used predictors	*/
				fprintf( logf, " ");
				if ( cparam->predparam->predictors[0] ) fprintf( logf, "Med");
				if ( cparam->predparam->predictors[1] ) fprintf( logf, "_Paeth");
				if ( cparam->predparam->predictors[2] ) fprintf( logf, "_Lin");
				if ( cparam->predparam->predictors[3] ) fprintf( logf, "_GAP");
				if ( cparam->predparam->predictors[4] ) fprintf( logf, "_CoBaLP");
				if ( cparam->predparam->predictors[5] ) fprintf( logf, "_TM");
			}
			else
			{
				fprintf( logf, "  .");
				fprintf( logf, "  .");
				fprintf( logf, "       .");
			}
		}
		} /* if !MixeD	*/

		{
			time_t sekunden = time ( NULL);
			tm *uhr = localtime ( &sekunden);
			fprintf( logf, "\t   %02d.%02d.%d", uhr->tm_mday, uhr->tm_mon + 1, uhr->tm_year + 1900);
			fprintf( logf, " %02d:%02d:%02d", uhr->tm_hour, uhr->tm_min + 1, uhr->tm_sec);
		}


		if ( aC->predMode == H26x )
		{
			fprintf( logf, "\t   %d", aC->H26x_Set->config->postTransform);

			fprintf( logf, "\t   %d", aC->H26x_Set->signalingBytes);

			if ( aC->H26x_Set->config->logPredictors == TRUE )
			{
				for ( int ii = 0; ii < NUM_INTRA_MODE; ii++ )
				{
					if ( aC->H26x_Set->config->availablePredictors[ii] )
						fprintf( logf, "\t   %d", aC->H26x_Set->usedPredictors[ii]);
					else
						fprintf( logf, "\t   Turned off");
				}
			}
			if ( aC->H26x_Set->config->logBlocks == TRUE )
			{
				for ( int ii = 0; ii < NUM_BLOCK_SIZES; ii++ )
				{
					if ( aC->H26x_Set->config->availableBlocks[ii] )
						fprintf( logf, "\t   %d", aC->H26x_Set->usedBlocks[ii]);
					else
						fprintf( logf, "\t   Turned off");

				}
			}

			if ( aC->H26x_Set->config->logTransformations == TRUE )
			{
				fprintf( logf, "\t   %d", aC->H26x_Set->usedTransformations);
			}
		}

		fclose ( logf);
//endfunc:
		if (logf != NULL) fclose ( logf);
		//Marshal::FreeHGlobal ( IntPtr ( Cfilename));

		//if (Cfilename != NULL) free( Cfilename);
	}
}