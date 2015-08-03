// server/CPlayerCar.h
// server/CPlayerCar.h
// server/CPlayerCar.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__server__CPlayerCar_H__
#define deferred__server__CPlayerCar_H__

#include "CBaseEntity.h"
#include <util/IEvent.h>
#include "vehicleinfo_t.h"
#include "CStaticModel.h"
#include <util/CSmoother.h>

class CEventManager;
class CPhysVehicle;
class CStaticModel;
class CLogicCamera;

class CPlayerCar: public CStaticModel, public IEventListener
{
	DECLARE_DATADESC_EDITOR(CStaticModel, CPlayerCar)
	DECLARE_ENTITY_LINKED()
public:
	CPlayerCar();
	void Spawn( bool bPlayerOneCar, bool bPlayerTwoCar );
	void Update( void );
	void Think( void );
	void Destroy( void );
	bool HandleEvent( const IEvent *evt );
	void UnregisterEvents( void );
	void RegisterEvents( CEventManager *pMgr );
	void OnCollisionStart(const CCollisionInfo &info);

	void SetOriginAndUpdatePhysics( const Vector3f &pos );
	void SetAnglesAndUpdatePhysics( const Angle3d &ang );
	void SetAnglesAndOriginAndUpdatePhysics( const Vector3f &pos, const Angle3d &ang );

	void ResetIfNeeded( void );
	void Reset( void );
	bool HadResetLastFrame( void ) { return m_bJustResetted; }
	bool IsUnderWater( void );
	
	float GetAmountUnderWater(void);

	inline void SetPlayerID( int id ) { m_iPlayerID = id; }
	inline void SetClientID( int id ) { m_iClientID = id; }
	inline int  GetPlayerID( void ) const { return m_iPlayerID; }
	inline int	GetClientID( void ) const { return m_iClientID; }

	void SetLastCheckpointPos( const Vector3f &pos );

private:
	NetVar( int, m_iPlayerID );
	NetVar( int, m_iClientID );
	bool		m_bShouldReset;
	bool		m_bJustResetted;
	Vector3f	m_vResetPosition;
	Angle3d		m_aResetAngles;

	float		m_fSpeed;
	float		m_fLastSteeringAngle;

	Vector3f	vLastCamPosition;
	Vector3f	vLastCamDirection;

	Vector3f	m_vLastCheckpointPos;

	float		CalculateMotorRpm( void );
	float		CalculateMotorForceAtWheels( float fMotorRpm );

	NetVar( BYTE, m_engineGear );
	NetVar( BYTE, m_engineLoad );
	NetVar( USHORT, m_engineRpm );
	
	float		m_timeEnteredWater;
	bool		m_bWasUnderWater;
	
	float			m_fInputAccelerate; //-1 Break, 0 Do nothing, 1 Accelerate
	float			m_fInputSteer; //-1 Left, 0 Straight on, 1 Right
	float			m_fInputUpDown; //-1 Down, 0 nothing, 1 Up
	float			m_fInputHandbrake; // 1 for brake, 0 for not

	CSmoother<float> m_steeringSmoother;

	//CLogicCamera *m_pCam;
	CPhysVehicle *m_pPhysVehicle;
	CStaticModel *m_pWheels[4];
	vehicleinfo_t m_info;

	Vector3f	m_vForwardVelocity;
};


#endif
