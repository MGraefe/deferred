// renderer/CBloomEffect.cpp
// renderer/CBloomEffect.cpp
// renderer/CBloomEffect.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"




//#include "CBloomEffect.h"
//#include "glheaders.h"
//#include "CRenderInterf.h"
//#include "renderer.h"
//#include "CShaders.h"
//#include "CSkyBox.h"
//
//void CBloomEffect::SetTexVars( void )
//{
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
//}
//
//
//void CBloomEffect::Init( void )
//{
//	m_iScreenWidth = g_RenderInterf->GetRenderer()->GetScreenWidth();
//	m_iScreenHeight = g_RenderInterf->GetRenderer()->GetScreenHeight();
//
//	glGenTextures( BLOOM_PASSES, m_iBlurTexs );
//	glGenTextures( BLOOM_PASSES, m_iTempTexs );
//
//	for( int i = 0; i < BLOOM_PASSES; i++ )
//	{
//		int size = IntPow( 2, i+1 );
//
//		glBindTexture( GL_TEXTURE_2D, m_iBlurTexs[i] );
//		SetTexVars();
//		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, m_iScreenWidth/size, m_iScreenHeight/size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//
//		glBindTexture( GL_TEXTURE_2D, m_iTempTexs[i] );
//		SetTexVars();
//		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, m_iScreenWidth/size, m_iScreenHeight/size, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//	}
//
//	//final output texture
//	glGenTextures( 1, &m_iFinalTex );
//	glBindTexture( GL_TEXTURE_2D, m_iFinalTex );
//	SetTexVars();
//	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, m_iScreenWidth, m_iScreenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//
//	//Bright-pass texture
//	glGenTextures( 1, &m_iBrightPassTex );
//	glBindTexture( GL_TEXTURE_2D, m_iBrightPassTex );
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
//	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, m_iScreenWidth, m_iScreenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//
//	//Create the screenbuffer
//	glGenFramebuffers(1, &m_iBlurFrameBuffer);
//	glBindFramebuffer(GL_FRAMEBUFFER, m_iBlurFrameBuffer);
//
//	//Attach our texture to the framebuffer
//	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_iBlurTexs[0], 0);
//
//	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//	if( status != GL_FRAMEBUFFER_COMPLETE )
//		error( "Error creating Blur-Framebuffer" );
//
//	//Unbind framebuffer for now!
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	//Get shader pointers
//	m_pBloomShader = (CShaderBloom*)GetShaderManager()->GetShader( "Bloom" );
//	m_pBloomCompositor = (CShaderBloomCompositor*)GetShaderManager()->GetShader( "BloomCompositor" );
//	m_pUnlitShader = (CShaderUnlit*)GetShaderManager()->GetShader( "Unlit" );
//}
//
//
//void CBloomEffect::Apply( UINT SceneTex, UINT DepthTex )
//{
//	//GetGLStateSaver()->Disable( GL_DEPTH_TEST );
//	GetGLStateSaver()->Enable( GL_TEXTURE_2D );
//
//	glBindTexture( GL_TEXTURE_2D, SceneTex );
//	glGenerateMipmap( GL_TEXTURE_2D );
//
//	//First brightpass the scene texture
//	m_pBloomShader->Enable();
//	m_pBloomShader->SetBrightPassMode( 1 );
//	m_pBloomShader->SetBrightpassThreshold( Vector3f( 0.4f, 0.4f, 0.4f ) );
//
//	glBindFramebuffer( GL_FRAMEBUFFER, m_iBlurFrameBuffer );
//	BindOutputTexture( m_iBrightPassTex );
//	glBindTexture( GL_TEXTURE_2D, SceneTex );
//	RenderFullscreenQuad();
//
//	//Then attach the depth texture
//	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTex, 0 );
//	glEnable( GL_DEPTH_TEST );
//
//	//Render additional glowing things with lower brightpass threshold
//	m_pBloomShader->SetBrightpassThreshold( vec3_null );
//
//	//Draw sun
//	CSkyBox *pSkyBox = g_RenderInterf->GetRenderer()->GetSkyBox();
//	pSkyBox->DrawSun( g_RenderInterf->GetRenderer()->GetLightDir() );
//
//	//Generate mipmaps for the brightpass tex
//	glBindTexture( GL_TEXTURE_2D, m_iBrightPassTex );
//	glGenerateMipmap( GL_TEXTURE_2D );
//
//	m_pBloomShader->SetBrightPassMode(0);
//
//	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0 );
//
//	for( int i = 0; i < BLOOM_PASSES; i++ )
//	{
//		int iSize = IntPow( 2, i+1 );
//		glViewport( 0, 0, m_iScreenWidth/iSize, m_iScreenHeight/iSize );
//
//		//Blur vertical
//		m_pBloomShader->SetBlurMode( BLUR_HORIZONTAL );
//		BindOutputTexture( m_iTempTexs[i] );
//		glBindTexture( GL_TEXTURE_2D, m_iBrightPassTex );
//		RenderFullscreenQuad();
//
//		//Blur horizontal
//		m_pBloomShader->SetBlurMode( BLUR_VERTICAL );
//		BindOutputTexture( m_iBlurTexs[i] );
//		glBindTexture( GL_TEXTURE_2D, m_iTempTexs[i] );
//		RenderFullscreenQuad();
//	}
//
//	//Restore viewport
//	glViewport( 0, 0, m_iScreenWidth, m_iScreenHeight );
//
//	//Now we have a set of brightpassed and blurred textures of descending size.
//	//Mix them together via the mix shader
//	BindOutputTexture( m_iFinalTex );
//	m_pBloomCompositor->Enable();
//
//	for( int i = 0; i < BLOOM_PASSES; i++ )
//	{
//		GetGLStateSaver()->ActiveTexture( GL_TEXTURE0+i );
//		glBindTexture( GL_TEXTURE_2D, m_iBlurTexs[i] );
//	}
//
//	RenderFullscreenQuad();
//
//	//Revert state changes
//	GetGLStateSaver()->ActiveTexture( GL_TEXTURE0 );
//	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
//	//GetGLStateSaver()->Enable( GL_DEPTH_TEST );
//}
//
//
//void CBloomEffect::BindOutputTexture( UINT index )
//{
//	//glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0 );
//	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, index, 0 );
//}
//
//
//UINT CBloomEffect::GetBlurTexture( void )
//{
//	return m_iFinalTex;
//}
//
//
//void CBloomEffect::RenderFullscreenQuad( void )
//{
//	glMatrixMode (GL_MODELVIEW);
//	glPushMatrix ();
//		glLoadIdentity ();
//		glMatrixMode (GL_PROJECTION);
//			glPushMatrix ();
//				glLoadIdentity ();
//				//glBegin (GL_QUADS);
//				//	glTexCoord2f( 0.0f, 0.0f ); glVertex3i (-1, -1, -1);
//				//	glTexCoord2f( 1.0f, 0.0f ); glVertex3i (1, -1, -1);
//				//	glTexCoord2f( 1.0f, 1.0f ); glVertex3i (1, 1, -1);
//				//	glTexCoord2f( 0.0f, 1.0f ); glVertex3i (-1, 1, -1);
//				//glEnd();
//				draw_quad_textured( -1.0f, -1.0f, 1.0f, 1.0f, -1.0f );
//			glPopMatrix ();
//		glMatrixMode (GL_MODELVIEW);
//	glPopMatrix ();
//}
