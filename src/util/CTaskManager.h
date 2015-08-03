// util/CTaskManager.h
// util/CTaskManager.h
// util/CTaskManager.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

//CTaskManager.h

#pragma once
#ifndef deferred__util__CTaskManager_H__
#define deferred__util__CTaskManager_H__

#include <vector>

class CTask;
class CTaskManager
{
public:
	CTaskManager();
	void AddTask(CTask *task);
	void RemoveTask(CTask *task);
	void UpdateTasks();

private:
	typedef std::vector<CTask*> TaskList;
	TaskList m_taskList;
};




#endif // deferred__util__CTaskManager_H__
