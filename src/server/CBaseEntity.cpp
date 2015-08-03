// server/CBaseEntity.cpp
// server/CBaseEntity.cpp
// server/CBaseEntity.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CBaseEntity.h"
#include "CEntityList.h"
#include "ServerInterfaces.h"
#include <util/CEventManager.h>
#include "datadesc_server.h"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityOutputList::ReadFromScript( const CScriptSubGroup *pScript )
{
	CScriptSubGroup *outGroup = pScript->GetSubGroup("outputs");
	if(!outGroup)
		return;

	const SubGroupMap *subGroups = outGroup->GetSubGroupMap();
	if(!subGroups)
		return;

	for(auto it = subGroups->begin(); it != subGroups->end(); ++it)
		if(it->first.compare("output") == 0)
			AddOutput(CEntityOutput(it->second));
}


BEGIN_DATADESC_EDITOR_NOBASE(CBaseEntity)
	FIELD_STRING( "name", m_name )
	FIELD_VECTOR3( "origin", m_vOrigin )
	FIELD_VECTOR3( "origin", m_vLastOrigin )
	FIELD_ANGLES( "angles", m_aAngle )
	FIELD_ANGLES( "angles", m_aLastAngle )

	//Network table, keep ordering in client
	NETTABLE_ENTRY( m_vOrigin )
	NETTABLE_ENTRY_PROXY( m_aAngle, CNetVarWriteProxyAngles )
	NETTABLE_ENTRY_STRING(m_name)
END_DATADESC_EDITOR()

CConVar network_tables_debug("network.tables.server.debug", "0");

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CBaseEntity::CBaseEntity()
{
	m_index = -1;
	m_bIsVisible = true;
	m_pParent = NULL;
	m_lastProcessedOutputId = 0;
	m_bNetworkChangedAll = false;
	m_isCollidable = true;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseEntity::Update( void )
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseEntity::SendNetworkSnapshot( int iPlayerID )
{
	CEntityCreateEvent *evt = CreateCreateEvent(ev::ENT_CREATE_INITIAL);
	if( evt )
	{
		singletons::g_pEvtMgr->AddEventToQueue(evt, iPlayerID);
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseEntity::PreUpdate( void )
{
	//Update Position based on velocity and angles based on Angular Velocity.
	//m_vOrigin = m_vOrigin + m_vVelocity * gpGlobals.frametime;
	m_aAngle = m_aAngle + m_aAngVelocity * gpGlobals->frametime;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseEntity::PostUpdate( void )
{
	m_vLastOrigin = m_vOrigin;
	m_aLastAngle = m_aAngle;
	m_vLastVelocity = m_vVelocity;
	m_aLastAngVelocity = m_aAngVelocity;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseEntity::Destroy( void )
{
	FireOutput("OnDestroy", this);

	//notify others:
	singletons::g_pEvtMgr->AddEventToQueue( new CEntityDeleteEvent( ev::ENT_DELETE, m_index ) );

	//delete ourselves
	singletons::g_pEntityList->DeleteEntity( m_index );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseEntity::SetVisible( bool visible )
{
	m_bIsVisible = visible;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseEntity::ToggleVisibility( void )
{
	SetVisible( !m_bIsVisible );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CBaseEntity::IsVisible( void )
{
	if( this->m_pParent == NULL )
		return m_bIsVisible;
	else if( this->m_bIsVisible )
		return m_pParent->IsVisible();
	else
		return false;

}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseEntity::SetName( const char *pName )
{
	m_name = pName;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseEntity::Spawn( void )
{
	CEntityCreateEvent *evt = CreateCreateEvent(ev::ENT_CREATE);
	if( evt )
		singletons::g_pEvtMgr->AddEventToQueue(evt);
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseEntity::Think( void )
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBaseEntity::OnDatadescFinished( void )
{
	
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBaseEntity::OnCollisionStart( const CCollisionInfo &info )
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBaseEntity::FireOutput( const std::string &name, CBaseEntity *activator, CBaseEntity *caller /*= NULL*/, float delay /*= 0.0f*/ )
{
	const CEntityOutputList::OutputList *list = m_outputList.GetOutputList(name);
	if( !list )
		return;
	for(const CEntityOutput &output : *list)
	{
		float targetTime = gpGlobals->curtime + output.getDelay() + delay;
		outputdata_t<CBaseEntity> srvOut(this, inputdata_t<CBaseEntity>(activator, caller ? caller : this, output.getParam()),
			output.getTarget(), output.getInput(), targetTime);
		singletons::g_pOutputManager->AddOutput(name, srvOut);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const char *CBaseEntity::GetEntityClass() const
{
	return NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const char *CBaseEntity::GetEntityCppClass() const
{
	return NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityCreateEvent *CBaseEntity::CreateCreateEvent(EventType type)
{
	if( GetEntityClass() == NULL || GetIndex() == -1 )
		return NULL;

	OutStream dummyOs(true);
	SerializeNetworkChanges(dummyOs, true);
	UINT size = dummyOs.tellp();
	char *data = new char[size];
	OutStream os(data, size);
	SerializeNetworkChanges(os, true);
	CEntityCreateEvent *evt = new CEntityCreateEvent(type, GetIndex(), GetEntityClass(), data, size, false);
	return evt;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBaseEntity::SerializeNetworkChange(OutStream &os, const CNetworkTableEntry *entry)
{
	if( entry )
	{
		os << entry->id;
		entry->proxyFunc(os, ((char*)this) + entry->offset, entry->memberSize, entry->networkSize, entry->count);
	}
	else if(network_tables_debug.GetBool())
		ConsoleMessage("CBaseEntity#SerializeNetworkChange: No such entry, entity: %s",
			GetDatadescList()->GetEntityClassName().c_str());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBaseEntity::SerializeNetworkChanges(OutStream &os, bool allVars /*= false*/)
{
	os << (USHORT)(GetIndex());
	UINT sizePos = os.tellp();
	os << (USHORT)0; //size dummy
	UINT startPos = os.tellp();

	const DatadescList<CBaseEntity> *list = GetDatadescList();
	if( m_bNetworkChangedAll || allVars )
	{
		const DatadescList<CBaseEntity>::NetTable &table = list->GetNetworkTable();
		for(auto it = table.begin(); it != table.end(); ++it)
			SerializeNetworkChange(os, &it->second);
	}
	else
	{
		for(size_t offset : m_netvarChangeSet)
			SerializeNetworkChange(os, list->GetNetworkEntryByOffset(offset));
	}

	UINT endPos = os.tellp();
	os.seekp(sizePos);
	os << (USHORT)(endPos - startPos);
	os.seekp(endPos);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBaseEntity::DeserializeNetworkChanges(InStream &is)
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBaseEntity::SetNetworkChanged(size_t memberOffset)
{ 
	if( !m_bNetworkChangedAll ) //no need if already flagged as all changed
		m_netvarChangeSet.insert(memberOffset); 
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBaseEntity::SetNetworkChangedAll()
{ 
	m_bNetworkChangedAll = true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBaseEntity::ResetNetworkChanged()
{
	m_bNetworkChangedAll = false;
	m_netvarChangeSet.clear();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBaseEntity::AddToList()
{
	singletons::g_pEntityList->AddEntity(this);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBaseEntity::SetCollidable( bool collidable )
{
	m_isCollidable = collidable;
	g_pPhysics->VSetCollidable(this, collidable);
}

