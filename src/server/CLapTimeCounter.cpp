// server/CLapTimeCounter.cpp
// server/CLapTimeCounter.cpp
// server/CLapTimeCounter.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CLapTimeCounter.h"
#include "ServerInterfaces.h"
#include "datadesc_server.h"
#include <util/CConVar.h>
#include <util/StringUtils.h>
#include "CPlayerCar.h"

LINK_ENTITY_TO_CLASSNAME_SERVER(CLapTimeCounter, logic_laptimecounter);

BEGIN_DATADESC_EDITOR(CLapTimeCounter)
	FIELD_INT("numCheckpoints", m_numCheckPoints)
	FIELD_INPUTFUNC("CheckpointPassed", InputCheckpointPassed)
	NETTABLE_ENTRY_ARRAY(m_timeTable, LAPTIMETABLE_NUM_ENTRIES)
END_DATADESC_EDITOR()


CConVar cv_debugLapCounter("server.lapcounter.debug", "0");

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CLapTimeCounter::CLapTimeCounter()
{
	m_numCheckPoints = 0;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLapTimeCounter::OnDatadescFinished( void )
{
	if(m_numCheckPoints == 0)
		m_numCheckPoints = 1;
	SyncNetworkTimeTable();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLapTimeCounter::OnPassCheckpoint( int playerId, int checkPointId )
{
	if(m_lastCheckpoints.find(playerId) != m_lastCheckpoints.end()) //player recorded?
	{
		int lastValidCheck = checkPointId == 0 ? m_numCheckPoints-1 : checkPointId-1;
		if(m_lastCheckpoints[playerId] == lastValidCheck) //last checkpoint valid?
		{
			m_lastCheckpoints[playerId] = checkPointId;
			if(cv_debugLapCounter.GetBool())
				ConsoleMessage("Player %i passed checkpoint %i", playerId, checkPointId);
			if(checkPointId == 0) //lap completed?
			{
				float lapTime = gpGlobals->curtime - m_lapStartTimes[playerId];
				m_lapStartTimes[playerId] = gpGlobals->curtime;
				if(m_roundTimes.find(playerId) == m_roundTimes.end() || lapTime < m_roundTimes[playerId])
				{
					m_roundTimes[playerId] = lapTime;
					SyncNetworkTimeTable();
				}
				
				if(cv_debugLapCounter.GetBool())
					ConsoleMessage("Player %i completed a lap in time %.2f", playerId, m_roundTimes[playerId]);
			}
		}
		else if(cv_debugLapCounter.GetBool())
			ConsoleMessage("Invalid checkpoint %i by player %i (last was %i)", checkPointId, playerId, m_lastCheckpoints[playerId]);
	}
	else if(checkPointId == 0) //player is not recorded, first lap-start?
	{
		m_lastCheckpoints[playerId] = 0;
		m_lapStartTimes[playerId] = gpGlobals->curtime;
		//TODO: send event for client lap-time-display
		if(cv_debugLapCounter.GetBool())
			ConsoleMessage("Player %i started it's first lap", playerId);
	}
	else if(cv_debugLapCounter.GetBool())
		ConsoleMessage("Invalid checkpoint %i by not recorded player %i", checkPointId, playerId);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLapTimeCounter::InputCheckpointPassed( const inputdata_t<CBaseEntity> &data )
{
	int checkpointId = StrUtils::GetInt(data.GetParam());
	if( !StrUtils::is_numeric(data.GetParam()) || checkpointId < 0 || checkpointId >= m_numCheckPoints)
	{
		ConsoleMessage("CLapTimeCounter#InputCheckpointPassed: invalid parameter");
		return;
	}
	
	CBaseEntity *activator = data.GetActivator();
	if(activator->GetEntityClass() == NULL || strcmp(activator->GetEntityClass(), "player_car") != 0)
	{
		if(cv_debugLapCounter.GetBool())
			ConsoleMessage("CLapTimeCounter#InputCheckpointPassed: invalid activator");
		return;
	}

	CPlayerCar *car = dynamic_cast<CPlayerCar*>(data.GetActivator());
	Assert(car);
	OnPassCheckpoint(car->GetClientID(), checkpointId);
	car->SetLastCheckpointPos(data.GetCaller()->GetOrigin());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLapTimeCounter::SyncNetworkTimeTable()
{
	auto it = m_roundTimes.begin();
	for(int i = 0; i < LAPTIMETABLE_NUM_ENTRIES; i++)
	{
		if(it != m_roundTimes.end())
		{
			m_timeTable[i].clientId = it->first;
			m_timeTable[i].time = it->second;
			++it;
		}
		else
		{
			m_timeTable[i].clientId = -1;
			m_timeTable[i].time = -1.0f;
		}
	}
}


