// starter/starter.h
// starter/starter.h
// starter/starter.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


// starter.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CstarterApp:
// See starter.cpp for the implementation of this class
//

class CstarterApp : public CWinApp
{
public:
	CstarterApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CstarterApp theApp;