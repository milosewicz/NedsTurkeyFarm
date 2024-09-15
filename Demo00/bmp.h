// bmp.h: header file for the bmp file reader

// Copyright Ian Parberry, 1999
// Last updated September 2, 1999

#include <ddraw.h>
#include <windows.h>
#include <windowsx.h>

#include "defines.h"

#ifndef __bmp_h__
#define __bmp_h__

class CBmpFileReader
{ // bmp file input class
  protected:
    BITMAPFILEHEADER m_BMPFileHead; // bmp header
    BITMAPINFOHEADER m_BMPFileInfo; // bmp file information
    RGBQUAD m_rgbPalette[COLORS];   // the palette
    BYTE *m_cImage;                 // the image
  public:
    CBmpFileReader();                             // constructor
    ~CBmpFileReader();                            // destructor
    BOOL load(const char *filename);                    // load from file
    BOOL draw(LPDIRECTDRAWSURFACE surface);       // draw image
    BOOL setpalette(LPDIRECTDRAWPALETTE palette); // set palette
};

#endif
