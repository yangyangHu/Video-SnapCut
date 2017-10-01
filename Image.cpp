// Image.cpp

#include "Image.h"

Image::Image()
{
	mWidth = 0;
	mHeight = 0;
	mChannels = 0;	
	mData = 0;
}

Image::~Image()
{
	Free ();
}


void Image::Free()
{
	mWidth = 0;
	mHeight = 0;
	mChannels = 0;
	if(mData )
	{
		delete []mData ;
		mData = 0;
	}
}


int Image::GetWidth()
{
	return mWidth ;
}

int Image::GetHeight()
{
	return mHeight ;
}

int Image::GetChannels()
{
	return mChannels ;
}

unsigned char* Image::GetData()
{
	return mData ;
}

/*inline int Image::GetWidth()
{
	return mWidth ;
}

inline int Image::GetHeight()
{
	return mHeight ;
}

inline int Image::GetChannels()
{
	return mChannels ;
}

inline unsigned char* Image::GetData()
{
	return mData ;
}*/