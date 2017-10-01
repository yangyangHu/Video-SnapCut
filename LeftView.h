// RightView.h : interface of the CLeftView class
//


#pragma once
#include <vector>
using namespace std;

#include "DataImage.h"
#include "Color.h"
#include "DataStructures.h"
#include "graph.h"
#include "VideoSnapCut.h"
#include "afxcmn.h"
#include "VideoReader.h"

class CLeftView : public CFormView
{
public:
	VideoSnapCut*	mVideoSnapCut;
	vector<PolygonF*> mForegroundBorderPolygons;	
	VideoReader mVideoReader;	
	vector<string> mFilesPath;
	int			mCurFrame;
	int			mMediaMode;			//1 singel image, 2 video, 3 folder

	int mSelectionMode;
	int mImageSegmented;

	void AddForegroundBorder(PolygonF *borderPolygon);

	
protected: // create from serialization only
	CLeftView(); 
	DECLARE_DYNCREATE(CLeftView)

public:
	enum{ IDD = IDD_INTERLEAVEDVIDEOCUTOUT_FORM };

// Attributes
public:
	CDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CLeftView();
	void ClearForegroundBorderPolygons();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	CString mFilePath;
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonLoad();

public:
	void LoadImageFile();
	int ListFiles(vector<string> &files);
	void LoadImageFromFile(string fileName);
	void LoadVideoFromFile(string fileName);
	void SetImageMat(const Mat& mFrameMat);
	

	
public:
	int mEdtNumberFrames;
	int mEdtCurrentFrame;
	
public:
	bool pointInPolygon(float x, float y);
	
public:
	afx_msg void OnBnClickedButtonselnone();
	afx_msg void OnBnClickedButtonclearframes();
	afx_msg void OnBnClickedButtonselfgmask();
	afx_msg void OnBnClickedButtonSampleContour();
	afx_msg void OnBnClickedButtonCreateMask();
	afx_msg void OnBnClickedButtonProbability();
	afx_msg void OnBnClickedButtonexplore();
	afx_msg void OnBnClickedButtonfirst();
	afx_msg void OnBnClickedButtonprev();
	afx_msg void OnBnClickedButtonnext();
	afx_msg void OnBnClickedButtonlast();
	afx_msg void OnBnClickedButtongo();
	afx_msg void OnBnClickedButtonregister();
	afx_msg void OnBnClickedButtonCalculateMasks();
	afx_msg void OnBnClickedButtonload();
	afx_msg void OnBnClickedButtonDistanceTransform();
	afx_msg void OnBnClickedButtonregister2();
	afx_msg void OnBnClickedButtonVideosnapcut();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};

#ifndef _DEBUG  // debug version in InterleavedVideoCutOutView.cpp
inline CDoc* CLeftView::GetDocument() const
   { return reinterpret_cast<CDoc*>(m_pDocument); }
#endif

