// server/CLapTimeCounter.h
// server/CLapTimeCounter.h
// server/CLapTimeCounter.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__server__CLapTimeCounter_H__
#define deferred__server__CLapTimeCounter_H__

#include "CBaseEntity.h"
#include "LapTimeTableEntry.h"


class CLapTimeCounter : public CBaseEntity
{
	DECLARE_DATADESC_EDITOR(CBaseEntity, CLapTimeCounter)
	DECLARE_ENTITY_LINKED()
public:
	CLapTimeCounter();
	virtual void OnDatadescFinished( void );
	void OnPassCheckpoint(int playerId, int checkPointId);
	void InputCheckpointPassed(const inputdata_t<CBaseEntity> &data);
	void SyncNetworkTimeTable();

private:
	int m_numCheckPoints;
	std::map<int, int> m_lastCheckpoints;
	std::map<int, float> m_lapStartTimes;
	std::map<int, float> m_roundTimes;

	NetVarArray(LapTimeTableEntry, m_timeTable, LAPTIMETABLE_NUM_ENTRIES);
};

#endif // deferred__server__CLapTimeCounter_H__