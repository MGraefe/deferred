// server/CServerMgr.cpp
// server/CServerMgr.cpp
// server/CServerMgr.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include <util/networking.h>
#ifndef _WINDOWS
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
#endif
#include "CServerMgr.h"
#include "serverstartdata.h"
#include "ServerInterfaces.h"
#include "CPlayerCar.h"
#include "CStaticModel.h"
#include "CVehicleRespawnPosition.h"

CNetClient::CNetClient( CNetSocket *socket ) :
	m_socket(socket),
	m_pCar(NULL),
	m_bSendPingAnswer(false),
	m_name()
{
}


CNetClient::~CNetClient()
{
	//delete m_socket; //Destructor handles disconnect
}


void CNetClient::UnregisterEvents( void )
{
	singletons::g_pEvtMgr->UnregisterListener( ev::ALL_EVENTS, this );
}


void CNetClient::Init( void )
{
	m_socket->Init();
	singletons::g_pEvtMgr->RegisterListener( ev::CLIENT_FINISHED_REGISTER, this );
	singletons::g_pEvtMgr->RegisterListener( ev::PING_REQUEST, this );
	singletons::g_pEvtMgr->RegisterListener( ev::SET_CLIENT_NAME, this );
}


bool GetCarSpawnPosition(Vector3f &pos, Angle3d &ang)
{
	auto spawns = singletons::g_pWorld->getInitialSpawnPositions();

	if(spawns.empty())
	{
		ConsoleMessage("WARNING: no spawn positions for car, using world center");
		pos = vec3_null;
		ang = vec3_null;
	}
	else
	{
		for(auto spawn : spawns)
		{
			if(!spawn->IsOccupied())
			{
				pos = spawn->GetOrigin();
				ang = spawn->GetAngle();
				return true;
			}
		}

		pos = spawns.front()->GetOrigin();
		ang = spawns.front()->GetAngle();
	}

	return false;
}


bool CNetClient::HandleEvent( const IEvent *evt )
{
	if( evt->GetType() == ev::CLIENT_FINISHED_REGISTER &&
		evt->GetClientID() == GetClientID() )
	{
		//Set client id
		singletons::g_pEvtMgr->AddEventToQueue( 
			new CIntegerDataEvent( 
				ev::SET_LOCAL_CLIENT_ID, GetClientID() ),
			GetClientID() );


		CPlayerCar *car = CREATE_ENTITY( CPlayerCar );
		Vector3f orig;
		Angle3d ang;
		GetCarSpawnPosition(orig, ang);
		car->SetOrigin( orig );
		car->SetAngle( ang );
		car->SetName( m_name.c_str() );
		car->SetVisible( true );
		car->SetPlayerID( 0 );
		car->SetClientID( GetClientID() );
		car->RegisterEvents( singletons::g_pEvtMgr );
		car->Spawn( true, false );
		m_pCar = car;

		singletons::g_pEntityList->SendSnapshot( GetClientID() );

		ConsoleMessage("CServerMgr: sending WORLD_CREATED Event");
		singletons::g_pEvtMgr->AddEventToQueue( 
			new CRenderWorldCreateEvent( ev::WORLD_CREATED, -1, singletons::g_pWorld->GetScriptName().c_str(), vec3_null, vec3_null ), GetClientID() );
		return true;
	}

	if( evt->GetType() == ev::PING_REQUEST &&
		evt->GetClientID() == GetClientID() )
	{
		m_bSendPingAnswer = true;
		m_iPingAnswerId = ((CIntegerDataEvent*)evt)->GetValue();
		return true;
	}


	if( evt->GetType() == ev::SET_CLIENT_NAME &&
		evt->GetClientID() == GetClientID() )
	{
		SetName(((CStringDataEvent*)evt)->GetString());
		return true;
	}

	return false;
}


void CNetClient::HandleDisconnect( void )
{
	//Delete the players car
	if(m_pCar)
		m_pCar->Destroy();

	//dont forget to unregister our socket from all events that he may have registered to
	m_socket->UnregisterEventListener();

	m_socket->Disconnect();
}

void CNetClient::Update( void )
{
	if( m_bSendPingAnswer )
	{
		singletons::g_pEvtMgr->AddEventToQueue( new CIntegerDataEvent(ev::PING_ANSWER, m_iPingAnswerId), GetClientID() );
		m_bSendPingAnswer = false;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CNetClient::SetName( const std::string &name )
{
	m_name = name;
	if(m_pCar)
		m_pCar->SetName(name.c_str());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const std::string & CNetClient::GetName()
{
	return m_name;
}



CServerMgr::CServerMgr( const serverstartdata_t &StartData ) :
	m_StartData(StartData)
{
	m_listensock = INVALID_SOCKET;
	m_iMaxClients = 16;
	m_iClientIdCounter = CLIENT_ID_CLIENTS_START;
}


CServerMgr::~CServerMgr()
{
	for( UINT i = 0; i < m_ClientList.size(); i++ )
	{
		m_ClientList[i]->GetSocket()->Disconnect();
		delete m_ClientList[i];
	}

	//WSACleanup();
}




void CServerMgr::Init( void )
{
	if( !InitSocketApi() )
		return;

	//create socket
	m_listensock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( m_listensock == INVALID_SOCKET )
	{
		error( "socket() function failed." );
		return;
	}

	//Set to nonblocking
	if( !SetSocketNonBlocking( m_listensock ) )
	{
		closesocket(m_listensock);
		return;
	}

	sockaddr_in sockAddrIn;
	if( m_StartData.type == SERVERTYPE_LOCAL )
		sockAddrIn.sin_addr.s_addr = INADDR_LOOPBACK;
	else
		sockAddrIn.sin_addr.s_addr = INADDR_ANY;

	sockAddrIn.sin_port = htons( m_StartData.port );
	sockAddrIn.sin_family = AF_INET;

	//bind to port/ip
	if( bind( m_listensock, (sockaddr*)&sockAddrIn, sizeof(sockAddrIn) ) == SOCKET_ERROR )
	{
		error( "bind() on port %u failed.", m_StartData.port );
		closesocket(m_listensock);
		return;
	}

	//Set to listen mode
	if( listen( m_listensock, 32 ) == SOCKET_ERROR )
	{
		error( "listen() on socket %i failed", m_listensock );
		closesocket(m_listensock);
		return;
	}
}

void CServerMgr::LookForNewClients( void )
{
	for( int i = m_ClientList.size(); i < m_iMaxClients; i++ )
	{
		sockaddr_in clientAdress;
		socklen_t clientAdressSize = (socklen_t)sizeof( clientAdress );
		SOCKET clientSocket = accept( m_listensock, (sockaddr*)&clientAdress, &clientAdressSize );
	
		if( clientSocket == INVALID_SOCKET )
		{
			int iError = GetLastSocketError();
			if( iError != SEWOULDBLOCK )
				error( "accept() failed, error code: %i.", iError );
			return;
		}

		// a new client arrived

		if( !SetSocketNonBlocking( clientSocket ) )
		{
			closesocket( clientSocket );
			continue;
		}

		if( !SetSocketNoDelay( clientSocket ) )
		{
			//edit: this is no criteria for disconnecting the poor client :(
			//closesocket( clientSocket );
			//continue;
		}

		singletons::g_pTimer->UpdateTime();
		float fServerTime = gpGlobals->curtime;
		int rc = send( clientSocket, (char*)&fServerTime, sizeof(float), 0 );
		if( rc != 4 )
		{
			closesocket( clientSocket );
			continue;
		}

		CNetSocket *socket = new CNetSocket(	clientSocket,
												(unsigned int)clientAdress.sin_addr.s_addr,
												singletons::g_pEvtMgr,
												m_iClientIdCounter,
												gpGlobals );
		m_iClientIdCounter++;

		CNetClient *client = new CNetClient( socket );

		client->Init();

		m_ClientList.push_back( client );

		//TODO: Handshake/version checking
	}
}

void CServerMgr::RecieveClientInput( void )
{
	std::vector<CNetClient*>::iterator it = m_ClientList.begin();

	while( it != m_ClientList.end() )
	{
		CNetClient *pClient = *it;
		CNetSocket *pSocket = pClient->GetSocket();
		
		//Recieve loop
		while( pSocket->Recv() )
		{ }

		//Check for disconnect
		if( pSocket->ShouldDisconnect() )
		{
			pClient->HandleDisconnect();
			delete pClient->GetSocket();
			pClient->SetSocket( NULL );
			pClient->UnregisterEvents();
			delete pClient;
			*it = NULL;
			it = m_ClientList.erase( it );
		}
		else
			it++;
	}
}


void CServerMgr::TransmitServerOutput( void )
{
	UINT size = m_ClientList.size();
	for( UINT i = 0; i < size; i++ )
	{
		m_ClientList[i]->GetSocket()->FlushPacket();
		m_ClientList[i]->GetSocket()->Send();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CServerMgr::UpdateClients( void )
{
	for( UINT i = 0; i < m_ClientList.size(); i++ )
		m_ClientList[i]->Update();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CServerMgr::SendNetworkUpdates( void )
{
	UINT dataSize = 0;
	char *data = singletons::g_pEntityList->GatherNetworkUpdate(dataSize);
	if( data != NULL && dataSize > 0 )
	{
		Assert(dataSize <= CNettableUpdate::sMaxDataSize);
		CNettableUpdate *evt = new CNettableUpdate(ev::NETTABLE_UPDATE, data, dataSize, false);
		singletons::g_pEvtMgr->AddEventToQueue(evt);
	}
}
