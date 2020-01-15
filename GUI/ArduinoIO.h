#pragma once
#include "Region.h"
#include "SerialPort.h"

using namespace System::Threading;

ref class ArduinoIO
{
public:

	static constexpr int width = 320;
	static constexpr int height = 240;

	static constexpr int plainConstTime = 103;
	static constexpr int plainPixelTime = 3;

	static constexpr int maxRegionCount = 200;
	static constexpr int regionSendSize = 5;
	static const int maxQueueSize = maxRegionCount * regionSendSize;
	static constexpr int inputWaitTime = 1000;

private:

	uint8_t* queue;
	int sendingID;
	int queueSize;
	Thread^ sendingThread;
	bool running;
	Mutex sizeMutex;
	bool mouseDown = false;

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

	void SendingProc();
	void WaitForK();
	void CheckTouchInput();
	void Send(int count);
	void SetCursor(int tx, int ty);

	void Push8(uint8_t v);
	void Push16(uint16_t v);

public:

	double clipTop = 0.0;
	double clipHeight = 1.0;
	double clipLeft = 0.0;
	double clipWidth = 1.0;
	bool enableTouch;

	SerialPort* serialPort;
	bool error;

	ArduinoIO(bool enableTouch);
	virtual ~ArduinoIO();
	void TryStart(LPCTSTR portName, long baudRate);
	void Stop();

	void Push(Region& region);
	int Size();
	int AvailableSpace();

	static int GetDrawingTime(int pixelCount);

	void NormalizeTouchCoord(int tx, int ty, double* nx, double* ny);
};
