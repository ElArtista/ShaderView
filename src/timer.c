#include "timer.h"
#include <windows.h>

long long get_timer_precision()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return freq.QuadPart;
}

long long get_timer_value()
{
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return count.QuadPart;
}

void sleep(long msec)
{
    Sleep(msec);
}
