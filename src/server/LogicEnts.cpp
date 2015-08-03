// server/LogicEnts.cpp
// server/LogicEnts.cpp
// server/LogicEnts.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "LogicEnts.h"
#include "ServerInterfaces.h"

BEGIN_DATADESC_EDITOR(CLogicBaseToggle)
	FIELD_INT("enabled", m_enabled)
	FIELD_INPUTFUNC("Enable", InputEnable)
	FIELD_INPUTFUNC("Disable", InputDisable)
	FIELD_INPUTFUNC("Toggle", InputToggle)
END_DATADESC_EDITOR()

CLogicBaseToggle::CLogicBaseToggle()
{
	m_enabled = true;
}

void CLogicBaseToggle::InputEnable(const inputdata_t<CBaseEntity> &data)
{
	Enable();
}

void CLogicBaseToggle::InputDisable(const inputdata_t<CBaseEntity> &data)
{
	Disable();
}

void CLogicBaseToggle::InputToggle(const inputdata_t<CBaseEntity> &data)
{
	Toggle();
}



BEGIN_DATADESC_EDITOR(CLogicRelay)
	FIELD_INPUTFUNC("Trigger", InputTrigger)
END_DATADESC_EDITOR()

LINK_ENTITY_TO_CLASSNAME_SERVER(CLogicRelay, logic_relay)

CLogicRelay::CLogicRelay()
{
	
}

void CLogicRelay::InputTrigger(const inputdata_t<CBaseEntity> &data)
{
	if( IsEnabled() )
		FireOutput("OnTrigger", data.GetActivator(), data.GetCaller());
}


BEGIN_DATADESC_EDITOR(CLogicTimer)
	FIELD_FLOAT("interval", m_interval)
	FIELD_FLOAT("randomTime", m_randomTime)
	FIELD_INPUTFUNC("SetInterval", InputSetInterval)
	FIELD_INPUTFUNC("SetRandomTime", InputSetRandomTime)
END_DATADESC_EDITOR()

LINK_ENTITY_TO_CLASSNAME_SERVER(CLogicTimer, logic_timer)

CLogicTimer::CLogicTimer()
{
	m_interval = 1.0f;
	m_randomTime = 0.0f;
	m_lastTime = gpGlobals->curtime;
	ChooseRandom();
}

void CLogicTimer::ChooseRandom()
{
	if( m_randomTime == 0.0f )
		m_randomTimeChoice = 0.0f;
	else
		m_randomTimeChoice = RandomFloat(-m_randomTime, m_randomTime);
}

void CLogicTimer::Update()
{
	if( IsEnabled() )
	{
		float targetTime = m_lastTime + m_interval + m_randomTimeChoice;
		if( gpGlobals->curtime >= targetTime)
		{
			m_lastTime = gpGlobals->curtime;
			ChooseRandom();
			FireOutput("OnTimer", this);
		}
	}
	BaseClass::Update();
}

void CLogicTimer::InputSetInterval(const inputdata_t<CBaseEntity> &data)
{
	m_interval = (float)atof(data.GetParam().c_str());
}

void CLogicTimer::InputSetRandomTime(const inputdata_t<CBaseEntity> &data)
{
	m_randomTime = (float)atof(data.GetParam().c_str());
}

void CLogicTimer::OnEnable()
{
	m_lastTime = gpGlobals->curtime;
	ChooseRandom();
	BaseClass::OnEnable();
}