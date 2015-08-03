// util/timer.cpp
// util/timer.cpp
// util/timer.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "timer.h"

#ifdef _WINDOWS
#include <mmsystem.h>
#else
#include <time.h>

double getTime(const timespec &ts)
{
	return ((double)ts.tv_sec) + ((double)ts.tv_nsec) * (1.0/(1000.0*1000.0*1000.0));
}
#endif
//#include "main.h"


float ToClientTime( float serveroffset, float fServerTime )
{
	return fServerTime - serveroffset;
}

float ToServerTime( float serveroffset, float fClientTime )
{
	return fClientTime + serveroffset;
}

void CCTime::InitTime( void )
{
	m_fPauseTime = 0;

#ifdef _WINDOWS
	//Init the Millisecond-Counter
	LARGE_INTEGER iFrequency;
	if( QueryPerformanceFrequency( &iFrequency ) )
	{
		bCounterAvailable = true;
		rawClockTimescale = iFrequency.QuadPart;
		//QueryPerformanceCounter( &iFrequency );

		LARGE_INTEGER CurrentTime;
		QueryPerformanceCounter( &CurrentTime );
		rawStartTime = CurrentTime.QuadPart;
	}
	else
	{
		bCounterAvailable = false;
		rawClockTimescale = 1000;
		rawStartTime = timeGetTime();
	}
#else
	timespec ts;
	if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
		error_fatal("Could not initialize real-time-clock");
	rawStartTime = getTime(ts);
	rawClockTimescale = -1;
	bCounterAvailable = true;
#endif

	m_pGlobals->framecount = 0;
	m_pGlobals->serveroffset = 0.0f;

	UpdateTime();
}

void CCTime::UpdateTime( void )
{
	lastrawtime = currawtime;
	currawtime = GetTime();
	m_pGlobals->frametime = float(currawtime - lastrawtime) * m_pGlobals->timescale;
	lastcurtime = m_pGlobals->curtime;
	m_pGlobals->curtime += m_pGlobals->frametime;
	m_pGlobals->framecount += 1;
	m_pGlobals->serveroffset = m_pGlobals->serveroffset_calculated;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
double CCTime::GetTime( void ) const
{
	double curtime;

#ifdef _WINDOWS
	if( bCounterAvailable )
	{
		LARGE_INTEGER CurrentTime;
		QueryPerformanceCounter( &CurrentTime );
		curtime = (float)((double)(CurrentTime.QuadPart - rawStartTime) / (double)rawClockTimescale);
	}
	else
	{
		curtime = (float)((double)(timeGetTime() - rawStartTime) / (double)rawClockTimescale);
	}
#else
	timespec ts;
	if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
		error_fatal("Could not get real-time-clock");
	curtime = getTime(ts) - rawStartTime;
#endif

	return curtime;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CCTime::CCTime( gpGlobals_t *pGlobals ) :
	m_fPauseTime(0.0f),
	m_pGlobals(pGlobals),
	rawStartTime(0),
	rawClockTimescale(0),
	bCounterAvailable(false),
	lastcurtime(0.0f),
	lastrawtime(0.0f),
	currawtime(0.0f)
{
	
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
gpGlobals_t * CCTime::GetGlobals( void )
{
	return m_pGlobals;
}

//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CCTime::Sleep(unsigned int millis)
{
#ifdef _WINDOWS
	::Sleep(millis);
#else
	usleep(millis*1000);
#endif
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
unsigned int CCTime::GetTimeMs(void) const
{
	return (unsigned int)(GetTime() * 1000.0 + 0.5);
}



