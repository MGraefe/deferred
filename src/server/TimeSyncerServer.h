// server/TimeSyncerServer.h
// server/TimeSyncerServer.h
// server/TimeSyncerServer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//TimeSyncerServer.h

#pragma once
#ifndef deferred__server__TimeSyncerServer_H__
#define deferred__server__TimeSyncerServer_H__


#include <util/CThread.h>
#include <util/networking.h>

class CTimeSyncerServer : public CThread
{
public:
	CTimeSyncerServer( USHORT port );
	void Stop(); //Hammertime

protected:
	void ThreadRun( void );
	
private:
	SOCKET m_sock;
	USHORT m_port;
	bool m_bShouldStop;

	static const unsigned int sleepTime = 1000;
};


#endif // deferred__server__TimeSyncerServer_H__
