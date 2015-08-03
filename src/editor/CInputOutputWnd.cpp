// editor/CInputOutputWnd.cpp
// editor/CInputOutputWnd.cpp
// editor/CInputOutputWnd.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "editor.h"
#include "CInputOutputWnd.h"
#include "afxdialogex.h"
#include "CEntityOutputWnd.h"
#include "editorDoc.h"


IMPLEMENT_DYNAMIC(CInputOutputWnd, CDialogEx)


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CInputOutputWnd::CInputOutputWnd(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInputOutputWnd::IDD, pParent)
{
	m_outputWnd = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CInputOutputWnd::~CInputOutputWnd()
{
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CInputOutputWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INOUT_TABMAIN, m_tabs);
}


BEGIN_MESSAGE_MAP(CInputOutputWnd, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
BOOL CInputOutputWnd::PreTranslateMessage(MSG* pMsg)
{
	//if (pMsg->message == WM_KEYDOWN)
	//{
	//	if ((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE))
	//		pMsg->wParam = VK_TAB;
	//}
	return CDialogEx::PreTranslateMessage(pMsg);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
BOOL CInputOutputWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_tabs.SetWindowPos(NULL, 0, 0, -1, -1, SWP_NOSIZE | SWP_NOZORDER);
	m_tabs.InitDialogs();
	
	return TRUE;  // return TRUE unless you set the focus to a control
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CInputOutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if( IsWindow(m_tabs.GetSafeHwnd()) )
	{
		m_tabs.SetWindowPos(&wndTop, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE);

		if(m_outputWnd)
		{
			CRect rect;
			m_tabs.GetClientRect(&rect);
			//m_tabs.GetItemRect(0, &rect);
			m_outputWnd->SetWindowPos(&wndTop, rect.left, rect.bottom, rect.Width(), rect.Height(), NULL );
		}
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CInputOutputWnd::UpdateEntitySelection(CeditorDoc *activeDoc)
{
	m_tabs.UpdateEntitySelection(activeDoc);
}


IMPLEMENT_DYNAMIC(CInputOutputTabCtrl, CTabCtrl)

CInputOutputTabCtrl::CInputOutputTabCtrl()
	: CTabCtrl()
{
	m_wndIds[0] = IDD_OUTPUTS;
	m_wnds[0] = new CEntityOutputWnd();
	m_wndNames[0] = _T("Outputs");
	m_lastSizeX = -1;
	m_lastSizeY = -1;
}


CInputOutputTabCtrl::~CInputOutputTabCtrl()
{
}


void CInputOutputTabCtrl::DoDataExchange(CDataExchange* pDX)
{
	CTabCtrl::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInputOutputTabCtrl, CTabCtrl)
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CInputOutputTabCtrl::InitDialogs()
{
	CRect clientRect;
	GetClientRect(clientRect);
	m_lastSizeX = clientRect.Width();
	m_lastSizeY = clientRect.Height();

	for(int i = 0; i < m_numTabs; i++)
	{
		m_wnds[i]->Create(m_wndIds[i], GetParent());
		m_wnds[i]->SetParent(this);
	}

	for(int i = 0; i < m_numTabs; i++)
		InsertItem(i, m_wndNames[i].GetBuffer());

	ActivateTabDialogs();
}


void CInputOutputTabCtrl::ActivateTabDialogs()
{
	int sel = GetCurSel();
	if(sel < 0 || sel > m_numTabs)
		error("CInputOutputTabCtrl#ActivateTabDialogs(): Unknown selection id.");
	else
	{
		CRect rectClient;
		GetClientRect(rectClient);
		AdjustRect(FALSE, rectClient);
		for(int i = 0; i < m_numTabs; i++)
			m_wnds[i]->SetWindowPos(&wndTop, rectClient.left, rectClient.top,
				rectClient.Width(), rectClient.Height(), i == sel ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);
	}
	Invalidate();
}


void CInputOutputTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CTabCtrl::OnSize(nType, cx, cy);
	ActivateTabDialogs();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CInputOutputTabCtrl::UpdateEntitySelection( CeditorDoc *activeDoc )
{
	((CEntityOutputWnd*)m_wnds[0])->UpdateEntitySelection(activeDoc);
}




BEGIN_MESSAGE_MAP(CInputOutputWndPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CInputOutputWndPane::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CDockablePane::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	m_inoutWnd.Create( IDD_INOUTPUTS, this );
	m_inoutWnd.ShowWindow(SW_SHOW);

	return 0;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CInputOutputWndPane::OnSize( UINT nType, int cx, int cy )
{
	CDockablePane::OnSize(nType, cx, cy);

	//m_inoutWnd.MoveWindow(0,0,cx,cy);
	m_inoutWnd.SetWindowPos(&wndTop, 0, 0, cx, cy, NULL);
}