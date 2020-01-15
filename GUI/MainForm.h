#pragma once
#include "RegionPusher.h"
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
		MainForm(void) : screenData(ArduinoIO::width, ArduinoIO::height)
		{
			InitializeComponent();
			//
			//TODO: W tym miejscu dodaj kod konstruktora
			//
			bi = new BITMAPINFO;
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
				capturer->Join();
				pusher->Stop();
			}
			delete bi;
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
	private: System::Windows::Forms::ComboBox^ comboBox2;
	private: System::Windows::Forms::ComboBox^ comboBox3;
	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::Label^ label2;
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
			this->comboBox3 = (gcnew System::Windows::Forms::ComboBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->statusStrip1 = (gcnew System::Windows::Forms::StatusStrip());
			this->toolStripStatusLabel1 = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->checkBox4 = (gcnew System::Windows::Forms::CheckBox());
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
			this->groupBox1->Controls->Add(this->comboBox3);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->comboBox2);
			this->groupBox1->Controls->Add(this->checkBox1);
			this->groupBox1->Controls->Add(this->checkBox3);
			this->groupBox1->Controls->Add(this->checkBox2);
			this->groupBox1->Location = System::Drawing::Point(12, 93);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(275, 138);
			this->groupBox1->TabIndex = 6;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Settings";
			//
			// comboBox3
			//
			this->comboBox3->FormattingEnabled = true;
			this->comboBox3->Items->AddRange(gcnew cli::array< System::Object^  >(5) { L"1", L"2", L"4", L"8", L"16" });
			this->comboBox3->Location = System::Drawing::Point(169, 111);
			this->comboBox3->Name = L"comboBox3";
			this->comboBox3->Size = System::Drawing::Size(50, 21);
			this->comboBox3->TabIndex = 9;
			this->comboBox3->Text = L"16";
			this->comboBox3->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::comboBox2_SelectedIndexChanged);
			//
			// label3
			//
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(151, 114);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(12, 13);
			this->label3->TabIndex = 8;
			this->label3->Text = L"x";
			//
			// label2
			//
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(6, 114);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(83, 13);
			this->label2->TabIndex = 7;
			this->label2->Text = L"Max block size: ";
			//
			// comboBox2
			//
			this->comboBox2->FormattingEnabled = true;
			this->comboBox2->Items->AddRange(gcnew cli::array< System::Object^  >(7) { L"1", L"2", L"4", L"8", L"16", L"32", L"64" });
			this->comboBox2->Location = System::Drawing::Point(95, 111);
			this->comboBox2->Name = L"comboBox2";
			this->comboBox2->Size = System::Drawing::Size(50, 21);
			this->comboBox2->TabIndex = 6;
			this->comboBox2->Text = L"64";
			this->comboBox2->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::comboBox2_SelectedIndexChanged);
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
			this->statusStrip1->Location = System::Drawing::Point(0, 234);
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
			this->checkBox4->CheckedChanged += gcnew System::EventHandler(this, &MainForm::checkBox4_CheckedChanged);
			//
			// MainForm
			//
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(299, 256);
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

		RegionPusher^ pusher;
		HDC hDesktopDC;
		HDC hMyDC;
		HBITMAP hBitmap;
		BITMAPINFO* bi;
		Array2D<RGBQUAD> screenData;
		bool running = false;
		Thread^ capturer;
		Windows::Forms::Timer^ statusTimer = gcnew Windows::Forms::Timer;
		System::EventHandler^ statusHandler;

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

		void InitCapturing()
		{
			hDesktopDC = GetDC(NULL);
			hMyDC = CreateCompatibleDC(hDesktopDC);
			memset(bi, 0, sizeof(BITMAPINFO));
			bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bi->bmiHeader.biPlanes = 1;
			bi->bmiHeader.biBitCount = 32;
			bi->bmiHeader.biCompression = BI_RGB;
			bi->bmiHeader.biWidth = ArduinoIO::width;
			bi->bmiHeader.biHeight = -ArduinoIO::height;
			hBitmap = CreateCompatibleBitmap(hDesktopDC, ArduinoIO::width, ArduinoIO::height);
			SelectObject(hMyDC, hBitmap);
		}

		void FreeCapturing()
		{
			DeleteObject(hBitmap);
			ReleaseDC(NULL, hDesktopDC);
			DeleteDC(hMyDC);
		}

		void CaptureProc()
		{
			InitCapturing();
			while (running)
			{
				clock_t start = clock();
				if (checkBox2->Checked)
				{
					SetStretchBltMode(hMyDC, HALFTONE);
				}
				else
				{
					SetStretchBltMode(hMyDC, COLORONCOLOR);
				}
				int screenX = GetSystemMetrics(SM_CXSCREEN);
				int screenY = GetSystemMetrics(SM_CYSCREEN);
				if (checkBox1->Checked)
				{
					POINT cursor;
					GetCursorPos(&cursor);
					pusher->arduino.clipWidth = ArduinoIO::width;
					pusher->arduino.clipHeight = ArduinoIO::height;
					if (cursor.x < pusher->arduino.clipLeft)
					{
						pusher->arduino.clipLeft = cursor.x;
					}
					else if (cursor.x > pusher->arduino.clipLeft + pusher->arduino.clipWidth)
					{
						pusher->arduino.clipLeft = cursor.x - pusher->arduino.clipWidth;
					}
					if (cursor.y < pusher->arduino.clipTop)
					{
						pusher->arduino.clipTop = cursor.y;
					}
					else if (cursor.y > pusher->arduino.clipTop + pusher->arduino.clipHeight)
					{
						pusher->arduino.clipTop = cursor.y - pusher->arduino.clipHeight;
					}
					pusher->arduino.clipWidth = ArduinoIO::width;
					pusher->arduino.clipHeight = ArduinoIO::height;
					BitBlt(hMyDC, 0, 0, ArduinoIO::width, ArduinoIO::height, hDesktopDC,
						(int)pusher->arduino.clipLeft, (int)pusher->arduino.clipTop, SRCCOPY);
				}
				else
				{
					pusher->arduino.clipLeft = 0;
					pusher->arduino.clipTop = 0;
					pusher->arduino.clipWidth = screenX;
					pusher->arduino.clipHeight = screenY;
					StretchBlt(hMyDC, 0, 0, ArduinoIO::width, ArduinoIO::height, hDesktopDC,
						0, 0, screenX, screenY, SRCCOPY);
				}
				GetDIBits(hMyDC, hBitmap, 0, ArduinoIO::height,
					screenData.data, bi, DIB_RGB_COLORS);
				if (checkBox3->Checked)
				{
					for (int y = 0; y < ArduinoIO::height; y++)
					{
						for (int x = 0; x < ArduinoIO::width; x++)
						{
							pusher->target.At(x, y) = Color16(screenData.At(x, y), x, y);
						}
					}
				}
				else
				{
					for (int y = 0; y < ArduinoIO::height; y++)
					{
						for (int x = 0; x < ArduinoIO::width; x++)
						{
							pusher->target.At(x, y) = Color16(screenData.At(x, y));
						}
					}
				}
				Sleep((clock() - start) / 2);
			}
			FreeCapturing();
		}

		void CheckStatus(Object^, EventArgs^)
		{
			if (pusher->arduino.error)
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
			capturer->Join();
			pusher->Stop();
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
			pusher = gcnew RegionPusher(Int32::Parse(comboBox2->Text), Int32::Parse(comboBox3->Text), checkBox4->Checked);
			pusher->TryStart(port, 2000000);
			if (pusher->arduino.serialPort->IsConnected())
			{
				running = true;
				capturer = gcnew Thread(gcnew ThreadStart(this, &MainForm::CaptureProc));
				capturer->Start();
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
				pusher->Stop();
				MessageBoxW(static_cast<HWND>(Handle.ToPointer()), L"Cannot connect to the port", L"Error", MB_ICONERROR);
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
	private: System::Void checkBox4_CheckedChanged(System::Object^ sender, System::EventArgs^ e)
	{
		pusher->arduino.enableTouch = checkBox4->Checked;
	}
	};
}
