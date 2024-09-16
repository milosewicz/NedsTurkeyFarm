// timer.h, header file for timer.cpp
// Copyright Ian Parberry, 1999
// Last updated September 20, 1999

// system includes
#include <windows.h> //needed for BOOL

#ifndef __timer_h__
#define __timer_h__

class CTimer
{ // game timer class
  protected:
    int m_nStartTime; // time that timer was started
  public:
    CTimer();             // constructor
    virtual void start(); // start the timer
    virtual int time();   // return the time in ms
    // has interval ms elapsed since start?
    BOOL elapsed(int &start, int interval);
};

#endif
