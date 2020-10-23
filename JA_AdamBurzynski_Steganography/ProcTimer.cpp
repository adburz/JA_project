#include "ProcTimer.h"

OperationTimer::OperationTimer()
{
    QueryPerformanceFrequency(&li);
    counterFrequency = li.QuadPart;
}

void OperationTimer::start()
{
    QueryPerformanceCounter(&li);
    //init start, last and total ticks
    counterStart = li.QuadPart;
    counterLast = li.QuadPart;
    counterTotalTicks = 0;
}

void OperationTimer::stop()
{
    QueryPerformanceCounter(&li);
    //increase total ticks, set end
    counterTotalTicks += li.QuadPart - counterLast;
    counterEnd = li.QuadPart;
    counterLast = li.QuadPart;
}

void OperationTimer::pause()
{
    QueryPerformanceCounter(&li);
    //increase total ticks, set counter last
    counterTotalTicks += li.QuadPart - counterLast;
    counterLast = li.QuadPart;
}

void OperationTimer::resume()
{
    QueryPerformanceCounter(&li);
    //set counterLast
    counterLast = li.QuadPart;
}