// server/CPlayerCar.cpp
// server/CPlayerCar.cpp
// server/CPlayerCar.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include <util/basic_types.h>
#include "CPlayerCar.h"
#include <util/CEventManager.h>
#include "ServerInterfaces.h"
#include "CBulletPhysics.h"
#include "CPhysVehicle.h"
#include <util/CScriptParser.h>
#include <util/error.h>
#include "CEntityList.h"
#include "CStaticModel.h"
#include "CLogicCamera.h"
#include "CVehicleRespawnPosition.h"
#include "CTrigger.h"

typedef CNetVarWriteProxyCast<BYTE, int> ClientIdProxy;

LINK_ENTITY_TO_CLASSNAME_SERVER(CPlayerCar, player_car)
BEGIN_DATADESC_EDITOR(CPlayerCar)
	NETTABLE_ENTRY_PROXY(m_iClientID, ClientIdProxy);
	NETTABLE_ENTRY(m_engineRpm)
	NETTABLE_ENTRY(m_engineGear)
	NETTABLE_ENTRY(m_engineLoad)
END_DATADESC_EDITOR()

const float g_fWaterHeight = -50.0f; //TODO: put somewhere else
const bool g_bWaterEnabled = true;

CPlayerCar::CPlayerCar()
{
	m_fSpeed = 0;
	m_fLastSteeringAngle = 0;
	m_fInputAccelerate = 0.0f;
	m_fInputSteer = 0.0f;
	m_fInputHandbrake = 0.0f;
	m_bShouldReset = false;
	m_iPlayerID = 0;
	m_iClientID = -1;
	m_engineRpm = 0;
	m_engineGear = 1;
	m_engineLoad = 0;
	m_bJustResetted = false;
	m_timeEnteredWater = 0.0f;
	m_bWasUnderWater = false;
}


void GetNearestSpawnPosition( const Vector3f &vIn, Vector3f &vOut, Angle3d &aOut )
{
	std::vector<CVehicleRespawnPosition*> &poss = singletons::g_pWorld->getRespawnPositions();
	if( poss.empty() )
		return;

	UINT index = 0;
	float mindistsq = VectorLengthSq( poss[0]->GetOrigin() - vIn );

	for( UINT i = 1; i < poss.size(); i++ )
	{
		float length = VectorLengthSq( poss[i]->GetOrigin() - vIn );
		if( length < mindistsq )
		{
			mindistsq = length;
			index = i;
		}
	}

	vOut = poss[index]->GetOrigin();
	aOut = poss[index]->GetAngle();
}




//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPlayerCar::Destroy( void )
{
	if( m_pPhysVehicle )
	{
		CBulletPhysics *pPhys = (CBulletPhysics*)g_pPhysics;
		if( pPhys )
			pPhys->RemoveVehicle( m_pPhysVehicle );
		delete m_pPhysVehicle;
	}

	for( int i = 0; i < 4; i++ )
	{
		if( m_pWheels[i] )
			m_pWheels[i]->Destroy();
	}

	CBaseEntity::Destroy();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPlayerCar::SetOriginAndUpdatePhysics( const Vector3f &pos )
{
	SetOrigin( pos );
	g_pPhysics->VSetActorOrigin( GetIndex(), pos );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPlayerCar::SetAnglesAndUpdatePhysics( const Angle3d &ang )
{
	SetAngle( ang );
	g_pPhysics->VSetActorAngles( GetIndex(), ang );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPlayerCar::SetAnglesAndOriginAndUpdatePhysics( const Vector3f &pos, const Angle3d &ang )
{
	SetOrigin( pos );
	SetAngle( ang );
	g_pPhysics->VSetActorOriginAndAngles( GetIndex(), pos, ang );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPlayerCar::Reset( void )
{
	Vector3f vPos(vec3_null);
	Angle3d aRot(vec3_null);
	if(m_vLastCheckpointPos == vec3_null)
	{
		vPos = m_vResetPosition;
		aRot = m_aResetAngles;
	}
	else
		GetNearestSpawnPosition( m_vLastCheckpointPos, vPos, aRot );
	SetAnglesAndOriginAndUpdatePhysics( vPos, aRot );
	g_pPhysics->VSetActorProperties( GetIndex(), vPos, aRot, vec3_null, vec3_null );
	m_bJustResetted = true;
}



//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
float CPlayerCar::GetAmountUnderWater(void)
{
	if( !g_bWaterEnabled )
		return 0.0f;

	float distUnderWater = g_fWaterHeight - GetOrigin().y;
	float carHeight = 15.0f;
	float amountUnderWater = clamp(distUnderWater/carHeight, 0.0f, 1.0f);
	return amountUnderWater;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CPlayerCar::IsUnderWater( void )
{
	const float thresh = 10.0f; //1m
	return g_bWaterEnabled && GetOrigin().y < g_fWaterHeight - thresh;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPlayerCar::ResetIfNeeded( void )
{
	if( m_bShouldReset )
	{
		m_bShouldReset = false;
		Reset();
		return;
	}

	//TODO: we shouldnt ask the renderer for this, but a logical representation of the world
	//bool bWaterEnabled = true;
	if( IsUnderWater() )
	{
		float timeUnderWater = gpGlobals->curtime - m_timeEnteredWater;
		if( m_bWasUnderWater && timeUnderWater > 2.0f )
			Reset();

		if( !m_bWasUnderWater )
		{
			m_timeEnteredWater = gpGlobals->curtime;
			m_bWasUnderWater = true;
		}		
	}
	else
		m_bWasUnderWater = false;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
float CPlayerCar::CalculateMotorRpm( void )
{
	//Get Ground contact for rear wheels
	bool bGroundContact = m_pPhysVehicle->HasGroundContact(2) || m_pPhysVehicle->HasGroundContact(3);
	bGroundContact = true;

	if( !bGroundContact )
	{
		//motor accelerates by 4000 rpm per second if no ground contact
		float desiredRpm = m_engineRpm + gpGlobals->frametime * 4000.0f;
		return min( desiredRpm, m_info.motorMaxRpm );
	}

	//Wheels have ground contact.

	//Get the linear forward velocity of the car
	Vector3f vForward;
	AngleToVector( GetAngle(), vForward );
	float fForwardVel = VectorDot(GetVelocity(), -vForward);

	float fWheelRpm = fForwardVel / (2.0f*PI*m_info.wheelRadius) * 60.0f;
	float fAxleAndGearTranslation = m_info.axleRatio * m_info.gearRatios[m_engineGear-1];
	float fMotorRpm = fWheelRpm * fAxleAndGearTranslation;

	return fMotorRpm;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
float CPlayerCar::CalculateMotorForceAtWheels( float fMotorRpm )
{
	//linear engine output curve
	//Calculate current engine output in kW.
	float fCurrentEngineOutput = (m_info.motorHorsePower/1.36f) * fMotorRpm / m_info.motorMaxRpm;
	float fEngineMomentum = fCurrentEngineOutput * 9550.0f / fMotorRpm;
	float fWheelMomentumTotal = fEngineMomentum * m_info.axleRatio * m_info.gearRatios[m_engineGear-1];
	float fWheelForce = fWheelMomentumTotal / (m_info.wheelRadius*0.1f) * 0.5f;
	return fWheelForce;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPlayerCar::Update( void )
{
	m_bJustResetted = false;


	//First check if we have to reset the car position
	ResetIfNeeded();

	//Calculate the new motor rpm
	float rpm = CalculateMotorRpm();
	if(rpm >= 0.0f)
	{
		if( rpm >= m_info.motorShiftUpRpm && m_engineGear < m_info.numberOfGears )
			m_engineGear++; //shift up
		else if( rpm < (USHORT)m_info.motorShiftDownRpm && m_engineGear > 1 )
			m_engineGear--; //shift down
	}
	else
	{
		m_engineGear = 1;
	}

	m_engineRpm = (USHORT)abs(CalculateMotorRpm());
	m_engineRpm = (USHORT)clamp<int>(m_engineRpm, (int)m_info.motorMinRpm, (int)m_info.motorMaxRpm);

	float fPossibleWheelForce = CalculateMotorForceAtWheels( m_engineRpm );

	//First calculate proper forward vector by multiplying the rotation matrix
	//with the untransformed forward-vector.
	Angle3d aCarDir = GetAngle();
	Matrix3 mCarRotMat;
	GetAngleMatrix3x3( aCarDir, mCarRotMat );
	Vector3f vForward = mCarRotMat * Vector3f(0,0,1);

	//update steering and gas
	if( m_pPhysVehicle )
	{
		m_pPhysVehicle->UpdateSteering(m_fInputSteer);

		float fVehSpeed = VectorDot( GetVelocity(), vForward );
		if( fVehSpeed > 2.0f && VectorDot( vForward, GetVelocity() ) < 0.0f )
			fVehSpeed = -fVehSpeed;

		bool bMovingBackward = fVehSpeed < -2.0f;
		bool bStandingStill = abs(fVehSpeed) <= 2.0f;

		float fGas = 0.0f;
		float fBreak = 0.0f;

		if( bStandingStill )
			fGas = m_fInputAccelerate;
		else if( bMovingBackward )
		{
			if( m_fInputAccelerate > 0.0f )
				fBreak = m_fInputAccelerate;
			else
				fGas = m_fInputAccelerate;
		}
		else
		{
			if( m_fInputAccelerate > 0.0f )
				fGas = m_fInputAccelerate;
			else
				fBreak = -m_fInputAccelerate;
		}

		//Disable gas if under water
		if(IsUnderWater())
			fGas = 0.0f;

		m_pPhysVehicle->UpdateGasBreak( fGas * fPossibleWheelForce, fBreak, m_fInputHandbrake );

		//singletons::g_pEvtMgr->AddEventToQueue( 
		//	new CCarMotorUpdateEvent( ev::CAR_MOTOR_UPDATE, this->GetIndex(), m_engineRpm, abs(fGas), m_engineGear ), RECIEVER_ID_ALL );
	}

	BaseClass::Update();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CPlayerCar::HandleEvent( const IEvent *evt )
{
	if( evt->GetType() != ev::CAR_ACCEL_UPDATE &&
		evt->GetType() != ev::CAR_STEER_UPDATE &&
		evt->GetType() != ev::CAR_UPDOWN_UPDATE &&
		evt->GetType() != ev::CAR_BRAKE_UPDATE &&
		evt->GetType() != ev::CAR_RESET )
		return false;

	if( evt->GetClientID() != GetClientID() )
		return false;

	CInputEvent *pEvent = (CInputEvent*)evt;
	if( !pEvent )
		return false;
	
	if( pEvent->GetPlayerID() != this->GetPlayerID() )
		return false;

	int iEvtType = pEvent->GetType();

	if( iEvtType == ev::CAR_ACCEL_UPDATE )
		m_fInputAccelerate = pEvent->GetVal();
	else if( iEvtType == ev::CAR_BRAKE_UPDATE )
		m_fInputHandbrake = pEvent->GetVal();
	else if( iEvtType == ev::CAR_STEER_UPDATE )
		m_fInputSteer = pEvent->GetVal();
	else if( iEvtType == ev::CAR_UPDOWN_UPDATE )
		m_fInputUpDown = pEvent->GetVal();
	else if( iEvtType == ev::CAR_RESET )
		m_bShouldReset = true;
	else
		return false;

	return true;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPlayerCar::UnregisterEvents( void )
{
	singletons::g_pEvtMgr->UnregisterListener( ev::ALL_EVENTS, this );

	/*
	CEventManager *pMgr = singletons::g_pEvtMgr;
	pMgr->RegisterListener( ev::CAR_ACCEL_UPDATE, this );
	pMgr->RegisterListener( ev::CAR_STEER_UPDATE, this );
	pMgr->RegisterListener( ev::CAR_UPDOWN_UPDATE, this );
	pMgr->RegisterListener( ev::CAR_RESET, this );
	*/
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPlayerCar::Think( void )
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPlayerCar::RegisterEvents( CEventManager *pMgr )
{
	pMgr->RegisterListener( ev::CAR_ACCEL_UPDATE, this );
	pMgr->RegisterListener( ev::CAR_STEER_UPDATE, this );
	pMgr->RegisterListener( ev::CAR_BRAKE_UPDATE, this );
	pMgr->RegisterListener( ev::CAR_UPDOWN_UPDATE, this );
	pMgr->RegisterListener( ev::CAR_RESET, this );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPlayerCar::Spawn( bool bPlayerOneCar, bool bPlayerTwoCar )
{
	m_vResetPosition = GetOrigin();
	m_aResetAngles = GetAngle();

	char pchScriptFile[] = "scripts/car.rps";
	CScriptParser parser;
	if( !parser.ParseFile( pchScriptFile ) )
	{
		error( "Error loading Car-Script \"%s\"", pchScriptFile );
		return;
	}

	std::string filename;
	parser.GetString("model", filename);
	SetFilename(filename.c_str());

	char pchWheel[128];
	parser.GetString( "wheelmodel", pchWheel,  ArrayLength(pchWheel) );

	//Create wheels
	for( int i = 0; i < 4; i++ )
	{
		m_pWheels[i] = CREATE_ENTITY(CStaticModel);
		m_pWheels[i]->SetFilename( pchWheel );
		m_pWheels[i]->Spawn();
	}

	//Calculate BB
	//Vector3f vMins( -22.0801f, 4.3f, -10.3831f );
	//Vector3f vMaxs( 20.001f, 19.922f, 10.3737f );
	//TODO: generalize this
	Vector3f vMins( -10.3737f, 4.3f, -22.0801f );
	Vector3f vMaxs( 10.3831f, 19.922f, 20.001f );
	Vector3f pvVerts[8];
	pvVerts[0].Init( vMins.x, vMins.y, vMins.z );
	pvVerts[1].Init( vMins.x, vMins.y, vMaxs.z );
	pvVerts[2].Init( vMins.x, vMaxs.y, vMins.z );
	pvVerts[3].Init( vMins.x, vMaxs.y, vMaxs.z );
	pvVerts[4].Init( vMaxs.x, vMins.y, vMins.z );
	pvVerts[5].Init( vMaxs.x, vMins.y, vMaxs.z );
	pvVerts[6].Init( vMaxs.x, vMaxs.y, vMins.z );
	pvVerts[7].Init( vMaxs.x, vMaxs.y, vMaxs.z );
	
	//char testval[256];
	parser.FindSubString( "Max" );

	m_info.mass = parser.GetFloat( "mass" );
	m_info.maxEngineForce = parser.GetFloat( "MaxEngineForce" ); //this should be engine/velocity dependent
	m_info.maxBreakingForce = parser.GetFloat( "MaxBreakingForce" );
	m_info.steeringClamp = parser.GetFloat( "SteeringClamp" );
	m_info.wheelRadius = parser.GetFloat( "WheelRadius" );
	m_info.wheelWidth = parser.GetFloat( "WheelWidth" );
	m_info.wheelFriction = parser.GetFloat( "WheelFriction" );
	m_info.suspensionStiffness = parser.GetFloat( "SuspensionStiffness" );
	m_info.suspensionDamping = parser.GetFloat( "SuspensionDamping" );
	m_info.suspensionCompression = parser.GetFloat( "SuspensionCompression" );
	m_info.rollInfluence = parser.GetFloat( "RollInfluence" );
	m_info.suspensionRestLength = parser.GetFloat( "SuspensionRestLength" );
	m_info.suspensionMaxTravel = parser.GetFloat( "SuspensionMaxTravel" ); //25 cm
	m_info.suspensionForce = parser.GetFloat( "SuspensionForce" );
	m_info.suspensionMaxForce = parser.GetFloat( "SuspensionMaxForce" );

	m_info.motorHorsePower = parser.GetFloat( "HorsePower" );
	m_info.motorMaxRpm = parser.GetFloat( "MaxRpm" );
	m_info.motorMinRpm = parser.GetFloat( "MinRpm" );
	m_info.motorShiftDownRpm = parser.GetFloat( "ShiftDownRpm" );
	m_info.motorShiftUpRpm = parser.GetFloat( "ShiftUpRpm" );
	m_info.numberOfGears = parser.GetInt( "NumberOfGears" );
	m_info.gearRatios[0] = parser.GetFloat( "Gear1Ratio" );
	m_info.gearRatios[1] = parser.GetFloat( "Gear2Ratio" );
	m_info.gearRatios[2] = parser.GetFloat( "Gear3Ratio" );
	m_info.gearRatios[3] = parser.GetFloat( "Gear4Ratio" );
	m_info.gearRatios[4] = parser.GetFloat( "Gear5Ratio" );
	m_info.gearRatios[5] = parser.GetFloat( "Gear6Ratio" );
	m_info.gearRatios[6] = parser.GetFloat( "Gear7Ratio" );
	m_info.axleRatio = parser.GetFloat( "AxleRatio" );

	m_info.cwValue = parser.GetFloat( "cwValue" );
	m_info.faceSurface = parser.GetFloat( "faceSurface" );

	float pfCenterOfMass[3];
	parser.GetFloats( "CenterOfMassOffset", 3, pfCenterOfMass );
	m_info.vMassCenterOfs = Vector3f( pfCenterOfMass );

	//Calculate Wheel positions
	float fAxisDistFront = parser.GetFloat( "AxisDistFront" );
	float fAxisDistRear = parser.GetFloat( "AxisDistRear" );
	float fAxisWidthFront = parser.GetFloat( "AxisWidthFront" );
	float fAxisWidthRear = parser.GetFloat( "AxisWidthRear" );
	float fAxisHeightOffsetFront = parser.GetFloat( "AxisHeightOffsetFront" );
	float fAxisHeightOffsetRear = parser.GetFloat( "AxisHeightOffsetRear" );

	m_info.vWheelPos[0] = Vector3f( fAxisWidthFront*0.5f, fAxisHeightOffsetFront, fAxisDistFront );		//FL
	m_info.vWheelPos[1] = Vector3f( -fAxisWidthFront*0.5f, fAxisHeightOffsetFront, fAxisDistFront );	//FR
	m_info.vWheelPos[2] = Vector3f( fAxisWidthRear*0.5f, fAxisHeightOffsetRear, -fAxisDistRear );		//RL
	m_info.vWheelPos[3] = Vector3f( -fAxisWidthRear*0.5f, fAxisHeightOffsetRear, -fAxisDistRear );		//RR

	m_pPhysVehicle = ((CBulletPhysics*)g_pPhysics)->CreateVehicle( this, (CBaseEntity**)m_pWheels, pvVerts, 8, m_info );

	BaseClass::Spawn();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPlayerCar::OnCollisionStart( const CCollisionInfo &info )
{
	if(info.ent1 && info.ent2 && info.ent1->IsCollidable() && info.ent2->IsCollidable())
	{
		singletons::g_pEvtMgr->AddEventToQueue( 
			new CCollisionEvent( ev::COLLISION_OCCURED, info.point, info.force ) );
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPlayerCar::SetLastCheckpointPos( const Vector3f &pos )
{
	m_vLastCheckpointPos = pos;
}
