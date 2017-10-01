/*
 * GrabCut implementation source code Copyright(c) 2005-2006 Justin Talbot
 *
 * All Rights Reserved.
 * For educational use only; commercial use expressly forbidden.
 * NO WARRANTY, express or implied, for this software.
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O


#include "Global.h"
#include "BMPImage.h"

using namespace cv;

// ImageDatas, really just a templatized 2D array. We use this for all the image variables.

template<class T>
class DataImage
{

public:
	DataImage();
	DataImage(unsigned int width, unsigned int height);
	void Initialize(unsigned int width, unsigned int height);
	~DataImage();

	T* ptr() { return m_image; }

	T& operator() (int x, int y) { clampX(x); clampY(y); return m_image[y*mWidth+x]; }
	const T& operator() (int x, int y) const { clampX(x); clampY(y); return m_image[y*mWidth+x]; }

	void fillRectangle(int x1, int y1, int x2, int y2, const T& t);
	void fill(const T& t);

	unsigned int width() const { return mWidth; }
	unsigned int height() const { return mHeight; }

	void CreateDataImage( BMPImage* srcImage ) ;
	void CreateDataImage( Mat* srcImage ) ;

private:

	void clampX(int& x) const { if (x < 0) x = 0; if (x >= (int)mWidth)  x = mWidth-1; }
	void clampY(int& y) const { if (y < 0) y = 0; if (y >= (int)mHeight) y = mHeight-1; }



	T* m_image;
	unsigned int mWidth, mHeight;
};



// DataImage member functions

// DataImage member functions
template<class T>
DataImage<T>::DataImage() : mWidth(0), mHeight(0), m_image(0)
{
	
}

template<class T>
DataImage<T>::DataImage(unsigned int width, unsigned int height) : mWidth(width), mHeight(height)
{
	m_image = new T[mWidth*mHeight];
}

template<class T>
void DataImage<T>::Initialize(unsigned int width, unsigned int height)
{
	mWidth = width;
	mHeight = height;
	m_image = new T[mWidth*mHeight];
}



template<class T>
DataImage<T>::~DataImage()
{
	if (m_image)
		delete [] m_image;
}

template<class T>
void DataImage<T>::CreateDataImage( BMPImage* srcImage )
{
	unsigned int width, height;

	width = srcImage->mWidth;
	height = srcImage->mHeight;
	float maximum = 255;
	
	int r, g, b;
	int index2;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			

			index2 = (i*srcImage->mStride)+j*3;

			r = srcImage->mData[index2+0];
			g = srcImage->mData[index2+1];
			b = srcImage->mData[index2+2];
			
			Real R = 0, G = 0, B = 0;
			R = (Real)(r)/maximum;
			G = (Real)(g)/maximum;
			B = (Real)(b)/maximum;
			
			(*this)(j, (height-1)-i) = Color(R,G,B);
		}
	}


}

template<class T>
void DataImage<T>::CreateDataImage( Mat* srcImage )
{
	unsigned int width, height;

	
	width = srcImage->cols;
	height = srcImage->rows;
	float maximum = 255;
	
	int r, g, b;
	int index2;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			
			Vec3b intensity = srcImage->at<Vec3b>(y, x);
			b = intensity.val[0];
			g = intensity.val[1];
			r = intensity.val[2];


			Real R = 0, G = 0, B = 0;
			R = (Real)(r)/maximum;
			G = (Real)(g)/maximum;
			B = (Real)(b)/maximum;
			
			(*this)(x, y) = Color(R,G,B);
		}
	}


}


template<class T>
void DataImage<T>::fillRectangle(int x1, int y1, int x2, int y2, const T& t)
{
	clampX(x1); clampY(y1);
	clampX(x2); clampY(y2);
	
	if(y1>y2) {int t=y1; y1=y2; y2=t;}
	if(x1>x2) {int t=x1; x1=x2; x2=t;}

	for (int i = y1; i <= y2; ++i)
	{
		for (int j = x1; j <= x2; ++j)
		{
			m_image[i*mWidth+j] = t;
		}
	}
}

template<class T>
void DataImage<T>::fill(const T& t)
{
	for (unsigned int i = 0; i < mWidth*mHeight; ++i) 
	{
		m_image[i] = t;
	}
}

#endif