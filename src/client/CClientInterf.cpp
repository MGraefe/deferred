// client/CClientInterf.cpp
// client/CClientInterf.cpp
// client/CClientInterf.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "CClientInterf.h"
#include <renderer/TextureManager.h>
#include <util/CEventManager.h>
#include <renderer/renderer.h>
#include <renderer/CCamera.h>
#include "CButtonInputMgr.h"
#include "sound/CSoundSystem.h"
#include <util/error.h>
#include <renderer/CFontManager.h>
#include <renderer/IWindow.h>
#include "CInputSystem.h"

CClientInterfaces g_ClientInterfObj;
CClientInterfaces *const g_ClientInterf = &g_ClientInterfObj;

gpGlobals_t gGlobals_Obj;
gpGlobals_t *gpGlobals = &gGlobals_Obj;

CCTime *g_pTimer = NULL;

CClientInterfaces::CClientInterfaces()
{
	m_iLocalClientID = -1;
	m_quitGame = false;
	NULL_INTERFACE( ClientEventMgr );
	NULL_INTERFACE( ButtonInputMgr );
	NULL_INTERFACE( SoundSystem );
	NULL_INTERFACE( ClientSocket );
	NULL_INTERFACE( InputSystem );
}

CClientInterfaces::~CClientInterfaces()
{
	Cleanup();
}

void CClientInterfaces::Init( void )
{
	m_pRenderInterfaces = GetRenderInterfaces();
	m_pClientEventMgr = new CEventManager( LOCATION_CLIENT );
	g_pTimer = m_pTimer = new CCTime( gpGlobals );
	INIT_INTERFACE( CSoundSystem, SoundSystem );
	INIT_INTERFACE( CButtonInputMgr, ButtonInputMgr );
	INIT_INTERFACE( CInputSystem, InputSystem );
}


void CClientInterfaces::Cleanup( void )
{
	CLEANUP_INTERFACE( ClientEventMgr );
	CLEANUP_INTERFACE( ButtonInputMgr );
	if(GetSoundSystem())
		GetSoundSystem()->Cleanup();
	CLEANUP_INTERFACE( SoundSystem );
	CLEANUP_INTERFACE( InputSystem );
}


bool CClientInterfaces::BindRenderingContext( threads_e thread )
{
	return GetWindow()->BindRenderingContext((UINT)thread);
}


bool CClientInterfaces::UnBindRenderingContext( threads_e thread )
{
	return GetWindow()->UnBindRenderingContext((UINT)thread);
}
