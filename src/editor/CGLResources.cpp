// editor/CGLResources.cpp
// editor/CGLResources.cpp
// editor/CGLResources.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "CGLResources.h"
#include <renderer/include.h>
#include <renderer/CRenderWorld.h>
#include <renderer/CSkyBox.h>
#include "ChildFrm.h"
#include "renderer.h"
#include "editorDoc.h"

//CGLResources g_GLResources;
CGLResources *GetGLResources(void)
{
	return GetActiveGLResources();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CGLResources::CGLResources()
{
	m_initialized = false;
	m_model = NULL;
	m_timer = NULL;
	m_EvtMgr = NULL;
	m_Globals = NULL;
	m_world = NULL;
	m_pRenderInterfaces = NULL;
	m_OglRenderCtx = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CGLResources::~CGLResources()
{
	//delete m_model;
	delete m_EvtMgr;
	delete m_timer;
	delete m_Globals;
	delete m_pRenderInterfaces;

	wglMakeCurrent( NULL, NULL );
	if( !wglDeleteContext( m_OglRenderCtx ) )
		MessageBox( NULL, _T("wglDeleteContext() failed..."), _T("error"), MB_OK );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGLResources::Init( HDC hDC, HWND hWnd, int resx3d, int resy3d )
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory( &pfd, sizeof(pfd) );
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	// Pixel format.
	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if( SetPixelFormat(hDC, pixelFormat, &pfd) == FALSE )
		MessageBox( NULL, _T("SetPixelFormat() failed..."), _T("error"), MB_OK );

	// Create the OpenGL Rendering Context, but only once
	if( m_OglRenderCtx == NULL )
	{
		m_OglRenderCtx = wglCreateContext(hDC);
		if( m_OglRenderCtx == NULL )
			MessageBox( NULL, _T("wglCreateContext() failed"), _T("error"), MB_OK );
	}

	if( wglMakeCurrent(hDC, m_OglRenderCtx) == FALSE )
		MessageBox( NULL, _T("wglMakeCurrent() failed..." ), _T("error"), MB_OK );


	if( !m_initialized )
	{
		m_pRenderInterfaces = new CRenderInterfaces();
		CRenderInterfaces::SetActiveInstance(m_pRenderInterfaces);

		m_Globals = new gpGlobals_t();
		m_timer = new CCTime( m_Globals );
		m_timer->InitTime();
		m_EvtMgr = new CEventManager( LOCATION_CLIENT );

		GetRenderInterfaces()->Init();
		GetRenderInterfaces()->AdditionalInit( m_EvtMgr, m_timer );

		IRenderer *pRenderer = GetRenderInterfaces()->GetRendererInterf();
		pRenderer->AttachToWindow( hDC, NULL, hWnd, resx3d, resy3d );
		pRenderer->SetLoadingScreenActive( false );

		m_initialized = true;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CRenderWorld *CGLResources::LoadWorldFromFile( const char *filename )
{
	CRenderList *pRenderList = GetRenderInterfaces()->GetRendererInterf()->GetRenderList();
	if(m_world)
	{
		GetRenderInterfaces()->GetRendererInterf()->SetWorld(NULL);
		pRenderList->DeleteStaticEntity(m_world);
	}

	m_world = new CRenderWorld();
	m_world->LoadWorld( filename );
	pRenderList->AddStaticEntity( m_world );
	GetRenderInterfaces()->GetRendererInterf()->SetWorld(m_world);
	return m_world;
}

CGLResources *g_GLResourcesActive = NULL;

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGLResources::SetActive( HDC hdc )
{
	wglMakeCurrent( hdc, m_OglRenderCtx );
	CRenderInterfaces::SetActiveInstance( m_pRenderInterfaces );
	g_GLResourcesActive = this;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGLResources::UpdateAndDraw()
{
	m_timer->UpdateTime();
	if(GetActiveGLResources() == this)
		GetActiveDocument()->UpdateAllViews(NULL);
}

