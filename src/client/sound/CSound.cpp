// client/sound/CSound.cpp
// client/sound/CSound.cpp
// client/sound/CSound.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "../stdafx.h"


#include "CSound.h"
#include "CSoundSystem.h"
#include "../CClientInterf.h"
#include <util/timer.h>

CSound::CSound() : 
	m_fVolume(1.0f),
	m_fPitch(1.0f),
	m_fDuration(1.0f),
	m_bPlaying(false),
	m_bLooped(false),
	m_bLoaded(false),
	m_pSoundBuffer(NULL),
	m_ALSoundSource(-1),
	m_fKillTime(0.0f),
	m_bTemporary(false)
{

}

CSound::~CSound()
{
	if( m_ALSoundSource != UINT(-1) )
		alDeleteSources(1, &m_ALSoundSource);
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSound::SetTemporary( const bool &temp )
{
	m_bTemporary = temp;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSound::SetVolume( const float &volume )
{
	m_fVolume = volume;

	if( !m_bLoaded )
		return;

	alSourcef( m_ALSoundSource, AL_GAIN, volume );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSound::SetPitch( const float &pitch )
{
	m_fPitch = pitch;

	if( !m_bLoaded )
		return;

	alSourcef( m_ALSoundSource, AL_PITCH, pitch );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSound::SetPosition( const Vector3f &pos )
{
	m_vPosition = pos;

	if( !m_bLoaded )
		return;

	alSource3f( m_ALSoundSource, AL_POSITION, pos.x, pos.y, pos.z );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSound::SetVelocity( const Vector3f &vel )
{
	m_vVelocity = vel;

	if( !m_bLoaded )
		return;

	alSource3f( m_ALSoundSource, AL_VELOCITY, vel.x, vel.y, vel.z );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSound::SetLooped( const bool &looped )
{
	m_bLooped = looped;

	if( !m_bLoaded )
		return;

	alSourcei( m_ALSoundSource, AL_LOOPING, looped );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSound::StartSound( void )
{
	if( !m_bLoaded )
		return;

	if( m_bTemporary )
	{
		m_fKillTime = gpGlobals->curtime + m_fDuration + 0.1f;
		g_ClientInterf->GetSoundSystem()->AddTempSound(this);
	}

	alSourcePlay( m_ALSoundSource );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSound::StopSound( void )
{
	if( !m_bLoaded )
		return;

	alSourceStop( m_ALSoundSource );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSound::LoadSound( const char *pFilename )
{
	LoadSound( g_ClientInterf->GetSoundSystem()->LoadSound( pFilename ) );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSound::LoadSound( soundbuffer_t *buffer )
{
	m_pSoundBuffer = buffer;

	if( !m_pSoundBuffer )
		return;
	
	//Create a source
	alGenSources( 1, &m_ALSoundSource );

	//Bind the buffer to the source
	alSourcei( m_ALSoundSource, AL_BUFFER, m_pSoundBuffer->buffer );
	alSourcei( m_ALSoundSource, AL_SOURCE_RELATIVE, AL_FALSE );
	alSourcef( m_ALSoundSource, AL_REFERENCE_DISTANCE, 5.0f );
	alSourcef( m_ALSoundSource, AL_ROLLOFF_FACTOR, 0.1f );


	//Copy the duration
	m_fDuration = m_pSoundBuffer->fDuration;
	m_bLoaded = true;
}
