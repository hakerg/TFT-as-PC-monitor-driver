#pragma once
#include "FrameGenerator.h"
#include "RegionFinder.h"
#include <vcclr.h>
#include <iostream>

namespace GUI {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;
	using namespace Microsoft::Win32;

	/// <summary>
	/// Podsumowanie informacji o MainForm
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
			//
			//TODO: W tym miejscu dodaj kod konstruktora
			//
			DetectAndConnect();
		}

	protected:
		/// <summary>
		/// Wyczyœæ wszystkie u¿ywane zasoby.
		/// </summary>
		~MainForm()
		{
			button1->Enabled = false;
			if (running)
			{
				button1->Text = L"Disconnecting...";
				running = false;
				frameGenerator->StopThread();
				sender->StopThread();
				arduino->StopThread();
				delete serialPort;
			}
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^ label1;

	private: System::Windows::Forms::Button^ button1;
	private: System::Windows::Forms::CheckBox^ checkBox1;
	private: System::Windows::Forms::CheckBox^ checkBox2;
	private: System::Windows::Forms::CheckBox^ checkBox3;
	private: System::Windows::Forms::GroupBox^ groupBox1;
	private: System::Windows::Forms::GroupBox^ groupBox2;

	private: System::Windows::Forms::ComboBox^ comboBox1;
	private: System::Windows::Forms::Button^ button2;
	private: System::Windows::Forms::StatusStrip^ statusStrip1;
	private: System::Windows::Forms::ToolStripStatusLabel^ toolStripStatusLabel1;




	private: System::Windows::Forms::CheckBox^ checkBox4;

	protected:

	private:
		/// <summary>
		/// Wymagana zmienna projektanta.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Metoda wymagana do obs³ugi projektanta — nie nale¿y modyfikowaæ
		/// jej zawartoœci w edytorze kodu.
		/// </summary>
		void InitializeComponent(void)
		{
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox2 = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox3 = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBox4 = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->statusStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(6, 22);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(55, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Port name";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(6, 46);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(263, 23);
			this->button1->TabIndex = 2;
			this->button1->Text = L"Connect";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MainForm::button1_Click);
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(6, 19);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(64, 17);
			this->checkBox1->TabIndex = 3;
			this->checkBox1->Text = L"Zoom in";
			this->checkBox1->UseVisualStyleBackColor = true;
			// 
			// checkBox2
			// 
			this->checkBox2->AutoSize = true;
			this->checkBox2->Checked = true;
			this->checkBox2->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox2->Location = System::Drawing::Point(6, 42);
			this->checkBox2->Name = L"checkBox2";
			this->checkBox2->Size = System::Drawing::Size(211, 17);
			this->checkBox2->TabIndex = 4;
			this->checkBox2->Text = L"HQ mode (may decrease the framerate)";
			this->checkBox2->UseVisualStyleBackColor = true;
			// 
			// checkBox3
			// 
			this->checkBox3->AutoSize = true;
			this->checkBox3->Checked = true;
			this->checkBox3->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox3->Location = System::Drawing::Point(6, 65);
			this->checkBox3->Name = L"checkBox3";
			this->checkBox3->Size = System::Drawing::Size(102, 17);
			this->checkBox3->TabIndex = 5;
			this->checkBox3->Text = L"Enable dithering";
			this->checkBox3->UseVisualStyleBackColor = true;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->checkBox4);
			this->groupBox1->Controls->Add(this->checkBox1);
			this->groupBox1->Controls->Add(this->checkBox3);
			this->groupBox1->Controls->Add(this->checkBox2);
			this->groupBox1->Location = System::Drawing::Point(12, 93);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(275, 111);
			this->groupBox1->TabIndex = 6;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Settings";
			// 
			// checkBox4
			// 
			this->checkBox4->AutoSize = true;
			this->checkBox4->Checked = true;
			this->checkBox4->CheckState = System::Windows::Forms::CheckState::Checked;
			this->checkBox4->Location = System::Drawing::Point(6, 88);
			this->checkBox4->Name = L"checkBox4";
			this->checkBox4->Size = System::Drawing::Size(115, 17);
			this->checkBox4->TabIndex = 10;
			this->checkBox4->Text = L"Enable touch input";
			this->checkBox4->UseVisualStyleBackColor = true;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->button2);
			this->groupBox2->Controls->Add(this->comboBox1);
			this->groupBox2->Controls->Add(this->label1);
			this->groupBox2->Controls->Add(this->button1);
			this->groupBox2->Location = System::Drawing::Point(12, 12);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(275, 75);
			this->groupBox2->TabIndex = 7;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Connection";
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(194, 17);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 4;
			this->button2->Text = L"Refresh";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MainForm::button2_Click);
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Location = System::Drawing::Point(67, 19);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(121, 21);
			this->comboBox1->TabIndex = 3;
			// 
			// statusStrip1
			// 
			this->statusStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->toolStripStatusLabel1 });
			this->statusStrip1->Location = System::Drawing::Point(0, 207);
			this->statusStrip1->Name = L"statusStrip1";
			this->statusStrip1->Size = System::Drawing::Size(299, 22);
			this->statusStrip1->TabIndex = 9;
			this->statusStrip1->Text = L"statusStrip1";
			// 
			// toolStripStatusLabel1
			// 
			this->toolStripStatusLabel1->Name = L"toolStripStatusLabel1";
			this->toolStripStatusLabel1->Size = System::Drawing::Size(122, 17);
			this->toolStripStatusLabel1->Text = L"Status: not connected";
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(299, 229);
			this->Controls->Add(this->statusStrip1);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->Name = L"MainForm";
			this->Text = L"Arduino TFT driver";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->statusStrip1->ResumeLayout(false);
			this->statusStrip1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private:

		static constexpr int defaultMaxRegionSizeX = 64;
		static constexpr int defaultMaxRegionSizeY = 16;

		bool running;
		SerialPort* serialPort;
		Arduino^ arduino;
		RegionFinder^ sender;
		FrameGenerator^ frameGenerator;
		Windows::Forms::Timer^ statusTimer = gcnew Windows::Forms::Timer;
		System::EventHandler^ statusHandler;
		ClipRectangle clipRectangle;

		void DetectAndConnect()
		{
			auto serialComm = Registry::LocalMachine
				->OpenSubKey("HARDWARE")
				->OpenSubKey("DEVICEMAP")
				->OpenSubKey("SERIALCOMM");

			auto valueNames = serialComm->GetValueNames();

			comboBox1->Items->Clear();

			for each (auto valueName in valueNames)
			{
				comboBox1->Items->Add(serialComm->GetValue(valueName));
			}
			if (comboBox1->Items->Count > 0)
			{
				comboBox1->Text = comboBox1->Items[0]->ToString();
			}
			if (comboBox1->Items->Count == 1 && !running)
			{
				TryConnect();
			}
		}

		void CheckStatus(Object^, EventArgs^)
		{
			if (arduino->error)
			{
				Disconnect();
				TryConnect();
			}
		}

		void Disconnect()
		{
			button1->Enabled = false;
			button1->Text = L"Disconnecting...";
			statusTimer->Tick -= statusHandler;
			running = false;
			frameGenerator->StopThread();
			sender->StopThread();
			arduino->StopThread();
			delete serialPort;
			button1->Enabled = true;
			toolStripStatusLabel1->Text = L"Status: not connected";
			toolStripStatusLabel1->ForeColor = Color::Black;
			button1->Text = L"Connect";
		}

		void TryConnect()
		{
			button1->Enabled = false;
			button1->Text = L"Connecting...";
			pin_ptr<const wchar_t> port = PtrToStringChars(comboBox1->Text);
			serialPort = new SerialPort(port, 2000000);
			if (serialPort->IsConnected()) {
				frameGenerator = gcnew FrameGenerator(checkBox1, checkBox2, checkBox3, % clipRectangle);
				sender = gcnew RegionFinder(% frameGenerator->frameQueue);
				arduino = gcnew Arduino(checkBox4, % clipRectangle, serialPort, % sender->buffer);
				running = true;
				frameGenerator->StartThread();
				sender->StartThread();
				arduino->StartThread();
				statusTimer->Interval = 1000;
				statusHandler = gcnew System::EventHandler(this, &GUI::MainForm::CheckStatus);
				statusTimer->Tick += statusHandler;
				statusTimer->Start();
				toolStripStatusLabel1->Text = L"Status: running";
				toolStripStatusLabel1->ForeColor = Color::Green;
				button1->Text = L"Disconnect";
			}
			else
			{
				pin_ptr<const wchar_t> msg = PtrToStringChars(L"Cannot connect to port " + comboBox1->Text);
				MessageBoxW(static_cast<HWND>(Handle.ToPointer()), msg, L"Error", MB_ICONERROR);
				button1->Text = L"Connect";
			}
			button1->Enabled = true;
		}

	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e)
	{
		if (running)
		{
			Disconnect();
		}
		else
		{
			TryConnect();
		}
	}
	private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e)
	{
		DetectAndConnect();
	}
	private: System::Void comboBox2_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e)
	{
		if (running)
		{
			Disconnect();
			TryConnect();
		}
	}
	};
}
