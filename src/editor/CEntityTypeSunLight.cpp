// editor/CEntityTypeSunLight.cpp
// editor/CEntityTypeSunLight.cpp
// editor/CEntityTypeSunLight.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "IEditorEntityFactory.h"
#include "CEntityTypeSunLight.h"
#include <util/StringUtils.h>

DECLARE_EDITOR_ENTITY_TYPE("SUNLIGHT", CEntityTypeSunLight)


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSunLight::OnCreate( void )
{
	CSunLight *light = new CSunLight(-1);
	m_lightIndex = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->AddSunLight( light );
	UpdateLightParams();

	BaseClass::OnCreate();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSunLight::OnDestroy( void )
{
	GetRenderInterfaces()->GetRendererInterf()->GetLightList()->RemoveLight(m_lightIndex);

	BaseClass::OnDestroy();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSunLight::OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval )
{
	BaseClass::OnParamChanged(key,oldval,newval);
	UpdateLightParams();

	if( key.compare("angles") == 0 )
	{
		Vector3f dir;
		AngleToVector(GetAbsAngles(), dir);
		char vec[128];
		sprintf_s( vec, "%.3f %.3f %.3f", dir.x, dir.y, dir.z );
		m_keyvals["direction"] = string(vec);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSunLight::UpdateLightParams( void )
{
	Vector4f color = StrUtils::GetVector4( GetValue("color") );
	color.x /= 255.0f;
	color.y /= 255.0f;
	color.z /= 255.0f;
	Vector4f specularColor = StrUtils::GetVector4( GetValue("specularColor") );
	specularColor.x /= 255.0f;
	specularColor.y /= 255.0f;
	specularColor.z /= 255.0f;
	Vector3f ambient1 = StrUtils::GetVector3( GetValue("ambientMin") ) / 255.0f;
	Vector3f ambient2 = StrUtils::GetVector3( GetValue("ambientMax") ) / 255.0f;

	Vector3f dir;
	AngleToVector(GetAbsAngles(), dir);

	CSunLight *light = GetRenderInterfaces()->GetRendererInterf()->GetLightList()->GetSunLight(m_lightIndex);
	light->SetDirection(dir.Normalize());
	light->SetColor(color);
	light->SetSpecular(specularColor);
	light->SetAmbient1(ambient1);
	light->SetAmbient2(ambient2);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeSunLight::Render( void )
{
	BaseClass::Render();

	if( IsSelected() )
	{
		Vector3f dir;
		AngleToVector(GetAbsAngles(), dir);
		draw_line(GetAbsCenter(), GetAbsCenter()+dir*10.0f);
	}
}




