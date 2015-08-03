// renderer/CRenderInterf.h
// renderer/CRenderInterf.h
// renderer/CRenderInterf.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CRenderInterf_H__
#define deferred__renderer__CRenderInterf_H__

#include <util/IInterfaces.h>
#include "Interfaces.h"
#include <util/threads_e.h>


//Forward declarations of interfaces
//We do this because we dont want do include every interface
class CTextureManager;
class CEventManager;
class CRenderer;
class CCamera;
class CFontManager;
class CShaderManager;
class CGeometryManager;
class CGLStateSaver;
class CMatrixStack;
class IRealtimeDebugInterface;
class CRealtimeDebugInterface;

class DLL CRenderInterfaces : public IRenderInterfaces
{
public:
	CRenderInterfaces();
	virtual ~CRenderInterfaces();

	void Init( void );
	void Cleanup( void );

	DECL_INTERFACE( CTextureManager, TextureManager );
	DECL_INTERFACE( ITextureManager, TextureManagerInterf );
	DECL_INTERFACE( CRenderer, Renderer );
	DECL_INTERFACE( IRenderer, RendererInterf );
	DECL_INTERFACE( CFontManager, FontManager );
	DECL_INTERFACE( IFontManager, FontManagerInterf );
	DECL_INTERFACE( CEventManager, RenderEventMgr );
	DECL_INTERFACE( CCTime, Timer );
	DECL_INTERFACE( CShaderManager, ShaderManager );
	DECL_INTERFACE( CGeometryManager, GeometryManager );
	DECL_INTERFACE( CGLStateSaver, GLStateSaver );
	DECL_INTERFACE( CMatrixStack, MVStack );
	DECL_INTERFACE( CMatrixStack, ProjStack );
	DECL_INTERFACE( IWindow, Window );
	DECL_INTERFACE( CRealtimeDebugInterface, RealtimeDebug );
	DECL_INTERFACE( IRealtimeDebugInterface, RealtimeDebugInterface );

public:
	void AdditionalInit( CEventManager *pRenderEvtMgr, CCTime *pRenderTimer );

	//inline void SetWnd( void* hWnd ) { m_hWnd = hWnd; }
	//inline void* GetWnd( void ) const { return m_hWnd; }

	//inline void SetDevCtx( void* devCtx ) { m_DevCtx = devCtx; }
	//inline void* GetDevCtx( void ) const { return m_DevCtx; }

	//inline void SetRenderCtx( int i, void *renderCtx ) { m_RenderCtx[i] = renderCtx; }
	//inline void* GetRenderCtx( int i ) const { return m_RenderCtx[i]; }

	////inline void SetLocalClientID( const int &id ) { m_iLocalClientID = id; }
	////inline int	GetLocalCliendID( void ) const { return m_iLocalClientID; }

	//inline void SetInstance( void* hInstance ) { m_Instance = hInstance; }
	//inline void* GetInstance( void ) { return m_Instance; }

	bool BindRenderingContext( threads_e thread );
	bool UnBindRenderingContext( threads_e thread );
	void SetWindow(IWindow *window) { m_pWindow = window; }

	static void SetActiveInstance( CRenderInterfaces *pRenderInterfaces );

private:
	//void* m_hWnd;
	//void* m_DevCtx;
	//void* m_Instance;
	//void* m_RenderCtx[2];
	//int m_iLocalClientID;
};

extern DLL CRenderInterfaces *g_RenderInterf;
extern CCTime *g_pTimer;
extern gpGlobals_t *gpGlobals;



#endif

