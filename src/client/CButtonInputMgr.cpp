// client/CButtonInputMgr.cpp
// client/CButtonInputMgr.cpp
// client/CButtonInputMgr.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CButtonInputMgr.h"
#include <util/CEventManager.h>
#include <util/error.h>
#include "CClientInterf.h"
#include <util/CConVar.h>

//#define LOG

CConVar input_smoothing_steer("input.smoothing.steer", "0.10");
CConVar input_smoothing_gasbreak("input.smoothing.gasbreak", "0.1");
CConVar input_smoothing_handbrake("input.smoothing.handbrake", "0.1");

CButtonInputMgr::CButtonInputMgr()
{
	m_fLastInputUpdateTime = 0.0f;
}

void CButtonInputMgr::Init(CEventManager *manager)
{
	m_pEventManager = manager;

	memset(m_keys, 0, sizeof(m_keys));
	memset(m_oldKeys, 0, sizeof(m_oldKeys));

	//m_pEventManager = new CEventManager;
	m_bMenuMode = false;

	m_KeyCarForward[0] = KeyCodes::KC_UP;
	m_KeyCarBackward[0] = KeyCodes::KC_DOWN;
	m_KeyCarLeft[0] = KeyCodes::KC_LEFT;
	m_KeyCarRight[0] = KeyCodes::KC_RIGHT;
	m_KeyCarReset[0] = KeyCodes::KC_BACK;
	m_KeyCarHandbrake[0] = KeyCodes::KC_SPACE;
	
	m_KeyCarForward[1] = KeyCodes::KC_W;
	m_KeyCarBackward[1] = KeyCodes::KC_S;
	m_KeyCarLeft[1] = KeyCodes::KC_A;
	m_KeyCarRight[1] = KeyCodes::KC_D;
	m_KeyCarReset[1] = KeyCodes::KC_R;
	m_KeyCarHandbrake[1] = KeyCodes::KC_LSHIFT;
	
	m_pContrButtonsFB[0] = new CContraryButtons(m_KeyCarBackward[0], m_KeyCarForward[0]); //Forward-Backward
	m_pContrButtonsLR[0] = new CContraryButtons(m_KeyCarLeft[0], m_KeyCarRight[0]); //Left-Right

	m_pContrButtonsFB[1] = new CContraryButtons(m_KeyCarBackward[1], m_KeyCarForward[1]); //Forward-Backward
	m_pContrButtonsLR[1] = new CContraryButtons(m_KeyCarLeft[1], m_KeyCarRight[1]); //Left-Right

	m_KeyFlyCamBackward = KeyCodes::KC_S;
	m_KeyFlyCamForward = KeyCodes::KC_W;
	m_KeyFlyCamLeft = KeyCodes::KC_A;
	m_KeyFlyCamRight = KeyCodes::KC_D;

	m_KeyScoreboard = KeyCodes::KC_TAB;

	m_pContrButFlyCamFB = new CContraryButtons( m_KeyFlyCamBackward, m_KeyFlyCamForward );
	m_pContrButFlyCamLR = new CContraryButtons( m_KeyFlyCamLeft, m_KeyFlyCamRight );

	m_pEventManager->RegisterListener(ev::KEYBOARD_PRESSED, this);
	m_pEventManager->RegisterListener(ev::KEYBOARD_RELEASED, this);
	m_pEventManager->RegisterListener(ev::MOUSE_PRESSED, this);
	m_pEventManager->RegisterListener(ev::MOUSE_RELEASED, this);
	//m_pEventManager->RegisterListener(ev::MOUSE_POS_UPDATE, this);
}

CButtonInputMgr::~CButtonInputMgr()
{

}

bool CButtonInputMgr::HandleEvent( const IEvent *evt )
{
	if(evt->GetType() == ev::KEYBOARD_PRESSED || evt->GetType() == ev::MOUSE_PRESSED)
	{
		HandleKeyDown((KeyCodes::KeyCode)((CMouseButtonEvent*)evt)->GetKeyCode());
		return true;
	}

	if(evt->GetType() == ev::KEYBOARD_RELEASED || evt->GetType() == ev::MOUSE_RELEASED)
	{
		HandleKeyUp((KeyCodes::KeyCode)((CMouseButtonEvent*)evt)->GetKeyCode());
		return true;
	}

	return false;
}


void CButtonInputMgr::UnregisterEvents( void )
{
	m_pEventManager->UnregisterListener(ev::KEYBOARD_PRESSED, this);
	m_pEventManager->UnregisterListener(ev::KEYBOARD_RELEASED, this);
	m_pEventManager->UnregisterListener(ev::MOUSE_PRESSED, this);
	m_pEventManager->UnregisterListener(ev::MOUSE_RELEASED, this);
}


void CButtonInputMgr::HandleKeyUp( KeyCodes::KeyCode key )
{
	if(key >= 0 && key < 256)
		m_keys[key] = false;

	HandleCarInputs( key );
	HandleFlyCamInputs( key );

	if(key == m_KeyScoreboard)
		m_pEventManager->AddEventToQueue(new CNoDataEvent(ev::SCOREBOARD_HIDE));
}


void CButtonInputMgr::HandleKeyDown( KeyCodes::KeyCode key )
{
	if(key >= 0 && key < 256)
		m_keys[key] = true;

	HandleCarInputs( key );
	HandleFlyCamInputs( key );

	if(key == m_KeyScoreboard)
		m_pEventManager->AddEventToQueue(new CNoDataEvent(ev::SCOREBOARD_SHOW));
}


void CButtonInputMgr::UpdateOldKeys()
{
	static_assert(sizeof(m_oldKeys) == sizeof(m_keys), "oldkeys has to have same size");
	memcpy(m_oldKeys, m_keys, sizeof(m_keys));
}


void CButtonInputMgr::HandleCarInputs( KeyCodes::KeyCode key )
{
	if(key < 0 || key >= 256 )
		return;

	for(int i = 0; i < 2; i++)
		if(key == m_KeyCarReset[i])
			m_pEventManager->AddEventToQueue( new CInputEvent(ev::CAR_RESET, 1.0f, i) );
}



void CButtonInputMgr::HandleFlyCamInputs( KeyCodes::KeyCode key )
{
	if(key < 0 || key >= 256 )
		return;

	//if( m_pContrButFlyCamFB->IsKey( key ) )
	//	m_pEventManager->AddEventToQueue( new CInputEvent(
	//		ev::FLYCAM_FB_UPDATE, (float)m_pContrButFlyCamFB->GetResultDir(m_keys), NULL ) );
	//else if( m_pContrButFlyCamLR->IsKey( key ) )
	//	m_pEventManager->AddEventToQueue( new CInputEvent(
	//		ev::FLYCAM_LR_UPDATE, (float)m_pContrButFlyCamLR->GetResultDir(m_keys), NULL ) );
}


void CButtonInputMgr::ResetAllKeys()
{
	for(int i = 0; i < 256; i++)
		if(m_keys[i])
			HandleKeyUp((KeyCodes::KeyCode)i);
	memset(m_keys, 0, sizeof(m_keys));
	memset(m_oldKeys, 0, sizeof(m_oldKeys));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CButtonInputMgr::Update()
{
	if(gpGlobals->curtime < m_fLastInputUpdateTime + 0.03f)
		return;
	m_fLastInputUpdateTime = gpGlobals->curtime;

	for( int i = 0; i < 1; i++ )
	{
		m_inputSmootherFB[i].SetSmoothing(input_smoothing_gasbreak.GetFloat());
		m_pEventManager->AddEventToQueue( new CInputEvent(
			ev::CAR_ACCEL_UPDATE, m_inputSmootherFB[i].Update(
			gpGlobals->curtime, (float)m_pContrButtonsFB[i]->GetResultDir(m_keys)), i ) );

		m_inputSmootherLR[i].SetSmoothing(input_smoothing_steer.GetFloat());
		m_pEventManager->AddEventToQueue( new CInputEvent(
			ev::CAR_STEER_UPDATE, m_inputSmootherLR[i].Update(
			gpGlobals->curtime, (float)m_pContrButtonsLR[i]->GetResultDir(m_keys)), i ) );

		m_inputSmootherHB[i].SetSmoothing(input_smoothing_handbrake.GetFloat());
		m_pEventManager->AddEventToQueue( new CInputEvent( 
			ev::CAR_BRAKE_UPDATE, m_inputSmootherHB[i].Update(
			gpGlobals->curtime, m_keys[m_KeyCarHandbrake[i]] ? 1.0f : 0.0f), i ) );
	}
}


int CContraryButtons::GetResultKey( bool *pKeys )
{
	if( pKeys[m_key1] && pKeys[m_key2] )
		return 0;
	if( pKeys[m_key1] )
		return m_key1;
	if( pKeys[m_key2] )
		return m_key2;

	return 0;
}

int CContraryButtons::GetResultDir( bool *pKeys )
{
	if( pKeys[m_key1] && pKeys[m_key2] )
		return 0;
	if( pKeys[m_key1] )
		return -1;
	if( pKeys[m_key2] )
		return 1;

	return 0;
}
