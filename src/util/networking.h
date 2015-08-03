// util/networking.h
// util/networking.h
// util/networking.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once
#ifndef deferred__util__networking_H__
#define deferred__util__networking_H__

#ifdef _WINDOWS
	#include <WinSock2.h>
	#include <ws2tcpip.h>
	#pragma comment(lib, "Ws2_32.lib")
#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <errno.h>
	typedef int SOCKET;
	#define SOCKET_ERROR -1
	#define INVALID_SOCKET -1
#endif

#include "timer.h"
#include "basic_types.h"
#include "IEvent.h"
#include "CEventManager.h"
#include <ostream>
#include <list>
#include "error.h"
#include "eventlist.h"
#include "CBinaryBufferStream.h"

int GetLastSocketError();


//system specific defines
#ifdef _WINDOWS
	#define SEWOULDBLOCK WSAEWOULDBLOCK
	#define SECONNRESET WSAECONNRESET
	#define SHUT_RDWR SD_BOTH
#else
	inline int closesocket(SOCKET sd)
	{
		return close(sd);
	}

	#define SEWOULDBLOCK EWOULDBLOCK
	#define SECONNRESET ECONNRESET

	#if SEWOULDBLOCK != EAGAIN
		#error What a crappy posix implementation
	#endif
#endif


//static const float g_InterpVal = 0.05f;

struct datagram_t
{
	datagram_t( UINT length, char *buf ) : l(length), b(buf) {}
	UINT l;
	char *b;
};

#pragma pack(push)
#pragma pack(1)
struct timemessage_t 
{
	float clientTime;
	float serverTime;
	SHORT msgtype;
};
#pragma pack(pop)


extern bool InitSocketApi( void );
extern bool SetSocketNonBlocking( SOCKET sock );
extern bool SetSocketNoDelay( SOCKET sock );



//This class accumulates incoming Events inside a small buffer to form
//reasonable large datagrams.
//When the Buffer is full, we add the datagram to our datagram list, which
//is traversed inside Send().
class CRemoteEventListener;
class CNetSocket
{
public:
	CNetSocket( SOCKET sock,
				const unsigned int &ip,
				CEventManager *pMgr,
				int iClientID,
				gpGlobals_t *pGlobals );
	~CNetSocket();
	void Init( void );
	inline void SetSocket( SOCKET sock ) { m_sock = sock; }
	inline void SetIP( unsigned int ip ) { m_ip = ip; }
	void SetEventManager( CEventManager *pEvtMgr );
	inline void SetClientID( const int &id ) { m_iClientID = id; }
	inline int GetClientID( void ) const { return m_iClientID; }
	void SendEvent( const IEvent *evt );
	void FlushPacket( void );
	bool Send( void );
	bool Recv( void );
	void Disconnect( void );
	void UnregisterEventListener( void );
	void HandleOutputEvent( IEvent* const pEvt );
	bool ShouldDisconnect( void ) { return m_iDisconnectFlag == 1; }

	inline UINT getBytesSent() { return m_bytesSent; }
	inline UINT getBytesReceived() { return m_bytesReceived; }

private:
	SOCKET m_sock;
	UINT m_ip;
	int m_iClientID;
	char m_iDisconnectFlag;

	//Things for sending
	OutStream m_SendStream;
	char *m_pchBuf;
	std::list<datagram_t> m_datagrams;
	UINT m_iSendOfs;
	UINT m_bytesSent;

	//Things for Recieving
	InStream m_RecvStream;
	char *m_pRecvBuf;
	UINT m_iRecvOfs;
	CEventManager *m_pEvtMgr;
	CRemoteEventListener *m_pMyListener;
	UINT m_bytesReceived;

	float m_fLastServerTime;
	float m_fServerTime;

	gpGlobals_t *m_pGlobals;

public:
	static const size_t sMaxPacketSize;
};


class CRemoteEventListener : public IEventListener
{
public:
	CRemoteEventListener() : m_pSocket(NULL) { }
	virtual ~CRemoteEventListener() { }
	CRemoteEventListener( CNetSocket *pSocket ) : m_pSocket(pSocket) { }
	inline bool HandleEvent( const IEvent *evt )
	{
		m_pSocket->SendEvent( evt );
		return true;
	}

	inline void UnregisterEvents( void )
	{
		//Dont forget to do this inside CNetSocket
	}

	inline CNetSocket *GetSocket( void ) { return m_pSocket; }
private:
	CNetSocket *m_pSocket;	
};


#endif
