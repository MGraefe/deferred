// util/timer.h
// util/timer.h
// util/timer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__timer_H__
#define deferred__util__timer_H__


static const float g_InterpVal = 0.05f;

class gpGlobals_t
{
public:
	gpGlobals_t() :
		curtime(0.1f),
		frametime(0.1f),
		timescale(1.0f),
		framecount(0),
		serveroffset(0.0f),
		serveroffset_calculated(0.0f),
		rtt(0.1f),
		ping(0.1f)
	{}

public:
	float 			curtime;
	float 			frametime;
	float			timescale;
	unsigned long	framecount;
	float			serveroffset;
	volatile float	serveroffset_calculated;
	volatile float	rtt;
	float			ping;
};


float ToClientTime( float serveroffset, float fServerTime );
float ToServerTime( float serveroffset, float fClientTime );

class CCTime
{
	public:
		CCTime( gpGlobals_t *pGlobals );
		void  InitTime( void );
		void  UpdateTime( void );
		double GetTime( void ) const;
		unsigned int GetTimeMs(void) const;
		gpGlobals_t *GetGlobals( void );
		static void Sleep(unsigned int millis);

	private:
		float m_fPauseTime;
		gpGlobals_t * const m_pGlobals;
		double			lastrawtime;
		double			currawtime;
		long long int	rawStartTime;
		long long int 	rawClockTimescale;
		bool			bCounterAvailable;
		float			lastcurtime;
};

//extern CCTime timer;

//---------------------------------------------------------------------------
#endif
