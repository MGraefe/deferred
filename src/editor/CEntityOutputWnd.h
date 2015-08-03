// editor/CEntityOutputWnd.h
// editor/CEntityOutputWnd.h
// editor/CEntityOutputWnd.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#include "afxcmn.h"
#include "COutputListWnd.h"
#include "afxwin.h"
#include "CEntityNameComboBox.h"
#include "CEntityOutputComboBox.h"

//Entity Output Window Sub Item indices


class CeditorDoc;
class CEntityOutputWnd : public CDialogEx
{
	DECLARE_DYNAMIC(CEntityOutputWnd)

public:
	CEntityOutputWnd(CWnd* pParent = NULL); 
	virtual ~CEntityOutputWnd();

	enum { IDD = IDD_OUTPUTS };

	void UpdateEntitySelection(CeditorDoc *activeDoc);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void UpdateList();
	void ArrangeChildren();
	void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	void OnListSelectionChanged();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

private:
	void UpdateToList();
	void ClearInputMask();
	void EnableInputMask(BOOL bEnable = TRUE);
	void UpdateEventComboBox();
	void UpdateInputComboBox();
	void UpdateTargetComboBox();
	void AddInputsForClassname(const std::string &classname);

public:
	COutputListWnd m_list;
	CCustomComboBox m_cbEvt;
	CEntityNameComboBox m_cbTarget;
	CCustomComboBox m_cbInput;
	CCustomComboBox m_cbParam;
	CEdit m_eDelay;
	CButton m_bOnlyOnce;

private:
	static const int m_listSpacingBottom;
	int m_lastSizeX;
	int m_lastSizeY;
	bool m_enableOutputToList;

public:
	afx_msg void OnCbnEditchangeOutputsCombo4();
	afx_msg void OnCbnSelchangeOutputsCombo4();
	afx_msg void OnCbnEditchangeOutputsCombo3();
	afx_msg void OnCbnSelchangeOutputsCombo3();
	afx_msg void OnCbnEditchangeOutputsCombo2();
	afx_msg void OnCbnSelchangeOutputsCombo2();
	afx_msg void OnCbnEditchangeOutputsCombo1();
	afx_msg void OnCbnSelchangeOutputsCombo1();
	afx_msg void OnEnChangeOutputsEdit1();
	afx_msg void OnBnClickedOutputsCheck1();
	afx_msg void OnBnClickedOutputsButtonAdd();
	afx_msg void OnBnClickedOutputsButtonCopy();
	afx_msg void OnBnClickedOutputsButtonPaste();
	afx_msg void OnBnClickedOutputsButtonDelete();
	virtual void OnOK() {};
	virtual void OnCancel() {};
	CButton m_bAdd;
	CButton m_bCopy;
	CButton m_bPaste;
	CButton m_bDelete;

	afx_msg void OnCbnKillfocusOutputsCombo3();
};
