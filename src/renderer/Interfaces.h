// renderer/Interfaces.h
// renderer/Interfaces.h
// renderer/Interfaces.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__Interfaces_H__
#define deferred__renderer__Interfaces_H__

#include <util/IEvent.h>
#include <util/IInterfaces.h>
#include <util/timer.h>

#include <util/dll.h>

class CLightList;
class CRenderList;
class CCamera;
class CSkyBox;
class CSunFlare;
class IDebugRenderer;
class CMatrixStack;
class IWindow;
class CEventManager;
class CRenderWorld;
class IRealtimeDebugInterface;
namespace gui { class CGuiElement; }

class DLL IRenderer : public IEventListener
{
public:
	//virtual ~IRenderer() = 0;
	virtual void Render( void ) = 0;
	virtual void RegisterEvents( void ) = 0;
	virtual int CreateGLWindow( const char *title, int width, int height, bool fullscreen, void *WndProc ) = 0;
#ifdef _WINDOWS
	virtual BOOL AttachToWindow( HDC hDC, HGLRC hlRC, HWND hWnd, int resx, int resy ) = 0;
#endif
	virtual void KillGLWindow(void) = 0;
	virtual void HandleContextRestore(void) = 0;
	virtual CLightList *GetLightList( void ) = 0;
	virtual CRenderList *GetRenderList( void ) = 0;
	virtual void SetPlayerOneCam( CCamera *pCam ) = 0;
	virtual CSkyBox *GetSkyBox( void ) = 0;
	virtual CSunFlare *GetSunFlare( void ) = 0;
	virtual void SetLoadingScreenActive( bool bActive ) = 0;
	//virtual void SetDeviceContext( HDC hDC ) = 0;
	virtual void ReSizeScene( int resx, int resy ) = 0;
	virtual void SetDebugRenderer( IDebugRenderer *pDebugRenderer ) = 0;
	virtual IDebugRenderer *GetDebugRenderer( void ) = 0;
	virtual void EnablePhysicsDebugger( IDebugRenderer *pDebugRenderer ) = 0;
	virtual void SetSeaColorDark(const Vector3f &color) = 0;
	virtual void SetSeaColorBright(const Vector3f &color) = 0;
	virtual void SetWorld( CRenderWorld * pWorld ) = 0;
	virtual CRenderWorld *GetWorld( void ) const = 0;
	virtual gui::CGuiElement *GetGuiRoot() const = 0;

	virtual bool IsVSyncSupported() const = 0;
	virtual void SetVSync(bool enabled) = 0;
	virtual bool IsVSyncEnabled() const = 0;
};


class DLL ITextureManager
{
public:
	virtual ~ITextureManager() {}
	virtual int	LoadMaterial( const char *name ) = 0;
	virtual void BindMaterial( int index ) = 0;
};

class DLL IFontManager
{

};

class DLL IRenderInterfaces : public IInterfaces
{
public:
	virtual ~IRenderInterfaces() {}
	DECL_INTERFACE_GET_ABSTRACT( IRenderer, RendererInterf );
	DECL_INTERFACE_GET_ABSTRACT( ITextureManager, TextureManagerInterf );
	DECL_INTERFACE_GET_ABSTRACT( IFontManager, FontManagerInterf );
	DECL_INTERFACE_GET_ABSTRACT( CMatrixStack, MVStack );
	DECL_INTERFACE_GET_ABSTRACT( CMatrixStack, ProjStack );
	DECL_INTERFACE_GET_ABSTRACT( IWindow, Window );
	DECL_INTERFACE_GET_ABSTRACT( IRealtimeDebugInterface, RealtimeDebugInterface );
	virtual void AdditionalInit( CEventManager *pRenderEvtMgr, CCTime *pRenderTimer ) = 0;

	//virtual void* GetWnd( void ) const = 0;
	//virtual void* GetDevCtx( void ) const = 0;
	//virtual void* GetRenderCtx( int i ) const = 0;
};

//class DLL IRenderEntity
//{
//	public:
//		virtual void RegisterEvents( CEventManager *pMgr ) = 0;
//};

//class DLL IBaseGeometry : public IRenderEntity
//{
//	public:
//};


//class DLL IModel : public IBaseGeometry
//{
//public:
//	virtual void Init( const char *filename, const char *shadername, const Vector3f &AbsOrigin, const Quaternion &AbsAngles ) = 0;
//};
//DLL IModel *IModel_Create( int index );


//class DLL IRenderList
//{
//public:
//	virtual void AddStaticEntity( IRenderEntity* const pEnt ) = 0;
//	virtual IRenderEntity* GetEntityByLogicIndex( int index ) = 0;
//};


//class DLL ILightList
//{
//public:
//	//virtual void AddPointLight( const CPointLight &PointLight ) = 0;
//};


DLL IRenderInterfaces *GetRenderInterfaces( void );

#endif
