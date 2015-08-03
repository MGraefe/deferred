// editor/CToolsWnd.h
// editor/CToolsWnd.h
// editor/CToolsWnd.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once


#include "resource.h"


class CToolsWnd : public CDialogEx
{
	DECLARE_DYNAMIC(CToolsWnd)

public:
	CToolsWnd(CWnd* pParent = NULL);
	virtual ~CToolsWnd();

	CButton *GetButton(tools_e tool);
	void DeactivateButtons( void );

	enum { IDD = IDD_TOOLS_WND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL CToolsWnd::PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK() {};
	virtual void OnCancel() {};
	afx_msg void OnBnClickedButtonToolCam();
	virtual BOOL OnInitDialog();
};
