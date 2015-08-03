// client/CClientLapTimeCounter.cpp
// client/CClientLapTimeCounter.cpp
// client/CClientLapTimeCounter.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CClientInterf.h"
#include "CClientLapTimeCounter.h"
#include "datadesc_client.h"

CClientLapTimeCounter *g_ClientLapTimeCounter = NULL;

LINK_ENTITY_TO_CLASSNAME_CLIENT(CClientLapTimeCounter, logic_laptimecounter);
BEGIN_DATADESC_EDITOR(CClientLapTimeCounter)
	NETTABLE_ENTRY_ARRAY(m_timeTable, LAPTIMETABLE_NUM_ENTRIES)
END_DATADESC_EDITOR()


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CClientLapTimeCounter::CClientLapTimeCounter(int index) :
	BaseClass(index)
{
	m_nettableChanged = true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CClientLapTimeCounter::OnSpawn()
{
	g_ClientLapTimeCounter = this;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CClientLapTimeCounter::TableHasChanged()
{
	return m_nettableChanged;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const LapTimeTableEntry * CClientLapTimeCounter::GetTable()
{
	return m_timeTable;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CClientLapTimeCounter::OnDataUpdate( void )
{
	m_nettableChanged = true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CClientLapTimeCounter::ResetTableChanged()
{
	m_nettableChanged = false;
}

