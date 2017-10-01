// BMPImage.cpp

#include "BMPImage.h"


BMPImage::BMPImage()
{

}

BMPImage::~BMPImage()
{
	Free ();
}

bool BMPImage::LoadFromFile(const char *strFileName)
{
	Free();

	// Error Check -- Make sure they passed in a valid file name
	if(!strFileName)
		return false;

	FILE *File = fopen(strFileName, "rb");

	// Error Check -- Make sure the file could be opened
	if(!File)
		return false;

	BITMAPFILEHEADER fileheader;

	// Read the BITMAPFILEHEADER
	if(!fread(&fileheader, sizeof(BITMAPFILEHEADER), 1, File))
	{
		fclose(File);
			return false;
	}

	// Check the type field to make sure we have a .bmp file
	if(memcmp(&fileheader.bfType, "BM", 2))
	{
		fclose(File);
			return false;
	}

	BITMAPINFOHEADER infoheader;
	
	// Read the BITMAPINFOHEADER.
	if(!fread(&infoheader, sizeof(BITMAPINFOHEADER), 1, File))
	{
		fclose(File);
		return false;
	}

	// We only support 24-bit and 32-bit .bmps so make sure that's what we have
	if((infoheader.biBitCount != 24) && (infoheader.biBitCount != 32))
	{
		fclose(File);
		return false;
	}

	mWidth = infoheader.biWidth;
	mHeight = infoheader.biHeight;
	mChannels = infoheader.biBitCount / 8;

	// Set stride -- The stride is the TRUE number of bytes in a line of pixels
	// Windows makes all the .bmps DWORD aligned (divisible evenly by 4)
	// So if you bitmap say was 103x103 pixels, Windows would add 1 "padding byte" to it
	// so in memory it would be 104x103 pixels.  The "padding bytes" do not get blit (drawn)
	// to the screen, they're just there so again everything is DWORD aligned which makes 
	// blitting (drawing to the screen) easier for the OS
	mStride = mWidth * mChannels;

	while((mStride % 4) != 0) // Ensure bmp_stride is DWORD aligned
		mStride++;
	
	mData = new unsigned char [mStride*mHeight];

	// Jump to the location where the pixel data is stored
	if(fseek(File, fileheader.bfOffBits, SEEK_SET))
	{
		fclose(File);
		return false;
	}
	
	unsigned int bytesPerLine = mWidth * mChannels; // Bytes per line (number of bytes in a scan line)
	
	// Calculate how many "padding" bytes there are -- WE DO NOT want to read in the
	// padding bytes (we will just skip over those)
	// **Remember** Windows adds padding bytes to ensure ALL .bmps are DWORD aligned
	// (divisible evenly by 4)
	unsigned int padding = mStride - bytesPerLine;

	// Loop over all the scan lines (all the rows of pixels in the image)
	for(int y = 0; y < mHeight; y++)
	{
		
		// Read the precise number of bytes that the scan line requires into the bitmap
		if(!fread(&mData[mStride * y], bytesPerLine, 1, File))
		{
			fclose(File);
				return false;
		}

		// Skip over any padding bytes.
		if(fseek(File, padding, SEEK_CUR))
		{
			fclose(File);
				return false;
		}

	}

	//move over the pixels and swap the red and blue components 
	//so that it will RGB instead of BGR
	unsigned char temp;
	int size = mWidth * mHeight*mChannels;

	for(int r=0;r<mHeight;r++)
	{
		for(int c = 0;c<mWidth;c++)
		{
			int px = r * mStride + c*3; 
			temp = mData [px+2];
			mData [px+2] = mData [px];
			mData [px] = temp;	
		}
	}




	fclose(File);
	return true; // If we get here .bmp was read in successfully
}

void BMPImage::Free()
{
	Image ::Free ();
	mStride = 0;
}
