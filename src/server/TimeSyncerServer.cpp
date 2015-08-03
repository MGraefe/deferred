// server/TimeSyncerServer.cpp
// server/TimeSyncerServer.cpp
// server/TimeSyncerServer.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "TimeSyncerServer.h"
#include "ServerInterfaces.h"
#include <util/timer.h>

#ifndef _WINDOWS
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <netinet/udp.h>
#endif

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CTimeSyncerServer::CTimeSyncerServer( USHORT port ) : CThread( "server_time_syncer" )
{
	m_port = port;
	m_bShouldStop = false;
	m_sock = INVALID_SOCKET;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTimeSyncerServer::Stop( void )
{
	m_bShouldStop = true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTimeSyncerServer::ThreadRun( void )
{
	while( !m_bShouldStop )
	{
		if( m_sock == INVALID_SOCKET )
		{
			m_sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
			if( m_sock == INVALID_SOCKET )
			{
				ConsoleMessage( "Server: CTimeSyncerServer: Cant open socket, trying again in %i ms.", sleepTime );
				CCTime::Sleep(sleepTime);
				continue;
			}

			sockaddr_in sockAddrIn;
			sockAddrIn.sin_addr.s_addr = INADDR_ANY;
			sockAddrIn.sin_port = htons( m_port );
			sockAddrIn.sin_family = AF_INET;

			if( bind(m_sock, (sockaddr*)&sockAddrIn, sizeof(sockAddrIn)) == SOCKET_ERROR )
			{
				ConsoleMessage( "Server: CTimeSyncerServer: bind() to port %i failed. Trying again in %i ms.", m_port, sleepTime );
				closesocket(m_sock);
				m_sock = INVALID_SOCKET;
				CCTime::Sleep(sleepTime);
				continue;
			}
		}

		
		timemessage_t request;
		sockaddr addrFrom;
		socklen_t addrFromLength = (socklen_t)sizeof(addrFrom);
		int rc = recvfrom( m_sock, (char*)(&request), sizeof(request), 0, &addrFrom, &addrFromLength );
		if( rc == SOCKET_ERROR )
		{
			ConsoleMessage( "Server: CTimeSyncerServer: error in recvfrom(). Trying to recover..." );
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
			CCTime::Sleep(sleepTime);
			continue;
		}

		if( request.msgtype != ev::TIME_SYNC_MESSAGE )
		{
			ConsoleMessage( "Server: CTimeSyncerServer: received wrong message-type. " );
			continue;
		}

		//put current servertime into request and return message.
		request.serverTime = (float)singletons::g_pTimer->GetTime();
		
		int sd = sendto( m_sock, (char*)(&request), sizeof(request), 0, &addrFrom, sizeof(addrFrom) );
		if( sd == SOCKET_ERROR )
		{
			ConsoleMessage( "Server: CTimeSyncerServer: sendto() failed somehow. Trying to recover..." );
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
			CCTime::Sleep(sleepTime);
			continue;
		}
	}

	if( m_sock != INVALID_SOCKET )
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
}
