// util/CThread.cpp
// util/CThread.cpp
// util/CThread.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CThread.h"
#ifdef _WINDOWS
	#include <process.h>
#endif
#include "error.h"



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CThread::CThread( const std::string &name ) : m_sName(name)
{
#ifdef _WINDOWS
	m_hThread = NULL;
#else
	m_thread = 0;
#endif
	m_bRunning = false;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CThread::~CThread()
{
	if( m_bRunning )
	{
		error_fatal( "%s: Closing a running thread by the destructor", m_sName.c_str() );
	}

#ifdef _WINDOWS
	if( m_hThread )
		CloseHandle( m_hThread );
#endif
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CThread::Start( void )
{
	if(m_bRunning)
	{
		error( "%s: Calling start() on an already running thread...", m_sName.c_str() );
		return;
	}

#ifdef _WINDOWS
	if(m_hThread)
	{
		CloseHandle( m_hThread );
		m_hThread = NULL;
	}

	m_bRunning = true;
	m_hThread = (void*)_beginthreadex( NULL, 0, ThreadFunc, reinterpret_cast<void*>(this), NULL, NULL );
	if(!m_hThread)
		error_fatal("%s: Creating a Thread failed...", m_sName.c_str());
#else
	m_bRunning = true;
	int res = pthread_create(&m_thread, NULL, &ThreadFunc, reinterpret_cast<void*>(this));
	if( res != 0 )
		error_fatal( "%s: Creating a Thread failed...", m_sName.c_str());
#endif
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------

//void CThread::InitThread( void )
//{
//	m_hThread = (void*)_beginthreadex( NULL, 0, ThreadFunc, (void*)this, CREATE_SUSPENDED, NULL );
//
//	if( !m_hThread )
//		error_fatal( "%s: Creating a Thread failed...", m_sName.c_str() );
//}




//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
//void CThread::Terminate( void )
//{
//	Assert( m_hThread );
//	if( !m_hThread )
//		return;
//
//	if( m_bRunning )
//	{
//		if( TerminateThread( m_hThread, -1 ) != TRUE )
//			error( "%s: Error terminating the Thread.", m_sName.c_str() );
//	}
//	else
//	{
//		if( CloseHandle(m_hThread) != TRUE )
//			error( "%s: Error closing handle of Thread.", m_sName.c_str() );
//		m_hThread = NULL;
//	}
//}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
//void CThread::Pause( void )
//{
//	Assert( m_hThread );
//	if( !m_hThread )
//		return;
//
//	if( SuspendThread( m_hThread ) == -1 )
//		error( "%s: Error Suspending the Thread.", m_sName.c_str() );
//}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
//void CThread::Resume( void )
//{
//	Assert( m_hThread );
//	if( !m_hThread )
//		return;
//
//	if( ResumeThread( m_hThread ) == -1 )
//		error( "%s: Error Resuming the Thread.", m_sName.c_str() );
//}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CThread::IsRunning( void )
{
	return m_bRunning;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CThread::StartSync( void )
{
	m_bRunning = true;
	ThreadRun();
	m_bRunning = false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
#ifdef _WINDOWS
unsigned int __stdcall CThread::ThreadFunc( void *Thread )
{
	CThread *pThread = (CThread*)Thread;
	pThread->ThreadRun();
	pThread->m_bRunning = false;

	_endthreadex(NULL);

	return 0;
}
#else
void *CThread::ThreadFunc( void *Thread )
{
	CThread *pThread = (CThread*)Thread;
	pThread->ThreadRun();
	pThread->m_bRunning = false;

	return 0;
}
#endif
