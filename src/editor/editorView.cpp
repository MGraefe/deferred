// editor/editorView.cpp
// editor/editorView.cpp
// editor/editorView.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

// editorView.cpp : implementation of the CeditorView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "editor.h"
#endif

#include "editorDoc.h"
#include "editorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CeditorView

IMPLEMENT_DYNCREATE(CeditorView, CView)

BEGIN_MESSAGE_MAP(CeditorView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CeditorView construction/destruction

CeditorView::CeditorView()
{
	// TODO: add construction code here

}

CeditorView::~CeditorView()
{
}

BOOL CeditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CeditorView drawing

void CeditorView::OnDraw(CDC* /*pDC*/)
{
	CeditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

void CeditorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CeditorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CeditorView diagnostics

#ifdef _DEBUG
void CeditorView::AssertValid() const
{
	CView::AssertValid();
}

void CeditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CeditorDoc* CeditorView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CeditorDoc)));
	return (CeditorDoc*)m_pDocument;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
//void CeditorView::OnCreate( LPCREATESTRUCT lpcs )
//{
//	
//}

#endif //_DEBUG


// CeditorView message handlers
