#ifndef __COLORCLASSIFIER__H__ 
#define __COLORCLASSIFIER__H__ 

#include "GMM.h"

class ColorClassifier;
struct PixelClassifier
{
	ColorClassifier *mColorClassifier[4];
	float mClassifierP_c_x[4];		// probability
	float mClassifierWx[4];		// weight
	float mClassifierF_s_x[4];		// weight
	int mNumClassifiers;
	PixelClassifier()
	{
		mNumClassifiers = 0;
		
		mColorClassifier[0] = 0;
		mColorClassifier[1] = 0;
		mColorClassifier[2] = 0;
		mColorClassifier[3] = 0;


		mClassifierP_c_x[0] = 0;
		mClassifierP_c_x[1] = 0;
		mClassifierP_c_x[2] = 0;
		mClassifierP_c_x[3] = 0;

		mClassifierWx[0] = 0;
		mClassifierWx[1] = 0;
		mClassifierWx[2] = 0;
		mClassifierWx[3] = 0;

		mClassifierF_s_x[0] = 0;
		mClassifierF_s_x[1] = 0;
		mClassifierF_s_x[2] = 0;
		mClassifierF_s_x[3] = 0;
	}

};

class ColorClassifier
{
public:
	ColorClassifier(void);
	~ColorClassifier(void);
	void FreeGMM();

	int Build();
public:
	DataImage<PixelClassifier>*			mDataImagePixelClassifier;

	float				mBoundingBoxCenter[2];
	
	Mat					mImageF;
	Mat					mCombinedMask;
	Mat					mDistanceTransform;

	GMM*				mForegroundGMM;
	GMM*				mBackgroundGMM;
	float				mF_C;
};


#endif