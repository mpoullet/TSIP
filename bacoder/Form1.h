/*****************************************************
* File..: Form1.h
* Desc..: header file for main function
* Author: T. Strutz
* Date..: 05.05.2011
* 27.05.2011 Strutz: skip_postBWT, YUVtuned
* 04.10.2011 new colour transforms, no EntropyForm enay more
* 06.07.2012 reduceForm von PVL KTKI10 AZ-Darmstadt-1 eingebunden
* 02.08.2012 command line mode from PVL KTKI Nephten-Essen
* 04.07.2014 support of transparency, background image
* 15.07.2014 INTERNET_ switches for compilitaion of public EXEs
******************************************************/
#pragma once

#include "stdafx.h"
#include <time.h>
#include "pdconvert.h"
#include "stats.h"

#include "ProgressForm.h"
#include "ConfigForm.h"
#include "AboutForm.h"
#include "paeth.h"
#include "glue.h"
#include "bitbuf.h"
#include "externpred.h"
#include "ExternalPrediction.h"
#include "CtxForm.h"
#include "ReduceForm.h"
#include "H26xConfigForm.h"
#include "fileIO.h"

#define INTERNET_	// switches off the logging and the GUI
//
#undef INTERNET_

/* Externes Symbol für Fehlercodes */
extern unsigned int ERRCODE; /* in Frm1.cpp	*/
/* Standardkontext-Datei */
extern char *ctx_default_file;
extern bool iserrorimage;

extern  char* const thisVersion;

namespace bacoder {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging; 
	using namespace System::Threading;
	using namespace System::Runtime::InteropServices;
	using namespace System::IO;
	using namespace System::Media;

	public ref class Form1 : public System::Windows::Forms::Form
	{
	public: 		
		Bitmap^ formImage;		// Aktuell geladenes Bild
		PicData *picdata;		// Codec-kompatibles Bildobjekt
		ProgressForm ^prg;		// Fortschrittsanzeige
		ConfigForm^ cfg;		// Config-Menü
		CtxForm^ ctxform;		// Context Formular
		H26xConfigForm^ H26xConfig;
		AboutForm^ aboutfrm;
		ReduceForm^ reducefrm;
		//String^ globFilename;
		String^ inFilename;
		String^ outFilename;
		String^ rct_string;


	private: System::Windows::Forms::ToolStripMenuItem^  contextDateiLadenToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  reduceColoursToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;

	public: 
/* should avoid: InvalidOperationException-Element mit der Meldung 
	"Der Zugriff auf das Steuerelement Steuerelementname erfolgte von einem anderen Thread 
	als dem Thread, für den es erstellt wurde" aus." *
static property bool CheckForIllegalCrossThreadCalls 
{
	bool get ();
	void set ( bool value);
}
*/
		/* Thread zum Anzeigen des Fortschritt-Fensters */
		void thfunc()
		{
			prg->Visible = false; /* Strutz*/
			prg->ShowDialog();
		}
		void thend()
		{
			prg->BeginClose();
		}

		/* Prüfe auf Fehler */
		void CheckError( char * text)
		{
			if (ERRCODE == 0) return;
			String^ errstring;
			String^ textManaged;
			switch (ERRCODE)
			{
				case PPM_OPEN_FAIL: errstring = "PPM/PGM could not be opened."; break;
				case PPM_DAMAGED  : errstring = "PPM/PGM seems to be damaged."; break;
				case PPM_INVALID  : errstring = "Not a valid PPM/PGM file."; break;
				case MALLOC_FAILED: errstring = "Malloc failed.\n"; break;
				case PRED_CRASHED : errstring = "External predictor crashed."; break;
				case CTX_NOT_SET  : errstring = "Reconstruction of image in context failed.\n"
										"Missing Standard Context File?\n\n"
										"--> Extras --> Standard Context File"; break;
			}
			ERRCODE = 0;
			textManaged = Marshal::PtrToStringAnsi( (IntPtr)text);
			MessageBox::Show( textManaged + errstring, "Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
		}



		Form1()
		{			
			InitializeComponent();
			prg = gcnew ProgressForm();
			// defined here because it will cause an error in init method
			this->Text = gcnew System::String( thisVersion); 
			picdata = NULL;			
			
		}

	protected:
		~Form1()
		{			
			/* Speicher freigeben */
			if (picdata != NULL)
			{
				DeletePicData( picdata);
			}			
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::StatusStrip^  statusStrip1;	
	private: System::Windows::Forms::ToolStripStatusLabel^  StatusLabel;
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItemLaden;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItemSpeichern;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItemSchließen;

	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem2;
	private: System::Windows::Forms::ToolStripMenuItem^  leerenToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  extrasToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItemInfos;
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::OpenFileDialog^  openFile;
	private: System::Windows::Forms::ToolStripMenuItem^  anFenstergrößeAnpassenToolStripMenuItem;
	private: System::Windows::Forms::SaveFileDialog^  saveFile;

	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::ToolStripMenuItem^  hereinzoomenToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  herauszoomenToolStripMenuItem;




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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->StatusLabel = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItemLaden = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItemSpeichern = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->leerenToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItemSchließen = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->extrasToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripMenuItemInfos = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->hereinzoomenToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->herauszoomenToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->anFenstergrößeAnpassenToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->contextDateiLadenToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->reduceColoursToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->openFile = (gcnew System::Windows::Forms::OpenFileDialog());
			this->saveFile = (gcnew System::Windows::Forms::SaveFileDialog());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->statusStrip1->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->StatusLabel});
			this->statusStrip1->Location = System::Drawing::Point(0, 531);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Padding = System::Windows::Forms::Padding(1, 0, 19, 0);
			this->statusStrip1->Size = System::Drawing::Size(629, 23);
			this->statusStrip1->TabIndex = 0;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// StatusLabel
			// 
			this->StatusLabel->Name = L"StatusLabel";
			this->StatusLabel->Size = System::Drawing::Size(49, 18);
			this->StatusLabel->Text = L"Status";
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->toolStripMenuItem1, 
				this->extrasToolStripMenuItem, this->aboutToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Padding = System::Windows::Forms::Padding(8, 2, 0, 2);
			this->menuStrip1->Size = System::Drawing::Size(629, 26);
			this->menuStrip1->TabIndex = 1;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->toolStripMenuItemLaden, 
				this->toolStripMenuItemSpeichern, this->leerenToolStripMenuItem, this->toolStripMenuItemSchließen});
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			this->toolStripMenuItem1->Size = System::Drawing::Size(63, 22);
			this->toolStripMenuItem1->Text = L"Image";
			// 
			// toolStripMenuItemLaden
			// 
			this->toolStripMenuItemLaden->Name = L"toolStripMenuItemLaden";
			this->toolStripMenuItemLaden->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::O));
			this->toolStripMenuItemLaden->Size = System::Drawing::Size(176, 22);
			this->toolStripMenuItemLaden->Text = L"Open";
			this->toolStripMenuItemLaden->Click += gcnew System::EventHandler(this, &Form1::toolStripMenuItemLaden_Click);
			// 
			// toolStripMenuItemSpeichern
			// 
			this->toolStripMenuItemSpeichern->Name = L"toolStripMenuItemSpeichern";
			this->toolStripMenuItemSpeichern->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::S));
			this->toolStripMenuItemSpeichern->Size = System::Drawing::Size(176, 22);
			this->toolStripMenuItemSpeichern->Text = L"Save as";
			this->toolStripMenuItemSpeichern->Click += gcnew System::EventHandler(this, &Form1::toolStripMenuItemSpeichern_Click);
			// 
			// leerenToolStripMenuItem
			// 
			this->leerenToolStripMenuItem->Name = L"leerenToolStripMenuItem";
			this->leerenToolStripMenuItem->Size = System::Drawing::Size(176, 22);
			this->leerenToolStripMenuItem->Text = L"Close";
			this->leerenToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::leerenToolStripMenuItem_Click);
			// 
			// toolStripMenuItemSchließen
			// 
			this->toolStripMenuItemSchließen->Name = L"toolStripMenuItemSchließen";
			this->toolStripMenuItemSchließen->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Control | System::Windows::Forms::Keys::Q));
			this->toolStripMenuItemSchließen->Size = System::Drawing::Size(176, 22);
			this->toolStripMenuItemSchließen->Text = L"Exit";
			this->toolStripMenuItemSchließen->Click += gcnew System::EventHandler(this, &Form1::toolStripMenuItemSchließen_Click);
			// 
			// extrasToolStripMenuItem
			// 
			this->extrasToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->toolStripMenuItemInfos, 
				this->hereinzoomenToolStripMenuItem, this->herauszoomenToolStripMenuItem, this->anFenstergrößeAnpassenToolStripMenuItem, this->contextDateiLadenToolStripMenuItem, 
				this->reduceColoursToolStripMenuItem});
			this->extrasToolStripMenuItem->Enabled = false;
			this->extrasToolStripMenuItem->Name = L"extrasToolStripMenuItem";
			this->extrasToolStripMenuItem->Size = System::Drawing::Size(61, 22);
			this->extrasToolStripMenuItem->Text = L"Extras";
			// 
			// toolStripMenuItemInfos
			// 
			this->toolStripMenuItemInfos->Name = L"toolStripMenuItemInfos";
			this->toolStripMenuItemInfos->Size = System::Drawing::Size(235, 22);
			this->toolStripMenuItemInfos->Text = L"Image informationen";
			this->toolStripMenuItemInfos->Click += gcnew System::EventHandler(this, &Form1::toolStripMenuItemInfos_Click);
			// 
			// hereinzoomenToolStripMenuItem
			// 
			this->hereinzoomenToolStripMenuItem->Name = L"hereinzoomenToolStripMenuItem";
			this->hereinzoomenToolStripMenuItem->Size = System::Drawing::Size(235, 22);
			this->hereinzoomenToolStripMenuItem->Text = L"Zoom in &+";
			this->hereinzoomenToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::hereinzoomenToolStripMenuItem_Click);
			// 
			// herauszoomenToolStripMenuItem
			// 
			this->herauszoomenToolStripMenuItem->Name = L"herauszoomenToolStripMenuItem";
			this->herauszoomenToolStripMenuItem->Size = System::Drawing::Size(235, 22);
			this->herauszoomenToolStripMenuItem->Text = L"Zoom out &-";
			this->herauszoomenToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::herauszoomenToolStripMenuItem_Click);
			// 
			// anFenstergrößeAnpassenToolStripMenuItem
			// 
			this->anFenstergrößeAnpassenToolStripMenuItem->Name = L"anFenstergrößeAnpassenToolStripMenuItem";
			this->anFenstergrößeAnpassenToolStripMenuItem->Size = System::Drawing::Size(235, 22);
			this->anFenstergrößeAnpassenToolStripMenuItem->Text = L"Fit to window";
			this->anFenstergrößeAnpassenToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::anFenstergrößeAnpassenToolStripMenuItem_Click);
			// 
			// contextDateiLadenToolStripMenuItem
			// 
			this->contextDateiLadenToolStripMenuItem->Name = L"contextDateiLadenToolStripMenuItem";
			this->contextDateiLadenToolStripMenuItem->Size = System::Drawing::Size(235, 22);
			this->contextDateiLadenToolStripMenuItem->Text = L"Set standard context file";
			this->contextDateiLadenToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::contextDateiLadenToolStripMenuItem_Click);
			// 
			// reduceColoursToolStripMenuItem
			// 
			this->reduceColoursToolStripMenuItem->Name = L"reduceColoursToolStripMenuItem";
			this->reduceColoursToolStripMenuItem->Size = System::Drawing::Size(235, 22);
			this->reduceColoursToolStripMenuItem->Text = L"Reduce Colours";
			this->reduceColoursToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::reduceColoursToolStripMenuItem_Click);
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(58, 22);
			this->aboutToolStripMenuItem->Text = L"About";
			this->aboutToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::aboutToolStripMenuItem_Click);
			// 
			// pictureBox1
			// 
			this->pictureBox1->BackColor = System::Drawing::Color::White;
			this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBox1->Location = System::Drawing::Point(0, 0);
			this->pictureBox1->Margin = System::Windows::Forms::Padding(4);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(629, 505);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pictureBox1->TabIndex = 2;
			this->pictureBox1->TabStop = false;
			// 
			// openFile
			// 
			this->openFile->Filter = resources->GetString(L"openFile.Filter");
			this->openFile->Title = L"Bilddatei laden...";
			// 
			// saveFile
			// 
			this->saveFile->Filter = resources->GetString(L"saveFile.Filter");
			this->saveFile->Title = L"Bilddatei speichern...";
			// 
			// panel1
			// 
			this->panel1->AutoScroll = true;
			this->panel1->Controls->Add(this->pictureBox1);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->panel1->Location = System::Drawing::Point(0, 26);
			this->panel1->Margin = System::Windows::Forms::Padding(4);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(629, 505);
			this->panel1->TabIndex = 3;
			// 
			// Form1
			// 
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::None;
			this->ClientSize = System::Drawing::Size(629, 554);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->menuStrip1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->MainMenuStrip = this->menuStrip1;
			this->Margin = System::Windows::Forms::Padding(4);
			this->Name = L"Form1";
			this->Text = L"<Version>";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->panel1->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private:
		/* Schließen */
		System::Void toolStripMenuItemSchließen_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			this->Close();
			e=e; sender=sender;
		}		
		/* proto typing	*/
		Void toolStripMenuItemLaden_Click(System::Object^  sender, System::EventArgs^  e);
		Void toolStripMenuItemSpeichern_Click(System::Object^  sender, System::EventArgs^  e);

		/* Hereinzoomen */
		System::Void hereinzoomenToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 	 
		{
			pictureBox1->Dock = DockStyle::None;
			pictureBox1->Width += 250;
			pictureBox1->Height += 250;
			e=e; sender=sender;
		}

		/* Herauszoomen */
		System::Void herauszoomenToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			pictureBox1->Dock = DockStyle::None;
			if (pictureBox1->Width > 300 && pictureBox1->Height > 300)
			{		
				pictureBox1->Width -= 250;
				pictureBox1->Height -= 250;
			}
			e=e; sender=sender;
		}


		/* Bildinformationen */
		System::Void toolStripMenuItemInfos_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			double entropy;

			/* Bild geladen? */
			if (pictureBox1->Image == nullptr)
			{
				MessageBox::Show("You have to open an image first.",
					"Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
				return;
			}

			/* Informationen ausgeben */
			entropy = Math::Round( PicEntropy(picdata), 3);
			MessageBox::Show("Width of image:\t" + picdata->width + " Pixel\n" + 
				"height of image:\t" + picdata->height + " Pixel\n" +
				"number of components:\t" + picdata->channel + "\n" + 
				"current colour space:\t" + ((picdata->space == RGBc) ? "RGB\n" : 
				(picdata->space == A7_1) ? "YUVr\n" : 
				(picdata->space == GREY) ? "GREY\n" :
				//										 (picdata->space == YCGCO) ? "YCGCO\n" :
				(picdata->space == A7_11) ? "YCgCo-R\n" : "") +
				"bits per value:\t" + picdata->bitperchannel + "\n" +
				"highest value:\t" + ((1u << picdata->bitperchannel) - 1u) + "\n"
				"mean entropy:\t" + entropy + " bits per value\n",								 
				"image information", MessageBoxButtons::OK);
			e=e; sender=sender;
		}


		/* Bild leeren */
		System::Void leerenToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			e=e; sender=sender;
			pictureBox1->Image = nullptr;
		}



		/* Zoom auf Fenster */
		System::Void anFenstergrößeAnpassenToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
		{
			pictureBox1->Dock = DockStyle::Fill;
			e=e; sender=sender;
		}

		/* Context Datei festlegen */
		System::Void contextDateiLadenToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			e=e; sender=sender;
			ctxform = gcnew CtxForm();
			ctxform->ShowDialog();
			// Strutz ctx_default_file = (ctxform->filename == "") ? NULL : ctxform->filename;
			ctx_default_file = (strcmp( ctxform->filename, "")==0) ? NULL : ctxform->filename;
			StatusLabel->Text = "Context file is set.";
		}

		/* About */
		System::Void aboutToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e)
		{
			aboutfrm = gcnew AboutForm();
			aboutfrm->ShowDialog();
			e=e; sender=sender;
		}
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) 
			{
				 e=e; sender=sender;
			}

			 /* Bildfarben durch Quantisierung reduzieren */
	private: System::Void reduceColoursToolStripMenuItem_Click(
				 System::Object^  sender, System::EventArgs^  e) 
			 {
				 /* Bild geladen? */
				 if (pictureBox1->Image == nullptr)
				 {
					 MessageBox::Show( "You have to open an image first.",
						 "Error", MessageBoxButtons::OK, MessageBoxIcon::Information);
					 return;
				 }
				 /* Neues Fenster zur Quantisierung aufrufen */
				 reducefrm = gcnew ReduceForm();

				 /* Feld für vorher/nachher Vergleich der Farben */
				 unsigned long colourReduction[2];

				 /* Parameter an das Fenster übergeben */
				 reducefrm->InsertParams( picdata, colourReduction);
				 reducefrm->ShowDialog();

				 if (reducefrm->DialogResult == System::Windows::Forms::DialogResult::OK){
					 /* Aktualisieren der Ansicht mit bearbeiteter Bilddatenstruktur */
					 formImage = GetBitmapFromPicData( picdata);
					 pictureBox1->Image = formImage;

					 /* Ausgabe der Farbanzahl vorher/nachher für Auswertung */
					 MessageBox::Show( colourReduction[0] + " colours have been quantised to " + 
						 colourReduction[1] + " colours!");
				 }

				 e=e; sender=sender;
			 }	 		 

	public: System::Void setImageOfPictureBox1(System::Drawing::Image^ bitmap)
			{
				pictureBox1->Image = bitmap;
				return;
			}	 

/*Anfang Team Hannover PNG-Transparenz*/
/*------------------------------------------------------------------
*	PNG Funktionsaufruf des Kachelhintergrunds
*
*
*-----------------------------------------------------------------*/
	System::Void setBackgroundOfPictureBox1(System::Drawing::Image^ bitmap)
	{
		pictureBox1->BackgroundImage = bitmap;
				return;
	}
/*Ende Team Hannover PNG-Transparenz*/

	};

}


