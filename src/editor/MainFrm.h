// editor/MainFrm.h
// editor/MainFrm.h
// editor/MainFrm.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "CToolsWndPane.h"
#include "CInputOutputWnd.h"

class CEntityDialog;

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	CPropertiesWnd *GetPropertiesWnd(void) { return &m_wndProperties; }
	CInputOutputWnd *GetInputOutputWnd(void) { return m_inoutWndPane.GetInOutWnd(); }
// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	HACCEL GetAccelTable() const { return m_hAccelTable; }

protected:  // control bar embedded members
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	COutputWnd        m_wndOutput;
	CInputOutputWndPane m_inoutWndPane;
	CToolsWndPane	  m_ToolsWndPane;
	CPropertiesWnd    m_wndProperties;
	CMFCCaptionBar    m_wndCaptionBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateCaptionBar();

private:
	CEntityDialog *m_pEntityDialog;
public:
	afx_msg void OnUpdateEditOpenProperties(CCmdUI *pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnEditDelete();
	afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);
	afx_msg void OnOpenEntityProperties();
	afx_msg void OnStartGame();
	afx_msg void OnUpdateStartGame(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewTogglelighting(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewTogglewater(CCmdUI *pCmdUI);
	afx_msg void OnViewTogglepostprocessing();
	afx_msg void OnUpdateViewTogglepostprocessing(CCmdUI *pCmdUI);
};
