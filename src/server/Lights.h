// server/Lights.h
// server/Lights.h
// server/Lights.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__server__Lights_H__
#define deferred__server__Lights_H__

#include "CBaseEntity.h"
#include "datadesc_server.h"

class CServerLight : public CBaseEntity
{
	DECLARE_DATADESC_EDITOR(CBaseEntity, CServerLight)
public:
	CServerLight() { 
		m_Params->constant = 0.0f;
		m_Params->linear = 3.0f;
		m_Params->quadratic = 2.0f;
		m_maxDist = -1.0f;
		m_enabled = true;
	}

	const lightparams_t &GetParams( void ) const { return m_Params; }
	void SetParams( const lightparams_t &params ) { m_Params = params; }

	inline void SetConstant( float constant ) { m_Params->constant = constant; }
	inline void SetLinear( float linear ) { m_Params->linear = linear; }
	inline void SetQuadratic( float quad ) { m_Params->quadratic = quad; }
	inline void SetBrightness( float brightness ) { m_vColor->w = brightness; }

	float GetMaxDist( void ) const { return m_maxDist; }
	void SetMaxDist(float maxDist) { m_maxDist = maxDist; }
	inline float GetConstant( void ) const { return m_Params->constant; }
	inline float GetLinear( void ) const { return m_Params->linear; }
	inline float GetQuadratic( void ) const { return m_Params->quadratic; }
	inline float GetBrightness( void ) const { return m_vColor->w; }

	inline const Vector4f &GetColor( void ) const { return m_vColor; }
	void SetColor( const Vector4f &col ) { m_vColor = col; }

	void SetEnabled(bool enabled = true);
	bool IsEnabled() const;

	void InputTurnOn(const inputdata_t<CBaseEntity> &data);
	void InputTurnOff(const inputdata_t<CBaseEntity> &data);
	void InputToggle(const inputdata_t<CBaseEntity> &data);

private:
	NetVar( Vector4f, m_vColor );
	NetVar( float, m_maxDist );
	NetVar( lightparams_t, m_Params );
	NetVar( bool, m_enabled );
};


class CServerSunLight : public CBaseEntity
{
	DECLARE_DATADESC_EDITOR(CBaseEntity, CServerSunLight)
	DECLARE_ENTITY_LINKED()
public:
	CServerSunLight() :
		m_direction(0, -sqrt(2.0f), -sqrt(2.0f)),
		m_color(Vector4f(1, 1, 1, 1)),
		m_ambient1(0.2f, 0.2f, 0.2f),
		m_ambient2(0.7f, 0.7f, 0.7f)
	{ }

	const Vector3f &GetDirection( void ) const { return m_direction; }
	const Vector4f &GetColor(void) const { return m_color; }
	const Vector4f &GetSpecular(void) const { return m_specular; }
	const Vector3f &GetAmbient1(void) const { return m_ambient1; }
	const Vector3f &GetAmbient2(void) const { return m_ambient2; }
	void SetDirection( const Vector3f &dir ) { m_direction = dir; }
	void SetColor( const Vector4f &color ) { m_color = color; }
	void SetSpecular( const Vector4f &spec ) { m_specular = spec; }
	void SetAmbient1( const Vector3f &amb1 ) { m_ambient1 = amb1; }
	void SetAmbient2( const Vector3f &amb2 ) { m_ambient2 = amb2; }

private:
	NetVarVector3f( m_direction );
	NetVar( Vector4f, m_color ); //r g b brightness-factor
	NetVarVector3f( m_ambient1 ); //for minimum sky visibility
	NetVarVector3f( m_ambient2 ); //for maximum sky visibility
	NetVar( Vector4f, m_specular );
};


class CServerPointLight : public CServerLight
{
	DECLARE_DATADESC_EDITOR(CServerLight, CServerPointLight)
	DECLARE_ENTITY_LINKED()
};


class CServerSpotLight : public CServerLight
{
	DECLARE_DATADESC_EDITOR(CServerLight, CServerSpotLight)
	DECLARE_ENTITY_LINKED()
public:
	float GetDegrees( void ) const { return m_fSpotDegrees; }
	void SetDegrees( float deg ) { m_fSpotDegrees = deg; }
	float GetExponent( void ) const { return m_fExponent; }
	void SetExponent( float exp ) { m_fExponent = exp; }
private:
	NetVar( float, m_fSpotDegrees );
	NetVar( float, m_fExponent );
};


class CServerSpotTexLight : public CServerLight
{
	DECLARE_DATADESC_EDITOR(CServerLight, CServerSpotTexLight)
	DECLARE_ENTITY_LINKED()
public:
	float GetFov() const { return m_fov; }
	void SetFov(float fov) { m_fov = fov; }
	float GetAspect() const { return m_aspect; }
	void SetAspect(float aspect) { m_aspect = aspect; }
	const std::string &GetTexFilename() const { return m_texFilename; }
	void SetTexFilename(const std::string &texFilename) { m_texFilename = texFilename; }

private:
	NetVar( float, m_fov );
	NetVar( float, m_aspect );
	NetVar( std::string, m_texFilename );
};

#endif // deferred__server__Lights_H__