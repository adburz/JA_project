#pragma once
#include <Windows.h>

class OperationTimer
{
    LARGE_INTEGER li;
    __int64 counterFrequency = 0;
    __int64 counterStart = 0;       
    __int64 counterEnd = 0;            
    __int64 counterLast = 0;       
    __int64 counterTicks = 0;    

public:
    OperationTimer();
    void start();
    void stop();

    __int64 getCounterTotalTicks() { return counterTicks; };
};