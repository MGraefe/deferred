// util/CThreadMutex.cpp
// util/CThreadMutex.cpp
// util/CThreadMutex.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "error.h"

//CThreadMutex is a wrapper for the Windows Critical Section or the Linux Mutex
#include "CThreadMutex.h"

#define CHECK_UNIX_ERROR(func) if((func) != 0) error_fatal(#func)

CThreadMutex::CThreadMutex(void)
{
#ifdef _WINDOWS
	m_Crit = new CRITICAL_SECTION;
	InitializeCriticalSection( (LPCRITICAL_SECTION)m_Crit );
	//No Error Code here
#else
	pthread_mutexattr_t ma;
	CHECK_UNIX_ERROR(pthread_mutexattr_init(&ma));
	CHECK_UNIX_ERROR(pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE));
	CHECK_UNIX_ERROR(pthread_mutexattr_setrobust(&ma, PTHREAD_MUTEX_STALLED));
	CHECK_UNIX_ERROR(pthread_mutex_init(&m_mutex, &ma));
	CHECK_UNIX_ERROR(pthread_mutexattr_destroy(&ma));
#endif
}


CThreadMutex::~CThreadMutex(void)
{
#ifdef _WINDOWS
	DeleteCriticalSection( (LPCRITICAL_SECTION)m_Crit );
	delete m_Crit;
#else
	CHECK_UNIX_ERROR(pthread_mutex_destroy(&m_mutex));
#endif
}


void CThreadMutex::SetOrWait( int iWaitTime )
{
#ifdef _WINDOWS
	EnterCriticalSection( (LPCRITICAL_SECTION)m_Crit );
#else
	CHECK_UNIX_ERROR(pthread_mutex_lock(&m_mutex));
#endif
}


void CThreadMutex::Release( void )
{
#ifdef _WINDOWS
	LeaveCriticalSection( (LPCRITICAL_SECTION)m_Crit );
#else
	CHECK_UNIX_ERROR(pthread_mutex_unlock(&m_mutex));
#endif
}




CThreadEvent::CThreadEvent()
{
	_init(false);
}


CThreadEvent::CThreadEvent( bool bInitial )
{
	_init(bInitial);
}

void CThreadEvent::_init(bool initial)
{
#ifdef _WINDOWS
	m_hEvent = CreateEvent( NULL, TRUE, initial ? TRUE : FALSE, NULL );
	if( m_hEvent == NULL )
	{
		MessageBox( NULL, "CreateEvent failed for some reason!", "Critical Failure", MB_OK|MB_ICONERROR );
	}
#else
	pthread_condattr_t ca;
	CHECK_UNIX_ERROR(pthread_condattr_init(&ca));
	CHECK_UNIX_ERROR(pthread_condattr_setpshared(&ca, PTHREAD_PROCESS_PRIVATE));
	CHECK_UNIX_ERROR(pthread_cond_init(&m_cond, &ca));
	CHECK_UNIX_ERROR(pthread_condattr_destroy(&ca));
	m_signaled = initial;
#endif
}

CThreadEvent::~CThreadEvent()
{
#ifdef _WINDOWS
	if( m_hEvent != NULL )
		CloseHandle( m_hEvent );
#else
	CHECK_UNIX_ERROR(pthread_cond_destroy(&m_cond));
#endif
}

void CThreadEvent::Trigger( void )
{
#ifdef _WINDOWS
	SetEvent( m_hEvent );
#else
	m_signaled = true;
	CHECK_UNIX_ERROR(pthread_cond_broadcast(&m_cond));
#endif
}

#ifndef _WINDOWS
void CThreadEvent::_wait(unsigned int maxwait)
{
	if(maxwait == 0xFFFFFFFF)
		pthread_cond_wait(&m_cond, &m_mutex.m_mutex);
	else
	{
		timespec ts;
		ts.tv_sec = maxwait / 1000;
		ts.tv_nsec = (maxwait % 1000) * 10e6;
		pthread_cond_timedwait(&m_cond, &m_mutex.m_mutex, &ts);
	}
}
#endif

bool CThreadEvent::WaitAndRelease( unsigned int maxwait )
{
#ifdef _WINDOWS
	DWORD ret = WaitForSingleObject( m_hEvent, maxwait );
	ResetEvent( m_hEvent );
	return ret == WAIT_OBJECT_0;
#else
	m_mutex.SetOrWait();
	while(!m_signaled)
		_wait(maxwait);
	m_mutex.Release();
	m_signaled = false;
	return true;
#endif
}

bool CThreadEvent::Wait( unsigned int maxwait )
{
#ifdef _WINDOWS
	return WaitForSingleObject( m_hEvent, maxwait ) == WAIT_OBJECT_0;
#else
	m_mutex.SetOrWait();
	while(!m_signaled)
		_wait(maxwait);
	m_mutex.Release();
	return true;
#endif
}

void CThreadEvent::Release( void )
{
#ifdef _WINDOWS
	ResetEvent( m_hEvent );
#else
	m_signaled = false;
#endif
}

bool CThreadEvent::Probe( void )
{
#ifdef _WINDOWS
	return WaitForSingleObject( m_hEvent, 0 ) == WAIT_OBJECT_0;
#else
	return m_signaled;
#endif
}
