// server/servermain.cpp
// server/servermain.cpp
// server/servermain.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "servermain.h"
#include "ServerInterfaces.h"
#include <util/timer.h>
#include "CPlayerCar.h"
#include "CPropPhysics.h"
#include "CStaticModel.h"
#include <util/CUnitTest.h>
#include "TimeSyncerServer.h"

#include <util/CConfigParser.h>
#include <util/ConsoleHandler.h>
#include <util/CConVar.h>

//HANDLE HMainThread = 0;
//DWORD MainThreadId = 0;

serverstartdata_t *g_ServerData;

//#define WORLD_FILENAME "def_coast1"
//#define WORLD_FILENAME "models/map.rpm"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void InitGame( void )
{
	singletons::g_pWorld = new CWorld(g_ServerData->mapname);
	singletons::g_pEntityList->AddEntity(singletons::g_pWorld);
	if( !singletons::g_pWorld->init() )
		error("Server: Error loading world \"%s\"", g_ServerData->mapname);
}


void InitConsoleHandler( void )
{
// 	singletons::g_pConsoleHandler = new ConsoleHandler();
// 	singletons::g_pConsoleHandler->AddCallback( &CConVarList::HandleConsoleInput, &CConVarList::GetConVarBase );
// 	singletons::g_pConsoleHandler->Start();
// 
// 	CConfigParser configParser;
// 	configParser.ParseConfigFile("config/server.cfg", singletons::g_pConsoleHandler);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void InitRealtimeDebugInterface()
{
	if(g_ServerData->pDebugInterface != NULL)
	{
		ConsoleMessage("Server: RealtimeDebugInterface ACTIVE");
		singletons::g_pRealtimeDebugInterface = (IRealtimeDebugInterface*)g_ServerData->pDebugInterface;
	}
	else
	{
		ConsoleMessage("Server: RealtimeDebugInterface NOT active.");
		singletons::g_pRealtimeDebugInterface = new CRealtimeDebugInterfaceDummy();
	}
}



class CServerThread : public CThread
{
public:
	CServerThread(serverstartdata_t *serverStartData) :
		CThread("ServerMain"),
		m_serverStartData(serverStartData)
	{}

	void ThreadRun();

private:
	serverstartdata_t * const m_serverStartData;
};


void CServerThread::ThreadRun()
{
#ifdef _WINDOWS
	TIMECAPS caps;
	timeGetDevCaps( &caps, sizeof(caps) );
	timeBeginPeriod( caps.wPeriodMin );
#endif
	InitConsoleHandler();

	ConsoleMessage("Launching Server on Port: %i with a tickrate of %i ticks/s", g_ServerData->port, g_ServerData->tickrate);

	InitRealtimeDebugInterface();
	ConsoleMessage("Realtime Debug Interface initialized");

	ev::EventSystemInit();
	ConsoleMessage("Event system initialized");

	CUnitTestList::RunAllTests();
	
	singletons::g_pServerMgr = new CServerMgr( *g_ServerData );
	singletons::g_pServerMgr->Init();
	ConsoleMessage("Server Manager initialized");

	singletons::Init();
	ConsoleMessage("Singletons initialized");

	InitGame();
	ConsoleMessage("Game initialized");

	CTimeSyncerServer *pTimeSyncer = new CTimeSyncerServer( g_ServerData->port );
	pTimeSyncer->Start();

	*m_serverStartData->pbIsUp = 1;

	while( *m_serverStartData->pbShutdown == 0 )
	{
		//update time
		unsigned int starttime = singletons::g_pTimer->GetTimeMs();
		singletons::g_pTimer->UpdateTime();

		singletons::g_pRealtimeDebugInterface->SetValue("ServerTick", std::to_string(gpGlobals->framecount).c_str());

		//look for new clients/recieve client input
		singletons::g_pServerMgr->LookForNewClients();
		singletons::g_pServerMgr->RecieveClientInput();

		//process events from clients (mainly input)
		singletons::g_pEvtMgr->ProcessEventQueue( gpGlobals->curtime );

		//update physical simulation
		singletons::g_pPhysics->VOnUpdate( gpGlobals->frametime );

		//sync physical simulation
		singletons::g_pPhysics->VSyncVisibleScene();

		//add begin snapshot message
		singletons::g_pEvtMgr->AddEventToQueue( new CFloatDataEvent( ev::SERVER_SNAPSHOT_START, gpGlobals->curtime ) );

		//update entities
		singletons::g_pEntityList->UpdateAllEntities();

		//update output system and call input functions
		singletons::g_pOutputManager->Update(singletons::g_pEntityList, gpGlobals->curtime);

		//update clients
		singletons::g_pServerMgr->UpdateClients();

		//send network changes
		singletons::g_pServerMgr->SendNetworkUpdates();

		//add a last event to the queue that symbolizes the end of the snapshot
		singletons::g_pEvtMgr->AddEventToQueue( new CFloatDataEvent( ev::SERVER_SNAPSHOT_END, gpGlobals->curtime ) ); 

		//process events to clients (mainly position updates etc.)
		singletons::g_pEvtMgr->ProcessEventQueue( gpGlobals->curtime );

		//send updates over the wire
		singletons::g_pServerMgr->TransmitServerOutput();

		const unsigned int targetFrameTime = 1000 / 50;
		unsigned int frametime = singletons::g_pTimer->GetTimeMs() - starttime;
		if( frametime < targetFrameTime )
			CCTime::Sleep( targetFrameTime - frametime );
		//Sleep(15);
	}

	singletons::Cleanup();
	pTimeSyncer->Stop();

#ifdef _WINDOWS
	timeEndPeriod( caps.wPeriodMin );
#endif

	*m_serverStartData->pbIsDown = 1;
}






SERVERDLL int servermain( const serverstartdata_t &data )
{
	//Init filesystem
	filesystem::Filesystem &filesys = filesystem::Filesystem::GetInstance();
	filesys.mountArchive("data.rpaf");

	g_ServerData = new serverstartdata_t( data );

	CServerThread *serverThread = new CServerThread(g_ServerData);

	if(data.bThreaded)
		serverThread->Start();
	else
		serverThread->StartSync();

	return SERVERCREATEERROR_NONE;
}


