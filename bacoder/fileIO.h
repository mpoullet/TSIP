/*****************************************************
* File..: fileIO.h
* Desc..: declaration of tools for filereading and writing
* Author: Michael Stegemann
* Date..: 08.08.2012
* 11.04.2014 new: check_filename()
 * 26.08.2015 int instead of cbool for modif_flag
******************************************************/

#pragma once

#include "Form1.h"
#include "stdafx.h"
#include <string.h>


namespace bacoder
{
ref class Form1;
}

extern char* const thisVersion;

PicData* loadPicFile ( System::String ^ filename, bacoder::Form1 ^ frame, 
											 int console_flag);
TimeSpan savePicFile ( System::String ^ filename,
                       PicData* picdata,
                       AutoConfig* aC,
                       ImageFeatures* iF,
                       INTERMEDIATE_VALS* iv,
                       CustomParam* cparam,
											 BYTES_STRUC *bytes,
											 //System::String^ %outName,		/* pointer to a managed string	*/
											 int console_flag);

void logResultsToFile ( char* logname,
                        TimeSpan span,
                        System::String ^ ct_string,
                        int modif_flag,
                        System::IO::FileInfo ^ fout,
											char* Cfilename,
											//wchar_t* inFileName,
											//wchar_t* outFileName,
                        PicData* picdata,
                        AutoConfig* aC,
                        ImageFeatures* iF,
                        INTERMEDIATE_VALS* iv,
                        CustomParam* cparam,
												BYTES_STRUC *bytes,
												int tileFlag);

int check_filename( String^ filename, String^% ascii_name, 
									  int console_flag);
