#pragma once
#include "ArduinoBuffer.h"
#include "ArduinoScreen.h"
#include "ClipRectangle.h"
#include "Region.h"
#include "SerialPort.h"
#include "ThreadWrapper.h"
#include <ctime>
#include <cmath>
#include <iostream>
#include <Windows.h>

using namespace System::Windows::Forms;
using namespace System::Threading;

ref class Arduino : public ThreadWrapper
{
public:
	static constexpr int maxRegionCount = 200;
	static constexpr int inputWaitTime = 1000;

private:
	ClipRectangle^ clipRectangle;
	bool mouseDown = false;
	CheckBox^ touchEnabled;
	int regionsToK;

	double touchLeft = 4000.0;
	double touchRight = 550.0;
	double touchTopA = -300.0;
	double touchTopB = -150.0;
	double touchTopC = 2050.0;
	double touchMiddleA = 0.0;
	double touchMiddleB = -100.0;
	double touchMiddleC = 2200.0;
	double touchBottomA = -300.0;
	double touchBottomB = 350.0;
	double touchBottomC = 2550.0;

	void WaitForK()
	{
		char buffer;
		clock_t timeout = clock() + inputWaitTime;
		while (running)
		{
			if (serialPort->Read(&buffer, 1) == 1)
			{
				if (buffer == 'K')
				{
					regionsToK = maxRegionCount;
					return;
				}
				else if (buffer == 'T')
				{
					int16_t pos[2]{};
					if (serialPort->Read((char*)pos, 4) == 4)
					{
						SetCursor(pos[0], pos[1]);
					}
					else
					{
						error = true;
					}
				}
				else
				{
					error = true;
				}
			}
			if (clock() > timeout)
			{
				error = true;
				break;
			}
		}
	}

	void CheckTouchInput()
	{
		char buffer[5];
		if (serialPort->Available() >= 5)
		{
			if (serialPort->Read(buffer, 5))
			{
				int16_t* pos = (int16_t*)(buffer + 1);
				SetCursor(pos[0], pos[1]);
			}
			else
			{
				error = true;
			}
		}
	}

	void SetCursor(int tx, int ty)
	{
		std::cout << tx << ", " << ty << std::endl;
		if (tx == -1)
		{
			if (mouseDown)
			{
				if (touchEnabled->Checked)
				{
					INPUT input = { 0 };
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					SendInput(1, &input, sizeof(INPUT));
				}
				mouseDown = false;
			}
		}
		else
		{
			if (touchEnabled->Checked)
			{
				double x;
				double y;
				NormalizeTouchCoord(tx, ty, &x, &y);
				x = x * clipRectangle->width + clipRectangle->left;
				y = y * clipRectangle->height + clipRectangle->top;
				Cursor::Current->Position = System::Drawing::Point((int)round(x), (int)round(y));
			}
			if (!mouseDown)
			{
				if (touchEnabled->Checked)
				{
					INPUT input = { 0 };
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					SendInput(1, &input, sizeof(INPUT));
				}
				mouseDown = true;
			}
		}
	}

public:
	SerialPort* serialPort;
	bool error;
	ArduinoBuffer^ buffer;

	Arduino(CheckBox^ touchEnabled, ClipRectangle^ clipRectangle, SerialPort* serialPort, ArduinoBuffer^ buffer) : ThreadWrapper("Arduino"),
		touchEnabled(touchEnabled), clipRectangle(clipRectangle), serialPort(serialPort), buffer(buffer)
	{
		error = false;
		regionsToK = 0;
	}

	void NormalizeTouchCoord(int tx, int ty, double* nx, double* ny)
	{
		*nx = (tx - touchLeft) / (touchRight - touchLeft);
		double touchTop = touchTopA * (*nx) * (*nx) + touchTopB * (*nx) + touchTopC;
		double touchMiddle = touchMiddleA * (*nx) * (*nx) + touchMiddleB * (*nx) + touchMiddleC;
		double touchBottom = touchBottomA * (*nx) * (*nx) + touchBottomB * (*nx) + touchBottomC;
		double c = touchTop;
		double b = 4.0 * touchMiddle - 3.0 * c - touchBottom;
		double a = touchBottom - c - b;
		c -= ty;
		double sqrtDelta = sqrt(b * b - 4 * a * c);
		*ny = (-b + sqrtDelta) / (a * 2.0);
		return;
	}

protected:
	// Odziedziczono za poœrednictwem elementu ThreadWrapper
	void ThreadLoop() override
	{
		while (running)
		{
			if (regionsToK == 0) {
				WaitForK();
			}

			CheckTouchInput();

			int regionCount = buffer->Size();
			if (regionCount > 0) {
				regionCount = min(regionCount, regionsToK);
				if (regionCount > 0) {
					uint8_t temp[1000]{};
					for (int i = 0; i < regionCount; i++) {
						buffer->Pop(&temp[i * Region::bytesSize]);
					}
					serialPort->WriteData((char*)temp, regionCount * Region::bytesSize);
					regionsToK -= regionCount;
				}
			}
			else {
				Sleep(1);
			}
		}
	}
};
