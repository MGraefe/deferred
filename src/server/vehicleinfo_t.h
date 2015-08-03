// server/vehicleinfo_t.h
// server/vehicleinfo_t.h
// server/vehicleinfo_t.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#ifndef VEHICLEINFO_T
#define VEHICLEINFO_T

#include <util/maths.h>

struct vehicleinfo_t
{
	float	maxEngineForce;//this should be engine/velocity dependent
	float	maxBreakingForce;

	float	steeringClamp;
	float	wheelRadius;
	float	wheelWidth;
	float	wheelFriction;
	float	suspensionStiffness;
	float	suspensionDamping;
	float	suspensionCompression;
	float	suspensionMaxTravel;
	float	suspensionMaxForce;
	float	suspensionForce;
	float	rollInfluence;
	float	mass;
	float	suspensionRestLength;
	float	motorHorsePower;
	float	motorMaxRpm;
	float	motorMinRpm;
	float	motorShiftUpRpm;
	float	motorShiftDownRpm;
	float	axleRatio;
	float	gearRatios[7];
	int		numberOfGears;

	//air density
	float	faceSurface;
	float	cwValue;

	Vector3f vMassCenterOfs;

	Vector3f vWheelPos[4];
};


/*
btVector3 	m_chassisConnectionPointCS
btVector3 	m_wheelDirectionCS
btVector3 	m_wheelAxleCS
btScalar 	m_suspensionRestLength1
btScalar 	m_maxSuspensionTravelCm
btScalar 	m_wheelsRadius
btScalar 	m_suspensionStiffness
btScalar 	m_wheelsDampingCompression
btScalar 	m_wheelsDampingRelaxation
btScalar 	m_frictionSlip
btScalar 	m_steering
btScalar 	m_rotation
btScalar 	m_deltaRotation
btScalar 	m_rollInfluence
btScalar 	m_maxSuspensionForce
btScalar 	m_engineForce
btScalar 	m_brake
bool 	m_bIsFrontWheel
void * 	m_clientInfo
btScalar 	m_clippedInvContactDotSuspension
btScalar 	m_suspensionRelativeVelocity
btScalar 	m_wheelsSuspensionForce
*/
#endif
