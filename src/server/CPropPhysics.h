// server/CPropPhysics.h
// server/CPropPhysics.h
// server/CPropPhysics.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__server__CPropPhysics_H__
#define deferred__server__CPropPhysics_H__

#include "CStaticModel.h"
#include <util/datadesc.h>

class CPropPhysics : public CStaticModel
{
	DECLARE_DATADESC_EDITOR(CStaticModel, CPropPhysics)
	DECLARE_ENTITY_LINKED()

public:
	CPropPhysics();
	void LoadModel();
	void SetOriginAndUpdatePhysics( const Vector3f &pos );
	void SetAnglesAndUpdatePhysics( const Angle3d &ang );
	void SetAnglesAndOriginAndUpdatePhysics( const Vector3f &pos, const Angle3d &ang );
	void Destroy( void );
	void Update( void );
	void OnDatadescFinished(void);
	void InputTest(const inputdata_t<CBaseEntity> &data);
	bool IsStatic() const { return m_static; }

private:
	bool m_static;

};

#endif
