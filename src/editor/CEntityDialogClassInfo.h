// editor/CEntityDialogClassInfo.h
// editor/CEntityDialogClassInfo.h
// editor/CEntityDialogClassInfo.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once
#include "afxwin.h"


// CEntityDialogClassInfo-Dialogfeld
class CEditorEntity;
class CEntityKeyValue;
class CEntityDialogClassInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CEntityDialogClassInfo)

public:
	CEntityDialogClassInfo(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CEntityDialogClassInfo();

// Dialogfelddaten
	enum { IDD = IDD_ENTITY_CLASS_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnInitDialog( void );
public:
	CComboBox m_cbClass;
	CListBox m_lbKey;
	CStatic m_gbValue;
	CEdit m_ecHelp;
	afx_msg void OnCbnSelchangeCbClass();
	afx_msg void OnLbnSelchangeListKey();
	CEdit m_ecValueRaw;
	
	void SetEditEntity( CEditorEntity *pEnt ) { m_editEnt = pEnt; }
	void UpdateCurrentParam( void );
	void UpdateColor(COLORREF color);
private:
	CEntityKeyValue *m_currentKey;
	CEditorEntity *m_editEnt;
	void CreateValueTypeDialogs( void );
	std::map<int,CDialogEx*> m_valueTypesMap;
	CDialogEx *m_activeDialog;
	bool m_bClosing;
public:
	afx_msg void OnBnClickedSelectColor();
	afx_msg void OnEnKillfocusEditValueRawText();
	virtual void OnOK();
	virtual void OnCancel();
};
