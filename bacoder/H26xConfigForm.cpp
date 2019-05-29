/*****************************************************
* File..: H26xConfigForm.c
* Desc..: GUI of the H26x Predictor
* Author: Michael Stegemann
* Date..: 12.11.2012
* 29.11.2012 Strutz: bugfix, if all block sizes are disabled
******************************************************/
#include "stdafx.h"
#include "H26xConfigForm.h"
#include "autoConfig.h"

using namespace bacoder;

Void H26xConfigForm::initConfiguration(H26xConfiguration* config)
{
	this->Config = config;
	setConfiguration();
}

Void H26xConfigForm::setConfiguration()
{
	for (int i = 0; i < NUM_INTRA_MODE; i++)
	{
		optPredictors->
			SetItemChecked(i,(Config->availablePredictors[i] == TRUE));
	}

	for (int i = 0; i < NUM_BLOCK_SIZES; i++)
	{
		optBlocksizes ->
			SetItemChecked(i,(Config->availableBlocks[i] == TRUE));
	}

	checkBox_PredLog ->Checked = (Config->logPredictors == TRUE);
	checkBox_BlockLog->Checked = (Config->logBlocks == TRUE);
	checkBox_TrafoLog->Checked = (Config->logTransformations == TRUE);

	checkBox_errPic  ->Checked = (Config->printErrPic == TRUE);

	switch(Config->postTransform)
	{
		case 0: radioNoTransform->Checked = true; break;
		case 1: radio53sep->Checked		  = true; break;
		case 2:	radio53noSep->Checked	  = true; break;
		default: radioNoTransform->Checked = true;
	
	}	
	text_MSEthres->Text = Config->mse_thresh.ToString();
}

Void H26xConfigForm::getConfiguration(H26xConfiguration* config)
{
	int flag = 0;
	for (int i = 0; i < NUM_INTRA_MODE; i++)
	{
		config->availablePredictors[i] =
			optPredictors->GetItemChecked(i);
	}

	for (int i = 0; i < NUM_BLOCK_SIZES; i++)
	{
		config->availableBlocks[i] =
			optBlocksizes->GetItemChecked(i);
		// Strutz: in case that all sizes were disabled
		if (config->availableBlocks[i]) flag = 1;
	}
	if (!flag)
	{
		/* enable largest size, however, noe feedback to user is given	*/
		config->availableBlocks[NUM_BLOCK_SIZES-1] = 1;
	}

	config->logPredictors = checkBox_PredLog->Checked;
	config->logBlocks = checkBox_BlockLog->Checked;
	config->logTransformations = checkBox_TrafoLog->Checked;

	config->printErrPic = checkBox_errPic->Checked;

	if (radioNoTransform->Checked == true)
	{
		config->postTransform = 0;
	}
	if (radio53sep->Checked == true)
	{
		config->postTransform = 1;
	}
	if (radio53noSep->Checked == true)
	{
		config->postTransform = 2;
	}	

	// ToDo: Create a UI element for the value
	config->mse_thresh = System::Double::Parse(text_MSEthres->Text);
}