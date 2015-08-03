// renderer/renderer.h
// renderer/renderer.h
// renderer/renderer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__renderer_H__
#define deferred__renderer__renderer_H__

#include <util/dll.h>
#include <util/IEvent.h>
#include "glheaders.h"
#include "texture_t.h"
#include <util/maths.h>
#include "CLightList.h"
#include "Interfaces.h"
#include "IDebugRenderer.h"

#define INVALID_TEX 0
#define INVALID_FBO 0

#define RENDER_BLUR_PASSES 1
#define SHADOW_CASCADES 4

//forward declarations
class CShaderSpotLight;
class CShaderSpotLightTex;
class CShaderPointLight;
class CShaderWaterNoDepth;
class CShaderUnlit;
class CShaderBloom;
class CShaderDeferredLighting;
class CShaderShadowMap;
class CShaderPostEffectCompositor;
class CShaderGBufferOut;
class CRenderList;
class CFont;
class CCamera;
class CViewFrustum;
class CBloomEffect;
class CSkyBox;
class CShaderWater;
class CShaderWaterPretty;
class CPostProcessor;
class CShaderAntiAlias;
class CSunFlare;
class COceanRenderer;

namespace gui
{
	class CGuiElement;
}


struct shadowspace_t
{
	Vector3f vMins; //z unused
	Vector3f vMaxs; //z unused
	Vector3f vCamPos;
};


class CRenderer : public IRenderer
{

public:
	CRenderer();
	virtual ~CRenderer();
	int CreateGLWindow( const char *title, int width, int height, bool fullscreen, void *eventCallback );
	void KillGLWindow(void);
	void Render( void );
	void ToggleWater( void );
	int  InitGL( void );
#ifdef _WINDOWS
	BOOL AttachToWindow( HDC hDC, HGLRC hlRC, HWND hWnd, int resx, int resy );
#endif
	void InitFog( bool bEnable );
	void ReSizeScene(int width, int height);
	void PreGameInit( void );
	void PostGameInit( void );
	void RegisterEvents( void );
	void RenderLoadingScreen( void );
	void InitLoadingScreen( void );
	void InitGui( void );
	void SetLoadingScreenActive( bool bActive );
	void DrawFullscreenQuad( unsigned int iTex = INVALID_TEX );
	void RestoreViewport(void);
	void Deactivate();
	void Activate();

	bool IsVSyncSupported() const;
	void SetVSync(bool enabled);
	bool IsVSyncEnabled() const;

	int GetScreenWidth( void ) { return m_iViewportWidth; }
	int GetScreenHeight( void ) { return m_iViewportHeight; }
	float *GetWaterColor( void ) { return m_vfWaterColor; }
	float GetFarClippingDist( void ) { return m_fFarClippingDist; }
	float GetNearClippingDist( void ) { return m_fNearClippingDist; }
	float GetWaterHeight( void ) { return m_fWaterHeight; }
	bool IsWaterEnabled( void ) { return m_bWaterEnabled; }
	const Vector3f &GetLightDir( void );
	CLightList *GetLightList( void ) { return &m_LightList; }
	
	CRenderList *GetRenderList( void ) { return m_pRenderList; }
	CSkyBox *GetSkyBox( void ) { return m_pSkyBox; }
	CSunFlare *GetSunFlare(void) { return m_pSunFlare; }

	void HandleContextRestore( void );

	bool HandleEvent( const IEvent *evt );
	void UnregisterEvents( void ) { }

	CCamera *GetActiveCam( void ) { return m_pActiveCam; }
	void SetActiveCam( CCamera *pCam ) { m_pActiveCam = pCam; }

	CCamera *GetPlayerOneCam( void ) { return m_pPlayerOneCam; }
	void SetPlayerOneCam( CCamera *pCam ) { m_pPlayerOneCam = pCam; }

	CCamera *GetPlayerTwoCam( void ) { return m_pPlayerTwoCam; }
	void SetPlayerTwoCam( CCamera *pCam ) { m_pPlayerTwoCam = pCam; }

	CViewFrustum *GetActiveFrustum( void ) { return m_pActiveFrustum; }
	void SetActiveFrustum( CViewFrustum *pFrustum ) { m_pActiveFrustum = pFrustum; }

	void SetActiveRenderpass( renderpasses_e pass ) { m_RenderParams.renderpass = pass; }
	renderpasses_e GetActiveRenderpass( void ) { return m_RenderParams.renderpass; }

	void SetActiveRenderparams( const CRenderParams &params ) { m_RenderParams = params; }
	const CRenderParams &GetActiveRenderparams(void) { return m_RenderParams; }

	COceanRenderer *GetOceanRenderer() const { return m_pOceanRenderer; }
	void SetSeaColorDark(const Vector3f &color) { m_seaColorDark = color; }
	void SetSeaColorBright(const Vector3f &color) { m_seaColorBright = color; }

	bool IsFullscreen( void ) { return m_bFullscreen; }

	void SetDebugRenderer( IDebugRenderer *pDebugRenderer ) { m_pDebugRenderer = pDebugRenderer; }
	IDebugRenderer *GetDebugRenderer( void ) { return m_pDebugRenderer; }

	void EnablePhysicsDebugger( IDebugRenderer *pDebugRenderer );

	void BindTexture( const unsigned int iTex, const unsigned int iUnit, const unsigned int iUnitAfterwards = UINT_MAX );
	void BindTextureArray( const unsigned int iTex, const unsigned int iUnit, const unsigned int iUnitAfterwards = UINT_MAX );
	
	void SetWorld( CRenderWorld * pWorld ) { m_pWorld = pWorld; }
	CRenderWorld *GetWorld() const { return m_pWorld; }

	gui::CGuiElement *GetGuiRoot() const { return m_pGuiRoot; }

private:
	int  Render3dScene( void );

	void UpdateInterpolation();

	void RenderDebugScene();
	void RenderSceneWireframed();
	void RenderDebugLights();
	void RenderGui( void );
	void RenderSceneGeometry( const CRenderParams &params );
	void RenderDebug( void );
	void RenderWaterReflections( void );
	void RenderWaterQuad( bool renderToBackbuffer );
	void RenderShadows( void );
	void InitRenderTextures( void );
	void InitWaterTextures( void );
	void CreateWaterFrameBuffer( void );
	void CreateFinalSceneFrameBuffer( void );
	void CreateShadowFramebuffer( void );
	void CreateGBuffer( void );
	void InitShaders( void );
	void CalcShadowTexMatrix( int cascade );
	void BlurBloomTexture( void );
	void DrawDeferredLighting( void );
	void RenderLights( void );
	void CalcShadowSpaceVars( const Vector3f &vCamPos, const Matrix3 &mLightSpaceMat,
		CViewFrustum *pFrustum, const int iShadowSpaces, const float *pShadowSpaceDistances, shadowspace_t *pShadowSpaces );
	void RenderShadowMapsDebug( void );
	void PerformAntiAliasing(UINT sceneColorTex, UINT sceneDepthTex, 
		UINT sceneNormalTex, UINT destTex);

	void InitProcs();
	bool GetVSyncSupportInfo();

private:
	bool m_bTwoPlayers;
	bool m_bDebugFrustum;
	bool m_bWaterEnabled;
	float m_fWaterHeight;
	float m_vfWaterColor[4];
	int m_iViewportX;
	int m_iViewportY;
	int m_iViewportWidth;
	int m_iViewportHeight;
	int m_iScreenWidth;
	int m_iScreenHeight;
	float m_fNearClippingDist;
	float m_fFarClippingDist;
	int m_iShadowMapHDSize;
	int m_iShadowMapSDSize;
	bool m_bFullscreen;
	IWindow *m_window;

public:
	Matrix4f *m_ShadowProjMtx;
	Matrix4f *m_ShadowModelvMtx;

private:
	unsigned int m_iGBuffer;
	unsigned int m_iGBufferTex0;
	unsigned int m_iGBufferTex1;
	unsigned int m_iGBufferTex2;

	//unsigned int m_iWaterReflTexture;
	//unsigned int m_iWaterDepthTexture;
	unsigned int m_iWaterFrameBuffer;
	unsigned int m_iWaterSceneTexture;

	unsigned int m_iFinalSceneTexture;
	unsigned int m_iFinalSceneFrameBuffer;
	unsigned int m_iFinalSceneDepthTexture;

	unsigned int m_iShadowFrameBuffer;
	unsigned int m_iShadowMaps;

	//unsigned int m_iNormalMap;

	texture_t m_tWaterNormalMap[29];
	//texture_t m_tWaterHeightMap[29];
	//texture_t m_tWaterTest;
	texture_t m_tLoadingCircle;
	texture_t m_tLogosTexture;
	float m_fLastLoadingCircleTime;
	float m_fLastLoadingCircleRot;
	bool m_bLoadingScreenActive;

	//CShaderWaterNoDepth *m_pWaterShader;
	CShaderUnlit *m_pUnlitShader;
	//CShaderBloom *m_pBloomShader;
	//CShaderPostEffectCompositor *m_pPostEffectCompositorShader;
	CShaderDeferredLighting *m_pShaderDeferredLighting;
	CShaderShadowMap *m_pShaderShadowMap;
	CShaderPointLight *m_pShaderPointLight;
	CShaderSpotLight *m_pShaderSpotLight;
	CShaderSpotLightTex *m_pShaderSpotTexLight;
	CShaderGBufferOut *m_pShaderGBuffer;
	CShaderWater *m_pShaderDeferredWater;
	CShaderWaterPretty *m_pShaderWaterPretty;
	CShaderAntiAlias *m_pShaderAntiAlias;
	CRenderList *m_pRenderList;
	CFont *m_pDebugFont;
	CCamera *m_pActiveCam;
	CCamera *m_pPlayerOneCam;
	CCamera *m_pPlayerTwoCam;
	CViewFrustum *m_pActiveFrustum;
	renderpasses_e m_ActiveRenderpass;
	gui::CGuiElement *m_pGuiRoot;
	//CBloomEffect *m_pBloomEffect;
	CLightList m_LightList;
	CSkyBox *m_pSkyBox;
	CSunFlare *m_pSunFlare;
	COceanRenderer *m_pOceanRenderer;
	Vector3f m_seaColorDark;
	Vector3f m_seaColorBright;
	CPostProcessor *m_pPostProcessor;
	IDebugRenderer *m_pDebugRenderer;
	CRenderWorld *m_pWorld;
	CRenderParams m_RenderParams;
};

#endif
