/*****************************************************
 * File..: ExternalPrediction.c
 * Desc..: parameter-selection dialog and functions
 * Author: T. Strutz
 * Date..: 09.06.2011
 ******************************************************/
#include "stdafx.h"
#include "ExternalPrediction.h"

using namespace bacoder;

/* Mean Mode */
void ExternalPrediction::radioButtonMean_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	e=e;
	bool check = ((RadioButton^)sender)->Checked;
	groupBox2->Enabled = !check;
	groupBoxContextParams->Enabled = check;
	groupBoxBlockSize->Enabled = !check;
}

/* Block Mode */
void ExternalPrediction::radioButtonBlock_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	bool check = ((RadioButton^)sender)->Checked;
	e=e;
	groupBox2->Enabled = !check;
	groupBoxContextParams->Enabled = !check;
	groupBoxBlockSize->Enabled = check;
}

/* Context Mode */
void ExternalPrediction::radioButtonContext_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	bool check = ((RadioButton^)sender)->Checked;
	e=e;
	
	// Blocks the mode selection and therefore removed
	// groupBoxMode->Enabled = check;
	groupBoxContextParams->Enabled = check;
	groupBoxBlockSize->Enabled = !check;
}

/* Context aus Datei */
void ExternalPrediction::checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
{
	buttonCtx->Enabled = ((CheckBox^)sender)->Checked;
	e=e;
}

/* Abbrechen */
void ExternalPrediction::buttonAbort_Click(System::Object^  sender, System::EventArgs^  e) 
{
	result = false;
	this->Close();
	e=e; sender=sender;
}

/* Weiter */
void ExternalPrediction::buttonOK_Click(System::Object^  sender, System::EventArgs^  e)
{
	uint tmp;

	e=e; sender=sender;
	/* Werte prüfen */
	if (radioButtonBlock->Checked)
	{
		try
		{
			tmp = Convert::ToInt32(textBoxWidth->Text);
			if (tmp < 1 || tmp > 256) throw 0; /* must fit a single byte	*/
		}
		catch (...)
		{
			MessageBox::Show("0 < block width <= 256", "Error", 
							 MessageBoxButtons::OK, MessageBoxIcon::Information);
			return;
		}			
		try
		{
			tmp = Convert::ToInt32(textBoxHeight->Text);
			if (tmp < 1 || tmp > 256) throw 0;
		}
		catch (...)
		{
			MessageBox::Show("0 < block height <= 256", "Error", 
							 MessageBoxButtons::OK, MessageBoxIcon::Information);
			return;
		}
	}

	if (radioButtonMean->Checked || radioButtonContext->Checked)
	{
		try
		{
			tmp = Convert::ToInt32(textBoxPX->Text);
			if (tmp < 0 || tmp > 10) throw 0;
		}
		catch (...)
		{
			MessageBox::Show("cxPx ungültig.", "Fehler", 
							 MessageBoxButtons::OK, MessageBoxIcon::Information);
			return;
		}			
		try
		{
			tmp = Convert::ToInt32(textBoxGrad->Text);
			if (tmp < 0 || tmp > 9) throw 0;
		}
		catch (...)
		{
			MessageBox::Show("cxGrad ungültig.", "Fehler", 
							 MessageBoxButtons::OK, MessageBoxIcon::Information);
			return;
		}
	}

	if (checkBoxCtx->Checked && textBox1->Text == "")
	{
		MessageBox::Show("Bitte Context-Datei angeben.", "Fehler", 
						 MessageBoxButtons::OK, MessageBoxIcon::Information);
		return;
	}

	pred[0] = checkBoxMED->Checked;
	pred[1] = checkBoxPaeth->Checked;
	pred[2] = checkBoxLinear->Checked;
	pred[3] = checkBoxGAP->Checked;
	pred[4] = checkBoxCobalp->Checked;
	pred[5] = checkBoxTM->Checked;
	
	bias[0] = checkBoxMEDB->Checked;
	bias[1] = checkBoxPaethB->Checked;
	bias[2] = checkBoxLinB->Checked;
	bias[3] = checkBoxGAPB->Checked;
	bias[4] = checkBoxCobalpB->Checked;
	bias[5] = checkBoxTMB->Checked;
	
	cxGrad = Convert::ToInt32(textBoxGrad->Text);
	cxPx   = Convert::ToInt32(textBoxPX->Text);
	bw     = Convert::ToInt32(textBoxWidth->Text);
	bh	   = Convert::ToInt32(textBoxHeight->Text);

	if (textBoxSigma->Enabled)
	{
		try
		{					
			tmp = Convert::ToInt32(textBoxSigma->Text);
			if (tmp < 0 || tmp > 20) throw 0;
			sigma = (uint)tmp;
		}
		catch (...)
		{
			MessageBox::Show("TM Sigma ungültig.", "Fehler", 
							 MessageBoxButtons::OK, MessageBoxIcon::Information);
			return;
		}
	}
	else
	{
		sigma = 0;
	}

	if (checkBoxCtx->Checked)
	{
		isctxfile = true;
		ctxfile   = textBox1->Text;
	}
	else
	{
		isctxfile = false;
	}

	if (radioButtonBlock->Checked)
	{
		blockmode = true;
		meanmode  = false;
	}
	if (radioButtonMean->Checked)
	{
		blockmode = false;
		meanmode  = true;
	}
	if (radioButtonContext->Checked)
	{
		blockmode = false;
		meanmode  = false;
	}

	result = true;
	this->Close();
}

/* Context Datei laden */
void ExternalPrediction::buttonCtx_Click(System::Object^  sender, System::EventArgs^  e)
{
	openFile->ShowDialog();
	textBox1->Text = openFile->FileName;
	textBox1->SelectionLength = textBox1->Text->Length;
	e=e; sender=sender;
}

/* Bias */
void ExternalPrediction::checkBoxTMB_CheckStateChanged(System::Object^  sender, System::EventArgs^  e)
{
	((CheckBox^)sender)->Text = ((CheckBox^)sender)->Checked ? "On" : "Off";
	e=e; 
}

/* Opt. Blockmode */
void ExternalPrediction::buttonPreset_Click(System::Object^  sender, System::EventArgs^  e)
{
	//char text[99];
	radioButtonBlock->Checked = true;

	//sprintf( text, "%d", curpic->width/30)
	//textBoxWidth->Text		= text;
	textBoxWidth->Text		= "24";
	textBoxHeight->Text		= "24";
	
	checkBoxLinear->Checked = true;
	checkBoxMED->Checked	= true;
	checkBoxGAP->Checked	= true;
	checkBoxPaeth->Checked	= true;
	checkBoxTM->Checked		= false;
	checkBoxCobalp->Checked = true;
	
	checkBoxLinB->Checked	 = false;
	checkBoxMEDB->Checked	 = false;			
	checkBoxPaethB->Checked  = false;
	checkBoxGAPB->Checked	 = false;
	checkBoxTMB->Checked	 = false;
	checkBoxCobalpB->Checked = false;
	e=e; sender=sender;
}

/* Opt. Contextmode */
void ExternalPrediction::buttonPresetContext_Click(System::Object^  sender, System::EventArgs^  e)
{
	radioButtonContext->Checked = true;
	textBoxPX->Text			= "2";
	textBoxGrad->Text		= "3";
	
	checkBoxLinear->Checked = true;
	checkBoxMED->Checked	= true;
	checkBoxGAP->Checked	= true;
	checkBoxPaeth->Checked	= true;
	checkBoxTM->Checked		= true;
	checkBoxCobalp->Checked = true;
	
	checkBoxLinB->Checked	 = false;
	checkBoxMEDB->Checked	 = false;			
	checkBoxPaethB->Checked  = false;
	checkBoxGAPB->Checked	 = false;
	checkBoxTMB->Checked	 = false;
	checkBoxCobalpB->Checked = false;
	e=e; sender=sender;
}

/* Only TM needs sigma enabled */
void ExternalPrediction::checkBoxTM_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
{
	textBoxSigma->Enabled = ((CheckBox ^)sender)->Checked;
	e=e; 
}

