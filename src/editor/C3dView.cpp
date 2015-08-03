// editor/C3dView.cpp
// editor/C3dView.cpp
// editor/C3dView.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "C3dView.h"
#include "ChildFrm.h"
#include "CGLResources.h"
#include "COpenGLView.h"

C3dView::C3dView() :
	IRenderView(VIEW_3D)
{

}


C3dView::~C3dView()
{
	delete m_Camera;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void C3dView::Init( HDC hDC, int resx, int resy )
{
	m_hDC = hDC;
	m_resx = resx;
	m_resy = resy;

	m_Camera = new CCamera( -1 );
	m_Camera->Init( false, false );
	m_Camera->Activate( 75.0f, 1.0f, 1000.0f, (float)resx/(float)resy );

	Angle3d vAng( -20.0f, 30.0f, 0.0f );
	Vector3f vPos;
	AngleToVector( vAng, vPos );

	m_Camera->SetAbsPos( vPos * -30.0f );
	m_Camera->SetAbsAngles( vAng );
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void C3dView::Resize( int resx, int resy )
{
	GetRenderInterfaces()->GetRendererInterf()->ReSizeScene(resx, resy);
	m_resx = resx;
	m_resy = resy;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void C3dView::Render( void )
{
	IRenderer *pRenderer = GetRenderInterfaces()->GetRendererInterf();
	//pRenderer->SetDeviceContext( m_hDC );
	//TODO: check if this works
	pRenderer->SetPlayerOneCam( m_Camera );
	pRenderer->Render();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
C3dView * C3dView::GetActive( void )
{
	CChildFrame *cf = GetActiveChildFrame();
	if(!cf)
		return NULL;

	COpenGLView *oglv = cf->GetView(VIEW_3D);
	if(!oglv)
		return NULL;

	return dynamic_cast<C3dView*>(oglv->GetRenderView());
}


//---------------------------------------------------------------
// Purpose: mousepos in window-local coordinates
//---------------------------------------------------------------
void C3dView::CreateMouseRay( const POINT &mousepos, Vector3f &rayorig, Vector3f &raydir )
{
	Vector2f windowCenter = Vector2f(m_resx, m_resy) * 0.5f;
	Vector2f clickPoint = Vector2f(mousepos.x, mousepos.y) - windowCenter;

	Vector2f pixelCoords( clickPoint.x / m_resy * 2.0f,
						  clickPoint.y / m_resy * -2.0f );
	float fTanHalfFov = TAN(m_Camera->GetFov()*0.5f);

	Vector3f dir( fTanHalfFov*pixelCoords.x, fTanHalfFov*pixelCoords.y, -1.0f );

	raydir = m_Camera->GetAbsAngles().GetMatrix3().GetTranspose() * dir;
	raydir.Normalize();
	rayorig = m_Camera->GetAbsPos();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector3f C3dView::MouseMoveToWorldMove( const Vector2f &mousemove, float camDist )
{
	Vector3f right, up, forward;
	m_Camera->GetAbsAngles().GetMatrix3().GetRightUpForwardVectors(right, up, forward);

	float sizeFactor = camDist * TAN(m_Camera->GetFov()*0.5f) / m_resy;
	Vector2f movedist = mousemove * sizeFactor;

	Vector3f move = right * movedist.x + up *  movedist.y;
	return move;
}


