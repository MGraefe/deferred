// client/CClientLapTimeCounter.h
// client/CClientLapTimeCounter.h
// client/CClientLapTimeCounter.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CClientLapTimeCounter_H__
#define deferred__client__CClientLapTimeCounter_H__

#include <renderer/CRenderEntity.h>
#include "../server/LapTimeTableEntry.h"

class CClientLapTimeCounter : public CRenderEntity
{
	DECLARE_DATADESC_EDITOR(CRenderEntity, CClientLapTimeCounter)
	DECLARE_ENTITY_LINKED()

public:
	CClientLapTimeCounter(int index);
	virtual void OnSpawn();
	bool TableHasChanged();
	void ResetTableChanged();
	const LapTimeTableEntry *GetTable();

	virtual void OnDataUpdate( void );

	virtual void VRender( const CRenderParams &params ) {}
	virtual void VOnRestore( void ) {}
	virtual void VPreDelete( void ) {}

private:
	bool m_nettableChanged;
	LapTimeTableEntry m_timeTable[LAPTIMETABLE_NUM_ENTRIES];
};


extern CClientLapTimeCounter *g_ClientLapTimeCounter;


#endif // deferred__client__CClientLapTimeCounter_H__