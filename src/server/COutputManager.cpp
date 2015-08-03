// server/COutputManager.cpp
// server/COutputManager.cpp
// server/COutputManager.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "COutputManager.h"
#include "CBaseEntity.h"
#include "CEntityList.h"
#include "ServerInterfaces.h"
#include <util/CConVar.h>

CConVar outputs_debug("outputs.debug", "0");


COutputManager::COutputManager()
{
	m_outputId = 1;
}


void COutputManager::Update(CEntityList *entList, float curtime) 
{
	while(!m_queue.empty() && m_queue.top().getProcessTime() <= curtime)
	{
		OutputT out = m_queue.top();
		m_queue.pop();
		FireOutput(entList, out);
	}
}


void COutputManager::printEntInfo(CBaseEntity *ent, const char *prefix) const
{
	if( !ent )
		ConsoleMessage("%s<null>", prefix);
	else
	{
		const char *name = ent->GetName();
		const char *cl = ent->GetEntityClass() ? ent->GetEntityClass() : "";
		const char *cppcl = ent->GetEntityCppClass() ? ent->GetEntityCppClass() : "";
		ConsoleMessage("%sname: \"%s\", index: %i, class: \"%s\", cppclass: \"%s\"",
			prefix, name, ent->GetIndex(), cl, cppcl);
	}
}


void COutputManager::AddOutput(const std::string &name, const OutputT &output)
{
	if( outputs_debug.GetBool() )	
	{
		ConsoleMessage("Entity-Output triggered: \"%s\" -> \"%s\", \"%s\" (\"%s\"), delay: %f",
			name.c_str(), output.getTarget().c_str(), output.getInput().c_str(), output.getInputData().GetParam().c_str(),
			output.getProcessTime() - gpGlobals->curtime);
		printEntInfo(output.getInputData().GetActivator(), "  Activator: ");
		printEntInfo(output.getInputData().GetCaller(), "  Caller: ");
	}
	m_queue.push(output);
}



bool COutputManager::FireOutput(CBaseEntity *ent, const OutputT &output)
{
	const DatadescList<CBaseEntity> *datadescList = ent->GetDatadescList();
	if( !datadescList )
		return false;

	const CDatadescInputEntry<CBaseEntity> *entry = datadescList->GetInputEntry(output.getInput());
	if( !entry )
		return false;

	if( outputs_debug.GetBool() )	
		ConsoleMessage("Calling Entity-Input %s on entity %s (%s, %i)", output.getInput().c_str(),
			output.getTarget().c_str(), ent->GetEntityClass() ? ent->GetEntityClass() : "<no type>",
			ent->GetEntityCppClass() ? ent->GetEntityCppClass() : "<no type>", ent->GetIndex());

	void (CBaseEntity::*func)(const inputdata_t<CBaseEntity>&) = static_cast<inputfunc>(entry->func);
	(*ent.*func)(output.getInputData());
	return true;
}


bool COutputManager::FireOutputSetId(CBaseEntity *ent, const OutputT &output)
{
	if( !ent )
		return false;
	ent->SetLastProcessedOutputId(m_outputId);
	return FireOutput(ent, output);
}


void COutputManager::FireOutput(CEntityList *entList, const OutputT &output)
{
	std::vector<int> ents;
	OutputSystem::genericTarget_e genTarget = OutputSystem::getGenericTarget(output.getTarget().c_str());

	if( genTarget == OutputSystem::TARGET_ERROR ) //is not a generic target
	{
		ents = entList->GetEntityArrayIndicesByName(output.getTarget().c_str());
		for(int i : ents)
			entList->GetEntityInArray(i)->SetLastProcessedOutputId(m_outputId);
		for(int arrayIdx : ents)
		{
			CBaseEntity *ent = entList->GetEntityInArray(arrayIdx);
			if( !ent || ent->GetLastProcessedOutputId() != m_outputId )
				continue;
			FireOutput(ent, output);
		}
	}
	else if( genTarget == OutputSystem::TARGET_SELF )
		FireOutputSetId(output.getSource(), output);
	else if( genTarget == OutputSystem::TARGET_ACTIVATOR )
		FireOutputSetId(output.getInputData().GetActivator(), output);
	else if( genTarget == OutputSystem::TARGET_CALLER )
		FireOutputSetId(output.getInputData().GetCaller(), output);
	else
	{
		AssertMsg(false, "Added a new generic-output-target?");
	}

	do
		m_outputId++;
	while(m_outputId == 0);
}


