#include <iostream>
#include <string>
#include <thread>
#include "RegionPusher.h"

bool running = true;
RegionPusher* sender;
HDC hDesktopDC;
HDC hMyDC;
HBITMAP hBitmap;
BITMAPINFO bi;
RGBQUAD screenData[RegionPusher::height][RegionPusher::width];
string port;

void InitCapturing()
{
	hDesktopDC = GetDC(NULL);
	hMyDC = CreateCompatibleDC(hDesktopDC);
	memset(&bi, 0, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biWidth = RegionPusher::width;
	bi.bmiHeader.biHeight = -RegionPusher::height;
	hBitmap = CreateCompatibleBitmap(hDesktopDC, RegionPusher::width, RegionPusher::height);
	SelectObject(hMyDC, hBitmap);
	SetStretchBltMode(hMyDC, COLORONCOLOR);
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
		StretchBlt(hMyDC, 0, 0, RegionPusher::width, RegionPusher::height, hDesktopDC,
			0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SRCCOPY);
		GetDIBits(hMyDC, hBitmap, 0, RegionPusher::height, screenData, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
		for (int y = 0; y < RegionPusher::height; y++)
		{
			for (int x = 0; x < RegionPusher::width; x++)
			{
				sender->target[y][x] = Color16(screenData[y][x]);
			}
		}
		Sleep((clock() - start) / 2);
	}
	FreeCapturing();
}

void Benchmark()
{
	ArduinoIO arduino(port.c_str(), 2000000);
	Sleep(1000);
	while (true)
	{
		for (int pc = 1; pc <= 128 * 128; pc *= 2)
		{
			for (int m = 0; m < Region::modeCount; m++)
			{
				int sx = min(128, pc);
				int sy = pc / sx;
				cout << "Mode " << m << " pixelcount " << pc << ": ";
				clock_t duration = clock();
				int count = -1;
				while (clock() <= duration + 5000 || count <= 0)
				{
					Region region(
						rand() % (RegionPusher::width - sx + 1),
						rand() % (RegionPusher::height - sy + 1),
						(Region::Mode)m, sx, sy);
					for (int i = 0; i < region.colorCount; i++)
					{
						region.colorData[i].Set({ (BYTE)rand(), (BYTE)rand(), (BYTE)rand() });
					}
					arduino.Push(region);
					count++;
				}
				duration = (clock() - duration) * 1000 / count;
				cout << duration << " micros\n";
			}
		}
	}
}

int main()
{
	cout << "Enter port\n";
	cin >> port;

	sender = new RegionPusher(port.c_str(), 2000000);

	if (sender->arduino.serialPort->IsConnected())
	{
		cout << "Connected\n";

		thread capturer(CaptureProc);

		system("PAUSE");

		running = false;
		capturer.join();
	}
	else
	{
		cout << "Connection error\n";
		system("PAUSE");
	}
	delete sender;
}
