// server/CWorld.cpp
// server/CWorld.cpp
// server/CWorld.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include <util/CScriptParser.h>
#include "CWorld.h"
#include "ServerInterfaces.h"
#include "datadesc_server.h"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CWorld::CWorld( std::string scriptname ) :
	m_scriptname(scriptname)
{
	m_loaded = false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CWorld::init( void )
{
	if( m_loaded ) 
	{
		error("CWorld#init(): already initialized!");
		return false;
	}

	std::string fullFileName = string("models/") + m_scriptname + ".dfmap";
	CScriptParser parser;

	if( !parser.ParseFile(fullFileName.c_str()) ) 
	{
		error("Error parsing file: \"%s\"", fullFileName.c_str());
		return false;
	}

	CScriptSubGroup *worldgroup = parser.GetSubGroup("WORLD");
	if( !worldgroup ) 
	{
		error("Could not find subgroup \"WORLD\" in world-script \"%s\"", fullFileName.c_str());
		return false;
	}

	CScriptSubGroup *keyvalsGroup = worldgroup->GetSubGroup("keyvals");
	if( !keyvalsGroup )
	{
		error("Client: Could not find subgroup \"keyvals\""
			"in Group \"WORLD\" in world-script \"%s\"", fullFileName.c_str());
		return false;
	}

	std::string worldModel;
	if( !keyvalsGroup->GetString("filename", worldModel) )
	{
		error("Could not find keyval \"filename\" in subgroup \"WORLD\" in world-script \"%s\"", fullFileName.c_str());
		return false;
	}

	if( !initPhysics(worldModel.c_str()) )
	{
		error("Error loading physics for world file \"%s\"", worldModel.c_str());
		return false;
	}

	if( !initEntities(parser) )
	{
		error("Error parsing entities");
		return false;
	}

	m_loaded = true;
	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const std::string & CWorld::GetScriptName( void )
{
	return m_scriptname;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CWorld::initPhysics( const char *filename )
{
	return singletons::g_pPhysics->VCreateWorld(this, filename);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CWorld::initEntities( const CScriptParser &parser )
{
	const SubGroupMap *subGroupMap = parser.GetSubGroupMap();
	SubGroupMap::const_iterator it = subGroupMap->begin();
	SubGroupMap::const_iterator itEnd = subGroupMap->end();

	for( ; it != itEnd; it++ )
	{
		if( it->first != "ENTITY" )
			continue;

		CScriptSubGroup *pSubGrp = it->second;

		//get classname, continue if not found
		std::string classname;
		if( !pSubGrp->GetString( "classname", classname ) )
			continue;

		const IEntityFactoryEditor<CBaseEntity> *pFactory = GetEditorFactoryServer(classname);
		if( !pFactory )
			continue;

		pFactory->CreateEntity(pSubGrp, -1);
	}
	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CWorld::addRespawnPosition( CVehicleRespawnPosition *position )
{
	m_respawnPositions.push_back(position);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CWorld::addInitialSpawnPosition( CVehicleRespawnPosition *position )
{
	m_initialSpawnPositions.push_back(position);

	//also add this spawn to respawns
	m_respawnPositions.push_back(position);
}
