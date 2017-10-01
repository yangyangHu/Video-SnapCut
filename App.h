// App.h : main header file for the InterleavedVideoCutOut application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

// CApp:
// See InterleavedVideoCutOut.cpp for the implementation of this class
//

class CLeftView;
class CRightView;

class CApp : public CWinApp
{
public:
	CApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	CLeftView *mLeftView;
	CRightView *mRightView;
};

extern CApp theApp;