// timer.cpp, the timer class
// Copyright Ian Parberry, 1999
// Last updated September 20, 1999

#include "timer.h"

CTimer::CTimer()
{ // constructor
    m_nStartTime = 0;
}

void CTimer::start()
{ // start the timer
    m_nStartTime = timeGetTime();
}

int CTimer::time()
{ // return the time
    return timeGetTime() - m_nStartTime;
}

int CTimer::elapsed(int &start, int interval)
{
    // has interval elapsed from start?
    int current_time = time();
    if (current_time >= start + interval)
    {
        start = current_time;
        return TRUE;
    }
    else
        return FALSE;
}
