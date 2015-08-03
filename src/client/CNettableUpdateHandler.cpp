// client/CNettableUpdateHandler.cpp
// client/CNettableUpdateHandler.cpp
// client/CNettableUpdateHandler.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include <util/CConVar.h>
#include <renderer/CRenderList.h>
#include <renderer/renderer.h>
#include "CNettableUpdateHandler.h"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CNettableUpdateHandler::HandleEvent( const CNettableUpdate *evt )
{
	bool interpMode = evt->GetType() == ev::NETTABLE_UPDATE_INTERP;
	CConVar *var = CConVarList::GetConVar("network.tables.client.debug");
	CNettableUpdate *updateEvt = (CNettableUpdate*)evt;
	InStream is(updateEvt->GetData(), updateEvt->GetDataSize());
	CRenderList *list = g_RenderInterf->GetRenderer()->GetRenderList();
	while(is.tellg() < updateEvt->GetDataSize())
	{
		USHORT index = -1;
		is >> index;
		if( checkError(is) ) 
			break;
		USHORT size = -1;
		is >> size;
		if( checkError(is) )
			break;
		UINT startPos = is.tellg();
		CRenderEntity *ent = list->GetEntityByLogicIndex(index);
		if( ent )
		{
			if( !ent->DeserializeNetworkChanges(is, size, interpMode) )
				break;
		}
		else if(var && var->GetBool())
		{
			ConsoleMessage("CNettableUpdateHandler: No entity for index %i", index);
		}
		is.seekg( startPos + size );
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CNettableUpdateHandler::checkError( const InStream &is )
{
	if( is.GetError() != InStream::ERROR_NONE )
	{
		if( is.GetError() == InStream::ERROR_READ )
			error("CNettableUpdateHandler: Stream read error");
		else
			error("CNettableUpdateHandler: Unknown stream error");
		return true;
	}
	return false;
}


