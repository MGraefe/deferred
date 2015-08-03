// client/TimeSyncer.h
// client/TimeSyncer.h
// client/TimeSyncer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

//TimeSyncer.h

#pragma once
#ifndef deferred__client__TimeSyncer_H__
#define deferred__client__TimeSyncer_H__

#include <util/CThread.h>
#include <util/FiniteTapContainer.h>
#include <util/networking.h>


class CTimeSyncer : public CThread
{
public:
	CTimeSyncer( UINT ipaddr, USHORT port );
	void Stop( void );

protected:
	virtual void ThreadRun( void );

private:
	void ReceiveMessage( sockaddr_in &addr, timemessage_t &request );

private:
	UINT m_ipaddr;
	USHORT m_port;
	volatile bool m_bShouldStop;
	SOCKET m_sock;
	CFiniteTapContainer<float,11> m_taps;
	
	static const DWORD sleepTime = 500; //ms
};

#endif // deferred__client__TimeSyncer_H__
