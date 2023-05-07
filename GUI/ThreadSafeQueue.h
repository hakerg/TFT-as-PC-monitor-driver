#pragma once

using namespace System::Collections::Generic;
using namespace System::Threading;

template <class T>
ref class ThreadSafeQueue {
private:
    bool isEmptyNoLock() {
        return queue->Count == 0;
    }

    bool isFullNoLock() {
        return maxElements != 0 && queue->Count >= maxElements;
    }

public:
    int maxElements;

    ThreadSafeQueue() {
        this->maxElements = 0;
    }

    ThreadSafeQueue(int maxElements) {
        this->maxElements = maxElements;
    }

    bool TryPush(T value) {
        Monitor::Enter(syncRoot);
        try {
            if (isFullNoLock()) {
                return false;
            }
            queue->Enqueue(value);
            Monitor::Pulse(syncRoot);
            return true;
        }
        finally {
            Monitor::Exit(syncRoot);
        }
    }

    void WaitAndPush(T value) {
        Monitor::Enter(syncRoot);
        try {
            while (isFullNoLock()) {
                Monitor::Wait(syncRoot);
            }
            queue->Enqueue(value);
            Monitor::Pulse(syncRoot);
        }
        finally {
            Monitor::Exit(syncRoot);
        }
    }

    bool TryPop(T% value) {
        Monitor::Enter(syncRoot);
        try {
            if (isEmptyNoLock()) {
                return false;
            }
            value = queue->Dequeue();
            Monitor::Pulse(syncRoot); // Notify any waiting threads
            return true;
        }
        finally {
            Monitor::Exit(syncRoot);
        }
    }

    T WaitAndPop() {
        Monitor::Enter(syncRoot);
        try {
            while (isEmptyNoLock()) {
                Monitor::Wait(syncRoot);
            }
            T value = queue->Dequeue();
            Monitor::Pulse(syncRoot); // Notify any waiting threads
            return value;
        }
        finally {
            Monitor::Exit(syncRoot);
        }
    }

    bool Empty() {
        Monitor::Enter(syncRoot);
        try {
            return isEmptyNoLock();
        }
        finally {
            Monitor::Exit(syncRoot);
        }
    }

    int Size() {
        Monitor::Enter(syncRoot);
        try {
            return queue->Count;
        }
        finally {
            Monitor::Exit(syncRoot);
        }
    }

    bool isFull() {
        Monitor::Enter(syncRoot);
        try {
            return isFullNoLock();
        }
        finally {
            Monitor::Exit(syncRoot);
        }
    }

    int AvailableSpace() {
        return maxElements - Size();
    }

private:
    Queue<T>^ queue = gcnew Queue<T>();
    Object^ syncRoot = gcnew Object();
};