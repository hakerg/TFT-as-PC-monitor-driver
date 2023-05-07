#pragma once
#include "Arduino.h"
#include "ArduinoScreen.h"
#include "Array2D.h"
#include "ClipRectangle.h"
#include "ThreadSafeQueue.h"
#include "ThreadWrapper.h"
#include <Windows.h>

using namespace System::Windows::Forms;

ref class FrameGenerator : public ThreadWrapper {
private:
	HDC hDesktopDC;
	HDC hMyDC;
	HBITMAP hBitmap;
	BITMAPINFO* bi;
	Array2D<RGBQUAD, ArduinoScreen::width, ArduinoScreen::height>* screenData;
	CheckBox^ zoomEnabled;
	CheckBox^ hqEnabled;
	CheckBox^ ditherEnabled;

	void InitCapturing()
	{
		bi = new BITMAPINFO;
		hDesktopDC = GetDC(NULL);
		hMyDC = CreateCompatibleDC(hDesktopDC);
		memset(bi, 0, sizeof(BITMAPINFO));
		bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi->bmiHeader.biPlanes = 1;
		bi->bmiHeader.biBitCount = 32;
		bi->bmiHeader.biCompression = BI_RGB;
		bi->bmiHeader.biWidth = ArduinoScreen::width;
		bi->bmiHeader.biHeight = -ArduinoScreen::height;
		hBitmap = CreateCompatibleBitmap(hDesktopDC, ArduinoScreen::width, ArduinoScreen::height);
		SelectObject(hMyDC, hBitmap);
		screenData = new Array2D<RGBQUAD, ArduinoScreen::width, ArduinoScreen::height>();
	}

	void FreeCapturing()
	{
		delete screenData;
		DeleteObject(hBitmap);
		ReleaseDC(NULL, hDesktopDC);
		DeleteDC(hMyDC);
		delete bi;
	}

protected:
	// Odziedziczono za poœrednictwem elementu ThreadWrapper
	void ThreadLoop() override {
		InitCapturing();
		while (running)
		{
			clock_t start = clock();
			if (hqEnabled->Checked)
			{
				SetStretchBltMode(hMyDC, HALFTONE);
			}
			else
			{
				SetStretchBltMode(hMyDC, COLORONCOLOR);
			}
			int screenX = GetSystemMetrics(SM_CXSCREEN);
			int screenY = GetSystemMetrics(SM_CYSCREEN);
			if (zoomEnabled->Checked)
			{
				POINT cursor;
				GetCursorPos(&cursor);
				clipRectangle->width = ArduinoScreen::width;
				clipRectangle->height = ArduinoScreen::height;
				if (cursor.x < clipRectangle->left)
				{
					clipRectangle->left = cursor.x;
				}
				else if (cursor.x > clipRectangle->left + clipRectangle->width)
				{
					clipRectangle->left = cursor.x - clipRectangle->width;
				}
				if (cursor.y < clipRectangle->top)
				{
					clipRectangle->top = cursor.y;
				}
				else if (cursor.y > clipRectangle->top + clipRectangle->height)
				{
					clipRectangle->top = cursor.y - clipRectangle->height;
				}
				clipRectangle->width = ArduinoScreen::width;
				clipRectangle->height = ArduinoScreen::height;
				BitBlt(hMyDC, 0, 0, ArduinoScreen::width, ArduinoScreen::height, hDesktopDC,
					(int)clipRectangle->left, (int)clipRectangle->top, SRCCOPY);
			}
			else
			{
				clipRectangle->left = 0;
				clipRectangle->top = 0;
				clipRectangle->width = screenX;
				clipRectangle->height = screenY;
				StretchBlt(hMyDC, 0, 0, ArduinoScreen::width, ArduinoScreen::height, hDesktopDC,
					0, 0, screenX, screenY, SRCCOPY);
			}
			GetDIBits(hMyDC, hBitmap, 0, ArduinoScreen::height,
				screenData->Data(), bi, DIB_RGB_COLORS);

			ArduinoScreen^ target = gcnew ArduinoScreen;
			if (ditherEnabled->Checked)
			{
				for (int y = 0; y < ArduinoScreen::height; y++)
				{
					for (int x = 0; x < ArduinoScreen::width; x++)
					{
						target->At(x, y) = gcnew Color16(screenData->At(x, y), x, y);
					}
				}
			}
			else
			{
				for (int y = 0; y < ArduinoScreen::height; y++)
				{
					for (int x = 0; x < ArduinoScreen::width; x++)
					{
						target->At(x, y) = gcnew Color16(screenData->At(x, y));
					}
				}
			}
			frameQueue.WaitAndPush(target);
		}
		FreeCapturing();
	}

public:
	ClipRectangle^ clipRectangle;
	ThreadSafeQueue<ArduinoScreen^> frameQueue;

	FrameGenerator(CheckBox^ zoomEnabled, CheckBox^ hqEnabled, CheckBox^ ditherEnabled, ClipRectangle^ clipRectangle) : frameQueue(1),
		zoomEnabled(zoomEnabled), hqEnabled(hqEnabled), ditherEnabled(ditherEnabled), clipRectangle(clipRectangle) {}
};