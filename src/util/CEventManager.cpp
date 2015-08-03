// util/CEventManager.cpp
// util/CEventManager.cpp
// util/CEventManager.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CEventManager.h"
#include "error.h"
#include "eventlist.h"
#include "networking.h"

#include "timer.h"

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CEventManager::CEventManager( int iLocation )
{
	m_bLogging = false;
	m_iLocation = iLocation;

	//if( ev::LAST != sizeof(ev::eventvals_str)/sizeof(char*) )
	//	error_fatal( "eventvals_str is incomplete." );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CEventManager::~CEventManager()
{
	//Empty event queues
	while( !m_EvtQueue.empty() )
	{
		IEvent *pEvt = m_EvtQueue.front();
		m_EvtQueue.pop_front();
		delete pEvt;
	}

	while( !m_DelayedEvtQueue.empty() )
	{
		IEvent *pEvt = m_DelayedEvtQueue.top();
		m_DelayedEvtQueue.pop();
		delete pEvt;
	}

	//Empty listener-list-map
	EventListenerMap::iterator it = m_EvtLstMap.begin();
	EventListenerMap::iterator itEnd = m_EvtLstMap.end();
	for( ; it != itEnd; it++ )
		delete it->second;

	if( m_bLogging )
		m_os.close();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEventManager::StartLogging( const char *pFileName )
{
	strcpy( m_LogFileName, pFileName );
	m_os.open( pFileName, std::ios::app );
	if( !m_os.is_open() )
	{
		error( "Could not write to the logfile \"%s\"", pFileName );
		return;
	}
	m_bLogging = true;

	m_os << "\n\n/////////////////////////////////\nNEW SESSION\n/////////////////////////////////\n\n\n";
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEventManager::StopLogging( void )
{
	if( m_bLogging )
	{
		m_os.close();
		m_bLogging = false;
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEventManager::RegisterListener( const EventType type, IEventListener* const pEvtListener )
{
	EventListenerMap::iterator it = m_EvtLstMap.find( type );

	//does an event already exist in our map?
	if( it == m_EvtLstMap.end() )
	{
		//Event-Type is nonexistent in our map, so create an entry
		EventListenerList *list = new EventListenerList;
		list->push_back( pEvtListener );

		m_EvtLstMap.insert( EventListerMapPair( type, list ) );
	}
	else
	{
		//Search for an already-there Listener
		EventListenerList *list = it->second;
		for( EventListenerList::iterator listIt = list->begin(), listItEnd = list->end(); listIt != listItEnd; listIt++ )
		{
			if( *listIt == pEvtListener )
			{
				//ConsoleMessage( "RegisterListener(): Already there." );
				return; //already there
			}
		}

		//add new entry
		list->push_back( pEvtListener );
	}


	//If this is a event which is generated at the remote-side we need
	//to tell the remote side that this side wants to register with this event.
	//We do this by sending a simple Event.
	//The Network Code should register a CRemoteEventListener to the Event-Type
	//REMOTE_LISTENER_REGISTER before all other Listeners register themselves.
	EventDictionary &dict = ev::GetEventDictionary();
	IEventFactory *pFactory = dict[type];
	int iRemoteLocation = m_iLocation == LOCATION_SERVER ? LOCATION_CLIENT : LOCATION_SERVER;
	if( pFactory->GetLocation() == iRemoteLocation /*||
		ev::GetEventDictionary()[type]->GetLocation() == LOCATION_BOTH*/ )
	{
		this->AddEventToQueue( new CIntegerDataEvent( ev::REMOTE_LISTENER_REGISTER, type ) );
	}


	if( m_bLogging )
		m_os << "Registered listener "<< pEvtListener << " to event \"" << ev::GetEventStringName(type) << "\".\n";
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEventManager::UnregisterListener( const EventType type, IEventListener* const pEvtListener )
{
	if( type == ev::ALL_EVENTS )
	{
		for( int i = 0; i < ev::LAST_EVENT; i++ )
			UnregisterListener(i, pEvtListener);

#ifdef _DEBUG
		//Test listener list for integrity
		EventListenerMap::iterator it = m_EvtLstMap.begin();
		for( ; it != m_EvtLstMap.end(); it++ )
		{
		
			auto listIt = it->second->begin();
			for( ; listIt != it->second->end(); listIt++ )
			{
				Assert( (*listIt) != pEvtListener );
			}
		}
#endif
		return;
	}

	EventListenerMap::iterator mapIt = m_EvtLstMap.find( type );
	
	//No need to remove if event does not exist
	if( mapIt == m_EvtLstMap.end() )
		return;

	EventListenerList *list = mapIt->second;
	EventListenerList::iterator listIt = list->begin(), listItEnd = list->end();
	while( listIt != listItEnd )
	{
		if( (*listIt) == pEvtListener )
		{
			listIt = list->erase(listIt);

			if( m_bLogging )
				m_os << "Unregistered listener "<< pEvtListener << " to event \"" << ev::GetEventStringName(type) << "\".\n";

			//return; //We guarantee that every listener is only once inside the class
		}
		else
			listIt++;
	}

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEventManager::ProcessEventQueue( float curtime )
{
	//iterate through all events
	while( !m_EvtQueue.empty() )
	{
		IEvent *pEvt = *m_EvtQueue.begin();
		FireEventImmediate( pEvt );
		m_EvtQueue.pop_front();
	}

	//iterate through delayed event queue
	while( !m_DelayedEvtQueue.empty() )
	{
		Assert( GetLocation() == LOCATION_CLIENT );
		IEvent *pEvt = m_DelayedEvtQueue.top();
		if( pEvt->GetProcessTime() < curtime )
		{
			//First pop this element because FireEventImmediate might delete the event,
			//which seems to cause errors in the pop function if done before.
			m_DelayedEvtQueue.pop();
			FireEventImmediate(pEvt);
		}
		else
			return;
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEventManager::FireEventImmediate( IEvent *const pEvent )
{
	EventListenerMap::iterator mapIt = m_EvtLstMap.find( pEvent->GetType() );

	//no entry for this event, skip
	if( mapIt == m_EvtLstMap.end() )
	{
		//we are done with this event
		delete pEvent;
		return;
	}

	if( m_bLogging )
		m_os << "Firing event of type \"" << ev::GetEventStringName(pEvent->GetType()) << "\".\n";

	EventListenerList *ListenerList = mapIt->second;

	//iterate through all listeners registered to this event
	EventListenerList::iterator listIt = ListenerList->begin();
	EventListenerList::iterator listItEnd = ListenerList->end();
	for( ; listIt != listItEnd; listIt++ )
	{
		if( pEvent->GetRecvID() == RECIEVER_ID_ALL )
			(*listIt)->HandleEvent( pEvent );
		else
		{
			CRemoteEventListener *pRemoteListener = (CRemoteEventListener*)(*listIt);
			if( (!pRemoteListener && pEvent->GetRecvID() == RECIEVER_ID_LOCAL) || //is the event and the listener local?
				(pRemoteListener && pEvent->GetRecvID() == pRemoteListener->GetSocket()->GetClientID() ) ) //is it a remote listener which socket fits?
			{
				(*listIt)->HandleEvent( pEvent );
			}
		}
	}

	//we are done with this event
	delete pEvent;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEventManager::AddEventToQueue( IEvent *const pEvent, const int &iRecieverID )
{

	pEvent->SetRecieverID( iRecieverID );
	m_EvtQueue.push_back( pEvent );

	if( m_bLogging )
	{
		char locale[] = "(remote)";
		if( !pEvent->IsRemoteEvent() )
			strcpy( locale, "(local)" );
		m_os << "Added " << locale << " event \"" << ev::GetEventStringName(pEvent->GetType()) << "\" to queue\n";
	}
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEventManager::AddDelayedEventToQueue( IEvent *const pEvent, float curtime, int iRecieverID )
{
	Assert( this->GetLocation() == LOCATION_CLIENT );

	pEvent->SetRecieverID( iRecieverID );
	m_DelayedEvtQueue.push( pEvent );

	if( m_bLogging )
	{
		char locale[] = "remote";
		if( !pEvent->IsRemoteEvent() )
			strcpy( locale, "local" );
		m_os << "Added delayed (" << (pEvent->GetProcessTime()-curtime) <<") "
			<< locale << " event \"" << ev::GetEventStringName(pEvent->GetType()) << "\" to queue\n";
	}
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEventManager::RemoveEventFromQueue( IEvent *const pEvent, const bool bRemoveAll )
{
	//iterate through all events
	EventQueue::iterator queueIt = m_EvtQueue.begin();
	EventQueue::iterator queueItEnd = m_EvtQueue.end();
	for( ; queueIt != queueItEnd; queueIt++ )
	{
		if( (*queueIt) == pEvent )
		{
			m_EvtQueue.erase( queueIt );
			if( !bRemoveAll )
				return;
		}
	}

	if( m_bLogging )
		m_os << "Removed event \"" << ev::GetEventStringName(pEvent->GetType()) << "\" from queue\n";
}
