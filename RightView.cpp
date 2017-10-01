// RightView.cpp : implementation of the CRightView class
//

#include "stdafx.h"
#include "App.h"

#include "Doc.h"
#include "RightView.h"
#include "LeftView.h"
#include "DataStructures.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
/*

Drawing the path between two pixels

pointA = waypointsToDraw.ElementAt(i);
pointB = waypointsToDraw.ElementAt(i + 1);
deltaVector = pointB - pointA;
distance = deltaVector.Length();
direction = deltaVector / distance;
for (int z = 1; z < distance; z++)
{
    newPoint = pointA + direction * (distance * (z/distance));
    //this is the newPoint - it will be every point/pixel between pointA and pointB. Put drawing code here

}


*/

// CRightView

IMPLEMENT_DYNCREATE(CRightView, CScrollView)

BEGIN_MESSAGE_MAP(CRightView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CRightView construction/destruction

CRightView::CRightView()
{
	mWidth = 0;
	mHeight = 0;
	mData = 0;
	
	((CApp*)AfxGetApp())->mRightView = this;
	mMouseDown = 0;
	mDrawBorder = true;
	mTempScribble = 0;
	mTempPolygon = 0;

}

CRightView::~CRightView()
{
}

BOOL CRightView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

// CRightView drawing

void CRightView::OnDraw(CDC* pDC)
{
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	if(!mData)
	{
		return;
	}
	CBitmap bmp;
	if (bmp.CreateBitmap(mWidth, 
		mHeight,
		1, 32,
		mData))
	{
		// Get the size of the bitmap
		BITMAP bmpInfo;
		bmp.GetBitmap(&bmpInfo);

		// Create an in-memory DC compatible with the
		// display DC we're using to paint
		CDC dcMemory;
		dcMemory.CreateCompatibleDC(pDC);

		// Select the bitmap into the in-memory DC
		CBitmap* pOldBitmap = dcMemory.SelectObject(&bmp);

		// Find a centerpoint for the bitmap in the client area
		CRect rect;
		GetClientRect(&rect);
		int nX = rect.left + (rect.Width() - bmpInfo.bmWidth) / 2;
		int nY = rect.top + (rect.Height() - bmpInfo.bmHeight) / 2;

		// Copy the Surface from the in-memory DC into the on-
		// screen DC to actually do the painting. Use the centerpoint
		// we computed for the target offset.
		pDC->BitBlt(0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcMemory, 0, 0, SRCCOPY);

		dcMemory.SelectObject(pOldBitmap);

		
		if(mDrawBorder)
		{
			// create and select a thick, black pen
			CPen pen;			
			CBrush brush;
			CBrush brushyellow;
			
			CBrush* pOldBrush;
			CPen* pOldPen;

			brush.CreateSolidBrush(RGB(0,0,255));
			brushyellow.CreateSolidBrush(RGB(255,255,0));
			pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));

			pOldBrush = pDC->SelectObject(&brush);
			pOldPen = pDC->SelectObject(&pen);
			for(uint i=0;i<((CApp*)AfxGetApp())->mLeftView->mForegroundBorderPolygons.size();i++)
			{
				PolygonF* polygon = ((CApp*)AfxGetApp())->mLeftView->mForegroundBorderPolygons[i];
				PointF point = polygon->mPoints[0];
				pDC->MoveTo((int)point.x,(int)point.y);
				pDC->SelectObject(pOldBrush);
				pOldBrush = pDC->SelectObject(&brushyellow);
				pDC->Ellipse((int)point.x-4,(int)point.y-4,(int)point.x+4,(int)point.y+4);
				pDC->SelectObject(pOldBrush);
				pOldBrush = pDC->SelectObject(&brush);
				pDC->Ellipse((int)point.x-1,(int)point.y-1,(int)point.x+1,(int)point.y+1);

				for(uint j = 0;j<polygon->mPoints.size();j++)
				{
					PointF point = polygon->mPoints[j];
					pDC->LineTo((int)point.x,(int)point.y);
					pDC->SelectObject(pOldBrush);
					pOldBrush = pDC->SelectObject(&brushyellow);
					pDC->Ellipse((int)point.x-4,(int)point.y-4,(int)point.x+4,(int)point.y+4);
					pDC->SelectObject(pOldBrush);
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Ellipse((int)point.x-1,(int)point.y-1,(int)point.x+1,(int)point.y+1);

				}
				pDC->LineTo((int)polygon->mPoints[0].x,(int)polygon->mPoints[0].y);
			}
			pDC->SelectObject(pOldPen);
			

			pDC->SelectObject(pOldBrush);
			brush.DeleteObject();
			brushyellow.DeleteObject();
			pen.DeleteObject();
		
			
					

			// put back the old objects
			
		}

		if(mDrawMasks)
		{
			
			// create and select a thick, black pen
			CPen pen;			
			CBrush brush;
			CBrush brushyellow;
			
			CBrush* pOldBrush;
			CPen* pOldPen;

			pen.CreatePen(PS_SOLID, 1, RGB(255, 255, 0));

			brush.CreateSolidBrush(RGB(0,0,255));
			
			brushyellow.CreateSolidBrush(RGB(255,255,0));
		
			
			pOldBrush = pDC->SelectObject(&brush);
			
			pOldPen = pDC->SelectObject(&pen);

			for(uint i=0;i<((CApp*)AfxGetApp())->mLeftView->mVideoSnapCut->mForegroundSampledBorderPolygons.size();i++)
			{
				PolygonF* polygon = ((CApp*)AfxGetApp())->mLeftView->mVideoSnapCut->mForegroundSampledBorderPolygons[i];
				PointF point = polygon->mPoints[0];
				//pDC->MoveTo(point.x,point.y);
				pDC->SelectObject(pOldBrush);
				pOldBrush = pDC->SelectObject(&brushyellow);
				pDC->Ellipse((int)point.x-2,(int)point.y-2,(int)point.x+2,(int)point.y+2);
				pDC->SelectObject(pOldBrush);
				pOldBrush = pDC->SelectObject(&brush);
				pDC->Ellipse((int)point.x-1,(int)point.y-1,(int)point.x+1,(int)point.y+1);

				for(uint j = 0;j<polygon->mPoints.size();j++)
				{
					PointF point = polygon->mPoints[j];
					//pDC->LineTo(point.x,point.y);
					pDC->SelectObject(pOldBrush);
					pOldBrush = pDC->SelectObject(&brushyellow);
					pDC->Ellipse((int)point.x-2,(int)point.y-2,(int)point.x+2,(int)point.y+2);
					pDC->SelectObject(pOldBrush);
					pOldBrush = pDC->SelectObject(&brush);
					pDC->Ellipse((int)point.x-1,(int)point.y-1,(int)point.x+1,(int)point.y+1);
					int maskWidth2 = VideoSnapCut::mMaskSize/2;
					pDC->MoveTo((int)point.x-maskWidth2 ,(int)point.y-maskWidth2 );
					pDC->LineTo((int)point.x+maskWidth2 ,(int)point.y-maskWidth2 );
					pDC->LineTo((int)point.x+maskWidth2 ,(int)point.y+maskWidth2 );
					pDC->LineTo((int)point.x-maskWidth2 ,(int)point.y+maskWidth2 );
					pDC->LineTo((int)point.x-maskWidth2 ,(int)point.y-maskWidth2 );
				}
				//pDC->LineTo(polygon->mPoints[0].x,polygon->mPoints[0].y);
			}
			pDC->SelectObject(pOldPen);
			
			pDC->SelectObject(pOldBrush);
			brush.DeleteObject();
			brushyellow.DeleteObject();
			pen.DeleteObject();

					

			// put back the old objects
			
		}



	}
	else
	{
		TRACE0("ERROR: Where's IDB_BITMAP1?\n");
	}
}

void CRightView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 4000;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// CRightView printing

BOOL CRightView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CRightView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CRightView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CRightView::ClearScreen()
{
	if(mData)
	{
		delete []mData;
		mData = 0;
	}
	Invalidate();
}

void CRightView::SetImageData(int width, int height, uchar* data, bool drawSrcibbles, bool drawMasks)
{
	if(mData)
	{
		delete []mData;
	}
	mDrawBorder = drawSrcibbles;
	mDrawMasks = drawMasks;
	mWidth = width;
	mHeight = height;
	mData = new uchar[mWidth*mHeight*4];
	for(int r=0;r<mHeight;r++)
	{
		for(int c = 0;c<mWidth;c++)
		{
			int index1 = (r*mWidth+c)*4;
			int index2 = (r*mWidth+c)*3;
			mData[index1+0] = data[index2+0];
			mData[index1+1] = data[index2+1];
			mData[index1+2] = data[index2+2];
			mData[index1+3] = 0;

			//mData[index1+0] = 255;
			//mData[index1+1] = 255;
			//mData[index1+2] = 0;
			//mData[index1+3] = 0;


		}
	}
	Invalidate();
}

// CRightView diagnostics

#ifdef _DEBUG
void CRightView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CRightView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CDoc* CRightView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
	return (CDoc*)m_pDocument;
}
#endif //_DEBUG


// CRightView message handlers

void CRightView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(!mData)
	{
		return;
	}
	CPoint pt = GetScrollPosition();
	point+=pt;
	CDC* pDC = GetDC();
	if(((CApp*)AfxGetApp())->mLeftView->mSelectionMode == 1)
	{
		CPen pen;
		pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		CPen* pOldPen = pDC->SelectObject(&pen);
		mTempScribble = new Scribble();
		mTempScribble->mPoints.push_back(PointI(point.x,point.y));
		point-=pt;
		mPrevPoint = point;
		pDC->MoveTo(point);
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
		mMouseDown = 1;
	}
	else if(((CApp*)AfxGetApp())->mLeftView->mSelectionMode == 2)
	{
		CPen pen;
		pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);

		mTempScribble = new Scribble();
		mTempScribble->mPoints.push_back(PointI(point.x,point.y));
		point-=pt;
		mPrevPoint = point;
		pDC->MoveTo(point);
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
		mMouseDown = 1;
	}
	else if(((CApp*)AfxGetApp())->mLeftView->mSelectionMode == 3)
	{
		CBrush brush;
		brush.CreateSolidBrush(RGB(0,0,255));

		CBrush brushyellow;
		brushyellow.CreateSolidBrush(RGB(255,255,0));
				
		CBrush* pOldBrush = pDC->SelectObject(&brush);
		CPen pen;
		pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);

		if(!mMouseDown)
		{
			mTempPolygon = new PolygonF();
						
		}		
		mTempPolygon->mPoints.push_back(PointF((float)point.x,(float)point.y));
		point-=pt;
		if(mMouseDown)
		{
			//if(mTempPolygon->mPoints.size() >= 2)
			//{
			//	PointF pointF = mTempPolygon->mPoints[mTempPolygon->mPoints.size()-2];
			//	CPoint ptScroll = GetScrollPosition();
			//	//pointF.x += ptScroll.x;
			//	//pointF.y += ptScroll.y;
			//	mPrevPoint = CPoint(pointF.x,pointF.y);
			//}
			pDC->MoveTo(mPrevPoint);
		}
		else
		{
			pDC->MoveTo(point);			
		}
		pDC->LineTo(point);
		mPrevPoint = point;		
		pDC->SelectObject(pOldBrush);
		pOldBrush = pDC->SelectObject(&brushyellow);
		pDC->Ellipse(point.x-4,point.y-4,point.x+4,point.y+4);
		pDC->SelectObject(pOldBrush);
		pOldBrush = pDC->SelectObject(&brush);
		pDC->Ellipse(point.x-1,point.y-1,point.x+1,point.y+1);
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
		pDC->SelectObject(pOldBrush);
		brush.DeleteObject();
		brushyellow.DeleteObject();
		mMouseDown = 1;

	}
	

	

	CScrollView::OnLButtonDown(nFlags, point);
}

void CRightView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(!mData || !mTempScribble)
	{
		return;
	}
	CPoint pt = GetScrollPosition();
	point+=pt;
	CDC* pDC = GetDC();
	if(((CApp*)AfxGetApp())->mLeftView->mSelectionMode == 1)
	{
		CPen pen;
		pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		CPen* pOldPen = pDC->SelectObject(&pen);
		mTempScribble->mPoints.push_back(PointI(point.x,point.y));
		//((CApp*)AfxGetApp())->mLeftView->AddForegroundScribble(mTempScribble);
		mTempScribble = 0;

		point-=pt;
		pDC->MoveTo(mPrevPoint);
		pDC->LineTo(point);
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
		mMouseDown = 0;
	}
	else if(((CApp*)AfxGetApp())->mLeftView->mSelectionMode == 2)
	{
		CPen pen;
		pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);

		mTempScribble->mPoints.push_back(PointI(point.x,point.y));
		//((CApp*)AfxGetApp())->mLeftView->AddBackgroundScribble(mTempScribble);
		point-=pt;
		pDC->MoveTo(mPrevPoint);
		pDC->LineTo(point);
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
		mMouseDown = 0;
	}
	else if(((CApp*)AfxGetApp())->mLeftView->mSelectionMode == 2)
	{
		//mMouseDown = 0;
	}

	
	CScrollView::OnLButtonUp(nFlags, point);
}

void CRightView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!mData)
	{
		return;
	}
	CPoint pt = GetScrollPosition();
	point+=pt;
	CDC* pDC = GetDC();
	if(mMouseDown)
	{
		if(((CApp*)AfxGetApp())->mLeftView->mSelectionMode == 1)
		{
			CPen pen;
			pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
			CPen* pOldPen = pDC->SelectObject(&pen);
			mTempScribble->mPoints.push_back(PointI(point.x,point.y));
			point-=pt;
			pDC->MoveTo(mPrevPoint);
			pDC->LineTo(point);
			mPrevPoint = point;
			if(mTempScribble->mPoints.size()>2)
			{
				CPoint pointA = mPrevPoint;
				CPoint pointB = point;
				float deltaVectorx = (float)pointB.x - (float)pointA.x;
				float deltaVectory = (float)pointB.y - (float)pointA.y;
				float distance = sqrtf(deltaVectorx*deltaVectorx+deltaVectory*deltaVectory);
				float directionx = deltaVectorx;
				float directiony = deltaVectory;
				directionx /= distance;
				directiony /= distance;
				for (int z = 1; z < distance; z++)
				{
					CPoint newPoint = pointA + CPoint((int)(directionx * z),(int)(directiony * z));
					CPoint preceedingPoint = CPoint( mTempScribble->mPoints[mTempScribble->mPoints.size()-1].x, mTempScribble->mPoints[mTempScribble->mPoints.size()-1].y );
					if(newPoint.x != preceedingPoint.x || newPoint.y != preceedingPoint.y)
					{
						mTempScribble->mPoints.push_back(PointI(newPoint.x,newPoint.y));
					}
					point-=pt;
					pDC->MoveTo(mPrevPoint);
					pDC->LineTo(point);
					mPrevPoint = point;
					//this is the newPoint - it will be every point/pixel between pointA and pointB. Put drawing code here

				}

			}
			pDC->SelectObject(pOldPen);
			pen.DeleteObject();
		}
		else if(((CApp*)AfxGetApp())->mLeftView->mSelectionMode == 2)
		{
			CPen pen;
			pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
			CPen* pOldPen = pDC->SelectObject(&pen);
			mTempScribble->mPoints.push_back(PointI(point.x,point.y));
			point-=pt;
			pDC->MoveTo(mPrevPoint);
			pDC->LineTo(point);
			mPrevPoint = point;
			if(mTempScribble->mPoints.size()>2)
			{
				CPoint pointA = mPrevPoint;
				CPoint pointB = point;
				float deltaVectorx = (float)(pointB.x - pointA.x);
				float deltaVectory = (float)(pointB.y - pointA.y);
				float distance = sqrtf(deltaVectorx*deltaVectorx+deltaVectory*deltaVectory);
				float directionx = deltaVectorx;
				float directiony = deltaVectory;
				directionx /= distance;
				directiony /= distance;
				for (int z = 1; z < distance; z++)
				{
					CPoint newPoint = pointA + CPoint((int)(directionx * z) ,(int)(directiony * z));
					CPoint preceedingPoint = CPoint( mTempScribble->mPoints[mTempScribble->mPoints.size()-1].x, mTempScribble->mPoints[mTempScribble->mPoints.size()-1].y );
					if(newPoint.x != preceedingPoint.x || newPoint.y != preceedingPoint.y)
					{
						mTempScribble->mPoints.push_back(PointI(newPoint.x,newPoint.y));
					}
					point-=pt;
					pDC->MoveTo(mPrevPoint);
					pDC->LineTo(point);
					mPrevPoint = point;
					//this is the newPoint - it will be every point/pixel between pointA and pointB. Put drawing code here

				}

			}
			pDC->SelectObject(pOldPen);
			pen.DeleteObject();
		}
	}
	CScrollView::OnMouseMove(nFlags, point);
}

void CRightView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if(!mData || !mTempPolygon)
	{
		return;
	}
	CPoint pt = GetScrollPosition();
	point+=pt;
	CDC* pDC = GetDC();
	if(mMouseDown && ((CApp*)AfxGetApp())->mLeftView->mSelectionMode == 3)
	{
		CBrush brush;
		brush.CreateSolidBrush(RGB(255,0,0));
		CBrush* pOldBrush = pDC->SelectObject(&brush);
		CPen pen;
		pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
		CPen* pOldPen = pDC->SelectObject(&pen);

		//mTempPolygon->mPoints.push_back(PointF(point.x,point.y));
		////((CApp*)AfxGetApp())->mLeftView->AddBackgroundScribble(mTempScribble);
		//point-=pt;
		//pDC->MoveTo(mPrevPoint);
		//pDC->LineTo(point);
		CPoint firstPoint = CPoint((int)mTempPolygon->mPoints[0].x,(int)mTempPolygon->mPoints[0].y);
		
		//CPoint lastPoint = CPoint(mTempPolygon->mPoints[mTempPolygon->mPoints.size()-1].x,mTempPolygon->mPoints[mTempPolygon->mPoints.size()-1].y);
		pDC->MoveTo(mPrevPoint);
		pDC->LineTo(firstPoint);

		((CApp*)AfxGetApp())->mLeftView->AddForegroundBorder(mTempPolygon);
		
		//pDC->Ellipse(point.x-2,point.y-2,point.x+2,point.y+2);
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
		pDC->SelectObject(pOldBrush);
		brush.DeleteObject();

		mTempPolygon = 0;
		mMouseDown = 0;
	}
	

	CScrollView::OnRButtonDown(nFlags, point);
}
