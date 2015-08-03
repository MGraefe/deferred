// renderer/CLightList.h
// renderer/CLightList.h
// renderer/CLightList.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__CLightList_H__
#define deferred__renderer__CLightList_H__

#include <util/basic_types.h>
#include "Interfaces.h"
#include <util/dll.h>
#include "CRenderEntity.h"
#include <util/datadesc.h>


PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING

class CLightList;
class CBaseGeometry;

class DLL CLight : public CRenderEntity
{
	DECLARE_DATADESC_EDITOR(CRenderEntity, CLight)
public:
	friend class CLightList;

	CLight( int index = -1 ) : BaseClass(index) { 
		m_Params.constant = 0.0f;
		m_Params.linear = 3.0f;
		m_Params.quadratic = 2.0f;
		m_maxDist = -1.0f;
		m_lightIndex = -1;
		m_enabled = true;
		UpdateDist();
	}

	void VRender( const CRenderParams &params ) { }
	void VOnRestore( void ) { }
	void VPreDelete( void ) { }

	virtual void Render( bool debug = false ) { return; }
	const lightparams_t &GetParams( void ) const { return m_Params; }
	void SetParams( const lightparams_t &params ) { m_Params = params; UpdateDist(); }

	inline void SetConstant( float constant ) { m_Params.constant = constant; UpdateDist(); }
	inline void SetLinear( float linear ) { m_Params.linear = linear; UpdateDist(); }
	inline void SetQuadratic( float quad ) { m_Params.quadratic = quad; UpdateDist(); }
	inline void SetBrightness( float brightness ) { m_vColor.w = brightness; UpdateDist(); }
	//void SetDist( float dist ) { m_Params.dist = dist; }
	void UpdateDist( void );
	float GetDist( void ) const { return m_maxDist < 0.0f ? m_Params.dist : min(m_maxDist, m_Params.dist); }
	float GetMaxDist( void ) const { return m_maxDist; }
	void SetMaxDist(float maxDist) { m_maxDist = maxDist; }
	inline float GetConstant( void ) const { return m_Params.constant; }
	inline float GetLinear( void ) const { return m_Params.linear; }
	inline float GetQuadratic( void ) const { return m_Params.quadratic; }
	inline float GetBrightness( void ) const { return m_vColor.w; }

	inline const Vector4f &GetColor( void ) const { return m_vColor; }
	void SetColor( const Vector4f &col ) { m_vColor = col; UpdateDist(); }
	int GetLightIndex( void ) { return m_lightIndex; }

	void OnSpawn( void ) { UpdateDist(); }

	bool IsEnabled() const { return m_enabled; }
	void SetEnabled(bool enabled = true) { m_enabled = enabled; }

protected:
	void RenderGeometry( CBaseGeometry *geom, bool debug, bool camInsideLight, bool nearPlaneIntersect, bool largeLight );

private:
	void SetLightIndex( int index ) { m_lightIndex = index; }
	Vector4f m_vColor;
	float m_maxDist;
	lightparams_t m_Params;
	int m_lightIndex;
	bool m_enabled;
};


class DLL CSunLight : public CRenderEntity
{
	DECLARE_DATADESC_EDITOR(CRenderEntity, CSunLight)
	DECLARE_ENTITY_LINKED()
public:
	CSunLight(int index) : CRenderEntity(index),
		m_direction(0, -sqrt(2.0f), -sqrt(2.0f)),
		m_color(1, 1, 1, 1),
		m_ambient1(0.2f, 0.2f, 0.2f),
		m_ambient2(0.7f, 0.7f, 0.7f),
		m_lightIndex(-1),
		m_enabled(true)
	{ }

	void VRender( const CRenderParams &params ) { }
	void VOnRestore( void ) { }
	void VPreDelete( void ) { }
	void OnSpawn();
	const Vector3f &GetDirection( void ) const { return m_direction; }
	const Vector4f &GetColor(void) const { return m_color; }
	const Vector4f &GetSpecular(void) const { return m_specular; }
	const Vector3f &GetAmbient1(void) const { return m_ambient1; }
	const Vector3f &GetAmbient2(void) const { return m_ambient2; }
	int GetLightIndex(void) const { return m_lightIndex; }
	void SetDirection( const Vector3f &dir ) { m_direction = dir; }
	void SetColor( const Vector4f &color ) { m_color = color; }
	void SetSpecular( const Vector4f &spec ) { m_specular = spec; }
	void SetAmbient1( const Vector3f &amb1 ) { m_ambient1 = amb1; }
	void SetAmbient2( const Vector3f &amb2 ) { m_ambient2 = amb2; }
	void SetLightIndex(int index) { m_lightIndex = index; }

	bool IsEnabled() const { return m_enabled; }
	void SetEnabled(bool enabled = true) { m_enabled = enabled; }
private:
	Vector3f m_direction;
	Vector4f m_color; //r g b brightness-factor
	Vector3f m_ambient1; //for minimum sky visibility
	Vector3f m_ambient2; //for maximum sky visibility
	Vector4f m_specular;
	int m_lightIndex;
	bool m_enabled;
};


class DLL CPointLight : public CLight
{
	DECLARE_DATADESC_EDITOR(CLight, CPointLight)
	DECLARE_ENTITY_LINKED()
public:
	CPointLight( int index = -1 ) : BaseClass(index) {}
	void Render( bool debug = false );
	bool IsNearPlaneIntersect( float dist, float maxDist, const Vector3f *nearVerts, const Vector3f &camPos );
	void OnSpawn();
};


class DLL CSpotLight : public CLight
{
	DECLARE_DATADESC_EDITOR(CLight, CSpotLight)
	DECLARE_ENTITY_LINKED()
public:
	CSpotLight( int index = -1 ) : BaseClass(index) { }
	void Render( bool debug = false );

	bool IsNearPlaneIntersect( float dist, float fMaxDist, float minDot,
		const Vector3f &lightDir, const Vector3f *nearVerts, const Vector3f &camPos );

	float GetDegrees( void ) const { return m_fSpotDegrees; }
	void SetDegrees( float deg ) { m_fSpotDegrees = deg; }
	float GetExponent( void ) const { return m_fExponent; }
	void SetExponent( float exp ) { m_fExponent = exp; }
	void OnSpawn();

private:
	float m_fSpotDegrees;
	float m_fExponent;
};



class DLL CSpotTexLight : public CLight
{
	DECLARE_DATADESC_EDITOR(CLight, CSpotTexLight)
	DECLARE_ENTITY_LINKED()

public:
	CSpotTexLight( int index = -1 ) : BaseClass(index) {}
	void Render( bool debug = false );
	void OnSpawn();
	void LoadTex();

	float GetFov() const { return m_fov; }
	void SetFov(float fov) { m_fov = fov; }
	float GetAspect() const { return m_aspect; }
	void SetAspect(float aspect) { m_aspect = aspect; }
	float GetNear() const { return 1.0f; }
	float GetFar() const { return 1000.0f; }
	UINT GetTexIndex() const { return m_texIndex; }
	void SetTexIndex( UINT texIndex ) { m_texIndex = texIndex; }
	void SetTexFilename( const std::string &filename ) { m_texFilename = filename; }
	const std::string &GetTexFilename() const { return m_texFilename; }

private:
	float m_fov;
	float m_aspect;
	std::string m_texFilename;
	UINT m_texIndex;
};


class DLL CLightList
{
public:
	CLightList() : m_indexCounter(0), m_unitSphere(NULL) { }

	void Init( void );

	int AddSpotLight( CSpotLight *SpotLight ) {
		m_SpotLights.push_back( SpotLight );
		m_SpotLights.back()->SetLightIndex(++m_indexCounter);
		return m_indexCounter;
	}

	int AddPointLight( CPointLight *PointLight ) {
		m_PointLights.push_back( PointLight );
		m_PointLights.back()->SetLightIndex(++m_indexCounter);
		return m_indexCounter;
	}

	int AddSunLight( CSunLight *SunLight ) {
		m_SunLights.push_back( SunLight );
		m_SunLights.back()->SetLightIndex(++m_indexCounter);
		return m_indexCounter;
	}

	int AddSpotTexLight( CSpotTexLight *SpotTexLight ) {
		m_SpotTexLights.push_back( SpotTexLight );
		m_SpotTexLights.back()->SetLightIndex(++m_indexCounter);
		return m_indexCounter;
	}

	//These could use some sort of light culling since these data get directly uploaded to the shader.
	CPointLight *GetPointLightByArrayIndex( int i ) { return m_PointLights[i]; }
	CSpotLight *GetSpotLightByArrayIndex( int i ) { return m_SpotLights[i]; }
	CSunLight *GetSunLightByArrayIndex( int i ) { return m_SunLights[i]; }
	CSpotTexLight *GetSpotTexLightByArrayIndex( int i ) { return m_SpotTexLights[i]; }

	int GetNumPointLights( void ) { return m_PointLights.size(); }
	int GetNumSpotLights( void ) { return m_SpotLights.size(); }
	int GetNumSunLights( void ) { return m_SunLights.size(); }
	int GetNumSpotTexLights( void ) { return m_SpotTexLights.size(); }

	CPointLight *GetPointLight( int index );
	CSpotLight *GetSpotLight( int index );
	CSunLight *GetSunLight( int index );
	CSpotTexLight *GetSpotTexLight( int index );

	void RemoveLight( int index );

	CBaseGeometry *GetUnitSphere(void) { return m_unitSphere; }
	CBaseGeometry *GetUnitCone(void) { return m_unitCone; }
	CBaseGeometry *GetUnitPyramid(void) { return m_unitPyramid; }
private:
	std::vector<CPointLight*> m_PointLights;
	std::vector<CSpotLight*> m_SpotLights;
	std::vector<CSunLight*> m_SunLights; //This is usually only one.
	std::vector<CSpotTexLight*> m_SpotTexLights;

	CBaseGeometry *m_unitSphere;
	CBaseGeometry *m_unitCone;
	CBaseGeometry *m_unitPyramid;
	int m_indexCounter;
};

POP_PRAGMA_WARNINGS

#endif
