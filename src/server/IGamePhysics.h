// server/IGamePhysics.h
// server/IGamePhysics.h
// server/IGamePhysics.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__server__IGamePhysics_H__
#define deferred__server__IGamePhysics_H__

#include <util/maths.h>

class CBaseEntity;
class CPhysFile;
class IGamePhysics
{
public:
	virtual ~IGamePhysics() { }
	virtual bool VInitialize( void ) = 0;
	virtual void VOnUpdate( float deltaS ) = 0;
	virtual void VSyncVisibleScene( void ) = 0;
	virtual void VRenderDiagnostics( void ) = 0;
	
	virtual void VCreateSphere( const float radius, CBaseEntity *pEnt ) = 0;
	virtual void VCreateBox( Vector3f *pvVerts8, CBaseEntity *pEnt, const Vector3f &vCenterOfMass = vec3_null ) = 0;
	virtual bool VCreateWorld( CBaseEntity *pWorld, const char *filename ) = 0;
	virtual void VCreateCollisionMesh( CBaseEntity *pWorld, const char *filename ) = 0;
	virtual void VCreateTrigger( CBaseEntity *pEnt, Vector3f *pvVerts8 ) = 0;
	virtual void VCreateFromPhysFile( CBaseEntity *pEnt, const CPhysFile *pFile, bool staticEntity ) = 0;

	virtual void VSetActorOrigin( const int &iActorID, const Vector3f &pos ) = 0;
	virtual void VSetActorAngles( const int &iActorID, const Angle3d &ang ) = 0;
	virtual void VSetActorOriginAndAngles( const int &iActorID, const Vector3f &pos, const Angle3d &ang ) = 0;
	virtual void VSetActorProperties( const int &iActorID, const Vector3f &position,
		const Angle3d &angles, const Vector3f &velocity, const Angle3d &angularVelocity ) = 0;
	virtual void VRemoveEntity( CBaseEntity *pEnt ) = 0;
	virtual void VSetCollidable( CBaseEntity* ent, bool collidable ) = 0;
};

extern IGamePhysics* g_pPhysics;

#endif
