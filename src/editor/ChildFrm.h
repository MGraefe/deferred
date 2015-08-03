// editor/ChildFrm.h
// editor/ChildFrm.h
// editor/ChildFrm.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

// ChildFrm.h : interface of the CChildFrame class
//

#pragma once
class CGLResources;
class COpenGLView;
class CCamera;

class CChildFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();
	virtual ~CChildFrame();
	COpenGLView *GetView( viewtypes_e viewtype );
	CGLResources *GetGLResources(void) { return m_pGLResources; }
	void MaximizeView(int row, int col);
	void ReorderViews();
	CCamera *GetActiveCamera();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual int OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext* pContext );

	afx_msg void OnOpenWorldProperties();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
	void CreateSplitterView( int row, int col, CCreateContext* pContext );
private:
	CSplitterWnd m_SplitterWnd;
	CGLResources *m_pGLResources;
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	UINT_PTR m_timerId; 
public:
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
};


CChildFrame *GetActiveChildFrame(void);
CGLResources *GetActiveGLResources(void);
