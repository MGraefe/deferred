// editor/CDlgEntityValColor.h
// editor/CDlgEntityValColor.h
// editor/CDlgEntityValColor.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once


// CDlgEntityValColor-Dialogfeld

class CDlgEntityValColor : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEntityValColor)

public:
	CDlgEntityValColor(CWnd* pParent = NULL);   // Standardkonstruktor
	virtual ~CDlgEntityValColor();

// Dialogfelddaten
	enum { IDD = IDD_ENTITY_VAL_COLOR_BRIGHTNESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSelectColor();
};
