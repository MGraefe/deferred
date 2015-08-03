// server/CViewFlyCam.h
// server/CViewFlyCam.h
// server/CViewFlyCam.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once
#ifndef deferred__server__CViewFlyCam_H__
#define deferred__server__CViewFlyCam_H__

#include "CBaseEntity.h"
#include "IEvent.h"
#include "maths.h"

class CViewFlyCam : public CBaseEntity, public IEventListener
{
public:
	CViewFlyCam();
	void Update( void );
	void Spawn( void );
	
	void SetDebugCam( bool bDebug ) { m_bIsDebugCam = bDebug; }

	bool HandleEvent( IEvent const &Event );

private:
	void CalculateLookPosAndDir( void );

private:
	Vector3d m_aLookDir;
	Vector3d m_vLookDir;
	Vector3d m_vLookRight;
	float m_fSpeedForward;
	float m_fSpeedSideward;
	float m_fMoveLR;
	float m_fMoveFB;
	Vector2d m_vMouseMove;
	bool m_bIsDebugCam;
};

#endif
