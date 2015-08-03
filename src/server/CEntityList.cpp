// server/CEntityList.cpp
// server/CEntityList.cpp
// server/CEntityList.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CEntityList.h"
#include <util/error.h>
#include <util/IEvent.h>
#include "ServerInterfaces.h"
//#include <typeinfo.h>


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CEntityList::CEntityList() //Constructor
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CEntityList::~CEntityList()
{
	int size = entitylist.size();

	for( int i = 0; i < size; i++ )
	{
		CBaseEntity *pEnt = entitylist[i];
		if( pEnt )
			delete pEnt;
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEntityList::SendSnapshot( int iPlayerID )
{
	UINT size = entitylist.size();
	for( UINT i = 0; i < size; i++ )
	{
		CBaseEntity *pEnt = GetEntityInArray(i);
		if( pEnt )
		{
			pEnt->SendNetworkSnapshot( iPlayerID );
		}
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEntityList::ThinkAllEntities( void )
{
	int max = GetCount();

	for( int i = 0; i < max; i++ )
	{
		CBaseEntity *pEnt = GetEntityInArray(i);
		if( pEnt )
			pEnt->Think();
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEntityList::UpdateAllEntities( void )
{
	int max = GetCount();

	for( int i = 0; i < max; i++ )
	{
		CBaseEntity *pEnt = GetEntityInArray(i);
		if( pEnt )
			pEnt->PreUpdate();
	}

	max = GetCount();

	for( int i = 0; i < max; i++ )
	{
		CBaseEntity *pEnt = GetEntityInArray(i);
		if( pEnt )
			pEnt->Update();
	}

	max = GetCount();
	for( int i = 0; i < max; i++ )
	{
		CBaseEntity *pEnt = GetEntityInArray(i);
		if( pEnt )
			pEnt->PostUpdate();
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CBaseEntity *CEntityList::AddEntity( CBaseEntity *pEnt )
{
	for( unsigned int i = 0; i < entitylist.size(); i++ )
	{
		if( entitylist[i] == NULL )
		{
			entitylist[i] = pEnt;
			pEnt->SetIndex( (int)i );
			return pEnt;
		}
	}

	//No free Slot has been found, so put it to the End!
	pEnt->SetIndex( entitylist.size() );
	entitylist.push_back( pEnt );
	return pEnt;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CBaseEntity *CEntityList::GetEntity( int index )
{
	for( unsigned int i = 0; i < entitylist.size(); i++ )
	{
		if( entitylist[i] && entitylist[i]->GetIndex() == index )
			return entitylist[i];
	}
	return NULL;
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CBaseEntity * CEntityList::GetEntityInArray( int i ) const
{
	if( i >= (int)entitylist.size() )
		return NULL;

	return entitylist[i];
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
int CEntityList::GetCount( void ) const
{
	return entitylist.size();
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CBaseEntity *CEntityList::GetEntity( const char *pName )
{
	for( unsigned int i = 0; i < entitylist.size(); i++ )
	{
		if( entitylist[i] != NULL && strcmp( pName, entitylist[i]->GetName() ) == 0 )
			return entitylist[i];
	}
	return NULL;
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEntityList::DeleteEntity( int index )
{
	//Search for the Ent!
	for( unsigned int i = 0; i < entitylist.size(); i++ )
	{
		if( entitylist[i] && entitylist[i]->GetIndex() == index )
		{
			CBaseEntity *pEnt = entitylist[i];
			if( !pEnt )
				continue;

			//check if this is a event listener and unregister from events if so
			IEventListener *pListener = dynamic_cast<IEventListener*>(pEnt);
			if( pListener )
				pListener->UnregisterEvents();

			//error( "deleting %s", typeid(*pEnt).name() );
			delete (pEnt);
			entitylist[i] = NULL;
			return;
		}
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEntityList::DeleteEntity( const char *pName )
{
	DeleteEntity( GetEntity(pName)->GetIndex() );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CEntityList::DeleteEntity( CBaseEntity *pEnt )
{
	DeleteEntity( pEnt->GetIndex() );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::vector<int> CEntityList::GetEntityArrayIndicesByName( const char *pName )
{
	std::vector<int> result;
	for(int i = 0; i < GetCount(); i++)
		if( entitylist[i] && strcmp(entitylist[i]->GetName(), pName) == 0 )
			result.push_back(i);
	return result;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
char *CEntityList::GatherNetworkUpdate( UINT &dataSize )
{
	//First count length via the CBinaryBufferStream's dummy functionality
	OutStream dummyOs(true);
	SerializeNetworkUpdates(dummyOs, false); //dummy serialization

	dataSize = dummyOs.tellp();
	if( dataSize == 0 ) //no data?
		return NULL;

	//once the size is known start the serious stuff
	char *buffer = new char[dataSize];
	OutStream os(buffer, dataSize);
	SerializeNetworkUpdates(os, true);
	Assert(os.tellp() == dataSize);
	return buffer;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityList::SerializeNetworkUpdates( OutStream &os, bool resetChanged )
{
	for(int i = 0; i < GetCount(); i++)
	{
		CBaseEntity *ent = GetEntityInArray(i);
		if(!ent || !ent->IsNetworkChanged() )
			continue;
		
		ent->SerializeNetworkChanges(os);

		if( resetChanged )
			ent->ResetNetworkChanged();
	}
}
