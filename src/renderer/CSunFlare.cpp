// renderer/CSunFlare.cpp
// renderer/CSunFlare.cpp
// renderer/CSunFlare.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "CSunFlare.h"
#include "TextureManager.h"
#include "Interfaces.h"
#include "CSprite.h"
#include "CShaders.h"
#include "renderer.h"
#include "CCamera.h"
#include "CLoadingMutex.h"
#include <util/CConVar.h>

CSunFlare::CSunFlare()
{
	m_materialId = -1;
	m_size = 10.0f;
	m_bFirstFrame = true;
}


void CSunFlare::Init(const char *sunTexName, float size)
{
	SetTexture(sunTexName);
	SetSize(size);
	m_bFirstFrame = true;
}


void CSunFlare::SetTexture(const char *sunTexName)
{
	m_materialId = g_RenderInterf->GetTextureManager()->LoadMaterial(sunTexName);
}


void CSunFlare::SetSize(float size)
{
	m_size = size;
	m_occlusionSize = size/20.0f;
}


void CSunFlare::RenderSprite(float size)
{
	CRenderer *pRenderer = g_RenderInterf->GetRenderer();
	CCamera *pCam = pRenderer->GetActiveCam();
	Vector3f sunDir = -pRenderer->GetLightDir();

	//Get screen-space coordinate of center
	Vector4f center = GetProjStack().GetMatrixObj() * 
		GetMVStack().GetMatrixObj() * 
		Vector4f(pCam->GetAbsPos()+sunDir*100.0f, 1.0f);
	if(center.w < 0.0f)
		return;
	Vector2f ndc(center.x/center.w, center.y/center.w);
	GetProjStack().PushMatrix();
	GetProjStack().SetIdentity();
	GetMVStack().PushMatrix();
	GetMVStack().SetIdentity();

	float screenWidth = (float)pRenderer->GetScreenWidth();
	float screenHeight = (float)pRenderer->GetScreenHeight();
	float aspect = screenHeight / screenWidth;
	float sy = size;
	float sx = sy * aspect;
	draw_quad_textured( ndc.x-sx, ndc.y-sy, ndc.x+sx, ndc.y+sy, -1.0f );

	GetProjStack().PopMatrix();
	GetMVStack().PopMatrix();
}


void CSunFlare::Render(void)
{
	if( m_occlusion < 0.001f || m_materialId == -1 )
		return;
	GetGLStateSaver()->ActiveTexture(GL_TEXTURE0);
	CShaderUnlit *shader = (CShaderUnlit*)GetShaderManager()->GetShader( "Unlit" );
	shader->Enable();
	shader->SetDrawColor(1.0f, 1.0f, 1.0f, lerpclamped(m_occlusion, 0.5f, 1.0f, 0.0f, 1.0f));
	shader->EnableTexturing(true);
	shader->SetSingleChannelAlpha(true);

	g_RenderInterf->GetTextureManager()->BindMaterial( m_materialId );
	glDepthMask(false);
	GetGLStateSaver()->Disable(GL_DEPTH_TEST);
	GetGLStateSaver()->Enable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	RenderSprite(m_size);
	GetGLStateSaver()->Enable(GL_DEPTH_TEST);
	glDepthMask(true);
	shader->SetSingleChannelAlpha(false);
}

CConVar render_sunflare_occludersize( "render.sunflare.occluderfactor", "0.05" );
void CSunFlare::QueryOcclusion(void)
{
	if( m_materialId == -1 || m_queryIds.empty() )
		return;

	glDepthRange(1.0, 1.0);
	glDepthMask(false);
	glColorMask(false, false, false, false);
	GetGLStateSaver()->DepthFunc(GL_EQUAL);
	GetGLStateSaver()->Disable(GL_BLEND);
	glBeginQuery(GL_SAMPLES_PASSED, m_queryIds.back());
	RenderSprite(m_size*render_sunflare_occludersize.GetFloat());
	glEndQuery(GL_SAMPLES_PASSED);
	GetGLStateSaver()->DepthFunc(GL_LESS);
	glColorMask(true, true, true, true);
	glDepthMask(true);
	glDepthRange(0.0, 1.0);
}


void CSunFlare::GenerateQuery(void)
{
	GLuint query;
	glGenQueries(1, &query);
	m_queryIds.push(query);
}


void CSunFlare::ResolveOcclusion(void)
{
	if( m_materialId == -1 )
		return;

	if( m_queryIds.empty() )
		GenerateQuery();
	else
	{
		GLuint resultThere;
		glGetQueryObjectuiv(m_queryIds.front(), GL_QUERY_RESULT_AVAILABLE, &resultThere);

		if( resultThere )
		{
			GLuint result;
			glGetQueryObjectuiv(m_queryIds.front(), GL_QUERY_RESULT, &result);
			//ConsoleMessage("sun occlusion pass: %i", (int)result);
			float pixelHeight = m_size*render_sunflare_occludersize.GetFloat()*(float)g_RenderInterf->GetRenderer()->GetScreenHeight();
			m_occlusion = (float)result / (pixelHeight*pixelHeight);
			m_occlusion = clamp(m_occlusion, 0.0f, 1.0f);
			m_queryIds.push(m_queryIds.front());
			m_queryIds.pop();
		}
		else if(!m_bFirstFrame) //result is not there, we need to delay querying by one additional frame
			GenerateQuery();
	}

	m_bFirstFrame = false;
}
