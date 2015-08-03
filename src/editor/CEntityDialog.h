// editor/CEntityDialog.h
// editor/CEntityDialog.h
// editor/CEntityDialog.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once
#include "afxcmn.h"
#include "CEntityDialogClassInfo.h"
#include "afxwin.h"

// CEntityDialog-Dialogfeld
class CEntityDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CEntityDialog)
	DECLARE_BASECLASS(CDialogEx)

public:
	CEntityDialog(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CEntityDialog();

// Dialogfelddaten
	enum { IDD = IDD_ENTITY_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tabControlMain;
	CEntityDialogClassInfo m_dialogClassInfo;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnInitDialog();

private:
	CEditorEntity *m_oldEntity;
	CEditorEntity *m_newEntity;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedApply();
	CButton m_btnApply;
};
