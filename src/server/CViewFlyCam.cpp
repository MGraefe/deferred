// server/CViewFlyCam.cpp
// server/CViewFlyCam.cpp
// server/CViewFlyCam.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#include "stdafx.h"

#if 0

#include "CViewFlyCam.h"
#include "CCamera.h"
#include "CGraphicInterf.h"
#include "CEventManager.h"
#include "CAppInterfaces.h"
#include "CButtonInputMgr.h"

CViewFlyCam::CViewFlyCam()
{
	m_vLookDir.Init( 0.0f, 0.0f, -1.0f );
	m_vLookRight.Init( 1.0f, 0.0f, 0.0f );
	m_aAngle.Init( 0.0f, 0.0f, 0.0f );
	m_fSpeedForward = 0;
	m_fSpeedSideward = 0;
	m_fMoveFB = 0.0f;
	m_fMoveLR = 0.0f;
	m_vMouseMove.Init( 0.0f, 0.0f );
	m_bIsDebugCam = false;
}


bool CViewFlyCam::HandleEvent( IEvent const &Event )
{
	switch( Event.GetType() )
	{
		case ev::MOUSE_POS_UPDATE:
			{
				CMouseMoveEvent *pEvent = (CMouseMoveEvent*)&Event;
				m_vMouseMove.x = (float)pEvent->GetX();
				m_vMouseMove.y = (float)pEvent->GetY();
				return true;
			}
		case ev::FLYCAM_LR_UPDATE:
			m_fMoveLR = ((CInputEvent*)&Event)->GetVal();
			return true;
		case ev::FLYCAM_FB_UPDATE:
			m_fMoveFB = ((CInputEvent*)&Event)->GetVal();
			return true;
	}

	return false;	
}


void CViewFlyCam::CalculateLookPosAndDir( void )
{
	Vector3d aLookAngle( -m_vMouseMove.y, -m_vMouseMove.x, 0.0f );
	m_aAngle += aLookAngle * 0.1f;
	m_aAngle.x = clamp( m_aAngle.x, -90.0f, 90.0f );

	AngleToVector( m_aAngle, m_vLookDir );
	m_vLookDir.Normalize();
	AngleToVector( Vector3d( 0.0f, m_aAngle.y-90.0f, 0.0f ), m_vLookRight );
	m_vLookRight.Normalize();
	
	const float fMaxSpeed = 400.0f;
	const float fMaxSpeedSideward = 300.0f;
	const float fTimeToMaxSpeed = 0.5f;
	const float fTimeToMaxSpeedSideward = 0.2f;

	if( m_fMoveFB > 0.5f )
		m_fSpeedForward = clamp( m_fSpeedForward + gpGlobals.frametime * fMaxSpeed/fTimeToMaxSpeed, -fMaxSpeed, fMaxSpeed );
	else if( m_fMoveFB < -0.5f )
		m_fSpeedForward = clamp( m_fSpeedForward - gpGlobals.frametime * fMaxSpeed/fTimeToMaxSpeed, -fMaxSpeed, fMaxSpeed );
	else if( m_fSpeedForward < 0 )
		m_fSpeedForward = clamp( m_fSpeedForward + gpGlobals.frametime * fMaxSpeed/fTimeToMaxSpeed, -fMaxSpeed, 0 );
	else 
		m_fSpeedForward = clamp( m_fSpeedForward - gpGlobals.frametime * fMaxSpeed/fTimeToMaxSpeed, 0, fMaxSpeed );

	if( m_fMoveLR > 0.5f)
		m_fSpeedSideward = clamp( m_fSpeedSideward + gpGlobals.frametime * fMaxSpeed/fTimeToMaxSpeedSideward,
										-fMaxSpeedSideward, fMaxSpeedSideward );
	else if( m_fMoveLR < -0.5f )
		m_fSpeedSideward = clamp( m_fSpeedSideward - gpGlobals.frametime * fMaxSpeed/fTimeToMaxSpeedSideward,
										-fMaxSpeedSideward, fMaxSpeedSideward );
	else if( m_fSpeedSideward < 0 )
		m_fSpeedSideward = clamp( m_fSpeedSideward + gpGlobals.frametime * fMaxSpeed/fTimeToMaxSpeedSideward,
										-fMaxSpeedSideward, 0 );
	else 
		m_fSpeedSideward = clamp( m_fSpeedSideward - gpGlobals.frametime * fMaxSpeed/fTimeToMaxSpeedSideward,
										0, fMaxSpeedSideward );

	m_vOrigin += m_vLookDir * m_fSpeedForward * gpGlobals.frametime;
	m_vOrigin += m_vLookRight * m_fSpeedSideward * gpGlobals.frametime;

	//Reset mouse move values
	m_vMouseMove.x = m_vMouseMove.y = 0.0f;
}


void CViewFlyCam::Spawn( void )
{
	//ShowCursor( FALSE );

	CEventManager *pMgr = g_AppInterf->GetButtonInputMgr()->GetButtonEventManager();
	pMgr->RegisterListener( ev::FLYCAM_LR_UPDATE, this );
	pMgr->RegisterListener( ev::FLYCAM_FB_UPDATE, this );
	pMgr->RegisterListener( ev::MOUSE_POS_UPDATE, this );
}


void CViewFlyCam::Update( void )
{
	CalculateLookPosAndDir();

	CEventManager* const pMgr = g_GraphicInterf->GetGraphicsEventMgr();

	//int iPosEvent = m_bIsDebugCam ? ev::CAMERA_DEBUG_POS_CHANGED : ev::CAMERA_POS_CHANGED;
	//int iOriEvent = m_bIsDebugCam ? ev::CAMERA_DEBUG_ORI_CHANGED : ev::CAMERA_ORI_CHANGED;

	//if( m_vOrigin != m_vLastOrigin )
	//{
	//	pMgr->AddEventToQueue( new CPosEvent( iPosEvent, this->GetIndex(), m_vOrigin ) );
	//}

	//if( m_aAngle != m_aLastAngle )
	//{
	//	pMgr->AddEventToQueue( new CPosEvent( iOriEvent, this->GetIndex(), m_aAngle ) );
	//}
}

#endif
