// editor/COpenGLView.h
// editor/COpenGLView.h
// editor/COpenGLView.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once

#include "IRenderView.h"
class CeditorDoc;
class CGLResources;
class COpenGLView : public CView
{
	DECLARE_CLASS( CView, COpenGLView );

protected: // create from serialization only
	COpenGLView();
	DECLARE_DYNCREATE(COpenGLView)
	bool m_bInitialized;
public:
	virtual ~COpenGLView();
	CeditorDoc* GetDocument() const;
	
	void AttachView( IRenderView *pView );
	IRenderView *GetRenderView( void ) { return m_RenderView; }
	viewtypes_e GetViewType(void) { return m_RenderView->GetViewType(); }
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual int OnCreate( LPCREATESTRUCT lpcs );
	virtual BOOL OnEraseBkgnd(CDC* pDC);

	void SetGLResources( CGLResources *pResources ) { m_pGLResources = pResources; }
	int InitOpenGL( CGLResources *pResources );

public:	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void DrawView( void );

protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
	IRenderView *m_RenderView;
	CGLResources *m_pGLResources;

};

#ifndef _DEBUG  // debug version in editorView.cpp
inline CeditorDoc* COpenGLView::GetDocument() const
{ return reinterpret_cast<CeditorDoc*>(m_pDocument); }
#endif

//extern HGLRC g_OglRenderCtx;
extern bool g_bCreate3dView;

#define TIMER_WASD_CONTROLS_REDRAW 548842
