// server/CPhysVehicle.h
// server/CPhysVehicle.h
// server/CPhysVehicle.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__server__CPhysVehicle_H__
#define deferred__server__CPhysVehicle_H__


#include "../../libs/bullet/bullet-2.77/src/btBulletCollisionCommon.h"
#include "../../libs/bullet/bullet-2.77/src/btBulletDynamicsCommon.h"
#include "vehicleinfo_t.h"
#include <util/maths.h>

class CBaseEntity;
class CPhysVehicle
{
	friend class CBulletPhysics;

private:
	CPhysVehicle( const CPhysVehicle &v ) { }
public:
	CPhysVehicle();
	~CPhysVehicle();
	void Create( CBulletPhysics *pPhysics, btDynamicsWorld *pDynamicsWorld, CBaseEntity *pEnt,
					CBaseEntity **pWheels, Vector3f *pVerts, const int iVerts, const vehicleinfo_t &info );
	void UpdateSteering( float fSteer ); //Ranges from -1 to 1
	void UpdateGasBreak( float fGas, float fBreak, float fHandBrake );
	bool HasGroundContact( int iWheelIndex );
	void Destroy( void );
	Vector3f &GetWheelPosition( const int i ) { return m_pWheelPos[i]; }
	Angle3d &GetWheelRotation( const int i ) { return m_pWheelRot[i]; }

private:
	CBaseEntity *m_pEnt;
	CBaseEntity *m_pWheels[4];
	vehicleinfo_t m_info;

	void Sync( void );

	btRaycastVehicle::btVehicleTuning	m_tuning;
	btVehicleRaycaster*	m_vehicleRayCaster;
	btRaycastVehicle*	m_vehicle;
	btCollisionShape*	m_wheelShape;
	btDynamicsWorld*	m_dynamicsWorld;
	btRigidBody*		m_carChassis;
	ActorMotionState	*m_actorMotionState;
	Vector3f			m_pWheelPos[4];
	Angle3d				m_pWheelRot[4];
};


#endif
