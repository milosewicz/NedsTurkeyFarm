// bmp.cpp: member functions for the bmp file reader

// Copyright Ian Parberry, 1999
// Last updated May 22, 2000

#include "bmp.h" //header file

// constructors and destructors

CBmpFileReader::CBmpFileReader()  // constructor
    : m_cImage(NULL)              // memory not yet allocate
{                 
    // Initialize BITMAPFILEHEADER and BITMAPINFOHEADER members to zero
    ZeroMemory(&m_BMPFileHead, sizeof(m_BMPFileHead));
    ZeroMemory(&m_BMPFileInfo, sizeof(m_BMPFileInfo));

    // Optionally, initialize the palette to zero as well
    ZeroMemory(m_rgbPalette, sizeof(m_rgbPalette));
}

CBmpFileReader::~CBmpFileReader()
{                      // destructor
    delete[] m_cImage; // reclaim memory from image data
}

// member functions

BOOL CBmpFileReader::load(const char *filename)
{                     // load bmp from this file
    HANDLE hfile;     // input file handle
    DWORD actualRead; // number of bytes actually read
    int image_size;   // size of image (width*height)
    BOOL OK = TRUE;   // no error has occurred yet
    // open input file for reading
    hfile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
    // bail if file could not be opened
    if (hfile == INVALID_HANDLE_VALUE)
        return FALSE;

    // read the header and info structures
    OK = ReadFile(hfile, &m_BMPFileHead, sizeof(m_BMPFileHead), &actualRead, NULL);
    if (OK)
        OK = ReadFile(hfile, &m_BMPFileInfo, sizeof(m_BMPFileInfo), &actualRead, NULL);
    // make sure the bmp is 8-bit color
    if (OK)
        OK = m_BMPFileInfo.biBitCount == COLOR_DEPTH;
    // get the bmp palette
    if (OK)
        OK = ReadFile(hfile, m_rgbPalette, sizeof(m_rgbPalette), &actualRead, NULL);
    // bail out is something went wrong
    if (!OK)
    {
        CloseHandle(hfile);
        return FALSE;
    }
    // allocate memory for image data
    image_size = m_BMPFileInfo.biWidth * m_BMPFileInfo.biHeight;
    if (m_cImage)
        delete[] m_cImage;           // dispose of any old space

    m_cImage = new BYTE[image_size]; // allocate new space
    if (!m_cImage)
    { // bail if alloc failed
        CloseHandle(hfile);
        return FALSE;
    }

    // round up line width to next multiple of 4
    int width = (m_BMPFileInfo.biWidth + 3) & ~3;
    
    // read bmp image
    int i = 0;                                     // counter
    BYTE trash[4];                                 // to hold the trash at the end of each line
    int remainder = width - m_BMPFileInfo.biWidth; // width of trash
    while (OK && i < m_BMPFileInfo.biHeight)
    {
        // read data
        OK = OK && ReadFile(hfile, (BYTE *)(m_cImage + i * m_BMPFileInfo.biWidth),
                            m_BMPFileInfo.biWidth, &actualRead, NULL);
        // read trash at end of line
        OK = OK && ReadFile(hfile, trash, remainder, &actualRead, NULL);
        i++; // next line
    }
    
    if (!OK)
        delete[] m_cImage; // clean up if failed
    
    // close up and exit
    CloseHandle(hfile);
    return OK;
} // load

BOOL CBmpFileReader::draw(LPDIRECTDRAWSURFACE surface)
{
    DDSURFACEDESC ddsd; // direct draw surface descriptor
    BYTE *dest, *src;   // destination and source pointers
    int src_width;      // width of source

    // init surface descriptor
    memset(&ddsd, 0, sizeof(DDSURFACEDESC));
    ddsd.dwSize = sizeof(ddsd);

    // lock down surface
    if (FAILED(surface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL)))
        return FALSE;

    // calculate addresses and width
    dest = (BYTE *)ddsd.lpSurface; // destination
    src = m_cImage + ((m_BMPFileInfo.biHeight - 1) * m_BMPFileInfo.biWidth);

    // trim bmp if too wide
    if (m_BMPFileInfo.biWidth > ddsd.lPitch)
        src_width = ddsd.lPitch;
    else
        src_width = m_BMPFileInfo.biWidth;

    // move data to surface
    for (int i = 0; i < m_BMPFileInfo.biHeight; i++)
    {
        memcpy(dest, src, src_width);
        dest += ddsd.lPitch;
        src -= src_width;
    }

    // clean up and return
    surface->Unlock(NULL);
    return TRUE;
} // draw

BOOL CBmpFileReader::setpalette(LPDIRECTDRAWPALETTE palette)
{
    PALETTEENTRY pe[COLORS]; // intermediate palette
    // construct pe[]
    for (int i = 0; i < COLORS; i++)
    {                                             // for each palette entry
        pe[i].peRed = m_rgbPalette[i].rgbRed;     // set red
        pe[i].peGreen = m_rgbPalette[i].rgbGreen; // set green
        pe[i].peBlue = m_rgbPalette[i].rgbBlue;   // set blue
    }
    // load direct draw palette to surface
    palette->SetEntries(NULL, 0, COLORS, pe);
    return TRUE;
}
