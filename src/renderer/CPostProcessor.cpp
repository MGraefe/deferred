// renderer/CPostProcessor.cpp
// renderer/CPostProcessor.cpp
// renderer/CPostProcessor.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "CPostProcessor.h"
#include "CShaders.h"
#include "renderer.h"
#include <util/CConVar.h>

CConVar render_bloom_lod_start("render.bloom.lod.start", "2");
CConVar render_bloom_lod_levels("render.bloom.lod.levels", "2");

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CPostProcessor::CPostProcessor()
{
	m_iFramebuffer = INVALID_FBO;
	m_bufTex2 = INVALID_TEX;
	m_LumTexIndex = 0;
	m_LumTexs[0] = INVALID_TEX;
	m_LumTexs[1] = INVALID_TEX;
	m_shaderBlur = NULL;
	m_shaderBloomComplete = NULL;
	m_shaderTonemap = NULL;
	m_shaderBrightpass = NULL;
	m_shaderExposure = NULL;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPostProcessor::Init( void )
{
	if( m_bufTex2 != INVALID_TEX )
		glDeleteTextures(1, &m_bufTex2);

	glGenTextures(1, &m_bufTex2);

	glBindTexture( GL_TEXTURE_2D, m_bufTex2 );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8,
		g_RenderInterf->GetRenderer()->GetScreenWidth(),
		g_RenderInterf->GetRenderer()->GetScreenHeight(),
		0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	if( m_LumTexs[0] != INVALID_TEX )
		glDeleteTextures(2, m_LumTexs);

	glGenTextures(2, m_LumTexs);
	for( int i = 0; i < 2; i++ )
	{
		float data = 0.5f;
		glBindTexture( GL_TEXTURE_2D, m_LumTexs[i] );
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 1, 1, 0, GL_RED, GL_FLOAT, &data);
	}

	if( m_iFramebuffer != INVALID_FBO )
		glDeleteFramebuffers(1, &m_iFramebuffer );

	//Create Water Framebuffer
	glGenFramebuffers(1, &m_iFramebuffer);

	m_shaderBlur = (CShaderBlur*)g_RenderInterf->GetShaderManager()->GetShader("Blur");
	m_shaderBloomComplete = (CShaderBloomComplete*)g_RenderInterf->GetShaderManager()->GetShader("BloomComplete");
	m_shaderTonemap = (CShaderTonemap*)g_RenderInterf->GetShaderManager()->GetShader("Tonemap");
	m_shaderBrightpass = (CShaderBrightpass*)g_RenderInterf->GetShaderManager()->GetShader("Brightpass");
	m_shaderExposure = (CShaderExposure*)g_RenderInterf->GetShaderManager()->GetShader("Exposure");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPostProcessor::OnResolutionChange(void)
{
	Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPostProcessor::ApplyPostProcessing( UINT sceneTex, UINT sceneDepthTex, UINT bufferTex, UINT destTex )
{
	glPushAttrib(GL_VIEWPORT_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, m_iFramebuffer);

	Assert(sceneTex != destTex && sceneDepthTex != destTex);

	int bloomLodStart = render_bloom_lod_start.GetInt();
	int bloomLodLevels = render_bloom_lod_levels.GetInt();

	//Bloom effect
	brightpass( sceneTex, m_bufTex2, m_LumTexs[m_LumTexIndex] );

	//Use destTex as temporary buffer
	for(int i = bloomLodStart; i < bloomLodStart+bloomLodLevels; i++)
		blur(true, m_bufTex2, bufferTex, i);
	for(int i = bloomLodStart; i < bloomLodStart+bloomLodLevels; i++)
		blur(false, bufferTex, m_bufTex2, i);

	//bloomComplete(sceneTex, bufferTex);

	calcExposure(sceneTex);
	tonemap(sceneTex, sceneDepthTex, m_bufTex2, m_LumTexs[m_LumTexIndex], destTex);

	//calcExposure(sceneTex);

	glPopAttrib();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPostProcessor::brightpass( UINT sourceTex, UINT destTex, UINT lumTex )
{
	int lodStart = render_bloom_lod_start.GetInt();
	int lodLevels = render_bloom_lod_levels.GetInt();

	int vpW = g_RenderInterf->GetRenderer()->GetScreenWidth() / IntPow(2, lodStart);
	int vpH = g_RenderInterf->GetRenderer()->GetScreenHeight() / IntPow(2, lodStart);

	//glBindTexture(GL_TEXTURE_2D, destTex);
	//glGenerateMipmap(GL_TEXTURE_2D);

	g_RenderInterf->GetRenderer()->BindTexture( lumTex, 1 );
	g_RenderInterf->GetRenderer()->BindTexture( sourceTex, 0 );
	glGenerateMipmap(GL_TEXTURE_2D);

	m_shaderBrightpass->Enable();

	for( int i = lodStart; i < lodStart+lodLevels; i++ )
	{
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTex, i );
		m_shaderBrightpass->SetLodLevel(i);
		draw_fullscreen_quad_no_tex();

		vpW /= 2;
		vpH /= 2;
		glViewport(0, 0, vpW, vpH);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPostProcessor::blur( bool hori, UINT sourceTex, UINT destTex, int lod )
{
	int vpW = g_RenderInterf->GetRenderer()->GetScreenWidth() / IntPow(2, lod);
	int vpH = g_RenderInterf->GetRenderer()->GetScreenHeight() / IntPow(2, lod);

	m_shaderBlur->Enable();
	m_shaderBlur->SetLodLevel(lod);
	m_shaderBlur->SetBlurDirection(hori);

	glViewport(0,0,vpW,vpH);
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTex, lod );	
	glBindTexture(GL_TEXTURE_2D, sourceTex);
	draw_fullscreen_quad();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPostProcessor::tonemap( UINT sceneTex, UINT sceneDepthTex, UINT bloomTex, UINT lumTex, UINT destTex )
{
	CRenderer *r = g_RenderInterf->GetRenderer();
	m_shaderTonemap->Enable();
	m_shaderTonemap->SetZBounds( r->GetNearClippingDist(), r->GetFarClippingDist() );
	m_shaderTonemap->SetBloomLodLevels(
		render_bloom_lod_start.GetInt(), render_bloom_lod_levels.GetInt());

	r->BindTexture( sceneTex,		0 );
	r->BindTexture( sceneDepthTex,	1 );
	r->BindTexture( bloomTex,		2 );
	r->BindTexture( lumTex,			3, 0 );

	if( destTex <= 0 )
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTex, 0);

	draw_fullscreen_quad();
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPostProcessor::bloomComplete( UINT sourceTex, UINT destTex )
{
	GetGLStateSaver()->Disable(GL_BLEND);

	m_shaderBloomComplete->Enable();
	m_shaderBloomComplete->SetBrightpassSubstract(Vector4f(-0.5f, -0.5f, -0.5f, -0.5f));

	glBindTexture(GL_TEXTURE_2D, destTex);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, sourceTex);
	glGenerateMipmap(GL_TEXTURE_2D);

	int lodStart = render_bloom_lod_start.GetInt();
	int lodLevels = render_bloom_lod_levels.GetInt();

	int vpW = g_RenderInterf->GetRenderer()->GetScreenWidth() / IntPow(2, lodStart);
	int vpH = g_RenderInterf->GetRenderer()->GetScreenHeight() / IntPow(2, lodStart);

	for( int i = lodStart; i < lodStart+lodLevels; i++ )
	{
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, destTex, i );
		m_shaderBloomComplete->SetLodLevel(i);
		draw_fullscreen_quad_no_tex();

		vpW /= 2;
		vpH /= 2;
		glViewport(0, 0, vpW, vpH);
	}

	g_RenderInterf->GetRenderer()->RestoreViewport();
}

CConVar render_hdr_exposure_readback("render.hdr.exposure.readback", "1");

//---------------------------------------------------------------
// Purpose: Calculate new scene luminance on the gpu
//---------------------------------------------------------------
void CPostProcessor::calcExposure( UINT sourceTex )
{
	//Enable Shader
	m_shaderExposure->Enable();
	m_shaderExposure->SetFrametime(gpGlobals->frametime);

	//Calculate ping-pong ids
	int srcId = m_LumTexIndex;
	int dstId = (m_LumTexIndex + 1) % 2;
	m_LumTexIndex = dstId;

	//set input textures
	GetGLStateSaver()->ActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, sourceTex);
	glGenerateMipmap(GL_TEXTURE_2D);
	GetGLStateSaver()->ActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_LumTexs[srcId]);

	//set output texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_LumTexs[dstId], 0);

	//draw
	glViewport(0, 0, 1, 1);
	draw_fullscreen_quad_no_tex();
	g_RenderInterf->GetRenderer()->RestoreViewport();
}




