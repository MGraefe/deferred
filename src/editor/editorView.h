// editor/editorView.h
// editor/editorView.h
// editor/editorView.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

// editorView.h : interface of the CeditorView class
//

#pragma once


class CeditorView : public CView
{
protected: // create from serialization only
	CeditorView();
	DECLARE_DYNCREATE(CeditorView)

// Attributes
public:
	CeditorDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CeditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in editorView.cpp
inline CeditorDoc* CeditorView::GetDocument() const
   { return reinterpret_cast<CeditorDoc*>(m_pDocument); }
#endif




