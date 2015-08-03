// server/CViewTracer.cpp
// server/CViewTracer.cpp
// server/CViewTracer.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#if 0

#include "CEntityList.h"
#include "CViewTracer.h"
#include "collision.h"
#include "debug.h"
#include "scenetree.h"
CViewTracer::CViewTracer()
{
	m_pCam = NULL;
	m_pWorld = NULL;
	m_pSphere = NULL;
}

void CViewTracer::Init( CCamera *pCam, rpm_file_t *pWorld )
{
	m_pCam = pCam;
	m_pWorld = pWorld;

	//m_pSphere = CREATE_ENTITY( CModel );
	//m_pSphere->Init( "models/sphere.rpm", "PixelLighting", Vector3d( 0, 0, 0 ), Vector3d( 0, 0, 0 ) );
	//m_pSphere->SetVisible( true );
}

void CViewTracer::Render( void )
{
	if( !m_pCam || !m_pWorld /*|| !m_pSphere*/ )
		return;

	CTrace trace;
	trace.vStart = m_pCam->GetAbsOrigin() + Vector3d( 0.0f, -100.0f, 0.0f );
	AngleToVector( m_pCam->GetAbsAngles(), trace.vDir );
	trace.vDir = -trace.vDir;
	trace.fMaxLength = 16384.0f;

	//for( int i = 0; i < 50; i++ ) //performance testing.
		BaseTrace( m_pWorld, &trace );

	//Draw a sphere at our end!
	//m_pSphere->SetAbsOrigin( trace.vEnd );

	Debug::DrawLine( trace.vStart, trace.vEnd, Color( 255, 0, 0 ) );
	Debug::DrawCube( trace.vEnd, 40.0f, Color(0, 255, 0 ), 3.0f );

	Debug::DrawLine( trace.vEnd, trace.vEnd + trace.vNorm * 40.0f, Color( 255, 255, 0 ), 4.0f );

}

#endif
