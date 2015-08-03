// client/sound/CCollisionSoundEmitter.h
// client/sound/CCollisionSoundEmitter.h
// client/sound/CCollisionSoundEmitter.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__sound__CCollisionSoundEmitter_H__
#define deferred__client__sound__CCollisionSoundEmitter_H__

#include <util/eventlist.h>
#include "CSoundSystem.h"
#include <vector>

struct collisionsound_t
{
	collisionsound_t( soundbuffer_t *buffer, const float &vol )
		: soundbuffer(buffer), volume(vol) { }
	soundbuffer_t *soundbuffer;
	float volume;
};

class CCollisionSoundEmitter : public IEventListener
{
public:
	void Init( void );
	void RegisterEvents( void );
	bool HandleEvent( const IEvent *evt );
	void UnregisterEvents( void );

private:
	void EmitSound( collisionsound_t *pSnd, const Vector3f &vPos );
	std::vector<collisionsound_t> m_Sounds;

};


#endif