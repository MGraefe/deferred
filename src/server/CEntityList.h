// server/CEntityList.h
// server/CEntityList.h
// server/CEntityList.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__server__CEntityList_H__
#define deferred__server__CEntityList_H__


#include "CBaseEntity.h"
#include <vector>

class CEntityList
{

public:
	CEntityList(); //Constructor
	~CEntityList();
	CBaseEntity *GetEntity( int index );
	CBaseEntity *GetEntity( const char *pName );
	CBaseEntity *GetEntityInArray( int i ) const;
	std::vector<int> GetEntityArrayIndicesByName(const char *pName);

	CBaseEntity *AddEntity( CBaseEntity *pEnt );
	int			GetCount( void ) const;

	void DeleteEntity( int index );
	void DeleteEntity( const char *pName );
	void DeleteEntity( CBaseEntity *pEnt );

	void SendSnapshot( int iPlayerID );

	void ThinkAllEntities( void );
	void UpdateAllEntities( void );

	//caller is responsible for delete[] on returned value
	//returns NULL when there is no data to be sent
	char *GatherNetworkUpdate( UINT &dataSize );

private:
	void SerializeNetworkUpdates(OutStream &os, bool resetChanged);

private:
	std::vector<CBaseEntity*> entitylist;
};




#endif
