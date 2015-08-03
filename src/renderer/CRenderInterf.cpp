// renderer/CRenderInterf.cpp
// renderer/CRenderInterf.cpp
// renderer/CRenderInterf.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CRenderInterf.h"
#include <util/CEventManager.h>
#include "TextureManager.h"
#include "renderer.h"
#include <util/error.h>
#include "CFontManager.h"
#include "CShaders.h"
#include "CGeometryManager.h"
#include "CGLStateSaver.h"
#include "CRealtimeDebugInterface.h"

#ifdef _WINDOWS
#include "CWindowsWindow.h"
#else
#include "IWindow.h"
#endif

//CRenderInterfaces g_RenderInterfObj;
CRenderInterfaces *g_RenderInterf = NULL;

gpGlobals_t *gpGlobals;
CCTime *g_pTimer;

CRenderInterfaces::CRenderInterfaces()
{
	NULL_INTERFACE( TextureManager );
	NULL_INTERFACE( Renderer );
	NULL_INTERFACE( RendererInterf );
	NULL_INTERFACE( RenderEventMgr );
	NULL_INTERFACE( Timer );
	NULL_INTERFACE( ShaderManager );
	NULL_INTERFACE( GeometryManager );
	NULL_INTERFACE( GLStateSaver );
	NULL_INTERFACE( MVStack );
	NULL_INTERFACE( ProjStack );
	NULL_INTERFACE( Window );
	NULL_INTERFACE( RealtimeDebugInterface );
}

CRenderInterfaces::~CRenderInterfaces()
{
	Cleanup();
}

void CRenderInterfaces::Init( void )
{
	ev::EventSystemInit();
	INIT_INTERFACE( CMatrixStack, MVStack );
	INIT_INTERFACE( CMatrixStack, ProjStack );
	INIT_INTERFACE( CGLStateSaver, GLStateSaver );
	INIT_INTERFACE( CTextureManager, TextureManager );
	INIT_INTERFACE( CRenderer, Renderer );
	INIT_INTERFACE( CFontManager, FontManager );
	INIT_INTERFACE( CShaderManager, ShaderManager );
	INIT_INTERFACE( CGeometryManager, GeometryManager );
	INIT_INTERFACE( CRealtimeDebugInterface, RealtimeDebug );

	m_pRealtimeDebugInterface = m_pRealtimeDebug;
	m_pRendererInterf = m_pRenderer;
	m_pFontManagerInterf = m_pFontManager;
	m_pTextureManagerInterf = m_pTextureManager;
}


void CRenderInterfaces::Cleanup( void )
{
	CLEANUP_INTERFACE( GeometryManager );
	CLEANUP_INTERFACE( TextureManager );
	CLEANUP_INTERFACE( Renderer );
	CLEANUP_INTERFACE( FontManager );
	CLEANUP_INTERFACE( ShaderManager );
	CLEANUP_INTERFACE( GLStateSaver );
	CLEANUP_INTERFACE( MVStack );
	CLEANUP_INTERFACE( ProjStack );
}


bool CRenderInterfaces::BindRenderingContext( threads_e thread )
{
	return GetWindow()->BindRenderingContext((UINT)thread);
}


bool CRenderInterfaces::UnBindRenderingContext( threads_e thread )
{
	return GetWindow()->UnBindRenderingContext((UINT)thread);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderInterfaces::AdditionalInit( CEventManager *pRenderEvtMgr, CCTime *pRenderTimer )
{
	g_pTimer = m_pTimer = pRenderTimer;
	gpGlobals = pRenderTimer->GetGlobals();
	m_pRenderEventMgr = pRenderEvtMgr;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderInterfaces::SetActiveInstance( CRenderInterfaces *pRenderInterfaces )
{
	if(g_RenderInterf == pRenderInterfaces)
		return;

	if(g_RenderInterf && g_RenderInterf->GetRenderer())
		g_RenderInterf->GetRenderer()->Deactivate();

	g_RenderInterf = pRenderInterfaces;
	g_pTimer = pRenderInterfaces->GetTimer();

	if( g_pTimer )
		gpGlobals = pRenderInterfaces->GetTimer()->GetGlobals();

	if(g_RenderInterf && g_RenderInterf->GetRenderer())
		g_RenderInterf->GetRenderer()->Activate();
}


DLL IRenderInterfaces *GetRenderInterfaces( void )
{
	return g_RenderInterf;
}
