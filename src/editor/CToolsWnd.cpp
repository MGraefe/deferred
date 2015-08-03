// editor/CToolsWnd.cpp
// editor/CToolsWnd.cpp
// editor/CToolsWnd.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
// CToolsWnd.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "CToolsWnd.h"
#include "afxdialogex.h"
#include "editorDoc.h"
#include "MainFrm.h"

// CToolsWnd-Dialogfeld

IMPLEMENT_DYNAMIC(CToolsWnd, CDialogEx)

CToolsWnd::CToolsWnd(CWnd* pParent /*=NULL*/)
	: CDialogEx(CToolsWnd::IDD, pParent)
{

}

CToolsWnd::~CToolsWnd()
{
}

void CToolsWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CToolsWnd, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_TOOL_CAM, &CToolsWnd::OnBnClickedButtonToolCam)
END_MESSAGE_MAP()


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
BOOL CToolsWnd::PreTranslateMessage(MSG* pMsg)
{
	//// since 'IsDialogMessage' will eat frame window accelerators,
	//// call all frame windows' PreTranslateMessage first	
	//if (pMsg->message == WM_KEYDOWN /*&& HIBYTE(GetKeyState(VK_CONTROL))*/ )
	//{
	//	// start with first parent frame
	//	CFrameWnd* pFrameWnd = GetParentFrame();  
	//	while (pFrameWnd != NULL)
	//	{
	//		// allow parent frames to translate before DialogMessage does
	//		if (pFrameWnd->PreTranslateMessage(pMsg))
	//			return TRUE;

	//		// try parent frames until there are no parent frames
	//		pFrameWnd = pFrameWnd->GetParentFrame();
	//	}
	//}	

	HACCEL hAccel = ((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetAccelTable();
	if(hAccel && ::TranslateAccelerator(AfxGetApp()->m_pMainWnd->m_hWnd, hAccel, pMsg))
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolsWnd::DeactivateButtons( void )
{
	GetButton(TOOL_EDITCAM)->SetState(FALSE);
}


void CToolsWnd::OnBnClickedButtonToolCam()
{
	DeactivateButtons();
	GetButton(TOOL_EDITCAM)->SetState(TRUE);
	if( GetActiveDocument() )
		GetActiveDocument()->SetActiveTool(TOOL_EDITCAM);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CButton * CToolsWnd::GetButton( tools_e tool )
{
	int id;
	switch(tool)
	{
	case TOOL_EDITCAM:
		id = IDC_BUTTON_TOOL_CAM;
		break;
	default:
		return NULL;
	}

	return (CButton*)GetDlgItem(id);
}


BOOL CToolsWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	DeactivateButtons();
	GetButton(TOOL_EDITCAM)->SetState(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}
