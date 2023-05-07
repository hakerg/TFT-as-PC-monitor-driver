#pragma once

using namespace System;
using namespace System::Threading;

public ref class ThreadWrapper abstract
{
public:
    ThreadWrapper()
    {
        running = false;
    }

    virtual ~ThreadWrapper()
    {
        StopThread();
    }

    void StartThread()
    {
        running = true;
        thread = gcnew Thread(gcnew ThreadStart(this, &ThreadWrapper::ThreadLoop));
        thread->Start();
    }

    void StopThread()
    {
        running = false;
        if (thread != nullptr)
        {
            thread->Join();
            delete thread;
            thread = nullptr;
        }
    }

protected:
    bool running;

    virtual void ThreadLoop() = 0;

private:
    Thread^ thread;
};