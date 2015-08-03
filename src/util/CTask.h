// util/CTask.h
// util/CTask.h
// util/CTask.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

//CTask.h

#pragma once
#ifndef deferred__util__CTask_H__
#define deferred__util__CTask_H__

class CTask
{
public:
	virtual void RunTask(void) = 0;
	virtual void InitTask(void) { }

	CTask( bool deleteOnDone = true );
	virtual ~CTask();

	void StartTask(void);
	void SetTaskDone(void);
	bool IsTaskDone(void);
	CTask *SetNextTask(CTask *task);
	CTask *GetNextTask(void);
	void SetDeleteOnDone( bool deleteOnDone );
	bool GetDeleteOnDone(void);

private:
	CTask *m_next;
	bool m_done;
	bool m_deleteOnDone;
};




#endif // deferred__util__CTask_H__
