/*****************************************************
* File..: ConfigForm.c
* Desc..: parameter-selection dialog and functions
* Author: T. Strutz
* Date..: 12.05.2011
* 03.05.2011 Strutz: new colour transform rYUV
* 17.06.2011 Strutz: YY|uvuv, wenn nicht RGB
* 04.07.2011 include T4
* 02.02.2012 extern autoConfig()
* 17.02.2012 ac->postBWTval must be set according MTF/IFC selection
 * 27.08.2013 type CompMode for ac->compression_mode
 * 02.09.2013 new HCXmode
 * 31.01.2014 autoConfig() w/o return value
 * 10.02.2014 new HXCauto 
 * 02.12.2014 new flags: 	aC->use_colFlag,	aC->use_predFlag 
 * 29.07.2015 aC->rct_manual_flag set 
 * 14.11.2016 include locoImproveFlag
 * 15.08.2017 new 	LSpred_searchSpace;	LSpred_numOfPreds, useCorrelation_flag
 * 21.08.2017 include LMSpredictor
 * 05.09.2017 include useColPrediction_flag
 ******************************************************/
#include "stdafx.h"
#include "ConfigForm.h"
#include "stats.h"
#include "image.h"
#include "autoConfig.h"

using namespace bacoder;


/*--------------------------------------------------------
* buttonAuto_Click()
* Auto Parameter settings in Config dialog
*-------------------------------------------------------*/
//	System::Void 
void ConfigForm::buttonAuto_Click(System::Object^  sender, System::EventArgs^  e) 
{
	double entropy[3]; /* entropies of colour components	*/
	IMAGEc image, *im;

	e=e; sender=sender;
	im = &image;

	/* Eingaben blockieren */
	this->Enabled = false;

	// modif_flag = 0; /* reset all manual modification of parameters */
	autoConfig( curpic, imageF, autoC, entropy, 0);
	InsertParams( curpic, textBoxFilename->Text, imageF, autoC, 0);

	/* GUI freischalten */
	this->Enabled = true;
} /* AUTO parameter settings */

/*--------------------------------------------------------
* getSelectedRCT()
*-------------------------------------------------------*/
//System::
Void ConfigForm::getSelectedRCT( void)
{
	if (groupBoxColourSpace->Enabled)
	{
		if (radioButtonRGB->Checked == true) { autoC->rct_type_used = RGBc;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA1_1->Checked == true) { autoC->rct_type_used = A1_1;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA1_2->Checked == true) { autoC->rct_type_used = A1_2;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA1_3->Checked == true) { autoC->rct_type_used = A1_3;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA1_4->Checked == true) { autoC->rct_type_used = A1_4;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA1_5->Checked == true) { autoC->rct_type_used = A1_5;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA1_6->Checked == true) { autoC->rct_type_used = A1_6;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA1_7->Checked == true) { autoC->rct_type_used = A1_7;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA1_8->Checked == true) { autoC->rct_type_used = A1_8;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA1_9->Checked == true) { autoC->rct_type_used = A1_9;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA1_10->Checked == true) { autoC->rct_type_used = A1_10;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA1_11->Checked == true) { autoC->rct_type_used = A1_11;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA1_12->Checked == true) { autoC->rct_type_used = A1_12;  autoC->rct_manual_flag = 1; return;}

		if (radioButtonA2_1->Checked == true)  { autoC->rct_type_used = A2_1;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA2_2->Checked == true)  { autoC->rct_type_used = A2_2;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA2_3->Checked == true)  { autoC->rct_type_used = A2_3;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA2_4->Checked == true)  { autoC->rct_type_used = A2_4;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA2_5->Checked == true)  { autoC->rct_type_used = A2_5;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA2_6->Checked == true)  { autoC->rct_type_used = A2_6;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA2_7->Checked == true)  { autoC->rct_type_used = A2_7;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA2_8->Checked == true)  { autoC->rct_type_used = A2_8;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA2_9->Checked == true)  { autoC->rct_type_used = A2_9;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA2_10->Checked == true) { autoC->rct_type_used = A2_10;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA2_11->Checked == true) { autoC->rct_type_used = A2_11;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA2_12->Checked == true) { autoC->rct_type_used = A2_12;  autoC->rct_manual_flag = 1; return;}

		if (radioButtonA3_1->Checked == true)  { autoC->rct_type_used = A3_1;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA3_2->Checked == true)  { autoC->rct_type_used = A3_2;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA3_3->Checked == true)  { autoC->rct_type_used = A3_3;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA3_4->Checked == true)  { autoC->rct_type_used = A3_4;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA3_5->Checked == true)  { autoC->rct_type_used = A3_5;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA3_6->Checked == true)  { autoC->rct_type_used = A3_6;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA3_7->Checked == true)  { autoC->rct_type_used = A3_7;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA3_8->Checked == true)  { autoC->rct_type_used = A3_8;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA3_9->Checked == true)  { autoC->rct_type_used = A3_9;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA3_10->Checked == true) { autoC->rct_type_used = A3_10;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA3_11->Checked == true) { autoC->rct_type_used = A3_11;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA3_12->Checked == true) { autoC->rct_type_used = A3_12;  autoC->rct_manual_flag = 1; return;}

		if (radioButtonA4_1->Checked == true)  { autoC->rct_type_used = A4_1;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA4_2->Checked == true)  { autoC->rct_type_used = A4_2;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA4_3->Checked == true)  { autoC->rct_type_used = A4_3;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA4_4->Checked == true)  { autoC->rct_type_used = A4_4;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA4_5->Checked == true)  { autoC->rct_type_used = A4_5;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA4_6->Checked == true)  { autoC->rct_type_used = A4_6;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA4_7->Checked == true)  { autoC->rct_type_used = A4_7;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA4_8->Checked == true)  { autoC->rct_type_used = A4_8;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA4_9->Checked == true)  { autoC->rct_type_used = A4_9;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA4_10->Checked == true) { autoC->rct_type_used = A4_10;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA4_11->Checked == true) { autoC->rct_type_used = A4_11;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA4_12->Checked == true) { autoC->rct_type_used = A4_12;  autoC->rct_manual_flag = 1; return;}

		if (radioButtonA5_1->Checked == true)  { autoC->rct_type_used = A5_1;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA5_2->Checked == true)  { autoC->rct_type_used = A5_2;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA5_3->Checked == true)  { autoC->rct_type_used = A5_3;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA5_4->Checked == true)  { autoC->rct_type_used = A5_4;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA5_5->Checked == true)  { autoC->rct_type_used = A5_5;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA5_6->Checked == true)  { autoC->rct_type_used = A5_6;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA5_7->Checked == true)  { autoC->rct_type_used = A5_7;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA5_8->Checked == true)  { autoC->rct_type_used = A5_8;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA5_9->Checked == true)  { autoC->rct_type_used = A5_9;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA5_10->Checked == true) { autoC->rct_type_used = A5_10;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA5_11->Checked == true) { autoC->rct_type_used = A5_11;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA5_12->Checked == true) { autoC->rct_type_used = A5_12;  autoC->rct_manual_flag = 1; return;}

		if (radioButtonA6_1->Checked == true)  { autoC->rct_type_used = A6_1;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA6_2->Checked == true)  { autoC->rct_type_used = A6_2;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA6_3->Checked == true)  { autoC->rct_type_used = A6_3;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA6_4->Checked == true)  { autoC->rct_type_used = A6_4;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA6_5->Checked == true)  { autoC->rct_type_used = A6_5;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA6_6->Checked == true)  { autoC->rct_type_used = A6_6;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA6_7->Checked == true)  { autoC->rct_type_used = A6_7;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA6_8->Checked == true)  { autoC->rct_type_used = A6_8;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA6_9->Checked == true)  { autoC->rct_type_used = A6_9;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA6_10->Checked == true) { autoC->rct_type_used = A6_10;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA6_11->Checked == true) { autoC->rct_type_used = A6_11;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA6_12->Checked == true) { autoC->rct_type_used = A6_12;  autoC->rct_manual_flag = 1; return;}

		if (radioButtonA7_1->Checked == true)  { autoC->rct_type_used = A7_1;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA7_2->Checked == true)  { autoC->rct_type_used = A7_2;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA7_3->Checked == true)  { autoC->rct_type_used = A7_3;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA7_4->Checked == true)  { autoC->rct_type_used = A7_4;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA7_5->Checked == true)  { autoC->rct_type_used = A7_5;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA7_6->Checked == true)  { autoC->rct_type_used = A7_6;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA7_7->Checked == true)  { autoC->rct_type_used = A7_7;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA7_8->Checked == true)  { autoC->rct_type_used = A7_8;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA7_9->Checked == true)  { autoC->rct_type_used = A7_9;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA7_10->Checked == true) { autoC->rct_type_used = A7_10;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA7_11->Checked == true) { autoC->rct_type_used = A7_11;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA7_12->Checked == true) { autoC->rct_type_used = A7_12;  autoC->rct_manual_flag = 1; return;}

		if (radioButtonA8_1->Checked == true)  { autoC->rct_type_used = A8_1;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA8_2->Checked == true)  { autoC->rct_type_used = A8_2;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA8_3->Checked == true)  { autoC->rct_type_used = A8_3;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA8_4->Checked == true)  { autoC->rct_type_used = A8_4;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA8_5->Checked == true)  { autoC->rct_type_used = A8_5;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA8_6->Checked == true)  { autoC->rct_type_used = A8_6;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA8_7->Checked == true)  { autoC->rct_type_used = A8_7;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA8_8->Checked == true)  { autoC->rct_type_used = A8_8;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA8_9->Checked == true)  { autoC->rct_type_used = A8_9;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA8_10->Checked == true) { autoC->rct_type_used = A8_10;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA8_11->Checked == true) { autoC->rct_type_used = A8_11;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA8_12->Checked == true) { autoC->rct_type_used = A8_12;  autoC->rct_manual_flag = 1; return;}

		if (radioButtonA9_1->Checked == true)  { autoC->rct_type_used = A9_1;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA9_2->Checked == true)  { autoC->rct_type_used = A9_2;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA9_3->Checked == true)  { autoC->rct_type_used = A9_3;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA9_4->Checked == true)  { autoC->rct_type_used = A9_4;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA9_5->Checked == true)  { autoC->rct_type_used = A9_5;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA9_6->Checked == true)  { autoC->rct_type_used = A9_6;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA9_7->Checked == true)  { autoC->rct_type_used = A9_7;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA9_8->Checked == true)  { autoC->rct_type_used = A9_8;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA9_9->Checked == true)  { autoC->rct_type_used = A9_9;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA9_10->Checked == true) { autoC->rct_type_used = A9_10;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA9_11->Checked == true) { autoC->rct_type_used = A9_11;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonA9_12->Checked == true) { autoC->rct_type_used = A9_12;  autoC->rct_manual_flag = 1; return;}

		if (radioButtonB1_1->Checked == true)  { autoC->rct_type_used = B1_1;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonB1_2->Checked == true)  { autoC->rct_type_used = B1_2;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonB2_1->Checked == true)  { autoC->rct_type_used = B2_1;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonB2_3->Checked == true)  { autoC->rct_type_used = B2_3;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonB3_2->Checked == true)  { autoC->rct_type_used = B3_2;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonB3_3->Checked == true)  { autoC->rct_type_used = B3_3;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonB4_1->Checked == true)  { autoC->rct_type_used = B4_1;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonB5_2->Checked == true)  { autoC->rct_type_used = B5_2;  autoC->rct_manual_flag = 1; return;}
		if (radioButtonB6_3->Checked == true)  { autoC->rct_type_used = B6_3;  autoC->rct_manual_flag = 1; return;}

		if (radioButtonPEI09->Checked == true)  { autoC->rct_type_used = PEI09;  autoC->rct_manual_flag = 1; return;}
	}
	else
	{
		autoC->rct_type_used = GREY;
	}
}

/*--------------------------------------------------------
* buttonCompress_Click()
* Next/Compress button in Config dialog
*-------------------------------------------------------*/
//System::
Void ConfigForm::buttonCompress_Click(System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;


	try	{autoC->RCTpercentage = (unsigned char)Convert::ToInt32(textBox_RCTpercentage->Text);}
	catch (...)	{ autoC->RCTpercentage = 0; }					
	if (autoC->RCTpercentage < 1 || autoC->RCTpercentage > 100)
	{
				MessageBox::Show("RCT percentage parameter must be between 1% and 100%",
					"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
				textBoxMTF->Text = "1";
				return;
	}
	try	{autoC->tileWidth  = Convert::ToInt32(textBoxTileWidth->Text);}
	catch (...)	{ autoC->tileWidth = 0; }					
	if (autoC->tileWidth < 64 && autoC->tileWidth < curpic->width)
	{
		MessageBox::Show("Tile-width value must be at least equal to 64 or image width",
			"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
		if (curpic->width < 64 ) 		textBoxTileWidth->Text = (curpic->width).ToString();
		else		textBoxTileWidth->Text = "64";
		return;
	}
	try	{autoC->tileHeight  = Convert::ToInt32(textBoxTileHeight->Text);}
	catch (...)	{ autoC->tileHeight = 0; }					
	if (autoC->tileHeight < 64 && autoC->tileHeight < curpic->height)
	{
		MessageBox::Show("Tile-height value must be at least equal to 64 or image height",
			"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
		if (curpic->height < 64 ) 		textBoxTileHeight->Text = (curpic->height).ToString();
		else		textBoxTileWidth->Text = "64";
		return;
	}

	
	// Strutz: why not setting an arbitrary large size?
	//         limitation is given by computer memory
	if (autoC->tileWidth > maxTileWidth && max( curpic->height, curpic->width) > maxTileWidth )
	{
		MessageBox::Show("Tile width must not be higher than " + maxTileWidth,
			"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
		textBoxTileWidth->Text = maxTileWidth.ToString();
		return;
	}
	if (autoC->tileHeight > maxTileHeight && max( curpic->height, curpic->width) > maxTileHeight )
	{
		MessageBox::Show("Tile width must not be higher than " + maxTileHeight,
			"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
		textBoxTileHeight->Text = maxTileHeight.ToString();
		return;
	}

	autoC->indexed_flag = 0;
	/* Farbraum holen */
	if (groupBoxColourSpace->Enabled)
	{
		if (radioButtonRGB->Checked == true) autoC->rct_type_used = RGBc;
		if (radioButtonA1_1->Checked == true) autoC->rct_type_used = A1_1;
		if (radioButtonA1_2->Checked == true) autoC->rct_type_used = A1_2;
		if (radioButtonA1_3->Checked == true) autoC->rct_type_used = A1_3;
		if (radioButtonA1_4->Checked == true) autoC->rct_type_used = A1_4;
		if (radioButtonA1_5->Checked == true) autoC->rct_type_used = A1_5;
		if (radioButtonA1_6->Checked == true) autoC->rct_type_used = A1_6;
		if (radioButtonA1_7->Checked == true) autoC->rct_type_used = A1_7;
		if (radioButtonA1_8->Checked == true) autoC->rct_type_used = A1_8;
		if (radioButtonA1_9->Checked == true) autoC->rct_type_used = A1_9;
		if (radioButtonA1_10->Checked == true) autoC->rct_type_used = A1_10;
		if (radioButtonA1_11->Checked == true) autoC->rct_type_used = A1_11;
		if (radioButtonA1_12->Checked == true) autoC->rct_type_used = A1_12;

		if (radioButtonA2_1->Checked == true)  autoC->rct_type_used = A2_1;
		if (radioButtonA2_2->Checked == true)  autoC->rct_type_used = A2_2;
		if (radioButtonA2_3->Checked == true)  autoC->rct_type_used = A2_3;
		if (radioButtonA2_4->Checked == true)  autoC->rct_type_used = A2_4;
		if (radioButtonA2_5->Checked == true)  autoC->rct_type_used = A2_5;
		if (radioButtonA2_6->Checked == true)  autoC->rct_type_used = A2_6;
		if (radioButtonA2_7->Checked == true)  autoC->rct_type_used = A2_7;
		if (radioButtonA2_8->Checked == true)  autoC->rct_type_used = A2_8;
		if (radioButtonA2_9->Checked == true)  autoC->rct_type_used = A2_9;
		if (radioButtonA2_10->Checked == true) autoC->rct_type_used = A2_10;
		if (radioButtonA2_11->Checked == true) autoC->rct_type_used = A2_11;
		if (radioButtonA2_12->Checked == true) autoC->rct_type_used = A2_12;

		if (radioButtonA3_1->Checked == true)  autoC->rct_type_used = A3_1;
		if (radioButtonA3_2->Checked == true)  autoC->rct_type_used = A3_2;
		if (radioButtonA3_3->Checked == true)  autoC->rct_type_used = A3_3;
		if (radioButtonA3_4->Checked == true)  autoC->rct_type_used = A3_4;
		if (radioButtonA3_5->Checked == true)  autoC->rct_type_used = A3_5;
		if (radioButtonA3_6->Checked == true)  autoC->rct_type_used = A3_6;
		if (radioButtonA3_7->Checked == true)  autoC->rct_type_used = A3_7;
		if (radioButtonA3_8->Checked == true)  autoC->rct_type_used = A3_8;
		if (radioButtonA3_9->Checked == true)  autoC->rct_type_used = A3_9;
		if (radioButtonA3_10->Checked == true) autoC->rct_type_used = A3_10;
		if (radioButtonA3_11->Checked == true) autoC->rct_type_used = A3_11;
		if (radioButtonA3_12->Checked == true) autoC->rct_type_used = A3_12;

		if (radioButtonA4_1->Checked == true)  autoC->rct_type_used = A4_1;
		if (radioButtonA4_2->Checked == true)  autoC->rct_type_used = A4_2;
		if (radioButtonA4_3->Checked == true)  autoC->rct_type_used = A4_3;
		if (radioButtonA4_4->Checked == true)  autoC->rct_type_used = A4_4;
		if (radioButtonA4_5->Checked == true)  autoC->rct_type_used = A4_5;
		if (radioButtonA4_6->Checked == true)  autoC->rct_type_used = A4_6;
		if (radioButtonA4_7->Checked == true)  autoC->rct_type_used = A4_7;
		if (radioButtonA4_8->Checked == true)  autoC->rct_type_used = A4_8;
		if (radioButtonA4_9->Checked == true)  autoC->rct_type_used = A4_9;
		if (radioButtonA4_10->Checked == true) autoC->rct_type_used = A4_10;
		if (radioButtonA4_11->Checked == true) autoC->rct_type_used = A4_11;
		if (radioButtonA4_12->Checked == true) autoC->rct_type_used = A4_12;

		if (radioButtonA5_1->Checked == true)  autoC->rct_type_used = A5_1;
		if (radioButtonA5_2->Checked == true)  autoC->rct_type_used = A5_2;
		if (radioButtonA5_3->Checked == true)  autoC->rct_type_used = A5_3;
		if (radioButtonA5_4->Checked == true)  autoC->rct_type_used = A5_4;
		if (radioButtonA5_5->Checked == true)  autoC->rct_type_used = A5_5;
		if (radioButtonA5_6->Checked == true)  autoC->rct_type_used = A5_6;
		if (radioButtonA5_7->Checked == true)  autoC->rct_type_used = A5_7;
		if (radioButtonA5_8->Checked == true)  autoC->rct_type_used = A5_8;
		if (radioButtonA5_9->Checked == true)  autoC->rct_type_used = A5_9;
		if (radioButtonA5_10->Checked == true) autoC->rct_type_used = A5_10;
		if (radioButtonA5_11->Checked == true) autoC->rct_type_used = A5_11;
		if (radioButtonA5_12->Checked == true) autoC->rct_type_used = A5_12;

		if (radioButtonA6_1->Checked == true)  autoC->rct_type_used = A6_1;
		if (radioButtonA6_2->Checked == true)  autoC->rct_type_used = A6_2;
		if (radioButtonA6_3->Checked == true)  autoC->rct_type_used = A6_3;
		if (radioButtonA6_4->Checked == true)  autoC->rct_type_used = A6_4;
		if (radioButtonA6_5->Checked == true)  autoC->rct_type_used = A6_5;
		if (radioButtonA6_6->Checked == true)  autoC->rct_type_used = A6_6;
		if (radioButtonA6_7->Checked == true)  autoC->rct_type_used = A6_7;
		if (radioButtonA6_8->Checked == true)  autoC->rct_type_used = A6_8;
		if (radioButtonA6_9->Checked == true)  autoC->rct_type_used = A6_9;
		if (radioButtonA6_10->Checked == true) autoC->rct_type_used = A6_10;
		if (radioButtonA6_11->Checked == true) autoC->rct_type_used = A6_11;
		if (radioButtonA6_12->Checked == true) autoC->rct_type_used = A6_12;

		if (radioButtonA7_1->Checked == true)  autoC->rct_type_used = A7_1;
		if (radioButtonA7_2->Checked == true)  autoC->rct_type_used = A7_2;
		if (radioButtonA7_3->Checked == true)  autoC->rct_type_used = A7_3;
		if (radioButtonA7_4->Checked == true)  autoC->rct_type_used = A7_4;
		if (radioButtonA7_5->Checked == true)  autoC->rct_type_used = A7_5;
		if (radioButtonA7_6->Checked == true)  autoC->rct_type_used = A7_6;
		if (radioButtonA7_7->Checked == true)  autoC->rct_type_used = A7_7;
		if (radioButtonA7_8->Checked == true)  autoC->rct_type_used = A7_8;
		if (radioButtonA7_9->Checked == true)  autoC->rct_type_used = A7_9;
		if (radioButtonA7_10->Checked == true) autoC->rct_type_used = A7_10;
		if (radioButtonA7_11->Checked == true) autoC->rct_type_used = A7_11;
		if (radioButtonA7_12->Checked == true) autoC->rct_type_used = A7_12;

		if (radioButtonA8_1->Checked == true)  autoC->rct_type_used = A8_1;
		if (radioButtonA8_2->Checked == true)  autoC->rct_type_used = A8_2;
		if (radioButtonA8_3->Checked == true)  autoC->rct_type_used = A8_3;
		if (radioButtonA8_4->Checked == true)  autoC->rct_type_used = A8_4;
		if (radioButtonA8_5->Checked == true)  autoC->rct_type_used = A8_5;
		if (radioButtonA8_6->Checked == true)  autoC->rct_type_used = A8_6;
		if (radioButtonA8_7->Checked == true)  autoC->rct_type_used = A8_7;
		if (radioButtonA8_8->Checked == true)  autoC->rct_type_used = A8_8;
		if (radioButtonA8_9->Checked == true)  autoC->rct_type_used = A8_9;
		if (radioButtonA8_10->Checked == true) autoC->rct_type_used = A8_10;
		if (radioButtonA8_11->Checked == true) autoC->rct_type_used = A8_11;
		if (radioButtonA8_12->Checked == true) autoC->rct_type_used = A8_12;

		if (radioButtonA9_1->Checked == true)  autoC->rct_type_used = A9_1;
		if (radioButtonA9_2->Checked == true)  autoC->rct_type_used = A9_2;
		if (radioButtonA9_3->Checked == true)  autoC->rct_type_used = A9_3;
		if (radioButtonA9_4->Checked == true)  autoC->rct_type_used = A9_4;
		if (radioButtonA9_5->Checked == true)  autoC->rct_type_used = A9_5;
		if (radioButtonA9_6->Checked == true)  autoC->rct_type_used = A9_6;
		if (radioButtonA9_7->Checked == true)  autoC->rct_type_used = A9_7;
		if (radioButtonA9_8->Checked == true)  autoC->rct_type_used = A9_8;
		if (radioButtonA9_9->Checked == true)  autoC->rct_type_used = A9_9;
		if (radioButtonA9_10->Checked == true) autoC->rct_type_used = A9_10;
		if (radioButtonA9_11->Checked == true) autoC->rct_type_used = A9_11;
		if (radioButtonA9_12->Checked == true) autoC->rct_type_used = A9_12;

		if (radioButtonB1_1->Checked == true)  autoC->rct_type_used = B1_1;
		if (radioButtonB1_2->Checked == true)  autoC->rct_type_used = B1_2;
		if (radioButtonB2_1->Checked == true)  autoC->rct_type_used = B2_1;
		if (radioButtonB2_3->Checked == true)  autoC->rct_type_used = B2_3;
		if (radioButtonB3_2->Checked == true)  autoC->rct_type_used = B3_2;
		if (radioButtonB3_3->Checked == true)  autoC->rct_type_used = B3_3;
		if (radioButtonB4_1->Checked == true)  autoC->rct_type_used = B4_1;
		if (radioButtonB5_2->Checked == true)  autoC->rct_type_used = B5_2;
		if (radioButtonB6_3->Checked == true)  autoC->rct_type_used = B6_3;

		if (radioButtonPEI09->Checked == true)  autoC->rct_type_used = PEI09;
		if (radioButtonA7_1s->Checked == true)  autoC->rct_type_used = A7_1s;
		if (radioButtonA7_1m->Checked == true)  autoC->rct_type_used = A7_1m;
	}
	else	if (checkBoxIndexedColours->Checked) 
	{
		autoC->indexed_flag = 1;
		if (checkBoxIndexedAdjacency->Checked)
		{
			autoC->indexed_adj = 1;
			if (checkBoxIndexedArith->Checked)
			{
				autoC->indexed_arith = 1;
			}
		}
		// autoC->histMode = NoMOD;
		if (radioButtonLuminanceSorting->Checked)
		{
			autoC->palette_sorting = 1;
		}
		else
		{
			autoC->palette_sorting = 0;
		}
	}
	else if (radioButtonHXC2mode->Checked)
	{
		autoC->rct_type_used = RGBc;
	}
	else
	{
		autoC->rct_type_used = GREY;
	}

	/* set LOCO parameters	*/
	// testing checkBoxT4->Enabled could result in change T4_flag without changing
	// any option ==> will caus modif_flag = true
	//if (checkBoxT4->Enabled == true && checkBoxT4->Checked == true) autoC->T4_flag = 1;
	//else autoC->T4_flag = 0;
	if (checkBoxT4->Checked == true) autoC->T4_flag = 1;
	else autoC->T4_flag = 0;
	if (checkBoxImprovedk->Checked == true) autoC->locoImproveFlag = 1;
	else autoC->locoImproveFlag = 0;

	/* set CoBaLP2 parameters	*/
	autoC->CoBaLP2_sA = (checkBoxSkipPredA->Checked == true) ? 1 : 0;
	autoC->CoBaLP2_sB = (checkBoxSkipPredB->Checked == true) ? 1 : 0;
	autoC->CoBaLP2_aR = (checkBoxEnablePredR->Checked == true) ? 1 : 0;
	//autoC->CoBaLP2_aL = (checkBoxEnablePredL->Checked == true) ? 1 : 0;
	autoC->CoBaLP2_sT = (checkBoxSkipTM->Checked == true) ? 1 : 0;
	autoC->CoBaLP2_constLines = (checkBoxConstLines->Checked == true) ? 1 : 0;

	/* set histogram modification mode	*/
	if (radioButtonHistogramNoneMod->Checked == true) autoC->histMode = NoMOD;
	else if (radioButtonHistogramCompaction->Checked == true) autoC->histMode = COMPACTION;
	else if (radioButtonHistogramPermutation->Checked == true) autoC->histMode = PERMUTATION;
	else if (radioButtonHistogramPermuteTSP->Checked == true) autoC->histMode = PERMUTE_2OPT;

	/* PredictionType holen */
	if (groupBoxPrediction->Enabled == true)
	{
		if (radioButtonNoPred->Checked  == true) autoC->predMode = NONE;
		if (radioButtonLeftNeighbour->Checked  == true) autoC->predMode = LEFTNEIGHBOUR;
		if (radioButtonPaeth->Checked  == true) autoC->predMode = PAETH;
		if (radioButtonMEDPAETH->Checked  == true) autoC->predMode = MEDPAETH;
		if (radioButtonMED->Checked  == true) autoC->predMode = MED;
		if (radioButtonExtern->Checked == true) autoC->predMode = ADAPT;
		if (radioButtonH26x->Checked == true) autoC->predMode = H26x;	
		if (radioButtonLSpred->Checked == true)
		{
			autoC->predMode = LSBLEND;
			autoC->LSpred_numOfPreds  = Convert::ToByte(textBoxNumOfPreds->Text);
			autoC->LSpred_searchSpace  = Convert::ToByte(textBoxSearchSpace->Text);
			if (checkBoxUseCorr->Checked == true) autoC->useCorrelation_flag = 1;	
			else autoC->useCorrelation_flag = 0;	
		}
		if (radioButtonLSpredHo->Checked == true)
		{
			autoC->predMode = LSPRED;	
			autoC->LSpred_numOfPreds  = Convert::ToByte(textBoxNumOfPreds->Text);
			autoC->LSpred_searchSpace  = Convert::ToByte(textBoxSearchSpace->Text);
		}
		if (radioButtonLMSpred->Checked == true)
		{
			autoC->predMode = LMSPRED;	
			autoC->LSpred_numOfPreds  = Convert::ToByte(textBoxNumOfPreds->Text);
			autoC->LSpred_searchSpace  = Convert::ToByte(textBoxSearchSpace->Text);
			if (checkBoxUseColPred->Checked == true) autoC->useColPrediction_flag = 1;	
			else autoC->useColPrediction_flag = 0;	
			if (checkBoxUseColPredParal->Checked == true) autoC->useColPredictionP_flag = 1;	
			else autoC->useColPredictionP_flag = 0;	
		}
	}
	else
	{
		autoC->predMode = NONE;
	}

	/* get size of segments */
	// already above !!
	//try	{
	//	unsigned long tmp;
	//	 tmp = Convert::ToInt32(textBoxTileWidth->Text);
	//	 //tmp = Convert::ToInt32(textBoxBlockWidth->Text);
	//	 autoC->tileWidth = tmp;
	//} 
	//catch (...) { autoC->tileWidth = 0; }

	/* set interleaving */
	if (radioButtonYuvYuvJointly->Checked == true)
	{
		autoC->interleaving_mode = YuvYuv; /* defined in autoConfig.h */
	}
	else if (radioButtonYYuvuv->Checked == true)
	{
		autoC->interleaving_mode = YY_uvuv;
	}
	else if (radioButtonYY_uuvv->Checked == true)
	{
		autoC->interleaving_mode = YY_uuvv;
	}
	else if (radioButtonYYuuvv->Checked == true)
	{
		autoC->interleaving_mode = YYuuvv;
	}
	else
	{
		autoC->interleaving_mode = Y_U_V;
	}

	if (checkBoxSkipPrecoding->Checked)
	{
		/* ensure that all precoding is skipped */
		autoC->skip_rlc1 = true;
		autoC->skip_postBWT = true;
		autoC->use_RLC2zero = false;
	}
	else
	{
	//}
	/* all parameters should be copied, because otherwise the 
	 * aC-Structure is filled with different values preventing the
	 * indication of AUTO settings
	 */
	//{
		autoC->skip_rlc1 = checkBoxSkipRLC->Checked ? 1u : 0;
		/* skip Post BWT */
		autoC->skip_postBWT = checkBoxSkipPostBWT->Checked ? 1u : 0;

		/* PostBWT Verfahren */
		if (radioButtonMTF->Checked == true)
		{
			autoC->postbwt = MTF;
			try	{autoC->MTF_val  = Convert::ToInt32(textBoxMTF->Text);}
			catch (...)	{ autoC->MTF_val = 100; }					
			if (autoC->MTF_val < 0 || autoC->MTF_val > 95)
			{
				MessageBox::Show("MTF parameter must be between 0 and 95",
					"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
				textBoxMTF->Text = "0";
				return;
			}
			// autoC->postBWTparam  = autoC->MTF_val; Strutz ??? problems wit AUTO indication
		}
		else if (radioButtonIFC->Checked == true)
		{
			autoC->postbwt = IFC;
			try	{autoC->IFCresetVal  = Convert::ToInt32(textBoxIFCresetVal->Text);}
			catch (...)	{ autoC->IFCresetVal = 0; }					
			if (autoC->IFCresetVal < 2 || autoC->IFCresetVal > 10000)
			{
				MessageBox::Show("IFC reset value must be between 2 and 10000",
					"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
				textBoxIFCresetVal->Text = "250";
				return;
			}
			// autoC->postBWTparam  = autoC->IFCresetVal; Strutz: problem with AUTO indication
			try	{autoC->IFCmaxCount  = Convert::ToInt32(textBoxIFCmaxCount->Text);}
			catch (...)	{ autoC->IFCmaxCount = 0; }					
			if (autoC->IFCmaxCount < 2 || autoC->IFCmaxCount > 10000)
			{
				MessageBox::Show("IFC maxCount value must be between 2 and 10000",
					"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
				textBoxIFCmaxCount->Text = "250";
				return;
			}
		}
		else /* radioButtonHXC3->Checked == true	*/
		{
			autoC->postbwt = HXC3;
		}
		/* use RLC2 with zeros */
		autoC->use_RLC2zero = checkBoxUseZeroRLC2->Checked ? 1u : 0;
	}

	/* coding options */
	if (radioButtonSeparateCoding->Checked == true)
	{
		autoC->separate_coding_flag = 1;
	}
	else
	{
		autoC->separate_coding_flag = 0;
	}
	if (radioButtonHuffmanCoding->Checked == true)
	{
		autoC->entropy_coding_method = 0;
	}
	else
	{
		autoC->entropy_coding_method = 1; /* use arithmetich coding */
	}

	if (radioButtonLOCOmode->Checked)
	{
		autoC->coding_method = LOCO;	/* LOCO	*/
	}
	else if (radioButtonTSIPmode->Checked)
	{
		autoC->coding_method = TSIP;	/* TSIP	*/
	} 
	else if (radioButtonHXCmode->Checked)
	{
		autoC->coding_method = HXC;	/* HXC	*/
	} 
	else if (radioButtonHXC2mode->Checked)
	{
		autoC->coding_method = HXC2;	/* HXC2	*/
	} 
	else if (radioButtonBitPlaneMode->Checked)
	{
		autoC->coding_method = BPC;	
	} 
	else if (radioButtonSCFmode->Checked)
	{
		autoC->coding_method = SCF;	
		autoC->SCF_directional = checkBoxSCF_directional->Checked;
		autoC->SCF_prediction = checkBoxSCFVertHoriPrediction->Checked;
		autoC->SCF_stage2tuning = checkBoxSCF_stage2tuning->Checked;
		autoC->SCF_colourMAP = checkBoxSCF_colourMAP->Checked;
		//try	{
		//	unsigned int tmp;
		//	 tmp = Convert::ToInt32(textBoxSCF_maxNumPatterns->Text);
		//	 autoC->SCF_maxNumPatterns = tmp;
		//} 
		//catch (...)	{ autoC->SCF_maxNumPatterns = 789; }					
		//if (autoC->SCF_maxNumPatterns < 1 || autoC->SCF_maxNumPatterns > 10000)
		//{
		//			MessageBox::Show("SCF_maxNumPatterns must be between 1 and 10000",
		//				"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
		//			textBoxMTF->Text = "1";
		//			return;
		//}

		autoC->SCF_maxNumPatterns = mapIdx2maxNumPatterns( trackBarPerformVSspeed->Value);

	} 
	else 
	{
		autoC->coding_method = CoBaLP2; /* CoBALP	*/
	}

	/* HXC parameters */
	try	{
		unsigned int tmp;
		 tmp = Convert::ToInt32(textBoxHXCtolerance->Text);
		 //if (tmp != autoC->HXCtolerance) modif_flag = 1;
		 autoC->HXCtolerance = tmp;

		 tmp = Convert::ToInt32(textBoxToleranceOffset1->Text);
		 // if (tmp != autoC->HXCtoleranceOffset1) modif_flag = 1;
		 autoC->HXCtoleranceOffset1 = tmp;

		 tmp = Convert::ToInt32(textBoxToleranceOffset2->Text);
		 // if (tmp != autoC->HXCtoleranceOffset2) modif_flag = 1;
		 autoC->HXCtoleranceOffset2 = tmp;

		 tmp = Convert::ToInt32(textBoxIncreaseFac->Text);
		 // if (tmp != autoC->HXCincreaseFac) modif_flag = 1;
		 autoC->HXCincreaseFac = tmp;

		 autoC->HXCauto = checkBoxHXCauto->Checked ? 1u : 0;
		 autoC->HXC2auto = checkBoxHXC2auto->Checked ? 1u : 0;
	} 
	catch (...)	{ autoC->HXCtolerance = 5; }					


	/* HXC2 parameters */
	try	{
		 autoC->HXC2tolerance[0][0] = Convert::ToInt32(textBoxTol0R->Text);
		 autoC->HXC2tolerance[0][1] = Convert::ToInt32(textBoxTol1R->Text);
		 autoC->HXC2tolerance[0][2] = Convert::ToInt32(textBoxTol2R->Text);
		 autoC->HXC2tolerance[0][3] = Convert::ToInt32(textBoxTol3R->Text);
		 autoC->HXC2tolerance[0][4] = Convert::ToInt32(textBoxTol4R->Text);
		 autoC->HXC2tolerance[0][5] = Convert::ToInt32(textBoxTol5R->Text);

		 autoC->HXC2tolerance[1][0] = Convert::ToInt32(textBoxTol0G->Text);
		 autoC->HXC2tolerance[1][1] = Convert::ToInt32(textBoxTol1G->Text);
		 autoC->HXC2tolerance[1][2] = Convert::ToInt32(textBoxTol2G->Text);
		 autoC->HXC2tolerance[1][3] = Convert::ToInt32(textBoxTol3G->Text);
		 autoC->HXC2tolerance[1][4] = Convert::ToInt32(textBoxTol4G->Text);
		 autoC->HXC2tolerance[1][5] = Convert::ToInt32(textBoxTol5G->Text);

		 autoC->HXC2tolerance[2][0] = Convert::ToInt32(textBoxTol0B->Text);
		 autoC->HXC2tolerance[2][1] = Convert::ToInt32(textBoxTol1B->Text);
		 autoC->HXC2tolerance[2][2] = Convert::ToInt32(textBoxTol2B->Text);
		 autoC->HXC2tolerance[2][3] = Convert::ToInt32(textBoxTol3B->Text);
		 autoC->HXC2tolerance[2][4] = Convert::ToInt32(textBoxTol4B->Text);
		 autoC->HXC2tolerance[2][5] = Convert::ToInt32(textBoxTol5B->Text);

		 autoC->HXC2tolerance[3][0] = Convert::ToInt32(textBoxTol0A->Text);
		 autoC->HXC2tolerance[3][1] = Convert::ToInt32(textBoxTol1A->Text);
		 autoC->HXC2tolerance[3][2] = Convert::ToInt32(textBoxTol2A->Text);
		 autoC->HXC2tolerance[3][3] = Convert::ToInt32(textBoxTol3A->Text);
		 autoC->HXC2tolerance[3][4] = Convert::ToInt32(textBoxTol4A->Text);
		 autoC->HXC2tolerance[3][5] = Convert::ToInt32(textBoxTol5A->Text);
		 autoC->HXC2increaseFac = Convert::ToInt32(textBoxIncreaseFac2->Text);
	} 
	catch (...)	{ autoC->HXC2tolerance[0][0] = 5; }					

	/* Huffman-Only-Modus */
	if (checkBoxSkipPrecoding->Checked) autoC->skip_precoding = 1;
	else autoC->skip_precoding = 0;

	/* Dateiname holen */
	changedfile = textBoxFilename->Text;
	if (changedfile == "")
	{
		MessageBox::Show("Filename is empty",
			"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
		return;
	}
	if (imageF->numColours == 0)
	{
		double colEntropy;
		unsigned long maxCount;
		imageF->numColours = GetNumColours( curpic, &maxCount, &colEntropy);
	}

	/* BPC */
	autoC->s_flg = checkBoxSignificance->Checked ? 1 :0;
	autoC->use_colFlag = checkBox_useColFlag->Checked ? 1 :0;
	autoC->use_predFlag = checkBox_usePredFlag->Checked ? 1 :0;

	/* Rückgabewert */
	result = 1u;
	this->Close();
}


/* Prädiktion ein und ausschalten */
//Void ConfigForm::checkBoxFilter_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
//{
//	if (checkBoxLOCO->Checked == false) 
//		groupBoxPrediction->Enabled = ((CheckBox^)sender)->Checked;
//	modif_flag = 1;
//	e=e;
//}

/* Skip precoding */
Void ConfigForm::checkBoxSkipPrecoding_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	//if (checkBoxLOCO->Checked == false)
	if (radioButtonTSIPmode->Checked)
	{
		if (checkBoxSkipPostBWT->Checked == false)
		{
			groupBoxPostBWT->Enabled = !(((CheckBox^ )sender)->Checked);
		}
		checkBoxSkipRLC->Enabled = !(((CheckBox^ )sender)->Checked);
		checkBoxSkipPostBWT->Enabled = !(((CheckBox^ )sender)->Checked);
		checkBoxSkipRLC->Enabled = !(((CheckBox^ )sender)->Checked);
		checkBoxUseZeroRLC2->Enabled = !(((CheckBox^ )sender)->Checked);
	}
	// modif_flag = 1;
	e=e;
}

/* Skip PostBWT */
Void ConfigForm::checkBoxSkipPostBWT_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	//if (checkBoxLOCO->Checked == false)
	if (radioButtonTSIPmode->Checked)
	{
		if (checkBoxSkipPrecoding->Checked == false)
		{
			groupBoxPostBWT->Enabled = !(((CheckBox^ )sender)->Checked);
		}
	}
	// modif_flag = 1;
	e=e;
}

/* MTF */
Void ConfigForm::radioButtonMTF_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	textBoxMTF->Enabled = ((RadioButton^)sender)->Checked;
	// modif_flag = 1;
	e=e;
}

/* IFC */
Void ConfigForm::radioButtonIFC_CheckedChanged( System::Object^ sender, System::EventArgs^ e)
{
	textBoxIFCresetVal->Enabled = ((RadioButton^)sender)->Checked;
	textBoxIFCmaxCount->Enabled = ((RadioButton^)sender)->Checked;
	// modif_flag = 1;
	e=e;
}

/* Schließen */
Void ConfigForm::buttonCancel_Click(System::Object^  sender, System::EventArgs^  e)
{
	result = 0u;
	this->Close();
	e=e; sender=sender;
}
