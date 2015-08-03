// server/ServerInterfaces.cpp
// server/ServerInterfaces.cpp
// server/ServerInterfaces.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "ServerInterfaces.h"
#include "COutputManager.h"

gpGlobals_t gGlobals_Obj;
gpGlobals_t *gpGlobals = &gGlobals_Obj;

namespace singletons{

IGamePhysics *g_pPhysics = NULL;
CEventManager *g_pEvtMgr = NULL;
CEntityList *g_pEntityList = NULL;
CCTime *g_pTimer = NULL;
CServerMgr *g_pServerMgr = NULL;
CWorld *g_pWorld = NULL;
ConsoleHandler *g_pConsoleHandler = NULL;
COutputManager *g_pOutputManager = NULL;
IRealtimeDebugInterface *g_pRealtimeDebugInterface = NULL;


void Init( void )
{
	g_pOutputManager = new COutputManager();

	g_pTimer = new CCTime( gpGlobals );
	g_pTimer->InitTime();

	g_pEntityList = new CEntityList();

	g_pEvtMgr = new CEventManager( LOCATION_SERVER );
	//g_pEvtMgr->StartLogging( "log/serverlog.txt" );

	g_pPhysics = new CBulletPhysics();
	g_pPhysics->VInitialize();
}

void Cleanup( void )
{
	//delete g_pPhysics;
	delete g_pEvtMgr;
	delete g_pEntityList;
	delete g_pServerMgr;
	delete g_pTimer;
}


};
