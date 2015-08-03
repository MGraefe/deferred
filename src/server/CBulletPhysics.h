// server/CBulletPhysics.h
// server/CBulletPhysics.h
// server/CBulletPhysics.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__server__CBulletPhysics_H__
#define deferred__server__CBulletPhysics_H__

//#ifdef _SERVER
//#pragma comment(lib, "BulletCollision.lib")
//#pragma comment(lib, "BulletDynamics.lib")
//#pragma comment(lib, "LinearMath.lib")
//#endif



#define BT_EULER_DEFAULT_ZYX
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include "IGamePhysics.h"
#include "vehicleinfo_t.h"
#include <map>
#include <set>
#include <vector>
#include <util/IPhysicsDebug.h>
#include <util/IEvent.h>
#include <util/CThreadMutex.h>
#include <util/CScriptParser.h>

class CBulletActor;
class CBulletDebugDrawer;
class CPhysVehicle;

enum PhysicsMaterial
{
	MAT_NORMAL = 0,
	MAT_WORLD,
	MAT_BOUNCY,
	MAT_PLAYDOUGH,
};


class CBulletPhysics : public IGamePhysics
{
friend class CPhysVehicle;

private: //NONCOPYABLE
	CBulletPhysics( const CBulletPhysics &p ) { }

public:
	CBulletPhysics();
	~CBulletPhysics();

	virtual bool VInitialize( void );
	virtual void VOnUpdate( float deltaS );
	virtual void VSyncVisibleScene( void );
	virtual void VRenderDiagnostics( void );
	
	virtual void VCreateSphere( const float radius, CBaseEntity *pEnt );
	virtual void VCreateBox( Vector3f *pvVerts8, CBaseEntity *pEnt, const Vector3f &vCenterOfMass = vec3_null );
	virtual bool VCreateWorld( CBaseEntity *pWorld, const char *filename );
	virtual void VCreateCollisionMesh( CBaseEntity *pWorld, const char *filename );
	virtual void VCreateTrigger( CBaseEntity *pEnt, Vector3f *pvVerts8 );
	virtual void VCreateFromPhysFile( CBaseEntity *pEnt, const CPhysFile *pFile, bool staticEntity );

	virtual void VSetActorOrigin( const int &iActorID, const Vector3f &pos );
	virtual void VSetActorAngles( const int &iActorID, const Angle3d &ang );
	virtual void VSetActorOriginAndAngles( const int &iActorID, const Vector3f &pos, const Angle3d &ang );
	virtual void VSetActorProperties( const int &iActorID, const Vector3f &position,
		const Angle3d &angles, const Vector3f &velocity, const Angle3d &angularVelocity );
	virtual void VSetCollidable( CBaseEntity* ent, bool collidable );

	CPhysVehicle* CreateVehicle( CBaseEntity *pLogic, CBaseEntity **pWheels, Vector3f *pShapeVerts, const int iShapeVerts, const vehicleinfo_t &info );
	void RemoveVehicle( CPhysVehicle *pVehicle );
	virtual void VRemoveEntity( CBaseEntity *pEnt );


	void SetDebugDrawer(btIDebugDraw *drawer);

private:
	void GetMaterialParams(const std::string &name, float &friction, float &restitution);
	void SendCollisionAddEvent( btPersistentManifold const *manifold,
								btRigidBody const *body0,
								btRigidBody const *body1 );
	void SendCollisionRemoveEvent( btRigidBody const *body0, btRigidBody const *body1 );

	btRigidBody *AddShape( CBaseEntity *pEnt, btCollisionShape *shape, btScalar mass, float friction = 0.5f, float restitution = 0.25f, bool isWorld = false, const Vector3f &vCenterOfMass = vec3_null );
	void RemoveCollisionObject( btCollisionObject *obj );
	btRigidBody *FindActorBody( const int &actorID ) const;
	CBulletActor *FindBulletActor( const int &actorID ) const;
	int FindActorID( const btRigidBody * const pRigid ) const;
	static void BulletInternalTickCallback( btDynamicsWorld *const world, btScalar const timeStep );

private:
	btDynamicsWorld			*m_dynamicsWorld;
	btBroadphaseInterface	*m_broadphase;
	btCollisionDispatcher	*m_dispatcher;
	btConstraintSolver		*m_solver;
	btDefaultCollisionConfiguration *m_collisionConfiguration;
	btIDebugDraw			*m_debugDrawer;
	CThreadMutex			m_debugMutex;

	typedef std::map<int,CBulletActor*> ActorIDToBulletActorMap;
	ActorIDToBulletActorMap m_actorBodies;

	typedef std::map<btRigidBody const *, int> RigidBodyToActorIDMap;
	RigidBodyToActorIDMap m_rigidBodyToActorId;

	typedef std::pair< btRigidBody const *, btRigidBody const *> CollisionPair;
	typedef std::set< CollisionPair > CollisionPairs;
	CollisionPairs m_previousTickCollisionPairs;

	typedef std::vector< CPhysVehicle* > VehicleList;
	VehicleList m_vehicleList;

	CScriptParser m_materialScript;
};


class CBulletActor
{
public:
	explicit CBulletActor(btRigidBody* pRigidBody) { m_pRigidBody = pRigidBody;}
	void SetTransformMatrix( const Vector3f &pos, const Angle3d &ang );
	void SetVelocity( const Vector3f &velocity );
	void SetAngularVelocity( const Angle3d &angVelocity );

public:
	btRigidBody* m_pRigidBody;  // the rigid body associted with this actor (should never be NULL)
};

//Bullet uses meters, game uses decimetres
inline btVector3 VecToBtVec( const Vector3f &v )
{
	return btVector3( v.x*0.1f, v.y*0.1f, v.z*0.1f );
}

inline Vector3f BtVecToVec( const btVector3 &v )
{
	return Vector3f( v.x()*10.0f, v.y()*10.0f, v.z()*10.0f );
}

inline Vector3f BtColToCol( const btVector3 &v )
{
	return Vector3f( v.x(), v.y(), v.z() );
}

const Angle3d BtQuatToAngle( const btQuaternion &quat );

class ActorMotionState : public btMotionState
{
public:
	Vector3f m_vWorldPos;
	btVector3 m_CenterOfMassOffset;
	Angle3d m_aWorldRot;
	//btTransform m_lastTrans;
	
	ActorMotionState( Vector3f const &vWorldPos, Angle3d const &aRotation, Vector3f const &vCenterOfMassOfs = vec3_null ) :
		m_vWorldPos(vWorldPos),
		m_aWorldRot(aRotation),
		m_CenterOfMassOffset(VecToBtVec(vCenterOfMassOfs))
		{ }
	//{ m_vWorldPos = vWorldPos; m_aWorldRot = aRotation; }
	
	// btMotionState interface:  Bullet calls these
	virtual void getWorldTransform( btTransform& worldTrans ) const;
	virtual void setWorldTransform( const btTransform& worldTrans );
};


struct MaterialData
{
	float m_restitution;
	float m_friction;
};

const MaterialData g_PhysMatVals[] = 
{
	//restitution	friction
	{	0.25f,		0.5f },		//MAT_NORMAL
	{	0.25f,		0.5f },		//MAT_WORLD
	{	0.95f,		0.5f },		//MAT_BOUNCY
	{	0.05f,		0.9f },		//MAT_PLAYDOUGH
};


class CBulletDebugDrawer : public btIDebugDraw, IEventListener
{
public:
	CBulletDebugDrawer();

	virtual void drawLine(  const btVector3 &from,
							const btVector3 &to,
							const btVector3 &color );
	virtual void drawContactPoint(  const btVector3 &PointOnB,
									const btVector3 &normalOnB,
									btScalar distance,
									int lifeTime,
									const btVector3 &color );
	virtual void reportErrorWarning( const char *warningString );
	virtual void draw3dText( const btVector3 &location,
		const char* textString );
	virtual void setDebugMode( int debugMode );
	virtual int getDebugMode() const;

	//From IEventListener
	virtual bool HandleEvent( const IEvent *evt );
	virtual void UnregisterEvents( void );

	void dispatch(void);
private:
	IPhysicsDebug *link;
};


//This is a bit hacky...
//DO NOT USE MULTIPLE INHERITANCE WITH THIS OBJECT
#include <renderer/IDebugRenderer.h>
class CBulletDebugDrawerCallback : public IDebugRenderer
{
public:
	CBulletDebugDrawerCallback(CBulletPhysics *physics);
	void RenderDebug(void);

private:
	CBulletPhysics *m_physics;
};

#endif
