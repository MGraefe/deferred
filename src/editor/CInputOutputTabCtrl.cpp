// editor/CInputOutputTabCtrl.cpp
// editor/CInputOutputTabCtrl.cpp
// editor/CInputOutputTabCtrl.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------

// D:\Programming\deferred\projectsrc\src\editor\CInputOutputTabCtrl.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "D:\Programming\deferred\projectsrc\src\editor\CInputOutputTabCtrl.h"
#include "afxdialogex.h"


// CInputOutputTabCtrl-Dialogfeld

IMPLEMENT_DYNAMIC(CInputOutputTabCtrl, CDialogEx)

CInputOutputTabCtrl::CInputOutputTabCtrl(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInputOutputTabCtrl::IDD, pParent)
{

}

CInputOutputTabCtrl::~CInputOutputTabCtrl()
{
}

void CInputOutputTabCtrl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInputOutputTabCtrl, CDialogEx)
END_MESSAGE_MAP()


// CInputOutputTabCtrl-Meldungshandler
