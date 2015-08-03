// client/CLoaderThread.h
// client/CLoaderThread.h
// client/CLoaderThread.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__client__CLoaderThread_H__
#define deferred__client__CLoaderThread_H__

#include <util/eventlist.h>
#include <list>
#include <util/CThread.h>
#include <util/CThreadMutex.h>
class CScriptSubGroup;
class CLoaderThread : public IEventListener, public CThread
{
public:
	CLoaderThread() : CThread( "LoaderThread" ), m_bFinishedLoading(false) { }
	bool HandleEvent( const IEvent *evt );
	void RegisterEvents( void );
	void UnregisterEvents( void );
	void StartLoading( void );
	void ThreadRun( void );
	bool HandleLoadEvent( const IEvent *pEvent );
	void LoadWorld( const char *scriptname );
	void LoadEntitiesFromMapFile( const char *pFilePath );

	//CThreadMutex *GetLoadMutex( void ) { return &m_LoadMutex; }
	//CThreadEvent *GetFinishedLoadingEvent( void ) { return &m_FinishedLoadingEvent; }

private:
	//static unsigned long __stdcall LoadThreadFunc( /*__in*/ void *lpParameter );
	void ParseAndAddPointLight( const CScriptSubGroup *grp );
	void LoadStaticWorldLights( const char *pFilePath );

private:
	std::list<IEvent*> m_EventList;
	//CThreadMutex m_LoadMutex;
	//CThreadEvent m_FinishedLoadingEvent;
	void *m_ThreadHandle;
	unsigned long m_ThreadId;
	bool m_bFinishedLoading;
};


extern CLoaderThread *g_pLoaderThread;

#endif
