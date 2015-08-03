// util/ConsoleHandler.h
// util/ConsoleHandler.h
// util/ConsoleHandler.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//ConsoleHandler.h

#pragma once
#ifndef deferred__util__ConsoleHandler_H__
#define deferred__util__ConsoleHandler_H__

#include "CThread.h"
#include "CThreadMutex.h"
#include <vector>
#include <list>
#include <string>

class CConVarBase;
typedef bool (ConsoleHandlerInputCallbackFunc)( const char* );
typedef CConVarBase* ConsoleHandlerGetConvarCallback( const std::string& );

class ConsoleHandler : public CThread
{
private:
	ConsoleHandler( const ConsoleHandler &handler ) : CThread( "" ) { }
public:
	ConsoleHandler();
	void ThreadRun( void );
	void AddCallback( ConsoleHandlerInputCallbackFunc *pCb, ConsoleHandlerGetConvarCallback *pGCb );
	void RemoveCallback( ConsoleHandlerInputCallbackFunc *pCb, ConsoleHandlerGetConvarCallback *pGCb );
	CConVarBase *GetConVarBase( const std::string &name );
	void HandleStoredInput(void);
private:
	std::vector<ConsoleHandlerInputCallbackFunc*> m_callbacks;
	std::vector<ConsoleHandlerGetConvarCallback*> m_getcallbacks;
	std::list<std::string> m_storedInput;
	CThreadMutex m_mutex;
};


#endif // deferred__util__ConsoleHandler_H__
