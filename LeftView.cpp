// InterleavedVideoCutOutView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "assert.h"
#include "time.h"
#include "stdlib.h"
#include "App.h"

#include "Doc.h"
#include "LeftView.h"
#include "RightView.h"
#include "BMPImage.h"

#include "FolderDlg.h"

#include "cv.h"
#include "cxcore.h"
#include"highgui.h"

#ifdef _DEBUG
#define new DEBUG_NEW 
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CFormView)

BEGIN_MESSAGE_MAP(CLeftView, CFormView)
	ON_BN_CLICKED(IDC_BUTTONOPEN, &CLeftView::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTONSELNONE, &CLeftView::OnBnClickedButtonselnone)
	ON_BN_CLICKED(IDC_BUTTONCLEARFRAMES, &CLeftView::OnBnClickedButtonclearframes)
	ON_BN_CLICKED(IDC_BUTTONSELFGMASK, &CLeftView::OnBnClickedButtonselfgmask)
	ON_BN_CLICKED(IDC_BUTTONSAMPLECONTOUR, &CLeftView::OnBnClickedButtonSampleContour)
	ON_BN_CLICKED(IDC_BUTTONMASK, &CLeftView::OnBnClickedButtonCreateMask)
	ON_BN_CLICKED(IDC_BUTTONDISTANCETRANSFORM, &CLeftView::OnBnClickedButtonDistanceTransform)
	ON_BN_CLICKED(IDC_BUTTONPROBABILITY, &CLeftView::OnBnClickedButtonProbability)
	ON_BN_CLICKED(IDC_BUTTONEXPLORE, &CLeftView::OnBnClickedButtonexplore)
	ON_BN_CLICKED(IDC_BUTTONFIRST, &CLeftView::OnBnClickedButtonfirst)
	ON_BN_CLICKED(IDC_BUTTONPREV, &CLeftView::OnBnClickedButtonprev)
	ON_BN_CLICKED(IDC_BUTTONNEXT, &CLeftView::OnBnClickedButtonnext)
	ON_BN_CLICKED(IDC_BUTTONLAST, &CLeftView::OnBnClickedButtonlast)
	ON_BN_CLICKED(IDC_BUTTONGO, &CLeftView::OnBnClickedButtongo)
	ON_BN_CLICKED(IDC_BUTTONREGISTER, &CLeftView::OnBnClickedButtonregister)
	ON_BN_CLICKED(IDC_BUTTONLOAD, &CLeftView::OnBnClickedButtonload)
	
	ON_BN_CLICKED(IDC_BUTTON_VIDEOSNAPCUT, &CLeftView::OnBnClickedButtonVideosnapcut)
	ON_BN_CLICKED(IDC_BUTTON2, &CLeftView::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CLeftView::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CLeftView::OnBnClickedButton4)
END_MESSAGE_MAP()

// CLeftView construction/destruction

CLeftView::CLeftView()
	: CFormView(CLeftView::IDD)
	, mFilePath(_T(""))
	, mEdtNumberFrames(0)
	, mEdtCurrentFrame(0)
{
	// TODO: add construction code here
	((CApp*)AfxGetApp())->mLeftView = this;
	mSelectionMode = 0;
	srand((uint)time(0));
	
	mImageSegmented = 0;


	mVideoSnapCut = 0;
	mVideoSnapCut = new VideoSnapCut();

}

CLeftView::~CLeftView()
{	
	ClearForegroundBorderPolygons();

	
}


void CLeftView::ClearForegroundBorderPolygons()
{
	for(uint i=0;i<mForegroundBorderPolygons.size();i++)
	{
		delete mForegroundBorderPolygons[i];
	}
	mForegroundBorderPolygons.clear();
}
void CLeftView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDITPATH, mFilePath);
	DDX_Text(pDX, IDC_EDITNUMBERFRAMES, mEdtNumberFrames);
	DDX_Text(pDX, IDC_EDITCURRENTRAME, mEdtCurrentFrame);
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CLeftView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	mFilePath = "E:\\Dev\\InterleavedVideoCutOut\\images\\snapcut2.bmp";
	//mFilePath = "C:\\Papers\\SiggraphAsis12\\Zhong12-SIGA-dataset\\HUMAN\\fq\\000.jpg";
	mFilePath = "E:\\Dev\\InterleavedVideoCutOut\\images\\videosnapcut.mp4";
	mMediaMode = 2;
	if(1)
	{
		mFilePath = "E:\\Projects\\images\\ANIMAL\\bear";
		mMediaMode = 3;
		mFilesPath.clear();
		ListFiles(mFilesPath);
	}
	UpdateData(0);

	
	LoadImageFile();

}


// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CFormView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDoc* CLeftView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
	return (CDoc*)m_pDocument;
}
#endif //_DEBUG


// CLeftView message handlers

void CLeftView::OnBnClickedButtonOpen()
{



	 CFileDialog dlg(TRUE,
		 NULL,NULL,OFN_OVERWRITEPROMPT,
		 //"Bitmap Files (*.bmp)|*.bmp|"
		 //"Avi Files (*.avi)|*.avi|"
		 //"MP4 Files (*.mp4)|*.mp4|"
		 //"All Files(*.*)|*.*||",
		 "Multimedia Files (*.bmp; *.jpg; *.png; *.avi; *.mp4)|*.bmp; *.jpg; *.png; *.avi; *.mp4|"
		 "Image Files (*.bmp; *.jpg; *.png)|*.bmp; *.jpg; *.png|"
		 "Video Files (*.avi; *.mp4)|*.avi; *.mp4|"
		 "All Files(*.*)|*.*||",		 
		 this);

	 
	 
	if (dlg.DoModal() == IDOK)
	{
		CString fileName = dlg.GetFileName();
		//CString folderName = dlg.GetFolderPath();
		mFilePath = dlg.GetPathName();
		
		int extIndex = mFilePath.ReverseFind('.');

		if(extIndex != -1) // file
		{
			int len = mFilePath.GetLength();	
			CString extension = mFilePath.Mid(extIndex+1,len-(extIndex+1));

			if( extension.CompareNoCase("bmp") == 0 || 
				extension.CompareNoCase("jpg") == 0 || 
				extension.CompareNoCase("png") == 0 || 
				extension.CompareNoCase("ppm") == 0 )
			{
				mMediaMode = 1;
			}
			else if(extension.CompareNoCase("avi") == 0 || 
			extension.CompareNoCase("mp4") == 0)
			{
				mMediaMode = 2;
			}
		}
		

		UpdateData(0);

		//LoadImageFile();

		((CApp*)AfxGetApp())->mRightView->Invalidate();
	}
}


int CLeftView::ListFiles(vector<string> &files)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	int Error;

	// Find the first file in the directory.
	CString allFilesPath = mFilePath + "\\*";
	hFind = FindFirstFile(allFilesPath, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		//printf ("Invalid file handle. Error is %u.\n", GetLastError());
		return 0;
	}
	else
	{
		// List all the other files in the directory.
		do 
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//If it's a directory nothing should happen. Just continue with the next file.
			}
			else
			{
				CString fileName(FindFileData.cFileName);
				if(fileName.Find(".bmp") != -1 || fileName.Find(".png") != -1 || fileName.Find(".jpg") != -1)
				{
					CString filePath = mFilePath + "\\" + fileName;
					//AfxMessageBox(filePath);
					files.push_back((LPCTSTR)filePath);
				}
				
			}

		}while (FindNextFile(hFind, &FindFileData) != 0);

		Error = GetLastError();
		FindClose(hFind);
		if (Error != ERROR_NO_MORE_FILES)
		{
			//printf ("FindNextFile error. Error is %u.\n", dwError);
			return 0;
		}
	}

	return 1;
}

void CLeftView::OnBnClickedButtonexplore()
{
	CFolderDialog dlg(  _T( "Select Images Folder" ), _T( "E:\\Dev\\InterleavedVideoCutOut\\images" ), this );
    if( dlg.DoModal() == IDOK  )
    {    
		mFilePath  = dlg.GetFolderPath();
		UpdateData(0);
		mFilesPath.clear();
		ListFiles(mFilesPath);
		mMediaMode = 3;
    }
}


void CLeftView::OnBnClickedButtonLoad()
{
	LoadImageFile();
}

void CLeftView::SetImageMat(const Mat& mFrameMat)
{
	ClearForegroundBorderPolygons();

	int width = mFrameMat.cols;
	int height = mFrameMat.rows;

	mVideoSnapCut->SetCurFrameMat(mFrameMat);

	uchar *tempData =  new uchar[width*height*3];

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			Vec3b intensity = mFrameMat.at<Vec3b>(y, x);
			uchar blue = intensity.val[0];
			uchar green = intensity.val[1];
			uchar red = intensity.val[2];
			int index1 = (y*width+x)*3;
			tempData[index1+0] = blue;
			tempData[index1+1] = green;
			tempData[index1+2] = red;
		}
	}


	((CApp*)AfxGetApp())->mRightView->SetImageData(width, height, tempData, (mImageSegmented != 1));		
	delete []tempData;
	

	UpdateData(0);
}

void CLeftView::LoadImageFromFile(string fileName)
{
	
	Mat mFrameMat = imread(fileName.data());
	
	if( !mFrameMat.empty() )
	{
		SetImageMat(mFrameMat);
		mVideoSnapCut->SetCurFrameMat(mFrameMat);		
	}

}

void CLeftView::LoadVideoFromFile(string fileName)
{	
	mVideoReader.Close();

	mVideoReader.Open(fileName);
		
	Mat mFrameMat;
	if(mVideoReader.GoToFirstFrame(mFrameMat))
	{
		if( !mFrameMat.empty() )
		{
			SetImageMat(mFrameMat);
			mVideoSnapCut->SetCurFrameMat(mFrameMat);
		}
	}
}



void CLeftView::LoadImageFile()
{
	UpdateData();	
	
	int extIndex = mFilePath.ReverseFind('.');
	
	
	if(extIndex != -1) // file
	{
		int len = mFilePath.GetLength();	
		CString extension = mFilePath.Mid(extIndex+1,len-(extIndex+1));

		if( extension.CompareNoCase("bmp") == 0 || 
			extension.CompareNoCase("jpg") == 0 || 
			extension.CompareNoCase("png") == 0 || 
			extension.CompareNoCase("ppm") == 0 )
		{
			LoadImageFromFile((LPCSTR)mFilePath);

			mEdtNumberFrames = 1;
			mEdtCurrentFrame = 1;
			UpdateData(0);
		}
		else if(extension.CompareNoCase("avi") == 0 || 
			extension.CompareNoCase("mp4") == 0)
		{
			LoadVideoFromFile((LPCSTR)mFilePath);
			mEdtNumberFrames = mVideoReader.mNumFrames;
			mCurFrame = mVideoReader.mCurFrame-1;
			mEdtCurrentFrame = mCurFrame+1;
			UpdateData(0);			
			mVideoSnapCut->SetVideoReader(&mVideoReader);
			mVideoSnapCut->SetCurFrame(mCurFrame);

		}
	}
	else // folder
	{
		if(mFilesPath.size()>0)
		{
			mCurFrame = 0;
			mEdtNumberFrames = mFilesPath.size();			
			mEdtCurrentFrame = mCurFrame + 1;
			UpdateData(0);

			LoadImageFromFile(mFilesPath[mCurFrame]);
			
			mVideoSnapCut->SetFilesPath(&mFilesPath);

			mVideoSnapCut->SetCurFrame(mCurFrame);
		}//if(mFilesPath.size()>0)

	}//else // folder
}





void CLeftView::AddForegroundBorder(PolygonF *borderPolygon)
{
	if(!borderPolygon)
	{
		return;
	}
	mForegroundBorderPolygons.push_back(borderPolygon);	
}




void CLeftView::OnBnClickedButtonselnone()
{
	mSelectionMode = 0;
}

void CLeftView::OnBnClickedButtonclearframes()
{
	if(mVideoSnapCut)
	{
		delete mVideoSnapCut;
		mVideoSnapCut = new VideoSnapCut();
	}

	
	((CApp*)AfxGetApp())->mRightView->ClearScreen();
}


void CLeftView::OnBnClickedButtonselfgmask()
{
	mSelectionMode = 3;
}




void CLeftView::OnBnClickedButtonSampleContour()
{

	//mVideoSnapCut->SetForegroundBorderPolygons(&mForegroundBorderPolygons);
	//mVideoSnapCut->CreateMasks();
	mVideoSnapCut->ResampleContour();
	
	((CApp*)AfxGetApp())->mRightView->mDrawBorder = true;
	((CApp*)AfxGetApp())->mRightView->Invalidate();
	
}

void CLeftView::OnBnClickedButtonCreateMask()
{
	mVideoSnapCut->SetForegroundBorderPolygons(&mForegroundBorderPolygons);
	mVideoSnapCut->CreateMasks();

	Mat mFrameMat = mVideoSnapCut->mImage;
	uchar *tempData =  new uchar[mFrameMat.cols*mFrameMat.rows*3];

	for(int r=0;r<mFrameMat.rows;r++)
	{
		for(int c = 0;c<mFrameMat.cols;c++)
		{
			int index1 = (r*mFrameMat.cols+c)*3;			
			Vec3b intensity = mFrameMat.at<Vec3b>(r, c);
			Color pix(intensity.val[2],intensity.val[1],intensity.val[0]);

			//pix.r *= 255; pix.r = pix.r < 0 ? 0 : pix.r > 255 ? 255 : pix.r;
			//pix.g *= 255; pix.g = pix.g < 0 ? 0 : pix.g > 255 ? 255 : pix.g;
			//pix.b *= 255; pix.b = pix.b < 0 ? 0 : pix.b > 255 ? 255 : pix.b;
			if(mVideoSnapCut->mCombinedMask.at<uchar>(r,c) == 1)
			{
				float t1 = 0.5, t2 = 1-t1;
				tempData[index1+0] = (uchar)(t1*255 + t2*pix.b);
				tempData[index1+1] = (uchar)(t1*255 + t2*pix.g);
				tempData[index1+2] = (uchar)(t1*255 + t2*pix.r);
				
			}
			else 
			{
				float t1 = 0.5, t2 = 1-t1;
				tempData[index1+0] = (uchar)(t1*0 + t2*pix.b);
				tempData[index1+1] = (uchar)(t1*0 + t2*pix.g);
				tempData[index1+2] = (uchar)(t1*0 + t2*pix.r);
			}
			
		}
	}
	((CApp*)AfxGetApp())->mRightView->SetImageData(mFrameMat.cols, mFrameMat.rows, tempData, false);		
	delete []tempData;

	//imwrite("e:\\CombinedMask.jpg", mVideoSnapCut->mCombinedMask);

}

bool CLeftView::pointInPolygon(float x, float y) 
{
	bool oddNodes=false;
	for(uint ii=0;ii<mForegroundBorderPolygons.size();ii++)
	{
		int polygonSize = mForegroundBorderPolygons[ii]->mPoints.size();
		if(polygonSize<2)
		{
			continue;
		}
		PolygonF* polygon = mForegroundBorderPolygons[ii];
		
		
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
	}
	return oddNodes;
}

void CLeftView::OnBnClickedButtonDistanceTransform()
{
	#define INF 1000000000
	mVideoSnapCut->SetForegroundBorderPolygons(&mForegroundBorderPolygons);
	mVideoSnapCut->CreateMasks();
	
	int randImageIndex =  rand()%mVideoSnapCut->mDistanceTransforms.size();
	Mat distTransform = mVideoSnapCut->mDistanceTransforms[randImageIndex];
	float maxDist = 0;
	 
	Mat mFrameMat = mVideoSnapCut->mImage;

	for(int r=0;r<mFrameMat.rows;r++)
	{
		for(int c = 0;c<mFrameMat.cols;c++)
		{
			if(distTransform.at<float>(r,c)>maxDist)
			{
				maxDist = distTransform.at<float>(r,c);
				if(maxDist == INF)
				{
					maxDist = maxDist;
				}
			}
		}
	}

	assert(maxDist != INF && maxDist != 0);
	
	uchar *tempData =  new uchar[mFrameMat.cols*mFrameMat.rows*3];

	for(int r=0;r<mFrameMat.rows;r++)
	{
		for(int c = 0;c<mFrameMat.cols;c++)
		{
			int index1 = (r*mFrameMat.cols+c)*3;			
			Vec3b intensity = mFrameMat.at<Vec3b>(r, c);
			Color pix(intensity.val[2],intensity.val[1],intensity.val[0]);
			//pix.r *= 255; pix.r = pix.r < 0 ? 0 : pix.r > 255 ? 255 : pix.r;
			//pix.g *= 255; pix.g = pix.g < 0 ? 0 : pix.g > 255 ? 255 : pix.g;
			//pix.b *= 255; pix.b = pix.b < 0 ? 0 : pix.b > 255 ? 255 : pix.b;
			float distNorm = distTransform.at<float>(r,c)/maxDist;
			int colorDistNorm = (int)(distNorm * 255);
			Color distPix((float)colorDistNorm, (float)colorDistNorm, (float)colorDistNorm);
				
		
			float t1 = 0.9f, t2 = 1-t1;
			tempData[index1+0] = (uchar)(t1*distPix.b + t2*pix.b);
			tempData[index1+1] = (uchar)(t1*distPix.g + t2*pix.g);
			tempData[index1+2] = (uchar)(t1*distPix.r + t2*pix.r);
			
			
		}
	}
	((CApp*)AfxGetApp())->mRightView->SetImageData(mFrameMat.cols, mFrameMat.rows, tempData, false);		
	delete []tempData;
}

void CLeftView::OnBnClickedButtonProbability()
{
	mVideoSnapCut->SetForegroundBorderPolygons(&mForegroundBorderPolygons);
	mVideoSnapCut->CreateMasks();
	mVideoSnapCut->BuildColorClassifiers();
	mVideoSnapCut->ClassifyPixels();	
	mVideoSnapCut->ResampleContour();
	((CApp*)AfxGetApp())->mRightView->Invalidate();
}


void CLeftView::OnBnClickedButtonfirst()
{	
	if(mMediaMode == 1)
	{
		return;
	}
	else if(mMediaMode == 2)
	{
		Mat mFrameMat;

		mVideoReader.GoToFirstFrame(mFrameMat);
		mCurFrame = mVideoReader.mCurFrame-1;
		mEdtCurrentFrame = mCurFrame+1;
		UpdateData(0);	
		if( !mFrameMat.empty() )
		{
			SetImageMat(mFrameMat);
			mVideoSnapCut->SetCurFrameMat(mFrameMat);
		}
		mVideoSnapCut->SetCurFrame(mCurFrame);
	}
	else if(mMediaMode == 3)
	{
		mCurFrame = 0;
		mEdtCurrentFrame = mCurFrame + 1;
		UpdateData(0);
		LoadImageFromFile(mFilesPath[mCurFrame]);	
		mVideoSnapCut->SetCurFrame(mCurFrame);
	}
}

void CLeftView::OnBnClickedButtonprev()
{
	if(mMediaMode == 1)
	{
		return;
	}
	else if(mMediaMode == 2)
	{
		Mat mFrameMat;

		mVideoReader.GetPrevFrame(mFrameMat);
		mCurFrame = mVideoReader.mCurFrame-1;
		mEdtCurrentFrame = mCurFrame+1;
		UpdateData(0);	
		if( !mFrameMat.empty() )
		{
			SetImageMat(mFrameMat);
			mVideoSnapCut->SetCurFrameMat(mFrameMat);
		}
		mVideoSnapCut->SetCurFrame(mCurFrame);
	
	}
	else if(mMediaMode == 3)
	{
		if(mCurFrame > 0)
		{
			mCurFrame--;
			mEdtCurrentFrame = mCurFrame + 1;
			UpdateData(0);
			LoadImageFromFile(mFilesPath[mCurFrame]);
			mVideoSnapCut->SetCurFrame(mCurFrame);
		}
	}
}

void CLeftView::OnBnClickedButtonnext()
{
	if(mMediaMode == 1)
	{
		return;
	}
	else if(mMediaMode == 2)
	{
		Mat mFrameMat;
		mVideoReader.GetNextFrame(mFrameMat);
		mCurFrame = mVideoReader.mCurFrame-1;
		mEdtCurrentFrame = mCurFrame+1;
		UpdateData(0);	
		if( !mFrameMat.empty() )
		{
			SetImageMat(mFrameMat);
			mVideoSnapCut->SetCurFrameMat(mFrameMat);
		}
		mVideoSnapCut->SetCurFrame(mCurFrame);
	
	}
	else if(mMediaMode == 3)
	{
		if(mFilesPath.size()>0)
		{
			if((uint)mCurFrame < mFilesPath.size()-1)
			{
				mCurFrame++;
				mEdtCurrentFrame = mCurFrame + 1;
				UpdateData(0);
				LoadImageFromFile(mFilesPath[mCurFrame]);
				mVideoSnapCut->SetCurFrame(mCurFrame);
			}
		}
	}
}

void CLeftView::OnBnClickedButtonlast()
{
	if(mMediaMode == 1)
	{
		return;
	}
	else if(mMediaMode == 2)
	{
		Mat mFrameMat;
		mVideoReader.GoToLastFrame(mFrameMat);
		mCurFrame = mVideoReader.mCurFrame-1;
		mEdtCurrentFrame = mCurFrame+1;
		UpdateData(0);	
		if( !mFrameMat.empty() )
		{
			SetImageMat(mFrameMat);
			mVideoSnapCut->SetCurFrameMat(mFrameMat);
		}
		mVideoSnapCut->SetCurFrame(mCurFrame);
	
	}
	else if(mMediaMode == 3)
	{
		if(mFilesPath.size()>0)
		{
			mCurFrame = mFilesPath.size()-1;
			mEdtCurrentFrame = mCurFrame + 1;
			UpdateData(0);
			LoadImageFromFile(mFilesPath[mCurFrame]);
			mVideoSnapCut->SetCurFrame(mCurFrame);
		}		
	}
}

void CLeftView::OnBnClickedButtongo()
{
	UpdateData();
	mCurFrame = mEdtCurrentFrame-1;
	mCurFrame = mCurFrame<0?0:mCurFrame;
	if(mMediaMode == 1)
	{
		return;
	}
	else if(mMediaMode == 2)
	{
		Mat mFrameMat;
		mVideoReader.GoToFrame(mCurFrame,mFrameMat);
		mCurFrame = mVideoReader.mCurFrame-1;
		mEdtCurrentFrame = mCurFrame+1;
		UpdateData(0);	
		if( !mFrameMat.empty() )
		{
			SetImageMat(mFrameMat);
			mVideoSnapCut->SetCurFrameMat(mFrameMat);
		}
		mVideoSnapCut->SetCurFrame(mCurFrame);

	
	}
	else if(mMediaMode == 3)
	{
		if(mFilesPath.size()>0)
		{
			mCurFrame = (uint)mCurFrame>mFilesPath.size()-1?mFilesPath.size()-1:mCurFrame;
			LoadImageFromFile(mFilesPath[mCurFrame]);
			mVideoSnapCut->SetCurFrame(mCurFrame);
		}
		
	}
	mEdtCurrentFrame = mCurFrame + 1;
	UpdateData(0);
}

void CLeftView::OnBnClickedButtonregister()
{
	mVideoSnapCut->RegisterCurFrameWithFrame(mCurFrame+1);
	mVideoSnapCut->BuildColorClassifiers();
	mVideoSnapCut->ClassifyPixels();
	
	mVideoSnapCut->ResampleContour();
	((CApp*)AfxGetApp())->mRightView->Invalidate();

	mCurFrame = mVideoSnapCut->mCurFrame;
	mEdtCurrentFrame = mCurFrame+1;
	UpdateData(0);	
}





void CLeftView::OnBnClickedButtonload()
{
	LoadImageFile();
}



void CLeftView::OnBnClickedButtonVideosnapcut()//视频目标抠图
{
	// TODO: 在此添加控件通知处理程序代码
	while (mEdtCurrentFrame<mEdtNumberFrames-2)
	{
		mVideoSnapCut->RegisterCurFrameWithFrame(mCurFrame+1);
		mVideoSnapCut->BuildColorClassifiers();
		mVideoSnapCut->ClassifyPixels();

		mVideoSnapCut->ResampleContour();
		((CApp*)AfxGetApp())->mRightView->Invalidate();

		mCurFrame = mVideoSnapCut->mCurFrame;
		mEdtCurrentFrame = mCurFrame+1;
		UpdateData(0);
		char c = cvWaitKey(1);
		if (c==27)
		{
			break;
		}
	}
}


void CLeftView::OnBnClickedButton2()//融图
{
	// TODO: 在此添加控件通知处理程序代码
	//选择前景文件夹
	BROWSEINFO bi;
	char chDir[MAX_PATH] = {0};
	memset((char *)&bi, 0, sizeof(bi));
	bi.hwndOwner = this->m_hWnd;
	bi.lpszTitle = "选择前景文件夹";
	bi.ulFlags = BIF_BROWSEFORCOMPUTER | BIF_RETURNONLYFSDIRS;	
	ITEMIDLIST *pList = NULL;
	pList = SHBrowseForFolder(&bi);
	if (pList == NULL)
	{
		return;		
	}
	SHGetPathFromIDList(pList, chDir);

	//选择背景图片
	CFileDialog dlg(TRUE, _T("*"), _T("所有图像文件|*.bmp;*.pcx;*.png;*.jpg;*.gif;*.tif;*.ico;*.dxf;*.cgm;*.cdr;*.wmf;*.eps;*.emf|位图(*.bmp;*.jpg;*.png;...)|*.bmp;*.pcx;*.png;*.jpg;*.gif;*.tif;*.ico|矢量图(*.wmf;*.eps;*.emf;..)|*.dxf;*.cgm;*.cdr;*.wmf;*.eps;*.emf"), OFN_ALLOWMULTISELECT);  
	if(IDOK!=dlg.DoModal())
		return;

	IplImage *imageBG;
	IplImage *imageBGResize;
	cvNamedWindow("SnapCutMix",CV_WINDOW_AUTOSIZE);
	IplImage* imageFG;
	IplImage* imageFGp;
	CvSize imageFGSize;
	int iFrame = 0;

	imageBG = cvLoadImage(dlg.GetPathName(),1);

	while (1)
	{
		string filenameFG(chDir);
		string filenameFGp(chDir);
		iFrame++;
		stringstream ss;
		string str;
		ss<<iFrame;
		ss>>str;
		filenameFG += "\\"+str+".jpg";
		imageFG = cvLoadImage(filenameFG.c_str());
		if(!imageFG)
		{
			break;
		}
		filenameFGp  += "\\p"+str+".jpg";
		imageFGp = cvLoadImage(filenameFGp.c_str(),CV_LOAD_IMAGE_GRAYSCALE);
		if(!imageFGp)
		{
			break;
		}
		cvThreshold(imageFGp, imageFGp, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);//可考虑采用大津阈值
		imageFGSize = cvGetSize(imageFG);
		

		//图像融合
		imageBGResize = cvCreateImage(imageFGSize,8,3);
		cvResize(imageBG,imageBGResize);

		for (int y=0;y<imageBGResize->height;y++)
		{
			for (int x=0;x<imageBGResize->width;x++)
			{
				if (cvGetReal2D(imageFGp,y,x)>0)
				{
					cvSet2D(imageBGResize,y,x,cvGet2D(imageFG,y,x));
				}
			}
		}

		cvShowImage("SnapCutMix",imageBGResize);
		char c = cvWaitKey(13);
		if (c==27)
		{
			break;
		}
	}
	cvWaitKey(0);
	cvReleaseImage(&imageBG);
	cvReleaseImage(&imageBGResize);
	cvReleaseImage(&imageFG);
	cvReleaseImage(&imageFGp);
	cvDestroyWindow("SnapCutMix");
}


void CLeftView::OnBnClickedButton3()//分割效果对比
{
	// TODO: 在此添加控件通知处理程序代码
	//选择文件夹
	BROWSEINFO bi;
	char chDir[MAX_PATH] = {0};
	memset((char *)&bi, 0, sizeof(bi));
	bi.hwndOwner = this->m_hWnd;
	bi.lpszTitle = "选择文件夹";

	bi.ulFlags = BIF_BROWSEFORCOMPUTER | BIF_RETURNONLYFSDIRS;	
	ITEMIDLIST *pList = NULL;
	pList = SHBrowseForFolder(&bi);
	if (pList == NULL)
	{
		return;		
	}
	SHGetPathFromIDList(pList, chDir);

	cvNamedWindow("SnapCutResult",CV_WINDOW_AUTOSIZE);
	IplImage* imageResult;
	IplImage* imageResultI;
	IplImage* imageTotal;
	int iFrame = 0;
	while (1)
	{
		string filename(chDir);
		string filenamep(chDir);
		iFrame++;
		stringstream ss;
		string str;
		ss<<iFrame;
		ss>>str;
		filename += "//"+str+".jpg";
		imageResult = cvLoadImage(filename.c_str());
		if(!imageResult)
		{
			break;
		}
		filenamep  += "//I"+str+".jpg";
		imageResultI = cvLoadImage(filenamep.c_str());

		//图像融合
		imageTotal = cvCreateImage(cvSize(imageResult->width*2,imageResult->height),8,3);
		cvZero(imageTotal);
		for (int y=0;y<imageTotal->height;y++)
		{
			for (int x=0;x<imageResultI->width;x++)
			{
				cvSet2D(imageTotal,y,x,cvGet2D(imageResultI,y,x));
			}
			for (int x=imageResultI->width;x<imageTotal->width;x++)
			{
				cvSet2D(imageTotal,y,x,cvGet2D(imageResult,y,x-imageResultI->width));
			}
		}

		cvShowImage("SnapCutResult",imageTotal);
		char c = cvWaitKey(13);
		if (c==27)
		{
			break;
		}
	}
	cvWaitKey(0);
	cvReleaseImage(&imageResult);
	cvReleaseImage(&imageResultI);
	cvReleaseImage(&imageTotal);
	cvDestroyWindow("SnapCutResult");
}


void CLeftView::OnBnClickedButton4()//
{
	// TODO: 在此添加控件通知处理程序代码
	//选择前景文件夹
	BROWSEINFO bi;
	char chDir[MAX_PATH] = {0};
	memset((char *)&bi, 0, sizeof(bi));
	bi.hwndOwner = this->m_hWnd;
	bi.lpszTitle = "选择前景文件夹";
	bi.ulFlags = BIF_BROWSEFORCOMPUTER | BIF_RETURNONLYFSDIRS;	
	ITEMIDLIST *pList = NULL;
	pList = SHBrowseForFolder(&bi);
	if (pList == NULL)
	{
		return;		
	}
	SHGetPathFromIDList(pList, chDir);

	cvNamedWindow("SnapCutSmooth",CV_WINDOW_AUTOSIZE);
	IplImage* imageFG;
	IplImage* imageFGI;
	IplImage* imageFGICopy;
	IplImage* imageFGp;
	IplImage* imageTotal;
	int iFrame = 0;
	while (1)
	{
		string filenameFG(chDir);
		string filenameFGI(chDir);
		string filenameFGp(chDir);
		iFrame++;
		stringstream ss;
		string str;
		ss<<iFrame;
		ss>>str;
		filenameFG += "\\"+str+".jpg";
		imageFG = cvLoadImage(filenameFG.c_str());
		if(!imageFG)
		{
			break;
		}
		filenameFGI  += "\\I"+str+".jpg";
		imageFGI = cvLoadImage(filenameFGI.c_str());
		if(!imageFGI)
		{
			break;
		}
		filenameFGp  += "\\p"+str+".jpg";
		imageFGp = cvLoadImage(filenameFGp.c_str(),CV_LOAD_IMAGE_GRAYSCALE);
		if(!imageFGp)
		{
			break;
		}
		cvThreshold(imageFGp, imageFGp, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);//可考虑采用大津阈值

		imageFGICopy = cvCloneImage(imageFGI);

		//背景模糊处理
		cvSmooth(imageFGI,imageFGI,CV_BLUR);
		cvSmooth(imageFGI,imageFGI,CV_BLUR);
		cvSmooth(imageFGI,imageFGI,CV_BLUR);
		
		imageTotal = cvCreateImage(cvSize(imageFGI->width*2,imageFGI->height),8,3);
		cvZero(imageTotal);
		for (int y=0;y<imageTotal->height;y++)
		{
			for (int x=0;x<imageFG->width;x++)
			{
				cvSet2D(imageTotal,y,x,cvGet2D(imageFGICopy,y,x));
			}
			for (int x=imageFG->width;x<imageTotal->width;x++)
			{
				if (cvGetReal2D(imageFGp,y,x-imageFG->width)>0)
				{
					cvSet2D(imageTotal,y,x,cvGet2D(imageFG,y,x-imageFG->width));
				}
				else
				{
					cvSet2D(imageTotal,y,x,cvGet2D(imageFGI,y,x-imageFG->width));
				}
			}
		}

		cvShowImage("SnapCutSmooth",imageTotal);
		char c = cvWaitKey(5);
		if (c==27)
		{
			break;
		}
	}
	cvWaitKey(0);
	cvReleaseImage(&imageFG);
	cvReleaseImage(&imageFGI);
	cvReleaseImage(&imageFGICopy);
	cvReleaseImage(&imageFGp);
	cvReleaseImage(&imageTotal);
	cvDestroyWindow("SnapCutSmooth");
}
