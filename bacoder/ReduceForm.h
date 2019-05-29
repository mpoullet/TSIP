/*****************************************************
 * Datei...: ReduceForm.h
 * Funktion: Header zu ReduceForm.cpp + Oberfläche
 * Autor...: Nico Hücker
 * Date....: 21.06.2012
 * 06.07.2012 translation to English
 ******************************************************/

#pragma once

#include "pdconvert.h"
#include "stats.h"
#include "ginterface.h"
#include "ExternalPrediction.h"
#include "externpred.h"
#include "CtxForm.h"
#include <string.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


/*
 * Prototyp der ReduceColours Methode
 */
unsigned long ReduceColours( PicData *pd, unsigned long distance, unsigned long *colourReduction);

namespace bacoder {

	/// <summary>
	/// Summary for ReduceForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ReduceForm : public System::Windows::Forms::Form
	{
	public:
		PicData *picdata;			/* Zeigerdeklaration zur Parameterübergabe */
		unsigned long *colourReduction;

		ReduceForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

		/* 
		 * Funktion um Parameter von Form1.h zu übergeben
		 */
		void InsertParams(PicData *pd, unsigned long *colours){
			picdata = pd;				/* Bilddatenstruktur übergeben */
			colourReduction = colours;	/* Feld für Farbvergleich vorher/nachher */
		}
		
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ReduceForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	protected: 
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  button1;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 25);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(204, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Maximum allowed Euclidean colour distace:";
			this->label1->Click += gcnew System::EventHandler(this, &ReduceForm::label1_Click);
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(54, 41);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(100, 20);
			this->textBox1->TabIndex = 1;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(54, 79);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(98, 39);
			this->button1->TabIndex = 2;
			this->button1->Text = L"Start";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &ReduceForm::button1_Click);
			this->button1->DialogResult = System::Windows::Forms::DialogResult::OK;
			// 
			// ReduceForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(231, 142);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->label1);
			this->Name = L"ReduceForm";
			this->Text = L"ReduceForm";
			this->Load += gcnew System::EventHandler(this, &ReduceForm::ReduceForm_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void ReduceForm_Load(System::Object^  sender, System::EventArgs^  e) {
				 e=e; sender=sender;
			 }
/* Prototyp der button1_Click Methode */
	 private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e);
	
	private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
				 e=e; sender=sender;
			 }
};
}
