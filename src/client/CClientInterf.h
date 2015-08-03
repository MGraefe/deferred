// client/CClientInterf.h
// client/CClientInterf.h
// client/CClientInterf.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CClientInterf_H__
#define deferred__client__CClientInterf_H__

#include <util/IInterfaces.h>
#include <renderer/Interfaces.h>
#include <util/threads_e.h>

//Forward declarations of interfaces
//We do this because we dont want do include every interface
class CEventManager;
class CCamera;
class CButtonInputMgr;
class CSoundSystem;
class CNetSocket;
class IWindow;
class CInputSystem;

class CClientInterfaces : public IInterfaces
{
public:
	CClientInterfaces();
	~CClientInterfaces();

	void Init( void );
	void Cleanup( void );

	DECL_INTERFACE( CEventManager, ClientEventMgr )
	DECL_INTERFACE( CButtonInputMgr, ButtonInputMgr );
	DECL_INTERFACE( CSoundSystem, SoundSystem );
	DECL_INTERFACE( CCTime, Timer );
	DECL_INTERFACE( CNetSocket, ClientSocket );
	DECL_INTERFACE( CInputSystem, InputSystem );

public:
	//inline void* GetWnd( void ) const { return m_pRenderInterfaces->GetWnd(); }
	//inline void* GetDevCtx( void ) const { return m_pRenderInterfaces->GetDevCtx(); }
	//inline void* GetRenderCtx( const int &i ) const { return m_pRenderInterfaces->GetRenderCtx(i); }

	inline void SetLocalClientID( int id ) { m_iLocalClientID = id; }
	inline int	GetLocalCliendID( void ) const { return m_iLocalClientID; }

	inline void SetClientSocket( CNetSocket *socket ) { m_pClientSocket = socket; }

	inline bool GetQuitGameState() const { return m_quitGame; }
	void SetQuitGameState(bool quit = true) { m_quitGame = quit; }

	bool BindRenderingContext( threads_e thread );
	bool UnBindRenderingContext( threads_e thread );

	IRenderer *GetRenderer( void ) { return m_pRenderInterfaces->GetRendererInterf(); }
	IFontManager *GetFontManager( void ) { return m_pRenderInterfaces->GetFontManagerInterf(); }
	ITextureManager *GetTextureManager( void ) { return m_pRenderInterfaces->GetTextureManagerInterf(); }
	IWindow *GetWindow() { return m_pRenderInterfaces->GetWindow(); }

private:
	int m_iLocalClientID;
	IRenderInterfaces *m_pRenderInterfaces;
	bool m_quitGame;
};

extern CClientInterfaces *const g_ClientInterf;
extern CCTime *g_pTimer;
extern gpGlobals_t *gpGlobals;

#endif

