// renderer/CViewFrustum.h
// renderer/CViewFrustum.h
// renderer/CViewFrustum.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CViewFrustum_H__
#define deferred__renderer__CViewFrustum_H__

#include <util/maths.h>

class CViewFrustum
{
public:
	CViewFrustum();

	void CalcPerspectiveFrustumValues( const float &fFOV, const float &fAspectRatio, const float &fNear,
		const float &fFar,const Vector3f &vCamPos, const Matrix3 &mCamRotationTrans );

	void CalcOrthogonalFrustumValues( const float &fNear, const float &fFar, const float &fLeft,
		const float &fRight, const float &fBottom, const float &fTop, const Matrix3 &mCamRotation,
		const Vector3f vCamPos );

	void SetAdditionalPlane( const Vector3f &vRef, const Vector3f &vNorm ) 
	{ m_AdditionalPlane.redefine(vRef,vNorm); }

	void EnableAdditionalPlane( void ) { m_bAdditionalPlaneEnabled = true; }
	void DisableAdditionalPlane( void ) { m_bAdditionalPlaneEnabled = false; }

	bool VertInFrustum( const Vector3f &vVert ) const;
	bool AxisAlignedCubeInFrustum( const Vector3f &vMins, const Vector3f &vMaxs ) const;
	//void SetWaterRelfMode( bool bWater ) { m_bWaterReflMode = bWater; }

	void DebugRender( void );

	const Vector3f *GetFarPlaneVerts( void ) const { return m_vFarPlaneVerts; }
	const Vector3f *GetNearPlaneVerts(void) const { return m_vNearPlaneVerts; }

private:
	CPlane m_Planes[6];
	CPlane m_AdditionalPlane;
	bool m_bAdditionalPlaneEnabled;
	Vector3f m_vViewPos;

	Vector3f m_vFarPlaneVerts[4];
	Vector3f m_vNearPlaneVerts[4];
};


#endif
