// server/CVehicleRespawnPosition.cpp
// server/CVehicleRespawnPosition.cpp
// server/CVehicleRespawnPosition.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CVehicleRespawnPosition.h"
#include "ServerInterfaces.h"
#include "datadesc_server.h"

LINK_ENTITY_TO_CLASSNAME_SERVER(CVehicleRespawnPosition, vehicle_respawn);

BEGIN_DATADESC_EDITOR(CVehicleRespawnPosition)
	FIELD_BOOL("initialSpawn", m_initialSpawn)
END_DATADESC_EDITOR()

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CVehicleRespawnPosition::OnDatadescFinished( void )
{
	if(m_initialSpawn)
		singletons::g_pWorld->addInitialSpawnPosition(this);
	else
		singletons::g_pWorld->addRespawnPosition(this);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CVehicleRespawnPosition::IsOccupied()
{
	//TODO: check for collisions with cars
	return false;
}
