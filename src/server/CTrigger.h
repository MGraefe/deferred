// server/CTrigger.h
// server/CTrigger.h
// server/CTrigger.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CTrigger.h

#pragma once
#ifndef deferred__server__CTrigger_H__
#define deferred__server__CTrigger_H__

#include "CBaseEntity.h"

class CTrigger : public CBaseEntity
{
	DECLARE_DATADESC_EDITOR(CBaseEntity, CTrigger)
	DECLARE_ENTITY_LINKED()

public:
	CTrigger();

	virtual void OnDatadescFinished( void );
	virtual void OnCollisionStart(const CCollisionInfo &info);
	virtual void Destroy( void );

	//void SetVerts( Vector3f *verts8 );

private:
	//Vector3f m_verts[8];
	float m_sizex;
	float m_sizey;
	float m_sizez;
};


#endif // deferred__server__CTrigger_H__
