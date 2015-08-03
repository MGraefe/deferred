// util/CEventManager.h
// util/CEventManager.h
// util/CEventManager.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__CEventManager_H__
#define deferred__util__CEventManager_H__

#include "eventlist.h"
#include "basic_types.h"
#include <list>
#include <map>
#include <queue>
#include <vector>
#include "instream.h"


typedef std::list< IEventListener* > EventListenerList;
typedef std::map< EventType, EventListenerList* > EventListenerMap;
typedef std::pair< EventType, EventListenerList* > EventListerMapPair;
typedef std::list< IEvent* > EventQueue;


class CEventComparison
{
public:
	bool operator()( const IEvent *pEvtLeft, const IEvent *pEvtRight ) const {
		return pEvtLeft->GetProcessTime() > pEvtRight->GetProcessTime();
	}
};

typedef std::priority_queue<IEvent*, std::vector<IEvent*>, CEventComparison> DelayedEventQueue;


#define DEBUG_EVENTMGR

class CEventManager
{
public:
	//Location is LOCATION_CLIENT, or LOCATION_SERVER
	CEventManager( int iLocation );
	~CEventManager();
	void StartLogging( const char *pFileName );
	void StopLogging( void );
	void RegisterListener( const EventType type, IEventListener* const pEvtListener );
	void UnregisterListener( const EventType type, IEventListener* const pEvtListener );
	void ProcessEventQueue( float curtime );
	void AddEventToQueue( IEvent* const pEvent, const int &iRecieverID = RECIEVER_ID_ALL );
	void AddDelayedEventToQueue( IEvent* const pEvent, float curtimeconst, int RecieverID = RECIEVER_ID_ALL );
	void RemoveEventFromQueue( IEvent* const pEvent, const bool bRemoveAll = false );
	void FireEventImmediate( IEvent* const pEvent );
	inline int GetLocation( void ) { return m_iLocation; }

private:
	EventListenerMap m_EvtLstMap;
	EventQueue m_EvtQueue;
	DelayedEventQueue m_DelayedEvtQueue;
	int m_iLocation;
	char m_LogFileName[128];
	bool m_bLogging;
	std::ofstream m_os;
};


#endif
