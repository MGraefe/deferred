// editor/CEntityTypeSpotLight.cpp
// editor/CEntityTypeSpotLight.cpp
// editor/CEntityTypeSpotLight.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "IEditorEntityFactory.h"
#include "CEntityTypeSpotLight.h"
#include <util/StringUtils.h>

DECLARE_EDITOR_ENTITY_TYPE("SPOTLIGHT", CEntityTypeSpotLight)


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSpotLight::OnCreate( void )
{
	CSpotLight *light = new CSpotLight(-1);
	m_lightIndex = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->AddSpotLight( light );
	UpdateLightParams();

	BaseClass::OnCreate();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSpotLight::OnDestroy( void )
{
	GetRenderInterfaces()->GetRendererInterf()->GetLightList()->RemoveLight(m_lightIndex);

	BaseClass::OnDestroy();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSpotLight::OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval )
{
	BaseClass::OnParamChanged(key,oldval,newval);
	UpdateLightParams();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSpotLight::UpdateLightParams( void )
{
	Vector3f constLinQuad = StrUtils::GetVector3( GetValue("constlinquad") ).Normalize();
	Vector4f color = StrUtils::GetVector4( GetValue("color") );
	float degrees = StrUtils::GetFloat( GetValue("degrees") );
	float maxDist = StrUtils::GetFloat( GetValue("maxdist") );
	color.x /= 255.0f;
	color.y /= 255.0f;
	color.z /= 255.0f;
	CSpotLight *light = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->GetSpotLight(m_lightIndex);
	light->SetColor( color );
	light->SetConstant( constLinQuad.x );
	light->SetLinear( constLinQuad.y );
	light->SetQuadratic( constLinQuad.z );
	light->SetDegrees( degrees );
	light->SetAbsPos( GetAbsCenter() );
	light->SetAbsAngles( Quaternion(GetAbsAngles()) );
	light->SetMaxDist(maxDist);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSpotLight::Render( void )
{
	BaseClass::Render();

	if( IsSelected() )
	{
		CBaseGeometry *geom = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->GetUnitCone();
		CSpotLight *light = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->GetSpotLight(m_lightIndex);

		//calculate light geometry scales
		float fMaxDist = light->GetDist();
		Vector3f scale;
		scale.z = fMaxDist;
		scale.x = scale.y = TAN(light->GetDegrees()*0.5f) * fMaxDist;
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




