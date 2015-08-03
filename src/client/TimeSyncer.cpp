// client/TimeSyncer.cpp
// client/TimeSyncer.cpp
// client/TimeSyncer.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "TimeSyncer.h"
#include <util/networking.h>
#include "CClientInterf.h"
#include <util/timer.h>
#ifndef _WINDOWS
#include <sys/select.h>
#endif

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CTimeSyncer::CTimeSyncer( UINT ipaddr, USHORT port ) : CThread( "client_time_syncer" )
{
	m_ipaddr = ipaddr;
	m_bShouldStop = false;
	m_sock = INVALID_SOCKET;
	m_port = port;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTimeSyncer::Stop( void )
{
	m_bShouldStop = true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTimeSyncer::ThreadRun( void )
{
	while( !m_bShouldStop )
	{
		if( m_sock == INVALID_SOCKET )
		{
			m_sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
			if( m_sock == INVALID_SOCKET )
			{
				ConsoleMessage( "Client: CTimeSyncer: cant create socket, socket() failed." );
				CCTime::Sleep(sleepTime);
				continue;
			}
		}

		sockaddr_in addr;
		timemessage_t request;

		//prepare address
		addr.sin_family = AF_INET;
		addr.sin_port = htons(m_port);
		addr.sin_addr.s_addr = m_ipaddr;

		//Prepare request
		request.msgtype = ev::TIME_SYNC_MESSAGE;
		request.clientTime = (float)g_pTimer->GetTime();
		request.serverTime = -1.0f;

		//Send request
		int sd = sendto( m_sock, (char*)(&request), sizeof(request), 0, (sockaddr*)(&addr), sizeof(addr) );
		if( sd == SOCKET_ERROR )
		{
			ConsoleMessage( "Client: CTimeSyncer: Sending time-sync message failed. Trying to recover..." );
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
			CCTime::Sleep(sleepTime);
			continue;
		}

		ReceiveMessage(addr, request);
	}

	if( m_sock != INVALID_SOCKET )
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTimeSyncer::ReceiveMessage( sockaddr_in &addr, timemessage_t &request )
{
	while( true )
	{
		//Prepare for select
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(m_sock, &fd);
//		fd.fd_count = 1;
//		fd.fd_array[0] = m_sock;

		timeval timeout;
		timeout.tv_sec = 1; //one second timeout
		timeout.tv_usec = 0;

		int ac = select( m_sock+1, &fd, NULL, NULL, &timeout );
		if( ac == SOCKET_ERROR )
		{
			ConsoleMessage( "Client: CTimeSyncer: Unusual error in select() function. Trying to recover..." );
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
			CCTime::Sleep(sleepTime);
			return;
		}

		//Timeout? Maybe a lost packet, or a seriously bad connection, however, try again.
		if( ac == 0 )
			return;

		//Receive packet
		timemessage_t answer;
#ifdef _WINDOWS
		int bufsize = (int)(sizeof(addr));
#else
		size_t bufsize = sizeof(addr);
#endif
		int rc = recvfrom(m_sock, (char*)(&answer), sizeof(answer), 0, (sockaddr*)(&addr), &bufsize );

		if( rc == SOCKET_ERROR )
		{
			ConsoleMessage( "Client: CTimeSyncer: recvfrom() failed. Trying to recover... " );
			closesocket(m_sock);
			m_sock = INVALID_SOCKET;
			CCTime::Sleep(sleepTime);
			return;
		}

		//if( bufsize != 4 )
		//	ConsoleMessage( "Client: CTimeSyncer: Received bogus address" );

		if( rc != sizeof(answer) )
		{
			ConsoleMessage( "Client: CTimeSyncer: Received fragmented packet." );
			CCTime::Sleep(sleepTime);
			return;
		}

		//Check server ip

		if( addr.sin_addr.s_addr != m_ipaddr )
		{
			ConsoleMessage( "Client: CTimeSyncer: Received time answer from different ip than the server ip. Funny thing." );
			CCTime::Sleep(sleepTime);
			return;
		}

		//Check message type
		if( answer.msgtype != ev::TIME_SYNC_MESSAGE )
		{
			ConsoleMessage( "Client: CTimeSyncer: Received message with different type?!" );
			CCTime::Sleep(sleepTime);
			return;
		}

		//Check if this is the reply to the message we want.
		if( answer.clientTime != request.clientTime )
		{
			ConsoleMessage( "Client: CTimeSyncer: Received message from different client or old message." );
			continue;
		}

		//When we get here there should be no errors, the message received should be ok.
		float currentTime = (float)g_pTimer->GetTime();
		float rtt = currentTime - answer.clientTime;
		//ConsoleMessage( "Client: RTT is %i ms.", ROUND(rtt*1000.0f) );
		float serveroffset = answer.serverTime - currentTime - rtt/2.0f;

		m_taps.addTap( serveroffset );
		gpGlobals->serveroffset_calculated = m_taps.getAverage();
		gpGlobals->rtt = rtt;
		CCTime::Sleep(sleepTime);
		return;
	}
}

