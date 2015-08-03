// client/sound/CSoundSystem.cpp
// client/sound/CSoundSystem.cpp
// client/sound/CSoundSystem.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "../stdafx.h"

#include "CSoundSystem.h"
#include "CSound.h"
#include <util/instream.h>
#include <util/timer.h>
#include "CCollisionSoundEmitter.h"
#include "../CClientInterf.h"

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CSoundSystem::CSoundSystem()
{
	m_pDevice = NULL;
	m_pContext = NULL;
	m_bEAX = false;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CSoundSystem::~CSoundSystem()
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSoundSystem::Init( void )
{
	// Initialization
	m_pDevice = alcOpenDevice( "Generic Hardware" );
	if( !m_pDevice )
	{
		m_pDevice = alcOpenDevice( NULL ); // select the "preferred device"
		if( !m_pDevice )
		{
			error( "Could open any sound device." );
			return;
		}
		else
			ConsoleMessage( "Could not open \"Generic Hardware\" device, using standard sound device instead" );
	}
	
	//Get Format
	//ALenum format = alGetEnumValue( "AL_FORMAT_51CHN16" );
	//if( format == 0 )
	//{
	//	error( "Couldnt get 5.1 format." );
	//	return;
	//}


	m_pContext = alcCreateContext( m_pDevice, NULL );
	alcMakeContextCurrent( m_pContext );

	
	// Check for EAX 2.0 support
	m_bEAX = alIsExtensionPresent("EAX2.0") == 1;

	alSpeedOfSound( 3433.0f ); //Speed of sound in decimetre units
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSoundSystem::InitCollisionSoundEmitter( void )
{
	m_pCollisionSoundEmitter = new CCollisionSoundEmitter;
	m_pCollisionSoundEmitter->Init();
	m_pCollisionSoundEmitter->RegisterEvents();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSoundSystem::Cleanup( void )
{
	alcMakeContextCurrent( NULL );
	
	if( m_pContext )
		alcDestroyContext( m_pContext );

	if( m_pDevice )
		alcCloseDevice( m_pDevice );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSoundSystem::SetListenerPosition( const Vector3f &pos ) const
{
	alListener3f( AL_POSITION, pos.x, pos.y, pos.z );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSoundSystem::SetListenerVelocity( const Vector3f &vel ) const
{
	alListener3f( AL_VELOCITY, vel.x, vel.y, vel.z );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSoundSystem::SetListenerOrientation( const Vector3f &vForward, const Vector3f &vUp ) const
{
	float ori[6] = { vForward.x, vForward.y, vForward.z, vUp.x, vUp.y, vUp.z };
	alListenerfv( AL_ORIENTATION, ori );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSoundSystem::SetListenerParameters( const Vector3f &vPosition,
											const Vector3f &vVelocity,
											const Vector3f &vForward,
											const Vector3f &vUp ) const
{
	SetListenerPosition( vPosition );
	SetListenerVelocity( vVelocity );
	SetListenerOrientation( vForward, vUp );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSoundSystem::SetMasterVolume( const float &vol ) const
{
	alListenerf( AL_GAIN, vol );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
soundbuffer_t *CSoundSystem::LoadSound( const char *pFilename )
{
	//try to find this file (maybe already loaded)
	SoundBufferList::iterator it = m_BufferList.begin();
	for( ; it != m_BufferList.end(); it++ )
	{
		soundbuffer_t *pBuf = *it;
		if( strcmp( pBuf->pFilename, pFilename ) == 0 )
		{
			pBuf->iReferences++;
			return pBuf;
		}
	}

	//File not already loaded, load it!
	return LoadWavFile( pFilename );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSoundSystem::DeleteSound( soundbuffer_t *pBuffer )
{
	if( pBuffer->iReferences == 1 )
	{
		//erase the entry from our list
		SoundBufferList::iterator it = m_BufferList.begin();
		for( ; it != m_BufferList.end(); it++ )
		{
			if( *it == pBuffer )
			{
				m_BufferList.erase( it );
				break;
			}
		}
		
		//Assert that we found the buffer in our list
		Assert( it != m_BufferList.end() );
		
		//erase the buffer
		alDeleteBuffers( 1, &pBuffer->buffer );
		delete pBuffer;
	}
	else
	{
		pBuffer->iReferences--;
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSoundSystem::AddTempSound( CSound *pSound )
{
	m_TempSounds.push( pSound );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CSoundSystem::UpdateTempSounds( void )
{
	while( !m_TempSounds.empty() && m_TempSounds.top()->GetKillTime() < gpGlobals->curtime )
	{
			//ConsoleMessage("Deleted Temporary sound %i", (int)m_TempSounds.top() );
			delete m_TempSounds.top();
			m_TempSounds.pop();
	}
}



#pragma pack(push,2)
struct waveheader_t
{
	UINT RiffMagic;
	UINT fileSizeMinusEight;
	UINT WaveMagic;
	UINT FmtMagic;
	UINT FmtHeaderLength;
	USHORT audioFormat;
	USHORT audioChannels;
	UINT sampleRate;
	UINT bytesPerSecond;
	USHORT blockAlign;
	USHORT bitsPerSample;
};
#pragma pack(pop)

inline UINT ToMagic( const char *pStr )
{
	return ((pStr[3]<<24) + (pStr[2]<<16) + (pStr[1]<<8) + pStr[0]);
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
soundbuffer_t *CSoundSystem::LoadWavFile( const char *pFilename )
{
	fs::ifstream is( pFilename, std::ios::binary | std::ios::in );
	if( !is.is_open() )
	{
		is.close();
		error( "SoundSystem: File not found: %s", pFilename );
		return NULL;
	}

	waveheader_t header;
	is.read( (char*)&header, sizeof(header) );

	if( is.eof() || 
		header.RiffMagic != ToMagic( "RIFF" ) || 
		header.WaveMagic != ToMagic( "WAVE" ) ||
		header.FmtMagic != ToMagic( "fmt " ) ||
		header.audioFormat != 0x0001 || //PCM
		( header.bitsPerSample != 8 && header.bitsPerSample != 16 ) ||
		( header.audioChannels != 1 && header.audioChannels != 2 ) )
	{
		is.close();
		error( "SoundSystem: Unsupported Filetype: %s", pFilename );
		return NULL;
	}

	//determine format
	int alFormat = -1;
	if( header.bitsPerSample == 8 )
	{
		if( header.audioChannels == 1 )
			alFormat = AL_FORMAT_MONO8;
		else
			alFormat = AL_FORMAT_STEREO8;
	}
	else if( header.bitsPerSample == 16 )
	{
		if( header.audioChannels == 1 )
			alFormat = AL_FORMAT_MONO16;
		else
			alFormat = AL_FORMAT_STEREO16;
	}

	UINT dataMagic;
	is.read( (char*)&dataMagic, 4 );

	if( dataMagic != ToMagic( "data" ) )
		return NULL;

	UINT pcmLength;
	is.read( (char*)&pcmLength, 4 );

	if( pcmLength > 100*1024*1024 ) //Maximum File size 100MB
		return NULL;

	//Read the data
	char *pcmBuffer = new char[pcmLength];
	is.read( pcmBuffer, pcmLength );
	is.close();

	//Load the sound into OpenAL
	ALuint albuffer;
	alGenBuffers(1, &albuffer);
	alBufferData( albuffer, alFormat, pcmBuffer, pcmLength, header.sampleRate );
	delete[] pcmBuffer; //not longer needed

	//fill the soundbuffer_t structure
	soundbuffer_t *pSoundbuffer = new soundbuffer_t;
	pSoundbuffer->buffer = albuffer;
	pSoundbuffer->fDuration = (float)pcmLength / (float)(header.sampleRate * (header.bitsPerSample/8) * header.audioChannels);
	pSoundbuffer->iReferences = 1;
	strcpy( pSoundbuffer->pFilename, pFilename );

	//push back in our list
	m_BufferList.push_back( pSoundbuffer );
	return pSoundbuffer;
}
