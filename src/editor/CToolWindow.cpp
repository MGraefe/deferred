// editor/CToolWindow.cpp
// editor/CToolWindow.cpp
// editor/CToolWindow.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
// CToolWindow.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "CToolWindow.h"
#include "targetver.h"

// CToolWindow

IMPLEMENT_DYNCREATE(CToolWindow, CDockablePane)

CToolWindow::CToolWindow()
	: CDockablePane()
	, m_ButtonPtr(0)
	, m_ButtonCam(0)
{

}

CToolWindow::~CToolWindow()
{
}

void CToolWindow::DoDataExchange(CDataExchange* pDX)
{
	CDockablePane::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CToolWindow, CDockablePane)
END_MESSAGE_MAP()


// CToolWindow-Diagnose

#ifdef _DEBUG
void CToolWindow::AssertValid() const
{
	CDockablePane::AssertValid();
}

#ifndef _WIN32_WCE
void CToolWindow::Dump(CDumpContext& dc) const
{
	CDockablePane::Dump(dc);
}
#endif
#endif //_DEBUG

