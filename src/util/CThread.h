// util/CThread.h
// util/CThread.h
// util/CThread.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__CThread_H__
#define deferred__util__CThread_H__

//#ifndef _MSC_VER
//#error Theres Currently only windows support in CThread.h
//#endif

#include <string>
#ifdef _LINUX
#include <pthread.h>
#endif

class CThread
{
public:
	CThread( const std::string &name );
	virtual ~CThread();
	void Start( void );
	void StartSync( void );
//	void Terminate( void );
//	void Pause( void );
//	void Resume( void );
	bool IsRunning( void );

protected:
	virtual void ThreadRun( void ) = 0;

private:
#ifdef _WINDOWS
	static unsigned int __stdcall ThreadFunc( void *Thread );
#else
	static void *ThreadFunc(void *Thread);
#endif

//	void InitThread( void );

private:
#ifdef _WINDOWS
	void *m_hThread;
#else
	pthread_t m_thread;
#endif
	volatile bool m_bRunning;
	std::string m_sName;
};



#endif
