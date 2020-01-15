#include "ArduinoIO.h"
#include <ctime>
#include <cmath>
#include <iostream>
#include <Windows.h>

using namespace System::Windows::Forms;

void ArduinoIO::SendingProc()
{
	WaitForK();
	while (running)
	{
		CheckTouchInput();
		if (queueSize == 0)
		{
			Sleep(1);
		}
		else
		{
			sizeMutex.WaitOne();
			int toSend = queueSize;
			int sendsToCheck = maxQueueSize - sendingID;
			sizeMutex.ReleaseMutex();
			if (toSend >= sendsToCheck && sendsToCheck)
			{
				Send(sendsToCheck);
				WaitForK();
			}
			else
			{
				Send(toSend);
			}
		}
	}
}

void ArduinoIO::WaitForK()
{
	char buffer;
	clock_t timeout = clock() + inputWaitTime;
	while (running)
	{
		if (serialPort->Read(&buffer, 1) == 1)
		{
			if (buffer == 'K')
			{
				return;
			}
			else if (buffer == 'T')
			{
				int16_t pos[2];
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

void ArduinoIO::CheckTouchInput()
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

void ArduinoIO::Send(int count)
{
	serialPort->WriteData((char*)&queue[sendingID], count);
	sizeMutex.WaitOne();
	sendingID = (sendingID + count) % maxQueueSize;
	queueSize -= count;
	sizeMutex.ReleaseMutex();
}

void ArduinoIO::SetCursor(int tx, int ty)
{
	std::cout << tx << ", " << ty << std::endl;
	if (tx == -1)
	{
		if (mouseDown)
		{
			if (enableTouch)
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
		if (enableTouch)
		{
			double x;
			double y;
			NormalizeTouchCoord(tx, ty, &x, &y);
			x = x * clipWidth + clipLeft;
			y = y * clipHeight + clipTop;
			Cursor::Current->Position = System::Drawing::Point((int)round(x), (int)round(y));
		}
		if (!mouseDown)
		{
			if (enableTouch)
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

ArduinoIO::ArduinoIO(bool enableTouch) : enableTouch(enableTouch)
{
	queue = new uint8_t[maxQueueSize];
}

ArduinoIO::~ArduinoIO()
{
	if (running)
	{
		Stop();
	}
	delete[] queue;
}

void ArduinoIO::TryStart(LPCTSTR portName, long baudRate)
{
	sendingID = 0;
	queueSize = 0;
	error = false;
	serialPort = new SerialPort(portName, baudRate);
	running = serialPort->IsConnected();
	sendingThread = gcnew Thread(gcnew ThreadStart(this, &ArduinoIO::SendingProc));
	sendingThread->Start();
}

void ArduinoIO::Stop()
{
	running = false;
	sendingThread->Join();
	delete serialPort;
}

void ArduinoIO::Push8(uint8_t v)
{
	while (!AvailableSpace())
	{
		sizeMutex.ReleaseMutex();
		Sleep(1);
		sizeMutex.WaitOne();
	}
	queue[(sendingID + queueSize) % maxQueueSize] = v;
	queueSize++;
}

void ArduinoIO::Push16(uint16_t v)
{
	Push8((uint8_t)v);
	Push8(v >> 8);
}

void ArduinoIO::Push(Region& region)
{
	sizeMutex.WaitOne();
	Push8(region.x);
	Push8(region.y);
	Push8(
		(region.x >> 8) |
		(((int)log2(region.sizeX) & 0xF) << 1) |
		((int)log2(region.sizeY) << 5)
	);
	Push16(region.color.To565());
	sizeMutex.ReleaseMutex();
}

int ArduinoIO::Size()
{
	return queueSize;
}

int ArduinoIO::AvailableSpace()
{
	return maxQueueSize - queueSize;
}

int ArduinoIO::GetDrawingTime(int pixelCount)
{
	return pixelCount * plainPixelTime + plainConstTime;
}

void ArduinoIO::NormalizeTouchCoord(int tx, int ty, double* nx, double* ny)
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