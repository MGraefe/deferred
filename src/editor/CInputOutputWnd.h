// editor/CInputOutputWnd.h
// editor/CInputOutputWnd.h
// editor/CInputOutputWnd.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#include "afxcmn.h"

class CEntityOutputWnd;
class CeditorDoc;

class CInputOutputTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CInputOutputTabCtrl)

public:
	CInputOutputTabCtrl();
	virtual ~CInputOutputTabCtrl();
	void InitDialogs();
	void UpdateEntitySelection(CeditorDoc *activeDoc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

private:
	static const int m_numTabs = 1;
	int m_wndIds[m_numTabs];
	CDialogEx *m_wnds[m_numTabs];
	CString m_wndNames[m_numTabs];
	int m_lastSizeX;
	int m_lastSizeY;

	void ActivateTabDialogs();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


class CInputOutputWnd : public CDialogEx
{
	DECLARE_DYNAMIC(CInputOutputWnd)

public:
	CInputOutputWnd(CWnd* pParent = NULL); 
	virtual ~CInputOutputWnd();

	enum { IDD = IDD_INOUTPUTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX); 
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	CInputOutputTabCtrl m_tabs;
	virtual BOOL OnInitDialog();
	void UpdateEntitySelection(CeditorDoc *activeDoc);
	virtual void OnOK() {};
	virtual void OnCancel() {};

private:
	CEntityOutputWnd *m_outputWnd;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};



class CInputOutputWndPane : public CDockablePane
{
public: 
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CInputOutputWnd *GetInOutWnd( void ) { return &m_inoutWnd; }

	DECLARE_MESSAGE_MAP()

private:
	CInputOutputWnd m_inoutWnd;
};


