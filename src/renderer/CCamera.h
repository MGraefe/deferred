// renderer/CCamera.h
// renderer/CCamera.h
// renderer/CCamera.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CCamera_H__
#define deferred__renderer__CCamera_H__

#include <util/maths.h>
#include "CRenderEntity.h"
#include <util/dll.h>

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING

class CViewFrustum;
class DLL CCamera : public CRenderEntity
{
public:
	CCamera( const int &index );
	~CCamera();
	void Activate( float fFOV, float fNear, float fFar, float fAspectRatio );
	void Init( bool bDebugCam, bool bDebugCamActive );
	
	void VRender( const CRenderParams &params ) { }
	void VOnRestore( void ) { }
	void VPreDelete( void ) { }

	void UpdateMatrices( void );

	CViewFrustum *const GetFrustum( void ) const { return m_pFrustum; }
	void SetDebugCam( const bool bDebug ) { m_bDebugCam = bDebug; }

	const Matrix4f &GetViewSpaceMat( void ) const { return *m_ViewSpaceMat; }
	const Matrix4f &GetProjMat( void ) const { return *m_ProjMat; }

	float GetFov( void ) { return m_fFOV; }
	float GetFar( void ) { return m_fFar; }
	float GetNear( void ) { return m_fNear; }
	float GetAspect( void ) { return m_fAspectRatio; }

private:
	float m_fFOV;
	float m_fNear;
	float m_fFar;
	float m_fAspectRatio;

	CViewFrustum *m_pFrustum;
	bool m_bDebugCam;
	bool m_bDebugCamActive;

	Matrix4f *m_ViewSpaceMat;
	Matrix4f *m_ProjMat;
};

POP_PRAGMA_WARNINGS

#endif
