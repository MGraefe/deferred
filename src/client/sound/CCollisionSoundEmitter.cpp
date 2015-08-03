// client/sound/CCollisionSoundEmitter.cpp
// client/sound/CCollisionSoundEmitter.cpp
// client/sound/CCollisionSoundEmitter.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "../stdafx.h"

#include "CCollisionSoundEmitter.h"
#include "../CClientInterf.h"
#include <util/CEventManager.h>
#include "CSound.h"

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCollisionSoundEmitter::Init( void )
{
	CSoundSystem *pSndSys = g_ClientInterf->GetSoundSystem();
	m_Sounds.push_back( 
		collisionsound_t(
			pSndSys->LoadSound( "sounds/vehicle_impact_medium1.wav" ),
			0.7f ) );
	m_Sounds.push_back( 
		collisionsound_t(
			pSndSys->LoadSound( "sounds/vehicle_impact_medium2.wav" ),
			0.7f ) );
	m_Sounds.push_back( 
		collisionsound_t(
			pSndSys->LoadSound( "sounds/vehicle_impact_medium3.wav" ),
			0.7f ) );
	m_Sounds.push_back( 
		collisionsound_t(
			pSndSys->LoadSound( "sounds/vehicle_impact_medium4.wav" ),
			0.7f ) );

	m_Sounds.push_back( 
		collisionsound_t(
			pSndSys->LoadSound( "sounds/vehicle_impact_heavy1.wav" ),
			1.0f ) );
	m_Sounds.push_back( 
		collisionsound_t(
			pSndSys->LoadSound( "sounds/vehicle_impact_heavy2.wav" ),
			1.0f ) );
	m_Sounds.push_back( 
		collisionsound_t(
			pSndSys->LoadSound( "sounds/vehicle_impact_heavy3.wav" ),
			1.0f ) );
	m_Sounds.push_back( 
		collisionsound_t(
			pSndSys->LoadSound( "sounds/vehicle_impact_heavy4.wav" ),
			1.0f ) );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCollisionSoundEmitter::RegisterEvents( void )
{
	g_ClientInterf->GetClientEventMgr()->RegisterListener( ev::COLLISION_OCCURED, this );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CCollisionSoundEmitter::HandleEvent( const IEvent *evt )
{
	if( evt->GetType() != ev::COLLISION_OCCURED )
		return false;

	CCollisionEvent *pEvt = (CCollisionEvent*)evt;
	
	float fForceThreshold = 5000.0f;
	int iSoundToEmit = 0;

	if( pEvt->GetForce() < fForceThreshold )
		iSoundToEmit = RandomInt(0,3);
	else
		iSoundToEmit = RandomInt(4,7);
	
	EmitSound( &m_Sounds[iSoundToEmit], pEvt->GetPosition() );

	return true;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCollisionSoundEmitter::EmitSound( collisionsound_t *pSnd, const Vector3f &vPos )
{
	CSound *pSound = new CSound;
	pSound->LoadSound( pSnd->soundbuffer );
	pSound->SetVolume( pSnd->volume );
	pSound->SetPosition( vPos );
	pSound->SetLooped( false );
	pSound->SetTemporary( true );
	pSound->SetPitch( RandomFloat(0.7f,1.3f) );
	pSound->StartSound();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCollisionSoundEmitter::UnregisterEvents( void )
{
	g_ClientInterf->GetClientEventMgr()->UnregisterListener( ev::COLLISION_OCCURED, this );
}
