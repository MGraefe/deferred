// renderer/CRealtimeDebugInterface.cpp
// renderer/CRealtimeDebugInterface.cpp
//CRealtimeDebugInterface.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CRealtimeDebugInterface.h"



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CRealtimeDebugInterface::CRealtimeDebugInterface()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRealtimeDebugInterface::SetValue( const char *name, const char *value )
{
	std::lock_guard<std::mutex> l(m_mutex);

	std::string sName(name);
	auto it = m_valueMap.find(sName);
	if(it == m_valueMap.end())
		m_valueMap[sName] = info_t(value, gpGlobals->curtime);
	else if(it->second.value.compare(value) != 0)
	{
		it->second.value = value;
		it->second.timeChanged = gpGlobals->curtime;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CRealtimeDebugInterface::ValueMap CRealtimeDebugInterface::GetValues()
{
	std::lock_guard<std::mutex> l(m_mutex);
	return m_valueMap;
}


