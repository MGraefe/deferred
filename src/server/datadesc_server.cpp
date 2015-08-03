// server/datadesc_server.cpp
// server/datadesc_server.cpp
// server/datadesc_server.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CBaseEntity.h"
#include "datadesc_server.h"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
EditorFactoryMapServer &GetEditorFactoryMapServer( void )
{
	static EditorFactoryMapServer factMap;
	return factMap;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const IEntityFactoryEditor<CBaseEntity> *GetEditorFactoryServer( const std::string name )
{
	EditorFactoryMapServer::const_iterator it = GetEditorFactoryMapServer().find(name);
	if( it == GetEditorFactoryMapServer().end() )
		return NULL;
	else
		return it->second;
}
