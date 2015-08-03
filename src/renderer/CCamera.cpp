// renderer/CCamera.cpp
// renderer/CCamera.cpp
// renderer/CCamera.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "glheaders.h"
#include "CCamera.h"
#include "CViewFrustum.h"
#include "CRenderInterf.h"
#include <util/CEventManager.h>
#include <util/error.h>
#include "CViewFrustum.h"
#include "renderer.h"
//#include "sound/CSoundSystem.h"

CCamera::CCamera( const int &index ) : CRenderEntity(index)
{
	m_bDebugCam = false;
	m_bDebugCamActive = false;
	m_ViewSpaceMat = new Matrix4();
	m_ProjMat = new Matrix4();
}


CCamera::~CCamera()
{
	delete m_ViewSpaceMat;
	delete m_ProjMat;
}


void CCamera::Init( bool bDebugCam, bool bDebugCamActive )
{
	m_bDebugCam = bDebugCam;
	m_bDebugCamActive = bDebugCamActive;

	//CEventManager *pMgr = g_RenderInterf->GetClientEventMgr();
	//if( !pMgr )
	//{
	//	error_fatal( "Initialization chain failed at CCamera Object" );
	//	return;
	//}

	if( !bDebugCam )
		m_pFrustum = new CViewFrustum;
}


void CCamera::Activate( float fFOV, float fNear, float fFar, float fAspectRatio )
{
	m_fFOV = fFOV;
	m_fNear = fNear;
	m_fFar = fFar;
	m_fAspectRatio = fAspectRatio;

	GetProjStack().SetPerspective( fFOV, fAspectRatio, fNear, fFar );
	m_ProjMat->Set( GetProjStack().GetMatrix() );

	if( !m_bDebugCam )
	{
		g_RenderInterf->GetRenderer()->SetActiveCam( this );
		g_RenderInterf->GetRenderer()->SetActiveFrustum( m_pFrustum );
	}
}


void CCamera::UpdateMatrices( void )
{
	if( !m_bDebugCam )
	{
		Matrix3 mat = GetAbsAngles().GetMatrix3();

		//First get right vorward up vectors
		//Vector3f vRight, vForward, vUp;
		//mat.GetRightUpForwardVectors( vRight, vUp, vForward );
		
		float fMat[16];
		mat.ToOpenGlMatrixTranspose( fMat );

		//rotate and translate the scene
		GetMVStack().SetMatrix( fMat );
		GetMVStack().Translate( -GetAbsPos() );

		m_ViewSpaceMat->Set( GetMVStack().GetMatrix() );
		//glLoadMatrixf( fMat );
		//glTranslatef( -m_vAbsPos.x, -m_vAbsPos.y, -m_vAbsPos.z );

		//Recalculate our frustum values
		m_pFrustum->CalcPerspectiveFrustumValues( m_fFOV, m_fAspectRatio, m_fNear, m_fFar, GetAbsPos(), mat.GetTranspose() );
	}
	else
	{
		glLoadIdentity();
		glRotatef( -GetAbsAngles().x, 1.0f, 0.0f, 0.0f );
		glRotatef( -GetAbsAngles().y, 0.0f, 1.0f, 0.0f );
		glRotatef( -GetAbsAngles().z, 0.0f, 0.0f, 1.0f );
		glTranslatef( -GetAbsPos().x, -GetAbsPos().y, -GetAbsPos().z );
	}
}

