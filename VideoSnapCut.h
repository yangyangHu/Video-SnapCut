#ifndef __VIDEOSNAPCUT__H__ 
#define __VIDEOSNAPCUT__H__ 

#include "DataImage.h"
#include "Color.h"
#include "DataStructures.h"
#include "ColorClassifier.h"
#include "DataImage.h"
#include "VideoReader.h"

#include <vector>
using namespace std;



class VideoSnapCut
{
public:
	VideoSnapCut(void);
	~VideoSnapCut(void);
	void FreeColorClassifiers();
	
	void FreeDataImagePixelClassifier();
	
	void SetForegroundBorderPolygons(vector<PolygonF*>* ForegroundBorderPolygons);

	void SetVideoReader(VideoReader* VideoReader);

	void SetFilesPath(vector<string>* FilesPath);

	void SetCurFrame(int CurFrame);

	void SetCurFrameMat(Mat CurFrameMat);

	int BuildColorClassifiers();

	int ClassifyPixels();

	void RegisterCurFrameWithFrame(int iFrame);

	void CreateMasks();
	Mat CreateMask(const PolygonF* polygon);
	Mat CreateContour(const Mat& Mask);
	Mat CreateDistanceTransforms(const Mat& Border);
	Mat CreatSampleContourImage(const Mat& image);
	void ClearMasks();
	bool PointInPolygon(const PolygonF* polygon, float y, float x) ;
	void DisplayImage(char* name, const Mat& mat, int channels = 1, bool multiply = true);
	void ImageSave(char* name, const Mat& mat, int channels = 1, bool multiply = true);//´æ´¢Í¼Ïñ
	void ImageProbabilitySave(char* name, const Mat& mat, int channels = 1, bool multiply = true);//´æ´¢Í¼Ïñ
	void InputImageSave(char* name, const Mat& mat, int channels = 1, bool multiply = true);//´æ´¢Í¼Ïñ
	void DisplayDistanceImage(char* name, const Mat& mat, int channels = 1, bool multiply = true);
	void ClearBorderSamples();		
	void SampleContour();
	void DisplayBorderSamples(int i);
	void ResampleContour();
public:	
	vector<PolygonF*>					mForegroundSampledBorderPolygons;	
	vector<PolygonF*>*					mForegroundBorderPolygons;
	vector<vector<ColorClassifier*>>	mColorClassifiers;
	DataImage<PixelClassifier>*			mDataImagePixelClassifier;
	VideoReader*						mVideoReader;	
	vector<string>*						mFilesPath;
	int									mCurFrame;
	int									mMediaMode;			//1 singel image, 2 video, 3 folder
	
	Mat									mImage;
	Mat									mImageF;
	vector<Mat>							mMasks;
	Mat									mCombinedMask;
	vector<Mat>							mBorders;
	vector<Mat>							mDistanceTransforms;
	vector<Mat>							mSampledContourImages;
	
public:
	static int		mMaskSize;
	static int		mMaskSize2; // /2
	static float	mSigmaS;
	static float	mSigmaS2;
	static int		mK;
	static int		mMaxColorClassifiers;
	
};


#endif