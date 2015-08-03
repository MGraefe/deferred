// client/clientmain.cpp
// client/clientmain.cpp
// client/clientmain.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include <util/networking.h>
#include <util/timer.h>
#include <renderer/CSkyBox.h>
#include "clientmain.h"
#include <util/globalvars.h>
#include <renderer/renderer.h>
#include <util/CEventManager.h>
#include "CClientInterf.h"
#include "CButtonInputMgr.h"
#include "server/servermain.h"
#include <renderer/TextureManager.h>
#include "sound/CSoundSystem.h"
#include "CLoaderThread.h"
#include <util/CScriptParser.h>
#include "TimeSyncer.h"
#include <util/ConsoleHandler.h>
#include <util/CConVar.h>
#include <util/CConfigParser.h>
#include <renderer/consoleCallback.h>
#include "server/consoleCallback.h"
#include <renderer/IWindow.h>
#include "CInputSystem.h"
#include "CNettableUpdateHandler.h"
#include <util/IRealtimeDebugInterface.h>
#include "CGuiLanguage.h"
#include "CGuiEventHandler.h"

#ifdef _LINUX
#include <linux/unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#define CLIENT_CONFIG_FILE "config/client.cfg"


//HDC			hDC=NULL;		// Private GDI Device Context
//HGLRC		hRC=NULL;		// Permanent Rendering Context
//HGLRC		hRC2=NULL;
//HWND		hWnd=NULL;		// Holds Our Window Handle
//HINSTANCE	hInstance;		// Holds The Instance Of The Application

//bool	keys[256];			// Array Used For The Keyboard Routine
//bool	lastkeys[256];

CConVar render_vsync_enabled("render.vsync", "0");
CConVar render_resolution_x("render.resolution.x", "1366");
CConVar render_resolution_y("render.resolution.y", "768");
CConVar render_windowed("render.windowed", "0");
CConVar client_language("client.language", "en_US");
CConVar lag_millis("lag.millis", "0");

bool g_bHasLocalServer = false;
bool g_bConnected = false;
volatile int g_bServerShutdown = 0;
volatile int g_bServerIsDown = 0;
volatile int g_bServerIsUp = 0;
CTimeSyncer *g_pTimeSyncer = NULL;

ConsoleHandler *g_pConsoleHandler = NULL;

char g_pchIP[128];

bool g_bSizingEnabled = false;

std::string g_sClientName = "Player";

class CNettableUpdateListener : public IEventListener
{
public:
	virtual bool HandleEvent( const IEvent *evt )
	{
		if( evt->GetType() != ev::NETTABLE_UPDATE  &&
			evt->GetType() != ev::NETTABLE_UPDATE_INTERP )
			return false;
		CNettableUpdateHandler::HandleEvent((CNettableUpdate*)evt);
		return true;
	}

	void RegisterEvents(void)
	{
		g_ClientInterf->GetClientEventMgr()->RegisterListener( ev::NETTABLE_UPDATE, this );
		g_ClientInterf->GetClientEventMgr()->RegisterListener( ev::NETTABLE_UPDATE_INTERP, this );
	}

	virtual void UnregisterEvents( void )
	{
		g_ClientInterf->GetClientEventMgr()->UnregisterListener( ev::NETTABLE_UPDATE, this );
		g_ClientInterf->GetClientEventMgr()->UnregisterListener( ev::NETTABLE_UPDATE_INTERP, this );
	}
};

CNettableUpdateListener *g_pNettableUpdateListener = NULL;


class CPhysicsDebugListener : public IEventListener
{
public:
	virtual bool HandleEvent( const IEvent *evt )
	{
		if( evt->GetType() != ev::START_PHYS_DEBUG )
			return false;
		if( !g_bHasLocalServer ) //Only supported on local servers!
			return false;
		CPointerDataEvent *pevt = (CPointerDataEvent*)evt;
		g_ClientInterf->GetRenderer()->EnablePhysicsDebugger((IDebugRenderer*)pevt->GetPointer());
		return true;
	}

	void RegisterEvents(void)
	{
		g_ClientInterf->GetClientEventMgr()->RegisterListener( ev::START_PHYS_DEBUG, this );
	}

	virtual void UnregisterEvents( void )
	{
		g_ClientInterf->GetClientEventMgr()->UnregisterListener( ev::START_PHYS_DEBUG, this );
	}
};


class CClientIdListener : public IEventListener
{
public:
	virtual bool HandleEvent( const IEvent *evt ) 
	{
		if( evt->GetType() != ev::SET_LOCAL_CLIENT_ID )
			return false;
		g_ClientInterf->SetLocalClientID( ((CIntegerDataEvent*)evt)->GetValue() );
		return true;
	}

	void RegisterEvents( void )
	{
		g_ClientInterf->GetClientEventMgr()->RegisterListener( ev::SET_LOCAL_CLIENT_ID, this );
	}

	virtual void UnregisterEvents( void ) 
	{
		g_ClientInterf->GetClientEventMgr()->UnregisterListener( ev::SET_LOCAL_CLIENT_ID, this );
	}
};


class CPingAnswerListener : public IEventListener
{
public:
	CPingAnswerListener()
	{
		m_iCtr = 0;
		m_fLastPingRequest = 0.0f;
	}

	void RegisterEvents( void )
	{
		g_ClientInterf->GetClientEventMgr()->RegisterListener( ev::PING_ANSWER, this );
	}

	virtual void UnregisterEvents( void )
	{
		g_ClientInterf->GetClientEventMgr()->UnregisterListener( ev::PING_ANSWER, this );
	}

	virtual bool HandleEvent( const IEvent *evt )
	{
		if( evt->GetType() != ev::PING_ANSWER )
			return false;

		CIntegerDataEvent *pEvent = (CIntegerDataEvent*)evt;
		int answerId = pEvent->GetValue();

		std::map<int,float>::iterator it = m_sendTimes.find(answerId);
		if( it == m_sendTimes.end() )
		{
			error( "Error in CPingAnswerListener" );
			return true;
		}
		
		gpGlobals->ping = gpGlobals->curtime - it->second;
		m_sendTimes.erase(it);
		return true;
	}

	void MaybeSendPingRequest( void )
	{
		if( gpGlobals->curtime < m_fLastPingRequest + 0.5f )
			return;

		int pingNr = m_iCtr++;
		CIntegerDataEvent *pEvt = new CIntegerDataEvent(ev::PING_REQUEST, pingNr );
		m_sendTimes[pingNr] = gpGlobals->curtime;
		g_ClientInterf->GetClientEventMgr()->AddEventToQueue(pEvt);
		m_fLastPingRequest = gpGlobals->curtime;
	}

private:
	std::map<int,float> m_sendTimes;
	float m_fLastPingRequest;
	UINT m_iCtr;
};

CPingAnswerListener *g_pPingAnswerListener = NULL;


CNetSocket *g_ClientSocket = NULL;
bool ConnectToServer( const char *pchAdress, USHORT iPort )
{
	if( !InitSocketApi() )
		return false;

	//Try to retrieve proper IP for a given hostname
	addrinfo hints;
	addrinfo *result = NULL;
	memset( &hints, 0, sizeof(addrinfo) );
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	char pchPort[6];
	_itoa( (int)iPort, pchPort, 10 );

	if( getaddrinfo( pchAdress, pchPort, &hints, &result ) != 0 )
		return false;
	
	if( result->ai_family != AF_INET )
	{
		error( "result->ai_family is not AF_INET!" );
		return false;
	}

	sockaddr_in ServerAdress = *((sockaddr_in*)result->ai_addr);

	ServerAdress.sin_port = htons( iPort );
	ServerAdress.sin_family = AF_INET;

	//Initialize Socket
	SOCKET sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( sock == INVALID_SOCKET )
	{
		error( "socket() failed." );
		closesocket(sock);
		return false;
	}


	//Connect to the server
	if( connect( sock, (sockaddr*)&ServerAdress, sizeof(ServerAdress) ) == SOCKET_ERROR )
	{
		error( "Connecting failed:\r\nTimeout, could not reach destination adress." );
		closesocket(sock);
		return false;
	}

	if( !SetSocketNonBlocking( sock ) )
	{
		closesocket(sock);
		return false;
	}

	if( !SetSocketNoDelay( sock ) )
	{
		closesocket(sock);
		return false;
	}

	//Recieve timestamp
	float fTimeStamp;
	int iTimeStampOfs = 0;
	while( iTimeStampOfs < (int)sizeof(float) )
	{
		int rc = recv( sock, ((char*)&fTimeStamp) + iTimeStampOfs, sizeof(float)-iTimeStampOfs, 0 );
		if( rc == 0 )
		{
			closesocket(sock);
			return false;
		}
		else if( rc == SOCKET_ERROR )
		{
			if( GetLastSocketError() != SEWOULDBLOCK )
			{
				closesocket(sock);
				return false;
			}				
		}
		else
			iTimeStampOfs += rc;
	}

	g_pTimer->UpdateTime();
	gpGlobals->serveroffset_calculated = gpGlobals->serveroffset = fTimeStamp - gpGlobals->curtime;

	g_ClientSocket = new CNetSocket(	sock,
										ServerAdress.sin_addr.s_addr,
										g_ClientInterf->GetClientEventMgr(),
										CLIENT_ID_SERVER,
										gpGlobals );

	g_ClientSocket->Init();
	g_ClientInterf->SetClientSocket(g_ClientSocket);

	CRemoteEventListener *pListener = new CRemoteEventListener(g_ClientSocket);
	g_ClientInterf->GetClientEventMgr()->RegisterListener( ev::CLIENT_FINISHED_REGISTER, pListener );

	g_bConnected = true;

	CClientIdListener * clientIdListener = new CClientIdListener();
	g_pPingAnswerListener = new CPingAnswerListener();
	g_pNettableUpdateListener = new CNettableUpdateListener();
	CPhysicsDebugListener *physDebugListener = new CPhysicsDebugListener();

	//Register events
	g_pLoaderThread->RegisterEvents();
	g_ClientInterf->GetRenderer()->RegisterEvents();
	clientIdListener->RegisterEvents();
	physDebugListener->RegisterEvents();
	g_pPingAnswerListener->RegisterEvents();
	g_pNettableUpdateListener->RegisterEvents();
	g_ClientInterf->GetSoundSystem()->InitCollisionSoundEmitter();
	g_ClientInterf->GetClientEventMgr()->RegisterListener( ev::SERVER_SNAPSHOT_START, clientIdListener );
	g_ClientInterf->GetClientEventMgr()->AddEventToQueue( new CStringDataEvent(ev::SET_CLIENT_NAME, g_sClientName.c_str()) );
	g_ClientInterf->GetClientEventMgr()->AddEventToQueue( new CNoDataEvent( ev::CLIENT_FINISHED_REGISTER ) );
	
	if( g_pTimeSyncer )
	{
		g_pTimeSyncer->Stop();
		delete g_pTimeSyncer;
		g_pTimeSyncer = NULL;
	}
	g_pTimeSyncer = new CTimeSyncer(ServerAdress.sin_addr.s_addr, iPort);
	g_pTimeSyncer->Start();

	return true;
}

#ifdef _LINUX
void PostQuitMessage(int exitCode)
{
	//TODO: implement
}
#endif

class CQuitButtonListener : public IEventListener
{
public:
	bool HandleEvent(const IEvent *evt)
	{
		if(evt->GetType() != ev::KEYBOARD_PRESSED)
			return false;
		
		CIntegerDataEvent *ev = (CIntegerDataEvent*)evt;
		if(ev->GetValue() == KeyCodes::KC_ESCAPE)
			PostQuitMessage(0);
		return true;
	}

	void RegisterEvents()
	{
		g_ClientInterf->GetClientEventMgr()->RegisterListener(ev::KEYBOARD_PRESSED, this);
	}

	void UnregisterEvents()
	{
		g_ClientInterf->GetClientEventMgr()->UnregisterListener(ev::KEYBOARD_PRESSED, this);
	}
};

CQuitButtonListener g_quitButtonListener;
bool g_bWndProcDisabled = false;

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
#ifdef _WINDOWS
LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	if(!g_bWndProcDisabled)
	{
		switch (uMsg)					// Check For Windows Messages
		{
			case WM_ACTIVATE:			// Watch For Window Activate Message
			{
				if( g_ClientInterf && g_ClientInterf->GetButtonInputMgr() )
					g_ClientInterf->GetButtonInputMgr()->ResetAllKeys();
				return 0;
			}

			case WM_SYSCOMMAND:				// Intercept System Commands
			{
				switch (wParam)				// Check System Calls
				{
					case SC_SCREENSAVE:		// Screensaver Trying To Start?
					case SC_MONITORPOWER:	// Monitor Trying To Enter Powersave?
					return 0;				// Prevent From Happening
				}
				break;
			}

			case WM_CLOSE:
			{
				PostQuitMessage(0);
				return 0;
			}

			case WM_SIZE:
			{
				int w = LOWORD(lParam);
				int h = HIWORD(lParam);
				if(g_bSizingEnabled && g_ClientInterf && g_ClientInterf->GetRenderer())
					g_ClientInterf->GetRenderer()->ReSizeScene(w, h);
				return 0;
			}
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
#endif


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void KillGLWindow(void)
{
	return g_ClientInterf->GetRenderer()->KillGLWindow();
}
 

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
BOOL CreateGLWindow(const char* title, int width, int height, bool fullscreen)
{
	void *eventCallback = NULL;
#ifdef _WINDOWS
	eventCallback = &WndProc;
#endif
	g_bWndProcDisabled = true; //Disable wndproc briefly to supress events that happen on wnd creation
	if(g_ClientInterf->GetRenderer()->CreateGLWindow(
		title, width, height, fullscreen, eventCallback) == FALSE)
		return FALSE;
	
	g_ClientInterf->GetRenderer()->SetVSync(render_vsync_enabled.GetBool());
	return TRUE;
}


bool RecieveServerInput( void )
{
	if( !g_ClientSocket )
		return false;

	while( g_ClientSocket->Recv() )
	{ }

	if( g_ClientSocket->ShouldDisconnect() )
		return false;

	return true;
}

std::ofstream g_clientTimeDebugFile("log/clienttime.txt");

void ThinkUpdateAndRender( void )
{
	//Handle stored console input
	if( g_pConsoleHandler )
		g_pConsoleHandler->HandleStoredInput();

	if( !g_bConnected && g_bHasLocalServer && g_bServerIsUp )
		ConnectToServer( "127.0.0.1", 21596 );

	//Update Time
	g_pTimer->UpdateTime();
	g_RenderInterf->GetRealtimeDebugInterface()->SetValue("time.client", std::to_string(gpGlobals->curtime).c_str());
	g_clientTimeDebugFile << gpGlobals->curtime << "\t" << gpGlobals->frametime << "\t";

	//Recieve input from server
	if( g_bConnected && !RecieveServerInput() )
		error_fatal( "Lost connection to Server..." );

	//Process Input Events
	g_ClientInterf->GetClientEventMgr()->ProcessEventQueue( gpGlobals->curtime );

	//Draw the Scene
	//if( g_ClientInterf->GetWindow()->IsActive() )
	double renderStart = g_pTimer->GetTime();
	g_ClientInterf->GetRenderer()->Render();
	double renderStop = g_pTimer->GetTime();
	g_clientTimeDebugFile << (renderStop - renderStart) << std::endl;

	//Send ping request
	if( g_pPingAnswerListener )
		g_pPingAnswerListener->MaybeSendPingRequest();
	
	
	//Update Input System
	g_ClientInterf->GetInputSystem()->Update();
	g_ClientInterf->GetButtonInputMgr()->Update();

	//Process Input Events
	g_ClientInterf->GetClientEventMgr()->ProcessEventQueue( gpGlobals->curtime );

	//Update old buttons
	g_ClientInterf->GetButtonInputMgr()->UpdateOldKeys();

	//send things
	if( g_ClientSocket )
	{
		g_ClientSocket->FlushPacket();
		g_ClientSocket->Send();
	}

	//update sounds
	g_ClientInterf->GetSoundSystem()->UpdateTempSounds();

	if(lag_millis.GetInt() > 0)
		CCTime::Sleep((UINT)lag_millis.GetInt());
}


int GetParamIntVal( const char *pCmdLine, const char *pParam, int iDefault )
{
	std::string paramComplete = std::string("-") + pParam + "=";

	const char *pStr = strstr( pCmdLine, paramComplete.c_str() );
	if( !pStr )
		return iDefault;

	pStr += paramComplete.size() * sizeof(char);
	
	int val = atoi( pStr );
	return val;
}

template<size_t SIZE>
bool GetParamCharVal( const char *pCmdLine, const char *pParam, char (&out)[SIZE], const char *pDefault )
{
	std::string paramComplete = std::string("-") + pParam + "=";

	//copy until occurence of space
	const char *pStr = strstr( pCmdLine, paramComplete.c_str() );
	if( !pStr )
	{
		strcpy_s( out, pDefault );
		return false;
	}

	pStr += paramComplete.length() * sizeof( char );

	const char *pSpace = strchr( pStr, ' ' );
	unsigned int length = !pSpace ? strlen(pStr) : pSpace - pStr;
	int quotes = 0;
	if(pStr[0] == '"' && pStr[length-1] == '"')
		quotes = 1;
	strncpy_s( out, pStr + quotes, length - 2*quotes );
	//out[length] = 0; //null-terminate
	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void StartServer( const char *mapname )
{
	g_bHasLocalServer = true;
	serverstartdata_t SrvStartData;
	SrvStartData.type = SERVERTYPE_LAN;
	SrvStartData.tickrate = 66;
	SrvStartData.port = 21596;
	SrvStartData.pbIsDown = &g_bServerIsDown;
	SrvStartData.pbShutdown = &g_bServerShutdown;
	SrvStartData.pbIsUp = &g_bServerIsUp;
	SrvStartData.bThreaded = true;
	SrvStartData.pDebugInterface = (void*)GetRenderInterfaces()->GetRealtimeDebugInterface();
	strcpy_s(SrvStartData.mapname, mapname);
	servermain(SrvStartData);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
DECLARE_CONSOLE_COMMAND(config_parse, "config.parse")
{
	CConfigParser parser;
	if( params.length() < 1 )
		parser.ParseConfigFile(CLIENT_CONFIG_FILE, g_pConsoleHandler);
	else
		parser.ParseConfigFile(params.c_str(), g_pConsoleHandler);
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
int clientMain(const char *cmdLine)
{
	bool done = false;

	//Initialize console system
	InitConsole();
	g_pConsoleHandler = new ConsoleHandler();
	g_pConsoleHandler->AddCallback( &CConVarList::HandleConsoleInput, &CConVarList::GetConVarBase );
	InitConsoleInputCallbackRenderer(g_pConsoleHandler);
	g_pConsoleHandler->Start();
	ConsoleMessage("Console System initialized");

	//Init filesystem
	filesystem::Filesystem &filesys = filesystem::Filesystem::GetInstance();
	filesys.mountArchive("data.rpaf");
	ConsoleMessage("Filesystem initialized");

	{
		char clientName[256];
		GetParamCharVal( cmdLine, "name", clientName, "Player" );
		g_sClientName = clientName;
	}


	//parse config
	CConfigParser configParser;
	configParser.ParseConfigFile(CLIENT_CONFIG_FILE, g_pConsoleHandler);

	//parse language
	gui::CGuiLanguage::GetInst().parseLang(client_language.GetString());

	//Initialize event system
	ev::EventSystemInit();
	CRenderInterfaces *pRenderInterf = new CRenderInterfaces();
	pRenderInterf->Init();
	CRenderInterfaces::SetActiveInstance(pRenderInterf);
	g_ClientInterf->Init();
	pRenderInterf->AdditionalInit( g_ClientInterf->GetClientEventMgr(), g_ClientInterf->GetTimer() );
	ConsoleMessage("Event system initialized");

	g_pLoaderThread = new CLoaderThread();
	ConsoleMessage("Loader Thread initialized");
	
	char mapname[64];
	if( !GetParamCharVal( cmdLine, "map", mapname, "def_coast1" ))
		ConsoleMessage("WARNING: no map specified, falling back to def_coast1");

	//Start a local server when no ip is told.
	if( !GetParamCharVal( cmdLine, "ip", g_pchIP, "127.0.0.1" ) )
	{
		InitConsoleInputCallbackServer(g_pConsoleHandler);
		StartServer(mapname);
		ConsoleMessage("Server started...");
	}

	//initialize time
	g_pTimer->InitTime();
	ConsoleMessage("Time initialized");

	// Create Our OpenGL Window
	int resx = GetParamIntVal( cmdLine, "resx", render_resolution_x.GetInt() );
	int resy = GetParamIntVal( cmdLine, "resy", render_resolution_y.GetInt() );
	bool fullscreen = GetParamIntVal( cmdLine, "windowed", render_windowed.GetInt() ) == 0;
	if (!CreateGLWindow("deferred", resx, resy, fullscreen))
		return 0;
	ConsoleMessage("Window initialized");

	//Initialize sound system
	g_ClientInterf->GetSoundSystem()->Init();

	//Initialize input system
	g_ClientInterf->GetInputSystem()->Init(g_ClientInterf->GetWindow(), g_ClientInterf->GetClientEventMgr());
	g_ClientInterf->GetButtonInputMgr()->Init(g_ClientInterf->GetClientEventMgr());
	g_quitButtonListener.RegisterEvents();

	//Initialize GUI Event handler
	gui::CGuiEventHandler *guiEvtHandler = new gui::CGuiEventHandler();
	guiEvtHandler->Init();

	//Connect to server
	if( !g_bHasLocalServer && !ConnectToServer( g_pchIP, 21596 ) )
	{
		error_fatal( "Could not connect to server..." );
		done = true;
	}

	//Update Time
	g_pTimer->UpdateTime();

	g_bWndProcDisabled = false;
	g_bSizingEnabled = true;

	//main loop
	while(!done && !g_ClientInterf->GetQuitGameState())
	{
		g_RenderInterf->GetWindow()->HandleMessages(done);
		ThinkUpdateAndRender();
	}

	ConsoleMessage("Received QUIT Message, shutting down.");

	guiEvtHandler->UnregisterEvents();
	delete guiEvtHandler;

	// Shutdown
	g_ClientInterf->GetSoundSystem()->SetMasterVolume( 0.0f );

	//delete client socket
	if( g_ClientSocket )
	{
		g_ClientSocket->Disconnect();
		delete g_ClientSocket;
	}

	if( g_pTimeSyncer )
	{
		g_pTimeSyncer->Stop();
		while(g_pTimeSyncer->IsRunning())
		{}
		delete g_pTimeSyncer;
		g_pTimeSyncer = NULL;
	}

	IWindow *window = g_ClientInterf->GetWindow();
	g_ClientInterf->Cleanup();
	g_RenderInterf->Cleanup();
	window->Kill();
	delete window;

	//shutdown server
	if( g_bHasLocalServer )
	{
		ConsoleMessage("Waiting for Server to shutdown...");
		g_bServerShutdown = 1;
		while( !g_bServerIsDown )
		{}
		ConsoleMessage("Server down.");
	}

	ConsoleMessage("Bye Bye");

#ifdef _LINUX
	_exit(0);
#endif

	return 0;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void initClientDll()
{
	//puts("Initializing client shared library");
	InitConsole();
}

//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
#ifdef _WINDOWS
BOOL WINAPI DllMain(
	__in  HINSTANCE hinstDLL,
	__in  DWORD fdwReason,
	__in  LPVOID lpvReserved
	)
{
	initClientDll();
	return TRUE;
}
#endif

//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
#ifdef __GNUC__
void __attribute__ ((constructor)) ClientDllMain(void)
{
	initClientDll();
}
#endif
