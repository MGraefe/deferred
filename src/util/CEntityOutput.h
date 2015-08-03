// util/CEntityOutput.h
// util/CEntityOutput.h
// util/CEntityOutput.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__CEntityOutput_H__
#define deferred__util__CEntityOutput_H__

#include <string>
#include <util/CScriptParser.h>

class CEntityOutput
{
public:
	CEntityOutput() 
	{
		m_delay = 0.0f; 
		m_onlyOnce = false;
	}

	CEntityOutput(const CEntityOutput &other) :
		m_event(other.m_event),
		m_target(other.m_target),
		m_input(other.m_input),
		m_param(other.m_param),
		m_delay(other.m_delay),
		m_onlyOnce(other.m_onlyOnce)
	{}

	CEntityOutput(const std::string &evt, const std::string &target, const std::string &input, const std::string &param, float delay, bool onlyOnce)
	{
		_init(evt, target, input, param, delay, onlyOnce);
	}

	CEntityOutput(const std::string &evt, const std::string &target, const std::string &input, const std::string &param, const std::string &delay, bool onlyOnce)
	{
		float fDelay = (float)atof(delay.c_str());
		_init(evt, target, input, param, fDelay, onlyOnce);
	}

	CEntityOutput(const CScriptSubGroup *group)
	{
		group->GetString("event", m_event);
		group->GetString("target", m_target);
		group->GetString("input", m_input);
		group->GetString("param", m_param);
		m_delay = group->GetFloat("delay", 0.0f);
		m_onlyOnce = group->GetInt("onlyOnce", 0) == 1;
	}

	inline void serialize(std::ostream &os, int identTabs = 0) const
	{
		std::string ident = "";
		for(int i = 0; i < identTabs; i++)
			ident += "\t";
		os << ident << "\"output\"\n" << ident << "{\n";
		os << ident << "\t\"event\" \"" << m_event << "\"\n";
		os << ident << "\t\"target\" \"" << m_target << "\"\n";
		os << ident << "\t\"input\" \"" << m_input << "\"\n";
		os << ident << "\t\"param\" \"" << m_param << "\"\n";
		os << ident << "\t\"delay\" \"" << m_delay << "\"\n";
		os << ident << "\t\"onlyOnce\" \"" << (m_onlyOnce ? 1 : 0) << "\"\n";
		os << ident << "}\n\n";
	}

	inline bool operator<(const CEntityOutput &other) const
	{
		int cmp;
		if( (cmp = m_event.compare(other.m_event)) != 0 )
			return cmp < 0;
		if( (cmp = m_target.compare(other.m_target)) != 0 )
			return cmp < 0;
		if( (cmp = m_input.compare(other.m_input)) != 0 )
			return cmp < 0;
		if( (cmp = m_param.compare(other.m_param)) != 0 )
			return cmp < 0;
		if( m_delay != other.m_delay )
			return m_delay < other.m_delay;
		if( m_onlyOnce != other.m_onlyOnce )
			return !m_onlyOnce;
		return false;
	}

	inline bool operator==(const CEntityOutput &other)
	{
		return	m_event.compare(other.m_event) == 0 &&
			m_target.compare(other.m_target) == 0 &&
			m_input.compare(other.m_input) == 0 &&
			m_param.compare(other.m_param) == 0 &&
			m_delay == other.m_delay &&
			m_onlyOnce == other.m_onlyOnce;
	}

	inline const std::string &getEvent() const { return m_event; }
	inline const std::string &getTarget() const { return m_target; }
	inline const std::string &getInput() const { return m_input; }
	inline const std::string &getParam() const { return m_param; }
	inline float getDelay() const { return m_delay; }
	inline bool getOnlyOnce() const { return m_onlyOnce; }

	inline void setEvent(const std::string &evt) { m_event = evt; }
	inline void setTarget(const std::string &target) { m_target = target; }
	inline void setInput(const std::string &input) { m_input = input; }
	inline void setParam(const std::string &param) { m_param = param; }
	inline void setDelay(float delay) { m_delay = delay; }
	inline void setDelay(const std::string &delay) { m_delay = (float)atof(delay.c_str()); }
	inline void setOnlyOnce(bool onlyOnce) { m_onlyOnce = onlyOnce; }

private:
	inline void _init(const std::string &evt, const std::string &target, const std::string &input, const std::string &param, float delay, bool onlyOnce)
	{
		m_event = evt;
		m_target = target;
		m_input = input;
		m_param = param;
		m_delay = delay;
		m_onlyOnce = onlyOnce;
	}

private:
	std::string m_event;
	std::string m_target;
	std::string m_input;
	std::string m_param;
	float m_delay;
	bool m_onlyOnce;
};

#endif // deferred__util__CEntityOutput_H__