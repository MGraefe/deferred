// server/CPhysVehicle.cpp
// server/CPhysVehicle.cpp
// server/CPhysVehicle.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CBulletPhysics.h"
#include "CPhysVehicle.h"
#include "CBaseEntity.h"
#include <util/error.h>
#include "CPlayerCar.h"
#include "ServerInterfaces.h"

CPhysVehicle::CPhysVehicle()
{

}

CPhysVehicle::~CPhysVehicle()
{
	delete m_vehicleRayCaster;
	delete m_vehicle;
	delete m_wheelShape;
	delete m_carChassis;
	delete m_actorMotionState;
}


//-----------------------------------------------------------------------
// Purpose: Creates a physical Vehicle
// Params:
// IN pDynamicsWorld: the dynamics world
// IN pEnt			: Pointer to the logical represantation of the car
// IN pVerts		: Pointer to an array of Vertexes for the Car-Chassis Collision Mesh
// IN iVerts		: Number of Vertexes in pVerts
// IN info			: Vehicle Information
//-----------------------------------------------------------------------
void CPhysVehicle::Create( CBulletPhysics *pPhysics, btDynamicsWorld *pDynamicsWorld, CBaseEntity *pEnt, CBaseEntity **pWheels,
	Vector3f *pVerts, const int iVerts, const vehicleinfo_t &info )
{
	m_dynamicsWorld = pDynamicsWorld;
	m_info = info;
	m_pEnt = pEnt;

	btConvexHullShape *chassisHull = new btConvexHullShape();
	for( int i = 0; i < iVerts; i++ )
		chassisHull->addPoint( VecToBtVec( pVerts[i] - info.vMassCenterOfs ) );

	btVector3 localInertia( 0.0f, 0.0f, 0.0f );
	if( info.mass > 0.01f )
		chassisHull->calculateLocalInertia( info.mass, localInertia );

	m_actorMotionState = new ActorMotionState( pEnt->GetOrigin(), pEnt->GetAngle(), info.vMassCenterOfs );

	//center of mass
	//btTransform ShapeTransform;
	//ShapeTransform.setIdentity();
	//ShapeTransform.setOrigin( VecToBtVec(-info.vMassCenterOfs) );

	//create a compound shape
	//btCompoundShape *pCmpShp = new btCompoundShape(true);
	//pCmpShp->addChildShape( ShapeTransform, chassisHull );

	btRigidBody::btRigidBodyConstructionInfo rbInfo( info.mass, m_actorMotionState, chassisHull, localInertia );

	rbInfo.m_restitution = 0.05f;
	rbInfo.m_friction =	0.3f;

	//create a rigid body and add it to the world
	m_carChassis = new btRigidBody( rbInfo );
	m_dynamicsWorld->addRigidBody( m_carChassis );

	//m_carChassis->setCenterOfMassTransform( btTransform(btMatrix3x3::getIdentity(), VecToBtVec(info.vMassCenterOfs)) );
	m_carChassis->setLinearVelocity(btVector3(0,0,0));
	m_carChassis->setAngularVelocity(btVector3(0,0,0));
	m_dynamicsWorld->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(
		m_carChassis->getBroadphaseHandle(),m_dynamicsWorld->getDispatcher());

	m_wheelShape = new btCylinderShapeX( btVector3(info.wheelWidth, info.wheelRadius, info.wheelRadius) );
	m_vehicleRayCaster = new btDefaultVehicleRaycaster(m_dynamicsWorld);
	m_vehicle = new btRaycastVehicle( m_tuning, m_carChassis, m_vehicleRayCaster );
	
	///never deactivate the vehicle
	m_carChassis->setActivationState(DISABLE_DEACTIVATION);

	m_dynamicsWorld->addVehicle(m_vehicle);

	m_vehicle->setCoordinateSystem(0,1,2);

	//Add 4 wheels:
	btVector3 wheelDir( 0.0f, -1.0f, 0.0f );
	btVector3 wheelAxis( -1.0f, 0.0f, 0.0f );
	for( int i = 0; i < 4; i++ )
	{
		m_vehicle->addWheel(
			VecToBtVec(info.vWheelPos[i] - info.vMassCenterOfs),
			wheelDir, 
			wheelAxis,
			info.suspensionRestLength * 0.1f,
			info.wheelRadius * 0.1f,
			m_tuning,
			(i < 2) );

		m_pWheels[i] = pWheels[i];
	}

	//Set wheel information
	for( int i=0;i<m_vehicle->getNumWheels(); i++ )
	{
		btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
		wheel.m_suspensionStiffness =		info.suspensionStiffness;
		wheel.m_wheelsDampingRelaxation =	info.suspensionDamping * 2.0f * sqrt(info.suspensionStiffness);
		wheel.m_wheelsDampingCompression =	info.suspensionCompression * 2.0f * sqrt(info.suspensionStiffness);
		wheel.m_suspensionRestLength1 =		info.suspensionRestLength*0.1f;
		wheel.m_maxSuspensionForce =		info.suspensionMaxForce;
		wheel.m_wheelsSuspensionForce =		info.suspensionForce;
		wheel.m_rollInfluence =				info.rollInfluence;
		wheel.m_frictionSlip =				info.wheelFriction;
	}

	//Create the Bullet Actor
	CBulletActor *pBulletActor = new CBulletActor(m_carChassis);

	//add the actor to our list
	//this is some strange map [] operator overloading.
	pPhysics->m_actorBodies[pEnt->GetIndex()] = pBulletActor;
	pPhysics->m_rigidBodyToActorId[m_carChassis] = pEnt->GetIndex();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPhysVehicle::Destroy( void )
{
	if( m_dynamicsWorld && m_vehicle && m_carChassis )
	{
		m_dynamicsWorld->removeVehicle( this->m_vehicle );
		m_dynamicsWorld->removeRigidBody( this->m_carChassis );
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPhysVehicle::UpdateSteering( float fSteer )
{
	if( !m_vehicle )
		return;

	m_vehicle->setSteeringValue( -fSteer * m_info.steeringClamp, 0 );
	m_vehicle->setSteeringValue( -fSteer * m_info.steeringClamp, 1 );
}

// p = density in kg/m^3
// cw = car's cw value
// A = car's front surface in m^2
// v = car's speed in m/s
// returns force in N
float calculateMediumResistance(float p, float cw, float A, float v)
{
	return p * cw * A * v * v * 0.5f;
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPhysVehicle::UpdateGasBreak( float fGas, float fBreak, float fHandBrake )
{
	//fGas *= m_info.maxEngineForce;
	fBreak *= m_info.maxBreakingForce;
	fHandBrake *= m_info.maxEngineForce;
	
	m_vehicle->applyEngineForce( fGas, 2 );
	m_vehicle->applyEngineForce( fGas, 3 );

	m_vehicle->setBrake( (fBreak)*0.7f*0.25f, 0 );
	m_vehicle->setBrake( (fBreak)*0.7f*0.25f, 1 );
	m_vehicle->setBrake( (fBreak+fHandBrake)*0.3f*0.25f, 2 );
	m_vehicle->setBrake( (fBreak+fHandBrake)*0.3f*0.25f, 3 );

	CPlayerCar *pCar = dynamic_cast<CPlayerCar*>(m_pEnt);
	if( !pCar || pCar->HadResetLastFrame() )
		return;
	
	btVector3 vLinVel( m_vehicle->getRigidBody()->getLinearVelocity() );
	float fSpeed = vLinVel.length();
	btVector3 vForceDir = vLinVel / -fSpeed;

	//Calculate medium-resistances
	float Fair = calculateMediumResistance(1.2041f, m_info.cwValue, m_info.faceSurface, fSpeed);
	float Fwater = calculateMediumResistance(1000.0f, m_info.cwValue, m_info.faceSurface*pCar->GetAmountUnderWater(), fSpeed);

	m_vehicle->getRigidBody()->applyCentralForce( vForceDir * (Fair+Fwater) );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CPhysVehicle::HasGroundContact( int iWheelIndex )
{
	Assert( iWheelIndex >= 0 && iWheelIndex < m_vehicle->getNumWheels() );

	return m_vehicle->getWheelInfo(iWheelIndex).m_raycastInfo.m_isInContact;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPhysVehicle::Sync( void )
{
	//m_vehicle->resetSuspension();
	/*m_vehicle->updateWheelTransform(0,true);
	m_vehicle->updateWheelTransform(1,true);
	m_vehicle->updateWheelTransform(2,true);
	m_vehicle->updateWheelTransform(3,true);*/
	
	for( int i = 0; i < 4; i++ )
	{	
		m_vehicle->updateWheelTransform( i, true );
		Vector3f wheelPos = BtVecToVec( m_vehicle->getWheelTransformWS(i).getOrigin() );
		Vector3f wheelRot = BtQuatToAngle( m_vehicle->getWheelTransformWS(i).getRotation() );
		if(wheelPos.IsValid())
		{
			m_pWheelPos[i] = wheelPos;
			m_pWheels[i]->SetOrigin( wheelPos );
		}
		if(wheelRot.IsValid())
		{
			m_pWheelRot[i] = wheelRot;
			m_pWheels[i]->SetAngle( wheelRot );
		}
	}

	{
		char msg[128];
		sprintf_s(msg, "FL %i, FR %i, RL %i, RR %i", HasGroundContact(0), HasGroundContact(1), HasGroundContact(2), HasGroundContact(3));
		singletons::g_pRealtimeDebugInterface->SetValue("car.wheel.contact", msg);

		sprintf_s(msg, "FL %f, FR %f, RL %f, RR %f", m_vehicle->getWheelInfo(0).m_skidInfo,
			m_vehicle->getWheelInfo(1).m_skidInfo,
			m_vehicle->getWheelInfo(2).m_skidInfo,
			m_vehicle->getWheelInfo(3).m_skidInfo);
		singletons::g_pRealtimeDebugInterface->SetValue("car.wheel.skid", msg);

		sprintf_s(msg, "FL %f, FR %f, RL %f, RR %f", m_vehicle->getWheelInfo(0).m_wheelsSuspensionForce,
			m_vehicle->getWheelInfo(1).m_wheelsSuspensionForce,
			m_vehicle->getWheelInfo(2).m_wheelsSuspensionForce,
			m_vehicle->getWheelInfo(3).m_wheelsSuspensionForce);
		singletons::g_pRealtimeDebugInterface->SetValue("car.wheel.force", msg);

		sprintf_s(msg, "%f km/h", m_vehicle->getCurrentSpeedKmHour());
		singletons::g_pRealtimeDebugInterface->SetValue("car.velocity.srv", msg);
	}

	//Always update car
	const Vector3f vPos = m_actorMotionState->m_vWorldPos;
	const Angle3d aOri = m_actorMotionState->m_aWorldRot;

	if(vPos.IsValid())
		m_pEnt->SetOrigin( vPos );
	if(aOri.IsValid())
		m_pEnt->SetAngle( aOri );
	m_pEnt->SetVelocity( BtVecToVec(m_carChassis->getLinearVelocity()) );
}
