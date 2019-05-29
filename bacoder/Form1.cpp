/*****************************************************
* File..: Form1.cpp
* Desc..: parameter-selection dialog and functions
* Author: T. Strutz
* Date..: xx.xx.2010
* 27.08.2013 type CompMode for ac->compression_mode
* 28.08.2013 usage of tmp in toolStripMenuItemLaden_Click()
* 31.01.2014 autoConfig() w/o return value
* 04.02.2014 bugfix if (aC->rct_type == GREY) 
* 28.02.2014 PlaySound after opening  of an image 
* 28.02.2014 bugfix minimum blockWidth is 2 or image width 
* 18.03.2014 Unicode filenames are translated to something feasible
* 18.03.2014 passing ^%outName to savePicFile() getting modified filename
* 19.03.2014 bugfix: append '.tsip' in saveFile name 
* 11.04.2014 clean up of file name management 
* 23.05.2014 bugfix in bpp calculation if bitdepth is > 8
* 30.06.2014 new tsipWrapper_X() whixh are also called in CLI.cpp
* 09.07.2014 if alpha channel w/o transparency: ignore it
* 01.10.2014 output Tile x of y tiles
 * 22.10.2014 bugfix, tilesize must be computed before autoConfig() is called for 
 *						the first time
 * 13.11.2014 bugfix, pd->channel_alloc, pd->channel was changed too early
							correction of pd->channel before autoconfig() is called
 * 26.03.2015 passing numberOfTiles to log-routine
 * 26.08.2015 int instead of cbool for modif_flag
 * 28.10.2015 bugfix  channel_alloc may not be overwritten!!
* 29.04.2016 new: coding_method_isPassed
* 01.09.2016 getTileSize() after etImageFeatures() as it requires numColours
******************************************************/
#include <assert.h>
#include "Windows.h" /* for PlaySound()	*/
#include "mmsystem.h"
#include "Form1.h"
#include "tile.h"
#include "H26x_Prediction.h"
#include "CLI.h"	/* for applyArgumentList()	*/

using namespace bacoder;

using namespace System; /* for GetPath	*/
using namespace System::IO; /* for GetPath	*/

unsigned int ERRCODE = 0;
char *ctx_default_file = NULL;
bool iserrorimage = false;

cbool applyArgumentList(cliArguments* cli, CustomParam* cparam);
/* cannot be moved to codec.h because of String^	*/
//unsigned long  writeGlobalHeaderToFile( PicData* pD, uint numberOfTiles, 
//															String^ filename);


/*--------------------------------------------------------
 * tsipWrapper_1()
 *-------------------------------------------------------*/
 void tsipWrapper_1( PicData *pd, ImageFeatures *iF, 
							 AutoConfig *aC, double entropy[], 
							 int coding_method_isPassed)
 {
	int chan;
	unsigned long pos;

		getImageFeatures( pd, iF, coding_method_isPassed, aC->coding_method);

		/* determine type of transparency newly for each tile if required	*/
		chan = 0;
		if ( pd->channel == 4)
		{
			/* true colour image plus alpha channel	*/
			chan = 3;
		}
		else if ( pd->channel == 2)
		{
			/* greyscale image plus alpha channel	*/
			chan = 1;
		}
		if ( chan)
		{	/* there is a alpha channel	*/
			int flag_0 = 0, flag_255 = 0, flag_x = 0;

			/* scan alpha channel for different values	*/
			for ( pos = 0; pos < pd->size; pos++)
			{
				if (pd->data[chan][pos] == 0) flag_0 = 1;
				else if (pd->data[chan][pos] == 255) flag_255 = 1;
				else
				{
					flag_x = 1;
					break;
				}
			}

			if (flag_x)
			{
				pd->transparency = 2; /* alpha channel	*/
			}
			else
			{
				if (flag_0 && flag_255)
				{
					pd->transparency = 1; /* binary transparency	*/
				}
				else
				{
					/* there is no transparancy in the alpha channel: ignore it */
					/* pd->channel = chan; is set downwards	*/
				}
			}
		} /* if (chan)	*/
	

		/* for greyscale images with binary transparancy, set a proper signal value	*/
		/* must be also computed in Form1.cpp !	*/
		if ( 0&& pd->space == GREY && pd->transparency == 1) // ignore this, trnasmit separate Alpha-Channel
		{
			unsigned int maxVal;

			pd->transparency = 3;

			/* get max value	*/
			//maxVal = 0;
			//for ( pos = 0; pos < pd->size; pos++)
			//{
			//	if ( maxVal < pd->data[0][pos]) maxVal = pd->data[0][pos];
			//}
			//pd->transparency_colour = maxVal + 1;
			pd->maxVal_orig[0]++;
			pd->transparency_colour = pd->maxVal_orig[0];

			pd->bitperchan[0] = 0;
			maxVal = pd->maxVal_orig[0];
			while (maxVal)
			{
				maxVal >>=1;
				pd->bitperchan[0]++;
			}
			pd->bitperchannel = max( pd->bitperchan[0], pd->bitperchan[1]);


			/* use this unique value as colour indicating transparency	*/
			/* the values may set shortly before transmission
			 * as this routine is called for each tile */
		}

		// uiiii:  channel_alloc may not be overwritten!!
		// pd->channel_alloc = pd->channel;
		/* prevent unnessecary operations	*/
		if ( pd->transparency == 0)
		{
			if ( pd->channel == 2) pd->channel = 1;
			if ( pd->channel == 4) pd->channel = 3;
		}

		/*
		 * if the number of colours is <= 256 it is most likely that a single colour is
		 * always associated with alpha = 0
		 * in this case, this colour is not present in the image
		 * When creating a palette, this colour could be used as marker for transparency.
		 * This is not implemented yet. GetIndexedColours() and GetIndexedColours_2() create
		 * always a new index (=numOfColours), when binary transparency has to be included.
		 */

		/* automatic decision making for compression parameters	*/
		fprintf( stderr, "\r set default parameters ...         ");
		autoConfig( pd, iF, aC, entropy, coding_method_isPassed);
 }

/*--------------------------------------------------------
 * tsipWrapper_2()
 *-------------------------------------------------------*/
 int tsipWrapper_2( PicData *tileData, ImageFeatures *iF, 
							 AutoConfig *aC, CustomParam *cparam,
							 cliArguments* cli, int gui_flag, int *modif_flag)
 {
		/* Prädiktionsparameter */
		if (aC->predMode == ADAPT)
		{
			newParameter( &cparam->predparam, 6u); /* six predictors in maximum */
			// cparam.predparam->blockWidth = 16 + tileData->pixels / 40000;
			cparam->predparam->blockWidth = min( tileData->width, tileData->height) / 20;
			if (iF->syn > 26) 
			{
				cparam->predparam->blockWidth /= 2;	/* bee teacher, demo_gift, woodgroove(27)	*/
			}
			if (cparam->predparam->blockWidth < 2) cparam->predparam->blockWidth = tileData->width;

			cparam->predparam->blockHeight = cparam->predparam->blockWidth;

			//cparam.predparam->blockMode = TRUE; already in newParameter() 
			//cparam.predparam->wMeanMode = FALSE;

			cparam->usextfile = false;
			cparam->ctxfile =  NULL;
			for (int i = 0; i < 6; i++)
			{
				cparam->predparam->predictors[i] = TRUE;
				cparam->predparam->bias[i]       = FALSE;
			}
			cparam->predparam->predictors[5] = FALSE;	/* default: disabled TM	*/

			if (gui_flag)
			{
				bacoder::ExternalPrediction::GetExternParams( cparam);

				if (cparam->predparam == NULL) return 0;
			}
			else
			{
				*modif_flag = ((applyArgumentList( cli, cparam)) == TRUE || 
							*modif_flag == TRUE);
			}
		}
		return 1;
 }

/*--------------------------------------------------------
 * rct2string()
 *-------------------------------------------------------*/
//String^ rct2string( AutoConfig* aC)
String^ rct2string( unsigned char rct_type)
{
	String^ ct_string;
	//switch (aC->rct_type_used)
	switch (rct_type)
	{
		case GREY:  ct_string = "GREY   "; break;
		case RGBc:  ct_string = "RGB    "; break;
		case A1_1:  ct_string = "A1_1   ";  break;
		case A1_2:  ct_string = "A1_2   ";  break;
		case A1_3:  ct_string = "A1_3   ";  break;
		case A1_4:  ct_string = "A1_4   ";  break;
		case A1_5:  ct_string = "A1_5   ";  break;
		case A1_6:  ct_string = "A1_6   ";  break;
		case A1_7:  ct_string = "A1_7   ";  break;
		case A1_8:  ct_string = "A1_8   ";  break;
		case A1_9:  ct_string = "A1_9   ";  break;
		case A1_10: ct_string = "A1_10  ";  break;
		case A1_11: ct_string = "A1_11  ";  break;
		case A1_12: ct_string = "A1_12  ";  break;

		case A2_1:  ct_string = "A2_1   ";  break;
		case A2_2:  ct_string = "A2_2   ";  break;
		case A2_3:  ct_string = "A2_3   ";  break;
		case A2_4:  ct_string = "A2_4   ";  break;
		case A2_5:  ct_string = "A2_5   ";  break;
		case A2_6:  ct_string = "A2_6   ";  break;
		case A2_7:  ct_string = "A2_7   ";  break;
		case A2_8:  ct_string = "A2_8   ";  break;
		case A2_9:  ct_string = "A2_9   ";  break;
		case A2_10: ct_string = "A2_10  ";  break;
		case A2_11: ct_string = "A2_11  ";  break;
		case A2_12: ct_string = "A2_12  ";  break;

		case A3_1:  ct_string = "A3_1   ";  break;
		case A3_2:  ct_string = "A3_2   ";  break;
		case A3_3:  ct_string = "A3_3   ";  break;
		case A3_4:  ct_string = "A3_4   ";  break;
		case A3_5:  ct_string = "A3_5   ";  break;
		case A3_6:  ct_string = "A3_6   ";  break;
		case A3_7:  ct_string = "A3_7   ";  break;
		case A3_8:  ct_string = "A3_8   ";  break;
		case A3_9:  ct_string = "A3_9   ";  break;
		case A3_10: ct_string = "A3_10  ";  break;
		case A3_11: ct_string = "A3_11  ";  break;
		case A3_12: ct_string = "A3_12  ";  break;

		case A4_1:  ct_string = "A4_1   ";  break;
		case A4_2:  ct_string = "A4_2   ";  break;
		case A4_3:  ct_string = "A4_3   ";  break;
		case A4_4:  ct_string = "A4_4   ";  break;
		case A4_5:  ct_string = "A4_5   ";  break;
		case A4_6:  ct_string = "A4_6   ";  break;
		case A4_7:  ct_string = "A4_7   ";  break;
		case A4_8:  ct_string = "A4_8   ";  break;
		case A4_9:  ct_string = "A4_9   ";  break;
		case A4_10: ct_string = "A4_10  ";  break;
		case A4_11: ct_string = "A4_11  ";  break;
		case A4_12: ct_string = "A4_12  ";  break;

		case A5_1:  ct_string = "A5_1   ";  break;
		case A5_2:  ct_string = "A5_2   ";  break;
		case A5_3:  ct_string = "A5_3   ";  break;
		case A5_4:  ct_string = "A5_4   ";  break;
		case A5_5:  ct_string = "A5_5   ";  break;
		case A5_6:  ct_string = "A5_6   ";  break;
		case A5_7:  ct_string = "A5_7   ";  break;
		case A5_8:  ct_string = "A5_8   ";  break;
		case A5_9:  ct_string = "A5_9   ";  break;
		case A5_10: ct_string = "A5_10  ";  break;
		case A5_11: ct_string = "A5_11  ";  break;
		case A5_12: ct_string = "A5_12  ";  break;

		case A6_1:  ct_string = "A6_1   ";  break;
		case A6_2:  ct_string = "A6_2   ";  break;
		case A6_3:  ct_string = "A6_3   ";  break;
		case A6_4:  ct_string = "A6_4   ";  break;
		case A6_5:  ct_string = "A6_5   ";  break;
		case A6_6:  ct_string = "A6_6   ";  break;
		case A6_7:  ct_string = "A6_7   ";  break;
		case A6_8:  ct_string = "A6_8   ";  break;
		case A6_9:  ct_string = "A6_9   ";  break;
		case A6_10: ct_string = "A6_10  ";  break;
		case A6_11: ct_string = "A6_11  ";  break;
		case A6_12: ct_string = "A6_12  ";  break;

		case A7_1:  ct_string = "YUVr   ";  break;
		case A7_2:  ct_string = "A7_2   ";  break;
		case A7_3:  ct_string = "A7_3   ";  break;
		case A7_4:  ct_string = "A7_4   ";  break;
		case A7_5:  ct_string = "A7_5   ";  break;
		case A7_6:  ct_string = "A7_6   ";  break;
		case A7_7:  ct_string = "A7_7   ";  break;
		case A7_8:  ct_string = "A7_8   ";  break;
		case A7_9:  ct_string = "A7_9   ";  break;
		case A7_10: ct_string = "A7_10  ";  break;
		case A7_11: ct_string = "YCgCoR ";  break;
		case A7_12: ct_string = "A7_12  ";  break;

		case A8_1:  ct_string = "A8_1   ";  break;
		case A8_2:  ct_string = "A8_2   ";  break;
		case A8_3:  ct_string = "A8_3   ";  break;
		case A8_4:  ct_string = "A8_4   ";  break;
		case A8_5:  ct_string = "A8_5   ";  break;
		case A8_6:  ct_string = "A8_6   ";  break;
		case A8_7:  ct_string = "A8_7   ";  break;
		case A8_8:  ct_string = "A8_8   ";  break;
		case A8_9:  ct_string = "A8_9   ";  break;
		case A8_10: ct_string = "A8_10  ";  break;
		case A8_11: ct_string = "A8_11  ";  break;
		case A8_12: ct_string = "A8_12  ";  break;

		case A9_1:  ct_string = "A9_1   ";  break;
		case A9_2:  ct_string = "A9_2   ";  break;
		case A9_3:  ct_string = "A9_3   ";  break;
		case A9_4:  ct_string = "A9_4   ";  break;
		case A9_5:  ct_string = "A9_5   ";  break;
		case A9_6:  ct_string = "A9_6   ";  break;
		case A9_7:  ct_string = "A9_7   ";  break;
		case A9_8:  ct_string = "A9_8   ";  break;
		case A9_9:  ct_string = "A9_9   ";  break;
		case A9_10: ct_string = "A9_10  ";  break;
		case A9_11: ct_string = "A9_11  ";  break;
		case A9_12: ct_string = "A9_12  ";  break;

		case B1_1:  ct_string = "B1_1   ";  break;
		case B1_2:  ct_string = "B1_2   ";  break;
		case B2_1:  ct_string = "B2_1   ";  break;
		case B2_3:  ct_string = "B2_3   ";  break;
		case B3_2:  ct_string = "B3_2   ";  break;
		case B3_3:  ct_string = "B3_3   ";  break;
		case B4_1:  ct_string = "B4_1   ";  break;
		case B5_2:  ct_string = "B5_2   ";  break;
		case B6_3:  ct_string = "B6_3   ";  break;

		case PEI09: ct_string = "PEI09  ";  break;
		case A7_1s:  ct_string = "A7_1s  ";  break;
		case A7_1m:  ct_string = "A7_1m  ";  break;

		case INDEXED_GBR:	ct_string = "Idx_GRB";	break;
		case INDEXED_LUM:	ct_string = "Idx_Lum";	break;

		default: ct_string = "??     ";break;
	}
	return ct_string;
}

/*--------------------------------------------------------
 * tsipWrapper_3()
 *-------------------------------------------------------*/
 TimeSpan tsipWrapper_3(  String ^ouputFileName,
											String ^inputFileName,
											PicData* tileData,
											AutoConfig* aC,
											ImageFeatures* iF,
											INTERMEDIATE_VALS* iv,
											CustomParam* cparam,
											BYTES_STRUC *bytes,
											int modif_flag,
											int log_flag, char *log_name, String^% ct_string,
											int console_flag)
{
	FileInfo^ fout;
	TimeSpan span;

	//if (pd->transparency == 3) /* greyscale image with binary transparency	*/
	//{
	//	/* replace greyvalues by the new transparency colour/value	*/
	//	for ( pos = 0; pos < pd->size; pos++)
	//	{
	//		if (pd->data[1][pos] == 0) /* this pixel is transparent	*/
	//		{
	//			pd->data[0][pos] = pd->transparency_colour;
	//		}
	//	}
	//	/* pd->data[1] is now obsolete, keep channel == 2, as it is allocates	*/ 
	//}
	span = savePicFile( ouputFileName, tileData, aC, iF, iv, cparam, bytes,/*outName,*/ console_flag);


	/* info of saved file */
	fout = gcnew FileInfo( ouputFileName);

	/* get RCT string from number of colour space	*/
	//ct_string = rct2string( aC);
	ct_string = rct2string( aC->rct_type_used);

	/* ---------------------------------------
	* do the logging of all parameters and results
	*/
	if (log_flag)
	{
		char *Cfilename = NULL;
		int is_ASCII_flag;
		String ^ascii_name;
		String ^%asciiName = ascii_name;

		fprintf( stderr, "\n logging ...");
		/* ----------------------------------------------------------
		 * check filename
		 * if Unicode character(s), then create something usful
		 */
		is_ASCII_flag = check_filename( inputFileName, asciiName, 0);
		//is_ASCII_flag = check_filename( ouputFileName, asciiName, 0);
		Cfilename = (char *)(void *)Marshal::StringToHGlobalAnsi( asciiName);

		// Function takes way to much arguments -> stack cluttering, consider refactoring

		/* logging for images without tiles or single tiles	*/
		logResultsToFile( log_name, span, ct_string, modif_flag, fout, 
			Cfilename, tileData, aC, iF, iv, cparam, bytes, 0);

		Marshal::FreeHGlobal( IntPtr( Cfilename));
		fprintf( stderr, " ready \n");
	}
	return span;
}

/*--------------------------------------------------------
* toolStripMenuItemLaden_Click()
* load image function
*-------------------------------------------------------*/
System::Void Form1::toolStripMenuItemLaden_Click( 
				System::Object^ sender, System::EventArgs^ e) 
{
	PicData *tmp;
	String^ inputFileName;	
	Thread^ th;

	e=e; sender=sender;

	openFile->FileName = "";				
	openFile->ShowDialog();
	inputFileName = openFile->FileName->ToString();

	if (inputFileName == "") return;

	th = gcnew Thread(gcnew ThreadStart( this, &Form1::thfunc));
	prg->Text = "Loading image ...";
	th->Start();

	/* If there was already an image loaded, then remove it first	*/
	if (this->picdata != NULL)
		DeletePicData(this->picdata);
	// this->picdata = loadPicFile( inputFileName, this);
	tmp = loadPicFile( inputFileName, this, 0); /* 0 ... GUI mode	*/

	/* use chekker board pappern as background	*/
	{
		int y, x;
		Bitmap^ bmp = gcnew Bitmap( 16, 16 );
		for ( y = 0; y < 8; y++) 
		{
			for ( x = 0; x < 8; x++)
			{
				bmp->SetPixel ( x, y, System::Drawing::Color::Gray );
				bmp->SetPixel ( x+8, y+8, System::Drawing::Color::Gray );
			}
		}
		this->setBackgroundOfPictureBox1(bmp);	/*Kacheln als Hintergrund*/
	}

	/*Ende Team Hannover PNG-Transparenz*/

	prg->BeginClose();

	//if (this->picdata == NULL) return;
	if (tmp == NULL) return;
	else  this->picdata = tmp;

	StatusLabel->Text = "Width: " + 
		pictureBox1->Image->Width.ToString() + 
		"px, Height: " + 
		pictureBox1->Image->Height.ToString() +
		"px, Image: " + Path::GetFileName( inputFileName);
	//Beep( 440, 100); /* Hz, ms*/
	//MessageBeep( MB_OK);
	//PlaySound( TEXT("SystemStart"), NULL, SND_ALIAS);
	// working PlaySound( TEXT("DeviceConnect"), NULL, SND_ALIAS);
	PlaySound( TEXT("WindowsUAC"), NULL, SND_ALIAS);
// working PlaySound( TEXT("SystemExclamation"), NULL, SND_ALIAS);
	// working PlaySound( TEXT("FaxBeep"), NULL, SND_ALIAS);
	// working 	PlaySound( TEXT("SystemNotification"), NULL, SND_ALIAS);
	//PlaySound( TEXT("FeedDiscovered"), NULL, SND_ALIAS);
}

/*--------------------------------------------------------
* toolStripMenuItemSpeichern_Click()
* save image function
*-------------------------------------------------------*/
Void Form1::toolStripMenuItemSpeichern_Click(System::Object^  sender, 
																						 System::EventArgs^  e) 
{
	char *log_name;
	String^ ouputFileName;	
	//String^% outName=filename;	
	Thread^ th;
	DateTime start, end;
// blendet Klassenmember aus, Form1.h line 73
//	String ^rct_string;
	String^% ct_string=rct_string;	
	String ^pred_string, ^histMode_string;
	int modif_flag;
	int idx, log_flag = 1; /* logging by default	*/
	long insize;
	FileInfo^ fout;

	CustomParam cparam; /* for ADAPT predictor	*/
	AutoConfig autoConf, *aC;
	ImageFeatures imageF, *iF;
	INTERMEDIATE_VALS intermed_vals, *iv;
	TimeSpan span;

	e=e; sender=sender;
	aC = &autoConf;
	iF = &imageF;
	iv = &intermed_vals;

	if (pictureBox1->Image == nullptr)
	{
		MessageBox::Show( "There is no image to be saved.",
			"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
		return;
	}

	// Dateiname ermitteln
	saveFile->FileName = "";			
	//saveFile->FileName = globFilename;			
	saveFile->FileName = inFilename;			
	// remove extension	
	idx = saveFile->FileName->LastIndexOf( '.');
	if (idx >= 0)
	{
		saveFile->FileName = saveFile->FileName->Remove( idx);
		saveFile->FileName = saveFile->FileName + ".tsip"; /* this is the default output */
	}

	// start dialog	
	if (saveFile->ShowDialog() != ::DialogResult::OK ||
		saveFile->FileName == "")
	{
		return;
	}
	ouputFileName = saveFile->FileName;	

	if (!ouputFileName->ToLower()->EndsWith( ".tsip"))
	{
		savePicFile( ouputFileName, picdata, NULL, NULL, NULL, NULL, NULL,/*outName,*/ 0);
	}
	else /* save as *.tsip	*/
	{
		// Do more complex stuff

		/* see CLI.cpp, consoleOperation() lines 1450ff. does almost the same */
		int i;
		double entropy[3]; /* entropies of colour components	*/
		unsigned char *usedColourSpaces= NULL;
		unsigned int numberOfTiles;
		unsigned int  xposFullPic, yposFullPic;
		PicData *tileData;
		//CompMode *usedMethods = NULL;
		int *usedMethods = NULL;
		AutoConfig original_aC;
		ImageFeatures original_iF;
		BYTES_STRUC bytes_obj, *bytes;

		bytes = &bytes_obj;

		/* progress */
//#ifndef _DEBUG
		th = gcnew Thread( gcnew ThreadStart( this, &Form1::thfunc));
		prg->Text = "getting image features ...";
		th->Start();
//#endif


		/* this is also called  in CLI.cppp in console_operation()
		 * get image properties and set default parameters
		 * for entire image
		 */
		 /* compute some statistics for automatic parameter selection	*/

		fprintf( stderr, "\r get features of tile %d ...          ",
			picdata->tileNum);
		tsipWrapper_1( picdata, iF, aC, entropy, 0); /* analyse the image	*/
		modif_flag = 1;//false; /* automatic settings only	*/

		/* tile size	*/
		getTileSize( picdata, &(aC->tileWidth), &(aC->tileHeight),
			&(aC->maxTileWidth), &(aC->maxTileHeight), iF->numColours);

		original_aC = *aC;
		original_iF = *iF;

//#ifndef _DEBUG
		prg->BeginClose();
//#endif
		{
			System::Windows::Forms::DialogResult result;
			unsigned int autoTileWidth, autoTileHeight;

			/* remember, as tile size must be set back if dialog has No result*/
			autoTileWidth = aC->tileWidth; 
			autoTileHeight = aC->tileHeight; 
			do /* till Dialog is successful	*/
			{
				aC->tileWidth = autoTileWidth;
				aC->tileHeight = autoTileHeight;
				/* Konfiguration laden */
				cfg = gcnew ConfigForm();
				cfg->InsertParams( picdata, saveFile->FileName, iF, aC, 0);
				cfg->ShowDialog();	

				modif_flag = /*(cbool)*/ compare_aC( aC, &original_aC); 

				ouputFileName = cfg->changedfile; /* local copy	*/
				/* this ouputFileName will be overwritten in tsipWrapper_3() ;-( */

				if (cfg->result == 0) return; /* dialog was cancelled	*/

				/* re-determination based on the chosen tile size	*/
				numberOfTiles = getNumberOfTiles( picdata, aC->tileWidth, aC->tileHeight);
				
				/* does this copies the entire structure ??	*/
				original_aC = *aC; /* keep  GUI settings for final logging	*/

				result = ::DialogResult::Yes;
				if (numberOfTiles > 16) 
				{
					result = MessageBox::Show(
						"The chosen tile size is only " + aC->tileWidth + " x " + 
						 aC->tileHeight +
						".\nDo you really want to click through " + numberOfTiles +
						" compression parameter dialogs?", 
						"Tiling of image", 
						MessageBoxButtons::YesNo, MessageBoxIcon::Question, 
						MessageBoxDefaultButton::Button2);
				}
			} while (result == ::DialogResult::No);
		}

		/* write global header and close file afterwards */
		{
			wchar_t *outFileName;
			outFileName = (wchar_t*)(void*)Marshal::StringToHGlobalUni( ouputFileName);

			writeGlobalHeader( picdata, numberOfTiles, outFileName);	

			Marshal::FreeHGlobal( IntPtr( outFileName));
		}
		bytes->num_globheader_bytes = numBytes; /* declared in bitbuf.c	*/

		/* in tile mode, allocate arrays for coding prameters,
		 * required only for logging
		 */
		if (numberOfTiles > 1)
		{
			//ALLOC( usedMethods, numberOfTiles, CompMode);
			ALLOC( usedMethods, numberOfTiles, int);
			ALLOC( usedColourSpaces, numberOfTiles, unsigned char);
		}

		//tmpPD = CopyPicData( picdata); /* copy entire image	*/
		/*Momentane Position der X und Y Zeiger, geben an wo die nächste Kachel aus dem Bild geschnitten werden soll*/
		xposFullPic = 0;
		yposFullPic = 0;
		for ( i = 0; i < (signed)numberOfTiles; i++)
		{
			tileData = getTile( picdata, aC->tileWidth, aC->tileHeight, 
												&xposFullPic, &yposFullPic, i);
			fprintf( stderr, " (total: %d tiles)\n", numberOfTiles); 
			if (numberOfTiles > 1)
			{ /* remember tile number	*/
				tileData->tileNum = (unsigned int)(i+1);

				th = gcnew Thread( gcnew ThreadStart( this, &Form1::thfunc));
				prg->Text = "getting features of tile " + tileData->tileNum + " ...";
				th->Start();
				/* problem: settings for the first tile are overwritten here 
				 * as the result of the global dialog is not memorised
				 */
				/* determine parameters for single tile
				 * other wise keep global settings
				 */
				tsipWrapper_1( tileData, iF, aC, entropy, 0);

				prg->BeginClose();
			}
			else
			{
				tileData->tileNum = 0;	/* marker for single segment	*/
			}


			if ( tileData->tileNum ) /* not for a single image, as dialog has already been done	*/
			{
				/* Konfiguration laden */
				cfg = gcnew ConfigForm();
				cfg->InsertParams( tileData, saveFile->FileName, iF, aC, numberOfTiles);
				cfg->ShowDialog();	
				if (cfg->result == 0) 
				{
					DeletePicData( tileData);
					//picdata = tmpPD; /* set pointer to original structure	*/
					return; /* dialog was cancelled	*/
				}

				modif_flag |= compare_aC( aC, &original_aC); 
			}

			/* prediction parameter	*/
			if ( 0 == tsipWrapper_2( tileData, iF, aC, &cparam, NULL, 1, &modif_flag) )
			{
				/* if dialog was cancelled	*/
				DeletePicData( tileData);
				//picdata = tmpPD; /* set pointer to original structure	*/
				return; 
			}

			if (aC->predMode == H26x)
			{	
				H26xConfiguration originalConfig;
				ALLOC( aC->H26x_Set,1u,H26xSettings);
				ALLOC( aC->H26x_Set->config,1u,H26xConfiguration);

				getH26xDefaultConfig( aC->H26x_Set->config);

				originalConfig = *(aC->H26x_Set->config);

				H26xConfig = gcnew H26xConfigForm();
				H26xConfig->initConfiguration( aC->H26x_Set->config);
				if (H26xConfig->ShowDialog() != ::DialogResult::OK)
				{
					return;
				}

				H26xConfig->getConfiguration(aC->H26x_Set->config);
			}

			/* progress */
			th = gcnew Thread( gcnew ThreadStart( this, &Form1::thfunc));
			if ( numberOfTiles > 1)
			{
				prg->Text = "saving tile " + tileData->tileNum + " of " + numberOfTiles + " ...";
			}
			else
			{
				prg->Text = "saving image ...";
			}
			th->Start();

			log_flag = cfg->checkBoxLogFile->Checked == true ? 1:0;
			log_name = (char*)(void*)Marshal::StringToHGlobalAnsi(cfg->textBoxLogFile->Text);

			/* call encoding routine, also called in CLI.cpp	*/
			span = tsipWrapper_3( ouputFileName, inFilename, tileData, aC, iF, iv, &cparam, &bytes_obj, 
				modif_flag, log_flag, log_name, ct_string, 0);
			
			Marshal::FreeHGlobal( IntPtr( log_name));

			/* must be after tsipWrapper_3() as in check_dependencies() the Indexed mode is set	*/
			if (numberOfTiles > 1)
			{
				usedMethods[i] = aC->coding_method;
				usedColourSpaces[i] = aC->rct_type_used;
			}

			prg->BeginClose(); /* close status bar	*/

			if (numberOfTiles < 2)
			{
				/*
				 * prepare a message box containing results
				 */

				ct_string = rct_string; /* copy from class variable	*/

				/* prepare a string	*/
				if (aC->histMode == NoMOD) histMode_string = "NoMOD";
				else if (aC->histMode == COMPACTION) histMode_string = "COMPACTION";
				else if (aC->histMode == PERMUTATION) histMode_string = "PERMUTATION";
				else if (aC->histMode == C_RESET) histMode_string = "C_RESET";
				else if (aC->histMode == P_RESET) histMode_string = "P_RESET";
				else if (aC->histMode == T_RESET) histMode_string = "T_RESET";

				insize = (tileData->size * tileData->channel * tileData->bitperchannel) / 8;
				fout = gcnew FileInfo( ouputFileName);

				if (aC->coding_method == TSIP) 
				{
					int comp, postBWTparam;
					float rlc1_fac[3];
					float rlc2_fac[3];
					uint bw = 0, bh = 0;

					if (aC->predMode == ADAPT)
					{
						bw = cparam.predparam->blockWidth;
						bh = cparam.predparam->blockHeight;
					}

					for (comp = 0; (unsigned)comp < 3; comp++)
					{						
						if (iv->RLC1_len[comp] > 0)
						{
							rlc1_fac[comp] = (float)iv->RLC1_len[comp]/ iv->data_len[comp];
						}
						else
						{
							rlc1_fac[comp] = 0;
						}
						if (iv->RLC2_len[comp] > 0) 
						{
							rlc2_fac[comp] = (float)iv->RLC2_len[comp] / iv->RLC1_len[comp];
						}
						else
						{
							rlc2_fac[comp] = 0;
						}
					}

					/* prepare some strings	*/
					switch (aC->predMode)
					{
						case LEFTNEIGHBOUR: pred_string = "LEFTNEIGHBOUR"; break;
						case PAETH:			pred_string = "PAETH";		   break;
						case MED:			pred_string = "MED";		   break;
						case MEDPAETH:      pred_string = "MEDPAETH";	   break;
						case ADAPT:			pred_string = "ADAPT";		   break;
						case H26x:			pred_string = "H26x";		   break;
						default: pred_string = "NoPred";				   break;
					}
					if (aC->postbwt == MTF) postBWTparam = aC->MTF_val;
					else	postBWTparam = aC->IFCresetVal;

					MessageBox::Show(
						"TSIP Coding succesfully executed! \n\n" +
						"Duration:\t" + Math::Round(span.TotalSeconds, 1) + "s\n" + 
						"Number of colours:\t" + iF->numColours + "\n" + 
						"Colour space:\t" + ct_string + "\n" + 
						"Transparency:\t" + picdata->transparency + "\n" + 
						"Hist Mode:\t" + histMode_string + "\n" + 
						"Prediction:\t" + pred_string + "\n" + 
						"BlockWidth :\t" + bw + " \n" + 
						"BlockHeight:\t" + bh + " \n" + 
						"Entropy comp. 0:\t" + Math::Round(iv->entropy[0], 3) + " bpp\n" + 
						"Entropy comp. 1:\t" + Math::Round(iv->entropy[1], 3) + " bpp\n" + 
						"Entropy comp. 2:\t" + Math::Round(iv->entropy[2], 3) + " bpp\n" + 
						"skip RLC1  :\t" + aC->skip_rlc1 + "\n" + 
						"skip precod:\t" + aC->skip_precoding + "\n" + 
						"RLC2 zeros :\t" + aC->use_RLC2zero + "\n" + 
						"RLC1 0:\t" + Math::Round(rlc1_fac[0], 3) + "\t Marker "+ iv->RLC1_marker[0] + "\n"+ 
						"RLC1 1:\t" + Math::Round(rlc1_fac[1], 3) + "\t Marker "+ iv->RLC1_marker[1] + "\n"+ 
						"RLC1 2:\t" + Math::Round(rlc1_fac[2], 3) + "\t Marker "+ iv->RLC1_marker[2] + "\n"+ 
						"RLC2 0:\t" + Math::Round(rlc2_fac[0], 3) + "\t Marker "+ iv->RLC2_marker[0] + "\n"+ 
						"RLC2 1:\t" + Math::Round(rlc2_fac[1], 3) + "\t Marker "+ iv->RLC2_marker[1] + "\n"+ 
						"RLC2 2:\t" + Math::Round(rlc2_fac[2], 3) + "\t Marker "+ iv->RLC2_marker[2] + "\n"+ 
						"IFC:\t\t" + (unsigned char)aC->postbwt + "\n" + 
						"MTF/IFC val:\t" + postBWTparam + "\n" + 
						"IFCmaxCount:\t" + aC->IFCmaxCount + "\n" + 
						"ArithCoding:\t" + aC->entropy_coding_method + "\n" + 
						"separate   :\t" + aC->separate_coding_flag + "\n" + 
						"Size original:\t" + insize + " Bytes\n" + 
						"Size compressed:\t" + fout->Length + " Bytes\n" +
						"  Data........:\t" + bytes->num_data_bytes + " Bytes\n" +
						"  Header....:\t" + bytes->num_header_bytes + " Bytes\n" +
						"  Ext. Pred..:\t" + bytes->num_extPred_bytes + " Bytes\n" +
						"  H26x Pred.:\t" + bytes->num_H26xPred_bytes + " Bytes\n" +
						"  Hist. Mode:\t" + bytes->num_histMode_bytes + " Bytes\n" +
						"  Palette.....:\t" + bytes->num_palette_bytes + " Bytes\n" +
						"Compression ratio:\t" + Math::Round(1.0 * insize / fout->Length, 3) + "\n" +
						"Bitrate [bpp]:\t" + Math::Round( 
									8.0 * fout->Length / (picdata->height * picdata->width), 3) + "\n",
						"Compression done.", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}

				if (aC->coding_method == LOCO) 
				{
					/* in rice coding mode. there is no access to the predicted data */
					MessageBox::Show(
						"LOCO-I succesfully executed! \n\n" +
						"Duration:\t" + Math::Round(span.TotalSeconds, 1) + "s\n" + 
						"Entropy R:\t" + Math::Round(iv->entropy[0], 3) + " bpp\n" + 
						"Entropy G:\t" + Math::Round(iv->entropy[1], 3) + " bpp\n" + 
						"Entropy B:\t" + Math::Round(iv->entropy[2], 3) + " bpp\n" + 
						"Number of colours:\t" + iF->numColours + "\n" + 
						"Colour space:\t" + ct_string + "\n" + 
						"Transparency:\t" + picdata->transparency + "\n" + 
						"Hist Mode:\t" + histMode_string + "\n" + 
						"Size original:\t" + insize + " Bytes\n" + 
						"Size compressed:\t" + fout->Length + " Bytes\n" +
						"  Data........:\t" + bytes->num_data_bytes + " Bytes\n" +
						"  Header....:\t" + bytes->num_header_bytes + " Bytes\n" +
						"  Hist. Mode:\t" + bytes->num_histMode_bytes + " Bytes\n" +
						"  Palette.....:\t" + bytes->num_palette_bytes + " Bytes\n" +
						"Compression ratio:\t" + Math::Round(1.0 * insize / fout->Length, 3) + "\n" +
						"Bitrate [bpp]:\t" + Math::Round( 
									8.0 * fout->Length / (picdata->height * picdata->width), 3) + "\n",
						"Compression ready.", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}

				if (aC->coding_method == HXC) 
				{
					/*  */
					MessageBox::Show(
						"HXC Coding succesfully executed! \n\n" +
						"Duration:\t" + Math::Round(span.TotalSeconds, 1) + "s\n" + 
						"Number of colours:\t" + iF->numColours + "\n" + 
						"Colour space:\t" + ct_string + "\n" + 
						"Transparency:\t" + picdata->transparency + "\n" + 
						"Hist Mode:\t" + histMode_string + "\n" + 
						"Size original:\t" + insize + " Bytes\n" + 
						"Size compressed:\t" + fout->Length + " Bytes\n" +
						"Compression ratio:\t" + Math::Round(1.0 * insize / fout->Length, 3) + "\n" +
						"  Data........:\t" + bytes->num_data_bytes + " Bytes\n" +
						"  Header....:\t" + bytes->num_header_bytes + " Bytes\n" +
						"  Ext. Pred..:\t" + bytes->num_extPred_bytes + " Bytes\n" +
						"  H26x Pred.:\t" + bytes->num_H26xPred_bytes + " Bytes\n" +
						"  Hist. Mode:\t" + bytes->num_histMode_bytes + " Bytes\n" +
						"  Palette.....:\t" + bytes->num_palette_bytes + " Bytes\n" +
						"Bitrate [bpp]:\t" + Math::Round( 
									8.0 * fout->Length / (picdata->height * picdata->width), 3) + "\n",
						"Compression done.", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}

				if (aC->coding_method == HXC2) 
				{
					/*  */
					MessageBox::Show(
						"HXC2 Coding succesfully executed! \n\n" +
						"Duration:\t" + Math::Round(span.TotalSeconds, 1) + "s\n" + 
						"Number of colours:\t" + iF->numColours + "\n" + 
						"Colour space:\t" + ct_string + "\n" + 
						"Transparency:\t" + picdata->transparency + "\n" + 
						"Size original:\t" + insize + " Bytes\n" + 
						"Size compressed:\t" + fout->Length + " Bytes\n" +
						"Compression ratio:\t" + Math::Round(1.0 * insize / fout->Length, 3) + "\n" +
						"  Data........:\t" + bytes->num_data_bytes + " Bytes\n" +
						"  Header....:\t" + bytes->num_header_bytes + " Bytes\n" +
						"  Hist. Mode:\t" + bytes->num_histMode_bytes + " Bytes\n" +
						"Bitrate [bpp]:\t" + Math::Round( 
									8.0 * fout->Length / (picdata->height * picdata->width), 3) + "\n",
						"Compression done.", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}
				if (aC->coding_method == SCF) 
				{
					/*  */
					MessageBox::Show(
						"SCF succesfully executed! \n\n" +
						"Duration:\t" + Math::Round(span.TotalSeconds, 1) + "s\n" + 
						"Number of colours:\t" + iF->numColours + "\n" + 
						"Colour space:\t" + ct_string + "\n" + 
						"Transparency:\t" + picdata->transparency + "\n" + 
						"Size original:\t" + insize + " Bytes\n" + 
						"Size compressed:\t" + fout->Length + " Bytes\n" +
						"Compression ratio:\t" + Math::Round(1.0 * insize / fout->Length, 3) + "\n" +
						"  Data........:\t" + bytes->num_data_bytes + " Bytes\n" +
						"  Header....:\t" + bytes->num_header_bytes + " Bytes\n" +
						"  Hist. Mode:\t" + bytes->num_histMode_bytes + " Bytes\n" +
						"Bitrate [bpp]:\t" + Math::Round( 
									8.0 * fout->Length / (picdata->height * picdata->width), 3) + "\n",
						"Compression done.", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}
				if (aC->coding_method == CoBaLP2) 
				{
					/*  */
					MessageBox::Show(
						"CoBaLP succesfully executed! \n\n" +
						"Duration:\t" + Math::Round(span.TotalSeconds, 1) + "s\n" + 
						"Number of colours:\t" + iF->numColours + "\n" + 
						"Colour space:\t" + ct_string + "\n" + 
						"Transparency:\t" + picdata->transparency + "\n" + 
						"Hist Mode:\t" + histMode_string + "\n" + 
						"Entropy comp. 0:\t" + Math::Round(iv->entropy[0], 3) + " bpp\n" + 
						"Entropy comp. 1:\t" + Math::Round(iv->entropy[1], 3) + " bpp\n" + 
						"Entropy comp. 2:\t" + Math::Round(iv->entropy[2], 3) + " bpp\n" + 
						"Size original:\t" + insize + " Bytes\n" + 
						"Size compressed:\t" + fout->Length + " Bytes\n" +
						"  Data........:\t" + bytes->num_data_bytes + " Bytes\n" +
						"  Header....:\t" + bytes->num_header_bytes + " Bytes\n" +
						"  Hist. Mode:\t" + bytes->num_histMode_bytes + " Bytes\n" +
						"  Palette.....:\t" + bytes->num_palette_bytes + " Bytes\n" +
						"Compression ratio:\t" + Math::Round(1.0 * insize / fout->Length, 3) + "\n" +
						"Bitrate [bpp]:\t" + Math::Round( 
									8.0 * fout->Length / (picdata->height * picdata->width), 3) + "\n",
						"Compression done.", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}

				if (aC->coding_method == BPC) 
				{
					MessageBox::Show(
						"Bit Plane Coding succesfully executed! \n\n" +
						"Duration:\t" + Math::Round(span.TotalSeconds, 1) + "s\n" +
						"Number of colours:\t" + iF->numColours + "\n" + 
						"Transparency:\t" + picdata->transparency + "\n" + 
						"Colour space:\t" + ct_string + "\n" + 
						"Hist Mode:\t" + histMode_string + "\n" + 
						"Size original:\t" + insize + " Bytes\n" + 
						"Size compressed:\t" + fout->Length + " Bytes\n" +
						"  Data........:\t" + bytes->num_data_bytes + " Bytes\n" +
						"  Header....:\t" + bytes->num_header_bytes + " Bytes\n" +
						"  Ext. Pred..:\t" + bytes->num_extPred_bytes + " Bytes\n" +
						"  Hist. Mode:\t" + bytes->num_histMode_bytes + " Bytes\n" +
						"  Palette.....:\t" + bytes->num_palette_bytes + " Bytes\n" +
						"Compression ratio:\t" + Math::Round(1.0 * insize / fout->Length, 3) + "\n" + 
						"Bitrate [bpp]:\t" + Math::Round( 
									8.0 * fout->Length / (picdata->height * picdata->width), 3) + "\n",
						"Compression done.", MessageBoxButtons::OK, MessageBoxIcon::Information);
				}
			}

			/* Parameter löschen */
			if (aC->predMode == ADAPT)
			{
				freeParameter( cparam.predparam);
			}

			if (aC->predMode == H26x)
			{
				FREE(aC->H26x_Set->config);
				FREE(aC->H26x_Set);
			}
			DeletePicData( tileData);
		} /* for all tiles	*/

		/* set pointer to original image	*/
		//picdata = tmpPD;

		/* ---------------------------------------
		* do the logging of entire image
		*/
		if (log_flag && numberOfTiles > 1)
		{
			char *Cfilename = NULL;
			int is_ASCII_flag;
			String ^ascii_name;
			String ^%asciiName = ascii_name;

			fprintf( stderr, "\n log for entire image ...");
			log_name = "log_TSIP.txt\0"; /* overwrites the GUI settings :-( */

			/* ----------------------------------------------------------
			 * check filename
			 * if Unicode character(s), then create something usful
			 */
			is_ASCII_flag = check_filename( inFilename, asciiName, 0);
			Cfilename = (char *)(void *)Marshal::StringToHGlobalAnsi( asciiName);

			/* check, whether all tiles are coded with the same method	*/
			original_aC.coding_method = usedMethods[0];
			for ( i = 1; i < (signed)numberOfTiles; i++)
			{
				if (usedMethods[i] != usedMethods[0])
				{
					original_aC.coding_method = MiXeD;
					break;
				}
			}
			/* check, whether all tiles are coded with the same colour space	*/
			original_aC.rct_type_used = usedColourSpaces[0];

			/* get RCT struing from colour-space number	*/
			//ct_string = rct2string( &original_aC);
			ct_string = rct2string( original_aC.rct_type_used);

			for ( i = 1; i < (signed)numberOfTiles; i++)
			{
				if (usedColourSpaces[i] != usedColourSpaces[0])
				{
					ct_string = "mixed  ";
					break;
				}
			}

			fout = gcnew FileInfo( ouputFileName);
			/* final logging for images compressed in tiles	*/
			logResultsToFile( log_name, span, ct_string, modif_flag, fout, 
				Cfilename, picdata, &original_aC, &original_iF, iv, &cparam, bytes,
				1);

			Marshal::FreeHGlobal( IntPtr( Cfilename));
		}

		if (numberOfTiles > 1)
		{
			FREE( usedMethods);
			FREE( usedColourSpaces);
		}


		//DeletePicData( tmp);
		StatusLabel->Text = "Width: " + 
				pictureBox1->Image->Width.ToString() + 
				"px, Height: " + 
				pictureBox1->Image->Height.ToString() +
				"px, File " + Path::GetFileName( cfg->changedfile) + " has been saved";
	}
}



