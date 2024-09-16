// main.cpp

// Read 640x480x256 bmp & display full screen (ESC to exit)

// Copyright Ian Parberry, 1999
// Last updated May 22, 2000

// system includes
#include <ddraw.h>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>

// system defines
#define WIN32_LEAN_AND_MEAN

// custom includes
#include "bmp.h"
#include "defines.h"
#include "timer.h"

// globals
LPDIRECTDRAW lpDirectDrawObject = NULL; // direct draw object
LPDIRECTDRAWSURFACE lpPrimary = NULL;   // primary surface
LPDIRECTDRAWPALETTE lpPrimaryPalette;   // its palette
LPDIRECTDRAWSURFACE lpSecondary = NULL; // back buffer
LPDIRECTDRAWPALETTE lpSecondaryPalette; // its palette
BOOL ActiveApp;                         // is this application active?

CBmpFileReader frame0; // first frame
CBmpFileReader frame1; // second frame

BOOL appIsRunning = TRUE;

CTimer Timer; // game timer

// helper functions

LPDIRECTDRAWPALETTE CreatePalette(LPDIRECTDRAWSURFACE surface);
BOOL InitDirectDraw(HWND hwnd);
HWND CreateDefaultWindow(const char *name, HINSTANCE hInstance);

BOOL LoadImages()
{
    // draw the first image to the primary surface
    if (!frame0.load("up.bmp"))
        return FALSE;                         // read from file
    if (!frame0.setpalette(lpPrimaryPalette)) // set palette
        return FALSE;
    if (!frame0.draw(lpPrimary))
        return FALSE; // draw
    // draw the second image to the secondary surface
    if (!frame1.load("down.bmp"))
        return FALSE;                           // read from file
    if (!frame1.setpalette(lpSecondaryPalette)) // set palette
        return FALSE;
    if (!frame1.draw(lpSecondary))
        return FALSE; // draw
    // success exit
    return TRUE;
} // LoadImages

BOOL RestoreSurfaces()
{ // restore all surfaces
    BOOL result = TRUE;
    if (SUCCEEDED(lpPrimary->Restore()))
        result = result && frame0.draw(lpPrimary) &&  // redraw image
                 frame0.setpalette(lpPrimaryPalette); // set palette
    else
        return FALSE;
    if (SUCCEEDED(lpSecondary->Restore()))
        result = result && frame1.draw(lpSecondary) &&  // redraw image
                 frame1.setpalette(lpSecondaryPalette); // set palette
    else
        return FALSE;
    return result;
} // RestoreSurfaces

BOOL PageFlip()
{ // return TRUE if page flip succeeds
    if (lpPrimary->Flip(NULL, DDFLIP_WAIT) == DDERR_SURFACELOST)
        return RestoreSurfaces();
    return TRUE;
} // PageFlip

BOOL ComposeFrame()
{ // compose a frame of animation
    static int last_time = Timer.time();
    if (Timer.elapsed(last_time, 1000))
    {
        PageFlip();
    }
    return TRUE;
} // ComposeFrame

BOOL ProcessFrame()
{                      // process a frame of animation
    ComposeFrame();    // compose a frame in secondary surface
    return TRUE;
} // ProcessFrame

BOOL keyboard_handler(WPARAM keystroke)
{                        // keyboard handler
    BOOL result = FALSE; // return TRUE if game is to end
    switch (keystroke)
    {
    case VK_ESCAPE:
        result = TRUE;
        break; // exit game
    case VK_SPACE:
        // result = !PageFlip();
        break;
    }
    return result;
} // keyboard_handler

// message handler (window procedure)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_ACTIVATEAPP:
        ActiveApp = wParam;
        OutputDebugString("WM_ACTIVATEAPP received\n");
        break;
    case WM_CREATE:
        OutputDebugString("WM_CREATE received\n");
        break;
    case WM_KEYDOWN: // keyboard hit
        if (keyboard_handler(wParam))
        {
            OutputDebugString("DestroyWindow called from WM_KEYDOWN\n");
            DestroyWindow(hwnd);
        }
        break;
    case WM_CLOSE:
        OutputDebugString("WM_CLOSE received\n");
        break;
    case WM_DESTROY: // end of game
        OutputDebugString("WM_DESTROY received\n");
        PostQuitMessage(0); // exit
        break;
    default: // default window procedure
        return DefWindowProc(hwnd, message, wParam, lParam);
    } // switch(message)
    return 0L;
} // WindowProc

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;   // current message
    HWND hwnd; // handle to fullscreen window
    hwnd = CreateDefaultWindow("directX Demo 0", hInstance);
    if (!hwnd)
        return FALSE;
    
    // set up window (maybe show later)
    // ShowWindow(hwnd, nCmdShow);
    // UpdateWindow(hwnd);
    
    // Simulate Stack Overflow!
    // unsigned char BigOldBuffer[2 * 1024 * 1024] = {};

    SetFocus(hwnd);    // allow input from keyboard
    ShowCursor(FALSE); // hide the cursor
    // init graphics
    BOOL OK = InitDirectDraw(hwnd); // initialize DirectDraw
    if (OK)
        OK = LoadImages(); // load images from disk
    if (!OK)
    { // bail out if initialization failed
        DestroyWindow(hwnd);
        return FALSE;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // message loop
    while (appIsRunning)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message != WM_QUIT)
            {             
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                OutputDebugString("WM_QUIT\n");
                appIsRunning = FALSE;
            }
        }
        else
        {
            ProcessFrame();
        }
        //else if (!ActiveApp)
        //    WaitMessage();
        /*else
        {
            OutputDebugString("WM_QUIT\n");
            appIsRunning = FALSE;
        }*/
    }

    // Relese resources and quit
    if (lpDirectDrawObject != NULL)
    {                          // if DD object exists
        if (lpPrimary != NULL) // if primary surface exists
        {
            if (lpSecondary != NULL)       // if secondary surface exists
                lpSecondary->Release();    // release secondary surface
            if (lpPrimary != NULL)         // if primary surface exists
                lpPrimary->Release();      // release primary surface
        }
        //lpDirectDrawObject->Release(); // release DD object
        lpDirectDrawObject = NULL;     // set to NULL after release
    }
    ShowCursor(TRUE); // show the mouse cursor

    OutputDebugString("ENDE.\n");
    
    return 0;
} // WinMain
