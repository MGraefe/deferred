// server/LogicEnts.h
// server/LogicEnts.h
// server/LogicEnts.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__server__LogicEnts_H__
#define deferred__server__LogicEnts_H__

#include "CBaseEntity.h"


class CLogicBaseToggle : public CBaseEntity
{
	DECLARE_DATADESC_EDITOR(CBaseEntity, CLogicBaseToggle);
public:
	CLogicBaseToggle();
	void InputEnable(const inputdata_t<CBaseEntity> &data);
	void InputDisable(const inputdata_t<CBaseEntity> &data);
	void InputToggle(const inputdata_t<CBaseEntity> &data);
	inline bool IsEnabled() const { return m_enabled != 0; }
	inline void Enable() { m_enabled = 1; OnEnable(); }
	inline void Disable() { m_enabled = 0; OnDisable(); }
	inline void Toggle() { if(m_enabled) Disable(); else Enable(); }
	inline virtual void OnEnable() {}
	inline virtual void OnDisable() {}
private:
	int m_enabled;
};


class CLogicRelay : public CLogicBaseToggle
{
	DECLARE_DATADESC_EDITOR(CLogicBaseToggle, CLogicRelay);
	DECLARE_ENTITY_LINKED()
public:
	CLogicRelay();
	void InputTrigger(const inputdata_t<CBaseEntity> &data);
};


class CLogicTimer : public CLogicBaseToggle
{
	DECLARE_DATADESC_EDITOR(CLogicBaseToggle, CLogicTimer);
	DECLARE_ENTITY_LINKED()
public:
	CLogicTimer();
	virtual void Update();
	void InputSetInterval(const inputdata_t<CBaseEntity> &data);
	void InputSetRandomTime(const inputdata_t<CBaseEntity> &data);
	virtual void OnEnable();
	void ChooseRandom();
private:
	float m_interval;
	float m_randomTime;
	float m_randomTimeChoice;
	float m_lastTime;
};

#endif