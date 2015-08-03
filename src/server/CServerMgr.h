// server/CServerMgr.h
// server/CServerMgr.h
// server/CServerMgr.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__server__CServerMgr_H__
#define deferred__server__CServerMgr_H__

#include <util/networking.h>
#include "serverstartdata.h"

class CBaseEntity;
class CNetClient : public IEventListener
{
public:
	CNetClient( CNetSocket *socket );
	virtual ~CNetClient();
	void Init( void );
	void Update( void );
	bool HandleEvent( const IEvent *evt );
	void UnregisterEvents( void );
	void HandleDisconnect( void );
	inline CNetSocket *GetSocket( void ) { return m_socket; }
	inline void SetSocket( CNetSocket *pSock ) { m_socket = pSock; }
	inline int GetClientID( void ) const { return m_socket->GetClientID(); }
	void SetName(const std::string &name);
	const std::string &GetName();

private:
	CNetSocket *m_socket;
	CBaseEntity *m_pCar;
	bool m_bSendPingAnswer;
	int m_iPingAnswerId;
	std::string m_name;
};



class CServerMgr
{
public:
	CServerMgr( const serverstartdata_t &StartData );
	~CServerMgr();
	void Init( void );
	void LookForNewClients( void );
	void RecieveClientInput( void );
	void TransmitServerOutput( void );
	void UpdateClients( void );
	void SendNetworkUpdates( void );
private:
	CServerMgr() {}
	CServerMgr(const CServerMgr &other) {}
	int m_iMaxClients;
	int m_iClientIdCounter;
	SOCKET m_listensock;
	serverstartdata_t m_StartData;
	std::vector<CNetClient*> m_ClientList;
};

#endif
