// client/CEntityCreateListener.cpp
// client/CEntityCreateListener.cpp
// client/CEntityCreateListener.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "CEntityCreateListener.h"
#include <util/eventlist.h>
#include "datadesc_client.h"
#include "CClientInterf.h"
#include <renderer/Interfaces.h>
#include <renderer/CRenderList.h>


bool CEntityCreateListener::HandleEventStatic( const IEvent *evt )
{
	CEntityCreateEvent *createEvt = (CEntityCreateEvent*)evt;
	CRenderList *renderList = g_ClientInterf->GetRenderer()->GetRenderList();

	//already there?
	CRenderEntity *existant = renderList->GetEntityByLogicIndex(createEvt->GetIndex());
	if( existant != NULL )
	{
		const char *existantClass = existant->GetEntityClass() ? existant->GetEntityClass() : "<no class>";
		ConsoleMessage("Already existant entity index: %i (Existant: %s, New: %s)", createEvt->GetIndex(),
			existantClass, createEvt->GetClassname().c_str());
		return true;
	}

	//get factory
	const IEntityFactoryEditor<CRenderEntity> *factory = GetEditorFactoryClient(createEvt->GetClassname());
	if( !factory )
	{
		ConsoleMessage("CEntityCreateListener: No factory for type %s", createEvt->GetClassname().c_str());
		return true;
	}

	//ConsoleMessage("Creating entity of type %s with index %i", createEvt->GetClassname().c_str(), createEvt->GetIndex());

	InStream is(createEvt->GetData(), createEvt->GetDataSize());
	USHORT index = -1, size = 0;
	is >> index >> size;
	Assert(index == createEvt->GetIndex());
	CRenderEntity *pEnt = factory->CreateEntity(NULL, createEvt->GetIndex());
	UINT startPos = is.tellg();
	pEnt->DeserializeNetworkChanges(is, size, false);
	is.seekg(startPos);
	pEnt->DeserializeNetworkChanges(is, size, true);
	g_ClientInterf->GetRenderer()->GetRenderList()->AddStaticEntity(pEnt);
	pEnt->OnSpawn();
	return true;
}

