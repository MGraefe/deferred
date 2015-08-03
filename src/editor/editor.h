// editor/editor.h
// editor/editor.h
// editor/editor.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

// editor.h : main header file for the editor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include <util/CEntityOutput.h>

class ConsoleHandler;
// CeditorApp:
// See editor.cpp for the implementation of this class
//
class CMainFrame;
class CeditorApp : public CWinAppEx
{
	DECLARE_MESSAGE_MAP()

public:
	CeditorApp();
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	CMainFrame *GetMainFrame(void) { return (CMainFrame*)m_pMainWnd; }

	ConsoleHandler *GetConsoleHandler( void ) { return m_pConsoleHandler; }
	std::vector<CEntityOutput> &GetOutputCopyPasteList() { return m_outputCopyPasteList; }

	void ToggleConVar(const std::string &name);

	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	
	
	afx_msg void OnViewTogglewater();
	afx_msg void OnAppAbout();
	afx_msg void OnOpenEntityProperties();
	
	afx_msg void OnEditNewentity();
	afx_msg void OnUpdateEditNewentity(CCmdUI *pCmdUI);
	afx_msg void OnViewTogglelighting();
	afx_msg void OnEditDuplicateselected();
	afx_msg void OnUpdateEditDuplicateselected(CCmdUI *pCmdUI);
	CLIPFORMAT GetEntityClipboardFormat() const;

private:
	ConsoleHandler *m_pConsoleHandler;
	std::vector<CEntityOutput> m_outputCopyPasteList;
	CLIPFORMAT m_entityClipboardFormat;

};

extern CeditorApp theApp;
