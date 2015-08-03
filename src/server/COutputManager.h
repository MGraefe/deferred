// server/COutputManager.h
// server/COutputManager.h
// server/COutputManager.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__server__COutputManager_H__
#define deferred__server__COutputManager_H__

#include <util/datadesc.h>
#include <util/OutputSystem.h>

class CEntityList;
class CBaseEntity;

class COutputManager
{
private:
	typedef inputfunc_t<CBaseEntity>::type inputfunc;
	typedef outputdata_t<CBaseEntity> OutputT;

public:
	COutputManager();

	void Update(CEntityList *entList, float curtime);
	void printEntInfo(CBaseEntity *ent, const char *prefix) const;
	void AddOutput(const std::string &name, const OutputT &output);

private:
	bool FireOutput(CBaseEntity *ent, const OutputT &output);
	bool FireOutputSetId(CBaseEntity *ent, const OutputT &output);
	void FireOutput(CEntityList *entList, const OutputT &output);

private:
	std::priority_queue<OutputT> m_queue;
	size_t m_outputId;
};

#endif // deferred__server__COutputManager_H__
