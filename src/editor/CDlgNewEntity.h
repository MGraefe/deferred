// editor/CDlgNewEntity.h
// editor/CDlgNewEntity.h
// editor/CDlgNewEntity.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once
#include "afxwin.h"


// CDlgNewEntity-Dialogfeld

class CDlgNewEntity : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgNewEntity)

public:
	CDlgNewEntity(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CDlgNewEntity();

// Dialogfelddaten
	enum { IDD = IDD_DLG_NEW_ENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_cbEntityType;
public:
	afx_msg void OnBnClickedOk();
	std::string m_entityType;
	virtual BOOL OnInitDialog();
};
