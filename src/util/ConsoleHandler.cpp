// util/ConsoleHandler.cpp
// util/ConsoleHandler.cpp
// util/ConsoleHandler.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "ConsoleHandler.h"
#include <iostream>
#include "error.h"
#include "CConVar.h"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void ConsoleHandler::RemoveCallback( ConsoleHandlerInputCallbackFunc *pCb, ConsoleHandlerGetConvarCallback *pGCb )
{
	m_mutex.SetOrWait();

	for( size_t i = 0; i < m_callbacks.size(); i++ )
		if( m_callbacks[i] == pCb )
			m_callbacks.erase( m_callbacks.begin() + i );

	for( size_t i = 0; i < m_getcallbacks.size(); i++ )
		if( m_getcallbacks[i] == pGCb )
			m_getcallbacks.erase( m_getcallbacks.begin() + i );

	m_mutex.Release();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void ConsoleHandler::AddCallback( ConsoleHandlerInputCallbackFunc *pCb, ConsoleHandlerGetConvarCallback *pGCb )
{
	m_mutex.SetOrWait();
	m_callbacks.push_back( pCb );
	m_getcallbacks.push_back( pGCb );
	m_mutex.Release();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void ConsoleHandler::ThreadRun( void )
{
	char buf[256];
	while( true )
	{
		//std::cin >> buf;
		std::cin.getline( buf, 256 );

		m_mutex.SetOrWait();
		m_storedInput.push_back( std::string(buf) );
		m_mutex.Release();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
ConsoleHandler::ConsoleHandler() : CThread( "Console Input Handler" )
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void ConsoleHandler::HandleStoredInput( void )
{
	m_mutex.SetOrWait();
	while( !m_storedInput.empty() )
	{
		std::string &input = *m_storedInput.begin();

		bool foundVar = false;
		for( size_t i = 0; i < m_callbacks.size(); i++ )
		{
			if( m_callbacks[i](input.c_str()) )
				foundVar = true;
		}

		if( !foundVar )
			ConsoleMessage( "Cannot interpret input or cannot find variable/command in input \"%s\"", input.c_str() );

		m_storedInput.pop_front();
	}
	m_mutex.Release();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CConVarBase * ConsoleHandler::GetConVarBase( const std::string &name )
{
	m_mutex.SetOrWait();
	for( size_t i = 0; i < m_getcallbacks.size(); i++ )
	{
		CConVarBase *cvar = m_getcallbacks[i](name);
		if( cvar != NULL )
		{
			m_mutex.Release();
			return cvar;
		}
	}
	m_mutex.Release();
	return NULL;
}
