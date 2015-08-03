// util/CTask.cpp
// util/CTask.cpp
// util/CTask.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CTask.h"
#include "error.h"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CTask::CTask( bool deleteOnDone )
{
	m_done = false;
	m_next = 0;
	m_deleteOnDone = deleteOnDone;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CTask::~CTask()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTask::StartTask( void )
{
	m_done = false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTask::SetTaskDone( void )
{
	m_done = true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CTask::IsTaskDone( void )
{
	return m_done;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CTask *CTask::SetNextTask( CTask *task )
{
	Assert(task);
	m_next = task;
	return m_next;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CTask *CTask::GetNextTask( void )
{
	return m_next;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTask::SetDeleteOnDone( bool deleteOnDone )
{
	m_deleteOnDone = deleteOnDone;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CTask::GetDeleteOnDone( void )
{
	return m_deleteOnDone;
}
