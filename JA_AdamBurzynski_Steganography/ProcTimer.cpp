#include "ProcTimer.h"

OperationTimer::OperationTimer()
{
    QueryPerformanceFrequency(&li);
    counterFrequency = li.QuadPart;
}

void OperationTimer::start()
{
    QueryPerformanceCounter(&li);

    counterStart = li.QuadPart;
    counterLast = li.QuadPart;
    counterTicks = 0;
}

void OperationTimer::stop()
{
    QueryPerformanceCounter(&li);

    counterTicks += li.QuadPart - counterLast;
    counterEnd = li.QuadPart;
    counterLast = li.QuadPart;
}