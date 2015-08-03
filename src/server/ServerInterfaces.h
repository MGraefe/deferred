// server/ServerInterfaces.h
// server/ServerInterfaces.h
// server/ServerInterfaces.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__server__ServerInterfaces_H__
#define deferred__server__ServerInterfaces_H__

#include "CBulletPhysics.h"
#include <util/CEventManager.h>
#include "CEntityList.h"
#include <util/timer.h>
#include "CServerMgr.h"
#include "CWorld.h"
#include "COutputManager.h"
#include <util/IRealtimeDebugInterface.h>

class ConsoleHandler;


namespace singletons
{
	extern IGamePhysics *g_pPhysics;
	extern CEventManager *g_pEvtMgr;
	extern CEntityList *g_pEntityList;
	extern CCTime *g_pTimer;
	extern CServerMgr *g_pServerMgr;
	extern CWorld *g_pWorld;
	extern ConsoleHandler *g_pConsoleHandler;
	extern COutputManager *g_pOutputManager;
	extern IRealtimeDebugInterface *g_pRealtimeDebugInterface;

	extern void Init( void );
	extern void Cleanup( void );
};

extern gpGlobals_t *gpGlobals;

#define CREATE_ENTITY(classname) (classname*)singletons::g_pEntityList->AddEntity( (CBaseEntity*)(new classname()) )

#endif
