// renderer/CRenderEntity.h
// renderer/CRenderEntity.h
// renderer/CRenderEntity.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CRenderEntity_H__
#define deferred__renderer__CRenderEntity_H__

#include <util/dll.h>
#include <util/maths.h>
#include <util/IEvent.h>
#include "Interfaces.h"
#include "CRenderParams.h"
#include "datadesc_renderer.h"

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING
//pure virtual CRenderEntity Class
//this is used by the renderer.

class CRenderEntity;
class DLL IInterpolatedValue
{
public:
	virtual void Interpolate(float alpha) = 0;
	virtual void Update() = 0;
	virtual void *GetNewMember() const = 0;
};

template<typename T>
class CInterpolatedValue : public IInterpolatedValue
{
public:
	CInterpolatedValue(CRenderEntity *parent, T *val) {
		m_old = m_new = *val;
		m_val = val;
		m_initialized = false;
	}
	
	inline void Update() {
		if( m_initialized )
		{
			m_old = m_new;
			m_new = m_newest;
		}
		else
		{
			m_old = m_new = m_newest;
			m_initialized = true;
		}
	}

	inline void Interpolate(float alpha) {
		*m_val = m_old * (1.0f - alpha) + m_new * alpha;
	}

	inline void *GetNewMember() const {
		return (void*)&m_newest;
	}

private:
	T *m_val;
	T m_newest;
	T m_new;
	T m_old;
	bool m_initialized;
};


template<>
inline void CInterpolatedValue<Quaternion>::Interpolate(float alpha)
{
	QuaternionSlerp(m_old, m_new, alpha, *m_val);
}


class DLL CRenderEntity
{
	DECLARE_DATADESC_EDITOR_NOBASE(CRenderEntity)

public: 
	CRenderEntity( int index = -1 );
	virtual ~CRenderEntity() {}

	//Event system
	virtual void UpdateInterpolation( const float &alpha );

	//Called after this entity was created from a script (should not happen atm)
	virtual void OnDatadescFinished() { }

	//called after the first network-table-data is received, so networked vars are valid
	virtual void OnSpawn( void ) { SetNetorkDataInitialized(true); }

	//called when at least one network-table-data has changed
	virtual void OnDataUpdate( void ) { }

	//Positioning
	Vector3f const &GetAbsPos( void ) const { return m_vAbsPos; }
	Quaternion const &GetAbsAngles( void ) const { return m_aAbsAngles; }
	inline void SetAbsPos( const Vector3f &pos ) { m_vOldPos = m_vAbsPos = pos; }
	inline void SetAbsAngles( const Quaternion &ang ) { m_aOldAngles = m_aAbsAngles = ang; }

	//Velocity is calculated automatically from position-changes inside UpdateInterpolation()
	inline Vector3f const &GetVelocity( void ) const { return m_vVelocity; }

	//Visibility
	void SetVisible( const bool &bVis ) { m_bIsVisible = bVis; }
	bool const &IsVisible( void ) const { return m_bIsVisible; }

	//Indexing
	inline int GetIndex( void ) const { return m_iIndex; }

	//Pure Virtual Render Method.
	virtual void VRender( const CRenderParams &params ) = 0;

	//Called on Resolution change / window change
	virtual void VOnRestore( void ) = 0;
	
	//Virtual delete Method.
	virtual void VPreDelete( void ) = 0;

	virtual const char *GetEntityClass() const { return NULL; }

	bool DeserializeNetworkChanges(InStream &is, UINT size, bool interpMode);

	inline bool IsNetworkDataInitialized() { return m_bNetworkDataInitialized; }
	inline void SetNetorkDataInitialized(bool init) { m_bNetworkDataInitialized = init; }
	
	const char *GetName() { return m_name.c_str(); }

private:
	Vector3f m_vAbsPos;
	Vector3f m_vOldPos;
	Vector3f m_vVelocity;
	Quaternion m_aAbsAngles;
	Quaternion m_aOldAngles;
	const int m_iIndex;
	bool m_bIsVisible;
	std::string m_name;

private:
	void InitInterpolators();
	std::vector<IInterpolatedValue*> m_interpValues;
	bool m_bNetworkDataInitialized;
};


POP_PRAGMA_WARNINGS

#endif
