// renderer/renderer.cpp
// renderer/renderer.cpp
// renderer/renderer.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include <util/globalvars.h>
#include "renderer.h"
#include <util/timer.h>
#include "CCamera.h"
#include "CSkyBox.h"
#include "CSunFlare.h"
#include "glheaders.h"
#include "CShaders.h"
#include "CViewFrustum.h"
#include <util/error.h>
#include "TextureManager.h"
#include "CRenderInterf.h"
#include "CRenderList.h"
#include "CRenderWorld.h"
#include "CRenderInterf.h"
#include <util/CEventManager.h>
#include <util/debug.h>
#include "CGeometryManager.h"
#include "CFont.h"
#include "CLoadingMutex.h"
#include "CGuiBuilder.h"
#include "CFontManager.h"
#include "CBloomEffect.h"
#include "CMatrixStack.h"
#include <util/CConVar.h>
#include "CPostProcessor.h"
#include "CPhysDebugDrawer.h"
#include "CWindowCreateInfo.h"
#include "COceanRenderer.h"

#ifdef _WINDOWS
	#include "CWindowsWindow.h"
	#include "CWindowsWindowAttached.h"
#else
	#include "CLinuxXWindow.h"
#endif /* _WINDOWS */


CConVar render_lights_point_debug( "render.lights.point.debug", "0", "Renders spheres around the lights." );
CConVar render_lights_spot_debug( "render.lights.spot.debug", "0", "Renders cones around the lights." );

CConVar render_shadows_mapsize( "render.shadows.mapsize", "1536", "Size of the Shadow-Map Textures, needs restart" );
float g_fShadowSpaceDists[SHADOW_CASCADES] = {54.9f, 126.0f, 293.8f, 1000.0f};


#ifdef _WINDOWS
const char *(__stdcall *wglGetExtensionsStringARB)(HDC) = NULL;
#endif

int (__stdcall *wglSwapIntervalEXT)(int) = NULL;
int (__stdcall *wglGetSwapIntervalEXT)(void) = NULL;


CRenderer::CRenderer()
{
	m_pDebugRenderer = NULL;

	m_ShadowProjMtx = new Matrix4[SHADOW_CASCADES];
	m_ShadowModelvMtx = new Matrix4[SHADOW_CASCADES];

	m_bTwoPlayers = false;
	m_pPlayerOneCam = NULL;
	m_pPlayerTwoCam = NULL;
	m_pDebugFont = NULL;
	m_bDebugFrustum = false;
	m_fWaterHeight = -50.0f;
	m_bWaterEnabled = true;
	m_iViewportWidth = 0;
	m_iViewportHeight = 0;
	m_window = NULL;
	m_vfWaterColor[0] = 0.05f;
	m_vfWaterColor[1] = 0.3f;
	m_vfWaterColor[2] = 0.05f;
	m_vfWaterColor[3] = 0.1f;
	m_fFarClippingDist = 4000.0f;
	m_fNearClippingDist = 1.0f;
	m_iShadowMapHDSize = render_shadows_mapsize.GetInt();
	m_iShadowMapSDSize = 2048;

	//Angle3d aLightDir = Angle3d( -45.0f, 0.0f, 0.0f );
	//AngleToVector( aLightDir, m_vLightDir );
	//m_vLightDir.Normalize();

	//Angle3d LightDirDebug;
	//VectorToAngle( m_vLightDir, LightDirDebug );

	m_iFinalSceneTexture = INVALID_TEX;
	m_iFinalSceneFrameBuffer = INVALID_FBO;
	m_iFinalSceneDepthTexture = INVALID_TEX;

	m_iWaterFrameBuffer = INVALID_FBO;
	m_iWaterSceneTexture = INVALID_TEX;

	m_iShadowFrameBuffer = INVALID_FBO;
	m_iShadowMaps = INVALID_TEX;

	m_iGBuffer = INVALID_FBO;
	m_iGBufferTex0 = INVALID_TEX;
	m_iGBufferTex1 = INVALID_TEX;
	m_iGBufferTex2 = INVALID_TEX;

	m_pRenderList = new CRenderList;

	m_tLoadingCircle.index = INVALID_TEX;
	m_tLogosTexture.index = INVALID_TEX;
	m_fLastLoadingCircleTime = 0.0f;
	m_fLastLoadingCircleRot = 0.0f;
	m_bLoadingScreenActive = true;

	m_pGuiRoot = NULL;
	m_pPostProcessor = NULL;
	m_pOceanRenderer = NULL;
	m_pSkyBox = new CSkyBox();
	m_pSunFlare = new CSunFlare();

	m_seaColorDark = m_seaColorBright = Vector3f(17.0f, 11.0f, 11.0f) / 255.0f;

	m_pWorld = NULL;
}

CRenderer::~CRenderer()
{
	glDeleteFramebuffers( 1, &m_iGBuffer );
	glDeleteTextures( 1, &m_iGBufferTex0 );
	glDeleteTextures( 1, &m_iGBufferTex1 );
	glDeleteTextures( 1, &m_iGBufferTex2 );


	glDeleteFramebuffers( 1, &m_iFinalSceneFrameBuffer );
	glDeleteTextures( 1, &m_iFinalSceneDepthTexture );
	glDeleteTextures( 1, &m_iFinalSceneTexture );

	glDeleteFramebuffers( 1, &m_iShadowFrameBuffer );
	glDeleteTextures( 1, &m_iShadowMaps );

	delete m_pRenderList;
	delete m_pSkyBox;
	delete m_pSunFlare;

	if( m_pGuiRoot )
		delete m_pGuiRoot;

	if( m_pDebugFont )
		delete m_pDebugFont;

	delete[] m_ShadowProjMtx;
	delete[] m_ShadowModelvMtx;

	if( m_pOceanRenderer )
		delete m_pOceanRenderer;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CRenderer::CreateGLWindow( const char* title, int width, int height, bool fullscreen, void *eventCallback )
{
	IWindow *window = NULL;
#ifdef _WINDOWS
	window = new CWindowsWindow();
#else
	window = new CLinuxXWindow();
#endif

	CWindowCreateInfo info;
	info.eventCallback = eventCallback;
	info.fullscreen = fullscreen;
	info.title = title;
	info.width = width;
	info.height = height;

	if(!window->Create(&info))
	{
		delete window;
		return FALSE;
	}

	m_bFullscreen = info.fullscreen;
	m_window = window;
	g_RenderInterf->SetWindow(window);

	if(!g_RenderInterf->BindRenderingContext( MAIN_THREAD ) ) // Try To Activate The Rendering Context
	{
		window->Kill();
		delete window;
		return FALSE;
	}

	GLenum err = glewInit();
	InitProcs();
	if( err != GLEW_OK )
	{
		//problem: glewInit failed, something is seriously wrong
		error("glewInit() Failed!");
		return FALSE;
	}

	ReSizeScene(width, height);	// Set Up Our Perspective GL Screen

	if (!InitGL()) // Initialize Our Newly Created GL Window
	{
		window->Kill();
		delete window;
		g_RenderInterf->SetWindow(NULL);
		error("Initialization Failed.");
		return FALSE;
	}

	PreGameInit();
	g_RenderInterf->GetTextureManager()->Init();
	PostGameInit();

	return TRUE;									// Success
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
#ifdef _WINDOWS
BOOL CRenderer::AttachToWindow( HDC hDC, HGLRC hlRC, HWND hWnd, int resx, int resy )
{
	CWindowsWindowAttached *window = new CWindowsWindowAttached();
	if(!window->Attach(hDC, hlRC, hWnd))
	{
		delete window;
		return FALSE;
	}

	m_window = window;
	g_RenderInterf->SetWindow(window);

	glewInit();
	InitProcs();

	ReSizeScene( resx, resy );
	InitGL();
	PreGameInit();
	g_RenderInterf->GetTextureManager()->Init();
	PostGameInit();

	return TRUE;
}
#endif

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::KillGLWindow(void)						// Properly Kill The Window
{
	if( m_window )
	{
		m_window->Kill();
		delete m_window;
		m_window = NULL;
	}
}

const Vector3f g_DefaultLightDir(0.0f, -1.0f, 0.0f);
//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const Vector3f &CRenderer::GetLightDir(void)
{
	if(GetLightList()->GetNumSunLights() > 0)
		return GetLightList()->GetSunLightByArrayIndex(0)->GetDirection();
	else
		return g_DefaultLightDir;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRenderer::HandleEvent( const IEvent *evt )
{
	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::ToggleWater( void )
{
	m_bWaterEnabled = !m_bWaterEnabled;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::RenderWaterReflections( void )
{
	//SetActiveRenderpass( CRenderer::RENDERPASS_WATER_REFL );

	////bind water framebuffer
	//glBindFramebuffer( GL_FRAMEBUFFER, m_iWaterFrameBuffer );
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	////enable the water clipping plane
	//CViewFrustum *pFrustum = GetActiveFrustum();
	//pFrustum->SetAdditionalPlane( Vector3f( 0.0f, GetWaterHeight(), 0.0f ), Vector3f(0.0f,1.0f,0.0f) );
	//pFrustum->EnableAdditionalPlane();

	//SetActiveRenderpass( CRenderer::RENDERPASS_WATER_REFL );

	//glFrontFace( GL_CW );
 //   glPushMatrix();
	//glTranslatef( 0.0f, m_fWaterHeight*2.0f, 0.0f );
	//glScalef(1.0, -1.0, 1.0);

	//GetSkyBox()->DrawSky( m_vLightDir, true );

	//double plane[4] = {0.0, 1.0, 0.0, (double)-m_fWaterHeight};
	//GetGLStateSaver()->Enable(GL_CLIP_PLANE0);
	//glClipPlane(GL_CLIP_PLANE0, plane);

	////Render our scene geometry
	//RenderSceneGeometry();

	//GetGLStateSaver()->Disable(GL_CLIP_PLANE0);
 //   glPopMatrix();
	//glFrontFace( GL_CCW );

	//pFrustum->DisableAdditionalPlane();

	////unbind the framebuffer
	//glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}


CConVar render_water_patch_size("render.water.patchsize", "150");

//---------------------------------------------------------------
// Purpose: Renders a large Quad at a given height
//---------------------------------------------------------------
//void CRenderer::RenderWaterQuad( bool renderToBackbuffer )
//{
//	GetGLStateSaver()->Enable( GL_DEPTH_TEST );
//	glDepthMask( GL_FALSE );
//
//	//copy the final scene texture
//	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, renderToBackbuffer ? 0 : m_iWaterFrameBuffer );
//	glBindFramebuffer( GL_READ_FRAMEBUFFER, m_iFinalSceneFrameBuffer );
//	glBlitFramebuffer(0, 0, m_iScreenWidth, m_iScreenHeight,
//		0, 0, m_iScreenWidth, m_iScreenHeight, 
//		GL_COLOR_BUFFER_BIT, GL_NEAREST );
//
//	glBindFramebuffer( GL_FRAMEBUFFER, renderToBackbuffer ? 0 : m_iFinalSceneFrameBuffer );
//
//	//Enable water shader
//	m_pShaderDeferredWater->Enable();
//	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );	// Type Of Blending To Use
//
//	//24 FPS, 29 Pictures
//	//const int iNormIndex = ((int)(gpGlobals->curtime*24.0f)) % 29;
//	//const int iNormIndex = 0;
//
//	GetMVStack().PushMatrix();
//	Vector3f vCamPos = GetActiveCam()->GetAbsPos();
//	GetMVStack().Translate( vCamPos.x, 0.0f, vCamPos.z );
//	//Water plane is moving with the player, but we need to conserve texture coordinates.
//	const float fSize = 3000.0f; //3 km Draw Size
//	const float fPatchFactors[] = {1.0f / 100.0f, 1.0f/100.0f}; //50 cm per Patch
//	float speeds[] = {5.0f, 0.0f};
//	float rotations[] = {45.0f, 45.0f+62.1254f};
//	Matrix2 rotMats[2] = {Matrix2(DEG2RAD(rotations[0])), Matrix2(DEG2RAD(rotations[1])) };
//	Vector2f vMoveSpeeds[] = {	rotMats[0] * Vector2f(1,0) * gpGlobals->curtime * fPatchFactors[0] * speeds[0], 
//		rotMats[1] * Vector2f(1,0) * gpGlobals->curtime * fPatchFactors[1] * speeds[1] };
//	Vector2f vTexCoords[4];
//	for( int i = 0; i < 2; i++ )
//	{
//		vTexCoords[2*i+0].x = ( vCamPos.x*0.5f ) * fPatchFactors[i];
//		vTexCoords[2*i+0].y = ( -vCamPos.z*0.5f ) * fPatchFactors[i];
//		vTexCoords[2*i+1].x = ( fSize + vCamPos.x*0.5f ) * fPatchFactors[i];
//		vTexCoords[2*i+1].y = ( fSize - vCamPos.z*0.5f ) * fPatchFactors[i];
//	}
//
//	Vector2f vUV[8] = {	Vector2f(vTexCoords[0].x, vTexCoords[1].y),
//		Vector2f(vTexCoords[0].x, vTexCoords[0].y),
//		Vector2f(vTexCoords[1].x, vTexCoords[0].y),
//		Vector2f(vTexCoords[1].x, vTexCoords[1].y),
//		Vector2f(vTexCoords[2].x, vTexCoords[3].y),
//		Vector2f(vTexCoords[2].x, vTexCoords[2].y),
//		Vector2f(vTexCoords[3].x, vTexCoords[2].y),
//		Vector2f(vTexCoords[3].x, vTexCoords[3].y) };
//
//	for( int i = 0; i < 8; i++ )
//		vUV[i] = rotMats[i/4] * (vUV[i] + vMoveSpeeds[i/4]);
//
//	//position								//normal				//tangent			//uv			//skyVisibility
//	float verts[] = {	-fSize, m_fWaterHeight, -fSize,		vUV[4].x, vUV[4].y, 0.0f,	1.0f, 0.0f, 0.0f,	vUV[0].x, vUV[0].y,		0.5f,
//		-fSize, m_fWaterHeight,  fSize,		vUV[5].x, vUV[5].y, 0.0f,	1.0f, 0.0f, 0.0f,	vUV[1].x, vUV[1].y,		0.5f,
//		fSize, m_fWaterHeight,  fSize,		vUV[6].x, vUV[6].y, 0.0f,	1.0f, 0.0f, 0.0f,	vUV[2].x, vUV[2].y,		0.5f,
//		fSize, m_fWaterHeight, -fSize,		vUV[7].x, vUV[7].y, 0.0f,	1.0f, 0.0f, 0.0f,	vUV[3].x, vUV[3].y,		0.5f, };
//
//	BindTexture( m_tWaterNormalMap[0].index, 0 );
//	BindTexture( m_tWaterNormalMap[1].index, 1 );
//	BindTexture( renderToBackbuffer ? m_iFinalSceneTexture : m_iWaterSceneTexture, 2 );
//	BindTexture( m_iFinalSceneDepthTexture, 3, 4 );
//	glBindTexture( GL_TEXTURE_CUBE_MAP, GetSkyBox()->GetCubeMapTexture() );
//	GetGLStateSaver()->ActiveTexture( GL_TEXTURE0 );
//
//	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
//	GetShaderManager()->InitVertexbufferVars( VBF_RENDERVERT, verts );
//	GetShaderManager()->GetActiveShader()->UpdateMatrices();
//	glDrawArrays( GL_QUADS, 0, 4 );
//	rglPopClientAttrib();
//	GetMVStack().PopMatrix();
//
//	glDepthMask( GL_TRUE );
//}


DECLARE_CONSOLE_COMMAND(render_water_reinit_cmd, "render.water.reinit")
{
	g_RenderInterf->GetRenderer()->GetOceanRenderer()->ReInit();
}


//---------------------------------------------------------------
// Purpose: Renders a large Quad at a given height
//---------------------------------------------------------------
void CRenderer::RenderWaterQuad( bool renderToBackbuffer )
{
	GetGLStateSaver()->Enable( GL_DEPTH_TEST );
	glDepthMask( GL_TRUE );

	//copy the final scene texture
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, renderToBackbuffer ? 0 : m_iWaterFrameBuffer );
	glBindFramebuffer( GL_READ_FRAMEBUFFER, m_iFinalSceneFrameBuffer );
	glBlitFramebuffer(0, 0, m_iScreenWidth, m_iScreenHeight,
		0, 0, m_iScreenWidth, m_iScreenHeight, 
		GL_COLOR_BUFFER_BIT, GL_NEAREST );

	glBindFramebuffer( GL_FRAMEBUFFER, renderToBackbuffer ? 0 : m_iFinalSceneFrameBuffer );

	m_pShaderWaterPretty->Enable();
	m_pShaderWaterPretty->SetCamPos(GetActiveCam()->GetAbsPos());
	m_pShaderWaterPretty->SetSeaColorBright(m_seaColorBright);
	m_pShaderWaterPretty->SetSeaColorDark(m_seaColorDark);
	CSunLight *light = m_LightList.GetNumSunLights() > 0 ? m_LightList.GetSunLightByArrayIndex(0) : NULL;
	Vector3f sunDir = light ? -light->GetDirection() : Vector3f(0.0f, 1.0f, 0.0f);
	m_pShaderWaterPretty->SetSunDir(sunDir.GetNormalized());
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );	// Type Of Blending To Use

	GetGLStateSaver()->ActiveTexture( GL_TEXTURE0 );
	m_pOceanRenderer->UploadAndBindTexture();
	BindTexture( renderToBackbuffer ? m_iFinalSceneTexture : m_iWaterSceneTexture, 1 );
	BindTexture( m_iFinalSceneDepthTexture, 2, 3 );
	glBindTexture( GL_TEXTURE_CUBE_MAP, GetSkyBox()->GetCubeMapTexture() );
	GetGLStateSaver()->ActiveTexture( GL_TEXTURE0 );

	m_pOceanRenderer->SetWaterHeight(GetWaterHeight());
	m_pOceanRenderer->Render(m_pShaderWaterPretty, GetActiveCam()->GetFrustum(), GetActiveCam()->GetAbsPos());

	glDepthMask( GL_TRUE );
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::PerformAntiAliasing(UINT sceneColorTex, UINT sceneDepthTex,
									UINT sceneNormalTex, UINT destTex)
{
	if(destTex > 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_iFinalSceneFrameBuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTex, 0);
	}
	else
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GetGLStateSaver()->Disable(GL_DEPTH_TEST);
	GetGLStateSaver()->Disable(GL_BLEND);
	m_pShaderAntiAlias->Enable();

	BindTexture(sceneNormalTex, 2);
	BindTexture(sceneDepthTex, 1);
	BindTexture(sceneColorTex, 0);

	draw_fullscreen_quad();

	if(destTex > 0)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_iFinalSceneTexture, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::RenderSceneGeometry( const CRenderParams &params )
{
	//update light position.
	//UpdateLightDir();

	m_pRenderList->RenderStatics(params);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CConVar render_shadows_debug_size( "render.shadows.debug.size", "150" );
void CRenderer::RenderShadowMapsDebug( void )
{
	CShaderShadowMapVisualize *shd = 
		(CShaderShadowMapVisualize*)GetShaderManager()->GetShader("ShadowMapVisualize");
	shd->Enable();
	BindTextureArray(m_iShadowMaps, 0);

	Vector2f start(10, m_iScreenHeight - 10);
	Vector2f spacing(10, 0);
	float size = render_shadows_debug_size.GetFloat();
	
	for( int i = 0; i < SHADOW_CASCADES; i++ )
	{
		float x = start.x + i*(size + spacing.x);
		float y = start.y;
		shd->SetSlice( (float)i );
		draw_quad_textured(x, y, x+size, y-size, 0.0f);
	}

	//Also draw frustum-slice as lines ontop
	m_pUnlitShader->Enable();
	m_pUnlitShader->EnableTexturing(false);
	m_pUnlitShader->SetDrawColor(1.0f, 0.0f, 0.0f, 1.0f);

	for( int i = 0; i < SHADOW_CASCADES; i++ )
	{
		CViewFrustum *frustum = GetActiveFrustum();
		Vector3f camPos = GetActiveCam()->GetAbsPos();
		const Vector3f *farVerts = frustum->GetFarPlaneVerts();
		Vector3f verts[8];
		for( int k = 0; k < 4; k++ )
			verts[k] = (i == 0) ? (camPos) : (camPos + farVerts[k] * (g_fShadowSpaceDists[i-1] / GetFarClippingDist())); 
		for( int k = 4; k < 8; k++ )
			verts[k] = camPos + farVerts[k-4] * (g_fShadowSpaceDists[i] / GetFarClippingDist());

		Vector3f vertsProj[8];
		//Transform into screen space
		for( int k = 0; k < 8; k++ )
		{
			Vector4f eye = m_ShadowProjMtx[i] * m_ShadowModelvMtx[i] * Vector4f(verts[k], 1.0f);
			Vector3f ndc = Vector3f(eye.x/eye.w, eye.y/eye.w, eye.z/eye.w);
			float sh = ((float)(m_iShadowMapHDSize))/2.0f;
			Vector3f wnd = Vector3f(sh*ndc.x + sh, sh*ndc.y + sh, 0.0f);
			float factor = size / (float)m_iShadowMapHDSize;
			vertsProj[k] = Vector3f(wnd.x*factor, -wnd.y*factor, 0.0f);
			vertsProj[k] += Vector3f(start.x, start.y, 0.0f) + Vector3f(size+spacing.x, 0.0f, 0.0f)*(float)i;
		}

		Vector3f l[24];
		for( int i = 0; i < 4; i++ )
		{
			l[i*6+0] = vertsProj[i];   l[i*6+1] = vertsProj[(i+1)<4?(i+1):0];
			l[i*6+2] = vertsProj[i+4]; l[i*6+3] = vertsProj[(i+5)<8?(i+5):4];
			l[i*6+4] = vertsProj[i];   l[i*6+5] = vertsProj[i+4];
		}
		draw_lines(l, 24);
	}

	m_pUnlitShader->EnableTexturing(true);
	m_pUnlitShader->SetDrawColor(1.0f, 1.0f, 1.0f, 1.0f);
}


CConVar render_shadows_debug( "render.shadows.debug", "0" );
CConVar render_draw_gui( "render.gui.draw", "1" );
//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::RenderGui( void )
{
	if( !m_pGuiRoot )
		return;

	if( !render_draw_gui.GetBool() )
		return;

	//glClear( GL_DEPTH_BUFFER_BIT );

	//make orthographic view
	float fHeight = (float)GetScreenHeight();
	float fWidth = (float)GetScreenWidth();

	GetProjStack().SetOrthographic(0.0, fWidth, fHeight, 0.0, -100.0, 100.0);
	GetMVStack().SetIdentity();
	GetMVStack().Translate( 0.0f, 0.0f, 1.0f );

	//disable depth test for the gui, we do z sorting
	GetGLStateSaver()->Disable( GL_DEPTH_TEST );
	GetGLStateSaver()->Disable( GL_CULL_FACE );

	if( render_shadows_debug.GetBool() )
		RenderShadowMapsDebug();

	//Enable the unlit shader
	m_pUnlitShader->Enable();
	m_pUnlitShader->SetDrawColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_pUnlitShader->EnableTexturing( true );

	GetGLStateSaver()->Enable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	//our gui root handles rendering of children
	m_pGuiRoot->Render();

	//Revert state changes
	GetGLStateSaver()->Enable( GL_DEPTH_TEST );
	GetGLStateSaver()->Enable( GL_CULL_FACE );

	//m_pUnlitShader->EnableTexturing( true );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::InitGui( void )
{
	m_pGuiRoot = new gui::CGuiElement();
	m_pGuiRoot->SetInitPos(vec3_null);
	m_pGuiRoot->SetInitSize((float)GetScreenWidth(), (float)GetScreenHeight());
	m_pGuiRoot->SetAnchors(gui::Anchors(gui::ANCHOR_LEFT, gui::ANCHOR_TOP, gui::ANCHOR_RIGHT, gui::ANCHOR_BOTTOM));

	g_RenderInterf->GetFontManager()->SetScaleFactor( 1.0f );
	g_RenderInterf->GetFontManager()->LoadFonts();

	gui::CGuiBuilder guiBuilder;
	guiBuilder.BuildGui( "scripts/hud.rps", m_pGuiRoot );
}


//---------------------------------------------------------------
// Purpose: Binds a texture to a given tex-unit
//---------------------------------------------------------------
void CRenderer::BindTexture( const unsigned int iTex, const unsigned int iUnit, const unsigned int iUnitAfterwards )
{
	GetGLStateSaver()->ActiveTexture( GL_TEXTURE0 + iUnit );
	GetGLStateSaver()->Enable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, iTex );

	if( iUnitAfterwards != UINT_MAX )
		GetGLStateSaver()->ActiveTexture( GL_TEXTURE0 + iUnitAfterwards );
}

//---------------------------------------------------------------
// Purpose: Binds a texture to a given tex-unit
//---------------------------------------------------------------
void CRenderer::BindTextureArray( const unsigned int iTex, const unsigned int iUnit, const unsigned int iUnitAfterwards )
{
	GetGLStateSaver()->ActiveTexture( GL_TEXTURE0 + iUnit );
	GetGLStateSaver()->Enable( GL_TEXTURE_2D_ARRAY );
	glBindTexture( GL_TEXTURE_2D_ARRAY, iTex );

	if( iUnitAfterwards != UINT_MAX )
		GetGLStateSaver()->ActiveTexture( GL_TEXTURE0 + iUnitAfterwards );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::RenderDebug( void )
{
	//bool bDebugFont = true;

	//if( g_bDebugPhysics || m_bDebugFrustum || true )
	//{
	//	m_pUnlitShader->Enable();
	//	m_pUnlitShader->EnableTexturing( false );
	//}

	//if( g_bDebugPhysics )
	//{
	//	//g_pPhysics->VRenderDiagnostics();
	//}

	////if( m_bDebugFrustum )
	////	g_ClientInterf->GetCamera()->GetFrustum()->DebugRender();

	//Debug::g_pRenderLineList->Render();
	//Debug::g_pRenderLineList->Clear();

	//if( g_bDebugPhysics || m_bDebugFrustum || true )
	//	m_pUnlitShader->EnableTexturing( true );

	//if( g_bDebugTextPerformance )
	//{
	//	m_pDebugFont->BindForRendering();
	//	for( int i = 0; i < 20; i++ )
	//	{
	//		Vector3f vPos(-100.0f, 25.0f + (float)i*19.0f, 200.0f);
	//		glTranslatef( vPos.x, vPos.y, vPos.z );
	//		m_pDebugFont->RenderString( "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789" );
	//		glTranslatef( -vPos.x, -vPos.y, -vPos.z );
	//	}
	//	m_pDebugFont->UnBindFromRendering();
	//}
}

CConVar render_postprocessing_enabled("render.postprocessing.enabled", "1");
CConVar render_antialiasing_enabled("render.antialiasing.enabled", "0");
CConVar render_shadows_render("render.shadows.render", "1");
CConVar render_wireframed("render.wireframed", "0");
CConVar render_water_draw( "render.water.draw", "1" );
CConVar render_sunflare_draw( "render.sunflare.draw", "1" );
//---------------------------------------------------------------
// Purpose: Render the 3d scene
//---------------------------------------------------------------
int CRenderer::Render3dScene( void )
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if( !GetActiveCam() )
	{
		CCTime::Sleep(10);
		return TRUE;
	}

	GetActiveCam()->Activate( 75.0f, GetNearClippingDist(), GetFarClippingDist(), (float)GetScreenWidth()/(float)GetScreenHeight() );
	GetMVStack().SetIdentity();

	//Render the Shadow Map
	if( render_shadows_render.GetBool() )
		RenderShadows();

	//Update Camera Matrices
	GetActiveCam()->UpdateMatrices();

	//Bind G-Buffer
	glBindFramebuffer( GL_FRAMEBUFFER, m_iGBuffer );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	GetGLStateSaver()->Disable( GL_BLEND );

	//Render scene without translucent things
	SetActiveRenderpass( RENDERPASS_NORMAL );
	m_pShaderGBuffer->Enable();
	RenderSceneGeometry(GetActiveRenderparams());

	RenderDebugLights();

	//Render translucent things
	SetActiveRenderpass( RENDERPASS_NORMAL_TRANSLUCENT );
	RenderSceneGeometry(GetActiveRenderparams());

	//Bind Final Scene FB and perform deferred lighting
	glBindFramebuffer( GL_FRAMEBUFFER, m_iFinalSceneFrameBuffer );
	glDepthMask( GL_FALSE );
	glClear( GL_COLOR_BUFFER_BIT );
	DrawDeferredLighting();

	//Draw Sky
	GetSkyBox()->DrawSky( GetLightDir() );

	//Draw SunFlare
	if( render_sunflare_draw.GetBool() )
	{
		GetSunFlare()->ResolveOcclusion();
		GetSunFlare()->Render();
		GetSunFlare()->QueryOcclusion();
	}
	
	if( render_wireframed.GetInt() > 0 )
		RenderSceneWireframed();


	//Draw Water
	if( render_water_draw.GetBool() )
		RenderWaterQuad(!render_postprocessing_enabled.GetBool() &&
			!render_antialiasing_enabled.GetBool() );

	//Perform post-processing
	if( render_postprocessing_enabled.GetBool() )
	{
		GetGLStateSaver()->Disable( GL_DEPTH_TEST );
		m_pPostProcessor->ApplyPostProcessing(
			m_iFinalSceneTexture,
			m_iFinalSceneDepthTexture,
			m_iGBufferTex0,
			render_antialiasing_enabled.GetBool() ? m_iGBufferTex0 : 0);
	}


	//Perform anti-aliasing
	if( render_antialiasing_enabled.GetBool() )
	{
		PerformAntiAliasing(
			render_postprocessing_enabled.GetBool() ? m_iGBufferTex0 : m_iFinalSceneTexture,
			m_iFinalSceneDepthTexture,
			m_iGBufferTex1,
			0);
	}


	//Draw additional debug if debug renderer is there
	if( m_pDebugRenderer )
		RenderDebugScene();


	glDepthMask(GL_TRUE);
	glClear( GL_DEPTH_BUFFER_BIT );

	return TRUE; // Everything Went OK
}

CConVar render_unlit("render.unlit", "0");

//---------------------------------------------------------------
// Purpose: Enables the final-compositing shader, binds all
//			needed textures and draws a fullscreen quad
//---------------------------------------------------------------
void CRenderer::DrawDeferredLighting( void )
{	

	//enable additive blending
	GetGLStateSaver()->Enable( GL_BLEND );
	glBlendFunc( GL_ONE, GL_ONE );

	//Render unlit
	if( render_unlit.GetBool() )
	{
		m_pUnlitShader->Enable();
		BindTexture( m_iGBufferTex0, 0 );
		glDisable(GL_DEPTH_TEST);
		DrawFullscreenQuad();
		glEnable(GL_DEPTH_TEST);
	}
	else //render lights
	{
		BindTexture( m_iGBufferTex0, 0 );
		BindTexture( m_iGBufferTex1, 1 );
		BindTexture( m_iFinalSceneDepthTexture, 2 );
		BindTextureArray( m_iShadowMaps, 3 );
		RenderLights();
	}

	GetGLStateSaver()->ActiveTexture( GL_TEXTURE0 ); //Reset Active Texture
}

CConVar render_lights_point_draw( "render.lights.point.draw", "1" );
CConVar render_lights_sun_draw( "render.lights.sun.draw", "1" );
CConVar render_lights_spot_draw( "render.lights.spot.draw", "1" );

void CRenderer::RenderLights( void )
{
	//glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_POLYGON_BIT );
	glDepthMask( GL_FALSE );

	Matrix3 mViewSpace( GetActiveCam()->GetAbsAngles().GetMatrix3() );
	Matrix4f mViewSpace4x4( GetActiveCam()->GetViewSpaceMat() );
	Matrix4f mViewSpace4x4Inverse( mViewSpace4x4.GetInverse() );

	//SUN LIGHT:
	if( render_lights_sun_draw.GetBool() && GetLightList()->GetNumSunLights() > 0 )
	{
		CSunLight *light = GetLightList()->GetSunLightByArrayIndex(0);
		if( light->IsEnabled() )
		{
			// Moving from unit cube [-1,1] to [0,1]
			const float dOfsHD = 0.5 - 0.0000;
			Matrix4f mUnitCube(	
				0.5f, 0.0f, 0.0f, dOfsHD, 
				0.0f, 0.5f, 0.0f, dOfsHD,
				0.0f, 0.0f, 0.5f, dOfsHD,
				0.0f, 0.0f, 0.0f, 1.0f );

			Matrix4f mShadowSpaces[SHADOW_CASCADES];
			for( int i = 0; i < SHADOW_CASCADES; i++ )
				mShadowSpaces[i] = mUnitCube * m_ShadowProjMtx[i] * m_ShadowModelvMtx[i] * mViewSpace4x4Inverse;

			m_pShaderDeferredLighting->Enable();
			m_pShaderDeferredLighting->UpdateLightingData( *light, mViewSpace, mViewSpace4x4, mShadowSpaces, SHADOW_CASCADES );

			glDisable( GL_DEPTH_TEST );
			draw_fullscreen_quad_no_tex();
			glEnable( GL_DEPTH_TEST );
		}
	}

	//POINT LIGHTS:
	if( render_lights_point_draw.GetBool() )
	{
		m_pShaderPointLight->Enable();
		for( int i = 0; i < m_LightList.GetNumPointLights(); i++ )
		{
			CPointLight &light = *m_LightList.GetPointLightByArrayIndex(i);
			if(light.IsEnabled())
			{
				m_pShaderPointLight->SetLightData( light, mViewSpace, GetActiveCam()->GetAbsPos() );
				light.Render();
			}
		}
	}
	
	//SPOT LIGHTS:
	if( render_lights_spot_draw.GetBool() )
	{
		m_pShaderSpotLight->Enable();
		GetGLStateSaver()->CullFace( GL_BACK );
		glDisable( GL_DEPTH_TEST );
		for( int i = 0; i < m_LightList.GetNumSpotLights(); i++ )
		{
			CSpotLight &light = *m_LightList.GetSpotLightByArrayIndex(i);
			if(light.IsEnabled())
			{
				m_pShaderSpotLight->SetLightData( light, mViewSpace, GetActiveCam()->GetAbsPos() );
				light.Render();
			}
		}
		glEnable( GL_DEPTH_TEST );
	}

	//SPOT TEX LIGHTS
	//if( render_lights_texspot_draw.GetBool() )
	//{
		m_pShaderSpotTexLight->Enable();
		GetGLStateSaver()->CullFace( GL_BACK );
		glDisable( GL_DEPTH_TEST );
		for( int i = 0; i < m_LightList.GetNumSpotTexLights(); i++ )
		{
			CSpotTexLight &light = *m_LightList.GetSpotTexLightByArrayIndex(i);
			//if(light.IsEnabled())
			//{
				BindTexture(light.GetTexIndex(), m_pShaderSpotTexLight->GetLightTexIndex());
				m_pShaderSpotTexLight->SetLightData(light, mViewSpace, GetActiveCam()->GetAbsPos() );
				light.Render();
			//}
		}
		glEnable(GL_DEPTH_TEST);
	//}

	GetGLStateSaver()->ActiveTexture(0);
	glDepthMask( GL_TRUE );
	GetGLStateSaver()->DepthFunc( GL_LESS );
	GetGLStateSaver()->CullFace( GL_BACK );
}


void stabilizeVector( Vector3f &vec, const Vector3f &step )
{
	vec.x = RoundToNearestMultiple( vec.x, step.x );
	vec.y = RoundToNearestMultiple( vec.y, step.y );
	vec.z = RoundToNearestMultiple( vec.z, step.z );
}


void GetLightSpaceMatrix( Matrix3 &mat, const Vector3f &vLightDir )
{
	Vector3f vUp(0.0f,1.0f,0.0f);
	if( vLightDir.y < -0.99f ) //light straigt down?
		vUp = Vector3f(1.0f,1.0f,0.0f);

	Vector3f vBasisForward = -vLightDir; //light dir is normalized
	Vector3f vBasisRight = VectorCross( vUp, vBasisForward ).Normalize();
	Vector3f vBasisUp = VectorCross( vBasisForward, vBasisRight ).Normalize();
	mat[0][0] = vBasisRight.x;		mat[0][1] = vBasisRight.y;		mat[0][2] = vBasisRight.z;
	mat[1][0] = vBasisUp.x;			mat[1][1] = vBasisUp.y;			mat[1][2] = vBasisUp.z;
	mat[2][0] = vBasisForward.x;	mat[2][1] = vBasisForward.y;	mat[2][2] = vBasisForward.z;

	//Vector3f test = mat * vLightDir;
}

CConVar render_shadows_quadraticfitting("render.shadows.quadraticfitting", "0");
CConVar render_shadows_stabilize("render.shadows.stabilize", "1");
void CRenderer::CalcShadowSpaceVars( const Vector3f &vCamPos, const Matrix3 &mLightSpaceMat, CViewFrustum *pFrustum,
	const int iShadowSpaces, const float *pShadowSpaceDistances, shadowspace_t *pShadowSpaces )
{
	//float fShadowMapSizeReziproc = 1.0f/fShadowMapSize;

	for( int i = 0; i < iShadowSpaces; i++ )
	{
		//Get Frustum Vertexes in light space
		shadowspace_t &shsp = pShadowSpaces[i];

		//near-plane
		Vector3f vFrustumVerts[8];
		if( i == 0 ) //first level has only one starting point instead of a plane
			vFrustumVerts[0] = vFrustumVerts[1] = vFrustumVerts[2] = vFrustumVerts[3] = vec3_null;
		else
		{
			for( int k = 0; k < 4; k++ )
			{
				vFrustumVerts[k] = pFrustum->GetFarPlaneVerts()[k];
				vFrustumVerts[k] *= pShadowSpaceDistances[i-1] / GetFarClippingDist();
				vFrustumVerts[k] = mLightSpaceMat * vFrustumVerts[k];
			}
		}

		//far-plane
		for( int k = 0; k < 4; k++ )
		{
			vFrustumVerts[k+4] = pFrustum->GetFarPlaneVerts()[k];
			vFrustumVerts[k+4] *= pShadowSpaceDistances[i] / GetFarClippingDist();
			vFrustumVerts[k+4] = mLightSpaceMat * vFrustumVerts[k+4];
		}

		//ok we got frustum vertexes in light space.
		//calculate mins and maxs
		shsp.vMins = shsp.vMaxs = vFrustumVerts[0];
		for( int k = 1; k < 8; k++ )
		{
			shsp.vMins.x = min( vFrustumVerts[k].x, shsp.vMins.x );
			shsp.vMins.y = min( vFrustumVerts[k].y, shsp.vMins.y );
			shsp.vMaxs.x = max( vFrustumVerts[k].x, shsp.vMaxs.x );
			shsp.vMaxs.y = max( vFrustumVerts[k].y, shsp.vMaxs.y );
		}

		shsp.vCamPos = mLightSpaceMat * vCamPos;
		shsp.vMins += shsp.vCamPos;
		shsp.vMaxs += shsp.vCamPos;

#if 0
		if( render_shadows_quadraticfitting.GetBool() )
		{
			//Now that we got mins and maxs we can calculate a quadratic bounding box
			Vector3f dist = shsp.vMaxs - shsp.vMins;
			int modifyIndex = dist.x < dist.y ? 0 : 1;
			if( shsp.vMins.distToSq(shsp.vCamPos) < 
				shsp.vMaxs.distToSq(shsp.vCamPos) )
			{   
				//mins is closer to the cam
				shsp.vMaxs[modifyIndex] = 
					shsp.vMins[modifyIndex]+dist[(modifyIndex+1)%2];
			}
			else
			{
				//maxs is closer to the cam
				shsp.vMins[modifyIndex] = 
					shsp.vMaxs[modifyIndex]-dist[(modifyIndex+1)%2];
			}
		}
		
		//First stabilization method
		if( render_shadows_stabilize.GetInt() == 1 )
		{
			//Calculate fixed steps to stabilize shadows on world.
			Vector3f projectionSize = shsp.vMaxs - shsp.vMins;
			//float size2 = max( abs(projectionSize.x), abs(projectionSize.y) );
			float size = pShadowSpaceDistances[i];
			size = sqrt(size*size*3.0f) * 2.0f * TAN(75.0f/2.0f);
			//float size = pShadowSpaceDistances[i];
			shsp.vMaxs.x = shsp.vMins.x + size * sign(projectionSize.x);
			shsp.vMaxs.y = shsp.vMins.y + size * sign(projectionSize.y);
			//shsp.vMins.x -= size * 0.5f * sign(projectionSize.x);
			//shsp.vMaxs.x += size * 0.5f * sign(projectionSize.x);
			//shsp.vMins.y -= size * 0.5f * sign(projectionSize.y);
			//shsp.vMaxs.y += size * 0.5f * sign(projectionSize.y);


			Vector3f worldUnitsPerTexel;
			worldUnitsPerTexel.x = worldUnitsPerTexel.y = worldUnitsPerTexel.z = size / (float)m_iShadowMapHDSize;

			stabilizeVector( shsp.vMins, worldUnitsPerTexel );
			stabilizeVector( shsp.vMaxs, worldUnitsPerTexel );
		}
#endif

		//second stabilization method
		if( render_shadows_stabilize.GetInt() == 2 )
		{
			//Calculate worst case size
			float worstCase1 = vFrustumVerts[0].distTo(vFrustumVerts[6]);
			float worstCase2 = vFrustumVerts[4].distTo(vFrustumVerts[6]);
			float worstCaseSize = max(worstCase1, worstCase2);

			//Calculate center of projected near/far plane
			Vector3f v0 = (vFrustumVerts[0]+vFrustumVerts[1]+vFrustumVerts[2]+vFrustumVerts[3])*0.25f;
			Vector3f v1 = (vFrustumVerts[4]+vFrustumVerts[5]+vFrustumVerts[6]+vFrustumVerts[7])*0.25f;

			//Select in which direction we expand
			if( v0.x <= v1.x && v0.y <= v1.y ) //Bottom left
			{ 
				shsp.vMaxs.x = shsp.vMins.x + worstCaseSize;
				shsp.vMaxs.y = shsp.vMins.y + worstCaseSize;
			}
			else if( v0.x <= v1.x && v0.y >= v1.y ) //Top Left
			{
				shsp.vMaxs.x = shsp.vMins.x + worstCaseSize;
				shsp.vMins.y = shsp.vMaxs.y - worstCaseSize;
			}
			else if( v0.x >= v1.x && v0.y >= v1.y ) //Top Right
			{
				shsp.vMins.x = shsp.vMaxs.x - worstCaseSize;
				shsp.vMins.y = shsp.vMaxs.y - worstCaseSize;
			}
			else //Bottom left
			{
				shsp.vMins.x = shsp.vMaxs.x - worstCaseSize;
				shsp.vMaxs.y = shsp.vMins.y + worstCaseSize;
			}

			Vector3f worldUnitsPerTexel;
			worldUnitsPerTexel.x = worldUnitsPerTexel.y = worldUnitsPerTexel.z = worstCaseSize / (float)m_iShadowMapHDSize;
			stabilizeVector( shsp.vMins, worldUnitsPerTexel );
			stabilizeVector( shsp.vMaxs, worldUnitsPerTexel );
		}
	}
}


CConVar render_shadows_offset("render.shadows.offset", "3.0");
//---------------------------------------------------------------
// Purpose: Render the Shadows
//---------------------------------------------------------------
void CRenderer::RenderShadows( void )
{
	SetActiveRenderpass( RENDERPASS_SHADOW );

	//temporary disable frustum culling
	bool bFrustumCullOld = g_bNoFrustumCull;
	//g_bNoFrustumCull = true;

	//do not write colors
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	//save old frustum
	CViewFrustum *pOldFrustum = GetActiveFrustum();

	GetMVStack().SetIdentity();
	Vector3f vCamPos = GetActiveCam()->GetAbsPos();
	
	shadowspace_t pShadowSpaces[SHADOW_CASCADES];
	Matrix3 mLightSpaceMat;
	Matrix3 mLightSpaceMatTrans;
	GetLightSpaceMatrix( mLightSpaceMat, GetLightDir() );
	mLightSpaceMatTrans = mLightSpaceMat.GetTranspose();
	CalcShadowSpaceVars( vCamPos, mLightSpaceMat, GetActiveCam()->GetFrustum(), SHADOW_CASCADES, g_fShadowSpaceDists, pShadowSpaces );

	//GetGLStateSaver()->CullFace( GL_FRONT ); //Render Back-Faces only
	GetGLStateSaver()->CullFace( GL_BACK );
	//glDisable( GL_CULL_FACE );
	glBindFramebuffer( GL_FRAMEBUFFER, m_iShadowFrameBuffer );
	glEnable(GL_POLYGON_OFFSET_FILL);
	for( int i = 0; i < SHADOW_CASCADES; i++ )
	{
		if( pShadowSpaces[i].vMins.x == pShadowSpaces[i].vMaxs.x ||
			pShadowSpaces[i].vMins.y == pShadowSpaces[i].vMaxs.y )
			continue;

		const float fNear = -2000.0f;
		const float fFar = 2000.0f;
		float texelWidth = abs(pShadowSpaces[i].vMaxs.x - pShadowSpaces[i].vMins.x) / (float)m_iShadowMapHDSize;
		//float depthRange = fFar - fNear;
		glPolygonOffset(1.0f, texelWidth * render_shadows_offset.GetFloat());
		

		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_iShadowMaps, 0, i);

		glViewport( 0, 0, m_iShadowMapHDSize, m_iShadowMapHDSize );
		glClear( GL_DEPTH_BUFFER_BIT );

		Vector3f camTranslation = GetLightDir().GetNormalized() * Vector3f(0,0,vCamPos.z).Dot(GetLightDir().GetNormalized());

		//calculate the new frustum

		CViewFrustum ShadowFrustum;
		ShadowFrustum.CalcOrthogonalFrustumValues( fNear, fFar,
			pShadowSpaces[i].vMins.x,
			pShadowSpaces[i].vMaxs.x,
			pShadowSpaces[i].vMins.y,
			pShadowSpaces[i].vMaxs.y, 
			mLightSpaceMatTrans,
			camTranslation );
		SetActiveFrustum( &ShadowFrustum );

		//make perspective
		GetProjStack().SetOrthographic( 
			pShadowSpaces[i].vMins.x,
			pShadowSpaces[i].vMaxs.x, 
			pShadowSpaces[i].vMins.y, 
			pShadowSpaces[i].vMaxs.y, 
			fNear, fFar );

		float oglmat[16];
		mLightSpaceMat.ToOpenGlMatrixTranspose( oglmat );
		GetMVStack().SetMatrix(oglmat);
		GetMVStack().Translate( -camTranslation );
		
		CalcShadowTexMatrix(i);

		m_pShaderShadowMap->Enable();
		m_pShaderShadowMap->UpdateMatrices();

		//Draw our geometry
		RenderSceneGeometry(GetActiveRenderparams());
	}

	//Restore old values
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	glViewport( 0, 0, m_iViewportWidth, m_iViewportHeight );
	GetProjStack().SetPerspective( 75.0f, (float)GetScreenWidth()/(float)GetScreenHeight(), GetNearClippingDist(), GetFarClippingDist() );
	GetMVStack().SetIdentity();
	glPolygonOffset(0.0f, 0.0f);
	glDisable(GL_POLYGON_OFFSET_FILL);

	//glEnable(GL_CULL_FACE);
	GetGLStateSaver()->CullFace( GL_BACK );
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	//glPolygonOffset( 0.0f, 0.0f );

	SetActiveFrustum( pOldFrustum );
	g_bNoFrustumCull = bFrustumCullOld;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CRenderer::CalcShadowTexMatrix( int cascade )
{
	// Grab modelview and transformation matrices
	m_ShadowModelvMtx[cascade].Set( GetMVStack().GetMatrix() );
	m_ShadowProjMtx[cascade].Set( GetProjStack().GetMatrix() );
}


//---------------------------------------------------------------
// Purpose: Draw a simple Fullscreen-Quad,
//---------------------------------------------------------------
void CRenderer::DrawFullscreenQuad( unsigned int iTex )
{
	if( iTex != INVALID_TEX )
		glBindTexture( GL_TEXTURE_2D, iTex );

	GetMVStack().PushMatrix();
		GetMVStack().SetIdentity();
		GetProjStack().PushMatrix();
			GetProjStack().SetIdentity();
			draw_quad_textured( -1.0f, -1.0f, 1.0f, 1.0f, -1.0f );
		GetProjStack().PopMatrix();
	GetMVStack().PopMatrix();
}


//---------------------------------------------------------------
// Purpose: All Setup For OpenGL Goes Here
//---------------------------------------------------------------
int CRenderer::InitGL( void )										
{
	//Clear all previously saved states (if any):
	GetGLStateSaver()->ClearStates();

	g_RenderInterf->GetTextureManager()->Init();

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	//glClearColor(195.0f/255.0f, 211.0f/255.0f, 215.0f/255.0f, 1.0f );	// Light Blue Background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0);
	glEnableClientState(GL_VERTEX_ARRAY);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);									// Depth Buffer Setup
	GetGLStateSaver()->Enable(GL_DEPTH_TEST);							// Enables Depth Testing
	GetGLStateSaver()->DepthFunc(GL_LESS);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	GetGLStateSaver()->Enable( GL_BLEND );									// Enable Blending
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );	// Type Of Blending To Use
	GetGLStateSaver()->Disable( GL_NORMALIZE );

	glFrontFace( GL_CCW ); //Culling counter-clockwise
	GetGLStateSaver()->Enable( GL_CULL_FACE );

	//InitFog( true );

	GetGLStateSaver()->Enable(GL_LIGHTING);
	GetGLStateSaver()->Enable(GL_LIGHT0);

	GetShaderManager()->LoadAllShaders();
	InitShaders();

	m_pDebugFont = new CFont();
	m_pDebugFont->LoadFont( "misc/comic.ttf", 16 );

	InitGui();

	//m_pBloomEffect->Init();

	return TRUE;										// Initialization Went OK
}


//---------------------------------------------------------------
// Purpose: Called when our current opengl context gets destroyed
//			(e.g. by a fullscreen toggle)
//			Basically reloads everything that lives inside the Graphics Card.
//---------------------------------------------------------------
void CRenderer::HandleContextRestore( void )
{
	//Reload shaders
	GetShaderManager()->LoadAllShaders();
	InitShaders();

	//Reload all Textures
	g_RenderInterf->GetTextureManager()->ReloadAllTextures();

	//Reload Skybox
	GetSkyBox()->OnRestore();

	//Models are reloaded inside VOnRestore when m_pRenderList->RestoreEntities() is called.
	GetGeometryManager()->DeleteAllModels();
	
	//Call VOnRestore on all renderable entities
	m_pRenderList->RestoreEntities();

	//reload water textures
	InitWaterTextures();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::InitFog( bool bEnable )
{
	if( bEnable )
	{
		//float fogColor[] = {0.3333f, 0.6941f, 0.9922f, 1.0f};
		float fogColor[] = { 144.0f/255.0f, 177.0f/255.0f, 200.0f/255.0f, 1.0f };
		//float fogColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glFogi( GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH );
		glFogfv( GL_FOG_COLOR, fogColor );		// Set Fog Color
		glFogf( GL_FOG_DENSITY, 1.0f/5000.0f );// How Dense Will The Fog Be
		glHint( GL_FOG_HINT, GL_DONT_CARE );	// Fog Hint Value
		glFogf( GL_FOG_START, 1.0f );			// Fog Start Depth
		glFogf( GL_FOG_END, 100.0f );			// Fog End Depth
		GetGLStateSaver()->Enable( GL_FOG );			// Enables GL_FOG
	}
	else
	{
		glFogi( GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH );
		glFogf( GL_FOG_DENSITY, 0.0 );			// How Dense Will The Fog Be
		GetGLStateSaver()->Disable( GL_FOG );					// Enables GL_FOG
	}
}


#define RELOAD_TEX(tex) if( (tex) != INVALID_TEX ){ glDeleteTextures(1, &(tex));} glGenTextures(1, &(tex));
void ReloadTex(unsigned int &tex)
{
	if(tex != INVALID_TEX)
		glDeleteTextures(1, &tex);
	glGenTextures(1, &tex);
}

//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CRenderer::InitRenderTextures( void )
{
	//Delete old Buffer-Textures if already used and generate new ones
	//RELOAD_TEX( m_iWaterReflTexture );
	//RELOAD_TEX( m_iWaterDepthTexture );
	ReloadTex( m_iFinalSceneTexture );
	ReloadTex( m_iFinalSceneDepthTexture );
	ReloadTex( m_iGBufferTex0 );
	ReloadTex( m_iGBufferTex1 );
	ReloadTex( m_iGBufferTex2 );
	ReloadTex( m_iShadowMaps );
	ReloadTex( m_iWaterSceneTexture );

	Vector4f vBorderColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glBindTexture( GL_TEXTURE_2D_ARRAY, m_iShadowMaps );
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, (float*)&vBorderColor );
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f); //disable anisotropic filtering
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT16, m_iShadowMapHDSize, m_iShadowMapHDSize,
		SHADOW_CASCADES, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	

	//Create GBuffer textures
	//Mixin up different formats should be valid with dx10 cards,
	//but it is dangerous and we should have a fallback procedure to RGBA16F on all texs.
	glBindTexture(GL_TEXTURE_2D, m_iGBufferTex0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap generation included in OpenGL v1.4
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f); //disable anisotropic filtering
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, m_iViewportWidth, m_iViewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, m_iGBufferTex1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE); // automatic mipmap generation included in OpenGL v1.4
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f); //disable anisotropic filtering
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_iViewportWidth, m_iViewportHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//Create Final Scene Color Texture
	glBindTexture( GL_TEXTURE_2D, m_iFinalSceneTexture );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f); //disable anisotropic filtering
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, m_iViewportWidth, m_iViewportHeight, 0, GL_RGBA, GL_FLOAT, 0);

	//Create Final Scene Depth Texture
	glBindTexture(GL_TEXTURE_2D, m_iFinalSceneDepthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f); //disable anisotropic filtering
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_iViewportWidth, m_iViewportHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	//Create water texture
	glBindTexture(GL_TEXTURE_2D, m_iWaterSceneTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f); //disable anisotropic filtering
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_iViewportWidth, m_iViewportHeight, 0, GL_RGBA, GL_FLOAT, 0);

}


//---------------------------------------------------------------
// Purpose: Called after vital Game-Components where initialized
//---------------------------------------------------------------
void CRenderer::InitWaterTextures( void )
{
	//Load the animated Water-Normal-Map
	//for( int i = 0; i < 29; i++ )
	//{
	//	char name[128] = "textures/water/water_normal/water_normal_";
	//	char num[3] = "00";
	//	if( i > 9 )
	//		_itoa( i, num, 10 );
	//	else
	//		_itoa( i, &num[1], 10 );
	//	strcat( name, num );
	//	strcat( name, "_00_00.tga" );
	//	g_RenderInterf->GetTextureManager()->LoadTexture( name, m_tWaterNormalMap[i] );
	//}
	g_RenderInterf->GetTextureManager()->LoadTexture( "textures/water/water1.dds", m_tWaterNormalMap[0] );
	g_RenderInterf->GetTextureManager()->LoadTexture( "textures/water/water2.dds", m_tWaterNormalMap[1] );

	//Load the animated Water-height-Map
	//for( int i = 0; i < 29; i++ )
	//{
	//	char name[128] = "textures/water/water_height/water_height_";
	//	char num[3] = "00";
	//	if( i > 9 )
	//		_itoa( i, num, 10 );
	//	else
	//		_itoa( i, &num[1], 10 );
	//	strcat( name, num );
	//	strcat( name, "_00_00.tga" );
	//	g_RenderInterf->GetTextureManager()->LoadTexture( name, m_tWaterHeightMap[i] );
	//}
	
	//g_RenderInterf->GetTextureManager()->LoadTexture( "textures/water_normal.tga", m_tWaterTest );

	//texture_t m_tNormalMap;
	//g_RenderInterf->GetTextureManager()->LoadTexture( "textures/test_normal.tga", m_tNormalMap );
	//m_iNormalMap = m_tNormalMap.index;

	//Init debug
	texture_t m_tWhiteTex;
	g_RenderInterf->GetTextureManager()->LoadTexture( "textures/white.tga", m_tWhiteTex );
	Debug::InitDebug( m_tWhiteTex.index );
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CRenderer::RenderLoadingScreen( void )
{
	//g_LoadingMutex->SetOrWait();
	m_pUnlitShader->Enable();
	m_pUnlitShader->EnableTexturing( true );

	if( m_tLoadingCircle.index != INVALID_TEX )
		glBindTexture( GL_TEXTURE_2D, m_tLoadingCircle.index );

	m_fLastLoadingCircleRot += 45.0f;
	float sh = (float)GetScreenHeight();
	float sw = (float)GetScreenWidth();

	float aspect =  sw / sh;
	float scale = 0.05f;


	GetMVStack().PushMatrix();
	GetProjStack().PushMatrix();
	GetMVStack().SetIdentity();
	GetProjStack().SetIdentity();
	GetProjStack().Scale( scale, scale * aspect, scale );
	GetProjStack().Rotate( m_fLastLoadingCircleRot, 0.0f, 0.0f, 1.0f );
	draw_quad_textured( -1.0f, -1.0f, 1.0f, 1.0f, -1.0f );
	
	//Render logos
#if 1
	if(m_tLogosTexture.index != INVALID_TEX)
	{
		float tw = (float)m_tLogosTexture.width;
		float th = (float)m_tLogosTexture.height;
		GetProjStack().SetIdentity();
		float logoX = (tw + 0.0f) / sw;
		float logoY = (sh - (th + 0.0f)) / sh * 2.0f - 1.0f;
		glBindTexture( GL_TEXTURE_2D, m_tLogosTexture.index );
		draw_quad_textured(-logoX, -1.0f, logoX, -logoY, 0.0f);
	}
#else
	DrawFullscreenQuad(m_tLogosTexture.index);
#endif
	GetProjStack().PopMatrix();
	GetMVStack().PopMatrix();
	//g_LoadingMutex->Release();

	if( g_LoadingFinishedEvent->Probe() )
	{
		//g_pLoaderThread->Terminate();
		m_bLoadingScreenActive = false;
		g_LoadingFinishedEvent->Release();
	}
	else
	{
		CCTime::Sleep( 100 );
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::InitLoadingScreen( void )
{
	g_RenderInterf->GetTextureManager()->LoadTexture( "textures/loadingcircle.dds", m_tLoadingCircle );
	g_RenderInterf->GetTextureManager()->LoadTexture( "textures/logos.tga", m_tLogosTexture );
	if(m_tLogosTexture.index != INVALID_TEX)
	{
		glBindTexture(GL_TEXTURE_2D, m_tLogosTexture.index);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
}


//---------------------------------------------------------------
// Purpose: Called after vital Game-Components where initialized
//---------------------------------------------------------------
void CRenderer::PostGameInit( void )
{
	InitWaterTextures();
	InitShaders();
	InitLoadingScreen();
	m_LightList.Init();

	m_pPostProcessor = new CPostProcessor();
	m_pPostProcessor->Init();

	m_pOceanRenderer = new COceanRenderer();
	m_pOceanRenderer->Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::RegisterEvents( void )
{
	m_pRenderList->RegisterEvents( g_RenderInterf->GetRenderEventMgr() );
	g_RenderInterf->GetRenderEventMgr()->RegisterListener( ev::WORLD_CREATED, this );
	//g_RenderInterf->GetRenderEventMgr()->RegisterListener( ev::SET_LOCAL_CLIENT_ID, this );
	//g_ClientInterf->GetClientEventMgr()->RegisterListener( ev::CAMERA_CREATE, this );
}

//---------------------------------------------------------------
// Purpose: Called before vital Game-Components where initialized
//---------------------------------------------------------------
void CRenderer::PreGameInit( void )
{

}


//---------------------------------------------------------------
// Purpose: Shaders are actually initialized by InitGame, so
//			we just load pointers to the already loaded Shaders
//---------------------------------------------------------------
void CRenderer::InitShaders( void )
{
	m_pUnlitShader = (CShaderUnlit*)GetShaderManager()->GetShader( "Unlit" );
	m_pShaderDeferredLighting = (CShaderDeferredLighting*)GetShaderManager()->GetShader("DeferredLighting");
	m_pShaderShadowMap = (CShaderShadowMap*)GetShaderManager()->GetShader( "ShadowMap" );
	m_pShaderPointLight = (CShaderPointLight*)GetShaderManager()->GetShader( "DeferredPoint" );
	m_pShaderSpotLight = (CShaderSpotLight*)GetShaderManager()->GetShader( "DeferredSpot" );
	m_pShaderSpotTexLight = (CShaderSpotLightTex*)GetShaderManager()->GetShader( "DeferredSpotTex" );
	m_pShaderGBuffer = (CShaderGBufferOut*)GetShaderManager()->GetShader( "GBufferOut" );
	m_pShaderDeferredWater = (CShaderWater*)GetShaderManager()->GetShader( "Water" );
	m_pShaderWaterPretty = (CShaderWaterPretty*)GetShaderManager()->GetShader( "WaterPretty" );
	m_pShaderAntiAlias = (CShaderAntiAlias*)GetShaderManager()->GetShader("AntiAlias");
}


//---------------------------------------------------------------
// Purpose: Create a Final-Scene Framebuffer and bind the textures
//---------------------------------------------------------------
void CRenderer::CreateFinalSceneFrameBuffer( void )
{
	if( m_iFinalSceneFrameBuffer != INVALID_FBO )
		glDeleteFramebuffers(1, &m_iFinalSceneFrameBuffer );

	//Create Final Scene Framebuffer
	glGenFramebuffers(1, &m_iFinalSceneFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_iFinalSceneFrameBuffer);

	//Bind Texture to Color output
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_iFinalSceneTexture, 0);
	//Bind Texture to Depth output
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iFinalSceneDepthTexture, 0);

	//GLenum buf = GL_COLOR_ATTACHMENT0;
	//glDrawBuffers( 1, &buf );

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
	if( status != GL_FRAMEBUFFER_COMPLETE )
		error( "Error creating Final-Scene-Framebuffer" );

	//unbind for now
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//---------------------------------------------------------------
// Purpose: Create a Water Framebuffer and bind the textures
//---------------------------------------------------------------
void CRenderer::CreateShadowFramebuffer( void )
{
	if( m_iShadowFrameBuffer != INVALID_FBO )
		glDeleteFramebuffers(1, &m_iShadowFrameBuffer );

	glGenFramebuffers(1, &m_iShadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_iShadowFrameBuffer);

	glDrawBuffer( GL_NONE );
	glReadBuffer( GL_NONE );

	//Bind Texture to Depth output
	glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_iShadowMaps, 0, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if( status != GL_FRAMEBUFFER_COMPLETE )
		error( "Error creating Shadow-Framebuffer" );

	//unbind for now
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CRenderer::CreateGBuffer( void )
{
	if( m_iGBuffer != INVALID_FBO )
		glDeleteFramebuffers( 1, &m_iGBuffer );

	glGenFramebuffers( 1, &m_iGBuffer );
	glBindFramebuffer( GL_FRAMEBUFFER, m_iGBuffer );
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_iGBufferTex0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_iGBufferTex1, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_iGBufferTex2, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iFinalSceneDepthTexture, 0);

	GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, buffers);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER);
	if( status != GL_FRAMEBUFFER_COMPLETE )
		error( "Error creating GBuffer" );

	//unbind for now
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//---------------------------------------------------------------
// Purpose: Create a Water Framebuffer and bind the textures
//---------------------------------------------------------------
void CRenderer::CreateWaterFrameBuffer( void )
{
	if( m_iWaterFrameBuffer != INVALID_FBO )
		glDeleteFramebuffers(1, &m_iWaterFrameBuffer );

	//Create Water Framebuffer
	glGenFramebuffers(1, &m_iWaterFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_iWaterFrameBuffer);

	//Attach our texture to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_iWaterSceneTexture, 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_iWaterDepthTexture, 0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if( status != GL_FRAMEBUFFER_COMPLETE )
		error( "Error creating Water-Framebuffer" );

	//Unbind framebuffer for now!
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


//---------------------------------------------------------------
// Purpose: Main rendering entry point
//---------------------------------------------------------------
void CRenderer::Render( void )
{
	// Clear Screen And Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if( m_bLoadingScreenActive )
	{
		if(m_window && m_window->IsActive())
			RenderLoadingScreen();
	}
	else/* if( !m_bTwoPlayers )*/
	{
		m_iViewportX = m_iViewportY = 0;
		m_iViewportWidth = m_iScreenWidth;
		m_iViewportHeight = m_iScreenHeight;
		glViewport( m_iViewportX, m_iViewportY, m_iViewportWidth, m_iViewportHeight );
		//m_pPlayerOneCam->Activate( 75.0f, m_fNearClippingDist, m_fFarClippingDist, (float)m_iViewportWidth / (float)m_iViewportHeight);
		SetActiveCam(m_pPlayerOneCam);
		UpdateInterpolation();

		if(m_window && m_window->IsActive())
		{
			Render3dScene();
			RenderGui();
		}
	}
	//else
	//{
	//	m_iViewportX = 0; 
	//	m_iViewportY = 0;
	//	m_iViewportWidth = m_iScreenWidth / 2 - 1;
	//	m_iViewportHeight = m_iScreenHeight;
	//	glViewport( m_iViewportX, m_iViewportY, m_iViewportWidth, m_iViewportHeight );
	//	m_pPlayerTwoCam->Activate( 75.0f, m_fNearClippingDist, m_fFarClippingDist, (float)m_iViewportWidth / (float)m_iViewportHeight );
	//	Render3dScene();

	//	m_iViewportX = m_iScreenWidth / 2 + 1;
	//	m_iViewportY = 0;
	//	m_iViewportWidth = m_iScreenWidth / 2 - 1;
	//	m_iViewportHeight = m_iScreenHeight;
	//	glViewport( m_iViewportX, m_iViewportY, m_iViewportWidth, m_iViewportHeight );
	//	m_pPlayerOneCam->Activate( 75.0f, m_fNearClippingDist, m_fFarClippingDist, (float)m_iViewportWidth / (float)m_iViewportHeight );
	//	Render3dScene();
	//}

	m_window->SwapBuffers();
}


//---------------------------------------------------------------
// Purpose: Also called on initial setup!
//---------------------------------------------------------------
void CRenderer::ReSizeScene(int width, int height)		// Resize And Initialize The GL Window
{
	if (height == 0)
		height = 1;		

	m_iScreenWidth = width;
	m_iScreenHeight = height;
	m_iViewportX = 0; 
	m_iViewportY = 0;
	m_iViewportWidth = width;
	m_iViewportHeight = height;

	//Create / Re-Create Rendertargets
	InitRenderTextures();
	CreateGBuffer();
	CreateWaterFrameBuffer();
	CreateFinalSceneFrameBuffer();
	CreateShadowFramebuffer();

	GetShaderManager()->InitializeShaders();

	if( m_pPostProcessor )
		m_pPostProcessor->OnResolutionChange();

	if(m_pGuiRoot)
		m_pGuiRoot->UpdatePosition(true);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::SetLoadingScreenActive( bool bActive )
{
	m_bLoadingScreenActive = bActive;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::RenderDebugLights()
{
	if( render_lights_point_debug.GetBool() )
		for( int i = 0; i < m_LightList.GetNumPointLights(); i++ )
			m_LightList.GetPointLightByArrayIndex(i)->Render(true);
	if( render_lights_spot_debug.GetBool() )
		for( int i = 0; i < m_LightList.GetNumSpotLights(); i++ )
			m_LightList.GetSpotLightByArrayIndex(i)->Render(true);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::RestoreViewport(void)
{
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::EnablePhysicsDebugger( IDebugRenderer *pDebugRenderer )
{
	SetDebugRenderer(pDebugRenderer);
	CPhysDebugDrawer *physDrawer = new CPhysDebugDrawer();
	CPointerDataEvent *evt = new CPointerDataEvent(ev::SET_PHYS_DEBUGGER, (void*)physDrawer);
	g_RenderInterf->GetRenderEventMgr()->AddEventToQueue(evt);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::RenderSceneWireframed()
{
	m_pUnlitShader->Enable();
	m_pUnlitShader->EnableTexturing(false);
	m_pUnlitShader->SetDrawColor(0.0f, 1.0f, 0.0f, 1.0f);
	SetActiveRenderpass( RENDERPASS_NORMAL );
	if( render_wireframed.GetInt() > 1 )
		GetGLStateSaver()->Disable(GL_DEPTH_TEST);
	if( render_wireframed.GetInt() > 2 )
		GetGLStateSaver()->Disable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	RenderSceneGeometry(GetActiveRenderparams());
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if( render_wireframed.GetInt() > 1 )
		GetGLStateSaver()->Enable(GL_DEPTH_TEST);
	if( render_wireframed.GetInt() > 2 )
		GetGLStateSaver()->Enable(GL_CULL_FACE);
	m_pUnlitShader->SetDrawColor(1.0f, 1.0f, 1.0f, 1.0f);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::RenderDebugScene()
{
	//copy the final scene depth texture
	CShader *depthCopyShader = GetShaderManager()->GetShader("DepthCopy");
	if( !depthCopyShader )
		error( "Cannot find shader DepthCopy!!");
	else
	{
		glDepthMask(GL_TRUE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		GetGLStateSaver()->Enable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		depthCopyShader->Enable();
		depthCopyShader->EnableTexturing(true);
		DrawFullscreenQuad(m_iFinalSceneDepthTexture);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		GetGLStateSaver()->Enable( GL_BLEND );
		m_pUnlitShader->Enable();
		m_pUnlitShader->EnableTexturing(false);
		m_pDebugRenderer->RenderDebug();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::Deactivate()
{
	if(m_pOceanRenderer)
		m_pOceanRenderer->Pause();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::Activate()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::UpdateInterpolation()
{
	//update entity interpolation
	m_pRenderList->UpdateInterpolation();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRenderer::IsVSyncSupported() const
{
	return wglSwapIntervalEXT != NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::SetVSync( bool enabled )
{
	if(IsVSyncSupported())
		wglSwapIntervalEXT(enabled ? 1 : 0);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRenderer::IsVSyncEnabled() const
{
	if(IsVSyncSupported())
		return wglGetSwapIntervalEXT() == 1;
	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRenderer::GetVSyncSupportInfo()
{
#ifdef _WINDOWS
	if(!wglGetExtensionsStringARB)
		return false;

	HDC hdc = GetDC((HWND)m_window->GetWindowSystemReference());
	if(!hdc)
		return false;

	const char *wglext = wglGetExtensionsStringARB(hdc);
	if(!wglext)
		return NULL;

	if(strstr(wglext, "WGL_EXT_swap_control") == 0)
		return false;

	return true;
#else
	return false;
#endif
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderer::InitProcs()
{
#ifdef _WINDOWS
	wglGetExtensionsStringARB = (const char*(__stdcall *)(HDC))wglGetProcAddress("wglGetExtensionsStringARB");
	if(wglGetExtensionsStringARB == NULL)
		error("wglGetProcAddress failed for wglGetExtensionsStringARB");

	if(GetVSyncSupportInfo())
	{
		wglSwapIntervalEXT = (BOOL(__stdcall *)(int))wglGetProcAddress("wglSwapIntervalEXT");
		wglGetSwapIntervalEXT = (int(__stdcall *)(void))wglGetProcAddress("wglGetSwapIntervalEXT");
	}
#endif
}



