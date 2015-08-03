// editor/CEntityTypeLight.cpp
// editor/CEntityTypeLight.cpp
// editor/CEntityTypeLight.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "IEditorEntityFactory.h"
#include "CEntityTypeLight.h"
#include <util/StringUtils.h>

DECLARE_EDITOR_ENTITY_TYPE("LIGHT", CEntityTypeLight)


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeLight::OnCreate( void )
{
	CPointLight *light = new CPointLight(-1);
	m_lightIndex = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->AddPointLight( light );
	UpdateLightParams();

	BaseClass::OnCreate();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeLight::OnDestroy( void )
{
	GetRenderInterfaces()->GetRendererInterf()->GetLightList()->RemoveLight(m_lightIndex);

	BaseClass::OnDestroy();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeLight::OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval )
{
	BaseClass::OnParamChanged(key,oldval,newval);
	UpdateLightParams();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeLight::UpdateLightParams( void )
{
	Vector3f constLinQuad = StrUtils::GetVector3( GetValue("constlinquad") ).Normalize();
	Vector4f color = StrUtils::GetVector4( GetValue("color") );
	float maxDist = StrUtils::GetFloat( GetValue("maxdist") );
	color.x /= 255.0f;
	color.y /= 255.0f;
	color.z /= 255.0f;
	CPointLight *light = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->GetPointLight(m_lightIndex);
	light->SetColor( color );
	light->SetConstant( constLinQuad.x );
	light->SetLinear( constLinQuad.y );
	light->SetQuadratic( constLinQuad.z );
	light->SetAbsPos( GetAbsCenter() );
	light->SetMaxDist( maxDist );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeLight::Render( void )
{
	BaseClass::Render();

	if( IsSelected() )
	{
		CBaseGeometry *geom = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->GetUnitSphere();
		CPointLight *light = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->GetPointLight(m_lightIndex);

		//calculate light geometry scales
		float fMaxDist = light->GetDist();
		geom->SetScale(fMaxDist);
		geom->SetAbsPos(GetAbsCenter());
		geom->SetAbsAngles(GetAbsAngles());

		CShader *pShd = GetShaderManager()->GetActiveShader();
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		GetGLStateSaver()->Disable(GL_CULL_FACE);
		pShd->EnableTexturing(false);
		pShd->SetDrawColor(1, 1, 0, 1);
		geom->RenderNoTextureBufferOnly(false);
		pShd->SetDrawColor(1, 1, 1, 1);
		GetGLStateSaver()->Enable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
