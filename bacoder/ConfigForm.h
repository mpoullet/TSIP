/************************************************************
 * File...: ConfigForm.h
 * Author.: Schmiedel, Strutz
 * Date...: 7.2.2011, 16.2.2011, 05.05.2011
 * 27.05.2011 Strutz: skip PostBWT
 * 03.05.2011 Strutz: new colour transform rYUV
 * 17.01.2012 Strutz: new variable synt for use in bacoder.cpp
 * 22.01.2012 Strutz: change text of button_OK if LOCO is checked
 * 12.07.2012 Titus:Changed logging to account for mapped indexing
 *  and special arithmetic coding
 * 27.07.2012 several changes
 * 30.10.2012 Stegemann: Added options for the H.26x predictor 
 * 02.09.2013 Strutz: new options (RCT, CoBaLP2, HCX,...) 
 * 06.09.2013 HCX Toelerance parameter 
 * 23.09.2013 CoBaLP2: allow indexed colours only if num of colours < 513 
 * 23.09.2013 force CheckChanged() for Loco 
 * 10.01.2014 CheckChanged() for permutation; 
 * 10.01.2014 disable permutation if numColours >2048
 * 27.01.2014 bugfix in radioCoBaLP and Bitplane: no idx_adj
 * 10.02.2014 new HXCauto 
 * 08.04.2014 reset aC->rct_type_used to rct_type_pred if CoBaLP or LOCO are enabled
 * 01.07.2014 support of tiles, segmentation for TSIP removed
 * 10.07.2014 maxTileWidth based on image format, max. 2^22 pixels
 * 02.12.2014 new flags: 	aC->use_colFlag,	aC->use_predFlag 
 * 24.04.2015 bugfix: enable groupcolourbox also if LOCO is automatically selected 
 * 30.08.2015 bugfix: in HXC2 selection, check colour space based on pred/nopred 
 * 03.09.2015 new: HXC3 option 
 * 03.11.2015- 1.12.2015 new: SCF options
 * 08.09.2016 SCF_maxNumPatterns
 * 14.11.2016 include locoImproveFlag
 * 14.08.2017 include LSpredictor
 * 21.08.2017 include LMSpredictor
 * 06.09.2017 include LMSColourPredictor
 ************************************************************/
#pragma once

#include <math.h>
#include "ginterface.h"
#include "colour4.h"
#include "autoConfig.h"
#include "codec.h" /* for min(a, b)	*/

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace bacoder
{
	public ref class ConfigForm : public System::Windows::Forms::Form
	{
	public:
		uint maxTileWidth, maxTileHeight, result;
		PicData *curpic;
		AutoConfig *autoC;	/* pointer to extern aC	*/
		ImageFeatures *imageF;	/* pointer to extern iF	*/

	private: System::Windows::Forms::GroupBox^  groupBoxCompressionMode;
	private: System::Windows::Forms::RadioButton^  radioButtonLOCOmode;
	private: System::Windows::Forms::RadioButton^  radioButtonTSIPmode;
	private: System::Windows::Forms::RadioButton^  radioButtonCoBaLP2mode;
	private: System::Windows::Forms::RadioButton^  radioButtonHXCmode;
	private: System::Windows::Forms::RadioButton^  radioButtonHXC2mode;
	private: System::Windows::Forms::RadioButton^  radioButtonBitPlaneMode;

	private: System::Windows::Forms::GroupBox^  groupBoxSegmentation;

	private: System::Windows::Forms::TextBox^  textBoxTileWidth;
	private: System::Windows::Forms::TextBox^  textBoxTileHeight;
	private: System::Windows::Forms::Label^  labelTileWidth;
	private: System::Windows::Forms::Label^  labelTileHeight;


	private: System::Windows::Forms::GroupBox^  groupBoxFilename;
	private: System::Windows::Forms::TextBox^  textBoxFilename;
	private: System::Windows::Forms::PictureBox^  pictureBox1;

	private: System::Windows::Forms::GroupBox^  groupBoxColourSpace;
	private: System::Windows::Forms::RadioButton^  radioButtonRGB;
	private: System::Windows::Forms::Label^  labelRCT_Y;
	private: System::Windows::Forms::Label^  labelRCT_UV;

	private: System::Windows::Forms::Label^  labelUV1;
	private: System::Windows::Forms::Label^  labelUV2;
	private: System::Windows::Forms::Label^  labelUV3;
	private: System::Windows::Forms::Label^  labelUV4;
	private: System::Windows::Forms::Label^  labelUV5;
	private: System::Windows::Forms::Label^  labelUV6;
	private: System::Windows::Forms::Label^  labelUV7;
	private: System::Windows::Forms::Label^  labelUV8;
	private: System::Windows::Forms::Label^  labelUV9;
	private: System::Windows::Forms::Label^  labelUV10;
	private: System::Windows::Forms::Label^  labelUV11;
	private: System::Windows::Forms::Label^  labelUV12;
	private: System::Windows::Forms::Label^  labelUV13;

	private: System::Windows::Forms::RadioButton^  radioButtonA1_1;
	private: System::Windows::Forms::RadioButton^  radioButtonA1_2;
	private: System::Windows::Forms::RadioButton^  radioButtonA1_3;
	private: System::Windows::Forms::RadioButton^  radioButtonA1_4;
	private: System::Windows::Forms::RadioButton^  radioButtonA1_5;
	private: System::Windows::Forms::RadioButton^  radioButtonA1_6;
	private: System::Windows::Forms::RadioButton^  radioButtonA1_7;
	private: System::Windows::Forms::RadioButton^  radioButtonA1_8;
	private: System::Windows::Forms::RadioButton^  radioButtonA1_9;
	private: System::Windows::Forms::RadioButton^  radioButtonA1_10;
	private: System::Windows::Forms::RadioButton^  radioButtonA1_11;
	private: System::Windows::Forms::RadioButton^  radioButtonA1_12;

	private: System::Windows::Forms::RadioButton^  radioButtonA2_1;
	private: System::Windows::Forms::RadioButton^  radioButtonA2_2;
	private: System::Windows::Forms::RadioButton^  radioButtonA2_3;
	private: System::Windows::Forms::RadioButton^  radioButtonA2_4;
	private: System::Windows::Forms::RadioButton^  radioButtonA2_5;
	private: System::Windows::Forms::RadioButton^  radioButtonA2_6;
	private: System::Windows::Forms::RadioButton^  radioButtonA2_7;
	private: System::Windows::Forms::RadioButton^  radioButtonA2_8;
	private: System::Windows::Forms::RadioButton^  radioButtonA2_9;
	private: System::Windows::Forms::RadioButton^  radioButtonA2_10;
	private: System::Windows::Forms::RadioButton^  radioButtonA2_11;
	private: System::Windows::Forms::RadioButton^  radioButtonA2_12;

	private: System::Windows::Forms::RadioButton^  radioButtonA3_1;
	private: System::Windows::Forms::RadioButton^  radioButtonA3_2;
	private: System::Windows::Forms::RadioButton^  radioButtonA3_3;
	private: System::Windows::Forms::RadioButton^  radioButtonA3_4;
	private: System::Windows::Forms::RadioButton^  radioButtonA3_5;
	private: System::Windows::Forms::RadioButton^  radioButtonA3_6;
	private: System::Windows::Forms::RadioButton^  radioButtonA3_7;
	private: System::Windows::Forms::RadioButton^  radioButtonA3_8;
	private: System::Windows::Forms::RadioButton^  radioButtonA3_9;
	private: System::Windows::Forms::RadioButton^  radioButtonA3_10;
	private: System::Windows::Forms::RadioButton^  radioButtonA3_11;
	private: System::Windows::Forms::RadioButton^  radioButtonA3_12;

	private: System::Windows::Forms::RadioButton^  radioButtonA4_1;
	private: System::Windows::Forms::RadioButton^  radioButtonA4_2;
	private: System::Windows::Forms::RadioButton^  radioButtonA4_3;
	private: System::Windows::Forms::RadioButton^  radioButtonA4_4;
	private: System::Windows::Forms::RadioButton^  radioButtonA4_5;
	private: System::Windows::Forms::RadioButton^  radioButtonA4_6;
	private: System::Windows::Forms::RadioButton^  radioButtonA4_7;
	private: System::Windows::Forms::RadioButton^  radioButtonA4_8;
	private: System::Windows::Forms::RadioButton^  radioButtonA4_9;
	private: System::Windows::Forms::RadioButton^  radioButtonA4_10;
	private: System::Windows::Forms::RadioButton^  radioButtonA4_11;
	private: System::Windows::Forms::RadioButton^  radioButtonA4_12;

	private: System::Windows::Forms::RadioButton^  radioButtonA5_1;
	private: System::Windows::Forms::RadioButton^  radioButtonA5_2;
	private: System::Windows::Forms::RadioButton^  radioButtonA5_3;
	private: System::Windows::Forms::RadioButton^  radioButtonA5_4;
	private: System::Windows::Forms::RadioButton^  radioButtonA5_5;
	private: System::Windows::Forms::RadioButton^  radioButtonA5_6;
	private: System::Windows::Forms::RadioButton^  radioButtonA5_9;
	private: System::Windows::Forms::RadioButton^  radioButtonA5_8;
	private: System::Windows::Forms::RadioButton^  radioButtonA5_7;
	private: System::Windows::Forms::RadioButton^  radioButtonA5_10;
	private: System::Windows::Forms::RadioButton^  radioButtonA5_11;
	private: System::Windows::Forms::RadioButton^  radioButtonA5_12;

	private: System::Windows::Forms::RadioButton^  radioButtonA6_1;
	private: System::Windows::Forms::RadioButton^  radioButtonA6_2;
	private: System::Windows::Forms::RadioButton^  radioButtonA6_3;
	private: System::Windows::Forms::RadioButton^  radioButtonA6_4;
	private: System::Windows::Forms::RadioButton^  radioButtonA6_5;
	private: System::Windows::Forms::RadioButton^  radioButtonA6_6;
	private: System::Windows::Forms::RadioButton^  radioButtonA6_7;
	private: System::Windows::Forms::RadioButton^  radioButtonA6_8;
	private: System::Windows::Forms::RadioButton^  radioButtonA6_9;
	private: System::Windows::Forms::RadioButton^  radioButtonA6_10;
	private: System::Windows::Forms::RadioButton^  radioButtonA6_11;
	private: System::Windows::Forms::RadioButton^  radioButtonA6_12;

	private: System::Windows::Forms::RadioButton^  radioButtonA7_1;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_2;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_3;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_4;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_5;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_6;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_7;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_8;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_9;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_10;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_11;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_12;

	private: System::Windows::Forms::RadioButton^  radioButtonA8_1;
	private: System::Windows::Forms::RadioButton^  radioButtonA8_2;
	private: System::Windows::Forms::RadioButton^  radioButtonA8_3;
	private: System::Windows::Forms::RadioButton^  radioButtonA8_4;
	private: System::Windows::Forms::RadioButton^  radioButtonA8_5;
	private: System::Windows::Forms::RadioButton^  radioButtonA8_6;
	private: System::Windows::Forms::RadioButton^  radioButtonA8_7;
	private: System::Windows::Forms::RadioButton^  radioButtonA8_8;
	private: System::Windows::Forms::RadioButton^  radioButtonA8_9;
	private: System::Windows::Forms::RadioButton^  radioButtonA8_10;
	private: System::Windows::Forms::RadioButton^  radioButtonA8_11;
	private: System::Windows::Forms::RadioButton^  radioButtonA8_12;

	private: System::Windows::Forms::RadioButton^  radioButtonA9_1;
	private: System::Windows::Forms::RadioButton^  radioButtonA9_2;
	private: System::Windows::Forms::RadioButton^  radioButtonA9_3;
	private: System::Windows::Forms::RadioButton^  radioButtonA9_4;
	private: System::Windows::Forms::RadioButton^  radioButtonA9_5;
	private: System::Windows::Forms::RadioButton^  radioButtonA9_6;
	private: System::Windows::Forms::RadioButton^  radioButtonA9_9;
	private: System::Windows::Forms::RadioButton^  radioButtonA9_8;
	private: System::Windows::Forms::RadioButton^  radioButtonA9_7;
	private: System::Windows::Forms::RadioButton^  radioButtonA9_10;
	private: System::Windows::Forms::RadioButton^  radioButtonA9_11;
	private: System::Windows::Forms::RadioButton^  radioButtonA9_12;

	private: System::Windows::Forms::RadioButton^  radioButtonB1_1;
	private: System::Windows::Forms::RadioButton^  radioButtonB1_2;
	private: System::Windows::Forms::RadioButton^  radioButtonB2_1;
	private: System::Windows::Forms::RadioButton^  radioButtonB2_3;
	private: System::Windows::Forms::RadioButton^  radioButtonB3_2;
	private: System::Windows::Forms::RadioButton^  radioButtonB3_3;
	private: System::Windows::Forms::RadioButton^  radioButtonB4_1;
	private: System::Windows::Forms::RadioButton^  radioButtonB5_2;
	private: System::Windows::Forms::RadioButton^  radioButtonB6_3;

	private: System::Windows::Forms::RadioButton^  radioButtonPEI09;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_1s;
	private: System::Windows::Forms::RadioButton^  radioButtonA7_1m;

	private: System::Windows::Forms::GroupBox^  groupBoxHistogramModification;
	private: System::Windows::Forms::RadioButton^  radioButtonHistogramNoneMod;
	private: System::Windows::Forms::RadioButton^  radioButtonHistogramCompaction;
	private: System::Windows::Forms::RadioButton^  radioButtonHistogramPermutation;

	private: System::Windows::Forms::GroupBox^  groupBoxPrediction;
	private: System::Windows::Forms::RadioButton^  radioButtonMED;
	private: System::Windows::Forms::RadioButton^  radioButtonMEDPAETH;
	private: System::Windows::Forms::RadioButton^  radioButtonExtern;
	private: System::Windows::Forms::RadioButton^  radioButtonPaeth;
	private: System::Windows::Forms::RadioButton^  radioButtonNoPred;
	private: System::Windows::Forms::RadioButton^  radioButtonLeftNeighbour;
	private: System::Windows::Forms::RadioButton^  radioButtonLSpred;
	private: System::Windows::Forms::RadioButton^  radioButtonLSpredHo;
	private: System::Windows::Forms::RadioButton^  radioButtonLMSpred;
	private: System::Windows::Forms::TextBox^  textBoxNumOfPreds;
	private: System::Windows::Forms::TextBox^  textBoxSearchSpace;
	private: System::Windows::Forms::Label^  labelSearchSpace;
	private: System::Windows::Forms::Label^  labelNumOfPreds;
	private: System::Windows::Forms::CheckBox^  checkBoxUseCorr;
	private: System::Windows::Forms::CheckBox^  checkBoxUseColPred;
	private: System::Windows::Forms::CheckBox^  checkBoxUseColPredParal;


	private: System::Windows::Forms::GroupBox^  groupBoxInterleavingMode;
	private: System::Windows::Forms::RadioButton^  radioButtonYYuvuv;
	private: System::Windows::Forms::RadioButton^  radioButtonYuvYuvJointly;
	private: System::Windows::Forms::RadioButton^  radioButtonYUVseparate;
	private: System::Windows::Forms::RadioButton^  radioButtonYYuuvv;
	private: System::Windows::Forms::RadioButton^  radioButtonYY_uuvv;

	private: System::Windows::Forms::CheckBox^  checkBoxSkipPrecoding;
	private: System::Windows::Forms::CheckBox^  checkBoxSkipRLC;
	private: System::Windows::Forms::CheckBox^  checkBoxSkipPostBWT;

	private: System::Windows::Forms::GroupBox^  groupBoxPostBWT;
	private: System::Windows::Forms::TextBox^  textBoxIFCmaxCount;
	private: System::Windows::Forms::TextBox^  textBoxIFCresetVal;
	private: System::Windows::Forms::TextBox^  textBoxMTF;
	private: System::Windows::Forms::RadioButton^  radioButtonIFC;
	private: System::Windows::Forms::RadioButton^  radioButtonMTF;
	private: System::Windows::Forms::RadioButton^  radioButtonHXC3;

	private: System::Windows::Forms::Label^  label_IFCmaxCount;
	private: System::Windows::Forms::Label^  label_IFCreset;

	private: System::Windows::Forms::GroupBox^  groupBoxEntropyCoding;
	private: System::Windows::Forms::RadioButton^  radioButtonArithmeticCoding;
	private: System::Windows::Forms::RadioButton^  radioButtonHuffmanCoding;


	private: System::Windows::Forms::CheckBox^  checkBoxT4;
	private: System::Windows::Forms::CheckBox^  checkBoxImprovedk;

	private: System::Windows::Forms::GroupBox^  groupBoxIndexing;
	private: System::Windows::Forms::CheckBox^  checkBoxIndexedColours;
	private: System::Windows::Forms::CheckBox^  checkBoxIndexedAdjacency;
	private: System::Windows::Forms::CheckBox^  checkBoxIndexedArith;

	private: System::Windows::Forms::GroupBox^  groupBoxPaletteSorting;
	private: System::Windows::Forms::RadioButton^  radioButtonLuminanceSorting;
	private: System::Windows::Forms::RadioButton^  radioButtonGRBSorting;

	private: System::Windows::Forms::GroupBox^  groupBoxLogFile;
//private: 
public: 
	System::Windows::Forms::CheckBox^  checkBoxLogFile;
	System::Windows::Forms::TextBox^  textBoxLogFile;

	private: System::Windows::Forms::GroupBox^  groupBoxSeries;
	private: System::Windows::Forms::RadioButton^  radioButtonSeparateCoding;
	private: System::Windows::Forms::RadioButton^  radioButtonJointlyCoding;

	private: System::Windows::Forms::CheckBox^  checkBoxUseZeroRLC2;
	private: System::Windows::Forms::RadioButton^  radioButtonH26x;

	private: System::Windows::Forms::GroupBox^  groupBoxHXCparams;
	private: System::Windows::Forms::TextBox^  textBoxHXCtolerance;
	private: System::Windows::Forms::TextBox^  textBoxToleranceOffset2;
	private: System::Windows::Forms::TextBox^  textBoxToleranceOffset1;
	private: System::Windows::Forms::TextBox^  textBoxIncreaseFac;
	private: System::Windows::Forms::Label^  labelTolerance;
	private: System::Windows::Forms::Label^  labelToleranceOffset1;
	private: System::Windows::Forms::Label^  labelToleranceOffset2;
	private: System::Windows::Forms::Label^  labelIncreaseFac;
	private: System::Windows::Forms::CheckBox^  checkBoxHXCauto;

	private: 
		System::Windows::Forms::GroupBox^  groupBoxCoBaLP2;
		System::Windows::Forms::CheckBox^  checkBoxSkipPredA;
		System::Windows::Forms::CheckBox^  checkBoxSkipTM;
private: System::Windows::Forms::CheckBox^  checkBoxSkipPredB;

private: System::Windows::Forms::CheckBox^  checkBoxEnablePredR;
private: System::Windows::Forms::CheckBox^  checkBoxConstLines;
private: System::Windows::Forms::GroupBox^  groupBoxBPC;
private: System::Windows::Forms::CheckBox^  checkBoxSignificance;

	private: System::Windows::Forms::GroupBox^  groupBoxHXC2params;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  labelTolR;
	private: System::Windows::Forms::TextBox^  textBoxTol0R;
	private: System::Windows::Forms::TextBox^  textBoxTol1R;
	private: System::Windows::Forms::TextBox^  textBoxTol2R;
	private: System::Windows::Forms::TextBox^  textBoxTol3R;
	private: System::Windows::Forms::TextBox^  textBoxTol4R;
	private: System::Windows::Forms::TextBox^  textBoxTol5R;
	private: System::Windows::Forms::Label^  labelTolG;
	private: System::Windows::Forms::TextBox^  textBoxTol0G;
	private: System::Windows::Forms::TextBox^  textBoxTol1G;
	private: System::Windows::Forms::TextBox^  textBoxTol2G;
	private: System::Windows::Forms::TextBox^  textBoxTol3G;
	private: System::Windows::Forms::TextBox^  textBoxTol4G;
	private: System::Windows::Forms::TextBox^  textBoxTol5G;

	private: System::Windows::Forms::Label^  labelTolB;
	private: System::Windows::Forms::TextBox^  textBoxTol0B;
	private: System::Windows::Forms::TextBox^  textBoxTol1B;
	private: System::Windows::Forms::TextBox^  textBoxTol2B;
	private: System::Windows::Forms::TextBox^  textBoxTol3B;
	private: System::Windows::Forms::TextBox^  textBoxTol4B;
	private: System::Windows::Forms::TextBox^  textBoxTol5B;


	private: System::Windows::Forms::Label^  labelTolA;
	private: System::Windows::Forms::TextBox^  textBoxTol0A;
	private: System::Windows::Forms::TextBox^  textBoxTol1A;
	private: System::Windows::Forms::TextBox^  textBoxTol2A;
	private: System::Windows::Forms::TextBox^  textBoxTol3A;
	private: System::Windows::Forms::TextBox^  textBoxTol5A;
	private: System::Windows::Forms::TextBox^  textBoxTol4A;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::TextBox^  textBoxIncreaseFac2;
private: System::Windows::Forms::Label^  labelIncreaseFac2;
private: System::Windows::Forms::CheckBox^  checkBoxHXC2auto;
private: System::Windows::Forms::CheckBox^  checkBox_usePredFlag;
private: System::Windows::Forms::CheckBox^  checkBox_useColFlag;
private: System::Windows::Forms::RadioButton^  radioButtonHistogramPermuteTSP;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::RadioButton^  radioButtonSCFmode;
private: System::Windows::Forms::GroupBox^  groupBoxSCF;
private: System::Windows::Forms::CheckBox^  checkBoxSCF_directional;
private: System::Windows::Forms::CheckBox^  checkBoxSCFVertHoriPrediction;
private: System::Windows::Forms::CheckBox^  checkBoxSCF_stage2tuning;
private: System::Windows::Forms::CheckBox^  checkBoxSCF_colourMAP;
private: System::Windows::Forms::TextBox^  textBox_RCTpercentage;



private: System::Windows::Forms::TrackBar^  trackBarPerformVSspeed;
private: System::Windows::Forms::Label^  label_SCF_maxNumPatt;






public: 
					 String^ changedfile;			
	public:
		ConfigForm(void)
		{
			InitializeComponent();			
		}

		/*-------------------------------------------------------------------------
		 * presets
		 *------------------------------------------------------------------------*/
		void InsertRCT( AutoConfig *aC)
		{
				/* set space in GUI */
				switch (aC->rct_type_used)
				{
					case RGBc: radioButtonRGB->Checked = true; break;
					case A1_1:  radioButtonA1_1->Checked = true; break;
					case A1_2:  radioButtonA1_2->Checked = true; break;
					case A1_3:  radioButtonA1_3->Checked = true; break;
					case A1_4:  radioButtonA1_4->Checked = true; break;
					case A1_5:  radioButtonA1_5->Checked = true; break;
					case A1_6:  radioButtonA1_6->Checked = true; break;
					case A1_7:  radioButtonA1_7->Checked = true; break;
					case A1_8:  radioButtonA1_8->Checked = true; break;
					case A1_9:  radioButtonA1_9->Checked = true; break;
					case A1_10: radioButtonA1_10->Checked = true; break;
					case A1_11: radioButtonA1_11->Checked = true; break;
					case A1_12: radioButtonA1_12->Checked = true; break;

					case A2_1:  radioButtonA2_1->Checked = true; break;
					case A2_2:  radioButtonA2_2->Checked = true; break;
					case A2_3:  radioButtonA2_3->Checked = true; break;
					case A2_4:  radioButtonA2_4->Checked = true; break;
					case A2_5:  radioButtonA2_5->Checked = true; break;
					case A2_6:  radioButtonA2_6->Checked = true; break;
					case A2_7:  radioButtonA2_7->Checked = true; break;
					case A2_8:  radioButtonA2_8->Checked = true; break;
					case A2_9:  radioButtonA2_9->Checked = true; break;
					case A2_10: radioButtonA2_10->Checked = true; break;
					case A2_11: radioButtonA2_11->Checked = true; break;
					case A2_12: radioButtonA2_12->Checked = true; break;

					case A3_1:  radioButtonA3_1->Checked = true; break;
					case A3_2:  radioButtonA3_2->Checked = true; break;
					case A3_3:  radioButtonA3_3->Checked = true; break;
					case A3_4:  radioButtonA3_4->Checked = true; break;
					case A3_5:  radioButtonA3_5->Checked = true; break;
					case A3_6:  radioButtonA3_6->Checked = true; break;
					case A3_7:  radioButtonA3_7->Checked = true; break;
					case A3_8:  radioButtonA3_8->Checked = true; break;
					case A3_9:  radioButtonA3_9->Checked = true; break;
					case A3_10: radioButtonA3_10->Checked = true; break;
					case A3_11: radioButtonA3_11->Checked = true; break;
					case A3_12: radioButtonA3_12->Checked = true; break;

					case A4_1:  radioButtonA4_1->Checked = true; break;
					case A4_2:  radioButtonA4_2->Checked = true; break;
					case A4_3:  radioButtonA4_3->Checked = true; break;
					case A4_4:  radioButtonA4_4->Checked = true; break;
					case A4_5:  radioButtonA4_5->Checked = true; break;
					case A4_6:  radioButtonA4_6->Checked = true; break;
					case A4_7:  radioButtonA4_7->Checked = true; break;
					case A4_8:  radioButtonA4_8->Checked = true; break;
					case A4_9:  radioButtonA4_9->Checked = true; break;
					case A4_10: radioButtonA4_10->Checked = true; break;
					case A4_11: radioButtonA4_11->Checked = true; break;
					case A4_12: radioButtonA4_12->Checked = true; break;

					case A5_1:  radioButtonA5_1->Checked = true; break;
					case A5_2:  radioButtonA5_2->Checked = true; break;
					case A5_3:  radioButtonA5_3->Checked = true; break;
					case A5_4:  radioButtonA5_4->Checked = true; break;
					case A5_5:  radioButtonA5_5->Checked = true; break;
					case A5_6:  radioButtonA5_6->Checked = true; break;
					case A5_7:  radioButtonA5_7->Checked = true; break;
					case A5_8:  radioButtonA5_8->Checked = true; break;
					case A5_9:  radioButtonA5_9->Checked = true; break;
					case A5_10: radioButtonA5_10->Checked = true; break;
					case A5_11: radioButtonA5_11->Checked = true; break;
					case A5_12: radioButtonA5_12->Checked = true; break;

					case A6_1:  radioButtonA6_1->Checked = true; break;
					case A6_2:  radioButtonA6_2->Checked = true; break;
					case A6_3:  radioButtonA6_3->Checked = true; break;
					case A6_4:  radioButtonA6_4->Checked = true; break;
					case A6_5:  radioButtonA6_5->Checked = true; break;
					case A6_6:  radioButtonA6_6->Checked = true; break;
					case A6_7:  radioButtonA6_7->Checked = true; break;
					case A6_8:  radioButtonA6_8->Checked = true; break;
					case A6_9:  radioButtonA6_9->Checked = true; break;
					case A6_10: radioButtonA6_10->Checked = true; break;
					case A6_11: radioButtonA6_11->Checked = true; break;
					case A6_12: radioButtonA6_12->Checked = true; break;

					case A7_1:  radioButtonA7_1->Checked = true; break;
					case A7_2:  radioButtonA7_2->Checked = true; break;
					case A7_3:  radioButtonA7_3->Checked = true; break;
					case A7_4:  radioButtonA7_4->Checked = true; break;
					case A7_5:  radioButtonA7_5->Checked = true; break;
					case A7_6:  radioButtonA7_6->Checked = true; break;
					case A7_7:  radioButtonA7_7->Checked = true; break;
					case A7_8:  radioButtonA7_8->Checked = true; break;
					case A7_9:  radioButtonA7_9->Checked = true; break;
					case A7_10: radioButtonA7_10->Checked = true; break;
					case A7_11: radioButtonA7_11->Checked = true; break;
					case A7_12: radioButtonA7_12->Checked = true; break;

					case A8_1:  radioButtonA8_1->Checked = true; break;
					case A8_2:  radioButtonA8_2->Checked = true; break;
					case A8_3:  radioButtonA8_3->Checked = true; break;
					case A8_4:  radioButtonA8_4->Checked = true; break;
					case A8_5:  radioButtonA8_5->Checked = true; break;
					case A8_6:  radioButtonA8_6->Checked = true; break;
					case A8_7:  radioButtonA8_7->Checked = true; break;
					case A8_8:  radioButtonA8_8->Checked = true; break;
					case A8_9:  radioButtonA8_9->Checked = true; break;
					case A8_10: radioButtonA8_10->Checked = true; break;
					case A8_11: radioButtonA8_11->Checked = true; break;
					case A8_12: radioButtonA8_12->Checked = true; break;

					case A9_1:  radioButtonA9_1->Checked = true; break;
					case A9_2:  radioButtonA9_2->Checked = true; break;
					case A9_3:  radioButtonA9_3->Checked = true; break;
					case A9_4:  radioButtonA9_4->Checked = true; break;
					case A9_5:  radioButtonA9_5->Checked = true; break;
					case A9_6:  radioButtonA9_6->Checked = true; break;
					case A9_7:  radioButtonA9_7->Checked = true; break;
					case A9_8:  radioButtonA9_8->Checked = true; break;
					case A9_9:  radioButtonA9_9->Checked = true; break;
					case A9_10: radioButtonA9_10->Checked = true; break;
					case A9_11: radioButtonA9_11->Checked = true; break;
					case A9_12: radioButtonA9_12->Checked = true; break;

					case B1_1:  radioButtonB1_1->Checked = true; break;
					case B1_2:  radioButtonB1_2->Checked = true; break;
					case B2_1:  radioButtonB2_1->Checked = true; break;
					case B2_3:  radioButtonB2_3->Checked = true; break;
					case B3_2:  radioButtonB3_2->Checked = true; break;
					case B3_3:  radioButtonB3_3->Checked = true; break;
					case B4_1:  radioButtonB4_1->Checked = true; break;
					case B5_2:  radioButtonB5_2->Checked = true; break;
					case B6_3:  radioButtonB6_3->Checked = true; break;

					case PEI09:  radioButtonPEI09->Checked = true; break;
					case A7_1s:  radioButtonA7_1s->Checked = true; break;
					case A7_1m:  radioButtonA7_1m->Checked = true; break;
				}
		}

		/*-------------------------------------------------------------------------
		 * presets
		 *------------------------------------------------------------------------*/
		void InsertParams( PicData *pd, String^ filename,
				ImageFeatures *iF,	AutoConfig *aC, unsigned int numberOfTiles)
		{
			/* set pointer for returning the interactive changes	*/
			autoC = aC;
			imageF = iF;
			curpic = pd;



			trackBarPerformVSspeed->Value = aC->SCF_maxNumPatternsIdx;
			autoC->SCF_maxNumPatterns = mapIdx2maxNumPatterns( aC->SCF_maxNumPatternsIdx);

			/* must be set, because interactively chose values must not be higher	*/
			maxTileWidth = autoC->maxTileWidth;
			maxTileHeight = autoC->maxTileHeight;

			if (iF->numColours > 2048) 
			{
				radioButtonHistogramPermutation->Enabled = false;
				radioButtonHistogramPermuteTSP->Enabled = false;
				if (autoC->histMode == PERMUTATION || autoC->histMode == PERMUTE_2OPT)
				{
					autoC->histMode = COMPACTION;
				}
			}
			switch (autoC->histMode)
			{
				case NoMOD:				radioButtonHistogramNoneMod->Checked = true; break;
				case COMPACTION:	radioButtonHistogramCompaction->Checked = true; break;
				case PERMUTATION:	radioButtonHistogramPermutation->Checked = true; break;
				case PERMUTE_2OPT:	radioButtonHistogramPermuteTSP->Checked = true; break;
			}
			//radioButtonHistogramNoneMod->Checked = aC->histMode ==  ? true:false;
			//radioButtonHistogramCompaction->Checked = aC->histMode == COMPACTION ? true:false;
			//radioButtonHistogramPermutation->Checked = aC->histMode == PERMUTATION ? true:false;
			checkBoxSkipRLC->Checked = aC->skip_rlc1 ? true:false;
			checkBoxSkipPostBWT->Checked = aC->skip_postBWT ? true:false;
			checkBoxSkipPrecoding->Checked = aC->skip_precoding ? true:false;
			checkBoxUseZeroRLC2->Checked = aC->use_RLC2zero ? true:false;

			checkBoxSkipPredA->Checked = aC->CoBaLP2_sA ? true:false;
			checkBoxSkipPredB->Checked = aC->CoBaLP2_sB ? true:false;
			checkBoxEnablePredR->Checked = aC->CoBaLP2_aR ? true:false;
			//checkBoxEnablePredL->Checked = aC->CoBaLP2_aL ? true:false;
			checkBoxSkipTM->Checked = aC->CoBaLP2_sT ? true:false; 
			checkBoxConstLines->Checked = aC->CoBaLP2_constLines ? true:false; 

			checkBoxUseCorr->Checked = aC->useCorrelation_flag ? true:false; 
			checkBoxUseColPred->Checked = aC->useColPrediction_flag ? true:false; 
			checkBoxUseColPredParal->Checked = aC->useColPredictionP_flag ? true:false; 
			textBoxSearchSpace->Text = (aC->LSpred_searchSpace).ToString();
			textBoxNumOfPreds->Text = (aC->LSpred_numOfPreds).ToString();

			textBoxHXCtolerance->Text = (aC->HXCtolerance).ToString();
			textBoxToleranceOffset1->Text = (aC->HXCtoleranceOffset1).ToString();
			textBoxToleranceOffset2->Text = (aC->HXCtoleranceOffset2).ToString();
			textBoxIncreaseFac->Text = (aC->HXCincreaseFac).ToString();
			checkBoxHXCauto->Checked = aC->HXCauto ? true:false;
			checkBoxHXC2auto->Checked = aC->HXC2auto ? true:false;
			checkBoxSCF_directional->Checked = aC->SCF_directional ? true:false;
			checkBoxSCFVertHoriPrediction->Checked = aC->SCF_prediction ? true:false;
			checkBoxSCF_stage2tuning->Checked = aC->SCF_stage2tuning ? true:false;
			checkBoxSCF_colourMAP->Checked = aC->SCF_colourMAP ? true:false;

			textBoxIncreaseFac2->Text = (aC->HXC2increaseFac).ToString();
			textBoxTol0R->Text = (aC->HXC2tolerance[0][0]).ToString();
			textBoxTol1R->Text = (aC->HXC2tolerance[0][1]).ToString();
			textBoxTol2R->Text = (aC->HXC2tolerance[0][2]).ToString();
			textBoxTol3R->Text = (aC->HXC2tolerance[0][3]).ToString();
			textBoxTol4R->Text = (aC->HXC2tolerance[0][4]).ToString();
			textBoxTol5R->Text = (aC->HXC2tolerance[0][5]).ToString();

			textBoxTol0G->Text = (aC->HXC2tolerance[1][0]).ToString();
			textBoxTol1G->Text = (aC->HXC2tolerance[1][1]).ToString();
			textBoxTol2G->Text = (aC->HXC2tolerance[1][2]).ToString();
			textBoxTol3G->Text = (aC->HXC2tolerance[1][3]).ToString();
			textBoxTol4G->Text = (aC->HXC2tolerance[1][4]).ToString();
			textBoxTol5G->Text = (aC->HXC2tolerance[1][5]).ToString();

			textBoxTol0B->Text = (aC->HXC2tolerance[2][0]).ToString();
			textBoxTol1B->Text = (aC->HXC2tolerance[2][1]).ToString();
			textBoxTol2B->Text = (aC->HXC2tolerance[2][2]).ToString();
			textBoxTol3B->Text = (aC->HXC2tolerance[2][3]).ToString();
			textBoxTol4B->Text = (aC->HXC2tolerance[2][4]).ToString();
			textBoxTol5B->Text = (aC->HXC2tolerance[2][5]).ToString();

			textBoxTol0A->Text = (aC->HXC2tolerance[3][0]).ToString();
			textBoxTol1A->Text = (aC->HXC2tolerance[3][1]).ToString();
			textBoxTol2A->Text = (aC->HXC2tolerance[3][2]).ToString();
			textBoxTol3A->Text = (aC->HXC2tolerance[3][3]).ToString();
			textBoxTol4A->Text = (aC->HXC2tolerance[3][4]).ToString();
			textBoxTol5A->Text = (aC->HXC2tolerance[3][5]).ToString();

			textBoxTileWidth->Text = (min( aC->tileWidth, maxTileWidth)).ToString();
			textBoxTileHeight->Text = (min( aC->tileHeight, maxTileHeight)).ToString();

			textBoxIFCresetVal->Text = (autoC->IFCresetVal).ToString();
			textBoxIFCmaxCount->Text = (autoC->IFCmaxCount).ToString();
			textBoxMTF->Text = (autoC->MTF_val).ToString();

			/* Farbraumtransformation bei Graustufe deaktivieren */
			if (curpic->channel == 1u)
			{
				groupBoxColourSpace->Enabled = false;
				groupBoxInterleavingMode->Enabled = false;
				radioButtonYUVseparate->Checked = true;
				groupBoxIndexing->Enabled = false;
				checkBoxIndexedColours->Enabled = false;
				checkBoxIndexedAdjacency->Enabled = false;
				checkBoxIndexedArith->Enabled = false;
				textBox_RCTpercentage->Enabled = false;
			}
			else
			{
				/* set space in GUI */
				InsertRCT( autoC);
				groupBoxColourSpace->Enabled = true;
				textBox_RCTpercentage->Enabled = true;
			}
			textBox_RCTpercentage->Text = (aC->RCTpercentage).ToString();

			if (autoC->indexed_flag)
			{
				checkBoxIndexedColours->Checked = true; 
				checkBoxIndexedAdjacency->Checked = aC->indexed_adj ? true:false;
				checkBoxIndexedArith->Checked = aC->indexed_arith ? true:false;
				groupBoxPaletteSorting->Enabled = true;
				groupBoxColourSpace->Enabled = false;
				groupBoxInterleavingMode->Enabled = false;
				radioButtonHistogramNoneMod->Checked = true; 
				if (autoC->palette_sorting == 1)
				{
					radioButtonLuminanceSorting->Checked = true;
				}
				else
				{
					radioButtonGRBSorting->Checked = true;
				}
			}

			this->Text = "Sy:" + imageF->syn + 
				//" Ro:" + imageF->roughness + 
				//" Mo:" + imageF->monotony + 
				//" Corr:" + imageF->corrcoeff + 
				" colDev0:" + imageF->rgbStdDev[0] + 
				" colDev1:" + imageF->rgbStdDev[1] + 
				" colDev2:" + imageF->rgbStdDev[2] + 
				" Mod:" + imageF->colModalPart + 
				" N:" + imageF->noise[0] + 
				" Col:"+ imageF->numColours;
			
			if (pd->tileNum == 0)
			{
				this->Text = this->Text + "  /   Settings of entire image";
			}
			else
			{
				this->Text = this->Text + "  /   Settings of tile " + pd->tileNum;
				this->Text = this->Text + " of " + numberOfTiles;
				groupBoxSegmentation->Enabled = false; /* tile size may not be modified any more	*/
			}
			/* set radio button for prediction	*/
			switch(aC->predMode)
			{
				case NONE: radioButtonNoPred->Checked = true; break;
				case LEFTNEIGHBOUR: radioButtonLeftNeighbour->Checked = true; break;
				case PAETH: radioButtonPaeth->Checked = true; break;
				case MED: radioButtonMED->Checked = true; break;
				case LSBLEND: radioButtonLSpred->Checked = true; break;
				case LSPRED: radioButtonLSpredHo->Checked = true; break;
				case LMSPRED: radioButtonLMSpred->Checked = true; break;
				case MEDPAETH: radioButtonMEDPAETH->Checked = true; break;
				case ADAPT: radioButtonExtern->Checked = true; break;
				case H26x: radioButtonH26x->Checked = true; break;
			}

			/* set radio button for postbwt	*/
			switch(autoC->postbwt)
			{
				case MTF: radioButtonMTF->Checked = true; break;
				case IFC: radioButtonIFC->Checked = true; break;
				case HXC3: radioButtonHXC3->Checked = true; break;
				default: radioButtonIFC->Checked = true; break;
			}

			/* set radio button for interlacing mode	*/
			switch(autoC->interleaving_mode)
			{
			  case Y_U_V: radioButtonYUVseparate->Checked = true; break;
				case YY_uvuv: radioButtonYYuvuv->Checked = true; break;
				case YY_uuvv: radioButtonYYuuvv->Checked = true; break;
				case YuvYuv: radioButtonYuvYuvJointly->Checked = true; break;
				case YYuuvv: radioButtonYYuuvv->Checked = true; break;
				default:radioButtonYUVseparate->Checked = true; break;
			}

			/* only active, if LOCO-I is enabled	*/
			checkBoxT4->Checked = aC->T4_flag ? true:false;
			checkBoxImprovedk->Checked = aC->locoImproveFlag ? true:false;

			if (autoC->separate_coding_flag)
			{
				radioButtonSeparateCoding->Checked = true;
			}
			else
			{
				radioButtonJointlyCoding->Checked = true;
			}

			if (autoC->entropy_coding_method)
			{
				radioButtonArithmeticCoding->Checked = true;
			}
			else
			{
				radioButtonHuffmanCoding->Checked = true;
			}

			/* force call of CkeckedChange	*/
			//radioButtonLOCOmode->Checked = true;
			//radioButtonLOCOmode->Checked = false;

			switch(autoC->coding_method)
			{
				case TSIP: /* TSIP */
					radioButtonTSIPmode->Checked = true;
					break;
				case LOCO: /* LOCO	*/
					radioButtonLOCOmode->Checked = true;
					checkBoxT4->Enabled = true; 
					checkBoxImprovedk->Enabled = true; 
					break;
				case BPC: /* bitplane coding	*/
					radioButtonBitPlaneMode->Checked = true;
					break;
				case HXC: /* HXC	*/
					radioButtonHXCmode->Checked = true;
					break;
				case HXC2: /* HXC2	*/
					radioButtonHXC2mode->Checked = true;
					break;
				case SCF: /* SCF	*/
					radioButtonSCFmode->Checked = true;
					break;
				case CoBaLP2: /* CoBaLP2	*/
					radioButtonCoBaLP2mode->Checked = true;
					break;
			}
			
			/* BPC	*/
			checkBoxSignificance->Checked = aC->s_flg ? true:false;
			checkBox_useColFlag->Checked = aC->use_colFlag ? true:false;
			checkBox_usePredFlag->Checked = aC->use_predFlag ? true:false;

			/* Dateiname übernehmen */
			textBoxFilename->Text = filename;
			textBoxFilename->SelectionLength = textBoxFilename->Text->Length;				

			// modif_flag = 0; /* is set as soon something was changed	*/
		//private: System::Void textBoxTileWidth_TextChanged(System::Object^  sender, System::EventArgs^  e) 
		//				 {
		//				 }
}
	

	protected:
		~ConfigForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Button^  buttonOK;
	private: System::Windows::Forms::Button^  buttonAbort;
	private: System::Windows::Forms::Button^  buttonAuto;




	private: System::Windows::Forms::Label^  label3;
private: System::ComponentModel::IContainer^  components;

	private:


#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			System::Windows::Forms::Label^  labelMTFvalue;
			System::Windows::Forms::Label^  label1_RCT;
			System::Windows::Forms::Label^  label2_RCT;
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(ConfigForm::typeid));
			this->radioButtonB5_2 = (gcnew System::Windows::Forms::RadioButton());
			this->groupBoxPostBWT = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxIFCmaxCount = (gcnew System::Windows::Forms::TextBox());
			this->label_IFCmaxCount = (gcnew System::Windows::Forms::Label());
			this->label_IFCreset = (gcnew System::Windows::Forms::Label());
			this->textBoxIFCresetVal = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->textBoxMTF = (gcnew System::Windows::Forms::TextBox());
			this->radioButtonIFC = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonMTF = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonHXC3 = (gcnew System::Windows::Forms::RadioButton());
			this->buttonOK = (gcnew System::Windows::Forms::Button());
			this->buttonAbort = (gcnew System::Windows::Forms::Button());
			this->buttonAuto = (gcnew System::Windows::Forms::Button());
			this->groupBoxSegmentation = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxTileHeight = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTileWidth = (gcnew System::Windows::Forms::TextBox());
			this->labelTileHeight = (gcnew System::Windows::Forms::Label());
			this->labelTileWidth = (gcnew System::Windows::Forms::Label());
			this->checkBoxSkipRLC = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxSkipPostBWT = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxSkipPrecoding = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxColourSpace = (gcnew System::Windows::Forms::GroupBox());
			this->textBox_RCTpercentage = (gcnew System::Windows::Forms::TextBox());
			this->radioButtonA7_1m = (gcnew System::Windows::Forms::RadioButton());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->radioButtonA7_1s = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonPEI09 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonB1_1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonB6_3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonB4_1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonB3_3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonB3_2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonB2_3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonB2_1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonB1_2 = (gcnew System::Windows::Forms::RadioButton());
			this->labelUV13 = (gcnew System::Windows::Forms::Label());
			this->labelUV1 = (gcnew System::Windows::Forms::Label());
			this->labelUV12 = (gcnew System::Windows::Forms::Label());
			this->labelUV11 = (gcnew System::Windows::Forms::Label());
			this->labelUV10 = (gcnew System::Windows::Forms::Label());
			this->labelUV9 = (gcnew System::Windows::Forms::Label());
			this->labelUV8 = (gcnew System::Windows::Forms::Label());
			this->labelUV7 = (gcnew System::Windows::Forms::Label());
			this->labelUV6 = (gcnew System::Windows::Forms::Label());
			this->labelUV5 = (gcnew System::Windows::Forms::Label());
			this->labelUV4 = (gcnew System::Windows::Forms::Label());
			this->labelUV3 = (gcnew System::Windows::Forms::Label());
			this->labelUV2 = (gcnew System::Windows::Forms::Label());
			this->radioButtonA1_10 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA1_11 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA1_12 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA1_9 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA1_8 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA1_7 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA1_4 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA1_5 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA1_6 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA1_3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA1_2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA1_1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA9_10 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA9_11 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA9_12 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA9_9 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA9_8 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA9_7 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA9_4 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA9_5 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA9_6 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA9_3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA9_2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA9_1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA8_10 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA8_11 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA8_12 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA8_9 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA8_8 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA8_7 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA8_4 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA8_5 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA8_6 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA8_3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA8_2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA8_1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA7_10 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA7_11 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA7_12 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA7_9 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA7_8 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA7_7 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA7_4 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA7_5 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA7_6 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA7_3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA7_2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA7_1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA6_10 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA6_11 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA6_12 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA6_9 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA6_8 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA6_7 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA6_4 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA6_5 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA6_6 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA6_3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA6_2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA6_1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA5_10 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA5_11 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA5_12 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA5_9 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA5_8 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA5_7 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA5_4 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA5_5 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA5_6 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA5_3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA5_2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA5_1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA4_10 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA4_11 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA4_12 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA4_9 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA4_8 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA4_7 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA4_4 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA4_5 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA4_6 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA4_3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA4_2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA4_1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA3_10 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA3_11 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA3_12 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA3_9 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA3_8 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA3_7 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA3_4 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA3_5 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA3_6 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA3_3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA3_2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA3_1 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA2_10 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA2_11 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA2_12 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA2_9 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA2_8 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA2_7 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA2_4 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA2_5 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA2_6 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA2_3 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA2_2 = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonA2_1 = (gcnew System::Windows::Forms::RadioButton());
			this->labelRCT_UV = (gcnew System::Windows::Forms::Label());
			this->labelRCT_Y = (gcnew System::Windows::Forms::Label());
			this->radioButtonRGB = (gcnew System::Windows::Forms::RadioButton());
			this->groupBoxIndexing = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxIndexedAdjacency = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxIndexedArith = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxIndexedColours = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxPrediction = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxUseColPredParal = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxUseColPred = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxUseCorr = (gcnew System::Windows::Forms::CheckBox());
			this->textBoxNumOfPreds = (gcnew System::Windows::Forms::TextBox());
			this->labelNumOfPreds = (gcnew System::Windows::Forms::Label());
			this->textBoxSearchSpace = (gcnew System::Windows::Forms::TextBox());
			this->labelSearchSpace = (gcnew System::Windows::Forms::Label());
			this->radioButtonLSpredHo = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonLSpred = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonLMSpred = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonH26x = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonLeftNeighbour = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonNoPred = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonMEDPAETH = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonMED = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonExtern = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonPaeth = (gcnew System::Windows::Forms::RadioButton());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->groupBoxFilename = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxFilename = (gcnew System::Windows::Forms::TextBox());
			this->groupBoxInterleavingMode = (gcnew System::Windows::Forms::GroupBox());
			this->radioButtonYY_uuvv = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonYYuuvv = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonYYuvuv = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonYuvYuvJointly = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonYUVseparate = (gcnew System::Windows::Forms::RadioButton());
			this->groupBoxEntropyCoding = (gcnew System::Windows::Forms::GroupBox());
			this->radioButtonArithmeticCoding = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonHuffmanCoding = (gcnew System::Windows::Forms::RadioButton());
			this->groupBoxHistogramModification = (gcnew System::Windows::Forms::GroupBox());
			this->radioButtonHistogramPermuteTSP = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonHistogramNoneMod = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonHistogramCompaction = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonHistogramPermutation = (gcnew System::Windows::Forms::RadioButton());
			this->checkBoxT4 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxImprovedk = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxLogFile = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxLogFile = (gcnew System::Windows::Forms::CheckBox());
			this->textBoxLogFile = (gcnew System::Windows::Forms::TextBox());
			this->groupBoxSeries = (gcnew System::Windows::Forms::GroupBox());
			this->radioButtonSeparateCoding = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonJointlyCoding = (gcnew System::Windows::Forms::RadioButton());
			this->checkBoxUseZeroRLC2 = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxPaletteSorting = (gcnew System::Windows::Forms::GroupBox());
			this->radioButtonLuminanceSorting = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonGRBSorting = (gcnew System::Windows::Forms::RadioButton());
			this->groupBoxCompressionMode = (gcnew System::Windows::Forms::GroupBox());
			this->radioButtonSCFmode = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonHXC2mode = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonHXCmode = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonCoBaLP2mode = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonLOCOmode = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonTSIPmode = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonBitPlaneMode = (gcnew System::Windows::Forms::RadioButton());
			this->groupBoxHXCparams = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxHXCauto = (gcnew System::Windows::Forms::CheckBox());
			this->textBoxIncreaseFac = (gcnew System::Windows::Forms::TextBox());
			this->labelIncreaseFac = (gcnew System::Windows::Forms::Label());
			this->textBoxToleranceOffset2 = (gcnew System::Windows::Forms::TextBox());
			this->textBoxToleranceOffset1 = (gcnew System::Windows::Forms::TextBox());
			this->labelToleranceOffset2 = (gcnew System::Windows::Forms::Label());
			this->labelToleranceOffset1 = (gcnew System::Windows::Forms::Label());
			this->textBoxHXCtolerance = (gcnew System::Windows::Forms::TextBox());
			this->labelTolerance = (gcnew System::Windows::Forms::Label());
			this->groupBoxCoBaLP2 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxConstLines = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxEnablePredR = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxSkipPredB = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxSkipPredA = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxSkipTM = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxBPC = (gcnew System::Windows::Forms::GroupBox());
			this->checkBox_usePredFlag = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_useColFlag = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxSignificance = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxHXC2params = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxHXC2auto = (gcnew System::Windows::Forms::CheckBox());
			this->textBoxIncreaseFac2 = (gcnew System::Windows::Forms::TextBox());
			this->labelIncreaseFac2 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->labelTolA = (gcnew System::Windows::Forms::Label());
			this->labelTolB = (gcnew System::Windows::Forms::Label());
			this->labelTolG = (gcnew System::Windows::Forms::Label());
			this->labelTolR = (gcnew System::Windows::Forms::Label());
			this->textBoxTol4A = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol5A = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol3A = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol2A = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol1A = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol0A = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol4G = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol5G = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol3G = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol2G = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol1G = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol0G = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol4B = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol5B = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol3B = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol2B = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol1B = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol0B = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol4R = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol5R = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol3R = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol2R = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol1R = (gcnew System::Windows::Forms::TextBox());
			this->textBoxTol0R = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->groupBoxSCF = (gcnew System::Windows::Forms::GroupBox());
			this->label_SCF_maxNumPatt = (gcnew System::Windows::Forms::Label());
			this->checkBoxSCF_colourMAP = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxSCF_stage2tuning = (gcnew System::Windows::Forms::CheckBox());
			this->trackBarPerformVSspeed = (gcnew System::Windows::Forms::TrackBar());
			this->checkBoxSCFVertHoriPrediction = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxSCF_directional = (gcnew System::Windows::Forms::CheckBox());
			labelMTFvalue = (gcnew System::Windows::Forms::Label());
			label1_RCT = (gcnew System::Windows::Forms::Label());
			label2_RCT = (gcnew System::Windows::Forms::Label());
			this->groupBoxPostBWT->SuspendLayout();
			this->groupBoxSegmentation->SuspendLayout();
			this->groupBoxColourSpace->SuspendLayout();
			this->groupBoxIndexing->SuspendLayout();
			this->groupBoxPrediction->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->groupBoxFilename->SuspendLayout();
			this->groupBoxInterleavingMode->SuspendLayout();
			this->groupBoxEntropyCoding->SuspendLayout();
			this->groupBoxHistogramModification->SuspendLayout();
			this->groupBoxLogFile->SuspendLayout();
			this->groupBoxSeries->SuspendLayout();
			this->groupBoxPaletteSorting->SuspendLayout();
			this->groupBoxCompressionMode->SuspendLayout();
			this->groupBoxHXCparams->SuspendLayout();
			this->groupBoxCoBaLP2->SuspendLayout();
			this->groupBoxBPC->SuspendLayout();
			this->groupBoxHXC2params->SuspendLayout();
			this->groupBoxSCF->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarPerformVSspeed))->BeginInit();
			this->SuspendLayout();
			// 
			// labelMTFvalue
			// 
			labelMTFvalue->AutoSize = true;
			labelMTFvalue->Location = System::Drawing::Point(17, 45);
			labelMTFvalue->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			labelMTFvalue->Name = L"labelMTFvalue";
			labelMTFvalue->Size = System::Drawing::Size(129, 17);
			labelMTFvalue->TabIndex = 5;
			labelMTFvalue->Text = L"Strenght (0 - 95%):";
			// 
			// label1_RCT
			// 
			label1_RCT->AutoSize = true;
			label1_RCT->Location = System::Drawing::Point(101, 36);
			label1_RCT->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			label1_RCT->Name = L"label1_RCT";
			label1_RCT->Size = System::Drawing::Size(45, 17);
			label1_RCT->TabIndex = 12;
			label1_RCT->Text = L"check";
			// 
			// label2_RCT
			// 
			label2_RCT->AutoSize = true;
			label2_RCT->Location = System::Drawing::Point(194, 38);
			label2_RCT->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			label2_RCT->Name = L"label2_RCT";
			label2_RCT->Size = System::Drawing::Size(102, 17);
			label2_RCT->TabIndex = 193;
			label2_RCT->Text = L"% of the image";
			// 
			// radioButtonB5_2
			// 
			this->radioButtonB5_2->AutoSize = true;
			this->radioButtonB5_2->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonB5_2->Location = System::Drawing::Point(262, 325);
			this->radioButtonB5_2->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonB5_2->Name = L"radioButtonB5_2";
			this->radioButtonB5_2->Size = System::Drawing::Size(16, 15);
			this->radioButtonB5_2->TabIndex = 185;
			this->radioButtonB5_2->UseVisualStyleBackColor = true;
			// 
			// groupBoxPostBWT
			// 
			this->groupBoxPostBWT->Controls->Add(this->textBoxIFCmaxCount);
			this->groupBoxPostBWT->Controls->Add(this->label_IFCmaxCount);
			this->groupBoxPostBWT->Controls->Add(this->label_IFCreset);
			this->groupBoxPostBWT->Controls->Add(this->textBoxIFCresetVal);
			this->groupBoxPostBWT->Controls->Add(this->label3);
			this->groupBoxPostBWT->Controls->Add(labelMTFvalue);
			this->groupBoxPostBWT->Controls->Add(this->textBoxMTF);
			this->groupBoxPostBWT->Controls->Add(this->radioButtonIFC);
			this->groupBoxPostBWT->Controls->Add(this->radioButtonMTF);
			this->groupBoxPostBWT->Controls->Add(this->radioButtonHXC3);
			this->groupBoxPostBWT->Enabled = false;
			this->groupBoxPostBWT->Location = System::Drawing::Point(15, 580);
			this->groupBoxPostBWT->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxPostBWT->Name = L"groupBoxPostBWT";
			this->groupBoxPostBWT->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxPostBWT->Size = System::Drawing::Size(574, 75);
			this->groupBoxPostBWT->TabIndex = 11;
			this->groupBoxPostBWT->TabStop = false;
			this->groupBoxPostBWT->Text = L"Post-BWT technique";
			// 
			// textBoxIFCmaxCount
			// 
			this->textBoxIFCmaxCount->Location = System::Drawing::Point(493, 46);
			this->textBoxIFCmaxCount->Margin = System::Windows::Forms::Padding(2);
			this->textBoxIFCmaxCount->Name = L"textBoxIFCmaxCount";
			this->textBoxIFCmaxCount->Size = System::Drawing::Size(70, 23);
			this->textBoxIFCmaxCount->TabIndex = 10;
			this->textBoxIFCmaxCount->Text = L"250";
			this->textBoxIFCmaxCount->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label_IFCmaxCount
			// 
			this->label_IFCmaxCount->AutoSize = true;
			this->label_IFCmaxCount->Location = System::Drawing::Point(400, 50);
			this->label_IFCmaxCount->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label_IFCmaxCount->Name = L"label_IFCmaxCount";
			this->label_IFCmaxCount->Size = System::Drawing::Size(76, 17);
			this->label_IFCmaxCount->TabIndex = 9;
			this->label_IFCmaxCount->Text = L"max count:";
			// 
			// label_IFCreset
			// 
			this->label_IFCreset->AutoSize = true;
			this->label_IFCreset->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->label_IFCreset->Location = System::Drawing::Point(346, 18);
			this->label_IFCreset->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label_IFCreset->Name = L"label_IFCreset";
			this->label_IFCreset->Size = System::Drawing::Size(124, 17);
			this->label_IFCreset->TabIndex = 8;
			this->label_IFCreset->Text = L"Reset (2 - 10000):";
			// 
			// textBoxIFCresetVal
			// 
			this->textBoxIFCresetVal->Location = System::Drawing::Point(493, 14);
			this->textBoxIFCresetVal->Margin = System::Windows::Forms::Padding(2);
			this->textBoxIFCresetVal->Name = L"textBoxIFCresetVal";
			this->textBoxIFCresetVal->Size = System::Drawing::Size(70, 23);
			this->textBoxIFCresetVal->TabIndex = 7;
			this->textBoxIFCresetVal->Text = L"3000";
			this->textBoxIFCresetVal->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(252, 42);
			this->label3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(20, 17);
			this->label3->TabIndex = 6;
			this->label3->Text = L"%";
			// 
			// textBoxMTF
			// 
			this->textBoxMTF->Enabled = false;
			this->textBoxMTF->Location = System::Drawing::Point(165, 39);
			this->textBoxMTF->Margin = System::Windows::Forms::Padding(2);
			this->textBoxMTF->Name = L"textBoxMTF";
			this->textBoxMTF->Size = System::Drawing::Size(82, 23);
			this->textBoxMTF->TabIndex = 4;
			this->textBoxMTF->Text = L"0";
			this->textBoxMTF->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// radioButtonIFC
			// 
			this->radioButtonIFC->AutoSize = true;
			this->radioButtonIFC->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonIFC->Location = System::Drawing::Point(291, 16);
			this->radioButtonIFC->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonIFC->Name = L"radioButtonIFC";
			this->radioButtonIFC->Size = System::Drawing::Size(48, 21);
			this->radioButtonIFC->TabIndex = 1;
			this->radioButtonIFC->Text = L"IFC";
			this->radioButtonIFC->UseVisualStyleBackColor = true;
			this->radioButtonIFC->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonIFC_CheckedChanged);
			// 
			// radioButtonMTF
			// 
			this->radioButtonMTF->AutoSize = true;
			this->radioButtonMTF->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonMTF->Location = System::Drawing::Point(7, 18);
			this->radioButtonMTF->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonMTF->Name = L"radioButtonMTF";
			this->radioButtonMTF->Size = System::Drawing::Size(56, 21);
			this->radioButtonMTF->TabIndex = 0;
			this->radioButtonMTF->Text = L"MTF";
			this->radioButtonMTF->UseVisualStyleBackColor = true;
			this->radioButtonMTF->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonMTF_CheckedChanged);
			// 
			// radioButtonHXC3
			// 
			this->radioButtonHXC3->AutoSize = true;
			this->radioButtonHXC3->Checked = true;
			this->radioButtonHXC3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonHXC3->Location = System::Drawing::Point(291, 41);
			this->radioButtonHXC3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonHXC3->Name = L"radioButtonHXC3";
			this->radioButtonHXC3->Size = System::Drawing::Size(64, 21);
			this->radioButtonHXC3->TabIndex = 11;
			this->radioButtonHXC3->TabStop = true;
			this->radioButtonHXC3->Text = L"HXC3";
			this->radioButtonHXC3->UseVisualStyleBackColor = true;
			this->radioButtonHXC3->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonHXC3_CheckedChanged);
			// 
			// buttonOK
			// 
			this->buttonOK->Location = System::Drawing::Point(782, 713);
			this->buttonOK->Margin = System::Windows::Forms::Padding(2);
			this->buttonOK->Name = L"buttonOK";
			this->buttonOK->Size = System::Drawing::Size(106, 32);
			this->buttonOK->TabIndex = 0;
			this->buttonOK->Text = L"Compress";
			this->buttonOK->UseVisualStyleBackColor = true;
			this->buttonOK->Click += gcnew System::EventHandler(this, &ConfigForm::buttonCompress_Click);
			// 
			// buttonAbort
			// 
			this->buttonAbort->Location = System::Drawing::Point(703, 714);
			this->buttonAbort->Margin = System::Windows::Forms::Padding(2);
			this->buttonAbort->Name = L"buttonAbort";
			this->buttonAbort->Size = System::Drawing::Size(71, 32);
			this->buttonAbort->TabIndex = 2;
			this->buttonAbort->Text = L"Cancel";
			this->buttonAbort->UseVisualStyleBackColor = true;
			this->buttonAbort->Click += gcnew System::EventHandler(this, &ConfigForm::buttonCancel_Click);
			// 
			// buttonAuto
			// 
			this->buttonAuto->ForeColor = System::Drawing::Color::Blue;
			this->buttonAuto->Location = System::Drawing::Point(617, 714);
			this->buttonAuto->Margin = System::Windows::Forms::Padding(2);
			this->buttonAuto->Name = L"buttonAuto";
			this->buttonAuto->Size = System::Drawing::Size(68, 32);
			this->buttonAuto->TabIndex = 1;
			this->buttonAuto->Text = L"Auto";
			this->buttonAuto->UseVisualStyleBackColor = true;
			this->buttonAuto->Click += gcnew System::EventHandler(this, &ConfigForm::buttonAuto_Click);
			// 
			// groupBoxSegmentation
			// 
			this->groupBoxSegmentation->Controls->Add(this->textBoxTileHeight);
			this->groupBoxSegmentation->Controls->Add(this->textBoxTileWidth);
			this->groupBoxSegmentation->Controls->Add(this->labelTileHeight);
			this->groupBoxSegmentation->Controls->Add(this->labelTileWidth);
			this->groupBoxSegmentation->Location = System::Drawing::Point(15, 11);
			this->groupBoxSegmentation->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxSegmentation->Name = L"groupBoxSegmentation";
			this->groupBoxSegmentation->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxSegmentation->Size = System::Drawing::Size(346, 50);
			this->groupBoxSegmentation->TabIndex = 7;
			this->groupBoxSegmentation->TabStop = false;
			this->groupBoxSegmentation->Text = L"Segmentation";
			// 
			// textBoxTileHeight
			// 
			this->textBoxTileHeight->Location = System::Drawing::Point(270, 18);
			this->textBoxTileHeight->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTileHeight->Name = L"textBoxTileHeight";
			this->textBoxTileHeight->Size = System::Drawing::Size(68, 23);
			this->textBoxTileHeight->TabIndex = 6;
			this->textBoxTileHeight->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTileWidth
			// 
			this->textBoxTileWidth->Location = System::Drawing::Point(126, 18);
			this->textBoxTileWidth->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTileWidth->Name = L"textBoxTileWidth";
			this->textBoxTileWidth->Size = System::Drawing::Size(65, 23);
			this->textBoxTileWidth->TabIndex = 1;
			this->textBoxTileWidth->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->textBoxTileWidth->TextChanged += gcnew System::EventHandler(this, &ConfigForm::textBoxTileWidth_TextChanged);
			// 
			// labelTileHeight
			// 
			this->labelTileHeight->AutoSize = true;
			this->labelTileHeight->Location = System::Drawing::Point(206, 22);
			this->labelTileHeight->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelTileHeight->Name = L"labelTileHeight";
			this->labelTileHeight->Size = System::Drawing::Size(53, 17);
			this->labelTileHeight->TabIndex = 5;
			this->labelTileHeight->Text = L"Height:";
			// 
			// labelTileWidth
			// 
			this->labelTileWidth->AutoSize = true;
			this->labelTileWidth->Location = System::Drawing::Point(34, 22);
			this->labelTileWidth->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelTileWidth->Name = L"labelTileWidth";
			this->labelTileWidth->Size = System::Drawing::Size(75, 17);
			this->labelTileWidth->TabIndex = 0;
			this->labelTileWidth->Text = L"Tile Width:";
			// 
			// checkBoxSkipRLC
			// 
			this->checkBoxSkipRLC->AutoSize = true;
			this->checkBoxSkipRLC->Enabled = false;
			this->checkBoxSkipRLC->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxSkipRLC->Location = System::Drawing::Point(381, 499);
			this->checkBoxSkipRLC->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxSkipRLC->Name = L"checkBoxSkipRLC";
			this->checkBoxSkipRLC->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxSkipRLC->Size = System::Drawing::Size(95, 21);
			this->checkBoxSkipRLC->TabIndex = 9;
			this->checkBoxSkipRLC->Text = L"skip RLC 1";
			this->checkBoxSkipRLC->UseVisualStyleBackColor = true;
			this->checkBoxSkipRLC->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxSkipRLC_CheckedChanged);
			// 
			// checkBoxSkipPostBWT
			// 
			this->checkBoxSkipPostBWT->AutoSize = true;
			this->checkBoxSkipPostBWT->Enabled = false;
			this->checkBoxSkipPostBWT->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxSkipPostBWT->Location = System::Drawing::Point(381, 551);
			this->checkBoxSkipPostBWT->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxSkipPostBWT->Name = L"checkBoxSkipPostBWT";
			this->checkBoxSkipPostBWT->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxSkipPostBWT->Size = System::Drawing::Size(185, 21);
			this->checkBoxSkipPostBWT->TabIndex = 13;
			this->checkBoxSkipPostBWT->Text = L"skip post-BWT technique";
			this->checkBoxSkipPostBWT->UseVisualStyleBackColor = true;
			this->checkBoxSkipPostBWT->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxSkipPostBWT_CheckedChanged);
			// 
			// checkBoxSkipPrecoding
			// 
			this->checkBoxSkipPrecoding->AutoSize = true;
			this->checkBoxSkipPrecoding->Enabled = false;
			this->checkBoxSkipPrecoding->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxSkipPrecoding->Location = System::Drawing::Point(380, 527);
			this->checkBoxSkipPrecoding->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxSkipPrecoding->Name = L"checkBoxSkipPrecoding";
			this->checkBoxSkipPrecoding->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxSkipPrecoding->Size = System::Drawing::Size(124, 21);
			this->checkBoxSkipPrecoding->TabIndex = 10;
			this->checkBoxSkipPrecoding->Text = L"skip pre-coding";
			this->checkBoxSkipPrecoding->UseVisualStyleBackColor = true;
			this->checkBoxSkipPrecoding->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxSkipPrecoding_CheckedChanged);
			// 
			// groupBoxColourSpace
			// 
			this->groupBoxColourSpace->AutoSize = true;
			this->groupBoxColourSpace->Controls->Add(label2_RCT);
			this->groupBoxColourSpace->Controls->Add(label1_RCT);
			this->groupBoxColourSpace->Controls->Add(this->textBox_RCTpercentage);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_1m);
			this->groupBoxColourSpace->Controls->Add(this->label11);
			this->groupBoxColourSpace->Controls->Add(this->label10);
			this->groupBoxColourSpace->Controls->Add(this->label9);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_1s);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonPEI09);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonB1_1);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonB6_3);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonB5_2);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonB4_1);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonB3_3);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonB3_2);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonB2_3);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonB2_1);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonB1_2);
			this->groupBoxColourSpace->Controls->Add(this->labelUV13);
			this->groupBoxColourSpace->Controls->Add(this->labelUV1);
			this->groupBoxColourSpace->Controls->Add(this->labelUV12);
			this->groupBoxColourSpace->Controls->Add(this->labelUV11);
			this->groupBoxColourSpace->Controls->Add(this->labelUV10);
			this->groupBoxColourSpace->Controls->Add(this->labelUV9);
			this->groupBoxColourSpace->Controls->Add(this->labelUV8);
			this->groupBoxColourSpace->Controls->Add(this->labelUV7);
			this->groupBoxColourSpace->Controls->Add(this->labelUV6);
			this->groupBoxColourSpace->Controls->Add(this->labelUV5);
			this->groupBoxColourSpace->Controls->Add(this->labelUV4);
			this->groupBoxColourSpace->Controls->Add(this->labelUV3);
			this->groupBoxColourSpace->Controls->Add(this->labelUV2);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA1_10);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA1_11);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA1_12);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA1_9);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA1_8);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA1_7);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA1_4);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA1_5);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA1_6);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA1_3);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA1_2);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA1_1);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA9_10);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA9_11);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA9_12);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA9_9);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA9_8);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA9_7);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA9_4);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA9_5);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA9_6);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA9_3);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA9_2);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA9_1);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA8_10);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA8_11);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA8_12);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA8_9);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA8_8);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA8_7);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA8_4);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA8_5);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA8_6);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA8_3);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA8_2);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA8_1);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_10);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_11);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_12);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_9);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_8);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_7);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_4);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_5);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_6);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_3);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_2);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA7_1);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA6_10);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA6_11);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA6_12);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA6_9);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA6_8);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA6_7);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA6_4);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA6_5);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA6_6);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA6_3);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA6_2);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA6_1);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA5_10);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA5_11);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA5_12);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA5_9);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA5_8);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA5_7);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA5_4);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA5_5);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA5_6);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA5_3);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA5_2);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA5_1);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA4_10);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA4_11);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA4_12);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA4_9);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA4_8);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA4_7);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA4_4);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA4_5);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA4_6);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA4_3);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA4_2);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA4_1);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA3_10);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA3_11);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA3_12);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA3_9);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA3_8);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA3_7);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA3_4);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA3_5);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA3_6);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA3_3);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA3_2);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA3_1);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA2_10);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA2_11);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA2_12);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA2_9);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA2_8);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA2_7);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA2_4);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA2_5);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA2_6);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA2_3);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA2_2);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonA2_1);
			this->groupBoxColourSpace->Controls->Add(this->labelRCT_UV);
			this->groupBoxColourSpace->Controls->Add(this->labelRCT_Y);
			this->groupBoxColourSpace->Controls->Add(this->radioButtonRGB);
			this->groupBoxColourSpace->Enabled = false;
			this->groupBoxColourSpace->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBoxColourSpace->Location = System::Drawing::Point(15, 79);
			this->groupBoxColourSpace->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxColourSpace->Name = L"groupBoxColourSpace";
			this->groupBoxColourSpace->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxColourSpace->Size = System::Drawing::Size(346, 391);
			this->groupBoxColourSpace->TabIndex = 3;
			this->groupBoxColourSpace->TabStop = false;
			this->groupBoxColourSpace->Text = L"Colour Space";
			this->groupBoxColourSpace->Enter += gcnew System::EventHandler(this, &ConfigForm::groupBoxColourSpace_Enter);
			// 
			// textBox_RCTpercentage
			// 
			this->textBox_RCTpercentage->Location = System::Drawing::Point(150, 34);
			this->textBox_RCTpercentage->Margin = System::Windows::Forms::Padding(2);
			this->textBox_RCTpercentage->MaxLength = 3;
			this->textBox_RCTpercentage->Name = L"textBox_RCTpercentage";
			this->textBox_RCTpercentage->Size = System::Drawing::Size(41, 23);
			this->textBox_RCTpercentage->TabIndex = 12;
			this->textBox_RCTpercentage->Text = L"0";
			this->textBox_RCTpercentage->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->textBox_RCTpercentage->TextChanged += gcnew System::EventHandler(this, &ConfigForm::textBox_RCTpercentage_TextChanged);
			// 
			// radioButtonA7_1m
			// 
			this->radioButtonA7_1m->AutoSize = true;
			this->radioButtonA7_1m->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA7_1m->Location = System::Drawing::Point(291, 351);
			this->radioButtonA7_1m->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_1m->Name = L"radioButtonA7_1m";
			this->radioButtonA7_1m->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_1m->TabIndex = 192;
			this->radioButtonA7_1m->UseVisualStyleBackColor = true;
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label11->Location = System::Drawing::Point(259, 349);
			this->label11->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(19, 17);
			this->label11->TabIndex = 191;
			this->label11->Text = L"m";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label10->Location = System::Drawing::Point(199, 346);
			this->label10->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(15, 17);
			this->label10->TabIndex = 190;
			this->label10->Text = L"s";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label9->Location = System::Drawing::Point(21, 342);
			this->label9->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(28, 17);
			this->label9->TabIndex = 189;
			this->label9->Text = L"Pei";
			// 
			// radioButtonA7_1s
			// 
			this->radioButtonA7_1s->AutoSize = true;
			this->radioButtonA7_1s->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA7_1s->Location = System::Drawing::Point(235, 349);
			this->radioButtonA7_1s->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_1s->Name = L"radioButtonA7_1s";
			this->radioButtonA7_1s->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_1s->TabIndex = 188;
			this->radioButtonA7_1s->UseVisualStyleBackColor = true;
			// 
			// radioButtonPEI09
			// 
			this->radioButtonPEI09->AutoSize = true;
			this->radioButtonPEI09->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonPEI09->Location = System::Drawing::Point(57, 344);
			this->radioButtonPEI09->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonPEI09->Name = L"radioButtonPEI09";
			this->radioButtonPEI09->Size = System::Drawing::Size(16, 15);
			this->radioButtonPEI09->TabIndex = 187;
			this->radioButtonPEI09->UseVisualStyleBackColor = true;
			// 
			// radioButtonB1_1
			// 
			this->radioButtonB1_1->AutoSize = true;
			this->radioButtonB1_1->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonB1_1->Location = System::Drawing::Point(57, 325);
			this->radioButtonB1_1->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonB1_1->Name = L"radioButtonB1_1";
			this->radioButtonB1_1->Size = System::Drawing::Size(16, 15);
			this->radioButtonB1_1->TabIndex = 178;
			this->radioButtonB1_1->UseVisualStyleBackColor = true;
			// 
			// radioButtonB6_3
			// 
			this->radioButtonB6_3->AutoSize = true;
			this->radioButtonB6_3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonB6_3->Location = System::Drawing::Point(291, 325);
			this->radioButtonB6_3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonB6_3->Name = L"radioButtonB6_3";
			this->radioButtonB6_3->Size = System::Drawing::Size(16, 15);
			this->radioButtonB6_3->TabIndex = 186;
			this->radioButtonB6_3->UseVisualStyleBackColor = true;
			// 
			// radioButtonB4_1
			// 
			this->radioButtonB4_1->AutoSize = true;
			this->radioButtonB4_1->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonB4_1->Location = System::Drawing::Point(235, 325);
			this->radioButtonB4_1->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonB4_1->Name = L"radioButtonB4_1";
			this->radioButtonB4_1->Size = System::Drawing::Size(16, 15);
			this->radioButtonB4_1->TabIndex = 184;
			this->radioButtonB4_1->UseVisualStyleBackColor = true;
			// 
			// radioButtonB3_3
			// 
			this->radioButtonB3_3->AutoSize = true;
			this->radioButtonB3_3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonB3_3->Location = System::Drawing::Point(205, 325);
			this->radioButtonB3_3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonB3_3->Name = L"radioButtonB3_3";
			this->radioButtonB3_3->Size = System::Drawing::Size(16, 15);
			this->radioButtonB3_3->TabIndex = 183;
			this->radioButtonB3_3->UseVisualStyleBackColor = true;
			// 
			// radioButtonB3_2
			// 
			this->radioButtonB3_2->AutoSize = true;
			this->radioButtonB3_2->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonB3_2->Location = System::Drawing::Point(178, 325);
			this->radioButtonB3_2->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonB3_2->Name = L"radioButtonB3_2";
			this->radioButtonB3_2->Size = System::Drawing::Size(16, 15);
			this->radioButtonB3_2->TabIndex = 182;
			this->radioButtonB3_2->UseVisualStyleBackColor = true;
			// 
			// radioButtonB2_3
			// 
			this->radioButtonB2_3->AutoSize = true;
			this->radioButtonB2_3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonB2_3->Location = System::Drawing::Point(147, 325);
			this->radioButtonB2_3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonB2_3->Name = L"radioButtonB2_3";
			this->radioButtonB2_3->Size = System::Drawing::Size(16, 15);
			this->radioButtonB2_3->TabIndex = 181;
			this->radioButtonB2_3->UseVisualStyleBackColor = true;
			// 
			// radioButtonB2_1
			// 
			this->radioButtonB2_1->AutoSize = true;
			this->radioButtonB2_1->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonB2_1->Location = System::Drawing::Point(114, 325);
			this->radioButtonB2_1->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonB2_1->Name = L"radioButtonB2_1";
			this->radioButtonB2_1->Size = System::Drawing::Size(16, 15);
			this->radioButtonB2_1->TabIndex = 180;
			this->radioButtonB2_1->UseVisualStyleBackColor = true;
			// 
			// radioButtonB1_2
			// 
			this->radioButtonB1_2->AutoSize = true;
			this->radioButtonB1_2->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonB1_2->Location = System::Drawing::Point(83, 325);
			this->radioButtonB1_2->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonB1_2->Name = L"radioButtonB1_2";
			this->radioButtonB1_2->Size = System::Drawing::Size(16, 15);
			this->radioButtonB1_2->TabIndex = 179;
			this->radioButtonB1_2->UseMnemonic = false;
			this->radioButtonB1_2->UseVisualStyleBackColor = true;
			// 
			// labelUV13
			// 
			this->labelUV13->AutoSize = true;
			this->labelUV13->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV13->Location = System::Drawing::Point(21, 322);
			this->labelUV13->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV13->Name = L"labelUV13";
			this->labelUV13->Size = System::Drawing::Size(24, 17);
			this->labelUV13->TabIndex = 177;
			this->labelUV13->Text = L"13";
			// 
			// labelUV1
			// 
			this->labelUV1->AutoSize = true;
			this->labelUV1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV1->Location = System::Drawing::Point(21, 92);
			this->labelUV1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV1->Name = L"labelUV1";
			this->labelUV1->Size = System::Drawing::Size(16, 17);
			this->labelUV1->TabIndex = 176;
			this->labelUV1->Text = L"1";
			// 
			// labelUV12
			// 
			this->labelUV12->AutoSize = true;
			this->labelUV12->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV12->Location = System::Drawing::Point(21, 302);
			this->labelUV12->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV12->Name = L"labelUV12";
			this->labelUV12->Size = System::Drawing::Size(24, 17);
			this->labelUV12->TabIndex = 175;
			this->labelUV12->Text = L"12";
			// 
			// labelUV11
			// 
			this->labelUV11->AutoSize = true;
			this->labelUV11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV11->Location = System::Drawing::Point(21, 286);
			this->labelUV11->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV11->Name = L"labelUV11";
			this->labelUV11->Size = System::Drawing::Size(24, 17);
			this->labelUV11->TabIndex = 174;
			this->labelUV11->Text = L"11";
			// 
			// labelUV10
			// 
			this->labelUV10->AutoSize = true;
			this->labelUV10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV10->Location = System::Drawing::Point(21, 269);
			this->labelUV10->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV10->Name = L"labelUV10";
			this->labelUV10->Size = System::Drawing::Size(24, 17);
			this->labelUV10->TabIndex = 173;
			this->labelUV10->Text = L"10";
			// 
			// labelUV9
			// 
			this->labelUV9->AutoSize = true;
			this->labelUV9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV9->Location = System::Drawing::Point(21, 244);
			this->labelUV9->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV9->Name = L"labelUV9";
			this->labelUV9->Size = System::Drawing::Size(16, 17);
			this->labelUV9->TabIndex = 172;
			this->labelUV9->Text = L"9";
			// 
			// labelUV8
			// 
			this->labelUV8->AutoSize = true;
			this->labelUV8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV8->Location = System::Drawing::Point(21, 222);
			this->labelUV8->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV8->Name = L"labelUV8";
			this->labelUV8->Size = System::Drawing::Size(16, 17);
			this->labelUV8->TabIndex = 171;
			this->labelUV8->Text = L"8";
			// 
			// labelUV7
			// 
			this->labelUV7->AutoSize = true;
			this->labelUV7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV7->Location = System::Drawing::Point(21, 205);
			this->labelUV7->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV7->Name = L"labelUV7";
			this->labelUV7->Size = System::Drawing::Size(16, 17);
			this->labelUV7->TabIndex = 170;
			this->labelUV7->Text = L"7";
			// 
			// labelUV6
			// 
			this->labelUV6->AutoSize = true;
			this->labelUV6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV6->Location = System::Drawing::Point(21, 188);
			this->labelUV6->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV6->Name = L"labelUV6";
			this->labelUV6->Size = System::Drawing::Size(16, 17);
			this->labelUV6->TabIndex = 169;
			this->labelUV6->Text = L"6";
			// 
			// labelUV5
			// 
			this->labelUV5->AutoSize = true;
			this->labelUV5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV5->Location = System::Drawing::Point(21, 170);
			this->labelUV5->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV5->Name = L"labelUV5";
			this->labelUV5->Size = System::Drawing::Size(16, 17);
			this->labelUV5->TabIndex = 168;
			this->labelUV5->Text = L"5";
			// 
			// labelUV4
			// 
			this->labelUV4->AutoSize = true;
			this->labelUV4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV4->Location = System::Drawing::Point(21, 151);
			this->labelUV4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV4->Name = L"labelUV4";
			this->labelUV4->Size = System::Drawing::Size(16, 17);
			this->labelUV4->TabIndex = 167;
			this->labelUV4->Text = L"4";
			// 
			// labelUV3
			// 
			this->labelUV3->AutoSize = true;
			this->labelUV3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV3->Location = System::Drawing::Point(21, 134);
			this->labelUV3->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV3->Name = L"labelUV3";
			this->labelUV3->Size = System::Drawing::Size(16, 17);
			this->labelUV3->TabIndex = 166;
			this->labelUV3->Text = L"3";
			// 
			// labelUV2
			// 
			this->labelUV2->AutoSize = true;
			this->labelUV2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelUV2->Location = System::Drawing::Point(21, 114);
			this->labelUV2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelUV2->Name = L"labelUV2";
			this->labelUV2->Size = System::Drawing::Size(16, 17);
			this->labelUV2->TabIndex = 165;
			this->labelUV2->Text = L"2";
			// 
			// radioButtonA1_10
			// 
			this->radioButtonA1_10->AutoSize = true;
			this->radioButtonA1_10->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA1_10->Location = System::Drawing::Point(57, 269);
			this->radioButtonA1_10->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA1_10->Name = L"radioButtonA1_10";
			this->radioButtonA1_10->Size = System::Drawing::Size(16, 15);
			this->radioButtonA1_10->TabIndex = 76;
			this->radioButtonA1_10->UseVisualStyleBackColor = true;
			// 
			// radioButtonA1_11
			// 
			this->radioButtonA1_11->AutoSize = true;
			this->radioButtonA1_11->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA1_11->Location = System::Drawing::Point(57, 286);
			this->radioButtonA1_11->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA1_11->Name = L"radioButtonA1_11";
			this->radioButtonA1_11->Size = System::Drawing::Size(16, 15);
			this->radioButtonA1_11->TabIndex = 75;
			this->radioButtonA1_11->UseVisualStyleBackColor = true;
			// 
			// radioButtonA1_12
			// 
			this->radioButtonA1_12->AutoSize = true;
			this->radioButtonA1_12->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA1_12->Location = System::Drawing::Point(57, 305);
			this->radioButtonA1_12->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA1_12->Name = L"radioButtonA1_12";
			this->radioButtonA1_12->Size = System::Drawing::Size(16, 15);
			this->radioButtonA1_12->TabIndex = 74;
			this->radioButtonA1_12->UseVisualStyleBackColor = true;
			// 
			// radioButtonA1_9
			// 
			this->radioButtonA1_9->AutoSize = true;
			this->radioButtonA1_9->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA1_9->Location = System::Drawing::Point(57, 244);
			this->radioButtonA1_9->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA1_9->Name = L"radioButtonA1_9";
			this->radioButtonA1_9->Size = System::Drawing::Size(16, 15);
			this->radioButtonA1_9->TabIndex = 73;
			this->radioButtonA1_9->UseVisualStyleBackColor = true;
			// 
			// radioButtonA1_8
			// 
			this->radioButtonA1_8->AutoSize = true;
			this->radioButtonA1_8->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA1_8->Location = System::Drawing::Point(57, 222);
			this->radioButtonA1_8->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA1_8->Name = L"radioButtonA1_8";
			this->radioButtonA1_8->Size = System::Drawing::Size(16, 15);
			this->radioButtonA1_8->TabIndex = 72;
			this->radioButtonA1_8->UseVisualStyleBackColor = true;
			// 
			// radioButtonA1_7
			// 
			this->radioButtonA1_7->AutoSize = true;
			this->radioButtonA1_7->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA1_7->Location = System::Drawing::Point(57, 205);
			this->radioButtonA1_7->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA1_7->Name = L"radioButtonA1_7";
			this->radioButtonA1_7->Size = System::Drawing::Size(16, 15);
			this->radioButtonA1_7->TabIndex = 71;
			this->radioButtonA1_7->UseVisualStyleBackColor = true;
			// 
			// radioButtonA1_4
			// 
			this->radioButtonA1_4->AutoSize = true;
			this->radioButtonA1_4->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA1_4->Location = System::Drawing::Point(57, 151);
			this->radioButtonA1_4->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA1_4->Name = L"radioButtonA1_4";
			this->radioButtonA1_4->Size = System::Drawing::Size(16, 15);
			this->radioButtonA1_4->TabIndex = 70;
			this->radioButtonA1_4->UseVisualStyleBackColor = true;
			// 
			// radioButtonA1_5
			// 
			this->radioButtonA1_5->AutoSize = true;
			this->radioButtonA1_5->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA1_5->Location = System::Drawing::Point(57, 170);
			this->radioButtonA1_5->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA1_5->Name = L"radioButtonA1_5";
			this->radioButtonA1_5->Size = System::Drawing::Size(16, 15);
			this->radioButtonA1_5->TabIndex = 69;
			this->radioButtonA1_5->UseVisualStyleBackColor = true;
			// 
			// radioButtonA1_6
			// 
			this->radioButtonA1_6->AutoSize = true;
			this->radioButtonA1_6->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA1_6->Location = System::Drawing::Point(57, 188);
			this->radioButtonA1_6->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA1_6->Name = L"radioButtonA1_6";
			this->radioButtonA1_6->Size = System::Drawing::Size(16, 15);
			this->radioButtonA1_6->TabIndex = 68;
			this->radioButtonA1_6->UseVisualStyleBackColor = true;
			// 
			// radioButtonA1_3
			// 
			this->radioButtonA1_3->AutoSize = true;
			this->radioButtonA1_3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA1_3->Location = System::Drawing::Point(57, 134);
			this->radioButtonA1_3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA1_3->Name = L"radioButtonA1_3";
			this->radioButtonA1_3->Size = System::Drawing::Size(16, 15);
			this->radioButtonA1_3->TabIndex = 67;
			this->radioButtonA1_3->UseVisualStyleBackColor = true;
			// 
			// radioButtonA1_2
			// 
			this->radioButtonA1_2->AutoSize = true;
			this->radioButtonA1_2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA1_2->Location = System::Drawing::Point(57, 114);
			this->radioButtonA1_2->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA1_2->Name = L"radioButtonA1_2";
			this->radioButtonA1_2->Size = System::Drawing::Size(16, 15);
			this->radioButtonA1_2->TabIndex = 66;
			this->radioButtonA1_2->UseVisualStyleBackColor = true;
			// 
			// radioButtonA1_1
			// 
			this->radioButtonA1_1->AutoSize = true;
			this->radioButtonA1_1->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA1_1->Location = System::Drawing::Point(57, 92);
			this->radioButtonA1_1->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA1_1->Name = L"radioButtonA1_1";
			this->radioButtonA1_1->Size = System::Drawing::Size(16, 15);
			this->radioButtonA1_1->TabIndex = 6;
			this->radioButtonA1_1->UseVisualStyleBackColor = true;
			this->radioButtonA1_1->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonAnyColour_CheckedChanged);
			// 
			// radioButtonA9_10
			// 
			this->radioButtonA9_10->AutoSize = true;
			this->radioButtonA9_10->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA9_10->Location = System::Drawing::Point(291, 269);
			this->radioButtonA9_10->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA9_10->Name = L"radioButtonA9_10";
			this->radioButtonA9_10->Size = System::Drawing::Size(16, 15);
			this->radioButtonA9_10->TabIndex = 164;
			this->radioButtonA9_10->UseVisualStyleBackColor = true;
			// 
			// radioButtonA9_11
			// 
			this->radioButtonA9_11->AutoSize = true;
			this->radioButtonA9_11->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA9_11->Location = System::Drawing::Point(291, 286);
			this->radioButtonA9_11->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA9_11->Name = L"radioButtonA9_11";
			this->radioButtonA9_11->Size = System::Drawing::Size(16, 15);
			this->radioButtonA9_11->TabIndex = 163;
			this->radioButtonA9_11->UseVisualStyleBackColor = true;
			// 
			// radioButtonA9_12
			// 
			this->radioButtonA9_12->AutoSize = true;
			this->radioButtonA9_12->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA9_12->Location = System::Drawing::Point(291, 305);
			this->radioButtonA9_12->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA9_12->Name = L"radioButtonA9_12";
			this->radioButtonA9_12->Size = System::Drawing::Size(16, 15);
			this->radioButtonA9_12->TabIndex = 162;
			this->radioButtonA9_12->UseVisualStyleBackColor = true;
			// 
			// radioButtonA9_9
			// 
			this->radioButtonA9_9->AutoSize = true;
			this->radioButtonA9_9->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA9_9->Location = System::Drawing::Point(291, 244);
			this->radioButtonA9_9->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA9_9->Name = L"radioButtonA9_9";
			this->radioButtonA9_9->Size = System::Drawing::Size(16, 15);
			this->radioButtonA9_9->TabIndex = 161;
			this->radioButtonA9_9->UseVisualStyleBackColor = true;
			// 
			// radioButtonA9_8
			// 
			this->radioButtonA9_8->AutoSize = true;
			this->radioButtonA9_8->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA9_8->Location = System::Drawing::Point(291, 222);
			this->radioButtonA9_8->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA9_8->Name = L"radioButtonA9_8";
			this->radioButtonA9_8->Size = System::Drawing::Size(16, 15);
			this->radioButtonA9_8->TabIndex = 160;
			this->radioButtonA9_8->UseVisualStyleBackColor = true;
			// 
			// radioButtonA9_7
			// 
			this->radioButtonA9_7->AutoSize = true;
			this->radioButtonA9_7->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA9_7->Location = System::Drawing::Point(291, 205);
			this->radioButtonA9_7->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA9_7->Name = L"radioButtonA9_7";
			this->radioButtonA9_7->Size = System::Drawing::Size(16, 15);
			this->radioButtonA9_7->TabIndex = 159;
			this->radioButtonA9_7->UseVisualStyleBackColor = true;
			// 
			// radioButtonA9_4
			// 
			this->radioButtonA9_4->AutoSize = true;
			this->radioButtonA9_4->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA9_4->Location = System::Drawing::Point(291, 151);
			this->radioButtonA9_4->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA9_4->Name = L"radioButtonA9_4";
			this->radioButtonA9_4->Size = System::Drawing::Size(16, 15);
			this->radioButtonA9_4->TabIndex = 158;
			this->radioButtonA9_4->UseVisualStyleBackColor = true;
			// 
			// radioButtonA9_5
			// 
			this->radioButtonA9_5->AutoSize = true;
			this->radioButtonA9_5->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA9_5->Location = System::Drawing::Point(291, 170);
			this->radioButtonA9_5->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA9_5->Name = L"radioButtonA9_5";
			this->radioButtonA9_5->Size = System::Drawing::Size(16, 15);
			this->radioButtonA9_5->TabIndex = 157;
			this->radioButtonA9_5->UseVisualStyleBackColor = true;
			// 
			// radioButtonA9_6
			// 
			this->radioButtonA9_6->AutoSize = true;
			this->radioButtonA9_6->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA9_6->Location = System::Drawing::Point(291, 188);
			this->radioButtonA9_6->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA9_6->Name = L"radioButtonA9_6";
			this->radioButtonA9_6->Size = System::Drawing::Size(16, 15);
			this->radioButtonA9_6->TabIndex = 156;
			this->radioButtonA9_6->UseVisualStyleBackColor = true;
			// 
			// radioButtonA9_3
			// 
			this->radioButtonA9_3->AutoSize = true;
			this->radioButtonA9_3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA9_3->Location = System::Drawing::Point(291, 134);
			this->radioButtonA9_3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA9_3->Name = L"radioButtonA9_3";
			this->radioButtonA9_3->Size = System::Drawing::Size(16, 15);
			this->radioButtonA9_3->TabIndex = 155;
			this->radioButtonA9_3->UseVisualStyleBackColor = true;
			// 
			// radioButtonA9_2
			// 
			this->radioButtonA9_2->AutoSize = true;
			this->radioButtonA9_2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA9_2->Location = System::Drawing::Point(291, 114);
			this->radioButtonA9_2->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA9_2->Name = L"radioButtonA9_2";
			this->radioButtonA9_2->Size = System::Drawing::Size(16, 15);
			this->radioButtonA9_2->TabIndex = 154;
			this->radioButtonA9_2->UseVisualStyleBackColor = true;
			// 
			// radioButtonA9_1
			// 
			this->radioButtonA9_1->AutoSize = true;
			this->radioButtonA9_1->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA9_1->Location = System::Drawing::Point(291, 92);
			this->radioButtonA9_1->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA9_1->Name = L"radioButtonA9_1";
			this->radioButtonA9_1->Size = System::Drawing::Size(16, 15);
			this->radioButtonA9_1->TabIndex = 65;
			this->radioButtonA9_1->UseVisualStyleBackColor = true;
			// 
			// radioButtonA8_10
			// 
			this->radioButtonA8_10->AutoSize = true;
			this->radioButtonA8_10->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA8_10->Location = System::Drawing::Point(262, 269);
			this->radioButtonA8_10->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA8_10->Name = L"radioButtonA8_10";
			this->radioButtonA8_10->Size = System::Drawing::Size(16, 15);
			this->radioButtonA8_10->TabIndex = 153;
			this->radioButtonA8_10->UseVisualStyleBackColor = true;
			// 
			// radioButtonA8_11
			// 
			this->radioButtonA8_11->AutoSize = true;
			this->radioButtonA8_11->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA8_11->Location = System::Drawing::Point(262, 286);
			this->radioButtonA8_11->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA8_11->Name = L"radioButtonA8_11";
			this->radioButtonA8_11->Size = System::Drawing::Size(16, 15);
			this->radioButtonA8_11->TabIndex = 152;
			this->radioButtonA8_11->UseVisualStyleBackColor = true;
			// 
			// radioButtonA8_12
			// 
			this->radioButtonA8_12->AutoSize = true;
			this->radioButtonA8_12->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA8_12->Location = System::Drawing::Point(262, 305);
			this->radioButtonA8_12->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA8_12->Name = L"radioButtonA8_12";
			this->radioButtonA8_12->Size = System::Drawing::Size(16, 15);
			this->radioButtonA8_12->TabIndex = 151;
			this->radioButtonA8_12->UseVisualStyleBackColor = true;
			// 
			// radioButtonA8_9
			// 
			this->radioButtonA8_9->AutoSize = true;
			this->radioButtonA8_9->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA8_9->Location = System::Drawing::Point(262, 244);
			this->radioButtonA8_9->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA8_9->Name = L"radioButtonA8_9";
			this->radioButtonA8_9->Size = System::Drawing::Size(16, 15);
			this->radioButtonA8_9->TabIndex = 150;
			this->radioButtonA8_9->UseVisualStyleBackColor = true;
			// 
			// radioButtonA8_8
			// 
			this->radioButtonA8_8->AutoSize = true;
			this->radioButtonA8_8->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA8_8->Location = System::Drawing::Point(262, 222);
			this->radioButtonA8_8->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA8_8->Name = L"radioButtonA8_8";
			this->radioButtonA8_8->Size = System::Drawing::Size(16, 15);
			this->radioButtonA8_8->TabIndex = 149;
			this->radioButtonA8_8->UseVisualStyleBackColor = true;
			// 
			// radioButtonA8_7
			// 
			this->radioButtonA8_7->AutoSize = true;
			this->radioButtonA8_7->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA8_7->Location = System::Drawing::Point(262, 205);
			this->radioButtonA8_7->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA8_7->Name = L"radioButtonA8_7";
			this->radioButtonA8_7->Size = System::Drawing::Size(16, 15);
			this->radioButtonA8_7->TabIndex = 148;
			this->radioButtonA8_7->UseVisualStyleBackColor = true;
			// 
			// radioButtonA8_4
			// 
			this->radioButtonA8_4->AutoSize = true;
			this->radioButtonA8_4->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA8_4->Location = System::Drawing::Point(262, 151);
			this->radioButtonA8_4->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA8_4->Name = L"radioButtonA8_4";
			this->radioButtonA8_4->Size = System::Drawing::Size(16, 15);
			this->radioButtonA8_4->TabIndex = 147;
			this->radioButtonA8_4->UseVisualStyleBackColor = true;
			// 
			// radioButtonA8_5
			// 
			this->radioButtonA8_5->AutoSize = true;
			this->radioButtonA8_5->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA8_5->Location = System::Drawing::Point(262, 170);
			this->radioButtonA8_5->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA8_5->Name = L"radioButtonA8_5";
			this->radioButtonA8_5->Size = System::Drawing::Size(16, 15);
			this->radioButtonA8_5->TabIndex = 146;
			this->radioButtonA8_5->UseVisualStyleBackColor = true;
			// 
			// radioButtonA8_6
			// 
			this->radioButtonA8_6->AutoSize = true;
			this->radioButtonA8_6->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA8_6->Location = System::Drawing::Point(262, 188);
			this->radioButtonA8_6->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA8_6->Name = L"radioButtonA8_6";
			this->radioButtonA8_6->Size = System::Drawing::Size(16, 15);
			this->radioButtonA8_6->TabIndex = 145;
			this->radioButtonA8_6->UseVisualStyleBackColor = true;
			// 
			// radioButtonA8_3
			// 
			this->radioButtonA8_3->AutoSize = true;
			this->radioButtonA8_3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA8_3->Location = System::Drawing::Point(262, 134);
			this->radioButtonA8_3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA8_3->Name = L"radioButtonA8_3";
			this->radioButtonA8_3->Size = System::Drawing::Size(16, 15);
			this->radioButtonA8_3->TabIndex = 144;
			this->radioButtonA8_3->UseVisualStyleBackColor = true;
			// 
			// radioButtonA8_2
			// 
			this->radioButtonA8_2->AutoSize = true;
			this->radioButtonA8_2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA8_2->Location = System::Drawing::Point(262, 114);
			this->radioButtonA8_2->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA8_2->Name = L"radioButtonA8_2";
			this->radioButtonA8_2->Size = System::Drawing::Size(16, 15);
			this->radioButtonA8_2->TabIndex = 143;
			this->radioButtonA8_2->UseVisualStyleBackColor = true;
			// 
			// radioButtonA8_1
			// 
			this->radioButtonA8_1->AutoSize = true;
			this->radioButtonA8_1->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA8_1->Location = System::Drawing::Point(262, 92);
			this->radioButtonA8_1->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA8_1->Name = L"radioButtonA8_1";
			this->radioButtonA8_1->Size = System::Drawing::Size(16, 15);
			this->radioButtonA8_1->TabIndex = 64;
			this->radioButtonA8_1->UseVisualStyleBackColor = true;
			// 
			// radioButtonA7_10
			// 
			this->radioButtonA7_10->AutoSize = true;
			this->radioButtonA7_10->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA7_10->Location = System::Drawing::Point(235, 269);
			this->radioButtonA7_10->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_10->Name = L"radioButtonA7_10";
			this->radioButtonA7_10->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_10->TabIndex = 142;
			this->radioButtonA7_10->UseVisualStyleBackColor = true;
			// 
			// radioButtonA7_11
			// 
			this->radioButtonA7_11->AutoSize = true;
			this->radioButtonA7_11->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA7_11->Location = System::Drawing::Point(235, 286);
			this->radioButtonA7_11->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_11->Name = L"radioButtonA7_11";
			this->radioButtonA7_11->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_11->TabIndex = 141;
			this->radioButtonA7_11->UseVisualStyleBackColor = true;
			// 
			// radioButtonA7_12
			// 
			this->radioButtonA7_12->AutoSize = true;
			this->radioButtonA7_12->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA7_12->Location = System::Drawing::Point(235, 305);
			this->radioButtonA7_12->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_12->Name = L"radioButtonA7_12";
			this->radioButtonA7_12->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_12->TabIndex = 140;
			this->radioButtonA7_12->UseVisualStyleBackColor = true;
			// 
			// radioButtonA7_9
			// 
			this->radioButtonA7_9->AutoSize = true;
			this->radioButtonA7_9->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA7_9->Location = System::Drawing::Point(235, 244);
			this->radioButtonA7_9->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_9->Name = L"radioButtonA7_9";
			this->radioButtonA7_9->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_9->TabIndex = 139;
			this->radioButtonA7_9->UseVisualStyleBackColor = true;
			// 
			// radioButtonA7_8
			// 
			this->radioButtonA7_8->AutoSize = true;
			this->radioButtonA7_8->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA7_8->Location = System::Drawing::Point(235, 222);
			this->radioButtonA7_8->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_8->Name = L"radioButtonA7_8";
			this->radioButtonA7_8->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_8->TabIndex = 138;
			this->radioButtonA7_8->UseVisualStyleBackColor = true;
			// 
			// radioButtonA7_7
			// 
			this->radioButtonA7_7->AutoSize = true;
			this->radioButtonA7_7->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA7_7->Location = System::Drawing::Point(235, 205);
			this->radioButtonA7_7->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_7->Name = L"radioButtonA7_7";
			this->radioButtonA7_7->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_7->TabIndex = 137;
			this->radioButtonA7_7->UseVisualStyleBackColor = true;
			// 
			// radioButtonA7_4
			// 
			this->radioButtonA7_4->AutoSize = true;
			this->radioButtonA7_4->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA7_4->Location = System::Drawing::Point(235, 151);
			this->radioButtonA7_4->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_4->Name = L"radioButtonA7_4";
			this->radioButtonA7_4->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_4->TabIndex = 136;
			this->radioButtonA7_4->UseVisualStyleBackColor = true;
			// 
			// radioButtonA7_5
			// 
			this->radioButtonA7_5->AutoSize = true;
			this->radioButtonA7_5->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA7_5->Location = System::Drawing::Point(235, 170);
			this->radioButtonA7_5->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_5->Name = L"radioButtonA7_5";
			this->radioButtonA7_5->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_5->TabIndex = 135;
			this->radioButtonA7_5->UseVisualStyleBackColor = true;
			// 
			// radioButtonA7_6
			// 
			this->radioButtonA7_6->AutoSize = true;
			this->radioButtonA7_6->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA7_6->Location = System::Drawing::Point(235, 188);
			this->radioButtonA7_6->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_6->Name = L"radioButtonA7_6";
			this->radioButtonA7_6->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_6->TabIndex = 134;
			this->radioButtonA7_6->UseVisualStyleBackColor = true;
			// 
			// radioButtonA7_3
			// 
			this->radioButtonA7_3->AutoSize = true;
			this->radioButtonA7_3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA7_3->Location = System::Drawing::Point(235, 134);
			this->radioButtonA7_3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_3->Name = L"radioButtonA7_3";
			this->radioButtonA7_3->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_3->TabIndex = 133;
			this->radioButtonA7_3->UseVisualStyleBackColor = true;
			// 
			// radioButtonA7_2
			// 
			this->radioButtonA7_2->AutoSize = true;
			this->radioButtonA7_2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA7_2->Location = System::Drawing::Point(235, 114);
			this->radioButtonA7_2->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_2->Name = L"radioButtonA7_2";
			this->radioButtonA7_2->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_2->TabIndex = 132;
			this->radioButtonA7_2->UseVisualStyleBackColor = true;
			// 
			// radioButtonA7_1
			// 
			this->radioButtonA7_1->AutoSize = true;
			this->radioButtonA7_1->Checked = true;
			this->radioButtonA7_1->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA7_1->Location = System::Drawing::Point(235, 92);
			this->radioButtonA7_1->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA7_1->Name = L"radioButtonA7_1";
			this->radioButtonA7_1->Size = System::Drawing::Size(16, 15);
			this->radioButtonA7_1->TabIndex = 63;
			this->radioButtonA7_1->TabStop = true;
			this->radioButtonA7_1->UseVisualStyleBackColor = true;
			// 
			// radioButtonA6_10
			// 
			this->radioButtonA6_10->AutoSize = true;
			this->radioButtonA6_10->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA6_10->Location = System::Drawing::Point(205, 269);
			this->radioButtonA6_10->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA6_10->Name = L"radioButtonA6_10";
			this->radioButtonA6_10->Size = System::Drawing::Size(16, 15);
			this->radioButtonA6_10->TabIndex = 131;
			this->radioButtonA6_10->UseVisualStyleBackColor = true;
			// 
			// radioButtonA6_11
			// 
			this->radioButtonA6_11->AutoSize = true;
			this->radioButtonA6_11->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA6_11->Location = System::Drawing::Point(205, 286);
			this->radioButtonA6_11->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA6_11->Name = L"radioButtonA6_11";
			this->radioButtonA6_11->Size = System::Drawing::Size(16, 15);
			this->radioButtonA6_11->TabIndex = 130;
			this->radioButtonA6_11->UseVisualStyleBackColor = true;
			// 
			// radioButtonA6_12
			// 
			this->radioButtonA6_12->AutoSize = true;
			this->radioButtonA6_12->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA6_12->Location = System::Drawing::Point(205, 305);
			this->radioButtonA6_12->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA6_12->Name = L"radioButtonA6_12";
			this->radioButtonA6_12->Size = System::Drawing::Size(16, 15);
			this->radioButtonA6_12->TabIndex = 129;
			this->radioButtonA6_12->UseVisualStyleBackColor = true;
			// 
			// radioButtonA6_9
			// 
			this->radioButtonA6_9->AutoSize = true;
			this->radioButtonA6_9->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA6_9->Location = System::Drawing::Point(205, 244);
			this->radioButtonA6_9->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA6_9->Name = L"radioButtonA6_9";
			this->radioButtonA6_9->Size = System::Drawing::Size(16, 15);
			this->radioButtonA6_9->TabIndex = 128;
			this->radioButtonA6_9->UseVisualStyleBackColor = true;
			// 
			// radioButtonA6_8
			// 
			this->radioButtonA6_8->AutoSize = true;
			this->radioButtonA6_8->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA6_8->Location = System::Drawing::Point(205, 222);
			this->radioButtonA6_8->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA6_8->Name = L"radioButtonA6_8";
			this->radioButtonA6_8->Size = System::Drawing::Size(16, 15);
			this->radioButtonA6_8->TabIndex = 127;
			this->radioButtonA6_8->UseVisualStyleBackColor = true;
			// 
			// radioButtonA6_7
			// 
			this->radioButtonA6_7->AutoSize = true;
			this->radioButtonA6_7->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA6_7->Location = System::Drawing::Point(205, 205);
			this->radioButtonA6_7->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA6_7->Name = L"radioButtonA6_7";
			this->radioButtonA6_7->Size = System::Drawing::Size(16, 15);
			this->radioButtonA6_7->TabIndex = 126;
			this->radioButtonA6_7->UseVisualStyleBackColor = true;
			// 
			// radioButtonA6_4
			// 
			this->radioButtonA6_4->AutoSize = true;
			this->radioButtonA6_4->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA6_4->Location = System::Drawing::Point(205, 151);
			this->radioButtonA6_4->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA6_4->Name = L"radioButtonA6_4";
			this->radioButtonA6_4->Size = System::Drawing::Size(16, 15);
			this->radioButtonA6_4->TabIndex = 125;
			this->radioButtonA6_4->UseVisualStyleBackColor = true;
			// 
			// radioButtonA6_5
			// 
			this->radioButtonA6_5->AutoSize = true;
			this->radioButtonA6_5->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA6_5->Location = System::Drawing::Point(205, 170);
			this->radioButtonA6_5->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA6_5->Name = L"radioButtonA6_5";
			this->radioButtonA6_5->Size = System::Drawing::Size(16, 15);
			this->radioButtonA6_5->TabIndex = 124;
			this->radioButtonA6_5->UseVisualStyleBackColor = true;
			// 
			// radioButtonA6_6
			// 
			this->radioButtonA6_6->AutoSize = true;
			this->radioButtonA6_6->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA6_6->Location = System::Drawing::Point(205, 188);
			this->radioButtonA6_6->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA6_6->Name = L"radioButtonA6_6";
			this->radioButtonA6_6->Size = System::Drawing::Size(16, 15);
			this->radioButtonA6_6->TabIndex = 123;
			this->radioButtonA6_6->UseVisualStyleBackColor = true;
			// 
			// radioButtonA6_3
			// 
			this->radioButtonA6_3->AutoSize = true;
			this->radioButtonA6_3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA6_3->Location = System::Drawing::Point(205, 134);
			this->radioButtonA6_3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA6_3->Name = L"radioButtonA6_3";
			this->radioButtonA6_3->Size = System::Drawing::Size(16, 15);
			this->radioButtonA6_3->TabIndex = 122;
			this->radioButtonA6_3->UseVisualStyleBackColor = true;
			// 
			// radioButtonA6_2
			// 
			this->radioButtonA6_2->AutoSize = true;
			this->radioButtonA6_2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA6_2->Location = System::Drawing::Point(205, 114);
			this->radioButtonA6_2->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA6_2->Name = L"radioButtonA6_2";
			this->radioButtonA6_2->Size = System::Drawing::Size(16, 15);
			this->radioButtonA6_2->TabIndex = 121;
			this->radioButtonA6_2->UseVisualStyleBackColor = true;
			// 
			// radioButtonA6_1
			// 
			this->radioButtonA6_1->AutoSize = true;
			this->radioButtonA6_1->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA6_1->Location = System::Drawing::Point(205, 92);
			this->radioButtonA6_1->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA6_1->Name = L"radioButtonA6_1";
			this->radioButtonA6_1->Size = System::Drawing::Size(16, 15);
			this->radioButtonA6_1->TabIndex = 62;
			this->radioButtonA6_1->UseVisualStyleBackColor = true;
			// 
			// radioButtonA5_10
			// 
			this->radioButtonA5_10->AutoSize = true;
			this->radioButtonA5_10->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA5_10->Location = System::Drawing::Point(178, 269);
			this->radioButtonA5_10->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA5_10->Name = L"radioButtonA5_10";
			this->radioButtonA5_10->Size = System::Drawing::Size(16, 15);
			this->radioButtonA5_10->TabIndex = 120;
			this->radioButtonA5_10->UseVisualStyleBackColor = true;
			// 
			// radioButtonA5_11
			// 
			this->radioButtonA5_11->AutoSize = true;
			this->radioButtonA5_11->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA5_11->Location = System::Drawing::Point(178, 286);
			this->radioButtonA5_11->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA5_11->Name = L"radioButtonA5_11";
			this->radioButtonA5_11->Size = System::Drawing::Size(16, 15);
			this->radioButtonA5_11->TabIndex = 119;
			this->radioButtonA5_11->UseVisualStyleBackColor = true;
			// 
			// radioButtonA5_12
			// 
			this->radioButtonA5_12->AutoSize = true;
			this->radioButtonA5_12->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA5_12->Location = System::Drawing::Point(178, 305);
			this->radioButtonA5_12->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA5_12->Name = L"radioButtonA5_12";
			this->radioButtonA5_12->Size = System::Drawing::Size(16, 15);
			this->radioButtonA5_12->TabIndex = 118;
			this->radioButtonA5_12->UseVisualStyleBackColor = true;
			// 
			// radioButtonA5_9
			// 
			this->radioButtonA5_9->AutoSize = true;
			this->radioButtonA5_9->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA5_9->Location = System::Drawing::Point(178, 244);
			this->radioButtonA5_9->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA5_9->Name = L"radioButtonA5_9";
			this->radioButtonA5_9->Size = System::Drawing::Size(16, 15);
			this->radioButtonA5_9->TabIndex = 117;
			this->radioButtonA5_9->UseVisualStyleBackColor = true;
			// 
			// radioButtonA5_8
			// 
			this->radioButtonA5_8->AutoSize = true;
			this->radioButtonA5_8->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA5_8->Location = System::Drawing::Point(178, 222);
			this->radioButtonA5_8->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA5_8->Name = L"radioButtonA5_8";
			this->radioButtonA5_8->Size = System::Drawing::Size(16, 15);
			this->radioButtonA5_8->TabIndex = 116;
			this->radioButtonA5_8->UseVisualStyleBackColor = true;
			// 
			// radioButtonA5_7
			// 
			this->radioButtonA5_7->AutoSize = true;
			this->radioButtonA5_7->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA5_7->Location = System::Drawing::Point(178, 205);
			this->radioButtonA5_7->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA5_7->Name = L"radioButtonA5_7";
			this->radioButtonA5_7->Size = System::Drawing::Size(16, 15);
			this->radioButtonA5_7->TabIndex = 115;
			this->radioButtonA5_7->UseVisualStyleBackColor = true;
			// 
			// radioButtonA5_4
			// 
			this->radioButtonA5_4->AutoSize = true;
			this->radioButtonA5_4->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA5_4->Location = System::Drawing::Point(178, 151);
			this->radioButtonA5_4->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA5_4->Name = L"radioButtonA5_4";
			this->radioButtonA5_4->Size = System::Drawing::Size(16, 15);
			this->radioButtonA5_4->TabIndex = 114;
			this->radioButtonA5_4->UseVisualStyleBackColor = true;
			// 
			// radioButtonA5_5
			// 
			this->radioButtonA5_5->AutoSize = true;
			this->radioButtonA5_5->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA5_5->Location = System::Drawing::Point(178, 170);
			this->radioButtonA5_5->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA5_5->Name = L"radioButtonA5_5";
			this->radioButtonA5_5->Size = System::Drawing::Size(16, 15);
			this->radioButtonA5_5->TabIndex = 113;
			this->radioButtonA5_5->UseVisualStyleBackColor = true;
			// 
			// radioButtonA5_6
			// 
			this->radioButtonA5_6->AutoSize = true;
			this->radioButtonA5_6->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA5_6->Location = System::Drawing::Point(178, 188);
			this->radioButtonA5_6->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA5_6->Name = L"radioButtonA5_6";
			this->radioButtonA5_6->Size = System::Drawing::Size(16, 15);
			this->radioButtonA5_6->TabIndex = 112;
			this->radioButtonA5_6->UseVisualStyleBackColor = true;
			// 
			// radioButtonA5_3
			// 
			this->radioButtonA5_3->AutoSize = true;
			this->radioButtonA5_3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA5_3->Location = System::Drawing::Point(178, 134);
			this->radioButtonA5_3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA5_3->Name = L"radioButtonA5_3";
			this->radioButtonA5_3->Size = System::Drawing::Size(16, 15);
			this->radioButtonA5_3->TabIndex = 111;
			this->radioButtonA5_3->UseVisualStyleBackColor = true;
			// 
			// radioButtonA5_2
			// 
			this->radioButtonA5_2->AutoSize = true;
			this->radioButtonA5_2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA5_2->Location = System::Drawing::Point(178, 114);
			this->radioButtonA5_2->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA5_2->Name = L"radioButtonA5_2";
			this->radioButtonA5_2->Size = System::Drawing::Size(16, 15);
			this->radioButtonA5_2->TabIndex = 110;
			this->radioButtonA5_2->UseVisualStyleBackColor = true;
			// 
			// radioButtonA5_1
			// 
			this->radioButtonA5_1->AutoSize = true;
			this->radioButtonA5_1->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA5_1->Location = System::Drawing::Point(178, 92);
			this->radioButtonA5_1->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA5_1->Name = L"radioButtonA5_1";
			this->radioButtonA5_1->Size = System::Drawing::Size(16, 15);
			this->radioButtonA5_1->TabIndex = 61;
			this->radioButtonA5_1->UseVisualStyleBackColor = true;
			// 
			// radioButtonA4_10
			// 
			this->radioButtonA4_10->AutoSize = true;
			this->radioButtonA4_10->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA4_10->Location = System::Drawing::Point(147, 269);
			this->radioButtonA4_10->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA4_10->Name = L"radioButtonA4_10";
			this->radioButtonA4_10->Size = System::Drawing::Size(16, 15);
			this->radioButtonA4_10->TabIndex = 109;
			this->radioButtonA4_10->UseVisualStyleBackColor = true;
			// 
			// radioButtonA4_11
			// 
			this->radioButtonA4_11->AutoSize = true;
			this->radioButtonA4_11->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA4_11->Location = System::Drawing::Point(147, 286);
			this->radioButtonA4_11->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA4_11->Name = L"radioButtonA4_11";
			this->radioButtonA4_11->Size = System::Drawing::Size(16, 15);
			this->radioButtonA4_11->TabIndex = 108;
			this->radioButtonA4_11->UseVisualStyleBackColor = true;
			// 
			// radioButtonA4_12
			// 
			this->radioButtonA4_12->AutoSize = true;
			this->radioButtonA4_12->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA4_12->Location = System::Drawing::Point(147, 305);
			this->radioButtonA4_12->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA4_12->Name = L"radioButtonA4_12";
			this->radioButtonA4_12->Size = System::Drawing::Size(16, 15);
			this->radioButtonA4_12->TabIndex = 107;
			this->radioButtonA4_12->UseVisualStyleBackColor = true;
			// 
			// radioButtonA4_9
			// 
			this->radioButtonA4_9->AutoSize = true;
			this->radioButtonA4_9->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA4_9->Location = System::Drawing::Point(147, 244);
			this->radioButtonA4_9->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA4_9->Name = L"radioButtonA4_9";
			this->radioButtonA4_9->Size = System::Drawing::Size(16, 15);
			this->radioButtonA4_9->TabIndex = 106;
			this->radioButtonA4_9->UseVisualStyleBackColor = true;
			// 
			// radioButtonA4_8
			// 
			this->radioButtonA4_8->AutoSize = true;
			this->radioButtonA4_8->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA4_8->Location = System::Drawing::Point(147, 222);
			this->radioButtonA4_8->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA4_8->Name = L"radioButtonA4_8";
			this->radioButtonA4_8->Size = System::Drawing::Size(16, 15);
			this->radioButtonA4_8->TabIndex = 105;
			this->radioButtonA4_8->UseVisualStyleBackColor = true;
			// 
			// radioButtonA4_7
			// 
			this->radioButtonA4_7->AutoSize = true;
			this->radioButtonA4_7->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA4_7->Location = System::Drawing::Point(147, 205);
			this->radioButtonA4_7->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA4_7->Name = L"radioButtonA4_7";
			this->radioButtonA4_7->Size = System::Drawing::Size(16, 15);
			this->radioButtonA4_7->TabIndex = 104;
			this->radioButtonA4_7->UseVisualStyleBackColor = true;
			// 
			// radioButtonA4_4
			// 
			this->radioButtonA4_4->AutoSize = true;
			this->radioButtonA4_4->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA4_4->Location = System::Drawing::Point(147, 151);
			this->radioButtonA4_4->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA4_4->Name = L"radioButtonA4_4";
			this->radioButtonA4_4->Size = System::Drawing::Size(16, 15);
			this->radioButtonA4_4->TabIndex = 103;
			this->radioButtonA4_4->UseVisualStyleBackColor = true;
			// 
			// radioButtonA4_5
			// 
			this->radioButtonA4_5->AutoSize = true;
			this->radioButtonA4_5->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA4_5->Location = System::Drawing::Point(147, 170);
			this->radioButtonA4_5->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA4_5->Name = L"radioButtonA4_5";
			this->radioButtonA4_5->Size = System::Drawing::Size(16, 15);
			this->radioButtonA4_5->TabIndex = 102;
			this->radioButtonA4_5->UseVisualStyleBackColor = true;
			// 
			// radioButtonA4_6
			// 
			this->radioButtonA4_6->AutoSize = true;
			this->radioButtonA4_6->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA4_6->Location = System::Drawing::Point(147, 188);
			this->radioButtonA4_6->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA4_6->Name = L"radioButtonA4_6";
			this->radioButtonA4_6->Size = System::Drawing::Size(16, 15);
			this->radioButtonA4_6->TabIndex = 101;
			this->radioButtonA4_6->UseVisualStyleBackColor = true;
			// 
			// radioButtonA4_3
			// 
			this->radioButtonA4_3->AutoSize = true;
			this->radioButtonA4_3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA4_3->Location = System::Drawing::Point(147, 134);
			this->radioButtonA4_3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA4_3->Name = L"radioButtonA4_3";
			this->radioButtonA4_3->Size = System::Drawing::Size(16, 15);
			this->radioButtonA4_3->TabIndex = 100;
			this->radioButtonA4_3->UseVisualStyleBackColor = true;
			// 
			// radioButtonA4_2
			// 
			this->radioButtonA4_2->AutoSize = true;
			this->radioButtonA4_2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA4_2->Location = System::Drawing::Point(147, 114);
			this->radioButtonA4_2->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA4_2->Name = L"radioButtonA4_2";
			this->radioButtonA4_2->Size = System::Drawing::Size(16, 15);
			this->radioButtonA4_2->TabIndex = 99;
			this->radioButtonA4_2->UseVisualStyleBackColor = true;
			// 
			// radioButtonA4_1
			// 
			this->radioButtonA4_1->AutoSize = true;
			this->radioButtonA4_1->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA4_1->Location = System::Drawing::Point(147, 92);
			this->radioButtonA4_1->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA4_1->Name = L"radioButtonA4_1";
			this->radioButtonA4_1->Size = System::Drawing::Size(16, 15);
			this->radioButtonA4_1->TabIndex = 60;
			this->radioButtonA4_1->UseVisualStyleBackColor = true;
			// 
			// radioButtonA3_10
			// 
			this->radioButtonA3_10->AutoSize = true;
			this->radioButtonA3_10->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA3_10->Location = System::Drawing::Point(114, 269);
			this->radioButtonA3_10->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA3_10->Name = L"radioButtonA3_10";
			this->radioButtonA3_10->Size = System::Drawing::Size(16, 15);
			this->radioButtonA3_10->TabIndex = 98;
			this->radioButtonA3_10->UseVisualStyleBackColor = true;
			// 
			// radioButtonA3_11
			// 
			this->radioButtonA3_11->AutoSize = true;
			this->radioButtonA3_11->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA3_11->Location = System::Drawing::Point(114, 286);
			this->radioButtonA3_11->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA3_11->Name = L"radioButtonA3_11";
			this->radioButtonA3_11->Size = System::Drawing::Size(16, 15);
			this->radioButtonA3_11->TabIndex = 97;
			this->radioButtonA3_11->UseVisualStyleBackColor = true;
			// 
			// radioButtonA3_12
			// 
			this->radioButtonA3_12->AutoSize = true;
			this->radioButtonA3_12->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA3_12->Location = System::Drawing::Point(114, 305);
			this->radioButtonA3_12->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA3_12->Name = L"radioButtonA3_12";
			this->radioButtonA3_12->Size = System::Drawing::Size(16, 15);
			this->radioButtonA3_12->TabIndex = 96;
			this->radioButtonA3_12->UseVisualStyleBackColor = true;
			// 
			// radioButtonA3_9
			// 
			this->radioButtonA3_9->AutoSize = true;
			this->radioButtonA3_9->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA3_9->Location = System::Drawing::Point(114, 244);
			this->radioButtonA3_9->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA3_9->Name = L"radioButtonA3_9";
			this->radioButtonA3_9->Size = System::Drawing::Size(16, 15);
			this->radioButtonA3_9->TabIndex = 95;
			this->radioButtonA3_9->UseVisualStyleBackColor = true;
			// 
			// radioButtonA3_8
			// 
			this->radioButtonA3_8->AutoSize = true;
			this->radioButtonA3_8->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA3_8->Location = System::Drawing::Point(114, 222);
			this->radioButtonA3_8->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA3_8->Name = L"radioButtonA3_8";
			this->radioButtonA3_8->Size = System::Drawing::Size(16, 15);
			this->radioButtonA3_8->TabIndex = 94;
			this->radioButtonA3_8->UseVisualStyleBackColor = true;
			// 
			// radioButtonA3_7
			// 
			this->radioButtonA3_7->AutoSize = true;
			this->radioButtonA3_7->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA3_7->Location = System::Drawing::Point(114, 205);
			this->radioButtonA3_7->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA3_7->Name = L"radioButtonA3_7";
			this->radioButtonA3_7->Size = System::Drawing::Size(16, 15);
			this->radioButtonA3_7->TabIndex = 93;
			this->radioButtonA3_7->UseVisualStyleBackColor = true;
			// 
			// radioButtonA3_4
			// 
			this->radioButtonA3_4->AutoSize = true;
			this->radioButtonA3_4->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA3_4->Location = System::Drawing::Point(114, 151);
			this->radioButtonA3_4->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA3_4->Name = L"radioButtonA3_4";
			this->radioButtonA3_4->Size = System::Drawing::Size(16, 15);
			this->radioButtonA3_4->TabIndex = 92;
			this->radioButtonA3_4->UseVisualStyleBackColor = true;
			// 
			// radioButtonA3_5
			// 
			this->radioButtonA3_5->AutoSize = true;
			this->radioButtonA3_5->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA3_5->Location = System::Drawing::Point(114, 170);
			this->radioButtonA3_5->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA3_5->Name = L"radioButtonA3_5";
			this->radioButtonA3_5->Size = System::Drawing::Size(16, 15);
			this->radioButtonA3_5->TabIndex = 91;
			this->radioButtonA3_5->UseVisualStyleBackColor = true;
			// 
			// radioButtonA3_6
			// 
			this->radioButtonA3_6->AutoSize = true;
			this->radioButtonA3_6->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA3_6->Location = System::Drawing::Point(114, 188);
			this->radioButtonA3_6->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA3_6->Name = L"radioButtonA3_6";
			this->radioButtonA3_6->Size = System::Drawing::Size(16, 15);
			this->radioButtonA3_6->TabIndex = 90;
			this->radioButtonA3_6->UseVisualStyleBackColor = true;
			// 
			// radioButtonA3_3
			// 
			this->radioButtonA3_3->AutoSize = true;
			this->radioButtonA3_3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA3_3->Location = System::Drawing::Point(114, 134);
			this->radioButtonA3_3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA3_3->Name = L"radioButtonA3_3";
			this->radioButtonA3_3->Size = System::Drawing::Size(16, 15);
			this->radioButtonA3_3->TabIndex = 89;
			this->radioButtonA3_3->UseVisualStyleBackColor = true;
			// 
			// radioButtonA3_2
			// 
			this->radioButtonA3_2->AutoSize = true;
			this->radioButtonA3_2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA3_2->Location = System::Drawing::Point(114, 114);
			this->radioButtonA3_2->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA3_2->Name = L"radioButtonA3_2";
			this->radioButtonA3_2->Size = System::Drawing::Size(16, 15);
			this->radioButtonA3_2->TabIndex = 88;
			this->radioButtonA3_2->UseVisualStyleBackColor = true;
			// 
			// radioButtonA3_1
			// 
			this->radioButtonA3_1->AutoSize = true;
			this->radioButtonA3_1->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA3_1->Location = System::Drawing::Point(114, 91);
			this->radioButtonA3_1->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA3_1->Name = L"radioButtonA3_1";
			this->radioButtonA3_1->Size = System::Drawing::Size(16, 15);
			this->radioButtonA3_1->TabIndex = 59;
			this->radioButtonA3_1->UseVisualStyleBackColor = true;
			// 
			// radioButtonA2_10
			// 
			this->radioButtonA2_10->AutoSize = true;
			this->radioButtonA2_10->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA2_10->Location = System::Drawing::Point(83, 269);
			this->radioButtonA2_10->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA2_10->Name = L"radioButtonA2_10";
			this->radioButtonA2_10->Size = System::Drawing::Size(16, 15);
			this->radioButtonA2_10->TabIndex = 87;
			this->radioButtonA2_10->UseVisualStyleBackColor = true;
			// 
			// radioButtonA2_11
			// 
			this->radioButtonA2_11->AutoSize = true;
			this->radioButtonA2_11->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA2_11->Location = System::Drawing::Point(83, 286);
			this->radioButtonA2_11->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA2_11->Name = L"radioButtonA2_11";
			this->radioButtonA2_11->Size = System::Drawing::Size(16, 15);
			this->radioButtonA2_11->TabIndex = 86;
			this->radioButtonA2_11->UseVisualStyleBackColor = true;
			// 
			// radioButtonA2_12
			// 
			this->radioButtonA2_12->AutoSize = true;
			this->radioButtonA2_12->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA2_12->Location = System::Drawing::Point(83, 305);
			this->radioButtonA2_12->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA2_12->Name = L"radioButtonA2_12";
			this->radioButtonA2_12->Size = System::Drawing::Size(16, 15);
			this->radioButtonA2_12->TabIndex = 85;
			this->radioButtonA2_12->UseVisualStyleBackColor = true;
			// 
			// radioButtonA2_9
			// 
			this->radioButtonA2_9->AutoSize = true;
			this->radioButtonA2_9->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA2_9->Location = System::Drawing::Point(83, 244);
			this->radioButtonA2_9->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA2_9->Name = L"radioButtonA2_9";
			this->radioButtonA2_9->Size = System::Drawing::Size(16, 15);
			this->radioButtonA2_9->TabIndex = 84;
			this->radioButtonA2_9->UseVisualStyleBackColor = true;
			// 
			// radioButtonA2_8
			// 
			this->radioButtonA2_8->AutoSize = true;
			this->radioButtonA2_8->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA2_8->Location = System::Drawing::Point(83, 222);
			this->radioButtonA2_8->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA2_8->Name = L"radioButtonA2_8";
			this->radioButtonA2_8->Size = System::Drawing::Size(16, 15);
			this->radioButtonA2_8->TabIndex = 83;
			this->radioButtonA2_8->UseVisualStyleBackColor = true;
			// 
			// radioButtonA2_7
			// 
			this->radioButtonA2_7->AutoSize = true;
			this->radioButtonA2_7->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA2_7->Location = System::Drawing::Point(83, 205);
			this->radioButtonA2_7->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA2_7->Name = L"radioButtonA2_7";
			this->radioButtonA2_7->Size = System::Drawing::Size(16, 15);
			this->radioButtonA2_7->TabIndex = 82;
			this->radioButtonA2_7->UseVisualStyleBackColor = true;
			// 
			// radioButtonA2_4
			// 
			this->radioButtonA2_4->AutoSize = true;
			this->radioButtonA2_4->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA2_4->Location = System::Drawing::Point(83, 151);
			this->radioButtonA2_4->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA2_4->Name = L"radioButtonA2_4";
			this->radioButtonA2_4->Size = System::Drawing::Size(16, 15);
			this->radioButtonA2_4->TabIndex = 81;
			this->radioButtonA2_4->UseVisualStyleBackColor = true;
			// 
			// radioButtonA2_5
			// 
			this->radioButtonA2_5->AutoSize = true;
			this->radioButtonA2_5->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA2_5->Location = System::Drawing::Point(83, 170);
			this->radioButtonA2_5->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA2_5->Name = L"radioButtonA2_5";
			this->radioButtonA2_5->Size = System::Drawing::Size(16, 15);
			this->radioButtonA2_5->TabIndex = 80;
			this->radioButtonA2_5->UseVisualStyleBackColor = true;
			// 
			// radioButtonA2_6
			// 
			this->radioButtonA2_6->AutoSize = true;
			this->radioButtonA2_6->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA2_6->Location = System::Drawing::Point(83, 188);
			this->radioButtonA2_6->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA2_6->Name = L"radioButtonA2_6";
			this->radioButtonA2_6->Size = System::Drawing::Size(16, 15);
			this->radioButtonA2_6->TabIndex = 79;
			this->radioButtonA2_6->UseVisualStyleBackColor = true;
			// 
			// radioButtonA2_3
			// 
			this->radioButtonA2_3->AutoSize = true;
			this->radioButtonA2_3->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA2_3->Location = System::Drawing::Point(83, 134);
			this->radioButtonA2_3->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA2_3->Name = L"radioButtonA2_3";
			this->radioButtonA2_3->Size = System::Drawing::Size(16, 15);
			this->radioButtonA2_3->TabIndex = 78;
			this->radioButtonA2_3->UseVisualStyleBackColor = true;
			// 
			// radioButtonA2_2
			// 
			this->radioButtonA2_2->AutoSize = true;
			this->radioButtonA2_2->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->radioButtonA2_2->Location = System::Drawing::Point(83, 114);
			this->radioButtonA2_2->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA2_2->Name = L"radioButtonA2_2";
			this->radioButtonA2_2->Size = System::Drawing::Size(16, 15);
			this->radioButtonA2_2->TabIndex = 77;
			this->radioButtonA2_2->UseVisualStyleBackColor = true;
			// 
			// radioButtonA2_1
			// 
			this->radioButtonA2_1->AutoSize = true;
			this->radioButtonA2_1->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonA2_1->Location = System::Drawing::Point(83, 92);
			this->radioButtonA2_1->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonA2_1->Name = L"radioButtonA2_1";
			this->radioButtonA2_1->Size = System::Drawing::Size(16, 15);
			this->radioButtonA2_1->TabIndex = 31;
			this->radioButtonA2_1->UseVisualStyleBackColor = true;
			// 
			// labelRCT_UV
			// 
			this->labelRCT_UV->AutoSize = true;
			this->labelRCT_UV->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->labelRCT_UV->Location = System::Drawing::Point(8, 75);
			this->labelRCT_UV->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelRCT_UV->Name = L"labelRCT_UV";
			this->labelRCT_UV->Size = System::Drawing::Size(27, 17);
			this->labelRCT_UV->TabIndex = 58;
			this->labelRCT_UV->Text = L"UV\n";
			this->labelRCT_UV->Click += gcnew System::EventHandler(this, &ConfigForm::labelRCT_UV_Click);
			// 
			// labelRCT_Y
			// 
			this->labelRCT_Y->AutoSize = true;
			this->labelRCT_Y->Location = System::Drawing::Point(39, 72);
			this->labelRCT_Y->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelRCT_Y->Name = L"labelRCT_Y";
			this->labelRCT_Y->Size = System::Drawing::Size(265, 17);
			this->labelRCT_Y->TabIndex = 7;
			this->labelRCT_Y->Text = L"Y  1     2      3      4      5    6     7     8     9";
			// 
			// radioButtonRGB
			// 
			this->radioButtonRGB->AutoSize = true;
			this->radioButtonRGB->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonRGB->Location = System::Drawing::Point(10, 34);
			this->radioButtonRGB->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonRGB->Name = L"radioButtonRGB";
			this->radioButtonRGB->Size = System::Drawing::Size(58, 21);
			this->radioButtonRGB->TabIndex = 4;
			this->radioButtonRGB->Text = L"RGB";
			this->radioButtonRGB->UseVisualStyleBackColor = true;
			this->radioButtonRGB->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonAnyColour_CheckedChanged);
			// 
			// groupBoxIndexing
			// 
			this->groupBoxIndexing->Controls->Add(this->checkBoxIndexedAdjacency);
			this->groupBoxIndexing->Controls->Add(this->checkBoxIndexedArith);
			this->groupBoxIndexing->Controls->Add(this->checkBoxIndexedColours);
			this->groupBoxIndexing->Enabled = false;
			this->groupBoxIndexing->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBoxIndexing->Location = System::Drawing::Point(376, 374);
			this->groupBoxIndexing->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxIndexing->Name = L"groupBoxIndexing";
			this->groupBoxIndexing->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxIndexing->Size = System::Drawing::Size(388, 50);
			this->groupBoxIndexing->TabIndex = 15;
			this->groupBoxIndexing->TabStop = false;
			this->groupBoxIndexing->Text = L"Indexing";
			// 
			// checkBoxIndexedAdjacency
			// 
			this->checkBoxIndexedAdjacency->AutoSize = true;
			this->checkBoxIndexedAdjacency->Enabled = false;
			this->checkBoxIndexedAdjacency->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxIndexedAdjacency->Location = System::Drawing::Point(151, 20);
			this->checkBoxIndexedAdjacency->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxIndexedAdjacency->Name = L"checkBoxIndexedAdjacency";
			this->checkBoxIndexedAdjacency->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxIndexedAdjacency->Size = System::Drawing::Size(99, 21);
			this->checkBoxIndexedAdjacency->TabIndex = 17;
			this->checkBoxIndexedAdjacency->Text = L"Indexed adj";
			this->checkBoxIndexedAdjacency->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxIndexedAdjacency->UseVisualStyleBackColor = true;
			this->checkBoxIndexedAdjacency->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxIndexedAdjacency_CheckedChanged);
			// 
			// checkBoxIndexedArith
			// 
			this->checkBoxIndexedArith->AutoSize = true;
			this->checkBoxIndexedArith->Enabled = false;
			this->checkBoxIndexedArith->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxIndexedArith->Location = System::Drawing::Point(262, 20);
			this->checkBoxIndexedArith->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxIndexedArith->Name = L"checkBoxIndexedArith";
			this->checkBoxIndexedArith->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxIndexedArith->Size = System::Drawing::Size(108, 21);
			this->checkBoxIndexedArith->TabIndex = 16;
			this->checkBoxIndexedArith->Text = L"Indexed arith";
			this->checkBoxIndexedArith->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxIndexedArith->UseVisualStyleBackColor = true;
			this->checkBoxIndexedArith->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxIndexedArith_CheckedChanged);
			this->checkBoxIndexedArith->EnabledChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxIndexedArith_EnabledChanged);
			// 
			// checkBoxIndexedColours
			// 
			this->checkBoxIndexedColours->AutoSize = true;
			this->checkBoxIndexedColours->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxIndexedColours->Location = System::Drawing::Point(4, 20);
			this->checkBoxIndexedColours->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxIndexedColours->Name = L"checkBoxIndexedColours";
			this->checkBoxIndexedColours->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxIndexedColours->Size = System::Drawing::Size(126, 21);
			this->checkBoxIndexedColours->TabIndex = 15;
			this->checkBoxIndexedColours->Text = L"Indexed colours";
			this->checkBoxIndexedColours->UseVisualStyleBackColor = true;
			this->checkBoxIndexedColours->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxIndexedColours_CheckedChanged);
			// 
			// groupBoxPrediction
			// 
			this->groupBoxPrediction->AccessibleDescription = L"selection of prediction mode";
			this->groupBoxPrediction->Controls->Add(this->checkBoxUseColPredParal);
			this->groupBoxPrediction->Controls->Add(this->checkBoxUseColPred);
			this->groupBoxPrediction->Controls->Add(this->checkBoxUseCorr);
			this->groupBoxPrediction->Controls->Add(this->textBoxNumOfPreds);
			this->groupBoxPrediction->Controls->Add(this->labelNumOfPreds);
			this->groupBoxPrediction->Controls->Add(this->textBoxSearchSpace);
			this->groupBoxPrediction->Controls->Add(this->labelSearchSpace);
			this->groupBoxPrediction->Controls->Add(this->radioButtonLSpredHo);
			this->groupBoxPrediction->Controls->Add(this->radioButtonLSpred);
			this->groupBoxPrediction->Controls->Add(this->radioButtonLMSpred);
			this->groupBoxPrediction->Controls->Add(this->radioButtonH26x);
			this->groupBoxPrediction->Controls->Add(this->radioButtonLeftNeighbour);
			this->groupBoxPrediction->Controls->Add(this->radioButtonNoPred);
			this->groupBoxPrediction->Controls->Add(this->radioButtonMEDPAETH);
			this->groupBoxPrediction->Controls->Add(this->radioButtonMED);
			this->groupBoxPrediction->Controls->Add(this->radioButtonExtern);
			this->groupBoxPrediction->Controls->Add(this->radioButtonPaeth);
			this->groupBoxPrediction->Enabled = false;
			this->groupBoxPrediction->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBoxPrediction->Location = System::Drawing::Point(376, 155);
			this->groupBoxPrediction->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxPrediction->Name = L"groupBoxPrediction";
			this->groupBoxPrediction->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxPrediction->Size = System::Drawing::Size(388, 215);
			this->groupBoxPrediction->TabIndex = 6;
			this->groupBoxPrediction->TabStop = false;
			this->groupBoxPrediction->Text = L"Prediction";
			this->groupBoxPrediction->Enter += gcnew System::EventHandler(this, &ConfigForm::groupBoxPrediction_Enter);
			// 
			// checkBoxUseColPredParal
			// 
			this->checkBoxUseColPredParal->AutoSize = true;
			this->checkBoxUseColPredParal->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxUseColPredParal->Location = System::Drawing::Point(200, 142);
			this->checkBoxUseColPredParal->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxUseColPredParal->Name = L"checkBoxUseColPredParal";
			this->checkBoxUseColPredParal->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxUseColPredParal->Size = System::Drawing::Size(106, 21);
			this->checkBoxUseColPredParal->TabIndex = 19;
			this->checkBoxUseColPredParal->Text = L"parallel pred";
			this->checkBoxUseColPredParal->UseVisualStyleBackColor = true;
			// 
			// checkBoxUseColPred
			// 
			this->checkBoxUseColPred->AutoSize = true;
			this->checkBoxUseColPred->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxUseColPred->Location = System::Drawing::Point(69, 142);
			this->checkBoxUseColPred->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxUseColPred->Name = L"checkBoxUseColPred";
			this->checkBoxUseColPred->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxUseColPred->Size = System::Drawing::Size(126, 21);
			this->checkBoxUseColPred->TabIndex = 18;
			this->checkBoxUseColPred->Text = L"use colour pred";
			this->checkBoxUseColPred->UseVisualStyleBackColor = true;
			this->checkBoxUseColPred->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxUseColPred_CheckedChanged);
			// 
			// checkBoxUseCorr
			// 
			this->checkBoxUseCorr->AutoSize = true;
			this->checkBoxUseCorr->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxUseCorr->Location = System::Drawing::Point(74, 166);
			this->checkBoxUseCorr->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxUseCorr->Name = L"checkBoxUseCorr";
			this->checkBoxUseCorr->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxUseCorr->Size = System::Drawing::Size(121, 21);
			this->checkBoxUseCorr->TabIndex = 16;
			this->checkBoxUseCorr->Text = L"use correlation";
			this->checkBoxUseCorr->UseVisualStyleBackColor = true;
			// 
			// textBoxNumOfPreds
			// 
			this->textBoxNumOfPreds->Location = System::Drawing::Point(349, 185);
			this->textBoxNumOfPreds->Margin = System::Windows::Forms::Padding(2);
			this->textBoxNumOfPreds->Name = L"textBoxNumOfPreds";
			this->textBoxNumOfPreds->ShortcutsEnabled = false;
			this->textBoxNumOfPreds->Size = System::Drawing::Size(35, 23);
			this->textBoxNumOfPreds->TabIndex = 14;
			this->textBoxNumOfPreds->Text = L"6";
			this->textBoxNumOfPreds->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// labelNumOfPreds
			// 
			this->labelNumOfPreds->AutoSize = true;
			this->labelNumOfPreds->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->labelNumOfPreds->Location = System::Drawing::Point(151, 188);
			this->labelNumOfPreds->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelNumOfPreds->Name = L"labelNumOfPreds";
			this->labelNumOfPreds->Size = System::Drawing::Size(194, 17);
			this->labelNumOfPreds->TabIndex = 13;
			this->labelNumOfPreds->Text = L"number of predictors (2 - 18):";
			// 
			// textBoxSearchSpace
			// 
			this->textBoxSearchSpace->Location = System::Drawing::Point(349, 162);
			this->textBoxSearchSpace->Margin = System::Windows::Forms::Padding(2);
			this->textBoxSearchSpace->Name = L"textBoxSearchSpace";
			this->textBoxSearchSpace->ShortcutsEnabled = false;
			this->textBoxSearchSpace->Size = System::Drawing::Size(35, 23);
			this->textBoxSearchSpace->TabIndex = 12;
			this->textBoxSearchSpace->Text = L"5";
			this->textBoxSearchSpace->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// labelSearchSpace
			// 
			this->labelSearchSpace->AutoSize = true;
			this->labelSearchSpace->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->labelSearchSpace->Location = System::Drawing::Point(197, 166);
			this->labelSearchSpace->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelSearchSpace->Name = L"labelSearchSpace";
			this->labelSearchSpace->Size = System::Drawing::Size(148, 17);
			this->labelSearchSpace->TabIndex = 12;
			this->labelSearchSpace->Text = L"search space (2 - 20):";
			// 
			// radioButtonLSpredHo
			// 
			this->radioButtonLSpredHo->AutoSize = true;
			this->radioButtonLSpredHo->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonLSpredHo->Location = System::Drawing::Point(189, 111);
			this->radioButtonLSpredHo->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonLSpredHo->Name = L"radioButtonLSpredHo";
			this->radioButtonLSpredHo->Size = System::Drawing::Size(174, 21);
			this->radioButtonLSpredHo->TabIndex = 7;
			this->radioButtonLSpredHo->Text = L"LS predictor high order";
			this->radioButtonLSpredHo->UseVisualStyleBackColor = true;
			this->radioButtonLSpredHo->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonLSpredHo_CheckedChanged);
			// 
			// radioButtonLSpred
			// 
			this->radioButtonLSpred->AutoSize = true;
			this->radioButtonLSpred->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonLSpred->Location = System::Drawing::Point(189, 90);
			this->radioButtonLSpred->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonLSpred->Name = L"radioButtonLSpred";
			this->radioButtonLSpred->Size = System::Drawing::Size(163, 21);
			this->radioButtonLSpred->TabIndex = 6;
			this->radioButtonLSpred->Text = L"LS predictor blending";
			this->radioButtonLSpred->UseVisualStyleBackColor = true;
			this->radioButtonLSpred->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonLSpred_CheckedChanged);
			// 
			// radioButtonLMSpred
			// 
			this->radioButtonLMSpred->AutoSize = true;
			this->radioButtonLMSpred->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonLMSpred->Location = System::Drawing::Point(17, 110);
			this->radioButtonLMSpred->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonLMSpred->Name = L"radioButtonLMSpred";
			this->radioButtonLMSpred->Size = System::Drawing::Size(116, 21);
			this->radioButtonLMSpred->TabIndex = 17;
			this->radioButtonLMSpred->Text = L"LMS predictor";
			this->radioButtonLMSpred->UseVisualStyleBackColor = true;
			this->radioButtonLMSpred->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonLMSpred_CheckedChanged);
			// 
			// radioButtonH26x
			// 
			this->radioButtonH26x->AutoSize = true;
			this->radioButtonH26x->Enabled = false;
			this->radioButtonH26x->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonH26x->Location = System::Drawing::Point(17, 89);
			this->radioButtonH26x->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonH26x->Name = L"radioButtonH26x";
			this->radioButtonH26x->Size = System::Drawing::Size(125, 21);
			this->radioButtonH26x->TabIndex = 0;
			this->radioButtonH26x->Text = L"H.26x Predictor";
			this->radioButtonH26x->UseVisualStyleBackColor = true;
			this->radioButtonH26x->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonH26x_CheckedChanged);
			// 
			// radioButtonLeftNeighbour
			// 
			this->radioButtonLeftNeighbour->AutoSize = true;
			this->radioButtonLeftNeighbour->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonLeftNeighbour->Location = System::Drawing::Point(17, 41);
			this->radioButtonLeftNeighbour->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonLeftNeighbour->Name = L"radioButtonLeftNeighbour";
			this->radioButtonLeftNeighbour->Size = System::Drawing::Size(120, 21);
			this->radioButtonLeftNeighbour->TabIndex = 5;
			this->radioButtonLeftNeighbour->Text = L"Left neighbour";
			this->radioButtonLeftNeighbour->UseVisualStyleBackColor = true;
			this->radioButtonLeftNeighbour->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonLeftNeighbour_CheckedChanged);
			// 
			// radioButtonNoPred
			// 
			this->radioButtonNoPred->AutoSize = true;
			this->radioButtonNoPred->Checked = true;
			this->radioButtonNoPred->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonNoPred->Location = System::Drawing::Point(17, 18);
			this->radioButtonNoPred->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonNoPred->Name = L"radioButtonNoPred";
			this->radioButtonNoPred->Size = System::Drawing::Size(62, 21);
			this->radioButtonNoPred->TabIndex = 4;
			this->radioButtonNoPred->TabStop = true;
			this->radioButtonNoPred->Text = L"None";
			this->radioButtonNoPred->UseVisualStyleBackColor = true;
			this->radioButtonNoPred->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonNoPred_CheckedChanged);
			// 
			// radioButtonMEDPAETH
			// 
			this->radioButtonMEDPAETH->AutoSize = true;
			this->radioButtonMEDPAETH->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonMEDPAETH->Location = System::Drawing::Point(189, 41);
			this->radioButtonMEDPAETH->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonMEDPAETH->Name = L"radioButtonMEDPAETH";
			this->radioButtonMEDPAETH->Size = System::Drawing::Size(159, 21);
			this->radioButtonMEDPAETH->TabIndex = 3;
			this->radioButtonMEDPAETH->Text = L"MED/Paeth predictor";
			this->radioButtonMEDPAETH->UseVisualStyleBackColor = true;
			this->radioButtonMEDPAETH->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonMEDPAETH_CheckedChanged);
			// 
			// radioButtonMED
			// 
			this->radioButtonMED->AutoSize = true;
			this->radioButtonMED->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonMED->Location = System::Drawing::Point(189, 18);
			this->radioButtonMED->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonMED->Name = L"radioButtonMED";
			this->radioButtonMED->Size = System::Drawing::Size(118, 21);
			this->radioButtonMED->TabIndex = 2;
			this->radioButtonMED->Text = L"MED predictor";
			this->radioButtonMED->UseVisualStyleBackColor = true;
			this->radioButtonMED->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonMED_CheckedChanged);
			// 
			// radioButtonExtern
			// 
			this->radioButtonExtern->AutoSize = true;
			this->radioButtonExtern->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonExtern->Location = System::Drawing::Point(189, 64);
			this->radioButtonExtern->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonExtern->Name = L"radioButtonExtern";
			this->radioButtonExtern->Size = System::Drawing::Size(171, 21);
			this->radioButtonExtern->TabIndex = 1;
			this->radioButtonExtern->Text = L"Advanced Mode (slow)";
			this->radioButtonExtern->UseVisualStyleBackColor = true;
			this->radioButtonExtern->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonExtern_CheckedChanged);
			// 
			// radioButtonPaeth
			// 
			this->radioButtonPaeth->AutoSize = true;
			this->radioButtonPaeth->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonPaeth->Location = System::Drawing::Point(17, 64);
			this->radioButtonPaeth->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonPaeth->Name = L"radioButtonPaeth";
			this->radioButtonPaeth->Size = System::Drawing::Size(125, 21);
			this->radioButtonPaeth->TabIndex = 0;
			this->radioButtonPaeth->Text = L"Paeth predictor";
			this->radioButtonPaeth->UseVisualStyleBackColor = true;
			this->radioButtonPaeth->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonPaeth_CheckedChanged);
			// 
			// pictureBox1
			// 
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->Location = System::Drawing::Point(676, 499);
			this->pictureBox1->Margin = System::Windows::Forms::Padding(2);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(68, 81);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::CenterImage;
			this->pictureBox1->TabIndex = 5;
			this->pictureBox1->TabStop = false;
			// 
			// groupBoxFilename
			// 
			this->groupBoxFilename->Controls->Add(this->textBoxFilename);
			this->groupBoxFilename->Location = System::Drawing::Point(850, 617);
			this->groupBoxFilename->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxFilename->Name = L"groupBoxFilename";
			this->groupBoxFilename->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxFilename->Size = System::Drawing::Size(344, 70);
			this->groupBoxFilename->TabIndex = 7;
			this->groupBoxFilename->TabStop = false;
			this->groupBoxFilename->Text = L"Filename modification";
			// 
			// textBoxFilename
			// 
			this->textBoxFilename->Location = System::Drawing::Point(4, 31);
			this->textBoxFilename->Margin = System::Windows::Forms::Padding(2);
			this->textBoxFilename->Name = L"textBoxFilename";
			this->textBoxFilename->Size = System::Drawing::Size(334, 23);
			this->textBoxFilename->TabIndex = 0;
			// 
			// groupBoxInterleavingMode
			// 
			this->groupBoxInterleavingMode->Controls->Add(this->radioButtonYY_uuvv);
			this->groupBoxInterleavingMode->Controls->Add(this->radioButtonYYuuvv);
			this->groupBoxInterleavingMode->Controls->Add(this->radioButtonYYuvuv);
			this->groupBoxInterleavingMode->Controls->Add(this->radioButtonYuvYuvJointly);
			this->groupBoxInterleavingMode->Controls->Add(this->radioButtonYUVseparate);
			this->groupBoxInterleavingMode->Enabled = false;
			this->groupBoxInterleavingMode->Location = System::Drawing::Point(15, 493);
			this->groupBoxInterleavingMode->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxInterleavingMode->Name = L"groupBoxInterleavingMode";
			this->groupBoxInterleavingMode->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxInterleavingMode->Size = System::Drawing::Size(346, 79);
			this->groupBoxInterleavingMode->TabIndex = 8;
			this->groupBoxInterleavingMode->TabStop = false;
			this->groupBoxInterleavingMode->Text = L"Interleaving Mode";
			this->groupBoxInterleavingMode->Enter += gcnew System::EventHandler(this, &ConfigForm::groupBoxInterleavingMode_Enter);
			// 
			// radioButtonYY_uuvv
			// 
			this->radioButtonYY_uuvv->AutoSize = true;
			this->radioButtonYY_uuvv->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonYY_uuvv->Location = System::Drawing::Point(114, 46);
			this->radioButtonYY_uuvv->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonYY_uuvv->Name = L"radioButtonYY_uuvv";
			this->radioButtonYY_uuvv->Size = System::Drawing::Size(101, 21);
			this->radioButtonYY_uuvv->TabIndex = 3;
			this->radioButtonYY_uuvv->Text = L"| YY | uuvv |";
			this->radioButtonYY_uuvv->UseVisualStyleBackColor = true;
			this->radioButtonYY_uuvv->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonYY_uuvv_CheckedChanged);
			// 
			// radioButtonYYuuvv
			// 
			this->radioButtonYYuuvv->AutoSize = true;
			this->radioButtonYYuuvv->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonYYuuvv->Location = System::Drawing::Point(228, 48);
			this->radioButtonYYuuvv->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonYYuuvv->Name = L"radioButtonYYuuvv";
			this->radioButtonYYuuvv->Size = System::Drawing::Size(90, 21);
			this->radioButtonYYuuvv->TabIndex = 4;
			this->radioButtonYYuuvv->Text = L"| YYuuvv |";
			this->radioButtonYYuuvv->UseVisualStyleBackColor = true;
			this->radioButtonYYuuvv->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonYYuuvv_CheckedChanged);
			// 
			// radioButtonYYuvuv
			// 
			this->radioButtonYYuvuv->AutoSize = true;
			this->radioButtonYYuvuv->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonYYuvuv->Location = System::Drawing::Point(114, 18);
			this->radioButtonYYuvuv->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonYYuvuv->Name = L"radioButtonYYuvuv";
			this->radioButtonYYuvuv->Size = System::Drawing::Size(101, 21);
			this->radioButtonYYuvuv->TabIndex = 1;
			this->radioButtonYYuvuv->Text = L"| YY | uvuv |";
			this->radioButtonYYuvuv->UseVisualStyleBackColor = true;
			this->radioButtonYYuvuv->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonYYuvuv_CheckedChanged);
			// 
			// radioButtonYuvYuvJointly
			// 
			this->radioButtonYuvYuvJointly->AutoSize = true;
			this->radioButtonYuvYuvJointly->Checked = true;
			this->radioButtonYuvYuvJointly->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonYuvYuvJointly->Location = System::Drawing::Point(228, 18);
			this->radioButtonYuvYuvJointly->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonYuvYuvJointly->Name = L"radioButtonYuvYuvJointly";
			this->radioButtonYuvYuvJointly->Size = System::Drawing::Size(90, 21);
			this->radioButtonYuvYuvJointly->TabIndex = 2;
			this->radioButtonYuvYuvJointly->TabStop = true;
			this->radioButtonYuvYuvJointly->Text = L"| YuvYuv |";
			this->radioButtonYuvYuvJointly->UseVisualStyleBackColor = true;
			this->radioButtonYuvYuvJointly->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonYuvYuvJointly_CheckedChanged);
			// 
			// radioButtonYUVseparate
			// 
			this->radioButtonYUVseparate->AutoSize = true;
			this->radioButtonYUVseparate->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonYUVseparate->Location = System::Drawing::Point(8, 18);
			this->radioButtonYUVseparate->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonYUVseparate->Name = L"radioButtonYUVseparate";
			this->radioButtonYUVseparate->Size = System::Drawing::Size(88, 21);
			this->radioButtonYUVseparate->TabIndex = 0;
			this->radioButtonYUVseparate->Text = L"| Y | u | v |";
			this->radioButtonYUVseparate->UseVisualStyleBackColor = true;
			this->radioButtonYUVseparate->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonYUVseparate_CheckedChanged);
			// 
			// groupBoxEntropyCoding
			// 
			this->groupBoxEntropyCoding->Controls->Add(this->radioButtonArithmeticCoding);
			this->groupBoxEntropyCoding->Controls->Add(this->radioButtonHuffmanCoding);
			this->groupBoxEntropyCoding->Enabled = false;
			this->groupBoxEntropyCoding->Location = System::Drawing::Point(11, 669);
			this->groupBoxEntropyCoding->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxEntropyCoding->Name = L"groupBoxEntropyCoding";
			this->groupBoxEntropyCoding->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxEntropyCoding->Size = System::Drawing::Size(186, 76);
			this->groupBoxEntropyCoding->TabIndex = 12;
			this->groupBoxEntropyCoding->TabStop = false;
			this->groupBoxEntropyCoding->Text = L"Entropy Coding";
			// 
			// radioButtonArithmeticCoding
			// 
			this->radioButtonArithmeticCoding->AutoSize = true;
			this->radioButtonArithmeticCoding->Checked = true;
			this->radioButtonArithmeticCoding->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonArithmeticCoding->Location = System::Drawing::Point(15, 48);
			this->radioButtonArithmeticCoding->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonArithmeticCoding->Name = L"radioButtonArithmeticCoding";
			this->radioButtonArithmeticCoding->Size = System::Drawing::Size(138, 21);
			this->radioButtonArithmeticCoding->TabIndex = 1;
			this->radioButtonArithmeticCoding->TabStop = true;
			this->radioButtonArithmeticCoding->Text = L"Arithmetic Coding";
			this->radioButtonArithmeticCoding->UseVisualStyleBackColor = true;
			this->radioButtonArithmeticCoding->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonArithmeticCoding_CheckedChanged);
			// 
			// radioButtonHuffmanCoding
			// 
			this->radioButtonHuffmanCoding->AutoSize = true;
			this->radioButtonHuffmanCoding->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonHuffmanCoding->Location = System::Drawing::Point(15, 26);
			this->radioButtonHuffmanCoding->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonHuffmanCoding->Name = L"radioButtonHuffmanCoding";
			this->radioButtonHuffmanCoding->Size = System::Drawing::Size(129, 21);
			this->radioButtonHuffmanCoding->TabIndex = 0;
			this->radioButtonHuffmanCoding->TabStop = true;
			this->radioButtonHuffmanCoding->Text = L"Huffman Coding";
			this->radioButtonHuffmanCoding->UseVisualStyleBackColor = true;
			this->radioButtonHuffmanCoding->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonHuffmanCoding_CheckedChanged);
			// 
			// groupBoxHistogramModification
			// 
			this->groupBoxHistogramModification->AccessibleDescription = L"reversible histogram modification";
			this->groupBoxHistogramModification->Controls->Add(this->radioButtonHistogramPermuteTSP);
			this->groupBoxHistogramModification->Controls->Add(this->radioButtonHistogramNoneMod);
			this->groupBoxHistogramModification->Controls->Add(this->radioButtonHistogramCompaction);
			this->groupBoxHistogramModification->Controls->Add(this->radioButtonHistogramPermutation);
			this->groupBoxHistogramModification->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->groupBoxHistogramModification->Location = System::Drawing::Point(374, 12);
			this->groupBoxHistogramModification->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxHistogramModification->Name = L"groupBoxHistogramModification";
			this->groupBoxHistogramModification->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxHistogramModification->Size = System::Drawing::Size(190, 139);
			this->groupBoxHistogramModification->TabIndex = 5;
			this->groupBoxHistogramModification->TabStop = false;
			this->groupBoxHistogramModification->Text = L"Histogram modification";
			// 
			// radioButtonHistogramPermuteTSP
			// 
			this->radioButtonHistogramPermuteTSP->AutoSize = true;
			this->radioButtonHistogramPermuteTSP->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonHistogramPermuteTSP->Location = System::Drawing::Point(12, 96);
			this->radioButtonHistogramPermuteTSP->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonHistogramPermuteTSP->Name = L"radioButtonHistogramPermuteTSP";
			this->radioButtonHistogramPermuteTSP->Size = System::Drawing::Size(112, 21);
			this->radioButtonHistogramPermuteTSP->TabIndex = 3;
			this->radioButtonHistogramPermuteTSP->TabStop = true;
			this->radioButtonHistogramPermuteTSP->Text = L"Permute TSP";
			this->radioButtonHistogramPermuteTSP->UseVisualStyleBackColor = true;
			// 
			// radioButtonHistogramNoneMod
			// 
			this->radioButtonHistogramNoneMod->AutoSize = true;
			this->radioButtonHistogramNoneMod->Checked = true;
			this->radioButtonHistogramNoneMod->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonHistogramNoneMod->Location = System::Drawing::Point(11, 24);
			this->radioButtonHistogramNoneMod->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonHistogramNoneMod->Name = L"radioButtonHistogramNoneMod";
			this->radioButtonHistogramNoneMod->Size = System::Drawing::Size(62, 21);
			this->radioButtonHistogramNoneMod->TabIndex = 0;
			this->radioButtonHistogramNoneMod->TabStop = true;
			this->radioButtonHistogramNoneMod->Text = L"None";
			this->radioButtonHistogramNoneMod->UseVisualStyleBackColor = true;
			this->radioButtonHistogramNoneMod->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonHistogramNoneMod_CheckedChanged);
			// 
			// radioButtonHistogramCompaction
			// 
			this->radioButtonHistogramCompaction->AutoSize = true;
			this->radioButtonHistogramCompaction->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonHistogramCompaction->Location = System::Drawing::Point(12, 45);
			this->radioButtonHistogramCompaction->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonHistogramCompaction->Name = L"radioButtonHistogramCompaction";
			this->radioButtonHistogramCompaction->Size = System::Drawing::Size(102, 21);
			this->radioButtonHistogramCompaction->TabIndex = 1;
			this->radioButtonHistogramCompaction->TabStop = true;
			this->radioButtonHistogramCompaction->Text = L"Compaction";
			this->radioButtonHistogramCompaction->UseVisualStyleBackColor = true;
			this->radioButtonHistogramCompaction->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonHistogramCompaction_CheckedChanged);
			// 
			// radioButtonHistogramPermutation
			// 
			this->radioButtonHistogramPermutation->AutoSize = true;
			this->radioButtonHistogramPermutation->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonHistogramPermutation->Location = System::Drawing::Point(12, 70);
			this->radioButtonHistogramPermutation->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonHistogramPermutation->Name = L"radioButtonHistogramPermutation";
			this->radioButtonHistogramPermutation->Size = System::Drawing::Size(104, 21);
			this->radioButtonHistogramPermutation->TabIndex = 2;
			this->radioButtonHistogramPermutation->TabStop = true;
			this->radioButtonHistogramPermutation->Text = L"Permutation";
			this->radioButtonHistogramPermutation->UseVisualStyleBackColor = true;
			this->radioButtonHistogramPermutation->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonHistogramPermutation_CheckedChanged);
			// 
			// checkBoxT4
			// 
			this->checkBoxT4->AutoSize = true;
			this->checkBoxT4->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxT4->Location = System::Drawing::Point(137, 70);
			this->checkBoxT4->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxT4->Name = L"checkBoxT4";
			this->checkBoxT4->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxT4->Size = System::Drawing::Size(71, 21);
			this->checkBoxT4->TabIndex = 14;
			this->checkBoxT4->Text = L"use T4";
			this->checkBoxT4->UseVisualStyleBackColor = true;
			this->checkBoxT4->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxT4_CheckedChanged);
			// 
			// checkBoxImprovedk
			// 
			this->checkBoxImprovedk->AutoSize = true;
			this->checkBoxImprovedk->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxImprovedk->Location = System::Drawing::Point(137, 95);
			this->checkBoxImprovedk->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxImprovedk->Name = L"checkBoxImprovedk";
			this->checkBoxImprovedk->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxImprovedk->Size = System::Drawing::Size(69, 21);
			this->checkBoxImprovedk->TabIndex = 15;
			this->checkBoxImprovedk->Text = L"impr. k";
			this->checkBoxImprovedk->UseVisualStyleBackColor = true;
			// 
			// groupBoxLogFile
			// 
			this->groupBoxLogFile->Controls->Add(this->checkBoxLogFile);
			this->groupBoxLogFile->Controls->Add(this->textBoxLogFile);
			this->groupBoxLogFile->Location = System::Drawing::Point(617, 617);
			this->groupBoxLogFile->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxLogFile->Name = L"groupBoxLogFile";
			this->groupBoxLogFile->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxLogFile->Size = System::Drawing::Size(224, 70);
			this->groupBoxLogFile->TabIndex = 8;
			this->groupBoxLogFile->TabStop = false;
			this->groupBoxLogFile->Text = L"Apend to logfile";
			// 
			// checkBoxLogFile
			// 
			this->checkBoxLogFile->AutoSize = true;
			this->checkBoxLogFile->Checked = true;
			this->checkBoxLogFile->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxLogFile->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxLogFile->Location = System::Drawing::Point(4, 34);
			this->checkBoxLogFile->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxLogFile->Name = L"checkBoxLogFile";
			this->checkBoxLogFile->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxLogFile->Size = System::Drawing::Size(31, 21);
			this->checkBoxLogFile->TabIndex = 15;
			this->checkBoxLogFile->Text = L".";
			this->checkBoxLogFile->UseVisualStyleBackColor = true;
			// 
			// textBoxLogFile
			// 
			this->textBoxLogFile->Location = System::Drawing::Point(56, 34);
			this->textBoxLogFile->Margin = System::Windows::Forms::Padding(2);
			this->textBoxLogFile->Name = L"textBoxLogFile";
			this->textBoxLogFile->Size = System::Drawing::Size(149, 23);
			this->textBoxLogFile->TabIndex = 0;
			this->textBoxLogFile->Text = L"log_TSIP.txt";
			// 
			// groupBoxSeries
			// 
			this->groupBoxSeries->Controls->Add(this->radioButtonSeparateCoding);
			this->groupBoxSeries->Controls->Add(this->radioButtonJointlyCoding);
			this->groupBoxSeries->Enabled = false;
			this->groupBoxSeries->Location = System::Drawing::Point(213, 671);
			this->groupBoxSeries->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxSeries->Name = L"groupBoxSeries";
			this->groupBoxSeries->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxSeries->Size = System::Drawing::Size(231, 75);
			this->groupBoxSeries->TabIndex = 13;
			this->groupBoxSeries->TabStop = false;
			this->groupBoxSeries->Text = L"Series";
			// 
			// radioButtonSeparateCoding
			// 
			this->radioButtonSeparateCoding->AutoSize = true;
			this->radioButtonSeparateCoding->Checked = true;
			this->radioButtonSeparateCoding->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonSeparateCoding->Location = System::Drawing::Point(17, 48);
			this->radioButtonSeparateCoding->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonSeparateCoding->Name = L"radioButtonSeparateCoding";
			this->radioButtonSeparateCoding->Size = System::Drawing::Size(84, 21);
			this->radioButtonSeparateCoding->TabIndex = 1;
			this->radioButtonSeparateCoding->TabStop = true;
			this->radioButtonSeparateCoding->Text = L"separate";
			this->radioButtonSeparateCoding->UseVisualStyleBackColor = true;
			this->radioButtonSeparateCoding->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonSeparateCoding_CheckedChanged);
			// 
			// radioButtonJointlyCoding
			// 
			this->radioButtonJointlyCoding->AutoSize = true;
			this->radioButtonJointlyCoding->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonJointlyCoding->Location = System::Drawing::Point(17, 26);
			this->radioButtonJointlyCoding->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonJointlyCoding->Name = L"radioButtonJointlyCoding";
			this->radioButtonJointlyCoding->Size = System::Drawing::Size(181, 21);
			this->radioButtonJointlyCoding->TabIndex = 0;
			this->radioButtonJointlyCoding->TabStop = true;
			this->radioButtonJointlyCoding->Text = L"Symbols and runs jointly";
			this->radioButtonJointlyCoding->UseVisualStyleBackColor = true;
			this->radioButtonJointlyCoding->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonJointlyCoding_CheckedChanged);
			// 
			// checkBoxUseZeroRLC2
			// 
			this->checkBoxUseZeroRLC2->AutoSize = true;
			this->checkBoxUseZeroRLC2->Enabled = false;
			this->checkBoxUseZeroRLC2->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxUseZeroRLC2->Location = System::Drawing::Point(491, 499);
			this->checkBoxUseZeroRLC2->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxUseZeroRLC2->Name = L"checkBoxUseZeroRLC2";
			this->checkBoxUseZeroRLC2->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxUseZeroRLC2->Size = System::Drawing::Size(148, 21);
			this->checkBoxUseZeroRLC2->TabIndex = 16;
			this->checkBoxUseZeroRLC2->Text = L"use RLC 2 of zeros";
			this->checkBoxUseZeroRLC2->UseVisualStyleBackColor = true;
			this->checkBoxUseZeroRLC2->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxUseZeroRLC2_CheckedChanged);
			// 
			// groupBoxPaletteSorting
			// 
			this->groupBoxPaletteSorting->Controls->Add(this->radioButtonLuminanceSorting);
			this->groupBoxPaletteSorting->Controls->Add(this->radioButtonGRBSorting);
			this->groupBoxPaletteSorting->Enabled = false;
			this->groupBoxPaletteSorting->Location = System::Drawing::Point(376, 428);
			this->groupBoxPaletteSorting->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxPaletteSorting->Name = L"groupBoxPaletteSorting";
			this->groupBoxPaletteSorting->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxPaletteSorting->Size = System::Drawing::Size(388, 59);
			this->groupBoxPaletteSorting->TabIndex = 14;
			this->groupBoxPaletteSorting->TabStop = false;
			this->groupBoxPaletteSorting->Text = L"Palette Sorting";
			// 
			// radioButtonLuminanceSorting
			// 
			this->radioButtonLuminanceSorting->AutoSize = true;
			this->radioButtonLuminanceSorting->Checked = true;
			this->radioButtonLuminanceSorting->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonLuminanceSorting->Location = System::Drawing::Point(205, 24);
			this->radioButtonLuminanceSorting->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonLuminanceSorting->Name = L"radioButtonLuminanceSorting";
			this->radioButtonLuminanceSorting->Size = System::Drawing::Size(135, 21);
			this->radioButtonLuminanceSorting->TabIndex = 1;
			this->radioButtonLuminanceSorting->TabStop = true;
			this->radioButtonLuminanceSorting->Text = L"luminance based";
			this->radioButtonLuminanceSorting->UseVisualStyleBackColor = true;
			this->radioButtonLuminanceSorting->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonLuminanceSorting_CheckedChanged);
			// 
			// radioButtonGRBSorting
			// 
			this->radioButtonGRBSorting->AutoSize = true;
			this->radioButtonGRBSorting->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonGRBSorting->Location = System::Drawing::Point(17, 24);
			this->radioButtonGRBSorting->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonGRBSorting->Name = L"radioButtonGRBSorting";
			this->radioButtonGRBSorting->Size = System::Drawing::Size(123, 21);
			this->radioButtonGRBSorting->TabIndex = 0;
			this->radioButtonGRBSorting->TabStop = true;
			this->radioButtonGRBSorting->Text = L"green-red-blue";
			this->radioButtonGRBSorting->UseVisualStyleBackColor = true;
			this->radioButtonGRBSorting->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonGRBSorting_CheckedChanged);
			// 
			// groupBoxCompressionMode
			// 
			this->groupBoxCompressionMode->AccessibleDescription = L"selection of the compression mode";
			this->groupBoxCompressionMode->Controls->Add(this->radioButtonSCFmode);
			this->groupBoxCompressionMode->Controls->Add(this->radioButtonHXC2mode);
			this->groupBoxCompressionMode->Controls->Add(this->radioButtonHXCmode);
			this->groupBoxCompressionMode->Controls->Add(this->radioButtonCoBaLP2mode);
			this->groupBoxCompressionMode->Controls->Add(this->radioButtonLOCOmode);
			this->groupBoxCompressionMode->Controls->Add(this->radioButtonTSIPmode);
			this->groupBoxCompressionMode->Controls->Add(this->radioButtonBitPlaneMode);
			this->groupBoxCompressionMode->Controls->Add(this->checkBoxT4);
			this->groupBoxCompressionMode->Controls->Add(this->checkBoxImprovedk);
			this->groupBoxCompressionMode->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->groupBoxCompressionMode->Location = System::Drawing::Point(578, 12);
			this->groupBoxCompressionMode->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxCompressionMode->Name = L"groupBoxCompressionMode";
			this->groupBoxCompressionMode->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxCompressionMode->Size = System::Drawing::Size(337, 126);
			this->groupBoxCompressionMode->TabIndex = 18;
			this->groupBoxCompressionMode->TabStop = false;
			this->groupBoxCompressionMode->Text = L"Compression Mode";
			// 
			// radioButtonSCFmode
			// 
			this->radioButtonSCFmode->AutoSize = true;
			this->radioButtonSCFmode->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonSCFmode->Location = System::Drawing::Point(232, 96);
			this->radioButtonSCFmode->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonSCFmode->Name = L"radioButtonSCFmode";
			this->radioButtonSCFmode->Size = System::Drawing::Size(54, 21);
			this->radioButtonSCFmode->TabIndex = 6;
			this->radioButtonSCFmode->TabStop = true;
			this->radioButtonSCFmode->Text = L"SCF";
			this->radioButtonSCFmode->UseVisualStyleBackColor = true;
			this->radioButtonSCFmode->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonSCFmode_CheckedChanged);
			// 
			// radioButtonHXC2mode
			// 
			this->radioButtonHXC2mode->AutoSize = true;
			this->radioButtonHXC2mode->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonHXC2mode->Location = System::Drawing::Point(232, 55);
			this->radioButtonHXC2mode->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonHXC2mode->Name = L"radioButtonHXC2mode";
			this->radioButtonHXC2mode->Size = System::Drawing::Size(77, 21);
			this->radioButtonHXC2mode->TabIndex = 5;
			this->radioButtonHXC2mode->TabStop = true;
			this->radioButtonHXC2mode->Text = L"H(X|C)2";
			this->radioButtonHXC2mode->UseVisualStyleBackColor = true;
			this->radioButtonHXC2mode->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonHXC2mode_CheckedChanged);
			// 
			// radioButtonHXCmode
			// 
			this->radioButtonHXCmode->AutoSize = true;
			this->radioButtonHXCmode->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonHXCmode->Location = System::Drawing::Point(232, 21);
			this->radioButtonHXCmode->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonHXCmode->Name = L"radioButtonHXCmode";
			this->radioButtonHXCmode->Size = System::Drawing::Size(69, 21);
			this->radioButtonHXCmode->TabIndex = 4;
			this->radioButtonHXCmode->TabStop = true;
			this->radioButtonHXCmode->Text = L"H(X|C)";
			this->radioButtonHXCmode->UseVisualStyleBackColor = true;
			this->radioButtonHXCmode->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonHXCmode_CheckedChanged);
			// 
			// radioButtonCoBaLP2mode
			// 
			this->radioButtonCoBaLP2mode->AutoSize = true;
			this->radioButtonCoBaLP2mode->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonCoBaLP2mode->Location = System::Drawing::Point(122, 24);
			this->radioButtonCoBaLP2mode->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonCoBaLP2mode->Name = L"radioButtonCoBaLP2mode";
			this->radioButtonCoBaLP2mode->Size = System::Drawing::Size(87, 21);
			this->radioButtonCoBaLP2mode->TabIndex = 3;
			this->radioButtonCoBaLP2mode->TabStop = true;
			this->radioButtonCoBaLP2mode->Text = L"CoBaLP2";
			this->radioButtonCoBaLP2mode->UseVisualStyleBackColor = true;
			this->radioButtonCoBaLP2mode->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonCoBaLP2mode_CheckedChanged);
			// 
			// radioButtonLOCOmode
			// 
			this->radioButtonLOCOmode->AutoSize = true;
			this->radioButtonLOCOmode->Checked = true;
			this->radioButtonLOCOmode->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonLOCOmode->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->radioButtonLOCOmode->Location = System::Drawing::Point(122, 55);
			this->radioButtonLOCOmode->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonLOCOmode->Name = L"radioButtonLOCOmode";
			this->radioButtonLOCOmode->Size = System::Drawing::Size(67, 21);
			this->radioButtonLOCOmode->TabIndex = 0;
			this->radioButtonLOCOmode->TabStop = true;
			this->radioButtonLOCOmode->Text = L"LOCO";
			this->radioButtonLOCOmode->UseVisualStyleBackColor = true;
			this->radioButtonLOCOmode->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonLOCOmode_CheckedChanged);
			// 
			// radioButtonTSIPmode
			// 
			this->radioButtonTSIPmode->AutoSize = true;
			this->radioButtonTSIPmode->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonTSIPmode->Location = System::Drawing::Point(17, 28);
			this->radioButtonTSIPmode->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonTSIPmode->Name = L"radioButtonTSIPmode";
			this->radioButtonTSIPmode->Size = System::Drawing::Size(58, 21);
			this->radioButtonTSIPmode->TabIndex = 1;
			this->radioButtonTSIPmode->TabStop = true;
			this->radioButtonTSIPmode->Text = L"TSIP";
			this->radioButtonTSIPmode->UseVisualStyleBackColor = true;
			this->radioButtonTSIPmode->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonTSIPmode_CheckedChanged);
			// 
			// radioButtonBitPlaneMode
			// 
			this->radioButtonBitPlaneMode->AutoSize = true;
			this->radioButtonBitPlaneMode->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->radioButtonBitPlaneMode->Location = System::Drawing::Point(17, 55);
			this->radioButtonBitPlaneMode->Margin = System::Windows::Forms::Padding(2);
			this->radioButtonBitPlaneMode->Name = L"radioButtonBitPlaneMode";
			this->radioButtonBitPlaneMode->Size = System::Drawing::Size(55, 21);
			this->radioButtonBitPlaneMode->TabIndex = 2;
			this->radioButtonBitPlaneMode->Text = L"BPC";
			this->radioButtonBitPlaneMode->UseVisualStyleBackColor = true;
			this->radioButtonBitPlaneMode->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::radioButtonBitPlaneMode_CheckedChanged);
			// 
			// groupBoxHXCparams
			// 
			this->groupBoxHXCparams->Controls->Add(this->checkBoxHXCauto);
			this->groupBoxHXCparams->Controls->Add(this->textBoxIncreaseFac);
			this->groupBoxHXCparams->Controls->Add(this->labelIncreaseFac);
			this->groupBoxHXCparams->Controls->Add(this->textBoxToleranceOffset2);
			this->groupBoxHXCparams->Controls->Add(this->textBoxToleranceOffset1);
			this->groupBoxHXCparams->Controls->Add(this->labelToleranceOffset2);
			this->groupBoxHXCparams->Controls->Add(this->labelToleranceOffset1);
			this->groupBoxHXCparams->Controls->Add(this->textBoxHXCtolerance);
			this->groupBoxHXCparams->Controls->Add(this->labelTolerance);
			this->groupBoxHXCparams->Enabled = false;
			this->groupBoxHXCparams->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBoxHXCparams->Location = System::Drawing::Point(770, 384);
			this->groupBoxHXCparams->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxHXCparams->Name = L"groupBoxHXCparams";
			this->groupBoxHXCparams->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxHXCparams->Size = System::Drawing::Size(153, 221);
			this->groupBoxHXCparams->TabIndex = 8;
			this->groupBoxHXCparams->TabStop = false;
			this->groupBoxHXCparams->Text = L"H(X|C)";
			// 
			// checkBoxHXCauto
			// 
			this->checkBoxHXCauto->AutoSize = true;
			this->checkBoxHXCauto->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxHXCauto->Location = System::Drawing::Point(10, 190);
			this->checkBoxHXCauto->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxHXCauto->Name = L"checkBoxHXCauto";
			this->checkBoxHXCauto->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxHXCauto->Size = System::Drawing::Size(110, 21);
			this->checkBoxHXCauto->TabIndex = 19;
			this->checkBoxHXCauto->Text = L"auto Settings";
			this->checkBoxHXCauto->UseVisualStyleBackColor = true;
			this->checkBoxHXCauto->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxHXCauto_CheckedChanged);
			// 
			// textBoxIncreaseFac
			// 
			this->textBoxIncreaseFac->Location = System::Drawing::Point(88, 141);
			this->textBoxIncreaseFac->Margin = System::Windows::Forms::Padding(2);
			this->textBoxIncreaseFac->MaxLength = 100;
			this->textBoxIncreaseFac->Name = L"textBoxIncreaseFac";
			this->textBoxIncreaseFac->Size = System::Drawing::Size(57, 23);
			this->textBoxIncreaseFac->TabIndex = 7;
			this->textBoxIncreaseFac->Text = L"1";
			this->textBoxIncreaseFac->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// labelIncreaseFac
			// 
			this->labelIncreaseFac->AutoSize = true;
			this->labelIncreaseFac->Location = System::Drawing::Point(4, 119);
			this->labelIncreaseFac->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelIncreaseFac->Name = L"labelIncreaseFac";
			this->labelIncreaseFac->Size = System::Drawing::Size(89, 17);
			this->labelIncreaseFac->TabIndex = 6;
			this->labelIncreaseFac->Text = L"IncreaseFac:";
			// 
			// textBoxToleranceOffset2
			// 
			this->textBoxToleranceOffset2->Location = System::Drawing::Point(88, 82);
			this->textBoxToleranceOffset2->Margin = System::Windows::Forms::Padding(2);
			this->textBoxToleranceOffset2->MaxLength = 100;
			this->textBoxToleranceOffset2->Name = L"textBoxToleranceOffset2";
			this->textBoxToleranceOffset2->Size = System::Drawing::Size(57, 23);
			this->textBoxToleranceOffset2->TabIndex = 5;
			this->textBoxToleranceOffset2->Text = L"0";
			this->textBoxToleranceOffset2->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxToleranceOffset1
			// 
			this->textBoxToleranceOffset1->Location = System::Drawing::Point(88, 51);
			this->textBoxToleranceOffset1->Margin = System::Windows::Forms::Padding(2);
			this->textBoxToleranceOffset1->MaxLength = 100;
			this->textBoxToleranceOffset1->Name = L"textBoxToleranceOffset1";
			this->textBoxToleranceOffset1->Size = System::Drawing::Size(57, 23);
			this->textBoxToleranceOffset1->TabIndex = 4;
			this->textBoxToleranceOffset1->Text = L"0";
			this->textBoxToleranceOffset1->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// labelToleranceOffset2
			// 
			this->labelToleranceOffset2->AutoSize = true;
			this->labelToleranceOffset2->Location = System::Drawing::Point(4, 86);
			this->labelToleranceOffset2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelToleranceOffset2->Name = L"labelToleranceOffset2";
			this->labelToleranceOffset2->Size = System::Drawing::Size(67, 17);
			this->labelToleranceOffset2->TabIndex = 3;
			this->labelToleranceOffset2->Text = L"Tol. Off2:";
			// 
			// labelToleranceOffset1
			// 
			this->labelToleranceOffset1->AutoSize = true;
			this->labelToleranceOffset1->Location = System::Drawing::Point(4, 55);
			this->labelToleranceOffset1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelToleranceOffset1->Name = L"labelToleranceOffset1";
			this->labelToleranceOffset1->Size = System::Drawing::Size(67, 17);
			this->labelToleranceOffset1->TabIndex = 2;
			this->labelToleranceOffset1->Text = L"Tol. Off1:";
			// 
			// textBoxHXCtolerance
			// 
			this->textBoxHXCtolerance->Location = System::Drawing::Point(88, 19);
			this->textBoxHXCtolerance->Margin = System::Windows::Forms::Padding(2);
			this->textBoxHXCtolerance->MaxLength = 100;
			this->textBoxHXCtolerance->Name = L"textBoxHXCtolerance";
			this->textBoxHXCtolerance->Size = System::Drawing::Size(57, 23);
			this->textBoxHXCtolerance->TabIndex = 1;
			this->textBoxHXCtolerance->Text = L"0";
			this->textBoxHXCtolerance->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// labelTolerance
			// 
			this->labelTolerance->AutoSize = true;
			this->labelTolerance->Location = System::Drawing::Point(4, 22);
			this->labelTolerance->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelTolerance->Name = L"labelTolerance";
			this->labelTolerance->Size = System::Drawing::Size(76, 17);
			this->labelTolerance->TabIndex = 0;
			this->labelTolerance->Text = L"Tolerance:";
			// 
			// groupBoxCoBaLP2
			// 
			this->groupBoxCoBaLP2->Controls->Add(this->checkBoxConstLines);
			this->groupBoxCoBaLP2->Controls->Add(this->checkBoxEnablePredR);
			this->groupBoxCoBaLP2->Controls->Add(this->checkBoxSkipPredB);
			this->groupBoxCoBaLP2->Controls->Add(this->checkBoxSkipPredA);
			this->groupBoxCoBaLP2->Controls->Add(this->checkBoxSkipTM);
			this->groupBoxCoBaLP2->Enabled = false;
			this->groupBoxCoBaLP2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBoxCoBaLP2->Location = System::Drawing::Point(770, 155);
			this->groupBoxCoBaLP2->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxCoBaLP2->Name = L"groupBoxCoBaLP2";
			this->groupBoxCoBaLP2->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxCoBaLP2->Size = System::Drawing::Size(153, 209);
			this->groupBoxCoBaLP2->TabIndex = 9;
			this->groupBoxCoBaLP2->TabStop = false;
			this->groupBoxCoBaLP2->Text = L"CoBaLP2";
			// 
			// checkBoxConstLines
			// 
			this->checkBoxConstLines->AutoSize = true;
			this->checkBoxConstLines->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxConstLines->Location = System::Drawing::Point(6, 130);
			this->checkBoxConstLines->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxConstLines->Name = L"checkBoxConstLines";
			this->checkBoxConstLines->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxConstLines->Size = System::Drawing::Size(124, 21);
			this->checkBoxConstLines->TabIndex = 23;
			this->checkBoxConstLines->Text = L"use ConstLines";
			this->checkBoxConstLines->UseVisualStyleBackColor = true;
			// 
			// checkBoxEnablePredR
			// 
			this->checkBoxEnablePredR->AutoSize = true;
			this->checkBoxEnablePredR->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxEnablePredR->Location = System::Drawing::Point(43, 102);
			this->checkBoxEnablePredR->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxEnablePredR->Name = L"checkBoxEnablePredR";
			this->checkBoxEnablePredR->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxEnablePredR->Size = System::Drawing::Size(93, 21);
			this->checkBoxEnablePredR->TabIndex = 22;
			this->checkBoxEnablePredR->Text = L"enable PR";
			this->checkBoxEnablePredR->UseVisualStyleBackColor = true;
			// 
			// checkBoxSkipPredB
			// 
			this->checkBoxSkipPredB->AutoSize = true;
			this->checkBoxSkipPredB->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxSkipPredB->Location = System::Drawing::Point(62, 72);
			this->checkBoxSkipPredB->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxSkipPredB->Name = L"checkBoxSkipPredB";
			this->checkBoxSkipPredB->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxSkipPredB->Size = System::Drawing::Size(74, 21);
			this->checkBoxSkipPredB->TabIndex = 21;
			this->checkBoxSkipPredB->Text = L"skip PB";
			this->checkBoxSkipPredB->UseVisualStyleBackColor = true;
			// 
			// checkBoxSkipPredA
			// 
			this->checkBoxSkipPredA->AutoSize = true;
			this->checkBoxSkipPredA->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxSkipPredA->Location = System::Drawing::Point(62, 41);
			this->checkBoxSkipPredA->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxSkipPredA->Name = L"checkBoxSkipPredA";
			this->checkBoxSkipPredA->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxSkipPredA->Size = System::Drawing::Size(74, 21);
			this->checkBoxSkipPredA->TabIndex = 20;
			this->checkBoxSkipPredA->Text = L"skip PA";
			this->checkBoxSkipPredA->UseVisualStyleBackColor = true;
			this->checkBoxSkipPredA->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxSkipPredA_CheckedChanged);
			// 
			// checkBoxSkipTM
			// 
			this->checkBoxSkipTM->AutoSize = true;
			this->checkBoxSkipTM->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxSkipTM->Location = System::Drawing::Point(60, 16);
			this->checkBoxSkipTM->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxSkipTM->Name = L"checkBoxSkipTM";
			this->checkBoxSkipTM->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxSkipTM->Size = System::Drawing::Size(76, 21);
			this->checkBoxSkipTM->TabIndex = 19;
			this->checkBoxSkipTM->Text = L"skip TM";
			this->checkBoxSkipTM->UseVisualStyleBackColor = true;
			this->checkBoxSkipTM->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxSkipTM_CheckedChanged);
			// 
			// groupBoxBPC
			// 
			this->groupBoxBPC->Controls->Add(this->checkBox_usePredFlag);
			this->groupBoxBPC->Controls->Add(this->checkBox_useColFlag);
			this->groupBoxBPC->Controls->Add(this->checkBoxSignificance);
			this->groupBoxBPC->Enabled = false;
			this->groupBoxBPC->Location = System::Drawing::Point(940, 117);
			this->groupBoxBPC->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxBPC->Name = L"groupBoxBPC";
			this->groupBoxBPC->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxBPC->Size = System::Drawing::Size(254, 80);
			this->groupBoxBPC->TabIndex = 24;
			this->groupBoxBPC->TabStop = false;
			this->groupBoxBPC->Text = L"BPC";
			// 
			// checkBox_usePredFlag
			// 
			this->checkBox_usePredFlag->AutoSize = true;
			this->checkBox_usePredFlag->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBox_usePredFlag->Location = System::Drawing::Point(131, 22);
			this->checkBox_usePredFlag->Margin = System::Windows::Forms::Padding(2);
			this->checkBox_usePredFlag->Name = L"checkBox_usePredFlag";
			this->checkBox_usePredFlag->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBox_usePredFlag->Size = System::Drawing::Size(110, 21);
			this->checkBox_usePredFlag->TabIndex = 21;
			this->checkBox_usePredFlag->Text = L"use predFlag";
			this->checkBox_usePredFlag->UseVisualStyleBackColor = true;
			// 
			// checkBox_useColFlag
			// 
			this->checkBox_useColFlag->AutoSize = true;
			this->checkBox_useColFlag->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBox_useColFlag->Location = System::Drawing::Point(23, 47);
			this->checkBox_useColFlag->Margin = System::Windows::Forms::Padding(2);
			this->checkBox_useColFlag->Name = L"checkBox_useColFlag";
			this->checkBox_useColFlag->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBox_useColFlag->Size = System::Drawing::Size(99, 21);
			this->checkBox_useColFlag->TabIndex = 20;
			this->checkBox_useColFlag->Text = L"use colFlag";
			this->checkBox_useColFlag->UseVisualStyleBackColor = true;
			// 
			// checkBoxSignificance
			// 
			this->checkBoxSignificance->AutoSize = true;
			this->checkBoxSignificance->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxSignificance->Location = System::Drawing::Point(17, 22);
			this->checkBoxSignificance->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxSignificance->Name = L"checkBoxSignificance";
			this->checkBoxSignificance->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxSignificance->Size = System::Drawing::Size(105, 21);
			this->checkBoxSignificance->TabIndex = 19;
			this->checkBoxSignificance->Text = L"check_signif";
			this->checkBoxSignificance->UseVisualStyleBackColor = true;
			// 
			// groupBoxHXC2params
			// 
			this->groupBoxHXC2params->Controls->Add(this->checkBoxHXC2auto);
			this->groupBoxHXC2params->Controls->Add(this->textBoxIncreaseFac2);
			this->groupBoxHXC2params->Controls->Add(this->labelIncreaseFac2);
			this->groupBoxHXC2params->Controls->Add(this->label8);
			this->groupBoxHXC2params->Controls->Add(this->label7);
			this->groupBoxHXC2params->Controls->Add(this->label6);
			this->groupBoxHXC2params->Controls->Add(this->label4);
			this->groupBoxHXC2params->Controls->Add(this->label2);
			this->groupBoxHXC2params->Controls->Add(this->label1);
			this->groupBoxHXC2params->Controls->Add(this->labelTolA);
			this->groupBoxHXC2params->Controls->Add(this->labelTolB);
			this->groupBoxHXC2params->Controls->Add(this->labelTolG);
			this->groupBoxHXC2params->Controls->Add(this->labelTolR);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol4A);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol5A);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol3A);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol2A);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol1A);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol0A);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol4G);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol5G);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol3G);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol2G);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol1G);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol0G);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol4B);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol5B);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol3B);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol2B);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol1B);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol0B);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol4R);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol5R);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol3R);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol2R);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol1R);
			this->groupBoxHXC2params->Controls->Add(this->textBoxTol0R);
			this->groupBoxHXC2params->Controls->Add(this->label5);
			this->groupBoxHXC2params->Enabled = false;
			this->groupBoxHXC2params->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBoxHXC2params->Location = System::Drawing::Point(940, 343);
			this->groupBoxHXC2params->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxHXC2params->Name = L"groupBoxHXC2params";
			this->groupBoxHXC2params->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxHXC2params->Size = System::Drawing::Size(254, 262);
			this->groupBoxHXC2params->TabIndex = 20;
			this->groupBoxHXC2params->TabStop = false;
			this->groupBoxHXC2params->Text = L"H(X|C)2 / SCF";
			// 
			// checkBoxHXC2auto
			// 
			this->checkBoxHXC2auto->AutoSize = true;
			this->checkBoxHXC2auto->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxHXC2auto->Location = System::Drawing::Point(8, 235);
			this->checkBoxHXC2auto->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxHXC2auto->Name = L"checkBoxHXC2auto";
			this->checkBoxHXC2auto->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxHXC2auto->Size = System::Drawing::Size(110, 21);
			this->checkBoxHXC2auto->TabIndex = 20;
			this->checkBoxHXC2auto->Text = L"auto Settings";
			this->checkBoxHXC2auto->UseVisualStyleBackColor = true;
			this->checkBoxHXC2auto->CheckedChanged += gcnew System::EventHandler(this, &ConfigForm::checkBoxHXC2auto_CheckedChanged);
			// 
			// textBoxIncreaseFac2
			// 
			this->textBoxIncreaseFac2->Location = System::Drawing::Point(111, 204);
			this->textBoxIncreaseFac2->Margin = System::Windows::Forms::Padding(2);
			this->textBoxIncreaseFac2->MaxLength = 100;
			this->textBoxIncreaseFac2->Name = L"textBoxIncreaseFac2";
			this->textBoxIncreaseFac2->Size = System::Drawing::Size(57, 23);
			this->textBoxIncreaseFac2->TabIndex = 20;
			this->textBoxIncreaseFac2->Text = L"1";
			this->textBoxIncreaseFac2->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// labelIncreaseFac2
			// 
			this->labelIncreaseFac2->AutoSize = true;
			this->labelIncreaseFac2->Location = System::Drawing::Point(9, 208);
			this->labelIncreaseFac2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelIncreaseFac2->Name = L"labelIncreaseFac2";
			this->labelIncreaseFac2->Size = System::Drawing::Size(89, 17);
			this->labelIncreaseFac2->TabIndex = 20;
			this->labelIncreaseFac2->Text = L"IncreaseFac:";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(173, 55);
			this->label8->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(16, 17);
			this->label8->TabIndex = 37;
			this->label8->Text = L"F";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(142, 55);
			this->label7->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(17, 17);
			this->label7->TabIndex = 36;
			this->label7->Text = L"E";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(108, 55);
			this->label6->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(18, 17);
			this->label6->TabIndex = 35;
			this->label6->Text = L"D";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(76, 55);
			this->label4->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(17, 17);
			this->label4->TabIndex = 34;
			this->label4->Text = L"C";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(43, 55);
			this->label2->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(17, 17);
			this->label2->TabIndex = 33;
			this->label2->Text = L"B";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(8, 55);
			this->label1->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(17, 17);
			this->label1->TabIndex = 32;
			this->label1->Text = L"A";
			// 
			// labelTolA
			// 
			this->labelTolA->AutoSize = true;
			this->labelTolA->Location = System::Drawing::Point(215, 172);
			this->labelTolA->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelTolA->Name = L"labelTolA";
			this->labelTolA->Size = System::Drawing::Size(17, 17);
			this->labelTolA->TabIndex = 31;
			this->labelTolA->Text = L"A";
			// 
			// labelTolB
			// 
			this->labelTolB->AutoSize = true;
			this->labelTolB->Location = System::Drawing::Point(215, 141);
			this->labelTolB->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelTolB->Name = L"labelTolB";
			this->labelTolB->Size = System::Drawing::Size(17, 17);
			this->labelTolB->TabIndex = 30;
			this->labelTolB->Text = L"B";
			// 
			// labelTolG
			// 
			this->labelTolG->AutoSize = true;
			this->labelTolG->Location = System::Drawing::Point(215, 110);
			this->labelTolG->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelTolG->Name = L"labelTolG";
			this->labelTolG->Size = System::Drawing::Size(19, 17);
			this->labelTolG->TabIndex = 29;
			this->labelTolG->Text = L"G";
			// 
			// labelTolR
			// 
			this->labelTolR->AutoSize = true;
			this->labelTolR->Location = System::Drawing::Point(215, 81);
			this->labelTolR->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->labelTolR->Name = L"labelTolR";
			this->labelTolR->Size = System::Drawing::Size(18, 17);
			this->labelTolR->TabIndex = 28;
			this->labelTolR->Text = L"R";
			// 
			// textBoxTol4A
			// 
			this->textBoxTol4A->Location = System::Drawing::Point(145, 172);
			this->textBoxTol4A->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol4A->MaxLength = 100;
			this->textBoxTol4A->Name = L"textBoxTol4A";
			this->textBoxTol4A->Size = System::Drawing::Size(30, 23);
			this->textBoxTol4A->TabIndex = 27;
			this->textBoxTol4A->Text = L"1";
			this->textBoxTol4A->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol5A
			// 
			this->textBoxTol5A->Location = System::Drawing::Point(177, 172);
			this->textBoxTol5A->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol5A->MaxLength = 100;
			this->textBoxTol5A->Name = L"textBoxTol5A";
			this->textBoxTol5A->Size = System::Drawing::Size(30, 23);
			this->textBoxTol5A->TabIndex = 26;
			this->textBoxTol5A->Text = L"1";
			this->textBoxTol5A->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol3A
			// 
			this->textBoxTol3A->Location = System::Drawing::Point(110, 172);
			this->textBoxTol3A->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol3A->MaxLength = 100;
			this->textBoxTol3A->Name = L"textBoxTol3A";
			this->textBoxTol3A->Size = System::Drawing::Size(30, 23);
			this->textBoxTol3A->TabIndex = 25;
			this->textBoxTol3A->Text = L"1";
			this->textBoxTol3A->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol2A
			// 
			this->textBoxTol2A->Location = System::Drawing::Point(75, 172);
			this->textBoxTol2A->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol2A->MaxLength = 100;
			this->textBoxTol2A->Name = L"textBoxTol2A";
			this->textBoxTol2A->Size = System::Drawing::Size(30, 23);
			this->textBoxTol2A->TabIndex = 24;
			this->textBoxTol2A->Text = L"0";
			this->textBoxTol2A->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol1A
			// 
			this->textBoxTol1A->Location = System::Drawing::Point(42, 172);
			this->textBoxTol1A->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol1A->MaxLength = 100;
			this->textBoxTol1A->Name = L"textBoxTol1A";
			this->textBoxTol1A->Size = System::Drawing::Size(29, 23);
			this->textBoxTol1A->TabIndex = 23;
			this->textBoxTol1A->Text = L"0";
			this->textBoxTol1A->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol0A
			// 
			this->textBoxTol0A->Location = System::Drawing::Point(8, 172);
			this->textBoxTol0A->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol0A->MaxLength = 100;
			this->textBoxTol0A->Name = L"textBoxTol0A";
			this->textBoxTol0A->Size = System::Drawing::Size(29, 23);
			this->textBoxTol0A->TabIndex = 22;
			this->textBoxTol0A->Text = L"0";
			this->textBoxTol0A->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol4G
			// 
			this->textBoxTol4G->Location = System::Drawing::Point(145, 110);
			this->textBoxTol4G->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol4G->MaxLength = 100;
			this->textBoxTol4G->Name = L"textBoxTol4G";
			this->textBoxTol4G->Size = System::Drawing::Size(30, 23);
			this->textBoxTol4G->TabIndex = 21;
			this->textBoxTol4G->Text = L"1";
			this->textBoxTol4G->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol5G
			// 
			this->textBoxTol5G->Location = System::Drawing::Point(177, 110);
			this->textBoxTol5G->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol5G->MaxLength = 100;
			this->textBoxTol5G->Name = L"textBoxTol5G";
			this->textBoxTol5G->Size = System::Drawing::Size(30, 23);
			this->textBoxTol5G->TabIndex = 20;
			this->textBoxTol5G->Text = L"1";
			this->textBoxTol5G->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol3G
			// 
			this->textBoxTol3G->Location = System::Drawing::Point(110, 110);
			this->textBoxTol3G->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol3G->MaxLength = 100;
			this->textBoxTol3G->Name = L"textBoxTol3G";
			this->textBoxTol3G->Size = System::Drawing::Size(30, 23);
			this->textBoxTol3G->TabIndex = 19;
			this->textBoxTol3G->Text = L"1";
			this->textBoxTol3G->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol2G
			// 
			this->textBoxTol2G->Location = System::Drawing::Point(75, 110);
			this->textBoxTol2G->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol2G->MaxLength = 100;
			this->textBoxTol2G->Name = L"textBoxTol2G";
			this->textBoxTol2G->Size = System::Drawing::Size(30, 23);
			this->textBoxTol2G->TabIndex = 18;
			this->textBoxTol2G->Text = L"0";
			this->textBoxTol2G->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol1G
			// 
			this->textBoxTol1G->Location = System::Drawing::Point(42, 110);
			this->textBoxTol1G->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol1G->MaxLength = 100;
			this->textBoxTol1G->Name = L"textBoxTol1G";
			this->textBoxTol1G->Size = System::Drawing::Size(29, 23);
			this->textBoxTol1G->TabIndex = 17;
			this->textBoxTol1G->Text = L"0";
			this->textBoxTol1G->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol0G
			// 
			this->textBoxTol0G->Location = System::Drawing::Point(8, 110);
			this->textBoxTol0G->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol0G->MaxLength = 100;
			this->textBoxTol0G->Name = L"textBoxTol0G";
			this->textBoxTol0G->Size = System::Drawing::Size(29, 23);
			this->textBoxTol0G->TabIndex = 16;
			this->textBoxTol0G->Text = L"0";
			this->textBoxTol0G->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol4B
			// 
			this->textBoxTol4B->Location = System::Drawing::Point(145, 141);
			this->textBoxTol4B->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol4B->MaxLength = 100;
			this->textBoxTol4B->Name = L"textBoxTol4B";
			this->textBoxTol4B->Size = System::Drawing::Size(30, 23);
			this->textBoxTol4B->TabIndex = 15;
			this->textBoxTol4B->Text = L"1";
			this->textBoxTol4B->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol5B
			// 
			this->textBoxTol5B->Location = System::Drawing::Point(177, 141);
			this->textBoxTol5B->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol5B->MaxLength = 100;
			this->textBoxTol5B->Name = L"textBoxTol5B";
			this->textBoxTol5B->Size = System::Drawing::Size(30, 23);
			this->textBoxTol5B->TabIndex = 14;
			this->textBoxTol5B->Text = L"1";
			this->textBoxTol5B->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol3B
			// 
			this->textBoxTol3B->Location = System::Drawing::Point(110, 141);
			this->textBoxTol3B->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol3B->MaxLength = 100;
			this->textBoxTol3B->Name = L"textBoxTol3B";
			this->textBoxTol3B->Size = System::Drawing::Size(30, 23);
			this->textBoxTol3B->TabIndex = 13;
			this->textBoxTol3B->Text = L"1";
			this->textBoxTol3B->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol2B
			// 
			this->textBoxTol2B->Location = System::Drawing::Point(75, 141);
			this->textBoxTol2B->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol2B->MaxLength = 100;
			this->textBoxTol2B->Name = L"textBoxTol2B";
			this->textBoxTol2B->Size = System::Drawing::Size(30, 23);
			this->textBoxTol2B->TabIndex = 12;
			this->textBoxTol2B->Text = L"0";
			this->textBoxTol2B->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol1B
			// 
			this->textBoxTol1B->Location = System::Drawing::Point(42, 141);
			this->textBoxTol1B->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol1B->MaxLength = 100;
			this->textBoxTol1B->Name = L"textBoxTol1B";
			this->textBoxTol1B->Size = System::Drawing::Size(29, 23);
			this->textBoxTol1B->TabIndex = 11;
			this->textBoxTol1B->Text = L"0";
			this->textBoxTol1B->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol0B
			// 
			this->textBoxTol0B->Location = System::Drawing::Point(8, 141);
			this->textBoxTol0B->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol0B->MaxLength = 100;
			this->textBoxTol0B->Name = L"textBoxTol0B";
			this->textBoxTol0B->Size = System::Drawing::Size(29, 23);
			this->textBoxTol0B->TabIndex = 10;
			this->textBoxTol0B->Text = L"0";
			this->textBoxTol0B->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol4R
			// 
			this->textBoxTol4R->Location = System::Drawing::Point(145, 78);
			this->textBoxTol4R->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol4R->MaxLength = 100;
			this->textBoxTol4R->Name = L"textBoxTol4R";
			this->textBoxTol4R->Size = System::Drawing::Size(30, 23);
			this->textBoxTol4R->TabIndex = 9;
			this->textBoxTol4R->Text = L"1";
			this->textBoxTol4R->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol5R
			// 
			this->textBoxTol5R->Location = System::Drawing::Point(177, 78);
			this->textBoxTol5R->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol5R->MaxLength = 100;
			this->textBoxTol5R->Name = L"textBoxTol5R";
			this->textBoxTol5R->Size = System::Drawing::Size(30, 23);
			this->textBoxTol5R->TabIndex = 8;
			this->textBoxTol5R->Text = L"1";
			this->textBoxTol5R->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol3R
			// 
			this->textBoxTol3R->Location = System::Drawing::Point(110, 78);
			this->textBoxTol3R->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol3R->MaxLength = 100;
			this->textBoxTol3R->Name = L"textBoxTol3R";
			this->textBoxTol3R->Size = System::Drawing::Size(30, 23);
			this->textBoxTol3R->TabIndex = 7;
			this->textBoxTol3R->Text = L"1";
			this->textBoxTol3R->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol2R
			// 
			this->textBoxTol2R->Location = System::Drawing::Point(75, 78);
			this->textBoxTol2R->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol2R->MaxLength = 100;
			this->textBoxTol2R->Name = L"textBoxTol2R";
			this->textBoxTol2R->Size = System::Drawing::Size(30, 23);
			this->textBoxTol2R->TabIndex = 5;
			this->textBoxTol2R->Text = L"0";
			this->textBoxTol2R->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol1R
			// 
			this->textBoxTol1R->Location = System::Drawing::Point(42, 78);
			this->textBoxTol1R->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol1R->MaxLength = 100;
			this->textBoxTol1R->Name = L"textBoxTol1R";
			this->textBoxTol1R->Size = System::Drawing::Size(29, 23);
			this->textBoxTol1R->TabIndex = 4;
			this->textBoxTol1R->Text = L"0";
			this->textBoxTol1R->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// textBoxTol0R
			// 
			this->textBoxTol0R->Location = System::Drawing::Point(8, 78);
			this->textBoxTol0R->Margin = System::Windows::Forms::Padding(2);
			this->textBoxTol0R->MaxLength = 100;
			this->textBoxTol0R->Name = L"textBoxTol0R";
			this->textBoxTol0R->Size = System::Drawing::Size(29, 23);
			this->textBoxTol0R->TabIndex = 1;
			this->textBoxTol0R->Text = L"0";
			this->textBoxTol0R->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(4, 22);
			this->label5->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(83, 17);
			this->label5->TabIndex = 0;
			this->label5->Text = L"Tolerances:";
			// 
			// groupBoxSCF
			// 
			this->groupBoxSCF->Controls->Add(this->label_SCF_maxNumPatt);
			this->groupBoxSCF->Controls->Add(this->checkBoxSCF_colourMAP);
			this->groupBoxSCF->Controls->Add(this->checkBoxSCF_stage2tuning);
			this->groupBoxSCF->Controls->Add(this->trackBarPerformVSspeed);
			this->groupBoxSCF->Controls->Add(this->checkBoxSCFVertHoriPrediction);
			this->groupBoxSCF->Controls->Add(this->checkBoxSCF_directional);
			this->groupBoxSCF->Enabled = false;
			this->groupBoxSCF->Location = System::Drawing::Point(940, 208);
			this->groupBoxSCF->Margin = System::Windows::Forms::Padding(2);
			this->groupBoxSCF->Name = L"groupBoxSCF";
			this->groupBoxSCF->Padding = System::Windows::Forms::Padding(2);
			this->groupBoxSCF->Size = System::Drawing::Size(254, 133);
			this->groupBoxSCF->TabIndex = 25;
			this->groupBoxSCF->TabStop = false;
			this->groupBoxSCF->Text = L"SCF";
			// 
			// label_SCF_maxNumPatt
			// 
			this->label_SCF_maxNumPatt->AutoSize = true;
			this->label_SCF_maxNumPatt->Location = System::Drawing::Point(40, 94);
			this->label_SCF_maxNumPatt->Margin = System::Windows::Forms::Padding(2, 0, 2, 0);
			this->label_SCF_maxNumPatt->Name = L"label_SCF_maxNumPatt";
			this->label_SCF_maxNumPatt->Size = System::Drawing::Size(192, 17);
			this->label_SCF_maxNumPatt->TabIndex = 39;
			this->label_SCF_maxNumPatt->Text = L"speed   <----->   performance";
			// 
			// checkBoxSCF_colourMAP
			// 
			this->checkBoxSCF_colourMAP->AutoSize = true;
			this->checkBoxSCF_colourMAP->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxSCF_colourMAP->Location = System::Drawing::Point(142, 41);
			this->checkBoxSCF_colourMAP->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxSCF_colourMAP->Name = L"checkBoxSCF_colourMAP";
			this->checkBoxSCF_colourMAP->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxSCF_colourMAP->Size = System::Drawing::Size(99, 21);
			this->checkBoxSCF_colourMAP->TabIndex = 41;
			this->checkBoxSCF_colourMAP->Text = L"colour MAP";
			this->checkBoxSCF_colourMAP->UseVisualStyleBackColor = true;
			// 
			// checkBoxSCF_stage2tuning
			// 
			this->checkBoxSCF_stage2tuning->AutoSize = true;
			this->checkBoxSCF_stage2tuning->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxSCF_stage2tuning->Location = System::Drawing::Point(2, 41);
			this->checkBoxSCF_stage2tuning->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxSCF_stage2tuning->Name = L"checkBoxSCF_stage2tuning";
			this->checkBoxSCF_stage2tuning->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxSCF_stage2tuning->Size = System::Drawing::Size(117, 21);
			this->checkBoxSCF_stage2tuning->TabIndex = 40;
			this->checkBoxSCF_stage2tuning->Text = L"stage 2 tuning";
			this->checkBoxSCF_stage2tuning->UseVisualStyleBackColor = true;
			// 
			// trackBarPerformVSspeed
			// 
			this->trackBarPerformVSspeed->AccessibleName = L"Performance vs. Speed";
			this->trackBarPerformVSspeed->BackColor = System::Drawing::SystemColors::Control;
			this->trackBarPerformVSspeed->Cursor = System::Windows::Forms::Cursors::SizeWE;
			this->trackBarPerformVSspeed->Location = System::Drawing::Point(42, 67);
			this->trackBarPerformVSspeed->Maximum = 8;
			this->trackBarPerformVSspeed->Name = L"trackBarPerformVSspeed";
			this->trackBarPerformVSspeed->Size = System::Drawing::Size(184, 53);
			this->trackBarPerformVSspeed->TabIndex = 26;
			this->trackBarPerformVSspeed->TickStyle = System::Windows::Forms::TickStyle::None;
			this->trackBarPerformVSspeed->Value = 1;
			// 
			// checkBoxSCFVertHoriPrediction
			// 
			this->checkBoxSCFVertHoriPrediction->AutoSize = true;
			this->checkBoxSCFVertHoriPrediction->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxSCFVertHoriPrediction->Location = System::Drawing::Point(7, 20);
			this->checkBoxSCFVertHoriPrediction->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxSCFVertHoriPrediction->Name = L"checkBoxSCFVertHoriPrediction";
			this->checkBoxSCFVertHoriPrediction->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxSCFVertHoriPrediction->Size = System::Drawing::Size(112, 21);
			this->checkBoxSCFVertHoriPrediction->TabIndex = 39;
			this->checkBoxSCFVertHoriPrediction->Text = L"v/h prediction";
			this->checkBoxSCFVertHoriPrediction->UseVisualStyleBackColor = true;
			// 
			// checkBoxSCF_directional
			// 
			this->checkBoxSCF_directional->AutoSize = true;
			this->checkBoxSCF_directional->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->checkBoxSCF_directional->Location = System::Drawing::Point(149, 20);
			this->checkBoxSCF_directional->Margin = System::Windows::Forms::Padding(2);
			this->checkBoxSCF_directional->Name = L"checkBoxSCF_directional";
			this->checkBoxSCF_directional->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->checkBoxSCF_directional->Size = System::Drawing::Size(92, 21);
			this->checkBoxSCF_directional->TabIndex = 38;
			this->checkBoxSCF_directional->Text = L"directional";
			this->checkBoxSCF_directional->UseVisualStyleBackColor = true;
			// 
			// ConfigForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoSize = true;
			this->ClientSize = System::Drawing::Size(1214, 766);
			this->ControlBox = false;
			this->Controls->Add(this->groupBoxSCF);
			this->Controls->Add(this->groupBoxHXC2params);
			this->Controls->Add(this->groupBoxBPC);
			this->Controls->Add(this->groupBoxCoBaLP2);
			this->Controls->Add(this->groupBoxHXCparams);
			this->Controls->Add(this->groupBoxCompressionMode);
			this->Controls->Add(this->groupBoxIndexing);
			this->Controls->Add(this->groupBoxPaletteSorting);
			this->Controls->Add(this->checkBoxUseZeroRLC2);
			this->Controls->Add(this->groupBoxSeries);
			this->Controls->Add(this->groupBoxLogFile);
			this->Controls->Add(this->checkBoxSkipPostBWT);
			this->Controls->Add(this->groupBoxHistogramModification);
			this->Controls->Add(this->checkBoxSkipRLC);
			this->Controls->Add(this->groupBoxEntropyCoding);
			this->Controls->Add(this->groupBoxFilename);
			this->Controls->Add(this->groupBoxInterleavingMode);
			this->Controls->Add(this->checkBoxSkipPrecoding);
			this->Controls->Add(this->buttonAuto);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->groupBoxPostBWT);
			this->Controls->Add(this->groupBoxPrediction);
			this->Controls->Add(this->groupBoxColourSpace);
			this->Controls->Add(this->groupBoxSegmentation);
			this->Controls->Add(this->buttonAbort);
			this->Controls->Add(this->buttonOK);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Margin = System::Windows::Forms::Padding(2);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->MinimumSize = System::Drawing::Size(800, 30);
			this->Name = L"ConfigForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Compression parameters";
			this->groupBoxPostBWT->ResumeLayout(false);
			this->groupBoxPostBWT->PerformLayout();
			this->groupBoxSegmentation->ResumeLayout(false);
			this->groupBoxSegmentation->PerformLayout();
			this->groupBoxColourSpace->ResumeLayout(false);
			this->groupBoxColourSpace->PerformLayout();
			this->groupBoxIndexing->ResumeLayout(false);
			this->groupBoxIndexing->PerformLayout();
			this->groupBoxPrediction->ResumeLayout(false);
			this->groupBoxPrediction->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->groupBoxFilename->ResumeLayout(false);
			this->groupBoxFilename->PerformLayout();
			this->groupBoxInterleavingMode->ResumeLayout(false);
			this->groupBoxInterleavingMode->PerformLayout();
			this->groupBoxEntropyCoding->ResumeLayout(false);
			this->groupBoxEntropyCoding->PerformLayout();
			this->groupBoxHistogramModification->ResumeLayout(false);
			this->groupBoxHistogramModification->PerformLayout();
			this->groupBoxLogFile->ResumeLayout(false);
			this->groupBoxLogFile->PerformLayout();
			this->groupBoxSeries->ResumeLayout(false);
			this->groupBoxSeries->PerformLayout();
			this->groupBoxPaletteSorting->ResumeLayout(false);
			this->groupBoxPaletteSorting->PerformLayout();
			this->groupBoxCompressionMode->ResumeLayout(false);
			this->groupBoxCompressionMode->PerformLayout();
			this->groupBoxHXCparams->ResumeLayout(false);
			this->groupBoxHXCparams->PerformLayout();
			this->groupBoxCoBaLP2->ResumeLayout(false);
			this->groupBoxCoBaLP2->PerformLayout();
			this->groupBoxBPC->ResumeLayout(false);
			this->groupBoxBPC->PerformLayout();
			this->groupBoxHXC2params->ResumeLayout(false);
			this->groupBoxHXC2params->PerformLayout();
			this->groupBoxSCF->ResumeLayout(false);
			this->groupBoxSCF->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackBarPerformVSspeed))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	/* change text of OK button, if extern prediction is used */
	System::Void radioButtonExtern_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		this->buttonOK->Text = "Next"; 

		if (radioButtonExtern->Checked)
		{
			autoC->predMode = ADAPT;
			InsertRCT( autoC);
		}
		checkBoxUseCorr->Enabled = radioButtonLSpred->Checked;
		checkBoxUseColPred->Enabled = radioButtonLMSpred->Checked;
		checkBoxUseColPredParal->Enabled = radioButtonLMSpred->Checked & checkBoxUseColPred->Enabled;
		textBoxSearchSpace->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		textBoxNumOfPreds->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		e=e; sender=sender;
	}
	System::Void radioButtonH26x_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		this->buttonOK->Text = "Next"; 

		if (radioButtonH26x->Checked)
		{
			autoC->predMode = H26x;
			InsertRCT( autoC);
		}
		checkBoxUseColPred->Enabled = radioButtonLMSpred->Checked;
		checkBoxUseColPredParal->Enabled = radioButtonLMSpred->Checked & checkBoxUseColPred->Enabled;
		checkBoxUseCorr->Enabled = radioButtonLSpred->Checked;
		textBoxSearchSpace->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		textBoxNumOfPreds->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		e=e; sender=sender;	
	}
	System::Void radioButtonLeftNeighbour_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		this->buttonOK->Text = "Compress"; 

		if (radioButtonLeftNeighbour->Checked)
		{
			autoC->predMode = LEFTNEIGHBOUR;
			InsertRCT( autoC);
		}
		checkBoxUseColPred->Enabled = radioButtonLMSpred->Checked;
		checkBoxUseColPredParal->Enabled = radioButtonLMSpred->Checked & checkBoxUseColPred->Enabled;
		checkBoxUseCorr->Enabled = radioButtonLSpred->Checked;
		textBoxSearchSpace->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		textBoxNumOfPreds->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		e=e; sender=sender;
	}
	System::Void radioButtonNoPred_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		this->buttonOK->Text = "Compress"; 

		getSelectedRCT();
		if (radioButtonNoPred->Checked)
		{
			autoC->predMode = NONE;
			if (autoC->rct_type_used != autoC->rct_type_nopr)
			{
				autoC->rct_type_used = autoC->rct_type_nopr;
				InsertRCT( autoC);
			}
		}
		else
		{
			if (autoC->rct_type_used != autoC->rct_type_pred)
			{
				autoC->rct_type_used = autoC->rct_type_pred;
			}
		}
		checkBoxUseColPred->Enabled = radioButtonLMSpred->Checked;
		checkBoxUseColPredParal->Enabled = radioButtonLMSpred->Checked & checkBoxUseColPred->Enabled;
		checkBoxUseCorr->Enabled = radioButtonLSpred->Checked;
		textBoxSearchSpace->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		textBoxNumOfPreds->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		e=e; sender=sender;
	}
	System::Void radioButtonLSpred_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{		
		this->buttonOK->Text = "Compress";	 
			//modif_flag = 1;
		if (radioButtonLSpred->Checked)
		{
			autoC->predMode = LSBLEND;
			InsertRCT( autoC);
		}
		checkBoxUseColPred->Enabled = radioButtonLMSpred->Checked;
		checkBoxUseColPredParal->Enabled = radioButtonLMSpred->Checked & checkBoxUseColPred->Enabled;
		checkBoxUseCorr->Enabled = radioButtonLSpred->Checked;
		textBoxSearchSpace->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		textBoxNumOfPreds->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		e=e; sender=sender;
	}
	System::Void radioButtonLMSpred_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{		
		this->buttonOK->Text = "Compress";	 
			//modif_flag = 1;
		if (radioButtonLMSpred->Checked)
		{
			autoC->predMode = LMSPRED;
			InsertRCT( autoC);
		}
		checkBoxUseColPred->Enabled = radioButtonLMSpred->Checked;
		checkBoxUseColPredParal->Enabled = radioButtonLMSpred->Checked & checkBoxUseColPred->Enabled;
		checkBoxUseCorr->Enabled = !radioButtonLMSpred->Checked;
		textBoxSearchSpace->Enabled = radioButtonLMSpred->Checked | radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		textBoxNumOfPreds->Enabled = radioButtonLMSpred->Checked | radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		e=e; sender=sender;
	}
	System::Void radioButtonLSpredHo_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{		
		this->buttonOK->Text = "Compress";	 
			//modif_flag = 1;
		if (radioButtonLSpredHo->Checked)
		{
			autoC->predMode = LSPRED;
			InsertRCT( autoC);
		}
		checkBoxUseColPred->Enabled = radioButtonLMSpred->Checked;
		checkBoxUseColPredParal->Enabled = radioButtonLMSpred->Checked & checkBoxUseColPred->Enabled;
		checkBoxUseCorr->Enabled = radioButtonLSpred->Checked;
		textBoxSearchSpace->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		textBoxNumOfPreds->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		e=e; sender=sender;
	}
	System::Void radioButtonMED_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{		
		this->buttonOK->Text = "Compress";	 
			//modif_flag = 1;
		if (radioButtonMED->Checked)
		{
			autoC->predMode = MED;
			InsertRCT( autoC);
		}
		checkBoxUseColPred->Enabled = radioButtonLMSpred->Checked;
		checkBoxUseColPredParal->Enabled = radioButtonLMSpred->Checked & checkBoxUseColPred->Enabled;
		checkBoxUseCorr->Enabled = radioButtonLSpred->Checked;
		textBoxSearchSpace->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		textBoxNumOfPreds->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		e=e; sender=sender;
	}

	System::Void radioButtonMEDPAETH_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{	
		this->buttonOK->Text = "Compress"; 
		//modif_flag = 1;
		if (radioButtonMEDPAETH->Checked)
		{
			autoC->predMode = MEDPAETH;
			InsertRCT( autoC);
		}
		checkBoxUseColPred->Enabled = radioButtonLMSpred->Checked;
		checkBoxUseColPredParal->Enabled = radioButtonLMSpred->Checked & checkBoxUseColPred->Enabled;
		checkBoxUseCorr->Enabled = radioButtonLSpred->Checked;
		textBoxSearchSpace->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		textBoxNumOfPreds->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		e=e; sender=sender;
	}

	System::Void radioButtonPaeth_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		this->buttonOK->Text = "Compress"; 
			// modif_flag = 1;
		if (radioButtonPaeth->Checked)
		{
			autoC->predMode = PAETH;
			//InsertParams( curpic, textBoxFilename->Text, imageF,	autoC);
			InsertRCT( autoC);
		}
		checkBoxUseColPred->Enabled = radioButtonLMSpred->Checked;
		checkBoxUseColPredParal->Enabled = radioButtonLMSpred->Checked & checkBoxUseColPred->Enabled;
		checkBoxUseCorr->Enabled = radioButtonLSpred->Checked;
		textBoxSearchSpace->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		textBoxNumOfPreds->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		e=e; sender=sender;
	}
	
	System::Void checkBoxUseColPred_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{		
			// modif_flag = 1;
			e=e; sender=sender;
			checkBoxUseColPredParal->Enabled = checkBoxUseColPred->Checked;
	}
	System::Void checkBoxT4_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{		
			// modif_flag = 1;
			e=e; sender=sender;
	}


	/* proto typing, definition in Config.cpp	*/
	/* Prädiktion ein und ausschalten */
	//Void checkBoxFilter_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	/* Skip precoding */
	Void checkBoxSkipPrecoding_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	/* Skip PostBWT technique */
  Void checkBoxSkipPostBWT_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	/* MTF */
	Void radioButtonMTF_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	/* IFC */
	Void radioButtonIFC_CheckedChanged(System::Object^  sender, System::EventArgs^  e);

	/* Schließen */
	Void buttonCancel_Click(System::Object^  sender, System::EventArgs^  e);
	Void buttonAuto_Click(System::Object^  sender, System::EventArgs^  e);
	Void buttonCompress_Click(System::Object^  sender, System::EventArgs^  e);

	Void ConfigForm::getSelectedRCT( void);

private: 
 System::Void radioButtonGRBSorting_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
 {
		// modif_flag = 1;
		e=e; sender=sender;
 }
 System::Void radioButtonLuminanceSorting_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
 {
		// modif_flag = 1;
		e=e; sender=sender;
 }

 
 System::Void groupBoxColourSpace_Enter(System::Object^  sender, System::EventArgs^  e) {
		// modif_flag = 1;
				 e=e; sender=sender; }
 System::Void groupBoxPrediction_Enter(System::Object^  sender, System::EventArgs^  e) {
		// modif_flag = 1;
				  e=e; sender=sender;}
 System::Void groupBoxInterleavingMode_Enter(System::Object^  sender, System::EventArgs^  e) {
		// modif_flag = 1;
				 e=e; sender=sender; }
 System::Void radioButtonYuvYuvJointly_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		// modif_flag = 1;
				 e=e; sender=sender; }
 System::Void radioButtonYYuvuv_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		// modif_flag = 1;
				  e=e; sender=sender;}
 System::Void radioButtonYUVseparate_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		// modif_flag = 1;
					 e=e; sender=sender;}

System::Void radioButtonHuffmanCoding_CheckedChanged(System::Object^  sender, System::EventArgs^  e){
		// modif_flag = 1;
					 e=e; sender=sender;}
 
	System::Void radioButtonAnyColour_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		/* do not enable groupbox of interleaving if LOCO is checked	*/
		//if (checkBoxLOCO->Checked  != true) 
		//		groupBoxInterleavingMode->Enabled = true;
		if (radioButtonTSIPmode->Checked) 
				groupBoxInterleavingMode->Enabled = true;
		// modif_flag = 1;
		e=e; sender=sender;
	}


System::Void radioButtonHistogramCompaction_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{ e=e; sender=sender;
 	// modif_flag = 1;
}
System::Void radioButtonHistogramPermutation_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{ e=e; sender=sender;
 	// modif_flag = 1;
}
System::Void radioButtonHistogramNoneMod_CheckedChanged( System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;
	// modif_flag = 1;
}

System::Void checkBoxSkipRLC_CheckedChanged( System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;
	// modif_flag = 1;
}
System::Void radioButtonArithmeticCoding_CheckedChanged( System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;
	// modif_flag = 1;
}
 
System::Void radioButtonSeparateCoding_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;
	// modif_flag = 1;
}
System::Void radioButtonJointlyCoding_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;
	// modif_flag = 1;
}


System::Void radioButtonYYuuvv_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	e=e; sender=sender;
	// modif_flag = 1;
}


System::Void checkBoxIndexedColours_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
{
	CheckBox^ ch = (CheckBox^) sender;
	e=e;
	
	/* same ------------------*/
	/* selection of sorting type	*/
	groupBoxPaletteSorting->Enabled = ch->Checked;
	/*  adjacency mapping of indexes	*/
	checkBoxIndexedAdjacency->Enabled = ch->Checked;

	/* opposite */
	groupBoxColourSpace->Enabled = !ch->Checked;
//	groupBoxHistogramModification->Enabled = !ch->Checked;
	radioButtonHistogramCompaction->Enabled = !ch->Checked;

	/* take dependency into account */
	if (ch->Checked) /* indexed colours is checked	*/
	{
		groupBoxInterleavingMode->Enabled = false;
		if(checkBoxIndexedAdjacency->Checked == true)
		{
			//if (checkBoxLOCO->Checked == false)
			if (radioButtonTSIPmode->Checked == true)
			{
				checkBoxIndexedArith->Enabled = true;
			}
		}
	}
	else	/* indexed colours is not checked	*/
	{
		checkBoxIndexedArith->Enabled = false;

		//if (!checkBoxLOCO->Checked)
		if (radioButtonTSIPmode->Checked)
		{
			/* enable interleaving, when LOCO is not checked	*/
			groupBoxInterleavingMode->Enabled = true;
			/* enable precoding	*/
			checkBoxSkipPrecoding->Enabled = true;
			if (!checkBoxSkipPrecoding->Checked)
			{
				checkBoxSkipRLC->Enabled = true;
				checkBoxSkipPostBWT->Enabled = true;
				checkBoxUseZeroRLC2->Enabled = true;
				if (!checkBoxSkipPostBWT->Checked)
				{
					groupBoxPostBWT->Enabled = true;
				}
			}
			groupBoxEntropyCoding->Enabled = true;
			groupBoxSeries->Enabled = true;
		}
	}
	// modif_flag = 1;
}


System::Void checkBoxIndexedAdjacency_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	CheckBox^ ch = (CheckBox^) sender;
	e=e;

	if (ch->Checked)
	{
		//if (checkBoxLOCO->Checked == false)
		if (radioButtonTSIPmode->Checked == true)
		{
			checkBoxIndexedArith->Enabled = true;
		}
	}
	else
	{
		checkBoxIndexedArith->Enabled = false;
		//if (checkBoxLOCO->Checked == false)
		if (radioButtonTSIPmode->Checked == true)
		{
			/* enable precoding	*/
			checkBoxSkipPrecoding->Enabled = true;
			if (checkBoxSkipPrecoding->Checked == false)
			{
				checkBoxSkipRLC->Enabled = true;
				checkBoxSkipPostBWT->Enabled = true;
				checkBoxUseZeroRLC2->Enabled = true;
				if (!checkBoxSkipPostBWT->Checked)
				{
					groupBoxPostBWT->Enabled = true;
				}
			}
			groupBoxEntropyCoding->Enabled = true;
			groupBoxSeries->Enabled = true;
		}
	}
	// modif_flag = 1;
}

System::Void checkBoxIndexedArith_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	CheckBox^ ch = (CheckBox^) sender;
	e=e;
	groupBoxEntropyCoding->Enabled = !ch->Checked;
	groupBoxSeries->Enabled = !ch->Checked;

	checkBoxSkipPrecoding->Enabled = !ch->Checked;

	if (ch->Checked)
	{
		checkBoxSkipRLC->Enabled = false;
		checkBoxUseZeroRLC2->Enabled = false;
		checkBoxSkipPostBWT->Enabled = false;
		groupBoxPostBWT->Enabled = false;
	}
	else
	{
		if (!checkBoxSkipPrecoding->Checked)
		{
			checkBoxSkipPostBWT->Enabled = true;
			checkBoxSkipRLC->Enabled = true;
			checkBoxUseZeroRLC2->Enabled = true;
			if (!checkBoxSkipPostBWT->Checked)
			{
				groupBoxPostBWT->Enabled = true;
			}
		}
	}
}

System::Void checkBoxIndexedArith_EnabledChanged(System::Object^  sender, System::EventArgs^  e) 
{
	CheckBox^ ch = (CheckBox^) sender;
	e=e;
	if (ch->Enabled && ch->Checked )
	{
		groupBoxEntropyCoding->Enabled = false;
		groupBoxSeries->Enabled = false;
		groupBoxPostBWT->Enabled = false;
		checkBoxSkipRLC->Enabled = false;
		checkBoxSkipPrecoding->Enabled = false;
		checkBoxSkipPostBWT->Enabled = false;
		checkBoxUseZeroRLC2->Enabled = false;
	}
}

System::Void checkBoxSkipTM_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;
	// modif_flag = 1;
}
System::Void checkBoxSkipPredA_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;
	// modif_flag = 1;
}

System::Void checkBoxUseZeroRLC2_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;
	// modif_flag = 1;
}

System::Void radioButtonYY_uuvv_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;
}

System::Void labelRCT_UV_Click(System::Object^  sender, System::EventArgs^  e) 
{
	e=e; sender=sender;
}


System::Void radioButtonHXC3_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;
	if (radioButtonTSIPmode->Checked)
	{
		groupBoxEntropyCoding->Enabled = !radioButtonHXC3->Checked;
		groupBoxSeries->Enabled = !radioButtonHXC3->Checked;
		checkBoxUseZeroRLC2->Enabled = !radioButtonHXC3->Checked;

		groupBoxHXC2params->Enabled = radioButtonHXC3->Checked; /* re-use of tolerances	*/
	}
}


System::Void radioButtonTSIPmode_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	/* parameter boxes for TSIP	*/
	groupBoxInterleavingMode->Enabled = radioButtonTSIPmode->Checked;
	groupBoxPaletteSorting->Enabled = radioButtonTSIPmode->Checked;
	//groupBoxSegmentation->Enabled = radioButtonTSIPmode->Checked;
	groupBoxPostBWT->Enabled = radioButtonTSIPmode->Checked;

	groupBoxEntropyCoding->Enabled = radioButtonTSIPmode->Checked && !radioButtonHXC3->Checked;

	groupBoxSeries->Enabled = radioButtonTSIPmode->Checked && !radioButtonHXC3->Checked;

	checkBoxSkipRLC->Enabled = radioButtonTSIPmode->Checked;
	checkBoxSkipPrecoding->Enabled = radioButtonTSIPmode->Checked;
	checkBoxSkipPostBWT->Enabled = radioButtonTSIPmode->Checked;
	checkBoxUseZeroRLC2->Enabled = radioButtonTSIPmode->Checked && !radioButtonHXC3->Checked;

	if (radioButtonTSIPmode->Checked)
	{
		autoC->coding_method = TSIP;	/* required as InsertParams() accesses autoC	*/

		groupBoxPrediction->Enabled = true;

		/* set correct colour space	*/
		if (radioButtonNoPred->Checked)
		{
			if (autoC->rct_type_used != autoC->rct_type_nopr)
			{
				autoC->rct_type_used = autoC->rct_type_nopr;
				InsertRCT( autoC);
			}
		}
		else
		{
			if (autoC->rct_type_used != autoC->rct_type_pred)
			{
				autoC->rct_type_used = autoC->rct_type_pred;
				InsertRCT( autoC);
			}
		}
		/* enable this flag for the proper prediction mode	*/
		checkBoxUseColPred->Enabled = radioButtonLMSpred->Checked;
		checkBoxUseColPredParal->Enabled = radioButtonLMSpred->Checked & checkBoxUseColPred->Enabled;
		checkBoxUseCorr->Enabled = radioButtonLSpred->Checked;
		textBoxSearchSpace->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
		textBoxNumOfPreds->Enabled = radioButtonLSpred->Checked | radioButtonLSpredHo->Checked;
 

		/* enable */
		if (curpic->channel >= 3u) 
		{
			groupBoxIndexing->Enabled = true;
		}
		else
		{
			groupBoxIndexing->Enabled = false;
		}

		/* disable	*/
		checkBoxHXCauto->Checked = false; /* avoid automatic selection of HXC */
		checkBoxHXC2auto->Checked = false; /* avoid automatic selection of HXC */

		if (radioButtonExtern->Checked == true)
		{
			this->buttonOK->Text = "Next";
		}
		else
		{
			this->buttonOK->Text = "Compress";
		}

		/* check dependencies	*/
		if (checkBoxSkipPrecoding->Checked)
		{
			checkBoxSkipRLC->Enabled = false;
			checkBoxSkipPostBWT->Enabled = false;
			groupBoxPostBWT->Enabled = false;
			checkBoxUseZeroRLC2->Enabled = false;
		}
		if (checkBoxSkipPostBWT->Checked)
		{
			groupBoxPostBWT->Enabled = false;
			checkBoxUseZeroRLC2->Enabled = false;
			checkBoxUseZeroRLC2->Checked = false;
		}


		if (groupBoxIndexing->Enabled &&
				checkBoxIndexedColours->Enabled &&
				checkBoxIndexedColours->Checked)
		{
			groupBoxColourSpace->Enabled = false;
			groupBoxInterleavingMode->Enabled = false;
			groupBoxPaletteSorting->Enabled = true;
			checkBoxIndexedAdjacency->Enabled = true;
			if (checkBoxIndexedAdjacency->Checked)
			{
				checkBoxIndexedArith->Enabled = true;
			}
			else
			{
				checkBoxIndexedArith->Enabled = false;
			}
		}
		else
		{
			if (curpic->channel >= 3u) groupBoxColourSpace->Enabled = true;
			groupBoxInterleavingMode->Enabled = true;
			groupBoxPaletteSorting->Enabled = false;
			checkBoxIndexedAdjacency->Enabled = false;
		}

		if (!checkBoxIndexedArith->Checked || !checkBoxIndexedArith->Enabled )
		{
				checkBoxSkipPrecoding->Enabled = true;
				if (checkBoxSkipPrecoding->Checked == false)
				{
					checkBoxSkipRLC->Enabled = true;
					checkBoxSkipPostBWT->Enabled = true;
					checkBoxUseZeroRLC2->Enabled = !radioButtonHXC3->Checked;
					if (!checkBoxSkipPostBWT->Checked)
					{
						groupBoxPostBWT->Enabled = true;
					}
				}
				groupBoxEntropyCoding->Enabled = !radioButtonHXC3->Checked;
				groupBoxSeries->Enabled = !radioButtonHXC3->Checked;
		}
	}
	e=e; sender=sender;
}

System::Void radioButtonLOCOmode_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	checkBoxT4->Enabled = radioButtonLOCOmode->Checked;
	checkBoxImprovedk->Enabled = radioButtonLOCOmode->Checked;

	if (radioButtonLOCOmode->Checked)
	{
		autoC->coding_method = LOCO;	/* required as InsertParams() accesses autoC	*/
		/* enable */
		if (curpic->channel >= 3u) groupBoxIndexing->Enabled = true;
		groupBoxPaletteSorting->Enabled = true;

		getSelectedRCT();
		if (autoC->rct_type_used != autoC->rct_type_pred)
		{
			autoC->rct_type_used = autoC->rct_type_pred;
			InsertRCT( autoC);
		}

		/* disable	*/
		checkBoxHXCauto->Checked = false;
		checkBoxHXC2auto->Checked = false;
		groupBoxPrediction->Enabled = false;

		this->buttonOK->Text = "Compress";
	}

	if (groupBoxIndexing->Enabled && checkBoxIndexedColours->Checked)
	{
		groupBoxColourSpace->Enabled = false;
		groupBoxPaletteSorting->Enabled = true;
		//checkBoxIndexedAdjacency->Enabled = true;
	}
	else
	{
		if (curpic->channel >= 3u) groupBoxColourSpace->Enabled = true;
		groupBoxPaletteSorting->Enabled = false;
	}

	e=e; sender=sender;
}


System::Void radioButtonHXCmode_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	/* parameter box for HXC	*/
	groupBoxHXCparams->Enabled = radioButtonHXCmode->Checked;
	
	if (radioButtonHXCmode->Checked)
	{
		autoC->coding_method = HXC;	/* required as InsertParams() accesses autoC	*/
		groupBoxPrediction->Enabled = true;
		checkBoxHXC2auto->Checked = false;

		getSelectedRCT();
		/* set correct colour space	*/
		if (radioButtonNoPred->Checked)
		{
			if (autoC->rct_type_used != autoC->rct_type_nopr)
			{
				autoC->rct_type_used = autoC->rct_type_nopr;
				InsertRCT( autoC);
			}
		}
		else
		{
			if (autoC->rct_type_used != autoC->rct_type_pred)
			{
				autoC->rct_type_used = autoC->rct_type_pred;
				InsertRCT( autoC);
			}
		}
		/* enable */
		if (curpic->channel >= 3u) groupBoxIndexing->Enabled = true;
		if (groupBoxIndexing->Enabled && checkBoxIndexedColours->Checked)
		{
			groupBoxColourSpace->Enabled = false;
			groupBoxPaletteSorting->Enabled = true;
		}
		else
		{
			if (curpic->channel >= 3u) groupBoxColourSpace->Enabled = true;
			groupBoxPaletteSorting->Enabled = false;
		}
		this->buttonOK->Text = "Compress";
	}
	e=e; sender=sender;
}

System::Void radioButtonHXC2mode_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	/* parameter box for HXC	*/
	groupBoxHXC2params->Enabled = radioButtonHXC2mode->Checked || radioButtonSCFmode->Checked ||
										(radioButtonTSIPmode->Checked && radioButtonHXC3->Checked);
	
	if (radioButtonHXC2mode->Checked)
	{
		autoC->coding_method = HXC2;	/* required as InsertParams() accesses autoC	*/
		groupBoxPrediction->Enabled = true;

		groupBoxIndexing->Enabled = false;
		//groupBoxColourSpace->Enabled = false;
		groupBoxPaletteSorting->Enabled = false;

		getSelectedRCT();
		/* set correct colour space	*/
		if (radioButtonNoPred->Checked)
		{
			if (autoC->rct_type_used != autoC->rct_type_nopr)
			{
				autoC->rct_type_used = autoC->rct_type_nopr;
				InsertRCT( autoC);
			}
		}
		else
		{
			if (autoC->rct_type_used != autoC->rct_type_pred)
			{
				autoC->rct_type_used = autoC->rct_type_pred;
				InsertRCT( autoC);
			}
		}

		checkBoxIndexedColours->Checked = false; /* disable indexing of colours	*/
		this->buttonOK->Text = "Compress";
	}
	e=e; sender=sender;
}

System::Void radioButtonSCFmode_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	/* parameter box for SCF	recycle HXC2 */
	groupBoxHXC2params->Enabled =  radioButtonHXC2mode->Checked || radioButtonSCFmode->Checked || 
										(radioButtonTSIPmode->Checked && radioButtonHXC3->Checked);
	//checkBoxSCF_directional->Enabled = radioButtonSCFmode->Checked;
	groupBoxSCF->Enabled = radioButtonSCFmode->Checked;
	
	if (radioButtonSCFmode->Checked)
	{
		autoC->coding_method = SCF;	/* required as InsertParams() accesses autoC	*/

		groupBoxPrediction->Enabled = true;

		groupBoxIndexing->Enabled = false;
		//groupBoxColourSpace->Enabled = false;
		groupBoxPaletteSorting->Enabled = false;

		getSelectedRCT();
		/* set correct colour space	*/
		if (radioButtonNoPred->Checked)
		{
			if (autoC->rct_type_used != autoC->rct_type_nopr)
			{
				autoC->rct_type_used = autoC->rct_type_nopr;
				InsertRCT( autoC);
			}
		}
		else
		{
			if (autoC->rct_type_used != autoC->rct_type_pred)
			{
				autoC->rct_type_used = autoC->rct_type_pred;
				InsertRCT( autoC);
			}
		}

		checkBoxIndexedColours->Checked = false; /* disable indexing of colours	*/
		this->buttonOK->Text = "Compress";
	}
	e=e; sender=sender;
}

System::Void radioButtonCoBaLP2mode_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	/* box for CoBaLP2	*/
	groupBoxCoBaLP2->Enabled = radioButtonCoBaLP2mode->Checked;
	
	if (radioButtonCoBaLP2mode->Checked)
	{
		autoC->coding_method = CoBaLP2;	/* required as InsertParams() accesses autoC	*/
		/* enable */
		if (curpic->channel >= 3u &&  imageF->numColours <= 1024) 
		{
			/* threshold of 1024 must be verified, avoid allocation error	*/
			groupBoxIndexing->Enabled = true;
		}
		else
		{
			groupBoxIndexing->Enabled = false;
			checkBoxIndexedColours->Checked = false;
		}

		groupBoxPaletteSorting->Enabled = true;

		getSelectedRCT();
		if (autoC->rct_type_used != autoC->rct_type_pred)
		{
			/* CoBaLP has always predcition!	*/
			autoC->rct_type_used = autoC->rct_type_pred;
				InsertRCT( autoC);
		}

		/* disable	*/
		checkBoxHXCauto->Checked = false;
		checkBoxHXC2auto->Checked = false;
		groupBoxPrediction->Enabled = false;

		if (checkBoxIndexedColours->Checked)
		{
			groupBoxColourSpace->Enabled = false;
			checkBoxIndexedAdjacency->Enabled = false;
			checkBoxIndexedArith->Enabled = false;
		}
		else
		{
			if (curpic->channel >= 3u) groupBoxColourSpace->Enabled = true;
			groupBoxPaletteSorting->Enabled = false;
			checkBoxIndexedAdjacency->Enabled = false;
		}

		this->buttonOK->Text = "Compress";
	}


	e=e; sender=sender;
}

System::Void radioButtonBitPlaneMode_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	if (radioButtonBitPlaneMode->Checked)
	{
		autoC->coding_method = BPC;	/* required as InsertParams() accesses autoC	*/
		groupBoxBPC->Enabled = true;
		/* set correct colour space	*/
		if (radioButtonNoPred->Checked)
		{
			if (autoC->rct_type_used != autoC->rct_type_nopr)
			{
				autoC->rct_type_used = autoC->rct_type_nopr;
				InsertRCT( autoC);
			}
		}
		else
		{
			if (autoC->rct_type_used != autoC->rct_type_pred)
			{
				autoC->rct_type_used = autoC->rct_type_pred;
				InsertRCT( autoC);
			}
		}

		/* enable */
		if (curpic->channel >= 3u) 
		{
			groupBoxIndexing->Enabled = true;
			if (checkBoxIndexedColours->Checked) groupBoxPaletteSorting->Enabled = true;
		}
		groupBoxPrediction->Enabled = true;

		/* disable	*/
		checkBoxHXCauto->Checked = false;
		checkBoxHXC2auto->Checked = false;

		if (checkBoxIndexedColours->Checked)
		{
			groupBoxColourSpace->Enabled = false;
			checkBoxIndexedAdjacency->Enabled = false;
			checkBoxIndexedArith->Enabled = false;
		}
		else
		{
			if (curpic->channel >= 3u) groupBoxColourSpace->Enabled = true;
			groupBoxPaletteSorting->Enabled = false;
			checkBoxIndexedAdjacency->Enabled = false;
		}
	}
	else
	{
		groupBoxBPC->Enabled = false;
	}

	if (radioButtonExtern->Checked == true)
	{
		this->buttonOK->Text = "Next";
	}
	else
	{
		this->buttonOK->Text = "Compress";
	}
	e=e; sender=sender;
	// modif_flag = 1;
}


private: System::Void checkBoxHXCauto_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;
	textBoxHXCtolerance->Enabled = !checkBoxHXCauto->Checked;
	textBoxToleranceOffset1->Enabled = !checkBoxHXCauto->Checked;
	textBoxToleranceOffset2->Enabled = !checkBoxHXCauto->Checked;
	textBoxIncreaseFac->Enabled = !checkBoxHXCauto->Checked;
		//	groupBoxHXCparams->Enabled = !checkBoxHXCauto->Checked;

}
private: System::Void checkBoxHXC2auto_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	e=e; sender=sender;

	//textBoxIncreaseFac2->Enabled = !checkBoxHXC2auto->Checked;

	//radioButtonHistogramCompaction->Checked = checkBoxHXC2auto->Checked; /* enable compaction of histograms	*/
	//radioButtonHistogramNoneMod->Checked = !checkBoxHXC2auto->Checked; /* enable compaction of histograms	*/

	textBoxTol0R->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol1R->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol2R->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol3R->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol4R->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol5R->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol0G->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol1G->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol2G->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol3G->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol4G->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol5G->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol0B->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol1B->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol2B->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol3B->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol4B->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol5B->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol0A->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol1A->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol2A->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol3A->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol4A->Enabled = !checkBoxHXC2auto->Checked;
	textBoxTol5A->Enabled = !checkBoxHXC2auto->Checked;

	textBoxIncreaseFac2->Enabled = !checkBoxHXC2auto->Checked;
}
private: System::Void textBoxTileWidth_TextChanged(System::Object^  sender, System::EventArgs^  e) 
{
	e=e; sender=sender;
}


//private: System::Void textBox3_TextChanged(System::Object^  sender, System::EventArgs^  e) {
//				 }
//private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
//				 }
//private: System::Void textBoxTol2_TextChanged(System::Object^  sender, System::EventArgs^  e) {
//				 }
//private: System::Void textBoxTol4_TextChanged(System::Object^  sender, System::EventArgs^  e) {
//				 }
//private: System::Void textBox2_TextChanged(System::Object^  sender, System::EventArgs^  e) {
//				 }
//private: System::Void textBox10_TextChanged(System::Object^  sender, System::EventArgs^  e) {
//				 }
//private: System::Void textBox5_TextChanged(System::Object^  sender, System::EventArgs^  e) {
//				 }

//private: System::Void pictureBox1_Click(System::Object^  sender, System::EventArgs^  e) {
//				 }
// private: System::Void label12_Click(System::Object^  sender, System::EventArgs^  e) {
//					}
private: System::Void textBox_RCTpercentage_TextChanged(System::Object^  sender, System::EventArgs^  e) 
{
	PicData *pd;
	IMAGEc img, *im;

	im = &img;
	pd = curpic;
	e=e; sender=sender;
	autoC->RCTpercentage = (unsigned char)Convert::ToInt32(textBox_RCTpercentage->Text);
	if (autoC->RCTpercentage < 1 || autoC->RCTpercentage > 100)
	{
		/* do nothing, when value is wrong	*/
		return;
	}
	if (pd->channel >= 3)
	{
		double entropy[4];
		unsigned int Yi, UVj;
		unsigned long set_size;

		set_size = (pd->size * autoC->RCTpercentage) / 100;
		/* determine best colour space based on entropy */
		im->height = pd->height;
		im->width = pd->width;
		im->size = pd->size;
		im->bit_depth[0] = pd->bitperchan[0];
		im->bit_depth[1] = pd->bitperchan[1];
		im->bit_depth[2] = pd->bitperchan[2];
		im->bit_depth_max = pd->bitperchannel;

#ifdef RCT_VARIANCE_CRITERION
		if (autoC->predMode != NONE || autoC->coding_method == LOCO || 
			autoC->coding_method == CoBaLP2 )
		{
			autoC->rct_type_pred = find_colourspace5( im, pd->data[0], 
				pd->data[1], pd->data[2], set_size, &Yi, &UVj,
				entropy,
				0, im->height,
				0, im->width, 1); /* with prediction	*/
		}
		else
		{
			autoC->rct_type_nopr = find_colourspace5( im, pd->data[0], 
					pd->data[1], pd->data[2], set_size, &Yi, &UVj,
					entropy,
					0, im->height,
					0, im->width, 0); /* without prediction	*/
		}
#else
		if (autoC->predMode != NONE || autoC->coding_method == LOCO || 
			autoC->coding_method == CoBaLP2 )
		{
			autoC->rct_type_pred = find_colourspace4( im, pd->data[0], 
				pd->data[1], pd->data[2], set_size, &Yi, &UVj,
				entropy,
				0, im->height,
				0, im->width, 1); /* with prediction	*/
		}
		else
		{
			autoC->rct_type_nopr = find_colourspace4( im, pd->data[0], 
					pd->data[1], pd->data[2], set_size, &Yi, &UVj,
					entropy,
					0, im->height,
					0, im->width, 0); /* without prediction	*/
		}
#endif
	}
	InsertParams( pd, textBoxFilename->Text, imageF, autoC, 0/* should be numOfTiles*/);
}
};
}
