// editor/CDlgNewEntity.cpp
// editor/CDlgNewEntity.cpp
// editor/CDlgNewEntity.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
// CDlgNewEntity.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "editor.h"
#include "CDlgNewEntity.h"
#include "afxdialogex.h"
#include "CEntityRegister.h"

// CDlgNewEntity-Dialogfeld

IMPLEMENT_DYNAMIC(CDlgNewEntity, CDialogEx)

CDlgNewEntity::CDlgNewEntity(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgNewEntity::IDD, pParent)
{

}

CDlgNewEntity::~CDlgNewEntity()
{
}

void CDlgNewEntity::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NEW_ENT_CMB_ENTITY_TYPE, m_cbEntityType);
}


BEGIN_MESSAGE_MAP(CDlgNewEntity, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgNewEntity::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgNewEntity-Meldungshandler


void CDlgNewEntity::OnBnClickedOk()
{
	CString selClass;
	m_cbEntityType.GetLBText( m_cbEntityType.GetCurSel(), selClass );

	m_entityType = CT2A(selClass);

	CDialogEx::OnOK();
}


BOOL CDlgNewEntity::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	std::vector<std::string> entNames;
	GetEntityRegister()->GetEntityNames(entNames);

	for( UINT i = 0; i < entNames.size(); i++ )
	{
		if( entNames[i][0] != '@' )
			m_cbEntityType.AddString( CA2T(entNames[i].c_str()) );
	}

	if( entNames.size() > 0 )
		m_cbEntityType.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// AUSNAHME: OCX-Eigenschaftenseite muss FALSE zurückgeben.
}
