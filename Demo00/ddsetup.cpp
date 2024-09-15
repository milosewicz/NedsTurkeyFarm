// ddsetup.cpp: directDraw setup and release
// Copyright Ian Parberry, 1999
// Last updated May 22, 2000

// system includes
#include <ddraw.h>
#include <windows.h>
#include <windowsx.h>

// custom includes
#include "defines.h"

// globals
extern LPDIRECTDRAW lpDirectDrawObject;      // direct draw object
extern LPDIRECTDRAWSURFACE lpPrimary;        // primary surface
extern LPDIRECTDRAWPALETTE lpPrimaryPalette; // its palette

// helper functions

LPDIRECTDRAWPALETTE CreatePalette(LPDIRECTDRAWSURFACE surface)
{
    // create surface palette
    PALETTEENTRY pe[COLORS];         // new palette
    LPDIRECTDRAWPALETTE lpDDPalette; // direct draw palette

    // construct pe[], set to black
    for (int i = 0; i < COLORS; i++)
        pe[i].peRed = pe[i].peGreen = pe[i].peBlue = 0;

    // create direct draw palette
    if (FAILED(lpDirectDrawObject->CreatePalette(DDPCAPS_8BIT, pe, &lpDDPalette, NULL)))
        return NULL;

    // load direct draw palette to surface
    surface->SetPalette(lpDDPalette);
    return lpDDPalette;
} // CreatePalette

BOOL InitDirectDraw(HWND hwnd)
{ // direct draw initialization
    // create and set up direct draw object
    if (FAILED(DirectDrawCreate(NULL, &lpDirectDrawObject, NULL)))
        return FALSE;
    // set cooperative level
    if (FAILED(lpDirectDrawObject->SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN)))
        return FALSE;
    // change screen resolution
    if (FAILED(lpDirectDrawObject->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_DEPTH)))
        return FALSE;

    // create the primary surface
    DDSURFACEDESC ddsd; // direct draw surface descriptor
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
    if (FAILED(lpDirectDrawObject->CreateSurface(&ddsd, &lpPrimary, NULL)))
        return FALSE;

    // create its palette
    lpPrimaryPalette = CreatePalette(lpPrimary);
    return TRUE;
} // InitDirectDraw

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// windows system functions
HWND CreateDefaultWindow(const char *name, HINSTANCE hInstance)
{
    WNDCLASS wc; // window registration info
    // register display window
    wc.style = CS_HREDRAW | CS_VREDRAW; // style
    wc.lpfnWndProc = WindowProc;        // window message handler
    wc.cbClsExtra = wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = name;
    RegisterClass(&wc);
    // create and set up fullscreen window
    return CreateWindowEx(WS_EX_TOPMOST, name, name, WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN),
                          GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);
}
