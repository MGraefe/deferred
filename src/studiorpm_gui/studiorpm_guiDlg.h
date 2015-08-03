// studiorpm_gui/studiorpm_guiDlg.h
// studiorpm_gui/studiorpm_guiDlg.h
// studiorpm_gui/studiorpm_guiDlg.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

// studiorpm_guiDlg.h: Headerdatei
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CstudiorpmGuiDlg-Dialogfeld
class ChildProcess;
class CstudiorpmGuiDlg : public CDialogEx
{
// Konstruktion
public:
	CstudiorpmGuiDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_STUDIORPM_GUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditFileInput;
	CEdit m_EditFileOutput;
	CEdit m_EditTexturePath;
	CEdit m_EditScale;
	CEdit m_EditConsole;
	CProgressCtrl m_ProgressBar;
	afx_msg void OnBnClickedBtnBrowseInput();
	afx_msg void OnBnClickedBtnBrowseOutput();
	afx_msg void OnBnClickedBtnScaleDefault();
	afx_msg void OnBnClickedBtnScaleInchDm();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedBtnOutputDefault();
	void AddTextToLog( CString str );

private:
	ChildProcess *m_childProcess;
	void SpawnChildProcess( void );
public:
	CEdit m_EditRotate;
	afx_msg void OnBnClickedBtnRotateDefault();
};
