// starter/starterDlg.h
// starter/starterDlg.h
// starter/starterDlg.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


// starterDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "CMaskedEditCtrl.h"


// CstarterDlg dialog
class CstarterDlg : public CDialogEx
{
// Construction
public:
	CstarterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_STARTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CComboBox m_cbResolution;
	void InitResolutions( void );
	CEdit m_eIP;
	afx_msg void OnBnClickedOk();

	void SaveToReg();

	void InitMaps();
	CButton m_bFullscreen;
	int m_iConnectChoice;
	CComboBox m_cbMap;
	CMaskedEditCtrl m_eNickname;
};
