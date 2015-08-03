// util/networking.cpp
// util/networking.cpp
// util/networking.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "networking.h"
#include <sstream>
#include "CBinaryBufferStream.h"
#include "timer.h"

//system specific includes
#ifndef _WINDOWS
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <errno.h>
#endif

//system specific error function
#ifdef _WINDOWS
	int GetLastSocketError()
	{
		return WSAGetLastError();
	}
#else
	int GetLastSocketError()
	{
		return errno;
	}
#endif


bool g_bSocketApiInitialized = false;
bool InitSocketApi( void )
{
	if( g_bSocketApiInitialized )
		return true;

#ifdef _WINDOWS
	//initialize socket lib
	WSADATA wsa;
	if( WSAStartup( MAKEWORD(2, 2), &wsa) )
	{
		error( "Socket activation failed" );
		return false;
	}
#endif

	g_bSocketApiInitialized = true;
	return true;
}


const size_t CNetSocket::sMaxPacketSize = std::numeric_limits<USHORT>::max() - 1;

#define DEBUG_NETWORKING
#ifdef DEBUG_NETWORKING
UINT g_uiMaxSendSizeDebug = 1;
UINT g_uiMaxRecvSizeDebug = 1;
#else
UINT g_uiMaxSendSizeDebug = CNetSocket::sMaxPacketSize;
UINT g_uiMaxRecvSizeDebug = CNetSocket::sMaxPacketSize;
#endif


CNetSocket::CNetSocket( SOCKET sock,
						const unsigned int &ip,
						CEventManager *pMgr,
						int iClientID,
						gpGlobals_t *pGlobals ) :
	m_sock(sock),
	m_ip(ip),
	m_SendStream(),
	m_iSendOfs(0),
	m_iRecvOfs(0),
	m_iDisconnectFlag(0),
	m_pEvtMgr(pMgr),
	m_iClientID(iClientID),
	m_RecvStream(),
	m_fLastServerTime(pGlobals->serveroffset),
	m_fServerTime(pGlobals->serveroffset),
	m_pMyListener(NULL),
	m_pGlobals(pGlobals),
	m_bytesReceived(0),
	m_bytesSent(0)
{ 
	m_pchBuf = new char[sMaxPacketSize];
	m_pRecvBuf = new char[sMaxPacketSize];
	memset(m_pchBuf, 0, sMaxPacketSize);
	m_SendStream.SetBuffer(m_pchBuf, sMaxPacketSize);
	m_RecvStream.SetBuffer(m_pRecvBuf, sMaxPacketSize);
	m_pMyListener = new CRemoteEventListener(this);
}


CNetSocket::~CNetSocket()
{
	//Disconnect();

	delete[] m_pchBuf;
	m_pchBuf = NULL;

	delete[] m_pRecvBuf;
	m_pRecvBuf = NULL;

	if( m_pMyListener )
	{
		m_pEvtMgr->UnregisterListener( ev::ALL_EVENTS, m_pMyListener );
		delete m_pMyListener;
		m_pMyListener = NULL;
	}
}


//Register a remote event listener to this client to transfer event-registrations
//to the remote side
void CNetSocket::Init( void )
{
	m_pEvtMgr->RegisterListener( ev::REMOTE_LISTENER_REGISTER, m_pMyListener );
}

bool SetSocketNonBlocking( SOCKET sock )
{
	//Set to nonblocking
#ifdef _WINDOWS
	u_long one = 1;
	if( ioctlsocket( sock, FIONBIO, &one ) == SOCKET_ERROR )
	{
		error( "ioctlsocket() on socket %i failed.", sock );
		return false;
	}
#else
	int current = fcntl(sock, F_GETFL);
	if(current == -1)
	{
		error( "fcntl(sock, F_GETFL) on socket %i failed", sock );
		return false;
	}

	if( fcntl(sock, F_SETFL, current | O_NONBLOCK) == -1 )
	{
		error( "fcntl(sock, F_SETFL, current | O_NONBLOCK) on socket %i failed", sock );
		return false;
	}
#endif

	//Set lingering
	linger ling;
	ling.l_onoff = 0;
	ling.l_linger = 0;

	if( setsockopt( sock, SOL_SOCKET, SO_LINGER, (const char*)&ling, sizeof(ling) ) == SOCKET_ERROR )
	{
		error( "setsockopt() with SO_LINGER failed on socket %i.", sock );
		return false;
	}

	return true;
}

bool SetSocketNoDelay( SOCKET sock )
{
	int flag = 1;
	if( setsockopt( sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int) ) == SOCKET_ERROR )
	{
		error( "setsocketopt() with TCP_NODELAY on socket %i failed with error %i", GetLastSocketError() );
		return false;
	}
	return true;
}

void CNetSocket::SetEventManager( CEventManager *pEvtMgr )
{ 
	m_pEvtMgr = pEvtMgr;
}


void CNetSocket::UnregisterEventListener( void )
{
	//unregister us from all events
	if( !m_pMyListener )
		return;
	
	m_pEvtMgr->UnregisterListener( ev::ALL_EVENTS, m_pMyListener );
	delete m_pMyListener;
	m_pMyListener = NULL;
}


void CNetSocket::Disconnect( void )
{
	if( m_sock == INVALID_SOCKET )
		return;
	
	shutdown( m_sock, SHUT_RDWR );

	if( closesocket( m_sock ) == SOCKET_ERROR )
		error( "closesocket() failed on socket %i with Error: %i", m_sock, GetLastSocketError() );

	m_sock = INVALID_SOCKET;
}


//Put an Event in our Buffer, if buffer is too small flush it
//(over the network)
void CNetSocket::SendEvent( const IEvent *evt )
{
	const int length = (int)m_SendStream.tellp();
	if( length + evt->GetNetworkSize() > sMaxPacketSize )
		FlushPacket();

	evt->VSerialize( m_SendStream );
}

//Put the packet in our datagram list
void CNetSocket::FlushPacket( void )
{
	int length = (int)m_SendStream.tellp();
	if( length > 0 )
	{
		m_datagrams.push_back( datagram_t(length, m_pchBuf) );
		m_pchBuf = new char[sMaxPacketSize];
		m_SendStream.SetBuffer( m_pchBuf, sMaxPacketSize ); //this automatically resets get and put ptr
	}
}

//Send the datagrams in our list over the net
//This one should be polled
bool CNetSocket::Send( void )
{
	while( !m_datagrams.empty() )
	{
		datagram_t &data = *m_datagrams.begin();
		int st = send( m_sock, data.b + m_iSendOfs, min(data.l - m_iSendOfs, g_uiMaxSendSizeDebug), 0 );
		if( st > 0 )
		{
			m_bytesSent += st;
			m_iSendOfs += st;
			if( m_iSendOfs == data.l )
			{
				delete[] data.b;
				m_datagrams.pop_front();
				m_iSendOfs = 0;
			}
		}
		else
		{
			int er = GetLastSocketError();
			if( er != SEWOULDBLOCK )
			{
				if( er != SECONNRESET )
					error( "Socket %i Error: %i", (int)m_sock, er );
				else
					ConsoleMessage( "Connection Reset by Peer on Socket %i", (int)m_sock );
				m_iDisconnectFlag = 1;
			}
			return false;
		}
	}
	return true;
}


bool CNetSocket::Recv( void )
{
	if( m_iRecvOfs >= sMaxPacketSize )
	{
		Assert( m_iRecvOfs < sMaxPacketSize );
		error( "Fatal Error in Recv() on socket %i", m_sock );
		m_iDisconnectFlag = 1;
		return false;
	}

	//rc stores the actual received bytes
	int rcRaw = recv( m_sock, m_pRecvBuf+m_iRecvOfs, min(sMaxPacketSize - m_iRecvOfs, g_uiMaxRecvSizeDebug), 0 );
	
	//Receive zero or anything that does not make sense?
	if( rcRaw <= 0 && rcRaw != SOCKET_ERROR )
	{
		m_iDisconnectFlag = 1;
		return false;
	}

	//Socket error? (this includes SEWOULDBLOCK, which simply means "there is no more data for you".)
	if( rcRaw == SOCKET_ERROR )
	{
		int er = GetLastSocketError();
		if( er != SEWOULDBLOCK )
		{
			error( "Socket %i Error: %i", (int)m_sock, er );
			m_iDisconnectFlag = 1;
		}
		return false;
	}

	//rc is now guaranteed to be >= 0
	UINT rc = (UINT)rcRaw; 

	m_bytesReceived += rc;

	//no packet type yet?
	if( m_iRecvOfs == 0 && rc < sizeof(EventType) )
	{
		m_iRecvOfs = rc;
		return true;
	}

	UINT iEvtOfs = 0; //The offset for already handled events in the input queue
	while(iEvtOfs < m_iRecvOfs + rc) //loop through received events
	{
		const UINT remBytes = m_iRecvOfs + rc - iEvtOfs;
		const char * const evtStart = m_pRecvBuf + iEvtOfs;

		EventType iEvtType = *((const EventType*)evtStart); //we are guaranteed to have this
		IEventFactory *pFactory = ev::GetEventFactory( iEvtType );
		if( !pFactory )
		{
			error( "Unknown Event: %i", iEvtType );
			m_iDisconnectFlag = 1; //We do not need clients that spam unknown stuff
			return false;
		}
		IEvent *evt = pFactory->Create();

		UINT iEvtSize = -1;
		if( evt->IsDynamicSize() )
		{
			//Enough bytes for header?
			if( IEventDynamicSized::GetHeaderSize() <= remBytes )
			{
				iEvtSize = *((IEventDynamicSized::SizeType*)(evtStart + IEvent::GetHeaderSize()));
				Assert( iEvtSize >= IEventDynamicSized::GetHeaderSize() );
				((IEventDynamicSized*)evt)->SetNetworkSizeReceived(iEvtSize);
			}
		}
		else
			iEvtSize = ((IEventStaticSized*)evt)->GetNetworkSize();
		Assert( iEvtSize == UINT(-1) || iEvtSize >= sizeof(EventType) ); //minimum size because of event id
					
		//If the event is incomplete, move the already received
		//bytes to the beginning of our receive buffer and return.
		if( iEvtSize == UINT(-1) || iEvtSize > remBytes )
		{
			//There is no need to move back the data if this is the fist event in the receive buffer.
			if( iEvtOfs > 0)
			{
				Assert( iEvtOfs < (int)sMaxPacketSize );
				Assert( remBytes + iEvtOfs <= sMaxPacketSize );
				memmove( m_pRecvBuf, evtStart, remBytes );
				m_iRecvOfs = remBytes;
			}
			else
				m_iRecvOfs += rc;
			delete evt;
			break;		
		}
		else //The event is fully complete
		{
			Assert( iEvtSize >= sizeof(EventType) );
			m_RecvStream.seekg( iEvtOfs ); //The recv stream operates on the receive buffer
			evt->VDeserialize( m_RecvStream );
			HandleOutputEvent( evt );

			iEvtOfs += iEvtSize;

			Assert( iEvtOfs <= m_iRecvOfs + rc );

			//are we done with all events?
			if( iEvtOfs == m_iRecvOfs + rc )
				m_iRecvOfs = 0;
		} //else event not complete?
	} //while iEvtOfs < rc ?

	return true;
}


//#ifdef _CLIENT
std::vector<float> g_SnapshotDelays;

//#endif

void CNetSocket::HandleOutputEvent( IEvent* const pEvt )
{
	//bool bReturnVal = true;
	pEvt->SetClientID( m_iClientID );

	//When the remote side wants to register with a local-generated event
	//we create a remoteeventlistener which sends the local-generated event
	//over the wire.
	if( pEvt->GetType() == ev::REMOTE_LISTENER_REGISTER )
	{
		CIntegerDataEvent *pListenerEvt = (CIntegerDataEvent*)pEvt;
		if( pListenerEvt )
			m_pEvtMgr->RegisterListener( pListenerEvt->GetValue(),
				m_pMyListener );
		return;
	}

	//The server side does not care about interpolation
	if( m_pEvtMgr->GetLocation() == LOCATION_CLIENT )
	{
		if( pEvt->GetType() == ev::SERVER_SNAPSHOT_START )
		{
			// We have to tell the interpolator the elapsed time since the last snapshot.
			// Since the interpolator lives "inside the past" we just add an event in front of our position-update-queue
			// that tells him the right value right before he is going to recieve the updates.

			m_fServerTime = ((CFloatDataEvent*)pEvt)->GetValue();
			float fSnapshotTime = m_fServerTime - m_fLastServerTime;

			CFloatDataEvent *pFltEvt = new CFloatDataEvent( ev::TELL_SNAPSHOT_TIME, fSnapshotTime );

			pFltEvt->SetProcessTime( ToClientTime(m_pGlobals->serveroffset, m_fLastServerTime) + g_InterpVal );
			m_pEvtMgr->AddDelayedEventToQueue( pFltEvt, m_pGlobals->curtime );

			pEvt->SetProcessTime( ToClientTime(m_pGlobals->serveroffset, m_fLastServerTime) + g_InterpVal );
		}
		else if( pEvt->GetType() == ev::SERVER_SNAPSHOT_END )
		{
			pEvt->SetProcessTime( ToClientTime(m_pGlobals->serveroffset, m_fServerTime) + g_InterpVal );
			m_fLastServerTime = m_fServerTime; //update last server time
		}
		else if( pEvt->GetType() == ev::NETTABLE_UPDATE )
		{
			// We copy the event and send a second one with identical data, but different type.
			// This is because data in events with type NETTABLE_UPDATE_INTERP are processed by the interpolators
			// for data which have interpolators set.
			// Data in events with type NETTABLE_UPDATE (that have no interpolators) are processed the regular way
			CNettableUpdate *newEvt = ((CNettableUpdate*)pEvt)->copy();
			newEvt->SetType(ev::NETTABLE_UPDATE_INTERP);
			newEvt->SetProcessTime( ToClientTime(m_pGlobals->serveroffset, m_fLastServerTime) + g_InterpVal );
			m_pEvtMgr->AddDelayedEventToQueue(newEvt, m_pGlobals->curtime);
			
			pEvt->SetProcessTime( ToClientTime(m_pGlobals->serveroffset, m_fServerTime) + g_InterpVal );
		}
		else
		{
			//normal events get the interpVal as processing time
			pEvt->SetProcessTime( ToClientTime(m_pGlobals->serveroffset, m_fServerTime) + g_InterpVal );
		}
		m_pEvtMgr->AddDelayedEventToQueue( pEvt, m_pGlobals->curtime );
	}
	else //m_pEvtMgr->GetLocation() == LOCATION_CLIENT
	{
		m_pEvtMgr->AddEventToQueue( pEvt );
	}
}


