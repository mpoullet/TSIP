#pragma once

#include "autoConfig.h"
#include "H26x_Config.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace bacoder {

	/// <summary>
	/// Zusammenfassung für H26xConfigForm
	///
	/// Warnung: Wenn Sie den Namen dieser Klasse ändern, müssen Sie auch
	///          die Ressourcendateiname-Eigenschaft für das Tool zur Kompilierung verwalteter Ressourcen ändern,
	///          das allen RESX-Dateien zugewiesen ist, von denen diese Klasse abhängt.
	///          Anderenfalls können die Designer nicht korrekt mit den lokalisierten Ressourcen
	///          arbeiten, die diesem Formular zugewiesen sind.
	/// </summary>
	public ref class H26xConfigForm : public System::Windows::Forms::Form
	{
	public:
		H26xConfigForm(void)
		{
			InitializeComponent();
			//
			//TODO: Konstruktorcode hier hinzufügen.
			//
		}

	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~H26xConfigForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::CheckBox^  checkBox_TrafoLog;
	protected: 

	protected: 

	protected: 
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::TextBox^  text_MSEthres;

	private: H26xConfiguration* Config;
	public: Void initConfiguration(H26xConfiguration* config);

	public: Void setConfiguration();
	public: Void getConfiguration(H26xConfiguration* config);
	private: System::Windows::Forms::CheckBox^  checkBox_BlockLog;
	private: System::Windows::Forms::CheckBox^  checkBox_PredLog;
	private: System::Windows::Forms::CheckedListBox^  optPredictors;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::CheckBox^  checkBox_errPic;

	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::CheckedListBox^  optBlocksizes;

	private: System::Windows::Forms::Button^  buttonPresetBlock;
	private: System::Windows::Forms::Button^  buttonAbort;
	private: System::Windows::Forms::Button^  buttonOK;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::RadioButton^  radio53noSep;
	private: System::Windows::Forms::RadioButton^  radio53sep;
	private: System::Windows::Forms::RadioButton^  radioNoTransform;


	protected: 


	protected: 

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
			this->checkBox_BlockLog = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_PredLog = (gcnew System::Windows::Forms::CheckBox());
			this->optPredictors = (gcnew System::Windows::Forms::CheckedListBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBox_TrafoLog = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox_errPic = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->optBlocksizes = (gcnew System::Windows::Forms::CheckedListBox());
			this->buttonPresetBlock = (gcnew System::Windows::Forms::Button());
			this->buttonAbort = (gcnew System::Windows::Forms::Button());
			this->buttonOK = (gcnew System::Windows::Forms::Button());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->radio53noSep = (gcnew System::Windows::Forms::RadioButton());
			this->radio53sep = (gcnew System::Windows::Forms::RadioButton());
			this->radioNoTransform = (gcnew System::Windows::Forms::RadioButton());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->text_MSEthres = (gcnew System::Windows::Forms::TextBox());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->SuspendLayout();
			// 
			// checkBox_BlockLog
			// 
			this->checkBox_BlockLog->AutoSize = true;
			this->checkBox_BlockLog->Location = System::Drawing::Point(10, 19);
			this->checkBox_BlockLog->Name = L"checkBox_BlockLog";
			this->checkBox_BlockLog->Size = System::Drawing::Size(101, 17);
			this->checkBox_BlockLog->TabIndex = 0;
			this->checkBox_BlockLog->Text = L"used blocksizes";
			this->checkBox_BlockLog->UseVisualStyleBackColor = true;
			this->checkBox_BlockLog->CheckedChanged += gcnew System::EventHandler(this, &H26xConfigForm::checkBox1_CheckedChanged);
			// 
			// checkBox_PredLog
			// 
			this->checkBox_PredLog->AutoSize = true;
			this->checkBox_PredLog->Location = System::Drawing::Point(10, 42);
			this->checkBox_PredLog->Name = L"checkBox_PredLog";
			this->checkBox_PredLog->Size = System::Drawing::Size(103, 17);
			this->checkBox_PredLog->TabIndex = 1;
			this->checkBox_PredLog->Text = L"used predictions";
			this->checkBox_PredLog->UseVisualStyleBackColor = true;
			// 
			// optPredictors
			// 
			this->optPredictors->CheckOnClick = true;
			this->optPredictors->FormattingEnabled = true;
			this->optPredictors->Items->AddRange(gcnew cli::array< System::Object^  >(35) {L"Planar", L"DC", L"2 ", L"3", L"4", L"5", 
				L"6", L"7", L"8", L"9", L"10", L"11", L"12", L"13", L"14", L"15", L"16", L"17", L"18", L"19", L"20", L"21", L"22", L"23", L"24", 
				L"25", L"26", L"27", L"28", L"29", L"30", L"31", L"32", L"33", L"34"});
			this->optPredictors->Location = System::Drawing::Point(9, 19);
			this->optPredictors->Name = L"optPredictors";
			this->optPredictors->Size = System::Drawing::Size(140, 124);
			this->optPredictors->TabIndex = 2;
			this->optPredictors->SelectedIndexChanged += gcnew System::EventHandler(this, &H26xConfigForm::checkedListBox1_SelectedIndexChanged);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->checkBox_TrafoLog);
			this->groupBox1->Controls->Add(this->checkBox_errPic);
			this->groupBox1->Controls->Add(this->checkBox_BlockLog);
			this->groupBox1->Controls->Add(this->checkBox_PredLog);
			this->groupBox1->Location = System::Drawing::Point(12, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(327, 112);
			this->groupBox1->TabIndex = 3;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Logging options";
			// 
			// checkBox_TrafoLog
			// 
			this->checkBox_TrafoLog->AutoSize = true;
			this->checkBox_TrafoLog->Location = System::Drawing::Point(10, 65);
			this->checkBox_TrafoLog->Name = L"checkBox_TrafoLog";
			this->checkBox_TrafoLog->Size = System::Drawing::Size(123, 17);
			this->checkBox_TrafoLog->TabIndex = 3;
			this->checkBox_TrafoLog->Text = L"used transformations";
			this->checkBox_TrafoLog->UseVisualStyleBackColor = true;
			// 
			// checkBox_errPic
			// 
			this->checkBox_errPic->AutoSize = true;
			this->checkBox_errPic->Location = System::Drawing::Point(10, 88);
			this->checkBox_errPic->Name = L"checkBox_errPic";
			this->checkBox_errPic->Size = System::Drawing::Size(99, 17);
			this->checkBox_errPic->TabIndex = 2;
			this->checkBox_errPic->Text = L"picture of errors";
			this->checkBox_errPic->UseVisualStyleBackColor = true;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->optBlocksizes);
			this->groupBox2->Controls->Add(this->optPredictors);
			this->groupBox2->Location = System::Drawing::Point(13, 131);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(326, 153);
			this->groupBox2->TabIndex = 4;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Prediction settings";
			this->groupBox2->Enter += gcnew System::EventHandler(this, &H26xConfigForm::groupBox2_Enter);
			// 
			// optBlocksizes
			// 
			this->optBlocksizes->BackColor = System::Drawing::SystemColors::Window;
			this->optBlocksizes->CheckOnClick = true;
			this->optBlocksizes->FormattingEnabled = true;
			this->optBlocksizes->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"4x4", L"8x8", L"16x16"});
			this->optBlocksizes->Location = System::Drawing::Point(178, 19);
			this->optBlocksizes->Name = L"optBlocksizes";
			this->optBlocksizes->Size = System::Drawing::Size(116, 124);
			this->optBlocksizes->TabIndex = 3;
			// 
			// buttonPresetBlock
			// 
			this->buttonPresetBlock->ForeColor = System::Drawing::Color::Blue;
			this->buttonPresetBlock->Location = System::Drawing::Point(12, 461);
			this->buttonPresetBlock->Name = L"buttonPresetBlock";
			this->buttonPresetBlock->Size = System::Drawing::Size(75, 23);
			this->buttonPresetBlock->TabIndex = 17;
			this->buttonPresetBlock->Text = L"Standard";
			this->buttonPresetBlock->UseVisualStyleBackColor = true;
			this->buttonPresetBlock->Click += gcnew System::EventHandler(this, &H26xConfigForm::buttonPresetBlock_Click);
			// 
			// buttonAbort
			// 
			this->buttonAbort->DialogResult = System::Windows::Forms::DialogResult::Abort;
			this->buttonAbort->Location = System::Drawing::Point(184, 461);
			this->buttonAbort->Name = L"buttonAbort";
			this->buttonAbort->Size = System::Drawing::Size(75, 23);
			this->buttonAbort->TabIndex = 16;
			this->buttonAbort->Text = L"Cancel";
			this->buttonAbort->UseVisualStyleBackColor = true;
			// 
			// buttonOK
			// 
			this->buttonOK->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->buttonOK->Location = System::Drawing::Point(265, 461);
			this->buttonOK->Name = L"buttonOK";
			this->buttonOK->Size = System::Drawing::Size(75, 23);
			this->buttonOK->TabIndex = 15;
			this->buttonOK->Text = L"Compress";
			this->buttonOK->UseVisualStyleBackColor = true;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->radio53noSep);
			this->groupBox3->Controls->Add(this->radio53sep);
			this->groupBox3->Controls->Add(this->radioNoTransform);
			this->groupBox3->Location = System::Drawing::Point(13, 349);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(326, 95);
			this->groupBox3->TabIndex = 18;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Post prediction transform";
			// 
			// radio53noSep
			// 
			this->radio53noSep->AutoSize = true;
			this->radio53noSep->Location = System::Drawing::Point(12, 69);
			this->radio53noSep->Name = L"radio53noSep";
			this->radio53noSep->Size = System::Drawing::Size(149, 17);
			this->radio53noSep->TabIndex = 2;
			this->radio53noSep->TabStop = true;
			this->radio53noSep->Text = L"5/3 Lifting (non separable)";
			this->radio53noSep->UseVisualStyleBackColor = true;
			// 
			// radio53sep
			// 
			this->radio53sep->AutoSize = true;
			this->radio53sep->Location = System::Drawing::Point(12, 46);
			this->radio53sep->Name = L"radio53sep";
			this->radio53sep->Size = System::Drawing::Size(128, 17);
			this->radio53sep->TabIndex = 1;
			this->radio53sep->TabStop = true;
			this->radio53sep->Text = L"5/3 Lifting (separable)";
			this->radio53sep->UseVisualStyleBackColor = true;
			this->radio53sep->CheckedChanged += gcnew System::EventHandler(this, &H26xConfigForm::radio53sep_CheckedChanged);
			// 
			// radioNoTransform
			// 
			this->radioNoTransform->AutoSize = true;
			this->radioNoTransform->Location = System::Drawing::Point(12, 23);
			this->radioNoTransform->Name = L"radioNoTransform";
			this->radioNoTransform->Size = System::Drawing::Size(49, 17);
			this->radioNoTransform->TabIndex = 0;
			this->radioNoTransform->TabStop = true;
			this->radioNoTransform->Text = L"none";
			this->radioNoTransform->UseVisualStyleBackColor = true;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->text_MSEthres);
			this->groupBox4->Location = System::Drawing::Point(13, 290);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(326, 51);
			this->groupBox4->TabIndex = 20;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"MSE Threshold";
			// 
			// text_MSEthres
			// 
			this->text_MSEthres->Location = System::Drawing::Point(10, 19);
			this->text_MSEthres->Name = L"text_MSEthres";
			this->text_MSEthres->Size = System::Drawing::Size(78, 20);
			this->text_MSEthres->TabIndex = 0;
			// 
			// H26xConfigForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(350, 495);
			this->ControlBox = false;
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->buttonPresetBlock);
			this->Controls->Add(this->buttonAbort);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->buttonOK);
			this->Name = L"H26xConfigForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Configuration of the H.26x Predictor";
			this->Load += gcnew System::EventHandler(this, &H26xConfigForm::H26xConfigForm_Load);
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void H26xConfigForm_Load(System::Object^  sender, System::EventArgs^  e) {
				 sender = sender; e = e;
			 }
	private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 sender = sender; e = e;
			 }
	private: System::Void groupBox2_Enter(System::Object^  sender, System::EventArgs^  e) {
				 sender = sender; e = e;
			 }
	private: System::Void checkedListBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
				 sender = sender; e = e;
			 }
	private: System::Void buttonPresetBlock_Click(System::Object^  sender, System::EventArgs^  e) {
				 sender = sender; e = e;
				 setConfiguration();
			 }
	private: System::Void radio53sep_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
				 sender = sender; e = e;
			 }
	};
}
