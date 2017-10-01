#ifndef __VIDEOREADER__H__
#define __VIDEOREADER__H__

#include <string>   // for strings


#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

using namespace std;
using namespace cv;

class VideoReader
{
public:
	string			mFileName;
	VideoCapture	mVideoCapture;
	Size			mVideoSize;
	int				mNumFrames;
	int				mCurFrame;

public:
	VideoReader(void);
	~VideoReader(void);

	int Open(const string FileName);
	int GetNextFrame(Mat& Frame);
	int GetPrevFrame(Mat& Frame);
	int GoToFrame(int FrameNo, Mat& Frame);
	int GoToFirstFrame(Mat& Frame);
	int GoToLastFrame(Mat& Frame);
	int GetFrame(int FrameNo, Mat& Frame);
	int Close();
};

#endif