// server/CVehicleRespawnPosition.h
// server/CVehicleRespawnPosition.h
// server/CVehicleRespawnPosition.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CVehicleRespawnPosition.h

#pragma once
#ifndef deferred__server__CVehicleRespawnPosition_H__
#define deferred__server__CVehicleRespawnPosition_H__

#include "CBaseEntity.h"

class CVehicleRespawnPosition : public CBaseEntity
{
	DECLARE_DATADESC_EDITOR(CBaseEntity, CVehicleRespawnPosition)
	DECLARE_ENTITY_LINKED()
public:
	virtual void OnDatadescFinished( void );
	bool IsOccupied();

private:
	bool m_initialSpawn;
};

#endif // deferred__server__CVehicleRespawnPosition_H__
