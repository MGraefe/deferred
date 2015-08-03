// studiorpm_gui/studiorpm_gui.h
// studiorpm_gui/studiorpm_gui.h
// studiorpm_gui/studiorpm_gui.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

// studiorpm_gui.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'stdafx.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CstudiorpmGuiApp:
// Siehe studiorpm_gui.cpp für die Implementierung dieser Klasse
//

class CstudiorpmGuiApp : public CWinApp
{
public:
	CstudiorpmGuiApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CstudiorpmGuiApp theApp;
