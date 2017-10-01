#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "stdafx.h"
#include "assert.h"
#include "time.h"
#include "stdlib.h"
#include "App.h"
#include "RightView.h"

#include "VideoSnapCut.h"


extern "C" 
{
	#include "sift.h"
	#include "imgfeatures.h"
	#include "kdtree.h"
	#include "utils.h"
	#include "xform.h"
}
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>


int		VideoSnapCut::mMaskSize = 30;
int		VideoSnapCut::mMaskSize2 = VideoSnapCut::mMaskSize/2;
float	VideoSnapCut::mSigmaS = 5;
float	VideoSnapCut::mSigmaS2 = mSigmaS*mSigmaS;
int		VideoSnapCut::mK = 3;
int		VideoSnapCut::mMaxColorClassifiers = 4; 

VideoSnapCut::VideoSnapCut(void)
{
	mDataImagePixelClassifier = 0;	
}

VideoSnapCut::~VideoSnapCut(void)
{
	FreeColorClassifiers();
}


void VideoSnapCut::FreeColorClassifiers()
{
	DataImage<PixelClassifier>& dataImagePixelClassifier = *mDataImagePixelClassifier;
	for(uint i=0;i<mColorClassifiers.size();i++)
	{
		vector<ColorClassifier*>& colorClassifiers = mColorClassifiers[i];
		for(uint i=0;i<colorClassifiers.size();i++)
		{
			delete colorClassifiers[i];
		}		
		colorClassifiers.clear();
	}
	mColorClassifiers.clear();

	for(int r=0;r<(int)mImageF.rows;r++)
	{
		for(int c = 0;c<(int)mImageF.cols;c++)
		{
			int& n = dataImagePixelClassifier(c,r).mNumClassifiers;
			n = 0;
		}//for(int r=0;r<(int)mImageF.rows;r++)
	}//for(int c = 0;c<(int)mImageF.cols;c++)
}

void VideoSnapCut::FreeDataImagePixelClassifier()
{
	if(mDataImagePixelClassifier)
	{
		delete mDataImagePixelClassifier;
	}
}

void VideoSnapCut::SetForegroundBorderPolygons(vector<PolygonF*>* ForegroundBorderPolygons)
{ 
	mForegroundBorderPolygons = ForegroundBorderPolygons;
}

void VideoSnapCut::SetVideoReader(VideoReader* VideoReader)
{
	mVideoReader = VideoReader; 
	mMediaMode = 2;
}

void VideoSnapCut::SetFilesPath(vector<string>* FilesPath)
{
	mFilesPath = FilesPath; 
	mMediaMode = 3;
}

void VideoSnapCut::SetCurFrame(int CurFrame)
{
	mCurFrame = CurFrame;
}

void VideoSnapCut::SetCurFrameMat(Mat CurFrameMat)
{
	mImage.release();

	mImage = CurFrameMat;
	
	mImageF.release();
	
	mImage.convertTo(mImageF,CV_32FC3,1.0 / 255.0);

	FreeDataImagePixelClassifier();
	mDataImagePixelClassifier = new DataImage<PixelClassifier>(mImageF.cols,mImageF.rows);
}


int VideoSnapCut::BuildColorClassifiers()
{
	DataImage<PixelClassifier>& dataImagePixelClassifier = *mDataImagePixelClassifier;
	
	FreeColorClassifiers();

	for(uint i=0;i<mSampledContourImages.size();i++)
	{
		Mat& sampledContourImages = mSampledContourImages[i];
		vector<ColorClassifier*> colorClassifiers;
		for(int r=0;r<(int)sampledContourImages.rows;r++)
		{
			for(int c = 0;c<(int)sampledContourImages.cols;c++)
			{
				int& pix = sampledContourImages.at<int>(r, c);
				if(pix != -1)
				{

					ColorClassifier* colorClassifier = new ColorClassifier();
					
					colorClassifier->mImageF = mImageF;
					colorClassifier->mDataImagePixelClassifier = mDataImagePixelClassifier;
					colorClassifier->mCombinedMask = mCombinedMask;
					colorClassifier->mDistanceTransform = mDistanceTransforms[i];
					colorClassifier->mBoundingBoxCenter[0] = (float)c;
					colorClassifier->mBoundingBoxCenter[1] = (float)r;
					
					if(colorClassifier->Build())
					{
						pix = colorClassifiers.size();
						colorClassifiers.push_back(colorClassifier);
					}
					else //if(colorClassifier->Build())
					{
						pix = -1;
						delete colorClassifier;
					}//else //if(colorClassifier->Build())
				}
			}//for(int c = 0;c<(int)sampledContourImages.cols;c++)
		}//for(int r=0;r<(int)sampledContourImages.rows;r++)
	
		mColorClassifiers.push_back(colorClassifiers);
		//DisplayBorderSamples(i);
	}//for(uint i=0;i<mSampledContourImages.size();i++)

	return 1; 
}

void VideoSnapCut::DisplayBorderSamples(int i)
{
	char windowName[256];
	sprintf(windowName,"Sampled contour before %d",i+1);
		
	DisplayImage(windowName,mBorders[i]);
	Mat& sampledContourImages = mSampledContourImages[i];
	
	Mat borderAfter;	
	cvtColor(mBorders[i], borderAfter, CV_GRAY2RGB );
	borderAfter.convertTo(borderAfter,CV_8UC3,255);
	
	for(int r=0;r<(int)borderAfter.rows;r++)
	{
		for(int c = 0;c<(int)borderAfter.cols;c++)
		{
			int pix = sampledContourImages.at<int>(r, c);
			if(pix != -1)
			{				
				Vec3b& intensity = borderAfter.at<Vec3b>(r, c);
				intensity.val[0] = 0;
				intensity.val[1] = 0;
				intensity.val[2] = 255;

			}			
		}
	}

	sprintf(windowName,"Sampled contour after %d",i+1);
	IplImage img = borderAfter;
	cvNamedWindow( windowName, 1 );
	cvShowImage( windowName, &img );
	char filename[256];
	sprintf(filename, "output\\%s.jpg", windowName);
	cvSaveImage(filename, &img);

	borderAfter.release();	

}
int VideoSnapCut::ClassifyPixels()
{
	DisplayImage("InputImage",mImage, 3, false);
	InputImageSave("InputImage",mImage, 3, false);

	DataImage<PixelClassifier>& dataImagePixelClassifier = *mDataImagePixelClassifier;

	Mat maskImageProb(mCombinedMask.size(), CV_32FC1);


	Mat maskImage = mImage.clone();

	for(int r=0;r<(int)mCombinedMask.rows;r++)
	{
		for(int c = 0;c<(int)mCombinedMask.cols;c++)
		{
			maskImageProb.at<float>(r,c) = 0.0;
		}
	}

	uchar* tempData =  new uchar[mImageF.cols*mImageF.rows*3];
	for(int r=0;r<(int)mImageF.rows;r++)
	{
		for(int c = 0;c<(int)mImageF.cols;c++)
		{
			int numClassifiers = dataImagePixelClassifier(c,r).mNumClassifiers;
			Vec3f intensity = mImageF.at<Vec3f>(r, c);
			Color pix(intensity.val[2],intensity.val[1],intensity.val[0]);
			for(int i = 0;i<numClassifiers;i++)
			{
				ColorClassifier* colorClassifiers = dataImagePixelClassifier(c,r).mColorClassifier[i];

				float Pc_xF = colorClassifiers->mForegroundGMM->p(pix);
				float Pc_xB = colorClassifiers->mBackgroundGMM->p(pix);
				float Pc_x = Pc_xF / (Pc_xF + Pc_xB);

				dataImagePixelClassifier(c,r).mClassifierP_c_x[i] = Pc_x;		// probability

				float dXCen = (r-colorClassifiers->mBoundingBoxCenter[1])*(r-colorClassifiers->mBoundingBoxCenter[1])+
						(c-colorClassifiers->mBoundingBoxCenter[0])*(c-colorClassifiers->mBoundingBoxCenter[0]);
				dXCen = sqrt(dXCen);
				
				dataImagePixelClassifier(c,r).mClassifierWx[i] = 1.0f/(dXCen + Epsilon);

				float D_x = colorClassifiers->mDistanceTransform.at<float>(r,c);
				float D_x2 = D_x*D_x;
				float F_x = 1 - exp(-D_x2/mSigmaS2);
				dataImagePixelClassifier(c,r).mClassifierF_s_x[i] = F_x;

			}
			int index1 = (r*mImageF.cols+c)*3;			
			
			pix.r *= 255; pix.r = pix.r < 0 ? 0 : pix.r > 255 ? 255 : pix.r;
			pix.g *= 255; pix.g = pix.g < 0 ? 0 : pix.g > 255 ? 255 : pix.g;
			pix.b *= 255; pix.b = pix.b < 0 ? 0 : pix.b > 255 ? 255 : pix.b;
			
			if(dataImagePixelClassifier(c,r).mNumClassifiers == 0)
			{
				if(mCombinedMask.at<uchar>(r, c)== 1)
				{

					tempData[index1+0] = 255;
					tempData[index1+1] = 255;
					tempData[index1+2] = 255;

					float t1 = 0.1, t2 = 1-t1;
					tempData[index1+0] = t1*255 + t2*pix.b;
					tempData[index1+1] = t1*255 + t2*pix.g;
					tempData[index1+2] = t1*255 + t2*pix.r;

					//tempData[index1+0] = 0.9*pix.b;
					//tempData[index1+1] = 0.9*pix.g;
					//tempData[index1+2] = 0.9*pix.r;

					maskImageProb.at<float>(r,c) = 1.0;

					maskImage.at<Vec3b>(r, c) = Vec3b(pix.b, pix.g, pix.r);
				}
				else
				{
					tempData[index1+0] = 0;
					tempData[index1+1] = 0;
					tempData[index1+2] = 0;

					//float t1 = 0.5, t2 = 1-t1;
					//tempData[index1+0] = t1*0 + t2*pix.b;
					//tempData[index1+1] = t1*0 + t2*pix.g;
					//tempData[index1+2] = t1*0 + t2*pix.r;

					//tempData[index1+0] = 0.0*pix.b;
					//tempData[index1+1] = 0.0*pix.g;
					//tempData[index1+2] = 0.0*pix.r;
					maskImageProb.at<float>(r,c) = 0.0;

					maskImage.at<Vec3b>(r, c) = Vec3b(tempData[index1+0], tempData[index1+1], tempData[index1+2]);

				}
			}
			else
			{

				float Pf_x = 0;
				float Wf_x = 0;
				for(int i = 0;i<numClassifiers;i++)
				{
					int L_x = 0;
					if(mCombinedMask.at<uchar>(r, c) == 1)
					{
						L_x = 1;
					}
					float F_c_x = dataImagePixelClassifier(c,r).mClassifierF_s_x[i];
					float P_c_x = dataImagePixelClassifier(c,r).mClassifierP_c_x[i];
					float P_F_X = F_c_x * L_x + (1-F_c_x)*P_c_x;
					Wf_x += dataImagePixelClassifier(c,r).mClassifierWx[i];
					Pf_x += P_F_X*dataImagePixelClassifier(c,r).mClassifierWx[i];
				}

				Pf_x /= Wf_x;

				maskImageProb.at<float>(r,c) = Pf_x;

				//	debug mClassifierF_s_x
				//Pf_x = dataImagePixelClassifier(c,r).mClassifierF_s_x[0];
				//int Pf_xColor = Pf_x * 255;
				//Pf_xColor = Pf_xColor < 0 ? 0 : Pf_xColor > 255 ? 255 : Pf_xColor;

				//	debug mF_C
				//int classifierID = dataImagePixelClassifier(c,r).mClassifierIDs[0];
				//ColorClassifier* classifier = mColorClassifiers[classifierID];
				//Pf_xColor = classifier->mF_C * 255;
				//Pf_xColor = Pf_xColor < 0 ? 0 : Pf_xColor > 255 ? 255 : Pf_xColor;

				//Pf_x *= 255; Pf_x = Pf_x < 0 ? 0 : Pf_x > 255 ? 255 : Pf_x;


				int Pf_xColor = (int)(Pf_x * 255);
				Pf_xColor = Pf_xColor < 0 ? 0 : Pf_xColor > 255 ? 255 : Pf_xColor;
				tempData[index1+0] = Pf_xColor;
				tempData[index1+1] = Pf_xColor;
				tempData[index1+2] = Pf_xColor;

				float t1 = 0.1, t2 = 1-t1;
				tempData[index1+0] = t1*Pf_xColor + t2*pix.b;
				tempData[index1+1] = t1*Pf_xColor + t2*pix.g;
				tempData[index1+2] = t1*Pf_xColor + t2*pix.r;

				maskImage.at<Vec3b>(r, c) = Vec3b(pix.b*Pf_x, pix.g*Pf_x, pix.r*Pf_x);

				//tempData[index1+0] = Pf_x*pix.b;
				//tempData[index1+1] = Pf_x*pix.g;
				//tempData[index1+2] = Pf_x*pix.r;
			}
			
		}
	}

	Mat maskImageProbChar = mCombinedMask.clone();

	for(int r=0;r<(int)mCombinedMask.rows;r++)
	{
		for(int c = 0;c<(int)mCombinedMask.cols;c++)
		{
			//float x = maskImageProb.at<float>(r,c);
			int pixelValue = maskImageProb.at<float>(r,c)*255;
			pixelValue = pixelValue<0?0:pixelValue>255?255:pixelValue;
			maskImageProbChar.at<uchar>(r,c) = pixelValue;
		}
	}


	DisplayImage("ProbilityMap",maskImageProbChar,1, false);
	//cvSaveImage("C:\\Users\\Lenovo\\Desktop\\cut\\1.jpg",&maskImage);
	ImageProbabilitySave("ProbilityMap",maskImageProbChar,1, false);
	ImageSave("MaskOutputImage",maskImage, 3, false);
	DisplayImage("MaskOutputImage",maskImage, 3, false);
	

	((CApp*)AfxGetApp())->mRightView->SetImageData(mImageF.cols, mImageF.rows, tempData, false,true);		
	delete []tempData;
	
	return 1;

}

void VideoSnapCut::RegisterCurFrameWithFrame(int iFrame)
{
	//	get frame I(t)
	//IplImage tempImg1 = mImage;
	IplImage img1 = mImage;
	
	//	get frame I(t+1)
	IplImage img2;
	Mat nextFrameMat;
	if(mMediaMode == 2)
	{
		mVideoReader->GetFrame(iFrame, nextFrameMat);
		img2 = nextFrameMat;
	}
	else if(mMediaMode == 3)
	{
		string path = (*mFilesPath)[iFrame];
		nextFrameMat = imread(path.data());
		img2 = nextFrameMat;
	}

	struct feature* feat1, * feat2, * feat;
	struct feature** nbrs;
	struct kd_node* kd_root;
	CvPoint pt1, pt2;
	double d0, d1;
	int n1, n2, k, i, m = 0;

	IplImage* stacked = stack_imgs( &img1, &img2 );

	n1 = sift_features( &img1, &feat1 );

	n2 = sift_features( &img2, &feat2 );
	
	//	select features inside the object
	int n1In = 0;
	for( i = 0; i < n1; i++ )
	{
		feat = feat1 + i;
		int x = (int)feat->x;
		int y = (int)feat->y;
		if(mCombinedMask.at<uchar>(y, x) == 1)
		{
			feat1[n1In] = (*feat);
			n1In++;
		}
	}

	//fprintf( stderr, "Building kd tree...\n" );
	kd_root = kdtree_build( feat2, n2 );
	
	for( i = 0; i < n1In; i++ )
	{
		feat = feat1 + i;
		k = kdtree_bbf_knn( kd_root, feat, 2, &nbrs, KDTREE_BBF_MAX_NN_CHKS );
		if( k == 2 ) 
		{
			d0 = descr_dist_sq( feat, nbrs[0] );
			d1 = descr_dist_sq( feat, nbrs[1] );
			if( d0 < d1 * NN_SQ_DIST_RATIO_THR )
			{
				pt1 = cvPoint( cvRound( feat->x ), cvRound( feat->y ) );
				pt2 = cvPoint( cvRound( nbrs[0]->x ), cvRound( nbrs[0]->y ) );
				pt2.y += img1.height;
				cvLine( stacked, pt1, pt2, CV_RGB(255,0,255), 1, 8, 0 );
				m++;
				feat1[i].fwd_match = nbrs[0];
			}
		}
		free( nbrs );
	}

	fprintf( stderr, "Found %d total matches\n", m );
	//display_big_img( stacked, "Matches" );
	//cvWaitKey( 0 );


	//UNCOMMENT BELOW TO SEE HOW RANSAC FUNCTION WORKS

	//Note that this line above:

	//feat1[i].fwd_match = nbrs[0];

	//is important for the RANSAC function to work.

	float matData[9] = {1,0,0, 0,1,0, 0,0,1};
	Mat transform(3, 3, CV_32FC1, matData);

	if(n1In>0)
	{
		CvMat* xfrom;
		xfrom = ransac_xform( feat1, n1In, FEATURE_FWD_MATCH, lsq_homog, 4, 0.01, homog_xfer_err, 3.0, NULL, NULL );
		if( xfrom )
		{	
			transform = cvarrToMat(xfrom);
		}
	}


	CvSize size;
	size.width = mImageF.cols;
	size.height = mImageF.rows;

	Mat mImageT1;
	warpPerspective(mImage, mImageT1, transform, size, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, BORDER_CONSTANT,cvScalarAll( 0 ));
	
	mImage.release();
	mImage = nextFrameMat.clone();
	//DisplayImage("Transformed int image",mImageT1, 3,false);
	
	Mat mImageFT1;	
	warpPerspective(mImageF, mImageFT1, transform, size, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, BORDER_CONSTANT,cvScalarAll( 0 ));
	
	mImageF.release();	
	mImage.convertTo(mImageF,CV_32FC3,1.0 / 255.0);

	//DisplayImage("Transformed float image",mImageFT1,3,false);

	for(uint i=0;i<mMasks.size();i++)
	{
		char windowName[256];
		
		CvSize size;
		size.width = mMasks[i].cols;
		size.height = mMasks[i].rows;
		
		Mat maskT1;
		warpPerspective(mMasks[i], maskT1, transform, size, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, BORDER_CONSTANT,cvScalarAll( 0 ));
		mMasks[i].release();
		mMasks[i] = maskT1;

		sprintf(windowName,"Transformed mask %d",i+1);
	//	DisplayImage(windowName,maskT1);

		Mat borderT1 = CreateContour(maskT1);
		mBorders[i].release();
		mBorders[i] = borderT1;

		sprintf(windowName,"Transformed border %d",i+1);
	//	DisplayImage(windowName,borderT1);

		Mat distanceTransformT1 = CreateDistanceTransforms(borderT1);
		mDistanceTransforms[i].release();
		mDistanceTransforms[i] = distanceTransformT1;

		sprintf(windowName,"Transformed distance transform %d",i+1);
	//	DisplayDistanceImage(windowName,distanceTransformT1);

		Mat sampledContourImageT1;		
		warpPerspective(mSampledContourImages[i], sampledContourImageT1, transform, size, CV_INTER_NN + CV_WARP_FILL_OUTLIERS, BORDER_CONSTANT,cvScalarAll( -1 ));
		
		mSampledContourImages[i].release();
		mSampledContourImages[i] = sampledContourImageT1;
		//DisplayBorderSamples(i);


	}

	Mat combinedMaskT1;
	warpPerspective(mCombinedMask, combinedMaskT1, transform, size, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, BORDER_CONSTANT,cvScalarAll( 0 ));
	//DisplayImage("Transformed combined mask",combinedMaskT1);
	mCombinedMask.release();
	mCombinedMask = combinedMaskT1;

	//BuildColorClassifiers();
	mCurFrame++;
		

}

void VideoSnapCut::ClearMasks()
{
	for(uint i=0;i<mMasks.size();i++)
	{
		mMasks[i].release();
	}
	mMasks.clear();
	mCombinedMask.release();

	for(uint i=0;i<mBorders.size();i++)
	{
		mBorders[i].release();
	}
	mBorders.clear();

	for(uint i=0;i<mDistanceTransforms.size();i++)
	{
		mDistanceTransforms[i].release();
	}
	mDistanceTransforms.clear();

	for(uint i=0;i<mSampledContourImages.size();i++)
	{
		mSampledContourImages[i].release();
	}
	mSampledContourImages.clear();
}

bool VideoSnapCut::PointInPolygon(const PolygonF* polygon, float y, float x) 
{
	bool oddNodes=false;
		
	int polygonSize = polygon->mPoints.size();
	int      i, j=polygonSize-1;
	
	for (i=0; i<polygonSize; i++) 
	{    
		if ((polygon->mPoints[i].y< y && polygon->mPoints[j].y>=y 
			|| polygon->mPoints[j].y< y && polygon->mPoints[i].y>=y) 
			&& (polygon->mPoints[i].x<=x || polygon->mPoints[j].x<=x)) 
		{
			oddNodes^=
				(polygon->mPoints[i].x+(y-polygon->mPoints[i].y)/
				(polygon->mPoints[j].y-polygon->mPoints[i].y)*
				(polygon->mPoints[j].x-polygon->mPoints[i].x)<x); 
		}
		j=i; 
	}

	return oddNodes;
}

void VideoSnapCut::DisplayImage(char* name, const Mat& mat, int channels/* = 1*/, bool multiply/* = true*/)
{
	Mat clonedMat = mat.clone();
	IplImage img = clonedMat;
	for( int y=0; y<img.height; y++ ) 
	{ 
		uchar* dest = (uchar*) ( img.imageData + y * img.widthStep ); 
						
		for( int x=0; x<img.width; x++ ) 
		{ 		
			for(int c=0;c<channels;c++)
			{
				if(multiply)
				{
					dest[x*channels + c] *= 255;
				}
			}
			
		}
	}
	cvNamedWindow( name, 1 );
	cvShowImage( name, &img );
	char filename[256];
	sprintf(filename, "output\\%s.jpg", name);
	cvSaveImage(filename, &img);
	clonedMat.release();
}

void VideoSnapCut::ImageSave(char* name, const Mat& mat, int channels/* = 1*/, bool multiply/* = true*/)
{
	Mat clonedMat = mat.clone();
	IplImage img = clonedMat;
	for( int y=0; y<img.height; y++ ) 
	{ 
		uchar* dest = (uchar*) ( img.imageData + y * img.widthStep ); 

		for( int x=0; x<img.width; x++ ) 
		{ 		
			for(int c=0;c<channels;c++)
			{
				if(multiply)
				{
					dest[x*channels + c] *= 255;
				}
			}

		}
	}
	
	char filename[256];
	sprintf(filename, "D:\\研究生期间各类文档\\数字图像处理\\第一轮\\数据\\分割结果\\cut\\%d.jpg", mCurFrame+1);
	cvSaveImage(filename, &img);
	clonedMat.release();
}

void VideoSnapCut::ImageProbabilitySave(char* name, const Mat& mat, int channels/* = 1*/, bool multiply/* = true*/)
{
	Mat clonedMat = mat.clone();
	IplImage img = clonedMat;
	for( int y=0; y<img.height; y++ ) 
	{ 
		uchar* dest = (uchar*) ( img.imageData + y * img.widthStep ); 

		for( int x=0; x<img.width; x++ ) 
		{ 		
			for(int c=0;c<channels;c++)
			{
				if(multiply)
				{
					dest[x*channels + c] *= 255;
				}
			}

		}
	}

	char filename[256];
	sprintf(filename, "D:\\研究生期间各类文档\\数字图像处理\\第一轮\\数据\\分割结果\\cut\\p%d.jpg", mCurFrame+1);
	cvSaveImage(filename, &img);
	clonedMat.release();
}

void VideoSnapCut::InputImageSave(char* name, const Mat& mat, int channels/* = 1*/, bool multiply/* = true*/)
{
	Mat clonedMat = mat.clone();
	IplImage img = clonedMat;
	for( int y=0; y<img.height; y++ ) 
	{ 
		uchar* dest = (uchar*) ( img.imageData + y * img.widthStep ); 

		for( int x=0; x<img.width; x++ ) 
		{ 		
			for(int c=0;c<channels;c++)
			{
				if(multiply)
				{
					dest[x*channels + c] *= 255;
				}
			}

		}
	}

	char filename[256];
	sprintf(filename, "D:\\研究生期间各类文档\\数字图像处理\\第一轮\\数据\\分割结果\\cut\\I%d.jpg", mCurFrame+1);
	cvSaveImage(filename, &img);
	clonedMat.release();
}

void VideoSnapCut::DisplayDistanceImage(char* name, const Mat& mat, int channels/* = 1*/, bool multiply/* = true*/)
{
	float maxDist = 0;
	Mat clonedMat = mat.clone();
	IplImage img = clonedMat;

	for( int y=0; y<img.height; y++ ) 
	{ 
		float* dest = (float*) ( img.imageData + y * img.widthStep ); 
						
		for( int x=0; x<img.width; x++ ) 
		{ 		
			if(dest[x]>maxDist)
			{
				maxDist = dest[x];
			}		
			
		}
	}

	for( int y=0; y<img.height; y++ ) 
	{ 
		float* dest = (float*) ( img.imageData + y * img.widthStep ); 
						
		for( int x=0; x<img.width; x++ ) 
		{ 		
			dest[x] /= maxDist;
			dest[x]  = dest[x]*0.9f + 1*0.1f; 
			
			
		}
	}
	cvNamedWindow( name, 1 );
	cvShowImage( name, &img );

	for( int y=0; y<img.height; y++ ) 
	{ 
		float* dest = (float*) ( img.imageData + y * img.widthStep ); 
						
		for( int x=0; x<img.width; x++ ) 
		{ 		
			dest[x] *= 255;			
		}
	}

	char filename[256];
	sprintf(filename, "output\\%s.jpg", name);
	cvSaveImage(filename, &img);

	clonedMat.release();
}


void VideoSnapCut::CreateMasks()
{
	ClearMasks();

	vector<PolygonF*>&		foregroundBorderPolygons =  *mForegroundBorderPolygons;

	char windowName[256];

	CvSize size;
	
	size.width = mImageF.cols;
	size.height = mImageF.rows;

	mCombinedMask.create(size, CV_8UC1);
	mCombinedMask = mCombinedMask.zeros(size, CV_8UC1);

	for(uint ii=0;ii<foregroundBorderPolygons.size();ii++)
	{
		int polygonSize = foregroundBorderPolygons[ii]->mPoints.size();
		if(polygonSize<2)
		{
			continue;
		}

		PolygonF* polygon = foregroundBorderPolygons[ii];
		
		Mat mask = CreateMask(polygon);

		mMasks.push_back(mask);

		
		sprintf(windowName,"Mask %d",ii);
		DisplayImage(windowName, mask);		

		Mat border = CreateContour(mask);

		mBorders.push_back(border);
		
		sprintf(windowName,"border %d",ii);
		DisplayImage(windowName, border);

		Mat distanceTransform = CreateDistanceTransforms(border);

		mDistanceTransforms.push_back(distanceTransform);


		sprintf(windowName,"distanceTransforms %d",ii);
		DisplayDistanceImage(windowName, distanceTransform,1,false);

		Mat sampledContourImage = CreatSampleContourImage(mImageF);		
		mSampledContourImages.push_back(sampledContourImage);


	}//for(int ii=0;ii<mForegroundBorderPolygons->size();ii++)

	
	sprintf(windowName,"Combined Mask");
	DisplayImage(windowName, mCombinedMask);

	SampleContour();

	
	
}

Mat VideoSnapCut::CreateMask(const PolygonF* Polygon)
{
	CvSize size;
	size.width = mImageF.cols;
	size.height = mImageF.rows;

	Mat mask(size, CV_8UC1);
			
	for(int y=0;y<mask.rows;y++)
	{
		for(int x = 0;x<mask.cols;x++)
		{
			mask.at<uchar>(y, x) = PointInPolygon(Polygon,(float)y,(float)x);
			mCombinedMask.at<uchar>(y, x) ^= mask.at<uchar>(y, x);
		}
	}
	return mask;
}
Mat VideoSnapCut::CreateContour(const Mat& Mask)
{
	CvSize size;
	size.width = Mask.cols;
	size.height = Mask.rows;

	Mat border(size, CV_8UC1);
			
	for(int y=0;y<border.rows;y++)
	{
		for(int x = 0;x<border.cols;x++)
		{
			bool borderPixel = false;
			int xx,yy;
			if(Mask.at<uchar>(y, x) == 1) // foreground
			{
				xx = x - 1;
				yy = y    ;
				if(xx==-1 || (xx >= 0 && !Mask.at<uchar>(yy, xx)))
				{
					borderPixel = true;						
				}
				
				xx = x + 1;
				yy = y    ;
				if(xx==border.cols || ( xx < border.cols && !Mask.at<uchar>(yy, xx) ))
				{
					borderPixel = true;						
				}
				
				xx = x	  ;	
				yy = y - 1;
				if(yy==-1 || (yy >= 0 && !Mask.at<uchar>(yy, xx)))
				{
					borderPixel = true;						
				}
				
				xx = x    ;
				yy = y + 1;
				if(yy==border.rows || (yy < border.rows && !Mask.at<uchar>(yy, xx)))
				{
					borderPixel = true;						
				}					
			}
			border.at<uchar>(y, x) = borderPixel;
		}
	}
	return border;

}

Mat VideoSnapCut::CreateDistanceTransforms(const Mat& Border)
{
	CvSize size;
	size.width = Border.cols;
	size.height = Border.rows;

	float dx = 1;
	float dy = 1;
	float dxy = sqrt(2.0f);

	Mat distanceTransforms(size, CV_32FC1);

	// clear 
	for(int y=0;y<distanceTransforms.rows;y++)
	{
		for(int x = 0;x<distanceTransforms.cols;x++)
		{
			if(Border.at<uchar>(y, x))
			{
				distanceTransforms.at<float>(y, x) = 0;
			}
			else
			{
				distanceTransforms.at<float>(y, x) = INF;
			}
		}
	}

	// forward pass
	for(int y=0;y<distanceTransforms.rows;y++)
	{
		for(int x = 0;x<distanceTransforms.cols;x++)
		{
			if(y-1>=0)
			{	
				distanceTransforms.at<float>(y, x) = min(distanceTransforms.at<float>(y, x), 
					distanceTransforms.at<float>(y-1, x) + dy);
			}
			if(x-1>=0)
			{	
				distanceTransforms.at<float>(y, x) = min(distanceTransforms.at<float>(y, x), 
					distanceTransforms.at<float>(y, x-1) + dx);
			}
			if(x-1>=0 && y-1>=0)
			{
				distanceTransforms.at<float>(y, x) = min(distanceTransforms.at<float>(y, x), 
					distanceTransforms.at<float>(y-1, x-1) + dxy);
			}
		}
	}

	// backword pass
	for(int y=distanceTransforms.rows-1;y>=0;y--)
	{
		for(int x=distanceTransforms.cols-1;x>=0;x--)
		{
			if(y+1<Border.rows)
			{
				distanceTransforms.at<float>(y, x) = min(distanceTransforms.at<float>(y, x), 
					distanceTransforms.at<float>(y+1, x) + dy);
			}
			if(x+1<Border.cols)
			{
				distanceTransforms.at<float>(y, x) = min(distanceTransforms.at<float>(y, x), 
					distanceTransforms.at<float>(y, x+1) + dx);
			}
			if(x+1<Border.cols && y+1<Border.rows)
			{
				distanceTransforms.at<float>(y, x) = min(distanceTransforms.at<float>(y, x), 
					distanceTransforms.at<float>(y+1, x+1) + dxy);
			}
		}
	}
	return distanceTransforms;
}
void VideoSnapCut::ClearBorderSamples()
{
	for(uint i=0;i<mForegroundSampledBorderPolygons.size();i++)
	{
		delete mForegroundSampledBorderPolygons[i];
	}
	mForegroundSampledBorderPolygons.clear();
}

Mat VideoSnapCut::CreatSampleContourImage(const Mat& image)
{
	CvSize size;
	size.width = image.cols;
	size.height = image.rows;

	Mat sampledContourImage(size,CV_32SC1);
	for(int r=0;r<sampledContourImage.rows;r++)
	{
		for(int c = 0;c<sampledContourImage.cols;c++)
		{
			int& pix = sampledContourImage.at<int>(r, c);
			pix = -1;			
		}
	}
	return sampledContourImage;
}

void VideoSnapCut::SampleContour()
{
	ClearBorderSamples();

	for(uint i=0;i<mForegroundBorderPolygons->size();i++)
	{
		Mat& sampledContourImages = mSampledContourImages[i];
		int polygonSize = (*mForegroundBorderPolygons)[i]->mPoints.size();
		if(polygonSize<1)
		{
			continue;
		}
		PolygonF* polygon = (*mForegroundBorderPolygons)[i];
		
		PolygonF* sampledPolygon = new PolygonF();
		
		float curDist = 0;
		int curPolygonPointIndex = 1;
		float curMagnitude;
		float sampleDist = VideoSnapCut::mMaskSize*2.0f/3;
		sampledPolygon->mPoints.push_back(polygon->mPoints[0]);
		PointF prevPoint = polygon->mPoints[0];
		PointF curPoint;
		

		while(curPolygonPointIndex<=polygonSize)
		{
			int curPolygonPointIndexMod = curPolygonPointIndex % polygonSize;
			curPoint = polygon->mPoints[curPolygonPointIndexMod];
			curMagnitude = sqrt((curPoint.x - prevPoint.x)*(curPoint.x - prevPoint.x)+(curPoint.y - prevPoint.y)*(curPoint.y - prevPoint.y));
			if(curDist+curMagnitude<sampleDist)
			{
				curDist += curMagnitude;
				prevPoint = curPoint;
				curPolygonPointIndex++;

			}
			else
			{
				float remainingDist = sampleDist - curDist;
				float t = remainingDist / curMagnitude;
				float x0 = prevPoint.x;
				float y0 = prevPoint.y;
				float dx = curPoint.x - prevPoint.x;
				float dy = curPoint.y - prevPoint.y;
				PointF newPoint = PointF(x0 + t * dx, y0 + t * dy);
				
				int c = (int)newPoint.x;
				int r = (int)newPoint.y;
				int& pix = sampledContourImages.at<int>(r, c);
				pix = sampledPolygon->mPoints.size();


				sampledPolygon->mPoints.push_back(newPoint);

				prevPoint = newPoint;
				curDist = 0;

			} 
		}
		PointF fisrtPoint = sampledPolygon->mPoints[0];
		PointF lastPoint = sampledPolygon->mPoints[sampledPolygon->mPoints.size()-1];
		float dx = lastPoint.x - fisrtPoint.x;
		float dy = lastPoint.y - fisrtPoint.y;				
		float dist = sqrt(dx*dx + dy*dy);
		if(dist<sampleDist*1.0)
		{
			sampledPolygon->mPoints.pop_back();
		}

		mForegroundSampledBorderPolygons.push_back(sampledPolygon);
	}
	
}

void VideoSnapCut::ResampleContour()
{
	ClearBorderSamples();

	for(uint i=0;i<mSampledContourImages.size();i++)
	{
		PolygonF* sampledPolygon = new PolygonF();

		Mat& sampledContourImages = mSampledContourImages[i];
		
		for(int r=0;r<(int)sampledContourImages.rows;r++)
		{
			for(int c = 0;c<(int)sampledContourImages.cols;c++)
			{
				int& pix = sampledContourImages.at<int>(r, c);
				if(pix != -1)
				{
					sampledPolygon->mPoints.push_back(PointF((float)c,(float)r));

				}
			}//for(int c = 0;c<(int)sampledContourImages.cols;c++)
		}//for(int r=0;r<(int)sampledContourImages.rows;r++)
	
		mForegroundSampledBorderPolygons.push_back(sampledPolygon);
		//DisplayBorderSamples(i);
	}//for(uint i=0;i<mSampledContourImages.size();i++)

}