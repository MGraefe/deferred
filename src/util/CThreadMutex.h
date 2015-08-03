// util/CThreadMutex.h
// util/CThreadMutex.h
// util/CThreadMutex.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once


//CThreadMutex is a wrapper for the Windows Critical Section or the Linux Mutex
class CThreadEvent;

class CThreadMutex
{
	friend class CThreadEvent;
public:
	CThreadMutex(void);
	~CThreadMutex(void);

	void SetOrWait( int iWaitTime = 1000 );
	void Release( void );

private:
#ifdef _WINDOWS
	void *m_Crit;
#else
	pthread_mutex_t m_mutex;
#endif
};


//CThreadEvent is a wrapper for the Windows Event Synchronization Object
class CThreadEvent
{
public:
	CThreadEvent(void);
	CThreadEvent(bool bInitial);
	~CThreadEvent(void);

	void Trigger( void );
	bool WaitAndRelease( unsigned int maxwait = 0xFFFFFFFF );
	bool Wait( unsigned int maxwait = 0xFFFFFFFF );
	void Release( void );
	bool Probe( void );

private:
#ifdef _WINDOWS
	void *m_hEvent;
#else
	CThreadMutex m_mutex;
	pthread_cond_t m_cond;
	volatile bool m_signaled;
	void _wait(unsigned int maxwait);
#endif
	void _init(bool initial);
};

