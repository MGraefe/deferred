// client/CAppInterfaces.cpp
// client/CAppInterfaces.cpp
// client/CAppInterfaces.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CAppInterfaces.h"
#include "CButtonInputMgr.h"

CAppInterfaces g_AppInterfObj;
CAppInterfaces *const g_AppInterf = &g_AppInterfObj;

CAppInterfaces::CAppInterfaces()
{
	NULL_INTERFACE( ButtonInputMgr );
	Init();
}

CAppInterfaces::~CAppInterfaces()
{
	Cleanup();
}

void CAppInterfaces::Init( void )
{
	INIT_INTERFACE( CButtonInputMgr, ButtonInputMgr );
}


void CAppInterfaces::Cleanup( void )
{
	CLEANUP_INTERFACE( ButtonInputMgr );
}
