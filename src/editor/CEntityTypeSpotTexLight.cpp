// editor/CEntityTypeSpotTexLight.cpp
// editor/CEntityTypeSpotTexLight.cpp
// editor/CEntityTypeSpotTexLight.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "IEditorEntityFactory.h"
#include "CEntityTypeSpotTexLight.h"
#include <util/StringUtils.h>

DECLARE_EDITOR_ENTITY_TYPE("SPOTTEXLIGHT", CEntityTypeSpotTexLight)


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSpotTexLight::OnCreate( void )
{
	CSpotTexLight *light = new CSpotTexLight(-1);
	m_lightIndex = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->AddSpotTexLight( light );
	UpdateLightParams();

	BaseClass::OnCreate();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSpotTexLight::OnDestroy( void )
{
	GetRenderInterfaces()->GetRendererInterf()->GetLightList()->RemoveLight(m_lightIndex);

	BaseClass::OnDestroy();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSpotTexLight::OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval )
{
	BaseClass::OnParamChanged(key,oldval,newval);
	UpdateLightParams();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSpotTexLight::UpdateLightParams( void )
{
	Vector3f constLinQuad = StrUtils::GetVector3( GetValue("constlinquad") ).Normalize();
	Vector4f color = StrUtils::GetVector4( GetValue("color") );
	float fov = StrUtils::GetFloat( GetValue("fov") );
	float aspect = StrUtils::GetFloat( GetValue("aspect") );
	std::string filename = GetValue("filename");
	color.x /= 255.0f;
	color.y /= 255.0f;
	color.z /= 255.0f;
	CSpotTexLight *light = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->GetSpotTexLight(m_lightIndex);
	light->SetColor( color );
	light->SetConstant( constLinQuad.x );
	light->SetLinear( constLinQuad.y );
	light->SetQuadratic( constLinQuad.z );
	light->SetAbsPos( GetAbsCenter() );
	light->SetAbsAngles( Quaternion(GetAbsAngles()) );
	light->SetFov(fov);
	light->SetAspect(aspect);
	light->SetTexFilename(filename);
	light->LoadTex();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSpotTexLight::Render( void )
{
	BaseClass::Render();

	if( IsSelected() )
	{
		CBaseGeometry *geom = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->GetUnitPyramid();
		CSpotTexLight *light = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->GetSpotTexLight(m_lightIndex);

		Vector3f scale;
		scale.x = TAN(light->GetFov()*0.5f*light->GetAspect()) * light->GetFar();
		scale.y = TAN(light->GetFov()*0.5f) * light->GetFar();
		scale.z = light->GetFar();

		geom->SetScale(scale);
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




