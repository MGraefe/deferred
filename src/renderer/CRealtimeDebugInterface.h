// renderer/CRealtimeDebugInterface.h
// renderer/CRealtimeDebugInterface.h
//CRealtimeDebugInterface.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CRealtimeDebugInterface_H__
#define deferred__renderer__CRealtimeDebugInterface_H__

#include <util/IRealtimeDebugInterface.h>
#include <util/basic_types.h>
#include <map>
#include <mutex>

class CRealtimeDebugInterface : public IRealtimeDebugInterface
{
public:
	struct info_t
	{
		std::string value;
		float timeChanged;

		info_t() :
			value(),
			timeChanged(0.0f)
		{}

		info_t(const std::string &value, float timeChanged) :
			value(value),
			timeChanged(timeChanged)
		{}
	};
	typedef std::map<std::string, info_t, LessThanCppStringObj> ValueMap;

public:
	CRealtimeDebugInterface();
	virtual void SetValue(const char *name, const char *value);
	ValueMap GetValues();

private:
	ValueMap m_valueMap;
	std::mutex m_mutex;
};

#endif // deferred__renderer__CRealtimeDebugInterface_H__