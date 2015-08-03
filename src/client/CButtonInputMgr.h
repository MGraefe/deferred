// client/CButtonInputMgr.h
// client/CButtonInputMgr.h
// client/CButtonInputMgr.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CButtonInputMgr_H__
#define deferred__client__CButtonInputMgr_H__

#include <util/IEvent.h>
#include "KeyCodes.h"
#include <util/CSmoother.h>

class CEventManager;

class CContraryButtons
{
public:
	CContraryButtons() : m_key1(0), m_key2(0) { }
	CContraryButtons( int key1, int key2 ) { m_key1 = key1; m_key2 = key2; }
	void SetKeys( int key1, int key2 ) { m_key1 = key1; m_key2 = key2; }
	int GetResultKey( bool *pKeys ); //key1, 0 or key2
	int GetResultDir( bool *pKeys );
	bool IsKey( const int key ) { return m_key1 == key || m_key2 == key; }
private:
	int m_key1;
	int m_key2;
};

class CButtonInputMgr : public IEventListener
{
public:
	CButtonInputMgr();
	virtual ~CButtonInputMgr();
	void Init(CEventManager *manager);

	void UpdateOldKeys();
	bool HandleEvent( const IEvent *evt );
	void UnregisterEvents( void );
	void ResetAllKeys( void );
	void Update();

private:
	void HandleKeyUp( KeyCodes::KeyCode key );
	void HandleKeyDown( KeyCodes::KeyCode key );
	void HandleCarInputs( KeyCodes::KeyCode key );

	void HandleFlyCamInputs( KeyCodes::KeyCode key );

private:
	bool m_bMenuMode;
	bool m_keys[256];
	bool m_oldKeys[256];
	CEventManager *m_pEventManager;
	CContraryButtons *m_pContrButtonsFB[2]; //Forward-Backward
	CContraryButtons *m_pContrButtonsLR[2]; //Left-Right
	CContraryButtons *m_pContrButFlyCamLR; //Fly-cam left right
	CContraryButtons *m_pContrButFlyCamFB; //Fly-cam forward backward

	CSmoother<float> m_inputSmootherFB[2];
	CSmoother<float> m_inputSmootherLR[2];
	CSmoother<float> m_inputSmootherHB[2];

	float m_fLastInputUpdateTime;

	int m_KeyCarForward[2];
	int m_KeyCarBackward[2];
	int m_KeyCarLeft[2];
	int m_KeyCarRight[2];
	int m_KeyCarReset[2];
	int m_KeyCarHandbrake[2];

	int m_KeyFlyCamForward;
	int m_KeyFlyCamBackward;
	int m_KeyFlyCamLeft;
	int m_KeyFlyCamRight;

	int m_KeyScoreboard;
};



#endif
