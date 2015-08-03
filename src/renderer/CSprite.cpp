// renderer/CSprite.cpp
// renderer/CSprite.cpp
// renderer/CSprite.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CSprite.h"
#include "TextureManager.h"
#include "Interfaces.h"
#include "CRenderInterf.h"
#include "renderer.h"
#include "CCamera.h"
#include "CShaders.h"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CSprite::CSprite( int index, bool pointSprite ) 
	: BaseClass(index),
	m_bPointSprite(pointSprite)
{
	m_axis = AXIS_ALL;
	m_bInitialized = false;
	m_material = 0;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CSprite::~CSprite()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSprite::Init( const char *texname, const Vector3f &origin, float width, float height, spriteaxis_e axis )
{
	m_material = g_RenderInterf->GetTextureManager()->LoadMaterial( texname );
	Init( m_material, origin, width, height, axis );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSprite::Init( int material, const Vector3f &origin, float width, float height, spriteaxis_e axis /*= AXIS_ALL */ )
{
	m_material = material;
	SetAbsPos(origin);
	m_width = width;
	m_height = height;
	m_axis = axis;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSprite::VRender( const CRenderParams &params )
{
	if( m_bPointSprite )
	{
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
		glPointParameteri( GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT );

		CShader *pOldShader = GetShaderManager()->GetActiveShader();
		GetShaderManager()->GetShader( "PointSprite" )->Enable();
		float fov = g_RenderInterf->GetRenderer()->GetActiveCam()->GetFov();
		float screenHeight = (float)g_RenderInterf->GetRenderer()->GetScreenHeight();
		g_RenderInterf->GetTextureManager()->BindMaterial( m_material );
		draw_point_sprite( GetAbsPos(), m_width*TAN(fov*0.5f)*screenHeight );
		pOldShader->Enable();


		glDisable(GL_POINT_SPRITE);
		glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	}
	else
	{
		CCamera *pCam = g_RenderInterf->GetRenderer()->GetActiveCam();
		if( !pCam )
			return;

		Vector3f vDir = GetAbsPos() - pCam->GetAbsPos();

		if( m_axis != AXIS_ALL )
			vDir[m_axis] = 0.0f;

		if( vDir.Dot(vDir) < 0.01f )
			return;

		Angle3d ang;
		VectorToAngle( vDir.Normalize(), ang );
		Quaternion quat(ang);

		SetAbsAngles(quat);

		trans_rot_model_start( GetAbsPos(), GetAbsAngles() );
		g_RenderInterf->GetTextureManager()->BindMaterial( m_material );
		draw_quad_textured_normal( -m_width/2.0f, -m_height/2.0f, m_width/2.0f, m_height/2.0f, 0.0f );
		trans_rot_model_end();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSprite::VOnRestore( void )
{
	//BaseClass::VOnRestore();
}
