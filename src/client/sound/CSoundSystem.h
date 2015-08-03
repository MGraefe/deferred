// client/sound/CSoundSystem.h
// client/sound/CSoundSystem.h
// client/sound/CSoundSystem.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__sound__CSoundSystem_H__
#define deferred__client__sound__CSoundSystem_H__

#include "AL/al.h"
#include "AL/alc.h"
#include <list>
#include <queue>
#include <util/maths.h>


#include "CSound.h"

//Forward decl.
class CCollisionSoundEmitter;

typedef std::list<soundbuffer_t*> SoundBufferList;

class CTempSoundComparison
{
public:
	bool operator() ( const CSound *pSoundLeft, const CSound *pSoundRight ) const {
		return pSoundLeft->GetKillTime() > pSoundRight->GetKillTime();
	}
};

typedef std::priority_queue<CSound*, std::vector<CSound*>, CTempSoundComparison> TempSoundList;


class CSoundSystem
{
public:
	CSoundSystem();
	~CSoundSystem();

	void Init( void );
	void Cleanup( void );

	soundbuffer_t *LoadSound( const char *pFilename );
	void DeleteSound( soundbuffer_t *pBuffer );
	
	void SetMasterVolume( const float &vol ) const;
	void SetListenerPosition( const Vector3f &pos ) const;
	void SetListenerVelocity( const Vector3f &vel ) const;
	void SetListenerOrientation( const Vector3f &vForward, const Vector3f &vUp ) const;
	void SetListenerParameters( const Vector3f &vPosition,
								const Vector3f &vVelocity,
								const Vector3f &vForward,
								const Vector3f &vUp ) const;

	void AddTempSound( CSound *pSound );
	void UpdateTempSounds( void );

	void InitCollisionSoundEmitter( void );
	inline CCollisionSoundEmitter *GetCollisionSoundEmitter( void ) const { 
		return m_pCollisionSoundEmitter; 
	}


	//inline int GetLastError( void ) { 
	//	int iError = m_iLastError; m_iLastError = 0; return iError;
	//}

private:
	soundbuffer_t *LoadWavFile( const char *pFilename );

private:
	//int m_iLastError;
	ALCdevice *m_pDevice;
	ALCcontext *m_pContext;
	bool m_bEAX;

	CCollisionSoundEmitter *m_pCollisionSoundEmitter;

	SoundBufferList m_BufferList;
	TempSoundList m_TempSounds;
};


#endif