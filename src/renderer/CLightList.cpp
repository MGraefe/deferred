// renderer/CLightList.cpp
// renderer/CLightList.cpp
// renderer/CLightList.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CLightList.h"
#include "CShaders.h"
#include <util/basicfuncs.h>
#include "CBaseGeometry.h"
#include "CRenderInterf.h"
#include "renderer.h"
#include "CCamera.h"
#include "CViewFrustum.h"
#include "TextureManager.h"


BEGIN_DATADESC_EDITOR(CLight)
	NETTABLE_ENTRY(m_vColor)
	NETTABLE_ENTRY(m_Params)
	NETTABLE_ENTRY(m_maxDist)
	NETTABLE_ENTRY(m_enabled)
END_DATADESC_EDITOR()

LINK_ENTITY_TO_CLASSNAME_RENDERER(CPointLight, light)
BEGIN_DATADESC_EDITOR(CPointLight)
END_DATADESC_EDITOR()

LINK_ENTITY_TO_CLASSNAME_RENDERER(CSpotLight, light_spot)
BEGIN_DATADESC_EDITOR(CSpotLight)
	NETTABLE_ENTRY(m_fSpotDegrees)
	NETTABLE_ENTRY(m_fExponent)
END_DATADESC_EDITOR()

LINK_ENTITY_TO_CLASSNAME_RENDERER(CSunLight, light_sun)
BEGIN_DATADESC_EDITOR(CSunLight)
	NETTABLE_ENTRY(m_direction)
	NETTABLE_ENTRY(m_color)
	NETTABLE_ENTRY(m_specular)
	NETTABLE_ENTRY(m_ambient1)
	NETTABLE_ENTRY(m_ambient2)
END_DATADESC_EDITOR()

LINK_ENTITY_TO_CLASSNAME_RENDERER(CSpotTexLight, light_spot_tex)
BEGIN_DATADESC_EDITOR(CSpotTexLight)
	NETTABLE_ENTRY(m_fov)
	NETTABLE_ENTRY(m_aspect)
	NETTABLE_ENTRY_STRING(m_texFilename)
END_DATADESC_EDITOR()


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLight::UpdateDist( void )
{
	m_Params.dist = ::CalcMaxDistPointLight( GetConstant(),
		GetLinear(), GetQuadratic(), GetBrightness(), 0.1f );
}


//---------------------------------------------------------------
// Purpose: There are 3 cases here:
//			1:  The near-plane intersects with the light volume OR
//				the light is large (intersects with far-plane) and we are
//				inside the light:
//				We cant do anything else than draw a fullscreen quad,
//				but this case is very rare so dont bother
//				(camInsideLight is ignored if nearPlaneIntersect is true)
//			2:	The camera is outside the light volume:
//				The "normal" case, just render the light geometry
//				with backface-culling and GL_LESS depth testing
//			3:	The camera is inside the light volume:
//				Render the light geometry with frontface-culling
//				and GL_GREATER depth testing
//---------------------------------------------------------------
void CLight::RenderGeometry( CBaseGeometry *geom, bool debug, bool camInsideLight, bool nearPlaneIntersect, bool largeLight )
{
	if( debug )
	{
		//For debug reasons only
		if( !nearPlaneIntersect )
		{
			glPushAttrib( GL_POLYGON_BIT );
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			glDisable(GL_CULL_FACE);
			if( camInsideLight )
				GetGLStateSaver()->DepthFunc( GL_GREATER );
			CRenderParams params;
			params.renderpass = RENDERPASS_NORMAL;
			geom->VRender(params);
			if( camInsideLight )
				GetGLStateSaver()->DepthFunc( GL_LESS );
			glPopAttrib();
		}
	}
	else
	{
		if( (largeLight&&camInsideLight) || nearPlaneIntersect )
		{
			glDisable(GL_DEPTH_TEST);
			GetGLStateSaver()->CullFace( GL_BACK );
			draw_fullscreen_quad_no_tex();
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			if( camInsideLight )
			{
				GetGLStateSaver()->DepthFunc( GL_GREATER );
				GetGLStateSaver()->CullFace( GL_FRONT );
			}
			else
			{
				GetGLStateSaver()->DepthFunc( GL_LESS );
				GetGLStateSaver()->CullFace( GL_BACK );
			}
			geom->RenderNoTextureBufferOnly(false);
		}
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSpotLight::Render( bool debug /*= false */ )
{
	CRenderer *pRenderer = g_RenderInterf->GetRenderer();
	CBaseGeometry *lightGeom = pRenderer->GetLightList()->GetUnitCone();

	if( !lightGeom )
	{
		if( !debug )
		{
			glDisable( GL_DEPTH_TEST );
			draw_fullscreen_quad_no_tex();
			glEnable( GL_DEPTH_TEST );
		}
		return;
	}

	const Vector3f &camPos = pRenderer->GetActiveCam()->GetAbsPos();
	const Vector3f *nearVerts = pRenderer->GetActiveCam()->GetFrustum()->GetNearPlaneVerts();
	Vector3f lightDir = GetAbsAngles() * Vector3f(0.0f, 0.0f, 1.0f);
	Vector3f lightToCam = camPos - GetAbsPos();
	float dist = lightToCam.Length();
	lightToCam /= dist;
	float minDot = COS(GetDegrees()*0.5f);
	float fMaxDist = GetDist();
	lightGeom->SetAbsPos( GetAbsPos() );
	lightGeom->SetAbsAngles( GetAbsAngles() );

	bool bCamInsideLight = dist < fMaxDist && lightDir.Dot(lightToCam) >= minDot;
	bool bNearPlaneIntersect = IsNearPlaneIntersect(dist, fMaxDist, minDot, lightDir, nearVerts, camPos);
	bool bVeryLargeLight = fMaxDist > pRenderer->GetFarClippingDist();
	
	//calculate light geometry scales
	Vector3f scale;
	scale.z = fMaxDist;
	scale.x = scale.y = TAN(GetDegrees()*0.5f) * fMaxDist;
	lightGeom->SetScale(scale);

	RenderGeometry( lightGeom, debug, bCamInsideLight, bNearPlaneIntersect, bVeryLargeLight );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
inline bool point_in_cone( float minDot, const Vector3f &coneOrig, const Vector3f &coneDir, const Vector3f &point )
{
	return (point-coneOrig).GetNormalized().Dot( coneDir ) > minDot;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CSpotLight::IsNearPlaneIntersect( float dist, float fMaxDist, float minDot,
	const Vector3f &lightDir, const Vector3f *nearVerts, const Vector3f &camPos )
{
	//Check if we intersect with the near-plane, but only if our camera distance is lower than the light dist
	bool bNearPlaneIntersect = false;
	if( dist < fMaxDist+3.0f )
	{
		bool bFirstPointInCone = point_in_cone(minDot, GetAbsPos(), lightDir, nearVerts[0]+camPos);
		for( int i = 1; i < 4; i++ )
		{
			if( bFirstPointInCone != point_in_cone(minDot, GetAbsPos(), lightDir, nearVerts[i]+camPos) )
			{
				bNearPlaneIntersect = true;
				break;
			}
		}
	}
	return bNearPlaneIntersect;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSpotLight::OnSpawn()
{
	BaseClass::OnSpawn();
	g_RenderInterf->GetRenderer()->GetLightList()->AddSpotLight(this);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPointLight::Render( bool debug )
{
	CRenderer *pRenderer = g_RenderInterf->GetRenderer();
	CBaseGeometry *lightGeom = pRenderer->GetLightList()->GetUnitSphere();
	
	if( !lightGeom )
	{
		if( !debug )
		{
			glDisable( GL_DEPTH_TEST );
			draw_fullscreen_quad_no_tex();
			glEnable( GL_DEPTH_TEST );
		}
		return;
	}


	float fMaxDist = GetDist();

	const Vector3f &camPos = pRenderer->GetActiveCam()->GetAbsPos();
	float dist = GetAbsPos().distTo( pRenderer->GetActiveCam()->GetAbsPos() );
	//float distSq = dist*dist;
	const Vector3f *nearVerts = pRenderer->GetActiveCam()->GetFrustum()->GetNearPlaneVerts();

	bool bCamInsideLight = dist < fMaxDist;
	bool bVeryLargeLight = fMaxDist*2.0f > pRenderer->GetFarClippingDist(); //is the light distance larger than the far-clipping dist?
	bool bNearPlaneIntersect = IsNearPlaneIntersect( dist, fMaxDist, nearVerts, camPos );

	lightGeom->SetAbsPos(this->GetAbsPos());
	lightGeom->SetScale(fMaxDist * (bCamInsideLight ? 1.04f : 0.96f));

	RenderGeometry( lightGeom, debug, bCamInsideLight, bNearPlaneIntersect, bVeryLargeLight );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CPointLight::IsNearPlaneIntersect( float dist, float maxDist, const Vector3f *nearVerts, const Vector3f &camPos )
{
	bool bNearPlaneIntersect = false;
	if( dist < maxDist+3.0f )
	{
		float maxDistSq = maxDist*maxDist;
		bool bFirstVertInside = (nearVerts[0]+camPos).distToSq(GetAbsPos()) < maxDistSq;
		for( int i = 1; i < 4; i++ )
		{
			if( bFirstVertInside != ((nearVerts[i]+camPos).distToSq(GetAbsPos()) < maxDistSq) )
			{
				bNearPlaneIntersect = true;
				break;
			}
		}
	}
	return bNearPlaneIntersect;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPointLight::OnSpawn()
{
	BaseClass::OnSpawn();
	g_RenderInterf->GetRenderer()->GetLightList()->AddPointLight(this);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSunLight::OnSpawn()
{
	BaseClass::OnSpawn();
	g_RenderInterf->GetRenderer()->GetLightList()->AddSunLight(this);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSpotTexLight::OnSpawn()
{
	BaseClass::OnSpawn();
	LoadTex();
	g_RenderInterf->GetRenderer()->GetLightList()->AddSpotTexLight(this);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSpotTexLight::Render( bool debug /*= false*/ )
{
	CRenderer *pRenderer = g_RenderInterf->GetRenderer();
	CBaseGeometry *geom = pRenderer->GetLightList()->GetUnitPyramid();

	if( !geom )
	{
		if( !debug )
		{
			glDisable( GL_DEPTH_TEST );
			draw_fullscreen_quad_no_tex();
			glEnable( GL_DEPTH_TEST );
		}
		return;
	}

	Vector3f scale;
	scale.x = TAN(GetFov() * 0.5f * GetAspect()) * GetFar();
	scale.y = TAN(GetFov() * 0.5f) * GetFar();
	scale.z = GetFar();

	geom->SetScale(scale);
	geom->SetAbsPos(GetAbsPos());
	geom->SetAbsAngles(GetAbsAngles());

	CViewFrustum frustum;
	frustum.CalcPerspectiveFrustumValues(GetFov(), GetAspect(), GetNear(), GetFar(), GetAbsPos(), GetAbsAngles().GetMatrix3().GetTranspose());
	const CCamera *cam = pRenderer->GetActiveCam();
	const CViewFrustum *viewFrustum = cam->GetFrustum();
	const Vector3f &camPos = cam->GetAbsPos();

	bool camInLight = frustum.VertInFrustum(camPos);
	bool nearPlaneIntersect = false;
	for(int i = 0; i < 4; i++)
	{
		if(frustum.VertInFrustum(viewFrustum->GetNearPlaneVerts()[i] + cam->GetAbsPos()))
		{
			nearPlaneIntersect = true;
			break;
		}
	}

	RenderGeometry(geom, debug, camInLight, nearPlaneIntersect, false);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSpotTexLight::LoadTex()
{
	texture_t tex;
	g_RenderInterf->GetTextureManager()->LoadTexture(m_texFilename.c_str(), tex);
	m_texIndex = tex.index;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLightList::RemoveLight( int index )
{
	for( UINT i = 0; i < m_SpotLights.size(); i++ )
		if( m_SpotLights[i]->GetLightIndex() == index )
		{
			m_SpotLights.erase( m_SpotLights.begin() + i );
			return;
		}

	for( UINT i = 0; i < m_PointLights.size(); i++ )
		if( m_PointLights[i]->GetLightIndex() == index )
		{
			m_PointLights.erase( m_PointLights.begin() + i );
			return;
		}

	for( UINT i = 0; i < m_SunLights.size(); i++ )
		if( m_SunLights[i]->GetLightIndex() == index )
		{
			m_SunLights.erase( m_SunLights.begin() + i );
			return;
		}

	for( UINT i = 0; i < m_SpotTexLights.size(); i++ )
		if( m_SpotTexLights[i]->GetLightIndex() == index )
		{
			m_SpotTexLights.erase( m_SpotTexLights.begin() + i );
			return;
		}

	AssertMsg(false, "Did not find the light with specified index...");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CPointLight * CLightList::GetPointLight( int index )
{ 
	for( UINT i = 0; i < m_PointLights.size(); i++ )
		if( m_PointLights[i]->GetLightIndex() == index )
			return m_PointLights[i];
	return NULL;
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CSpotLight * CLightList::GetSpotLight( int index )
{ 
	for( UINT i = 0; i < m_SpotLights.size(); i++ )
		if( m_SpotLights[i]->GetLightIndex() == index )
			return m_SpotLights[i];
	return NULL;
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CSunLight * CLightList::GetSunLight( int index )
{ 
	for( UINT i = 0; i < m_SunLights.size(); i++ )
		if( m_SunLights[i]->GetLightIndex() == index )
			return m_SunLights[i];
	return NULL;
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CSpotTexLight * CLightList::GetSpotTexLight( int index )
{
	for( UINT i = 0; i < m_SpotLights.size(); i++ )
		if( m_SpotTexLights[i]->GetLightIndex() == index )
			return m_SpotTexLights[i];
	return NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLightList::Init( void )
{
	m_unitSphere = new CBaseGeometry(-1);
	if( !m_unitSphere->LoadGeometry( "models/sphere_12x12.rpm", "Unlit" ) )
	{
		error("Could not find point-light-geometry. Expect performance drops." );
		delete m_unitSphere;
		m_unitSphere = NULL;
	}

	m_unitCone = new CBaseGeometry(-1);
	if( !m_unitCone->LoadGeometry( "models/unitcone.rpm", "Unlit" ) )
	{
		error( "Could not find spot-light-geometry. Expect performance drops." );
		delete m_unitCone;
		m_unitCone = NULL;
	}

	m_unitPyramid = new CBaseGeometry(-1);
	if( !m_unitPyramid->LoadGeometry( "models/unitpyramid.rpm", "Unlit" ) )
	{
		error( "Could not find unit-pyramid-geometry. Expect performance drops." );
		delete m_unitPyramid;
		m_unitPyramid = NULL;
	}
}
