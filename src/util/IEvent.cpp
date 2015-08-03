// util/IEvent.cpp
// util/IEvent.cpp
// util/IEvent.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "IEvent.h"
#include "eventlist.h"
#include "error.h"

namespace ev
{
NOINLINE EventDictionary &GetEventDictionary( void )
{
	static EventDictionary s_EventDictionary( ev::LAST_EVENT, NULL );
	return s_EventDictionary;
}


NOINLINE EventNameDictionary &GetEventNameDictionary( void )
{
	static EventNameDictionary s_EventNameDictionary( ev::LAST_EVENT, NULL );
	return s_EventNameDictionary;
}


NOINLINE CEventCounter* GetEventCounter( void )
{
	static CEventCounter s_EventCounter;
	return &s_EventCounter;
}

static const char EventErrorName[] = "EVENTNAME NOT FOUND!!";

const char *GetEventStringName( const EventType &type )
{
	if((size_t)type < GetEventDictionary().size())
		return GetEventNameDictionary()[type];
	else
		return NULL;
}

IEventFactory* GetEventFactory( const EventType &eventname )
{
	if((size_t)eventname < GetEventDictionary().size())
		return GetEventDictionary()[eventname];
	else
		return NULL;
}




}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int IEventFactory::GetLocation( void ) const
{
	return m_iLocation;
}
