/************************************************************
 * File...: ExternalPrediction.h
 * Author.: Strutz
 * Date...: 9.6.2011
 ************************************************************/
#include "stdafx.h"
#include "glue.h"

#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Runtime::InteropServices;

// Strutz #include "externpred\prediction.h"
#include "prediction.h"


namespace bacoder {

	public ref class ExternalPrediction : public System::Windows::Forms::Form
	{
	public:

		bool result;	 
		bool *pred, *bias;
		uint bw, bh, cxPx, cxGrad;
		String^ ctxfile;
		bool blockmode, meanmode, isctxfile;
		uint sigma;
		
		ExternalPrediction(void)
		{
			InitializeComponent();	
			pred = new bool[6];
			bias = new bool[6];

		}

		/* presets */
		void InsertParams( CustomParam *cp)
		{
			textBoxWidth->Text = cp->predparam->blockWidth.ToString();
			textBoxHeight->Text = cp->predparam->blockHeight.ToString();
			textBoxSigma->Text = cp->predparam->tmSigma.ToString();
			textBoxGrad->Text = cp->predparam->cxGrad.ToString();
			textBoxPX->Text = cp->predparam->cxPx.ToString();
			if (cp->predparam->predictors[0]) checkBoxMED->Checked = true;
			else checkBoxMED->Checked = false;
			if (cp->predparam->predictors[1]) checkBoxPaeth->Checked = true;
			else checkBoxPaeth->Checked = false;
			if (cp->predparam->predictors[2]) checkBoxLinear->Checked = true;
			else checkBoxLinear->Checked = false;
			if (cp->predparam->predictors[3]) checkBoxGAP->Checked = true;
			else checkBoxGAP->Checked = false;
			if (cp->predparam->predictors[4]) checkBoxCobalp->Checked = true;
			else checkBoxCobalp->Checked = false;
			if (cp->predparam->predictors[5]) checkBoxTM->Checked = true;
			else checkBoxTM->Checked = false;

			if (cp->predparam->bias[0]) checkBoxMEDB->Checked = true;
			else		checkBoxMEDB->Checked = false;
			if (cp->predparam->bias[1]) checkBoxPaethB->Checked = true;
			else		checkBoxPaethB->Checked = false;
			if (cp->predparam->bias[2]) checkBoxLinB->Checked = true;
			else		checkBoxLinB->Checked = false;
			if (cp->predparam->bias[3]) checkBoxGAPB->Checked = true;
			else		checkBoxGAPB->Checked = false;
			if (cp->predparam->bias[4]) checkBoxCobalpB->Checked = true;
			else		 checkBoxCobalpB->Checked = false;
			if (cp->predparam->bias[5]) checkBoxTMB->Checked = true;
			else		checkBoxTMB->Checked = false;
			if (cp->predparam->blockMode == 1) radioButtonBlock->Checked = true;
			else if (cp->predparam->wMeanMode == 1) radioButtonMean->Checked = true;
		}
	

	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~ExternalPrediction()
		{
			if (components)
			{
				delete components;
			}
			delete [] bias;
			delete [] pred;
		}
	public: 

		static void ExternalPrediction::GetExternParams( CustomParam *cp)
		{
			ExternalPrediction^ extpred; // Prädiktionsparameter

			extpred = gcnew ExternalPrediction();

			extpred->InsertParams( cp);
			extpred->ShowDialog();
			if (extpred->result == false) 
			{
				cp->predparam = NULL;
				//return cp;
				return;
			}

			/* Parameter zusammenbauen 
			 * extpred->Values are set in	ExternalPrediction::buttonOK_Click(), ExternalPrediction.cpp
			 */
			//  extern newParameter( &(cp->predparam), 6u);
			cp->predparam->blockHeight = extpred->bh;
			cp->predparam->blockWidth  = extpred->bw;
			cp->predparam->cxGrad = extpred->cxGrad;
			cp->predparam->cxPx   = extpred->cxPx;
			cp->predparam->tmSigma = (float)extpred->sigma;
			cp->predparam->blockMode = extpred->blockmode ? TRUE : FALSE;
			cp->predparam->wMeanMode = extpred->meanmode  ? TRUE : FALSE;
			cp->usextfile = extpred->isctxfile;
			cp->ctxfile = extpred->isctxfile ? (char *)(void *)Marshal::StringToHGlobalAnsi(extpred->ctxfile) : NULL;
			for (int i = 0; i < 6; i++)
			{
				cp->predparam->predictors[i] = extpred->pred[i] ? TRUE : FALSE;
				cp->predparam->bias[i]       = extpred->bias[i] ? TRUE : FALSE;
			}
			
			//return cp;
			return;
		};

	private: System::Windows::Forms::RadioButton^  radioButtonContext;

	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::TextBox^  textBoxSigma;


	private: System::Windows::Forms::RadioButton^  radioButtonBlock;
	private: System::Windows::Forms::Button^  buttonOK;
	private: System::Windows::Forms::Button^  buttonAbort;

	private: System::Windows::Forms::OpenFileDialog^  openFile;

	private: System::Windows::Forms::Button^  buttonCtx;
	private: System::Windows::Forms::TextBox^  textBox1;

	private: System::Windows::Forms::RadioButton^  radioButtonMean;
	private: System::Windows::Forms::GroupBox^  groupBoxMode;

	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::CheckBox^  checkBoxCtx;
	private: System::Windows::Forms::Button^  buttonPresetBlock;
	private: System::Windows::Forms::Button^  buttonPresetContext;
	private: System::Windows::Forms::GroupBox^  groupBoxContextParams;


	private: System::Windows::Forms::TextBox^  textBoxGrad;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  textBoxPX;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::GroupBox^  groupBoxBlockSize;

	private: System::Windows::Forms::TextBox^  textBoxHeight;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  textBoxWidth;

	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::GroupBox^  groupBoxPredictors;
	private: System::Windows::Forms::Label^  label5;

	private: System::Windows::Forms::CheckBox^  checkBoxMED;
	private: System::Windows::Forms::CheckBox^  checkBoxGAP;
	private: System::Windows::Forms::CheckBox^  checkBoxLinear;
	private: System::Windows::Forms::CheckBox^  checkBoxPaeth;
	private: System::Windows::Forms::CheckBox^  checkBoxTM;
	private: System::Windows::Forms::CheckBox^  checkBoxCobalp;

	private: System::Windows::Forms::CheckBox^  checkBoxCobalpB;
	private: System::Windows::Forms::CheckBox^  checkBoxGAPB;
	private: System::Windows::Forms::CheckBox^  checkBoxLinB;
	private: System::Windows::Forms::CheckBox^  checkBoxPaethB;
	private: System::Windows::Forms::CheckBox^  checkBoxMEDB;
	private: System::Windows::Forms::CheckBox^  checkBoxTMB;
	private: System::Windows::Forms::Label^  label6;

	
	private:
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Erforderliche Methode für die Designerunterstützung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(ExternalPrediction::typeid));
			this->radioButtonContext = (gcnew System::Windows::Forms::RadioButton());
			this->radioButtonBlock = (gcnew System::Windows::Forms::RadioButton());
			this->buttonOK = (gcnew System::Windows::Forms::Button());
			this->buttonAbort = (gcnew System::Windows::Forms::Button());
			this->openFile = (gcnew System::Windows::Forms::OpenFileDialog());
			this->buttonCtx = (gcnew System::Windows::Forms::Button());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->radioButtonMean = (gcnew System::Windows::Forms::RadioButton());
			this->groupBoxMode = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBoxCtx = (gcnew System::Windows::Forms::CheckBox());
			this->groupBoxContextParams = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxGrad = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->textBoxPX = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBoxBlockSize = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxHeight = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->textBoxWidth = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->groupBoxPredictors = (gcnew System::Windows::Forms::GroupBox());
			this->textBoxSigma = (gcnew System::Windows::Forms::TextBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->checkBoxTMB = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxCobalpB = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxGAPB = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxLinB = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxPaethB = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxMEDB = (gcnew System::Windows::Forms::CheckBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->checkBoxTM = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxCobalp = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxGAP = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxLinear = (gcnew System::Windows::Forms::CheckBox());
			this->checkBoxPaeth = (gcnew System::Windows::Forms::CheckBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->checkBoxMED = (gcnew System::Windows::Forms::CheckBox());
			this->buttonPresetBlock = (gcnew System::Windows::Forms::Button());
			this->buttonPresetContext = (gcnew System::Windows::Forms::Button());
			this->groupBoxMode->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBoxContextParams->SuspendLayout();
			this->groupBoxBlockSize->SuspendLayout();
			this->groupBoxPredictors->SuspendLayout();
			this->SuspendLayout();
			// 
			// radioButtonContext
			// 
			this->radioButtonContext->AutoSize = true;
			this->radioButtonContext->Enabled = false;
			this->radioButtonContext->Location = System::Drawing::Point(293, 23);
			this->radioButtonContext->Margin = System::Windows::Forms::Padding(4);
			this->radioButtonContext->Name = L"radioButtonContext";
			this->radioButtonContext->Size = System::Drawing::Size(112, 21);
			this->radioButtonContext->TabIndex = 1;
			this->radioButtonContext->Text = L"Context Mode";
			this->radioButtonContext->UseVisualStyleBackColor = true;
			this->radioButtonContext->CheckedChanged += gcnew System::EventHandler(this, &ExternalPrediction::radioButtonContext_CheckedChanged);
			// 
			// radioButtonBlock
			// 
			this->radioButtonBlock->AutoSize = true;
			this->radioButtonBlock->Checked = true;
			this->radioButtonBlock->Location = System::Drawing::Point(164, 23);
			this->radioButtonBlock->Margin = System::Windows::Forms::Padding(4);
			this->radioButtonBlock->Name = L"radioButtonBlock";
			this->radioButtonBlock->Size = System::Drawing::Size(99, 21);
			this->radioButtonBlock->TabIndex = 3;
			this->radioButtonBlock->TabStop = true;
			this->radioButtonBlock->Text = L"Block Mode";
			this->radioButtonBlock->UseVisualStyleBackColor = true;
			this->radioButtonBlock->CheckedChanged += gcnew System::EventHandler(this, &ExternalPrediction::radioButtonBlock_CheckedChanged);
			// 
			// buttonOK
			// 
			this->buttonOK->Location = System::Drawing::Point(361, 474);
			this->buttonOK->Margin = System::Windows::Forms::Padding(4);
			this->buttonOK->Name = L"buttonOK";
			this->buttonOK->Size = System::Drawing::Size(100, 28);
			this->buttonOK->TabIndex = 5;
			this->buttonOK->Text = L"Compress";
			this->buttonOK->UseVisualStyleBackColor = true;
			this->buttonOK->Click += gcnew System::EventHandler(this, &ExternalPrediction::buttonOK_Click);
			// 
			// buttonAbort
			// 
			this->buttonAbort->Location = System::Drawing::Point(253, 474);
			this->buttonAbort->Margin = System::Windows::Forms::Padding(4);
			this->buttonAbort->Name = L"buttonAbort";
			this->buttonAbort->Size = System::Drawing::Size(100, 28);
			this->buttonAbort->TabIndex = 6;
			this->buttonAbort->Text = L"Cancel";
			this->buttonAbort->UseVisualStyleBackColor = true;
			this->buttonAbort->Click += gcnew System::EventHandler(this, &ExternalPrediction::buttonAbort_Click);
			// 
			// openFile
			// 
			this->openFile->Filter = L"Context Files *.ctx|*.ctx";
			// 
			// buttonCtx
			// 
			this->buttonCtx->Enabled = false;
			this->buttonCtx->Location = System::Drawing::Point(396, 50);
			this->buttonCtx->Margin = System::Windows::Forms::Padding(4);
			this->buttonCtx->Name = L"buttonCtx";
			this->buttonCtx->Size = System::Drawing::Size(43, 25);
			this->buttonCtx->TabIndex = 8;
			this->buttonCtx->Text = L"[...]";
			this->buttonCtx->UseVisualStyleBackColor = true;
			this->buttonCtx->Click += gcnew System::EventHandler(this, &ExternalPrediction::buttonCtx_Click);
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(8, 52);
			this->textBox1->Margin = System::Windows::Forms::Padding(4);
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->Size = System::Drawing::Size(379, 22);
			this->textBox1->TabIndex = 1;
			// 
			// radioButtonMean
			// 
			this->radioButtonMean->AutoSize = true;
			this->radioButtonMean->Enabled = false;
			this->radioButtonMean->Location = System::Drawing::Point(39, 23);
			this->radioButtonMean->Margin = System::Windows::Forms::Padding(4);
			this->radioButtonMean->Name = L"radioButtonMean";
			this->radioButtonMean->Size = System::Drawing::Size(100, 21);
			this->radioButtonMean->TabIndex = 7;
			this->radioButtonMean->Text = L"Mean Mode";
			this->radioButtonMean->UseVisualStyleBackColor = true;
			this->radioButtonMean->CheckedChanged += gcnew System::EventHandler(this, &ExternalPrediction::radioButtonMean_CheckedChanged);
			// 
			// groupBoxMode
			// 
			this->groupBoxMode->Controls->Add(this->radioButtonBlock);
			this->groupBoxMode->Controls->Add(this->radioButtonMean);
			this->groupBoxMode->Controls->Add(this->radioButtonContext);
			this->groupBoxMode->Location = System::Drawing::Point(16, 15);
			this->groupBoxMode->Margin = System::Windows::Forms::Padding(4);
			this->groupBoxMode->Name = L"groupBoxMode";
			this->groupBoxMode->Padding = System::Windows::Forms::Padding(4);
			this->groupBoxMode->Size = System::Drawing::Size(444, 50);
			this->groupBoxMode->TabIndex = 8;
			this->groupBoxMode->TabStop = false;
			this->groupBoxMode->Text = L"Mode";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->checkBoxCtx);
			this->groupBox2->Controls->Add(this->buttonCtx);
			this->groupBox2->Controls->Add(this->textBox1);
			this->groupBox2->Enabled = false;
			this->groupBox2->Location = System::Drawing::Point(16, 73);
			this->groupBox2->Margin = System::Windows::Forms::Padding(4);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Padding = System::Windows::Forms::Padding(4);
			this->groupBox2->Size = System::Drawing::Size(447, 82);
			this->groupBox2->TabIndex = 9;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Context Mode";
			// 
			// checkBoxCtx
			// 
			this->checkBoxCtx->AutoSize = true;
			this->checkBoxCtx->Location = System::Drawing::Point(8, 23);
			this->checkBoxCtx->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxCtx->Name = L"checkBoxCtx";
			this->checkBoxCtx->Size = System::Drawing::Size(164, 21);
			this->checkBoxCtx->TabIndex = 9;
			this->checkBoxCtx->Text = L"Context data from file:";
			this->checkBoxCtx->UseVisualStyleBackColor = true;
			this->checkBoxCtx->CheckedChanged += gcnew System::EventHandler(this, &ExternalPrediction::checkBox1_CheckedChanged);
			// 
			// groupBoxContextParams
			// 
			this->groupBoxContextParams->Controls->Add(this->textBoxGrad);
			this->groupBoxContextParams->Controls->Add(this->label2);
			this->groupBoxContextParams->Controls->Add(this->textBoxPX);
			this->groupBoxContextParams->Controls->Add(this->label1);
			this->groupBoxContextParams->Enabled = false;
			this->groupBoxContextParams->Location = System::Drawing::Point(16, 156);
			this->groupBoxContextParams->Margin = System::Windows::Forms::Padding(4);
			this->groupBoxContextParams->Name = L"groupBoxContextParams";
			this->groupBoxContextParams->Padding = System::Windows::Forms::Padding(4);
			this->groupBoxContextParams->Size = System::Drawing::Size(447, 56);
			this->groupBoxContextParams->TabIndex = 10;
			this->groupBoxContextParams->TabStop = false;
			this->groupBoxContextParams->Text = L"Context Mode / Mean Mode";
			// 
			// textBoxGrad
			// 
			this->textBoxGrad->Location = System::Drawing::Point(337, 23);
			this->textBoxGrad->Margin = System::Windows::Forms::Padding(4);
			this->textBoxGrad->Name = L"textBoxGrad";
			this->textBoxGrad->Size = System::Drawing::Size(76, 22);
			this->textBoxGrad->TabIndex = 3;
			this->textBoxGrad->Text = L"4";
			this->textBoxGrad->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(239, 27);
			this->label2->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(92, 17);
			this->label2->TabIndex = 2;
			this->label2->Text = L"cxGrad (0-9):";
			// 
			// textBoxPX
			// 
			this->textBoxPX->Location = System::Drawing::Point(127, 23);
			this->textBoxPX->Margin = System::Windows::Forms::Padding(4);
			this->textBoxPX->Name = L"textBoxPX";
			this->textBoxPX->Size = System::Drawing::Size(76, 22);
			this->textBoxPX->TabIndex = 1;
			this->textBoxPX->Text = L"2";
			this->textBoxPX->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(35, 27);
			this->label1->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(83, 17);
			this->label1->TabIndex = 0;
			this->label1->Text = L"cxPx (0-10):";
			// 
			// groupBoxBlockSize
			// 
			this->groupBoxBlockSize->Controls->Add(this->textBoxHeight);
			this->groupBoxBlockSize->Controls->Add(this->label3);
			this->groupBoxBlockSize->Controls->Add(this->textBoxWidth);
			this->groupBoxBlockSize->Controls->Add(this->label4);
			this->groupBoxBlockSize->Location = System::Drawing::Point(16, 220);
			this->groupBoxBlockSize->Margin = System::Windows::Forms::Padding(4);
			this->groupBoxBlockSize->Name = L"groupBoxBlockSize";
			this->groupBoxBlockSize->Padding = System::Windows::Forms::Padding(4);
			this->groupBoxBlockSize->Size = System::Drawing::Size(447, 54);
			this->groupBoxBlockSize->TabIndex = 11;
			this->groupBoxBlockSize->TabStop = false;
			this->groupBoxBlockSize->Text = L"Block Mode";
			// 
			// textBoxHeight
			// 
			this->textBoxHeight->Location = System::Drawing::Point(337, 23);
			this->textBoxHeight->Margin = System::Windows::Forms::Padding(4);
			this->textBoxHeight->Name = L"textBoxHeight";
			this->textBoxHeight->Size = System::Drawing::Size(76, 22);
			this->textBoxHeight->TabIndex = 3;
			this->textBoxHeight->Text = L"24";
			this->textBoxHeight->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(248, 27);
			this->label3->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(89, 17);
			this->label3->TabIndex = 2;
			this->label3->Text = L"Block height:";
			// 
			// textBoxWidth
			// 
			this->textBoxWidth->Location = System::Drawing::Point(127, 23);
			this->textBoxWidth->Margin = System::Windows::Forms::Padding(4);
			this->textBoxWidth->Name = L"textBoxWidth";
			this->textBoxWidth->Size = System::Drawing::Size(76, 22);
			this->textBoxWidth->TabIndex = 1;
			this->textBoxWidth->Text = L"24";
			this->textBoxWidth->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(35, 27);
			this->label4->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(82, 17);
			this->label4->TabIndex = 0;
			this->label4->Text = L"Block width:";
			// 
			// groupBoxPredictors
			// 
			this->groupBoxPredictors->Controls->Add(this->textBoxSigma);
			this->groupBoxPredictors->Controls->Add(this->label7);
			this->groupBoxPredictors->Controls->Add(this->checkBoxTMB);
			this->groupBoxPredictors->Controls->Add(this->checkBoxCobalpB);
			this->groupBoxPredictors->Controls->Add(this->checkBoxGAPB);
			this->groupBoxPredictors->Controls->Add(this->checkBoxLinB);
			this->groupBoxPredictors->Controls->Add(this->checkBoxPaethB);
			this->groupBoxPredictors->Controls->Add(this->checkBoxMEDB);
			this->groupBoxPredictors->Controls->Add(this->label6);
			this->groupBoxPredictors->Controls->Add(this->checkBoxTM);
			this->groupBoxPredictors->Controls->Add(this->checkBoxCobalp);
			this->groupBoxPredictors->Controls->Add(this->checkBoxGAP);
			this->groupBoxPredictors->Controls->Add(this->checkBoxLinear);
			this->groupBoxPredictors->Controls->Add(this->checkBoxPaeth);
			this->groupBoxPredictors->Controls->Add(this->label5);
			this->groupBoxPredictors->Controls->Add(this->checkBoxMED);
			this->groupBoxPredictors->Location = System::Drawing::Point(16, 282);
			this->groupBoxPredictors->Margin = System::Windows::Forms::Padding(4);
			this->groupBoxPredictors->Name = L"groupBoxPredictors";
			this->groupBoxPredictors->Padding = System::Windows::Forms::Padding(4);
			this->groupBoxPredictors->Size = System::Drawing::Size(447, 184);
			this->groupBoxPredictors->TabIndex = 12;
			this->groupBoxPredictors->TabStop = false;
			this->groupBoxPredictors->Text = L"Predictors";
			// 
			// textBoxSigma
			// 
			this->textBoxSigma->Enabled = false;
			this->textBoxSigma->Location = System::Drawing::Point(337, 62);
			this->textBoxSigma->Margin = System::Windows::Forms::Padding(4);
			this->textBoxSigma->Name = L"textBoxSigma";
			this->textBoxSigma->Size = System::Drawing::Size(87, 22);
			this->textBoxSigma->TabIndex = 15;
			this->textBoxSigma->Text = L"0";
			this->textBoxSigma->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(333, 36);
			this->label7->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(94, 17);
			this->label7->TabIndex = 14;
			this->label7->Text = L"Sigma (0-20):";
			// 
			// checkBoxTMB
			// 
			this->checkBoxTMB->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBoxTMB->Location = System::Drawing::Point(252, 143);
			this->checkBoxTMB->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxTMB->Name = L"checkBoxTMB";
			this->checkBoxTMB->Size = System::Drawing::Size(73, 28);
			this->checkBoxTMB->TabIndex = 13;
			this->checkBoxTMB->Text = L"Off";
			this->checkBoxTMB->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxTMB->UseVisualStyleBackColor = true;
			this->checkBoxTMB->CheckStateChanged += gcnew System::EventHandler(this, &ExternalPrediction::checkBoxTMB_CheckStateChanged);
			// 
			// checkBoxCobalpB
			// 
			this->checkBoxCobalpB->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBoxCobalpB->Location = System::Drawing::Point(171, 143);
			this->checkBoxCobalpB->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxCobalpB->Name = L"checkBoxCobalpB";
			this->checkBoxCobalpB->Size = System::Drawing::Size(73, 28);
			this->checkBoxCobalpB->TabIndex = 12;
			this->checkBoxCobalpB->Text = L"Off";
			this->checkBoxCobalpB->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxCobalpB->UseVisualStyleBackColor = true;
			this->checkBoxCobalpB->CheckStateChanged += gcnew System::EventHandler(this, &ExternalPrediction::checkBoxTMB_CheckStateChanged);
			// 
			// checkBoxGAPB
			// 
			this->checkBoxGAPB->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBoxGAPB->Location = System::Drawing::Point(89, 143);
			this->checkBoxGAPB->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxGAPB->Name = L"checkBoxGAPB";
			this->checkBoxGAPB->Size = System::Drawing::Size(73, 28);
			this->checkBoxGAPB->TabIndex = 11;
			this->checkBoxGAPB->Text = L"Off";
			this->checkBoxGAPB->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxGAPB->UseVisualStyleBackColor = true;
			this->checkBoxGAPB->CheckStateChanged += gcnew System::EventHandler(this, &ExternalPrediction::checkBoxTMB_CheckStateChanged);
			// 
			// checkBoxLinB
			// 
			this->checkBoxLinB->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBoxLinB->Location = System::Drawing::Point(252, 107);
			this->checkBoxLinB->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxLinB->Name = L"checkBoxLinB";
			this->checkBoxLinB->Size = System::Drawing::Size(73, 28);
			this->checkBoxLinB->TabIndex = 10;
			this->checkBoxLinB->Text = L"Off";
			this->checkBoxLinB->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxLinB->UseVisualStyleBackColor = true;
			this->checkBoxLinB->CheckStateChanged += gcnew System::EventHandler(this, &ExternalPrediction::checkBoxTMB_CheckStateChanged);
			// 
			// checkBoxPaethB
			// 
			this->checkBoxPaethB->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBoxPaethB->Location = System::Drawing::Point(171, 107);
			this->checkBoxPaethB->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxPaethB->Name = L"checkBoxPaethB";
			this->checkBoxPaethB->Size = System::Drawing::Size(73, 28);
			this->checkBoxPaethB->TabIndex = 9;
			this->checkBoxPaethB->Text = L"Off";
			this->checkBoxPaethB->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxPaethB->UseVisualStyleBackColor = true;
			this->checkBoxPaethB->CheckStateChanged += gcnew System::EventHandler(this, &ExternalPrediction::checkBoxTMB_CheckStateChanged);
			// 
			// checkBoxMEDB
			// 
			this->checkBoxMEDB->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBoxMEDB->Location = System::Drawing::Point(89, 107);
			this->checkBoxMEDB->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxMEDB->Name = L"checkBoxMEDB";
			this->checkBoxMEDB->Size = System::Drawing::Size(73, 28);
			this->checkBoxMEDB->TabIndex = 8;
			this->checkBoxMEDB->Text = L"Off";
			this->checkBoxMEDB->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxMEDB->UseVisualStyleBackColor = true;
			this->checkBoxMEDB->CheckStateChanged += gcnew System::EventHandler(this, &ExternalPrediction::checkBoxTMB_CheckStateChanged);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(33, 113);
			this->label6->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(39, 17);
			this->label6->TabIndex = 7;
			this->label6->Text = L"Bias:";
			// 
			// checkBoxTM
			// 
			this->checkBoxTM->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBoxTM->Location = System::Drawing::Point(252, 59);
			this->checkBoxTM->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxTM->Name = L"checkBoxTM";
			this->checkBoxTM->Size = System::Drawing::Size(73, 28);
			this->checkBoxTM->TabIndex = 6;
			this->checkBoxTM->Text = L"TM";
			this->checkBoxTM->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxTM->UseVisualStyleBackColor = true;
			this->checkBoxTM->CheckedChanged += gcnew System::EventHandler(this, &ExternalPrediction::checkBoxTM_CheckedChanged);
			// 
			// checkBoxCobalp
			// 
			this->checkBoxCobalp->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBoxCobalp->Checked = true;
			this->checkBoxCobalp->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxCobalp->Location = System::Drawing::Point(171, 59);
			this->checkBoxCobalp->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxCobalp->Name = L"checkBoxCobalp";
			this->checkBoxCobalp->Size = System::Drawing::Size(73, 28);
			this->checkBoxCobalp->TabIndex = 5;
			this->checkBoxCobalp->Text = L"CoBALP";
			this->checkBoxCobalp->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxCobalp->UseVisualStyleBackColor = true;
			// 
			// checkBoxGAP
			// 
			this->checkBoxGAP->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBoxGAP->Checked = true;
			this->checkBoxGAP->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxGAP->Location = System::Drawing::Point(89, 59);
			this->checkBoxGAP->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxGAP->Name = L"checkBoxGAP";
			this->checkBoxGAP->Size = System::Drawing::Size(73, 28);
			this->checkBoxGAP->TabIndex = 4;
			this->checkBoxGAP->Text = L"GAP";
			this->checkBoxGAP->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxGAP->UseVisualStyleBackColor = true;
			// 
			// checkBoxLinear
			// 
			this->checkBoxLinear->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBoxLinear->Checked = true;
			this->checkBoxLinear->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxLinear->Location = System::Drawing::Point(252, 23);
			this->checkBoxLinear->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxLinear->Name = L"checkBoxLinear";
			this->checkBoxLinear->Size = System::Drawing::Size(73, 28);
			this->checkBoxLinear->TabIndex = 3;
			this->checkBoxLinear->Text = L"Linear";
			this->checkBoxLinear->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxLinear->UseVisualStyleBackColor = true;
			// 
			// checkBoxPaeth
			// 
			this->checkBoxPaeth->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBoxPaeth->Checked = true;
			this->checkBoxPaeth->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxPaeth->Location = System::Drawing::Point(171, 23);
			this->checkBoxPaeth->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxPaeth->Name = L"checkBoxPaeth";
			this->checkBoxPaeth->Size = System::Drawing::Size(73, 28);
			this->checkBoxPaeth->TabIndex = 2;
			this->checkBoxPaeth->Text = L"Paeth";
			this->checkBoxPaeth->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxPaeth->UseVisualStyleBackColor = true;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(33, 36);
			this->label5->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(37, 17);
			this->label5->TabIndex = 1;
			this->label5->Text = L"Use:";
			// 
			// checkBoxMED
			// 
			this->checkBoxMED->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBoxMED->Checked = true;
			this->checkBoxMED->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBoxMED->Location = System::Drawing::Point(89, 23);
			this->checkBoxMED->Margin = System::Windows::Forms::Padding(4);
			this->checkBoxMED->Name = L"checkBoxMED";
			this->checkBoxMED->Size = System::Drawing::Size(73, 28);
			this->checkBoxMED->TabIndex = 0;
			this->checkBoxMED->Text = L"MED";
			this->checkBoxMED->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBoxMED->UseVisualStyleBackColor = true;
			// 
			// buttonPresetBlock
			// 
			this->buttonPresetBlock->ForeColor = System::Drawing::Color::Blue;
			this->buttonPresetBlock->Location = System::Drawing::Point(14, 474);
			this->buttonPresetBlock->Margin = System::Windows::Forms::Padding(4);
			this->buttonPresetBlock->Name = L"buttonPresetBlock";
			this->buttonPresetBlock->Size = System::Drawing::Size(100, 28);
			this->buttonPresetBlock->TabIndex = 13;
			this->buttonPresetBlock->Text = L"Opt. Block";
			this->buttonPresetBlock->UseVisualStyleBackColor = true;
			this->buttonPresetBlock->Click += gcnew System::EventHandler(this, &ExternalPrediction::buttonPreset_Click);
			// 
			// buttonPresetContext
			// 
			this->buttonPresetContext->ForeColor = System::Drawing::Color::Blue;
			this->buttonPresetContext->Location = System::Drawing::Point(122, 474);
			this->buttonPresetContext->Margin = System::Windows::Forms::Padding(4);
			this->buttonPresetContext->Name = L"buttonPresetContext";
			this->buttonPresetContext->Size = System::Drawing::Size(100, 28);
			this->buttonPresetContext->TabIndex = 14;
			this->buttonPresetContext->Text = L"Opt. Context";
			this->buttonPresetContext->UseVisualStyleBackColor = true;
			this->buttonPresetContext->Click += gcnew System::EventHandler(this, &ExternalPrediction::buttonPresetContext_Click);
			// 
			// ExternalPrediction
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(477, 516);
			this->ControlBox = false;
			this->Controls->Add(this->buttonPresetContext);
			this->Controls->Add(this->buttonPresetBlock);
			this->Controls->Add(this->groupBoxPredictors);
			this->Controls->Add(this->groupBoxBlockSize);
			this->Controls->Add(this->groupBoxContextParams);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBoxMode);
			this->Controls->Add(this->buttonAbort);
			this->Controls->Add(this->buttonOK);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"ExternalPrediction";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Prediction parameter";
			this->groupBoxMode->ResumeLayout(false);
			this->groupBoxMode->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->groupBoxContextParams->ResumeLayout(false);
			this->groupBoxContextParams->PerformLayout();
			this->groupBoxBlockSize->ResumeLayout(false);
			this->groupBoxBlockSize->PerformLayout();
			this->groupBoxPredictors->ResumeLayout(false);
			this->groupBoxPredictors->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
		System::Void radioButtonContext_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void radioButtonBlock_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void radioButtonMean_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void buttonOK_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void buttonAbort_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void checkBoxTM_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void checkBoxTMB_CheckStateChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
		System::Void buttonCtx_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void buttonPresetContext_Click(System::Object^  sender, System::EventArgs^  e);
		System::Void buttonPreset_Click(System::Object^  sender, System::EventArgs^  e);
};
}
