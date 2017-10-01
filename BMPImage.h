// BMPImage.h: interface for the BMPImage class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __BMPIMAGE_H
#define __BMPIMAGE_H

#include <stdio.h>
#include <Windows.h>
#include "Image.h"


class BMPImage  :public Image
{
public:
	int mStride;		// The TRUE number of bytes in a scan line (in a line of pixels in memory)
public:
	void Free();
	virtual bool LoadFromFile(const char *strFileName);
	BMPImage();
	virtual ~BMPImage();

};

#endif
