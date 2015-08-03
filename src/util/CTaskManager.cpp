// util/CTaskManager.cpp
// util/CTaskManager.cpp
// util/CTaskManager.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "CTaskManager.h"
#include "CTask.h"
#include <algorithm>

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CTaskManager::CTaskManager()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTaskManager::AddTask( CTask *task )
{
	m_taskList.push_back(task);
	task->StartTask();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTaskManager::RemoveTask( CTask *task )
{
	TaskList::iterator it = m_taskList.begin();
	while( it != m_taskList.end() )
	{
		if( *it == task )
		{
			it = m_taskList.erase( it );
		}
		else
			it++;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTaskManager::UpdateTasks()
{
	TaskList::iterator it = m_taskList.begin();

	while( it != m_taskList.end() )
	{
		CTask *task = *it;
		task->RunTask();

		if( task->IsTaskDone() )
		{
			CTask *next = task->GetNextTask();
			if( next )
				AddTask(next);
			it = m_taskList.erase(it);

			if( task->GetDeleteOnDone() )
				delete task;
		}
		else
			it++;
	}
}
