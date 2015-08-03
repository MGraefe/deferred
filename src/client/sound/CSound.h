// client/sound/CSound.h
// client/sound/CSound.h
// client/sound/CSound.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once
#ifndef deferred__client__sound__CSound_H__
#define deferred__client__sound__CSound_H__

#include <util/maths.h>

struct soundbuffer_t
{
	int iReferences;
	UINT buffer;
	float fDuration;
	char pFilename[64];
};

class CSound
{
public:
	CSound();
	~CSound();

	void SetVolume( const float &volume );
	void SetPitch( const float &pitch );
	void SetPosition( const Vector3f &pos );
	void SetVelocity( const Vector3f &vel );
	void StartSound( void );
	void StopSound( void );
	void LoadSound( const char *pFilename );
	void LoadSound( soundbuffer_t *buffer );
	void SetLooped( const bool &looped );
	void SetTemporary( const bool &temp );

	inline float GetVolume( void ) const { return m_fVolume; }
	inline float GetPitch( void ) const { return m_fPitch; }
	inline float GetDuration( void ) const { return m_fDuration; }
	inline const float &GetKillTime( void ) const { return m_fKillTime; }
	inline const Vector3f &GetPosition( void ) const { return m_vPosition; }
	inline const Vector3f &GetVelocity( void ) const { return m_vVelocity; }
	//char *GetFilename( void ) { return m_pFilename; }
	inline bool IsPlaying( void ) const { return m_bPlaying; }
	inline bool IsLooped( void ) const { return m_bLooped; }
	
private:
	float m_fVolume;
	float m_fPitch;
	float m_fDuration;
	Vector3f m_vPosition;
	Vector3f m_vVelocity;
	//char m_pFilename[64];
	bool m_bPlaying;
	bool m_bLooped;
	bool m_bLoaded;
	bool m_bTemporary;
	float m_fKillTime;

	soundbuffer_t *m_pSoundBuffer;
	unsigned int m_ALSoundSource;
};


#endif