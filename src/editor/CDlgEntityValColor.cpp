// editor/CDlgEntityValColor.cpp
// editor/CDlgEntityValColor.cpp
// editor/CDlgEntityValColor.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
// CDlgEntityValColor.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "CDlgEntityValColor.h"
#include "afxdialogex.h"


// CDlgEntityValColor-Dialogfeld

IMPLEMENT_DYNAMIC(CDlgEntityValColor, CDialogEx)

CDlgEntityValColor::CDlgEntityValColor(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgEntityValColor::IDD, pParent)
{

}

CDlgEntityValColor::~CDlgEntityValColor()
{
}

void CDlgEntityValColor::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEntityValColor, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SELECT_COLOR, &CDlgEntityValColor::OnBnClickedBtnSelectColor)
END_MESSAGE_MAP()


// CDlgEntityValColor-Meldungshandler


void CDlgEntityValColor::OnBnClickedBtnSelectColor()
{
	//Escallade message to parent
	if( GetParent() )
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(IDC_BTN_SELECT_COLOR, BN_CLICKED) );
}
