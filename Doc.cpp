// InterleavedVideoCutOutDoc.cpp : implementation of the CDoc class
//

#include "stdafx.h"
#include "App.h"

#include "Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDoc

IMPLEMENT_DYNCREATE(CDoc, CDocument)

BEGIN_MESSAGE_MAP(CDoc, CDocument)
END_MESSAGE_MAP()


// CDoc construction/destruction

CDoc::CDoc()
{
	// TODO: add one-time construction code here

}

CDoc::~CDoc()
{
}

BOOL CDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CDoc serialization

void CDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CDoc diagnostics

#ifdef _DEBUG
void CDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CDoc commands
