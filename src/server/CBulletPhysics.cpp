// server/CBulletPhysics.cpp
// server/CBulletPhysics.cpp
// server/CBulletPhysics.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CBulletPhysics.h"
#include "CEntityList.h"
#include "ServerRPM.h"
#include <util/error.h>
#include <util/debug.h>
#include "CPhysVehicle.h"
#include <util/globalvars.h>
#include "ServerInterfaces.h"
#include <algorithm>
#include <iterator>
#include <util/CConVar.h>
#include <util/CPhysFile.h>
#include <util/CPhysConvexCompoundShape.h>
#include <util/CPhysConvexHull.h>
#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>

IGamePhysics *g_pPhysics;
CBulletPhysics g_bulletPhysicsObj;


static bool CustomContactCallback(btManifoldPoint& cp, const btCollisionObject* colObj0, int partId0, int index0, const btCollisionObject* colObj1, int partId1, int index1)
{
	btAdjustInternalEdgeContacts(cp, colObj1, colObj0, partId1, index1);
	return false;
}



//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CBulletPhysics::CBulletPhysics()
{
	m_dynamicsWorld = NULL;
	m_broadphase = NULL;
	m_dispatcher = NULL;
	m_solver = NULL;
	m_collisionConfiguration = NULL;
	m_debugDrawer = NULL;
	g_pPhysics = this;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CBulletPhysics::~CBulletPhysics()
{
	//delete any objects that are still in the world
	//for( int i = m_dynamicsWorld->getNumCollisionObjects()-1; i >= 0; --i )
	//{
	//	btCollisionObject *const obj = m_dynamicsWorld->getCollisionObjectArray()[i];
	//	RemoveCollisionObject( obj );
	//}

	//Remove BulletActors
	for( ActorIDToBulletActorMap::iterator it = m_actorBodies.begin();
		 it != m_actorBodies.end();
		 ++it )
	{
		CBulletActor *pBulletActor = it->second;
		delete pBulletActor;
	}

	//Remove Cars
	for( unsigned int i = 0; i < m_vehicleList.size(); i++ )
	{
		delete m_vehicleList[i];
	}

	m_actorBodies.clear();

	if( m_dynamicsWorld )
		delete m_dynamicsWorld;
	if( m_broadphase )
		delete m_broadphase;
	if( m_dispatcher )
		delete m_dispatcher;
	if( m_solver )
		delete m_solver;
	if( m_collisionConfiguration )
		delete m_collisionConfiguration;
	if( m_debugDrawer )
		delete m_debugDrawer;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CBulletPhysics::VInitialize( void )
{
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher( m_collisionConfiguration );
	m_broadphase = new btDbvtBroadphase();
	m_solver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(	m_dispatcher,
													m_broadphase,
													m_solver,
													m_collisionConfiguration );

	m_dynamicsWorld->setInternalTickCallback( BulletInternalTickCallback );
	m_dynamicsWorld->setWorldUserInfo( this );

	m_dynamicsWorld->setGravity( btVector3(0.0f, -9.81f, 0.0f) );

	//This is bullets great style: random global variable outside of any namespace...
	gContactAddedCallback = CustomContactCallback;

	const char matFile[] = "scripts/materialgroups.txt";
	if(!m_materialScript.ParseFile(matFile))
		error("Could not parse file %s.", matFile);

	return true;
}


//-----------------------------------------------------------------------
// Purpose: Update the physical world
//-----------------------------------------------------------------------
void CBulletPhysics::VOnUpdate( float deltaS )
{
	bool handleMutex = m_debugDrawer != NULL;
	if( handleMutex )
		m_debugMutex.SetOrWait();
	if( m_debugDrawer ) //Prevent drawing inside stepSimulation, quick hack
		m_dynamicsWorld->setDebugDrawer(NULL);
	m_dynamicsWorld->stepSimulation( deltaS, 4, 1.0f/120.0f );
	if( m_debugDrawer )
		m_dynamicsWorld->setDebugDrawer(m_debugDrawer);
	if( handleMutex )
		m_debugMutex.Release();

}


//-----------------------------------------------------------------------
// Purpose: Keep physical objects and their graphic representations in sync
//-----------------------------------------------------------------------
void CBulletPhysics::VSyncVisibleScene( void )
{
	for( ActorIDToBulletActorMap::const_iterator it = m_actorBodies.begin();
		 it != m_actorBodies.end();
		 ++it )
	{
		int const id = it->first;

		//this is safe only when all objects in the world are created by AddShape()
		ActorMotionState const * const actorMotionState = 
			(ActorMotionState*)(it->second->m_pRigidBody->getMotionState());
		Assert( actorMotionState );

		CBaseEntity *pActor = singletons::g_pEntityList->GetEntity( id );
		if( !pActor )
			continue;

		if(actorMotionState->m_vWorldPos.IsValid())
			pActor->SetOrigin( actorMotionState->m_vWorldPos );
		if(actorMotionState->m_aWorldRot.IsValid())
			pActor->SetAngle( actorMotionState->m_aWorldRot );
	}

	//sync vehicles
	const int iVehicles = m_vehicleList.size();
	for( int i = 0; i < iVehicles; i++ )
	{
		m_vehicleList[i]->Sync();
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::VRenderDiagnostics( void )
{
	m_debugMutex.SetOrWait();
	m_dynamicsWorld->debugDrawWorld();
	dynamic_cast<CBulletDebugDrawer*>(m_debugDrawer)->dispatch();
	m_debugMutex.Release();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::VCreateSphere( const float radius, CBaseEntity *pEnt )
{
	Assert( pEnt );

	btSphereShape* const collisionShape = new btSphereShape( radius*0.1f );
	float const volume = (4.0f/3.0f) * PI * radius * 0.1f * radius * radius;
	float const mass = volume * 0.0005f;

	AddShape( pEnt, collisionShape, mass );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::VCreateBox( Vector3f *pvVerts8, CBaseEntity *pEnt, const Vector3f &vCenterOfMass )
{
	Assert( pEnt );

	btConvexHullShape* const shape = new btConvexHullShape();

	for( int i = 0; i < 8; i++ )
		shape->addPoint( VecToBtVec(pvVerts8[i] - vCenterOfMass) );

	float mass = 1000.0f;
	AddShape( pEnt, shape, mass );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::VCreateFromPhysFile( CBaseEntity *pEnt, const CPhysFile *pFile, bool staticEntity )
{
	if( pFile->GetShape() == NULL || 
		pFile->GetShape()->GetSize() == 0 || 
		pFile->GetShape()->GetShape(0).GetSize() < 4 )
	{
		ConsoleMessage("CBulletPhysics#VCreateFromPhysFile(): empty .phys file");
		return;
	}
	
	btTransform identityTransform(btMatrix3x3::getIdentity());
	btCompoundShape *compoundShape = new btCompoundShape();
	const CPhysConvexCompoundShape *fileComp = pFile->GetShape();
	for(UINT i = 0; i < fileComp->GetSize(); i++ )
	{
		const CPhysConvexHull &fileHull = fileComp->GetShape(i);
		btConvexHullShape *shape = new btConvexHullShape();
		for(UINT k = 0; k < fileHull.GetSize(); k++ )
			shape->addPoint(VecToBtVec(fileHull.GetVertex(k) - pFile->GetCenterOfMass()));
		compoundShape->addChildShape(identityTransform, shape);
	}

	int matIndex = fileComp->GetShape(0).GetMaterialIndex();
	std::string physMaterial = fileComp->GetMaterialNameById(matIndex);
	float friction, restitution;
	GetMaterialParams(physMaterial, friction, restitution);

	float mass = staticEntity ? 0.0f : pFile->GetMass();
	AddShape(pEnt, compoundShape, mass, friction, restitution, false, pFile->GetCenterOfMass());
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CPhysVehicle *CBulletPhysics::CreateVehicle( CBaseEntity *pLogic, CBaseEntity **pWheels, Vector3f *pShapeVerts, const int iShapeVerts, const vehicleinfo_t &info )
{
	CPhysVehicle *pVehicle = new CPhysVehicle();
	pVehicle->Create( this, m_dynamicsWorld, pLogic, pWheels, pShapeVerts, iShapeVerts, info );

	//push back in our list
	m_vehicleList.push_back( pVehicle );

	return pVehicle;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::RemoveVehicle( CPhysVehicle *pVehicle )
{
	for( UINT i = 0; i < m_vehicleList.size(); i++ )
	{
		if( m_vehicleList[i] == pVehicle )
		{
			pVehicle->Destroy();
			m_vehicleList.erase( m_vehicleList.begin() + i );
			return;
		}
	}
}




//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CBulletPhysics::VCreateWorld( CBaseEntity *pWorld, const char *filename )
{
	unsigned int timeStart = singletons::g_pTimer->GetTimeMs();

	int iTris;
	rendertri_t *pTris = LoadRPMFileServer( filename, iTris );
	if( !pTris )
	{
		error( "%s: World not found: \"%s\"", __FILE__, filename );
		return false;
	}

	btTriangleMesh *pMesh = new btTriangleMesh();

	for( int i = 0; i < iTris; i++ )
	{
		rendertri_t *pTri = &pTris[i];
		pMesh->addTriangle( VecToBtVec(pTri->pVerts[0].vPos),
							VecToBtVec(pTri->pVerts[1].vPos),
							VecToBtVec(pTri->pVerts[2].vPos),
							true );
	}
	delete[] pTris;

	btBvhTriangleMeshShape *pMeshShape = new btBvhTriangleMeshShape( pMesh, true, true );
	btTriangleInfoMap *pTriInfoMap = new btTriangleInfoMap();
	btGenerateInternalEdgeInfo(pMeshShape, pTriInfoMap);

	btRigidBody *body = AddShape( pWorld, pMeshShape, 0.0f, MAT_WORLD, true );
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	
	unsigned int time = singletons::g_pTimer->GetTimeMs() - timeStart;
	ConsoleMessage( "Creating the World-Physics took us %i ms.", time );

	return true;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::VCreateCollisionMesh( CBaseEntity *pWorld, const char *filename )
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::VCreateTrigger( CBaseEntity *pEnt, Vector3f *pvVerts8 )
{
	Assert( pEnt );

	btConvexHullShape* const shape = new btConvexHullShape();

	for( int i = 0; i < 8; i++ )
		shape->addPoint( VecToBtVec(pvVerts8[i]) );

	float mass = 1.0f;
	AddShape( pEnt, shape, mass );

	btRigidBody *body = m_actorBodies[pEnt->GetIndex()]->m_pRigidBody;
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	body->setGravity(btVector3(0,0,0));
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::SendCollisionAddEvent( btPersistentManifold const *manifold,
								btRigidBody const *body0,
								btRigidBody const *body1 )
{
	CCollisionInfo info;

	//find the contact point with the largest impulse
	info.force = 0.0f;
	int iLargestForcePoint = 0;
	int iContactPoints = manifold->getNumContacts();

	if( iContactPoints < 1 )
		return;

	for( int i = 0; i < iContactPoints; i++ )
	{
		float fPointForce = manifold->getContactPoint(i).getAppliedImpulse();
		if( fPointForce > info.force )
		{
			info.force = fPointForce;
			iLargestForcePoint = i;
		}
	}

	info.point = BtVecToVec(manifold->getContactPoint(iLargestForcePoint).getPositionWorldOnA());

	int id0 = FindActorID(body0);
	int id1 = FindActorID(body1);
	info.ent1 = singletons::g_pEntityList->GetEntity(id0);
	info.ent2 = singletons::g_pEntityList->GetEntity(id1);
	
	if( info.ent1 )
		info.ent1->OnCollisionStart(info);
	if( info.ent2 )
		info.ent2->OnCollisionStart(info);

	//singletons::g_pEvtMgr->AddEventToQueue( 
	//	new CCollisionEvent( ev::COLLISION_OCCURED, vContactPos, force ) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBulletPhysics::GetMaterialParams( const std::string &name, float &friction, float &restitution )
{
	friction = 0.5f;
	restitution = 0.25f;
	CScriptSubGroup *materialGrp = m_materialScript.GetSubGroup(name.c_str());
	if(materialGrp)
	{
		friction = materialGrp->GetFloat("friction", friction);
		restitution = materialGrp->GetFloat("restitution", restitution);
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::SendCollisionRemoveEvent( btRigidBody const *body0, btRigidBody const *body1 )
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
btRigidBody *CBulletPhysics::AddShape( CBaseEntity *pEnt, btCollisionShape *shape, btScalar mass, float friction /*= 0.5f*/, float restitution /*= 0.25f*/, bool isWorld /*= false*/, const Vector3f &vCenterOfMass /*= vec3_null */ )
{
	Assert( pEnt );
	int id = pEnt->GetIndex();
	Assert( id >= 0 );

	btVector3 localInertia( 0.0f, 0.0f, 0.0f );
	if( mass > 0.001f )
		shape->calculateLocalInertia( mass, localInertia );

	ActorMotionState* const myMotionState = new ActorMotionState( pEnt->GetOrigin(), pEnt->GetAngle(), vCenterOfMass );
	btRigidBody::btRigidBodyConstructionInfo rbInfo( mass, myMotionState, shape, localInertia );

	rbInfo.m_restitution = restitution;
	rbInfo.m_friction =	friction;

	btRigidBody* const body = new btRigidBody( rbInfo );
	if(!pEnt->IsCollidable())
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	m_dynamicsWorld->addRigidBody( body );

	//Create the Bullet Actor
	CBulletActor *pBulletActor = new CBulletActor(body);

	//add the actor to our list
	//this is some strange map [] operator overloading.
	m_actorBodies[id] = pBulletActor;
	m_rigidBodyToActorId[body] = id;

	return body;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::RemoveCollisionObject( btCollisionObject *obj )
{
	delete obj;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
btRigidBody *CBulletPhysics::FindActorBody( const int &actorID ) const
{
	CBulletActor *pActor = FindBulletActor( actorID );
	if( !pActor )
		return NULL;
	return pActor->m_pRigidBody;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CBulletActor *CBulletPhysics::FindBulletActor( const int &actorID ) const
{
	ActorIDToBulletActorMap::const_iterator it = m_actorBodies.find( actorID );
	
	if( it == m_actorBodies.end() )
		return NULL;

	return it->second;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
int CBulletPhysics::FindActorID( const btRigidBody * const pRigid ) const
{
	RigidBodyToActorIDMap::const_iterator it = m_rigidBodyToActorId.find( pRigid );
	if( it == m_rigidBodyToActorId.end() )
		return -1;

	return it->second;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::VSetActorOrigin( const int &iActorID, const Vector3f &pos )
{
	CBulletActor *pActor = FindBulletActor( iActorID );
	Assert( pActor );

	pActor->m_pRigidBody->getWorldTransform().setOrigin( VecToBtVec( pos ) );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::VSetActorAngles( const int &iActorID, const Angle3d &ang )
{
	CBulletActor *pActor = FindBulletActor( iActorID );
	Assert( pActor );

	//calc rotation matrice
	float mat[9];
	GetAngleMatrix3x3( ang, mat );
	const btMatrix3x3 btMat( mat[0], mat[1], mat[2], mat[3], mat[4], mat[5], mat[6], mat[7], mat[8] );

	const btTransform btTrans( btMat, pActor->m_pRigidBody->getWorldTransform().getOrigin() );
	pActor->m_pRigidBody->setWorldTransform(btTrans);
}


//-----------------------------------------------------------------------
// Purpose: This one is faster than two single-set functions in a row
//-----------------------------------------------------------------------
void CBulletPhysics::VSetActorOriginAndAngles( const int &iActorID, const Vector3f &pos, const Angle3d &ang )
{
	CBulletActor *pActor = FindBulletActor( iActorID );
	Assert( pActor );

	pActor->SetTransformMatrix( pos, ang );
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::VSetActorProperties( const int &iActorID, const Vector3f &position,
	const Angle3d &angles, const Vector3f &velocity, const Angle3d &angularVelocity )
{
	CBulletActor *pActor = FindBulletActor( iActorID );
	Assert( pActor );

	pActor->SetTransformMatrix( position, angles );
	pActor->SetVelocity( velocity );
	pActor->SetAngularVelocity( velocity );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::BulletInternalTickCallback( btDynamicsWorld *const world, btScalar const timeStep )
{
	Assert( world );
	Assert( world->getWorldUserInfo() );

	CBulletPhysics *This = (CBulletPhysics*)world->getWorldUserInfo();
	
	CollisionPairs currentCollisionPairs;

	btDispatcher * const dispatcher = world->getDispatcher();
	for( int i = 0; i < dispatcher->getNumManifolds(); i++ )
	{
		btPersistentManifold const * const manifold = dispatcher->getManifoldByIndexInternal(i);
		Assert( manifold );

		//we need contact points
		if( manifold->getNumContacts() < 1 )
			continue;

		btRigidBody const *body0 = (btRigidBody*)manifold->getBody0();
		btRigidBody const *body1 = (btRigidBody*)manifold->getBody1();

		//always create pair in a predictable order
		if( body0 > body1 )
		{
			btRigidBody const *swapped = body0;
			body0 = body1;
			body1 = swapped;
		}

		CollisionPair const pair = std::make_pair( body0, body1 );
		currentCollisionPairs.insert( pair );

		//was the pair present before?
		if( This->m_previousTickCollisionPairs.find( pair ) == This->m_previousTickCollisionPairs.end() )
		{
			This->SendCollisionAddEvent( manifold, body0, body1 );
		}
	}

	CollisionPairs removedCollisionPairs;

	//use the STL set difference function to find collision pairs that existed during
	//the previous tick but not any more
	std::set_difference( This->m_previousTickCollisionPairs.begin(),
		This->m_previousTickCollisionPairs.end(),
		currentCollisionPairs.begin(),
		currentCollisionPairs.end(),
		std::inserter( removedCollisionPairs, removedCollisionPairs.begin() ) );

	for( CollisionPairs::const_iterator it = removedCollisionPairs.begin(),
		itEnd = removedCollisionPairs.end(); it != itEnd; it++ )
	{
		This->SendCollisionRemoveEvent( (btRigidBody const * const)it->first,
			(btRigidBody const * const)it->second );
	}

	//The current tick becomes the previous tick.
	This->m_previousTickCollisionPairs = currentCollisionPairs;
}


//---------------------------------------------------------------
// Purpose: Remove all Actors associated with this entitity
//---------------------------------------------------------------
void CBulletPhysics::VRemoveEntity( CBaseEntity *pEnt )
{
	int id = pEnt->GetIndex();
	CBulletActor *actor = FindBulletActor(id);
	if( !actor )
	{
		error("CBulletPhysics#VRemoveEntity() could not find actor for id %i", pEnt->GetIndex() );
		return;
	}

	btRigidBody *body = actor->m_pRigidBody;
	if( body )
	{
		m_dynamicsWorld->removeRigidBody(body);
		RigidBodyToActorIDMap::const_iterator it = m_rigidBodyToActorId.find(body);
		if( it != m_rigidBodyToActorId.end() )
			m_rigidBodyToActorId.erase(it);
	}
	else
		error("CBulletPhysics#VRemoveEntity() could not find btRigidBody for actor with id %i", id);
	
	m_actorBodies.erase(m_actorBodies.find(id));
	delete body;
	delete actor;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletPhysics::SetDebugDrawer(btIDebugDraw *drawer)
{
	m_debugDrawer = drawer;
	m_dynamicsWorld->setDebugDrawer( drawer );
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBulletPhysics::VSetCollidable( CBaseEntity* ent, bool collidable )
{
	CBulletActor *actor = FindBulletActor(ent->GetIndex());
	if( !actor )
	{
		error("CBulletPhysics#VSetCollidable() could not find actor for id %i", ent->GetIndex() );
		return;
	}

	btRigidBody *body = actor->m_pRigidBody;
	if(body)
	{
		if(collidable)
			actor->m_pRigidBody->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
		else
			actor->m_pRigidBody->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}



//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void ActorMotionState::getWorldTransform( btTransform& worldTrans ) const
{
	float mat[9];
	GetAngleMatrix3x3( m_aWorldRot, mat );
	const btMatrix3x3 btMat( mat[0], mat[1], mat[2], mat[3], mat[4], mat[5], mat[6], mat[7], mat[8] );
	btVector3 offset = btMat * m_CenterOfMassOffset;
	worldTrans = btTransform( btMat, VecToBtVec(m_vWorldPos) + offset );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void ActorMotionState::setWorldTransform( const btTransform& worldTrans )
{
	btVector3 offset = worldTrans.getBasis() * m_CenterOfMassOffset;

	const btVector3 pos = worldTrans.getOrigin();
	const btQuaternion rot = worldTrans.getRotation();

	m_vWorldPos = BtVecToVec( pos - offset );
	m_aWorldRot = BtQuatToAngle( rot );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
const Angle3d BtQuatToAngle( const btQuaternion &quat )
{
	const float w = quat.getW();
	const float x = quat.getX();
	const float y = quat.getY();
	const float z = quat.getZ();
	//const float sqw = w*w;
	const float sqx = x*x;
	const float sqy = y*y;
	const float sqz = z*z;

	Angle3d ang;

#if 1
	float test = w*y - z*x;
	if( test > ASIN_MAX_HALF )
	{
		ang.x = 0.0f;
		ang.y = 90.0f;
		ang.z = -2.0f * ATAN2(x,w);
		return ang;
	}
	if( test < -ASIN_MAX_HALF )
	{
		ang.x = 0.0f;
		ang.y = -90.0f;
		ang.z = 2.0f * ATAN2(x,w);
		return ang;
	}
#endif

	ang.x = ATAN2( 2.0f*(w*x + y*z), 1.0f - 2.0f*(sqx + sqy) );
	ang.y = ASIN( 2.0f * test );
	ang.z = ATAN2( 2.0f * (w*z + x*y), 1.0f - 2.0f*(sqy + sqz) );

	if( !ang.IsValid() )
		return Angle3d(0.0f, 0.0f, 0.0f);

	//Assert( ang.IsValid() );

	return ang;

	//ang.x = atan2( 2.0f * (y*z + x*w), (-sqx - sqy + sqz + sqw) ) * (180.0f/PI);
	//ang.y =	asin( -2.0f * (x*z - y*w) ) * (180.0f/PI);
	//ang.z = atan2( 2.0f * (x*y + z*w), (sqx - sqy - sqz + sqw) ) * (180.0f/PI);
	//return ang;
}


/*	rotxrad = atan2(2.0 * ( y*z + x*w ) , ( -sqx - sqy + sqz + sqw ));
	rotyrad = asin(-2.0 * ( x*z - y*w ));
	rotzrad = atan2(2.0 * ( x*y + z*w ) , (  sqx - sqy - sqz + sqw ));
	*/

CConVar physics_debug_lines("physics.debug.lines", "1");
CConVar physics_debug_contactpoints("physics.debug.contactpoints", "1");
CConVar physics_debug_text("physics.debug.text", "1");

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
DECLARE_CONSOLE_COMMAND(physics_debug_start, "physics.debug.start")
{
	CBulletPhysics *phys = (CBulletPhysics*)singletons::g_pPhysics;
	CBulletDebugDrawer *drawer = new CBulletDebugDrawer();
	phys->SetDebugDrawer(drawer);

	CBulletDebugDrawerCallback *callback = new CBulletDebugDrawerCallback(phys);

	CPointerDataEvent *evt = new CPointerDataEvent(ev::START_PHYS_DEBUG, (void*)callback);
	singletons::g_pEvtMgr->AddEventToQueue(evt);
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CBulletDebugDrawer::CBulletDebugDrawer()
{
	link = NULL;
	singletons::g_pEvtMgr->RegisterListener(ev::SET_PHYS_DEBUGGER, this);
}


//-----------------------------------------------------------------------
// Purpose: From IEventListener
//-----------------------------------------------------------------------
bool CBulletDebugDrawer::HandleEvent( const IEvent *evt )
{
	if( evt->GetType() == ev::SET_PHYS_DEBUGGER )
	{
		CPointerDataEvent *pevt = (CPointerDataEvent*)evt;
		link = (IPhysicsDebug*)pevt->GetPointer();
		return true;
	}
	else
		return false;
}

//-----------------------------------------------------------------------
// Purpose: From IEventListener
//-----------------------------------------------------------------------
void CBulletDebugDrawer::UnregisterEvents( void )
{
	singletons::g_pEvtMgr->UnregisterListener(ev::SET_PHYS_DEBUGGER, this);
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletDebugDrawer::drawLine(  const btVector3 &from,
	const btVector3 &to,
	const btVector3 &color )
{
	if(link && physics_debug_lines.GetBool())
		link->drawLine( BtVecToVec(from), BtVecToVec(to), BtColToCol(color) );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletDebugDrawer::drawContactPoint(  const btVector3 &PointOnB,
	const btVector3 &normalOnB,
	btScalar distance,
	int lifeTime,
	const btVector3 &color )
{
	if(link && physics_debug_contactpoints.GetBool())
		link->drawContactPoint(
			BtVecToVec(PointOnB),
			BtVecToVec(normalOnB),
			(float)distance,
			lifeTime,
			BtColToCol(color));
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletDebugDrawer::reportErrorWarning( const char *warningString )
{
	if(link)
		link->reportErrorWarning( warningString );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletDebugDrawer::draw3dText( const btVector3 &location,
	const char* textString )
{
	if(link && physics_debug_text.GetBool())
		link->draw3dText(BtVecToVec(location), textString);
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletDebugDrawer::setDebugMode( int debugMode )
{
	
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
int CBulletDebugDrawer::getDebugMode() const
{
	return -1;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletDebugDrawer::dispatch(void)
{
	if(link)
		link->dispatch();
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CBulletDebugDrawerCallback::CBulletDebugDrawerCallback(CBulletPhysics *physics)
	: m_physics(physics)
{

}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletDebugDrawerCallback::RenderDebug(void)
{
	m_physics->VRenderDiagnostics();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletActor::SetTransformMatrix( const Vector3f &pos, const Angle3d &ang )
{
	//calc rotation matrice
	float mat[9];
	GetAngleMatrix3x3( ang, mat );
	const btMatrix3x3 btMat( mat[0], mat[1], mat[2], mat[3], mat[4], mat[5], mat[6], mat[7], mat[8] );

	const btTransform btTrans( btMat, VecToBtVec(pos) );
	m_pRigidBody->setWorldTransform(btTrans);
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletActor::SetVelocity( const Vector3f &velocity )
{
	m_pRigidBody->setLinearVelocity( VecToBtVec(velocity) );
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBulletActor::SetAngularVelocity( const Angle3d &angVelocity )
{
	m_pRigidBody->setAngularVelocity( VecToBtVec( angVelocity ) );
}
