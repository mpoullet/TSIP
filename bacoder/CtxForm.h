#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Runtime::InteropServices;

namespace bacoder {

	/// <summary>
	/// Zusammenfassung für CtxForm
	///
	/// Warnung: Wenn Sie den Namen dieser Klasse ändern, müssen Sie auch
	///          die Ressourcendateiname-Eigenschaft für das Tool zur Kompilierung verwalteter Ressourcen ändern,
	///          das allen RESX-Dateien zugewiesen ist, von denen diese Klasse abhängt.
	///          Anderenfalls können die Designer nicht korrekt mit den lokalisierten Ressourcen
	///          arbeiten, die diesem Formular zugewiesen sind.
	/// </summary>
	public ref class CtxForm : public System::Windows::Forms::Form
	{
	public:

		char *filename;

		CtxForm(void)
		{
			InitializeComponent();
			filename = NULL;
			//
			//TODO: Konstruktorcode hier hinzufügen.
			//
		}

	protected:
		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		~CtxForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  buttonBrowse;
	private: System::Windows::Forms::Button^  buttonOK;
	private: System::Windows::Forms::OpenFileDialog^  openFile;
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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(CtxForm::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->buttonBrowse = (gcnew System::Windows::Forms::Button());
			this->buttonOK = (gcnew System::Windows::Forms::Button());
			this->openFile = (gcnew System::Windows::Forms::OpenFileDialog());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(9, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(74, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Context Datei:";
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(12, 25);
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->Size = System::Drawing::Size(268, 20);
			this->textBox1->TabIndex = 1;
			// 
			// buttonBrowse
			// 
			this->buttonBrowse->Location = System::Drawing::Point(286, 25);
			this->buttonBrowse->Name = L"buttonBrowse";
			this->buttonBrowse->Size = System::Drawing::Size(40, 20);
			this->buttonBrowse->TabIndex = 2;
			this->buttonBrowse->Text = L"[...]";
			this->buttonBrowse->UseVisualStyleBackColor = true;
			this->buttonBrowse->Click += gcnew System::EventHandler(this, &CtxForm::buttonBrowse_Click);
			// 
			// buttonOK
			// 
			this->buttonOK->Location = System::Drawing::Point(12, 66);
			this->buttonOK->Name = L"buttonOK";
			this->buttonOK->Size = System::Drawing::Size(314, 23);
			this->buttonOK->TabIndex = 3;
			this->buttonOK->Text = L"Übernehmen";
			this->buttonOK->UseVisualStyleBackColor = true;
			this->buttonOK->Click += gcnew System::EventHandler(this, &CtxForm::buttonOK_Click);
			// 
			// openFile
			// 
			this->openFile->Filter = L"Context Dateien *.ctx|*.ctx";
			// 
			// CtxForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(338, 101);
			this->ControlBox = false;
			this->Controls->Add(this->buttonOK);
			this->Controls->Add(this->buttonBrowse);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"CtxForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Context Datei festlegen";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	
		System::Void buttonBrowse_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			openFile->ShowDialog();
			textBox1->Text = openFile->FileName;
			textBox1->SelectionLength = textBox1->Text->Length;
				e=e; sender=sender;
		}

		System::Void buttonOK_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			filename = (char *)(void *)Marshal::StringToHGlobalAnsi(textBox1->Text);
			this->Close();
				e=e; sender=sender;
		}
};
}
