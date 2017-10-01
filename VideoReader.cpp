#include "VideoReader.h"

//http://opencv.willowgarage.com/documentation/cpp/c++_cheatsheet.html
VideoReader::VideoReader(void)
{
	
}

VideoReader::~VideoReader(void)
{
}


int VideoReader::Open(const string FileName)
{
	mFileName = FileName;
	mVideoCapture.open(mFileName);
    if ( !mVideoCapture.isOpened() )
    {   
		return 0;
    }
	mVideoSize = Size((int) mVideoCapture.get(CV_CAP_PROP_FRAME_WIDTH),
		(int) mVideoCapture.get(CV_CAP_PROP_FRAME_HEIGHT));

	mNumFrames = (int)mVideoCapture.get(CV_CAP_PROP_FRAME_COUNT);

	mCurFrame = -1;

	return 1;	
	
}

int VideoReader::Close()
{
    if ( mVideoCapture.isOpened() )
    {   
		mVideoCapture.release();
		return 1;
    }
	return 0;
}

int VideoReader::GetNextFrame(Mat& Frame)
{	
	int curFrame = (int)mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);

	if(curFrame<mNumFrames-1)
	{
		mVideoCapture >> Frame;
	}
	else
	{
		GoToLastFrame(Frame);
	}
	if( Frame.empty() )
	{
		return 0;
	}


	mCurFrame = (int)mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);
	
	return 1;
}

int VideoReader::GetPrevFrame(Mat& Frame)
{
	int curFrame = (int)mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);
	if(curFrame>1)
	{
		curFrame-=2;
	}
	else
	{
		curFrame = 0;
	}
	int ret = (int)mVideoCapture.set(CV_CAP_PROP_POS_FRAMES, curFrame);

	mVideoCapture >> Frame;

	if( Frame.empty() )
	{
		return 0;
	}

	
	mCurFrame = (int)mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);

	return 1;
}
int VideoReader::GoToFrame(int FrameNo, Mat& Frame)
{
	FrameNo = FrameNo<0?0:FrameNo>mNumFrames-1?mNumFrames-1:FrameNo;
	int ret = (int)mVideoCapture.set(CV_CAP_PROP_POS_FRAMES, FrameNo);

	mVideoCapture >> Frame;

	if( Frame.empty() )
	{
		return 0;
	}

	
	mCurFrame = (int)mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);

	return 1;

}

int VideoReader::GetFrame(int FrameNo, Mat& Frame)
{
	int oldFrameNo = (int)mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);
	FrameNo = FrameNo<0?0:FrameNo>mNumFrames-1?mNumFrames-1:FrameNo;
	int ret = (int)mVideoCapture.set(CV_CAP_PROP_POS_FRAMES, FrameNo);

	mVideoCapture >> Frame;

	if( Frame.empty() )
	{
		return 0;
	}
	
	
	ret = (int)mVideoCapture.set(CV_CAP_PROP_POS_FRAMES, oldFrameNo);
	
	mCurFrame = (int)mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);


	return 1;

}


int VideoReader::GoToFirstFrame(Mat& Frame)
{
	int ret = (int)mVideoCapture.set(CV_CAP_PROP_POS_FRAMES, 0);

	mVideoCapture >> Frame;

	if( Frame.empty() )
	{
		return 0;
	}

	
	mCurFrame = (int)mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);

	return 1;

}
int VideoReader::GoToLastFrame(Mat& Frame)
{
	int lastFrame = mNumFrames-1;

	int ret = (int)mVideoCapture.set(CV_CAP_PROP_POS_FRAMES, lastFrame);

	mVideoCapture >> Frame;

	if( Frame.empty() )
	{
		return 0;
	}
	
	mCurFrame = (int)mVideoCapture.get(CV_CAP_PROP_POS_FRAMES);

	return 1;

}
