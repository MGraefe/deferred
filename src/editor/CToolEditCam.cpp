// editor/CToolEditCam.cpp
// editor/CToolEditCam.cpp
// editor/CToolEditCam.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CToolEditCam.h"
#include "CToolCamera.h"
#include "CToolPointer.h"



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolEditCam::CToolEditCam() :
	BaseClass(TOOL_EDITCAM)
{
	m_camera = new CToolCamera();
	m_pointer = new CToolPointer();
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolEditCam::~CToolEditCam()
{
	if(m_camera)
		delete m_camera;
	if(m_pointer)
		delete m_pointer;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::Init( void )
{
	m_camera->Init();
	m_pointer->Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnMouseMove( COpenGLView* view, UINT nFlags, CPoint point )
{
	m_camera->OnMouseMove(view, nFlags, point);
	m_pointer->OnMouseMove(view, nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnLButtonDown( COpenGLView* view, UINT nFlags, CPoint point )
{
	m_camera->OnLButtonDown(view, nFlags, point);
	m_pointer->OnLButtonDown(view, nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnRButtonDown( COpenGLView* view, UINT nFlags, CPoint point )
{
	m_camera->OnRButtonDown(view, nFlags, point);
	m_pointer->OnRButtonDown(view, nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnWheelScroll( COpenGLView* view, UINT nFlags, short zDelta, CPoint pt )
{
	m_camera->OnWheelScroll(view, nFlags, zDelta, pt);
	m_pointer->OnWheelScroll(view, nFlags, zDelta, pt);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnVScroll( COpenGLView* view, UINT code, int pos )
{
	m_camera->OnVScroll(view, code, pos);
	m_pointer->OnVScroll(view, code, pos);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnHScroll( COpenGLView* view, UINT code, int pos )
{
	m_camera->OnHScroll(view, code, pos);
	m_pointer->OnHScroll(view, code, pos);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnKeyDown( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags )
{
	m_camera->OnKeyDown(view, nChar, nRepCnt, flags);
	m_pointer->OnKeyDown(view, nChar, nRepCnt, flags);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnKeyUp( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags )
{
	m_camera->OnKeyUp(view, nChar, nRepCnt, flags);
	m_pointer->OnKeyUp(view, nChar, nRepCnt, flags);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::Render3d( void )
{
	m_camera->Render3d();
	m_pointer->Render3d();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnMButtonDown( COpenGLView* view, UINT nFlags, CPoint point )
{
	m_camera->OnMButtonDown(view, nFlags, point);
	m_pointer->OnMButtonDown(view, nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnLButtonUp( COpenGLView* view, UINT nFlags, CPoint point )
{
	m_camera->OnLButtonUp(view, nFlags, point);
	m_pointer->OnLButtonUp(view, nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnRButtonUp( COpenGLView* view, UINT nFlags, CPoint point )
{
	m_camera->OnRButtonUp(view, nFlags, point);
	m_pointer->OnRButtonUp(view, nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnMButtonUp( COpenGLView* view, UINT nFlags, CPoint point )
{
	m_camera->OnMButtonUp(view, nFlags, point);
	m_pointer->OnMButtonUp(view, nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnRButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point )
{
	m_camera->OnRButtonDblClk(view, nFlags, point);
	m_pointer->OnRButtonDblClk(view, nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnMButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point )
{
	m_camera->OnMButtonDblClk(view, nFlags, point);
	m_pointer->OnMButtonDblClk(view, nFlags, point);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEditCam::OnLButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point )
{
	m_camera->OnLButtonDblClk(view, nFlags, point);
	m_pointer->OnLButtonDblClk(view, nFlags, point);
}
