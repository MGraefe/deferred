// renderer/CGLStateSaver.cpp
// renderer/CGLStateSaver.cpp
// renderer/CGLStateSaver.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "glheaders.h"

//CGLStateSaver StateSaver;

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CGLStateSaver::CGLStateSaver()
{
	ClearStates();
}


//---------------------------------------------------------------
// Purpose: Should be called when OpenGL is initialized 
//			(on start and on fullscreen change)
//---------------------------------------------------------------
void CGLStateSaver::ClearStates( void )
{
	m_ServerStates.clear();
	m_VertexAttribStates.clear();
	m_iLastVertexAttrib = 0;
	m_iLastActiveTexture = 0;
	m_iLastDepthFunc = 0;
	m_iLastCullFace = 0;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
bool CGLStateSaver::GetState( const UINT &state )
{
	StateMap::iterator it = m_ServerStates.find( state );
	if( it == m_ServerStates.end() )
		return false;
	else
		return it->second;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CGLStateSaver::Enable( UINT state )
{
	StateMap::iterator it = m_ServerStates.find( state );
	if( it == m_ServerStates.end() )
	{
		::glEnable( state ); //we dont know the status so go the safe way.
		m_ServerStates[state] = true;
	}
	else if( !it->second )
	{
		::glEnable( state );
		it->second = true;
	}
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CGLStateSaver::Disable( UINT state )
{
	StateMap::iterator it = m_ServerStates.find( state );
	if( it == m_ServerStates.end() )
	{
		::glDisable( state ); //we dont know the status so go the safe way.
		m_ServerStates[state] = false;
	}
	else if( it->second )
	{
		::glDisable( state );
		it->second = false;
	}
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CGLStateSaver::EnableClientState( UINT state )
{
	StateMap::iterator it = m_ServerStates.find( state );
	if( it == m_ServerStates.end() )
	{
		::glEnableClientState( state ); //we dont know the status so go the safe way.
		m_ServerStates[state] = true;
	}
	else if( !it->second )
	{
		::glEnableClientState( state );
		it->second = true;
	}
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CGLStateSaver::DisableClientState( UINT state )
{
	StateMap::iterator it = m_ServerStates.find( state );
	if( it == m_ServerStates.end() )
	{
		::glDisableClientState( state ); //we dont know the status so go the safe way.
		m_ServerStates[state] = false;
	}
	else if( it->second )
	{
		::glDisableClientState( state );
		it->second = false;
	}
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CGLStateSaver::EnableVertexAttribArray( UINT attrib )
{
	m_iLastVertexAttrib = attrib;

	StateMap::iterator it = m_VertexAttribStates.find( attrib );
	if( it == m_VertexAttribStates.end() )
	{
		::glEnableVertexAttribArray( attrib ); //we dont know the status so go the safe way.
		m_ServerStates[attrib] = true;
	}
	else if( !it->second )
	{
		::glEnableVertexAttribArray( attrib );
		it->second = true;
	}
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CGLStateSaver::DisableVertexAttribArray( UINT attrib, bool bDisableLastUsed )
{
	if( bDisableLastUsed )
		attrib = m_iLastVertexAttrib;

	StateMap::iterator it = m_VertexAttribStates.find( attrib );
	if( it == m_VertexAttribStates.end() )
	{
		::glDisableVertexAttribArray( attrib ); //we dont know the status so go the safe way.
		m_ServerStates[attrib] = false;
	}
	else if( it->second )
	{
		::glDisableVertexAttribArray( attrib );
		it->second = false;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGLStateSaver::ActiveTexture( UINT tex )
{
	if( tex != m_iLastActiveTexture )
	{
		glActiveTexture(tex);
		m_iLastActiveTexture = tex;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGLStateSaver::DepthFunc( UINT func )
{
	if( func != m_iLastDepthFunc )
	{
		glDepthFunc(func);
		m_iLastDepthFunc = func;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGLStateSaver::CullFace( UINT face )
{
	if( face != m_iLastCullFace )
	{
		glCullFace(face);
		m_iLastCullFace = face;
	}
}

