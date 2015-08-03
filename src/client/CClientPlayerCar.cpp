// client/CClientPlayerCar.cpp
// client/CClientPlayerCar.cpp
// client/CClientPlayerCar.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "CClientPlayerCar.h"
#include <renderer/CCamera.h>
#include "CClientInterf.h"
#include <renderer/renderer.h>
#include <util/timer.h>
#include <util/eventlist.h>
#include "CClientInterf.h"
#include "sound/CSoundSystem.h"
#include "sound/CSound.h"
#include <util/CEventManager.h>
#include <renderer/CLightList.h>
#include <util/CConVar.h>
#include "datadesc_client.h"

LINK_ENTITY_TO_CLASSNAME_CLIENT(CClientPlayerCar, player_car);
BEGIN_DATADESC_EDITOR(CClientPlayerCar)
	NETTABLE_ENTRY(m_playerCarId)
	NETTABLE_ENTRY(m_engineRpm)
	NETTABLE_ENTRY(m_engineGear)
	NETTABLE_ENTRY(m_engineLoad)
END_DATADESC_EDITOR()


CClientPlayerCar *g_ClientPlayerCar = NULL;

std::vector<CClientPlayerCar*> CClientPlayerCar::s_playerCars = std::vector<CClientPlayerCar*>();

CClientPlayerCar *GetLocalPlayerCar( void )
{
	return g_ClientPlayerCar;
}



CClientPlayerCar::CClientPlayerCar( const int &index ) : 
	CModel(index),
	//m_camDirInterp(1.0f/20.0f, 100, 0.1f, Vector3f(0.0f, 0.0f, -1.0f)),
	//m_camPosInterp(1.0f/20.0f, 100, 0.1f),
	m_camDirInterp(Vector3f(0.0f, 0.0f, -1.0f), 10.0f, 0.0f),
	m_camPosInterp(vec3_null, 10.0f, 0.0f),
	m_pCam(NULL),
	m_engineRpm(1000),
	m_engineGear(1),
	m_engineLoad(0)
{

}

void CClientPlayerCar::VPreDelete( void )
{
	removeFromVector(s_playerCars, this);

	ConsoleMessage( "Stopping Sound on Client car..." );
	m_pSound->StopSound();
	delete m_pSound;

	g_RenderInterf->GetRenderer()->GetLightList()->RemoveLight(m_pSpotLight->GetLightIndex());
	delete m_pSpotLight;
}


void CClientPlayerCar::OnSpawn()
{
	BaseClass::OnSpawn();

	ConsoleMessage("Spawned Player car with client id %i", (int)m_playerCarId);

	if( m_playerCarId == g_ClientInterf->GetLocalCliendID() )
	{
		g_ClientPlayerCar = this;

		//Create a cam
		m_pCam = new CCamera( -1 );
		m_pCam->Init( false, false );
		g_ClientInterf->GetRenderer()->SetPlayerOneCam( m_pCam );

		m_vLastCamDirection = Vector3f(0.0f, 0.0f, -1.0f);
		m_vLastCamPosition = GetAbsPos();
		//update camera position once to prevent glitching
		//UpdateCameraPosition();
	}

	//Create a sound
	m_pSound = new CSound;
	m_pSound->LoadSound( "sounds/v8/v8_firstgear_rev_loop1.wav" );
	m_pSound->SetLooped( true );
	m_pSound->SetVolume( 0.0f );
	m_pSound->StartSound();

	//Create the spot light
	m_pSpotLight = new CSpotTexLight();
	m_pSpotLight->SetTexFilename("textures/lighttex.tga");
	m_pSpotLight->SetColor( Vector4f(1.0f, 1.0f, 1.0f, 60.0f) );
	Vector3f clq(0.0f, 1000.0f, 30.0f);
	clq.Normalize();
	m_pSpotLight->SetConstant( clq.x );
	m_pSpotLight->SetLinear( clq.y );
	m_pSpotLight->SetQuadratic( clq.z );
	m_pSpotLight->SetFov(45.0f);
	m_pSpotLight->SetAspect(3.5f);
	m_pSpotLight->SetEnabled(true);
	m_pSpotLight->LoadTex();
	g_RenderInterf->GetRenderer()->GetLightList()->AddSpotTexLight(m_pSpotLight);

	s_playerCars.push_back(this);
}


CConVar camera_smooth("camera.smooth", "1");
CConVar camera_height("camera.height", "30");
CConVar camera_dist("camera.dist", "40");
CConVar camera_look_height("camera.look.height", "30");
CConVar camera_smooth_factor("camera.smooth.factor", "0.6");
CConVar camera_speed_min("camera.speed.min", "5.0");
CConVar camera_movement_scaley("camera.movement.scaley", "0.0");

void CClientPlayerCar::UpdateCameraPosition( void )
{
	if( !m_pCam )
		return;

	Quaternion aCarDir = GetAbsAngles();
	Vector3f vCarForward = aCarDir * Vector3f(0.0f, 0.0f, 1.0f);

	const Vector3f &vCarVelocity = GetVelocity();
	Vector3f vCarMoveDir = vCarVelocity;
	
	vCarMoveDir.y *= camera_movement_scaley.GetFloat();
	if(vCarMoveDir.LengthSq() < camera_speed_min.GetFloat()*camera_speed_min.GetFloat())
		vCarMoveDir = vCarForward;
	else
		vCarMoveDir.Normalize();

	//m_camDirInterp.SetAlpha(camera_smooth_factor.GetFloat());
	//m_camDirInterp.SetNew(vCarMoveDir);
	m_camDirInterp.SetSmoothing(camera_smooth_factor.GetFloat());

	//vCarMoveDir = m_camDirInterp.Update(gpGlobals->curtime);

	//float moveDirAlpha = clamp(gpGlobals->frametime * camera_smooth_factor.GetFloat(), 0.0f, 1.0f);
	//vCarMoveDir = m_vLastMoveDirection * moveDirAlpha + vCarMoveDir * (1.0f-moveDirAlpha);
	//m_vLastMoveDirection = vCarMoveDir;

	Vector3f vCarPos = GetAbsPos();

	//float alpha = clamp(gpGlobals->frametime * camera_smooth_factor.GetFloat(), 0.0f, 1.0f); //Diese Gewichtungen funktionieren so nicht.
	Vector3f vCamDesiredPos = (vCarPos + vCarMoveDir * -camera_dist.GetFloat());
	//m_camPosInterp.SetAlpha(camera_smooth_factor.GetFloat());
	m_camPosInterp.SetSmoothing(camera_smooth_factor.GetFloat());
	//m_camPosInterp.SetNew(vCamDesiredPos);
	Vector3f vCamPos;
	if( camera_smooth.GetBool() )
		vCamPos = m_camPosInterp.Update(gpGlobals->curtime, vCamDesiredPos);
	else
		vCamPos = vCamDesiredPos;

	//Do not follow car under water
	if( IsUnderWater(vCamPos, 1.0f) /*|| (IsUnderWater(vCarPos, -15.0f) && IsUnderWater(vCamPos, 10.0f))*/ ) 
		vCamPos.y = m_vLastCamPosition.y;

	m_vLastCamPosition = vCamPos;

	vCamPos.y += camera_height.GetFloat();
	Vector3f vCamLookTarget = vCarPos + Vector3f( 0, camera_look_height.GetFloat(), 0 ) /*+ vForward * 100.0f*/;
	Vector3f vCamDir = ((vCamLookTarget - vCamPos).Normalize());

	m_vLastCamDirection = vCamDir;
	Angle3d aCamDir;
	VectorToAngle( vCamDir, aCamDir );

	//Set Camera position
	Vector3f vVelocity = (vCamPos - m_pCam->GetAbsPos()) * (1.0f/gpGlobals->frametime);
	m_pCam->SetAbsPos( vCamPos );
	m_pCam->SetAbsAngles( Quaternion(aCamDir) );

	//update the sound listener position:
	Vector3f vCamForward, vCamUp, vCamRight;
	m_pCam->GetAbsAngles().GetMatrix3().GetRightUpForwardVectors( vCamRight, vCamUp, vCamForward );
	g_ClientInterf->GetSoundSystem()->SetListenerParameters( m_pCam->GetAbsPos(), vVelocity, vCamForward, vCamUp );
}


void CClientPlayerCar::UpdateInterpolation( const float &alpha )
{
	CModel::UpdateInterpolation(alpha);

	UpdateCameraPosition();

	float fPitchLoad = lerp( GetRpm(), 1000.0f, 5000.0f, 0.5f, 1.5f );
	//float fPitchNoLoad = lerp( GetRpm(), 1000.0f, 5000.0f, 1.0f, 4.0f );
	float fVolume = lerp( GetEngineLoad(), 0.0f, 1.0f, 0.4f, 1.0f );

	m_pSound->SetPitch( fPitchLoad );
	m_pSound->SetVolume( fVolume );
	m_pSound->SetPosition( GetAbsPos() );
	m_pSound->SetVelocity( GetVelocity() );

	Vector3f vForward, vRight, vUp;
	GetAbsAngles().GetMatrix3().GetRightUpForwardVectors(vRight, vUp, vForward);
	m_pSpotLight->SetAbsPos( GetAbsPos() + vUp * 10.0f + vForward * -18.0f);

	Quaternion rot = Quaternion(vRight, 9.7f) * Quaternion(vUp, 180.0f) * GetAbsAngles();
	m_pSpotLight->SetAbsAngles( rot );
}


bool CClientPlayerCar::IsUnderWater(const Vector3f &pos, float margin)
{
	float waterHeight = g_RenderInterf->GetRenderer()->GetWaterHeight();
	bool waterActive = g_RenderInterf->GetRenderer()->IsWaterEnabled();

	return waterActive && pos.y < waterHeight + margin;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CClientPlayerCar::GetSpeedKmh()
{
	return GetVelocity().Length() * 0.36f;
}
