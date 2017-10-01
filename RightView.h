// RightView.h : interface of the CRightView class
//


#pragma once

#include "DataStructures.h"



class CDoc;

class CRightView : public CScrollView
{
	Scribble *mTempScribble;
	PolygonF	*mTempPolygon;


	CPoint mPrevPoint;
protected: // create from serialization only
	CRightView();
	DECLARE_DYNCREATE(CRightView)
	int mMouseDown;

// Attributes
public:
	CDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CRightView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	int mWidth;
	int mHeight;
	uchar* mData;
	bool mDrawBorder;
	bool mDrawMasks;
	void SetImageData(int width, int height, uchar* data, bool drawSrcibbles = false, bool drawMasks = false);
	void ClearScreen();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in RightView.cpp
inline CDoc* CRightView::GetDocument() const
   { return reinterpret_cast<CDoc*>(m_pDocument); }
#endif

