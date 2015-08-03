// renderer/CSkyBox.cpp
// renderer/CSkyBox.cpp
// renderer/CSkyBox.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "CSkyBox.h"
#include "glheaders.h"
#include "CCamera.h"
#include "CShaders.h"
#include "renderer.h"
#include "glfunctions.h"
#include "CViewFrustum.h"
#include "CRenderInterf.h"
#include "TextureManager.h"
#include <util/CConVar.h>
#include "CLoadingMutex.h"

CSkyBox::CSkyBox()
{
	m_bSkyLoaded = false;
}


CSkyBox::~CSkyBox()
{
	glDeleteTextures( 1, &m_iCubeMapTex );
}


void CSkyBox::LoadSkyTextures( const char *pSkyName )
{
	//store names
	if( !m_bSkyLoaded )
	{
		strcpy( m_pSkyName, pSkyName );
	}

	const char pchSkyAppends[6][4] = { "_px", "_nx", "_py", "_ny", "_pz", "_nz" };

	int texTargets[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};

	glGenTextures( 1, &m_iCubeMapTex );

	for( int i = 0; i < 6; i++ )
	{
		string skyFileName("textures/sky/");
		skyFileName += pSkyName;
		skyFileName += pchSkyAppends[i];
		skyFileName += ".dds";

		//Set clamping
		g_LoadingMutex->SetOrWait();

		//Load the cube map texture
		glBindTexture( GL_TEXTURE_2D, 0 );
		glEnable(GL_TEXTURE_CUBE_MAP);
		glBindTexture( GL_TEXTURE_CUBE_MAP, m_iCubeMapTex );
		texture_t tex;
		tex.index = m_iCubeMapTex;
		g_LoadingMutex->Release();
		ConsoleMessage("Loading sky-tex \"%s\"", skyFileName.c_str());
		if( !g_RenderInterf->GetTextureManager()->LoadImageFromFile( skyFileName.c_str(), m_iCubeMapTex, tex, texTargets[i], true, true ) )
			error("Error loading sky \"%s\", expect black sky.", pSkyName);
	}

	g_LoadingMutex->SetOrWait();
	glBindTexture( GL_TEXTURE_CUBE_MAP, m_iCubeMapTex );
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	g_LoadingMutex->Release();

	m_bSkyLoaded = true;

	m_pShader = (CShaderUnlit*)GetShaderManager()->GetShader( "Unlit" );
	m_pShaderSky = (CShaderSky*)GetShaderManager()->GetShader( "Sky" );
}




void CSkyBox::OnRestore( void )
{
	LoadSkyTextures( m_pSkyName );
}

CConVar render_sky_brightness("render.sky.brightness", "1.0");
void CSkyBox::DrawSky( Vector3f vSunDir, bool bReflection)
{
#if 0
	if( !m_bSkyLoaded )
		return;
	
	m_pShader->Enable();
	glDepthRange(1.0, 1.0);
	GetGLStateSaver()->Enable(GL_DEPTH_TEST);
	GetGLStateSaver()->DepthFunc(GL_EQUAL);
	glDepthMask( GL_FALSE );
	GetGLStateSaver()->Enable( GL_TEXTURE_2D );

	GetMVStack().PushMatrix();
	Vector3f vCamPos = g_RenderInterf->GetRenderer()->GetActiveCam()->GetAbsPos();
	if( bReflection )
		vCamPos.y = -vCamPos.y + g_RenderInterf->GetRenderer()->GetWaterHeight() * 2.0f;

	GetMVStack().Translate( vCamPos );

	GetShaderManager()->GetActiveShader()->UpdateMatrices();

	float d = 1000.0f;

	//Skybox vertexes:
	float verts[] = {	

		//Right Face
		1.0f, 1.0f, d, d, -d, //Top Left
		1.0f, 0.0f, d, -d, -d, //Bottom Left
		0.0f, 0.0f, d, -d, d, //Bottom Right
		0.0f, 1.0f, d, d, d, //Top Right

		//Left Face
		1.0f, 1.0f, -d, d, d, //Top Left
		1.0f, 0.0f, -d, -d, d, //Bottom Left
		0.0f, 0.0f, -d, -d, -d, //Bottom Right
		0.0f, 1.0f, -d, d, -d, //Top Right

		//Up Face
		0.0f, 1.0f, -d, d, d, //Top Left
		0.0f, 0.0f, -d, d, -d, //Bottom Left
		1.0f, 0.0f, d, d, -d, //Bottom Right
		1.0f, 1.0f, d, d, d, //Top Right

		//Down Face
		1.0f, 1.0f, -d, -d, -d, //Top Left
		1.0f, 0.0f, -d, -d, d, //Bottom Left
		0.0f, 0.0f, d, -d, d, //Bottom Right
		0.0f, 1.0f, d, -d, -d, //Top Right

		//Back Face
		1.0f, 1.0f, d, d, d, //Top Left
		1.0f, 0.0f, d, -d, d, //Bottom Left
		0.0f, 0.0f, -d, -d, d, //Bottom Right
		0.0f, 1.0f, -d, d, d, //Top Right

		//Front Face
		1.0f, 1.0f,-d, d, -d, //Top Left
		1.0f, 0.0f, -d, -d, -d, //Bottom Left
		0.0f, 0.0f, d, -d, -d, //Bottom Right
		0.0f, 1.0f, d, d, -d, //Top Right
	}; 

	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	init_vertexbuffer_vars_uvcoord_pos( verts );

	for( int i = 0; i < 6; i++ )
	{
		glBindTexture( GL_TEXTURE_2D, m_SkyTexs[i].index );
		glDrawArrays( GL_QUADS, i*4, 4 );
	}

	rglPopClientAttrib();
	GetMVStack().PopMatrix();

	glDepthMask( GL_TRUE ); //Re-Enable Z-Buffer
	GetGLStateSaver()->DepthFunc(GL_LESS);
	glDepthRange(0.0, 1.0);
#else
	if( !m_bSkyLoaded )
		return;
	
	m_pShaderSky->Enable();
	m_pShaderSky->SetBrightness(render_sky_brightness.GetFloat());
	glDepthRange(1.0, 1.0);
	GetGLStateSaver()->Enable(GL_DEPTH_TEST);
	GetGLStateSaver()->DepthFunc(GL_EQUAL);
	glDepthMask( GL_FALSE );
	GetGLStateSaver()->Enable( GL_TEXTURE_2D );
	
	CCamera *cam = g_RenderInterf->GetRenderer()->GetActiveCam();
	GetMVStack().PushMatrix();
	GetMVStack().Translate( cam->GetAbsPos() );
	//GetMVStack().Rotate(cam->GetAbsAngles().GetConjugate());

	m_pShaderSky->UpdateMatrices();

	float d = 1000.0f;

	//Skybox vertexes:
	float verts[] = {	

		//Right Face
		d, d, -d, //Top Left
		d, -d, -d, //Bottom Left
		d, -d, d, //Bottom Right
		d, d, d, //Top Right

		//Left Face
		-d, d, d, //Top Left
		-d, -d, d, //Bottom Left
		-d, -d, -d, //Bottom Right
		-d, d, -d, //Top Right

		//Up Face
		-d, d, d, //Top Left
		-d, d, -d, //Bottom Left
		d, d, -d, //Bottom Right
		d, d, d, //Top Right

		//Down Face
		-d, -d, -d, //Top Left
		-d, -d, d, //Bottom Left
		d, -d, d, //Bottom Right
		d, -d, -d, //Top Right

		//Back Face
		d, d, d, //Top Left
		d, -d, d, //Bottom Left
		-d, -d, d, //Bottom Right
		-d, d, d, //Top Right

		//Front Face
		-d, d, -d, //Top Left
		-d, -d, -d, //Bottom Left
		d, -d, -d, //Bottom Right
		d, d, -d, //Top Right
	}; 

	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars(VBF_POS, verts);

	GetGLStateSaver()->ActiveTexture( GL_TEXTURE0 );
	GetGLStateSaver()->Enable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_iCubeMapTex);
	
	glDrawArrays(GL_QUADS, 0, 6*4);

	rglPopClientAttrib();
	GetMVStack().PopMatrix();

	GetGLStateSaver()->Disable(GL_TEXTURE_CUBE_MAP);

	glDepthMask( GL_TRUE ); //Re-Enable Z-Buffer
	GetGLStateSaver()->DepthFunc(GL_LESS);
	glDepthRange(0.0, 1.0);
#endif
}



