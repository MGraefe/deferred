// server/CWorld.h
// server/CWorld.h
// server/CWorld.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CWorld.h

#pragma once
#ifndef deferred__server__CWorld_H__
#define deferred__server__CWorld_H__

#include <string>
#include "CBaseEntity.h"

class CVehicleRespawnPosition;

class CWorld : public CBaseEntity
{
public:
	CWorld(std::string scriptname);
	bool init();
	const std::string &GetScriptName();
	bool isLoaded() { return m_loaded; }

	void addRespawnPosition(CVehicleRespawnPosition *position);
	void addInitialSpawnPosition(CVehicleRespawnPosition *position);
	std::vector<CVehicleRespawnPosition*> &getRespawnPositions() { return m_respawnPositions; }
	std::vector<CVehicleRespawnPosition*> &getInitialSpawnPositions() { return m_initialSpawnPositions; }
private:
	bool initPhysics(const char *filename);
	bool initEntities(const CScriptParser &parser);
	std::string m_scriptname;
	bool m_loaded;
	std::vector<CVehicleRespawnPosition*> m_respawnPositions;
	std::vector<CVehicleRespawnPosition*> m_initialSpawnPositions;
};


#endif // deferred__server__CWorld_H__
