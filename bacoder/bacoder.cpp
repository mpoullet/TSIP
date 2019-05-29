/*****************************************************
 * File..: bacoder.c
 * Desc..: main file in project
 * Author: T. Strutz
 * Date..: 05.05.2011
 * 03.06.2011 Strutz: new parameters for rYUV
 * 23.01.2012 separate reset_val an max_weigth for counts rescaling
 * 03.02.2012 new: aC-> ans iF-> structures
 * 07.02.2012 output to log file
 * 02.08.2012 command line mode from PVL KTKI Nephten-Essen, debugging
 * 30.10.2012 Stegemann: CLI replaced
 * 27.11.2012 Strutz: check filenames for input and output
 * 12.02.2014 new: usage()
 ******************************************************/
#include <assert.h>
#include <time.h>
#include "windows.h"
#include "memoryUsage.h"

#pragma comment( lib, "H26x_Predictor" )

#include "stdafx.h"
#include "autoConfig.h"
#include "imageFeatures.h"
#include "Form1.h"
#include "CLI.h"
//
#define _CRTDBG_MAP_ALLOC /* memory leaks: put filename */
#include <stdlib.h>
#include <crtdbg.h>


void usage(void); /* defined in CLI.cpp	*/

using namespace bacoder;

char* const thisVersion = "TSIPcoder --- Version 2.17\0";

//char outname[512];
wchar_t outname[512]; /* for handover of modified filenames	*/

[STAThreadAttribute]
/*--------------------------------------------------------
 * main()
 *-------------------------------------------------------*/
int main( array<System::String ^> ^args)
{
	int console_flag;
	
	// Aktivieren visueller Effekte von Windows XP, bevor Steuerelemente erstellt werden
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault( false); 

	/* check memory usage	
	 *  see https://msdn.microsoft.com/de-de/library/x98tx3cf.aspx
	 */
	//
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	//	_CrtSetBreakAlloc( 212);
	/* 1015 steht hier für eine Speicherbelegungsnummer, welche 
	 * in {}-Klammern im Memory-Report ausgegeben wurde. Bei erneuter 
	 * Ausführung unterbricht das Programm dann an der Stelle, an dem 
	 * der jeweilige Speicher allokiert wurde.
	 */
	console_flag = 0;
	// Hauptfenster erstellen und ausführen
	try
	{
		/* check, whether parameters are sufficient for non-interactive run	*/
		SessionParameters* sP = parseArgumentString(args);

		if (sP != NULL)
		{
			/* check whether sufficient info is given	*/
			if ( sP->inFN != NULL)
			{
				/* filename must be given	*/
				if (wcslen( sP->inFN ))
					console_flag = 1;
				/* check whether output name is given	*/
				if ( sP->outFN == NULL)
				{
					int len, i;
					/* set *.tsip name	if input is not tsip */
					wcscpy( outname, sP->inFN);
					len = wcslen( outname);
					assert( len < 512-5);
					for (i = len-1; i > 0; i--)
					{
						if (outname[i] == '.')
						{
							if ( outname[i+1] == 't' && outname[i+2] == 's' &&
									 outname[i+3] == 'i' && outname[i+4] == 'p')
							{
								/* write new extension for decoding */
								outname[i] = '_';
								outname[i+1] = 'r';
								outname[i+2] = 'e';
								outname[i+3] = 'c';
								outname[i+4] = 'o';
								outname[i+5] = '.';
								outname[i+6] = 'p';
								outname[i+7] = 'p';
								outname[i+8] = 'm';
								outname[i+9] = '\0';
							}
							else
							{
								/* write new extension for encoding */
								outname[i+1] = 't';
								outname[i+2] = 's';
								outname[i+3] = 'i';
								outname[i+4] = 'p';
								outname[i+5] = '\0';
							}
							break;
						}
					}
					if (i == 0) /* no extension found */
					{
						/* unknown image type */
						console_flag = 0;
					}
					else	sP->outFN = outname;
				}
			}
		}

		if (console_flag)
		{
			consoleOperation( sP);
			if (sP->pD != NULL) DeletePicData( sP->pD);
		}
		else
		{
			// Strutz no GUI
			usage(); /*print help*/
			// FreeConsole(); keep console for stderr output etc.		
#ifndef INTERNET_  /* defined in Form1.h */
			Application::Run( gcnew Form1());
			/* Form1::picdata is freed in ~Form()	*/
#endif
		}	
		delete sP;
	_CrtDumpMemoryLeaks();
		FreeConsole();
	}
	catch(...)
	{
		return 0;
	}
	return 0;
}

