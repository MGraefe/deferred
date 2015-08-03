// editor/CEntityDialog.cpp
// editor/CEntityDialog.cpp
// editor/CEntityDialog.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
// CEntityDialog.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "CEntityDialog.h"
#include "afxdialogex.h"
#include "CEntityDialogClassInfo.h"
#include "editorDoc.h"
#include "CEditorEntity.h"
#include "CEditorEntityList.h"

// CEntityDialog-Dialogfeld

IMPLEMENT_DYNAMIC(CEntityDialog, CDialogEx)

CEntityDialog::CEntityDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEntityDialog::IDD, pParent)
{
	m_oldEntity = NULL;
	m_newEntity = NULL;
}

CEntityDialog::~CEntityDialog()
{
}

void CEntityDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabControlMain);
	DDX_Control(pDX, IDC_APPLY, m_btnApply);
}


BEGIN_MESSAGE_MAP(CEntityDialog, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CEntityDialog::OnTcnSelchangeTab1)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDOK, &CEntityDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CEntityDialog::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_APPLY, &CEntityDialog::OnBnClickedApply)
END_MESSAGE_MAP()


// CEntityDialog-Meldungshandler


void CEntityDialog::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Fügen Sie hier Ihren Kontrollbehandlungscode für die Benachrichtigung ein.
	*pResult = 0;
}


int CEntityDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
BOOL CEntityDialog::OnInitDialog()
{
	if( !CDialogEx::OnInitDialog() )
		return FALSE;

	//Create copy of editable entity
	m_oldEntity = GetActiveDocument()->GetSelectedEntity(0);
	m_newEntity = m_oldEntity->Clone();
	Assert( m_oldEntity && m_newEntity );
	GetActiveDocument()->GetEntityList()->ReplaceEntity( m_oldEntity, m_newEntity );
	GetActiveDocument()->SelectSingleEntity( m_newEntity );
	m_dialogClassInfo.SetEditEntity(m_newEntity);

	//Get Tab Control
	CTabCtrl *pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_TAB1);

	//Create Tab Content dialog and adjust
	m_dialogClassInfo.Create(IDD_ENTITY_CLASS_INFO, pTabCtrl);

	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT | TCIF_PARAM;
	tcItem.pszText = _T("Class Info");
	tcItem.lParam = (LPARAM)&m_dialogClassInfo;
	pTabCtrl->InsertItem(0, &tcItem);

	RECT wndrect;
	GetClientRect(&wndrect);
	wndrect.top -= 15;
	wndrect.bottom -= 30;
	pTabCtrl->AdjustRect( FALSE, &wndrect );
	pTabCtrl->MoveWindow( &wndrect );

	CRect rect;
	pTabCtrl->GetItemRect(0, &rect);
	m_dialogClassInfo.SetWindowPos(NULL, rect.left, rect.bottom + 1,
		wndrect.right-wndrect.left-10, wndrect.bottom-wndrect.top-30, /*SWP_NOSIZE |*/ SWP_NOZORDER );
	m_dialogClassInfo.ShowWindow(SW_SHOW);

	return TRUE;
}


void CEntityDialog::OnBnClickedOk()
{
	Assert( m_oldEntity && m_newEntity );

	delete m_oldEntity;
	m_oldEntity = NULL;

	GetActiveDocument()->SelectSingleEntity( m_newEntity );
	m_dialogClassInfo.SetEditEntity( NULL );

	BaseClass::OnOK();
}


void CEntityDialog::OnBnClickedCancel()
{
	Assert( m_newEntity && m_oldEntity );

	//revert changes
	GetActiveDocument()->GetEntityList()->ReplaceEntity( m_newEntity, m_oldEntity );
	delete m_newEntity;
	m_newEntity = NULL;
	GetActiveDocument()->SelectSingleEntity( m_oldEntity );
	m_dialogClassInfo.SetEditEntity( NULL );

	BaseClass::OnCancel();
}


void CEntityDialog::OnBnClickedApply()
{
	Assert( m_newEntity && m_oldEntity );

	delete m_oldEntity;
	m_oldEntity = NULL;

	m_oldEntity = m_newEntity;
	m_newEntity = m_oldEntity->Clone();
	GetActiveDocument()->GetEntityList()->ReplaceEntity( m_oldEntity, m_newEntity );
	m_dialogClassInfo.SetEditEntity(m_newEntity);

	GetActiveDocument()->SelectSingleEntity(m_newEntity);
}


