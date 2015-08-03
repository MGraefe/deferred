// renderer/CViewFrustum.cpp
// renderer/CViewFrustum.cpp
// renderer/CViewFrustum.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CViewFrustum.h"
#include "scenetree.h"
#include <util/debug.h>
#include "CRenderInterf.h"
#include "renderer.h"
#include <util/debug.h>
#include <util/basicfuncs.h>

CViewFrustum::CViewFrustum()
{ 
	m_bAdditionalPlaneEnabled = false;
}

void CViewFrustum::DebugRender( void )
{
	//Debug::DrawLine( m_vNearPlaneVerts[0], m_vFarPlaneVerts[0], Color(255,0,0) );
	//Debug::DrawLine( m_vNearPlaneVerts[1], m_vFarPlaneVerts[1], Color(255,0,0) );
	//Debug::DrawLine( m_vNearPlaneVerts[2], m_vFarPlaneVerts[2], Color(255,0,0) );
	//Debug::DrawLine( m_vNearPlaneVerts[3], m_vFarPlaneVerts[3], Color(255,0,0) );
	//Debug::DrawLine( m_vNearPlaneVerts[0], m_vNearPlaneVerts[1], Color(255,0,0) );
	//Debug::DrawLine( m_vNearPlaneVerts[1], m_vNearPlaneVerts[2], Color(255,0,0) );
	//Debug::DrawLine( m_vNearPlaneVerts[2], m_vNearPlaneVerts[3], Color(255,0,0) );
	//Debug::DrawLine( m_vNearPlaneVerts[3], m_vNearPlaneVerts[0], Color(255,0,0) );
}


void CViewFrustum::CalcPerspectiveFrustumValues( const float &fFOV, const float &fAspectRatio, const float &fNear, 
										const float &fFar, const Vector3f &vCamPos, const Matrix3 &mCamRotationTrans )
{
	float fTan = TAN( fFOV*0.5f );

	//Calculate width and height of far plane
	float fHFarHalf = fTan * fFar;	
	float fWFarHalf = fHFarHalf * fAspectRatio;

	//Calculate width and height of near plane
	float fHNearHalf = fTan * fNear;
	float fWNearHalf = fHNearHalf * fAspectRatio;

	//Calculate initial coordinates of far-plane (CCW, oriented to viewer)
	m_vFarPlaneVerts[0].Init( -fWFarHalf, fHFarHalf, -fFar );
	m_vFarPlaneVerts[1].Init( -fWFarHalf, -fHFarHalf, -fFar );
	m_vFarPlaneVerts[2].Init( fWFarHalf, -fHFarHalf, -fFar );
	m_vFarPlaneVerts[3].Init( fWFarHalf, fHFarHalf, -fFar );
	m_vNearPlaneVerts[0].Init( -fWNearHalf, fHNearHalf, -fNear );
	m_vNearPlaneVerts[1].Init( -fWNearHalf, -fHNearHalf, -fNear );
	m_vNearPlaneVerts[2].Init( fWNearHalf, -fHNearHalf, -fNear );
	m_vNearPlaneVerts[3].Init( fWNearHalf, fHNearHalf, -fNear );
	Vector3f vMid( 0.0f, 0.0f, -(fFar-fNear)*0.5f );

	//Calculate actual position of far-plane by multiplying these Vectors with the Camera rotation Matrix.
	for( int i = 0; i < 4; i++ )
		m_vFarPlaneVerts[i] = mCamRotationTrans * m_vFarPlaneVerts[i];
	for( int i = 0; i < 4; i++ )
		m_vNearPlaneVerts[i] = mCamRotationTrans * m_vNearPlaneVerts[i];

	//vMid = mCamRotationTrans * vMid + vCamPos;

	//near plane, is always the first one to be checked
	m_Planes[0].redefine(m_vNearPlaneVerts[2] + vCamPos,
		-VectorCross( m_vNearPlaneVerts[1]-m_vNearPlaneVerts[2], m_vNearPlaneVerts[3]-m_vNearPlaneVerts[2] ).Normalize() );

	for( int i = 1; i < 5; i++ )
	{
		int k = i > 3 ? 0 : i;
		m_Planes[i].redefine( vCamPos, VectorCross( m_vNearPlaneVerts[i-1], m_vNearPlaneVerts[k] ).Normalize() );
		//m_Planes[i].m_vNorm = VectorCross( m_vNearPlaneVerts[i-1], m_vNearPlaneVerts[k] );
		//m_Planes[i].m_vRef = vCamPos;
	}

	//far plane
	m_Planes[5].redefine(m_vFarPlaneVerts[2] + vCamPos,
		VectorCross( m_vFarPlaneVerts[1]-m_vFarPlaneVerts[2], m_vFarPlaneVerts[3]-m_vFarPlaneVerts[2] ).Normalize() );
	//m_Planes[5].m_vNorm = VectorCross( m_vFarPlaneVerts[1]-m_vFarPlaneVerts[2], m_vFarPlaneVerts[3]-m_vFarPlaneVerts[2] );
	//m_Planes[5].m_vRef = m_vFarPlaneVerts[2] + vCamPos;

	m_vViewPos = vCamPos;
}


//vertexes in the following order:
//	 3_____________7
//   /|			  /|
// 2/____________/6|
//	| |			 | |	//Camera is looking "in" your screen
//  | |			 | |	
//  |1|_ _ _ _ _ |_|5	
//  | /			 | /
// 0|/___________|/4
void CViewFrustum::CalcOrthogonalFrustumValues( const float &fNear, const float &fFar, const float &fLeft,
		const float &fRight, const float &fBottom, const float &fTop, const Matrix3 &mCamRotation,
		const Vector3f vCamPos )
{
	Vector3f vVerts[8] = 
		{	Vector3f( fLeft,	fBottom,	-fNear ),	//000
			Vector3f( fLeft,	fBottom,	-fFar ),	//001
			Vector3f( fLeft,	fTop,		-fNear ),	//010
			Vector3f( fLeft,	fTop,		-fFar ),	//011
			Vector3f( fRight,	fBottom,	-fNear ),	//100
			Vector3f( fRight,	fBottom,	-fFar ),	//101
			Vector3f( fRight,	fTop,		-fNear ),	//110
			Vector3f( fRight,	fTop,		-fFar ) };	//111

	for( int i = 0; i < 8; i++ )
		vVerts[i] = mCamRotation * vVerts[i];

	m_Planes[0].redefine(vVerts[0] + vCamPos, VectorCross( vVerts[2]-vVerts[0], vVerts[1]-vVerts[0] ).Normalize() );
	m_Planes[1].redefine(vVerts[4] + vCamPos, VectorCross( vVerts[0]-vVerts[4], vVerts[5]-vVerts[4] ).Normalize() );
	m_Planes[2].redefine(vVerts[6] + vCamPos, VectorCross( vVerts[4]-vVerts[6], vVerts[7]-vVerts[6] ).Normalize() );
	m_Planes[3].redefine(vVerts[2] + vCamPos, VectorCross( vVerts[6]-vVerts[2], vVerts[3]-vVerts[2] ).Normalize() );
	m_Planes[4].redefine(vVerts[1] + vCamPos, VectorCross( vVerts[3]-vVerts[1], vVerts[5]-vVerts[1] ).Normalize() );
	m_Planes[5].redefine(vVerts[0] + vCamPos, VectorCross( vVerts[4]-vVerts[0], vVerts[2]-vVerts[0] ).Normalize() );

	//copy them to our local structures
	for( int i = 0; i < 4; i++ )
	{
		m_vNearPlaneVerts[i] = vVerts[i];
		m_vFarPlaneVerts[i] = vVerts[i+4];
	}
}


//needs a seriously robust and efficent implementation.
bool CViewFrustum::VertInFrustum( const Vector3f &vVert ) const
{
	if( m_Planes[0].IsVertexAbove( vVert ) ||
		m_Planes[1].IsVertexAbove( vVert ) ||
		m_Planes[2].IsVertexAbove( vVert ) ||
		m_Planes[3].IsVertexAbove( vVert ) ||
		m_Planes[4].IsVertexAbove( vVert ) ||
		m_Planes[5].IsVertexAbove( vVert ) )
		return false;

	return true;
}



//TODO: needs a robust and fast implementation
//NOTENOTE: It is only guaranteed that the box is outside the frustum if this function
//			returns false. If it returns true the box is inside the frustum in most cases
//			but that is not guaranteed.
bool CViewFrustum::AxisAlignedCubeInFrustum( const Vector3f &vMins, const Vector3f &vMaxs ) const
{
	Vector3f vBoxCenter = (vMins + vMaxs) * 0.5f;
	Vector3f vBoxSizeHalf = (vMaxs - vMins) * 0.5f;

	for( int i = 0; i < 6; i++ )
		if( m_Planes[i].IsBoxAbove(vBoxCenter,vBoxSizeHalf) )
			return false;

	if( m_bAdditionalPlaneEnabled && m_AdditionalPlane.IsNonCenteredBoxAbove(vMins,vMaxs) )
		return false;

	//Vector3f vCubeVerts[8];
	//GetCubeVerts( vMins, vMaxs, vCubeVerts );
	//for( int i = 0; i < 6; i++ )
	//{
	//	if( m_Planes[i].IsVertexAbove( vCubeVerts[0] ) &&
	//		m_Planes[i].IsVertexAbove( vCubeVerts[1] ) &&
	//		m_Planes[i].IsVertexAbove( vCubeVerts[2] ) &&
	//		m_Planes[i].IsVertexAbove( vCubeVerts[3] ) &&
	//		m_Planes[i].IsVertexAbove( vCubeVerts[4] ) &&
	//		m_Planes[i].IsVertexAbove( vCubeVerts[5] ) &&
	//		m_Planes[i].IsVertexAbove( vCubeVerts[6] ) &&
	//		m_Planes[i].IsVertexAbove( vCubeVerts[7] ) )
	//		return false;
	//}

	//if( m_bAdditionalPlaneEnabled )
	//{
	//	if( m_AdditionalPlane.IsVertexAbove( vCubeVerts[0] ) &&
	//		m_AdditionalPlane.IsVertexAbove( vCubeVerts[1] ) &&
	//		m_AdditionalPlane.IsVertexAbove( vCubeVerts[2] ) &&
	//		m_AdditionalPlane.IsVertexAbove( vCubeVerts[3] ) &&
	//		m_AdditionalPlane.IsVertexAbove( vCubeVerts[4] ) &&
	//		m_AdditionalPlane.IsVertexAbove( vCubeVerts[5] ) &&
	//		m_AdditionalPlane.IsVertexAbove( vCubeVerts[6] ) &&
	//		m_AdditionalPlane.IsVertexAbove( vCubeVerts[7] ) )
	//		return false;
	//}

	return true;
}
