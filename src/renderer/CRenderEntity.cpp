// renderer/CRenderEntity.cpp
// renderer/CRenderEntity.cpp
// renderer/CRenderEntity.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CRenderEntity.h"
#include <util/CEventManager.h>
#include <util/eventlist.h>
#include "CRenderInterf.h"
#include <util/CConVar.h>

CConVar network_tables_debug("network.tables.client.debug", "0");

BEGIN_DATADESC_EDITOR_NOBASE(CRenderEntity)
	FIELD_CHAR_ARRAY( "name", m_name, 256 )
	FIELD_VECTOR3( "origin", m_vAbsPos );
	FIELD_VECTOR3( "origin", m_vOldPos );
	FIELD_ANGLE_TO_QUATERNION( "angles", m_aAbsAngles );
	FIELD_ANGLE_TO_QUATERNION( "angles", m_aOldAngles );

	NETTABLE_ENTRY_INTERP(m_vAbsPos, INTERP_VECTOR);
	NETTABLE_ENTRY_RAW_PROXY_INTERP(m_aAbsAngles, CNetVarReadProxyAngles, INTERP_QUATERNION);
	NETTABLE_ENTRY_STRING(m_name);
END_DATADESC_EDITOR()


CRenderEntity::CRenderEntity( int index /*= -1*/ ) : 
	m_iIndex(index),
	m_bIsVisible(true),
	m_bNetworkDataInitialized(false)
{
	m_name[0] = 0;
	if(index != -1)
		InitInterpolators();
}


void CRenderEntity::UpdateInterpolation( const float &alpha )
{
	m_aOldAngles = m_aAbsAngles;
	m_vOldPos = m_vAbsPos;

	for(IInterpolatedValue *val : m_interpValues)
		if( val )
			val->Interpolate(alpha);

	m_vVelocity = (m_vAbsPos - m_vOldPos) / gpGlobals->frametime;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRenderEntity::DeserializeNetworkChanges( InStream &is, UINT size, bool interpMode )
{
	const DatadescList<CRenderEntity> *list = GetDatadescList();
	if( !list )
		return false;

	UINT isStart = is.tellg();
	while(is.tellg() < isStart + size)
	{
		CNetworkTableEntry::Idtype id = -1;
		is >> id;
		UINT startPos = is.tellg();
		const CNetworkTableEntry *entry = list->GetNetworkEntryById(id);
		if( entry )
		{
			//char *member = ((char*)this) + entry->offset;
			bool streamOk = true;
			if( interpMode && entry->interpType != INTERP_NONE )
			{
				AssertMsg(entry->count == 1, "Interpolated nettable-entries cannot be an array");
				Assert(entry->id < m_interpValues.size());
				IInterpolatedValue *interp = m_interpValues[entry->id];
				Assert(interp);
				streamOk = entry->proxyFunc(is, interp->GetNewMember(), entry->memberSize, entry->networkSize, entry->count);
				interp->Update();
			}

			if( !interpMode && entry->interpType == INTERP_NONE )
			{
				streamOk = entry->proxyFunc(is, ((char*)this) + entry->offset, entry->memberSize, entry->networkSize, entry->count);
			}

			is.seekg(startPos);
			size_t netSize = entry->networkSize;
			size_t dynSize = entry->proxySizeFunc(is);
			Assert(netSize != size_t(-1) && dynSize == size_t(-1) || netSize == size_t(-1) && dynSize != size_t(-1));
			size_t size = netSize != size_t(-1) ? netSize : dynSize;
			is.seekg(startPos + size * entry->count);
			if( !streamOk || is.GetError() != InStream::ERROR_NONE )
			{
				if( network_tables_debug.GetBool() )
					ConsoleMessage("CRenderEntity#DeserializeNetworkChanges(): stream read error for entity %s",
						list->GetEntityClassName().c_str());
				return false;
			}
		}
		else
		{
			error_fatal("CRenderEntity#DeserializeNetworkChanges(): No datadesc entry for entry id %i on entity %s.\nWatch your net-tables!",
				id, list->GetEntityClassName().c_str());
		}
	}

	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderEntity::InitInterpolators()
{
	Assert(m_interpValues.empty());

	const DatadescList<CRenderEntity> *list = GetDatadescList();
	if( !list )
		return;

	m_interpValues.resize(list->GetNetworkTable().size(), NULL);
	for(const DatadescList<CRenderEntity>::NetTable::value_type &entry : list->GetNetworkTable())
	{
		IInterpolatedValue *interp;
		char *member = ((char*)this) + entry.second.offset;
		netvarinterptype_e type = entry.second.interpType;
		switch(type)
		{
		case INTERP_FLOAT:
			interp = new CInterpolatedValue<float>(this, (float*)member);
			break;
		case INTERP_VECTOR:
			interp = new CInterpolatedValue<Vector3f>(this, (Vector3f*)member);
			break;
		case INTERP_QUATERNION:
			interp = new CInterpolatedValue<Quaternion>(this, (Quaternion*)member);
			break;
		default:
			interp = NULL;
		}
		Assert(entry.second.id >= 0 && entry.second.id < m_interpValues.size());
		m_interpValues[entry.second.id] = interp;
	}
}

