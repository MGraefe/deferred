// client/CClientPlayerCar.h
// client/CClientPlayerCar.h
// client/CClientPlayerCar.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__client__CClientPlayerCar_H__
#define deferred__client__CClientPlayerCar_H__

#include <renderer/Interfaces.h>
#include <renderer/CModel.h>
#include <math.h>
#include <util/CSmoothSteppedInterpolator.h>
#include <util/CSmoother.h>

class CCamera;
class CSound;
class CSpotTexLight;
class CClientPlayerCar : public CModel
{
	DECLARE_DATADESC_EDITOR(CModel, CClientPlayerCar)
	DECLARE_ENTITY_LINKED();
public:
	CClientPlayerCar( const int &index );

	virtual void UpdateInterpolation( const float &alpha );
	virtual void OnSpawn();
	void UpdateCameraPosition( void );
	/*bool HandleEvent( const IEvent *evt );*/
	void VPreDelete( void );

	bool IsUnderWater(const Vector3f &pos, float margin);

	float GetRpm( void ) { return (float)m_engineRpm; }
	float GetEngineLoad( void ) { return (float)m_engineLoad / 100.0f; }
	int GetGear( void ) { return (int)m_engineGear; }
	int GetPlayerId( void ) { return m_playerCarId; }

	static const std::vector<CClientPlayerCar*> &GetPlayerCars() { return s_playerCars; }
	float GetSpeedKmh();
private:
	static std::vector<CClientPlayerCar*> s_playerCars;
	Vector3f m_vLastCamPosition;
	Vector3f m_vLastCamDirection;
	Vector3f m_vLastMoveDirection;
	bool m_bLocalCar;
	CCamera *m_pCam;
	CSound *m_pSound;
	CSound *m_pNoLoadSound;
	CSpotTexLight *m_pSpotLight;
	float m_fLastThrottleTime;
	CSmoother<Vector3f> m_camPosInterp;
	CSmoother<Vector3f> m_camDirInterp;

	SHORT m_engineRpm;
	BYTE m_engineGear;
	BYTE m_engineLoad;
	BYTE m_playerCarId;
};


extern CClientPlayerCar *g_ClientPlayerCar;
CClientPlayerCar *GetLocalPlayerCar( void );


#endif
