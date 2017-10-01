// Image.h: interface for the Image class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __IMAGE_H
#define __IMAGE_H


class Image  
{
public:
	int mWidth;		// The width of the bitmap
	int mHeight;		// The height of the bitmap
	int mChannels;	// How many channels is the bitmap (3 == 24-bit, 4 == 32-bit)	
	unsigned char *mData; // This is a pointer to the actual pixels of the bitmap

public:

	Image();
	virtual ~Image();

	int GetWidth();
	int GetHeight();
	int GetChannels();
	unsigned char* GetData();	
	
	virtual bool LoadFromFile(const char *strFileName) = 0;
	virtual void Free();
	
};

#endif



