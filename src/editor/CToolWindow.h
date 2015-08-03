// editor/CToolWindow.h
// editor/CToolWindow.h
// editor/CToolWindow.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once




class CToolWindow : public CDockablePane
{
	DECLARE_DYNCREATE(CToolWindow)

public:
	CToolWindow(); 
	virtual ~CToolWindow();

public:
	enum { IDD = ID_TOOLS_WND_PANE };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

	DECLARE_MESSAGE_MAP()
public:
	int m_ButtonPtr;
	int m_ButtonCam;
};


