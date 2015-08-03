// server/Lights.cpp
// server/Lights.cpp
// server/Lights.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------



#include "stdafx.h"

#include "Lights.h"


BEGIN_DATADESC_EDITOR(CServerLight)
	FIELD_COLOR4( "color", m_vColor )
	FIELD_VECTOR3_NORMALIZED( "constlinquad", m_Params )
	FIELD_FLOAT( "maxdist", m_maxDist )
	FIELD_BOOL( "enabled", m_enabled )

	FIELD_INPUTFUNC("TurnOn", InputTurnOn)
	FIELD_INPUTFUNC("TurnOff", InputTurnOff)
	FIELD_INPUTFUNC("Toggle", InputToggle)

	NETTABLE_ENTRY(m_vColor)
	NETTABLE_ENTRY(m_Params)
	NETTABLE_ENTRY(m_maxDist)
	NETTABLE_ENTRY(m_enabled);
END_DATADESC_EDITOR()


LINK_ENTITY_TO_CLASSNAME_SERVER(CServerPointLight, light)
BEGIN_DATADESC_EDITOR(CServerPointLight)
END_DATADESC_EDITOR()


LINK_ENTITY_TO_CLASSNAME_SERVER(CServerSpotLight, light_spot)
BEGIN_DATADESC_EDITOR(CServerSpotLight)
	FIELD_FLOAT( "degrees", m_fSpotDegrees )
	FIELD_FLOAT( "exponent", m_fExponent )

	NETTABLE_ENTRY(m_fSpotDegrees)
	NETTABLE_ENTRY(m_fExponent)
END_DATADESC_EDITOR()


LINK_ENTITY_TO_CLASSNAME_SERVER(CServerSunLight, light_sun)
BEGIN_DATADESC_EDITOR(CServerSunLight)
	FIELD_VECTOR3_NORMALIZED( "direction", m_direction )
	FIELD_COLOR4( "color", m_color )
	FIELD_COLOR4( "specularColor", m_specular )
	FIELD_COLOR3( "ambientMin", m_ambient1 )
	FIELD_COLOR3( "ambientMax", m_ambient2 )

	NETTABLE_ENTRY(m_direction)
	NETTABLE_ENTRY(m_color)
	NETTABLE_ENTRY(m_specular)
	NETTABLE_ENTRY(m_ambient1)
	NETTABLE_ENTRY(m_ambient2)
END_DATADESC_EDITOR()


LINK_ENTITY_TO_CLASSNAME_SERVER(CServerSpotTexLight, light_spot_tex)
BEGIN_DATADESC_EDITOR(CServerSpotTexLight)
	FIELD_FLOAT( "fov", m_fov )
	FIELD_FLOAT( "aspect", m_aspect )
	FIELD_STRING( "filename", m_texFilename )

	NETTABLE_ENTRY(m_fov)
	NETTABLE_ENTRY(m_aspect)
	NETTABLE_ENTRY_STRING(m_texFilename)
END_DATADESC_EDITOR()


void CServerLight::InputTurnOn(const inputdata_t<CBaseEntity> &data)
{
	m_enabled = true;
}

void CServerLight::InputTurnOff(const inputdata_t<CBaseEntity> &data)
{
	m_enabled = false;
}

void CServerLight::InputToggle(const inputdata_t<CBaseEntity> &data)
{
	m_enabled = !m_enabled;
}

void CServerLight::SetEnabled(bool enabled /*= true*/)
{
	m_enabled = enabled;
}

bool CServerLight::IsEnabled() const
{
	return m_enabled;
}