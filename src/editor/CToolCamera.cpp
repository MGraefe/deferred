// editor/CToolCamera.cpp
// editor/CToolCamera.cpp
// editor/CToolCamera.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "CToolCamera.h"
#include "CCamera.h"
#include "C3dView.h"
#include "ChildFrm.h"
#include "COpenGLView.h"


std::map<UINT, CToolCamera*> g_timerList;
UINT g_iTimerIdCounter = 1;

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolCamera::CToolCamera() : 
	IEditorTool(TOOL_GENERIC)
{
	m_LastMouse.x = m_LastMouse.y = 0;
	memset( m_keys, 0, sizeof m_keys );
	m_timerActive = false;
	m_bFirstFrame = true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolCamera::~CToolCamera()
{
	if( m_timerActive )
	{
		g_timerList.erase( m_timer );
		::KillTimer( NULL, m_timer );
		m_timerActive = false;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolCamera::Init( void )
{
	if( !m_timerActive )
	{
		UINT id = ++g_iTimerIdCounter;
		m_timer = ::SetTimer( NULL, id, 10, (TIMERPROC)&CallbackTimerProc );
		g_timerList[m_timer] = this;
		m_lastTime = ((float)timeGetTime()) / 1000.0f;
		m_timerActive = true;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolCamera::OnMouseMove( COpenGLView* view, UINT nFlags, CPoint point )
{
	if( nFlags & MK_RBUTTON )
	{
		float mouseSens = 0.5f;
		int diffx = point.x - m_LastMouse.x;
		int diffy = point.y - m_LastMouse.y;
		m_LastMouse = point;

		m_camAngles.x = clamp(m_camAngles.x - ((float)diffy) * mouseSens, -90.0f, 90.0f);
		m_camAngles.y -= ((float)diffx) * mouseSens;
		while(m_camAngles.y > 180.0f)
			m_camAngles.y -= 360.0f;
		while(m_camAngles.y < -180.0f)
			m_camAngles.y += 360.0f;

		Quaternion rotX = Quaternion(Vector3f(1.0f, 0.0f, 0.0f), m_camAngles.x );
		Quaternion rotY = Quaternion(Vector3f(0.0f, 1.0f, 0.0f), m_camAngles.y );
		Quaternion rotZ = Quaternion(Vector3f(0.0f, 0.0f, 0.0f), 0.0f);

		CCamera *cam = C3dView::GetActive()->GetCamera();
		cam->SetAbsAngles(rotZ * rotY * rotX);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolCamera::OnKeyDown( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags )
{
	m_keys[nChar] = true;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CToolCamera::UpdateCameraPosition( float elapsed )
{
	float forward = 0.0f;
	float right = 0.0f;
	float fSpeedForward = m_keys[VK_SHIFT] ? 1000.0f : 100.0f;
	float fSpeedRight = fSpeedForward;

	if( m_keys['W'] )
		forward += 1.0f;
	if( m_keys['S'] )
		forward -= 1.0f;
	if( m_keys['A'] )
		right -= 1.0f;
	if( m_keys['D'] )
		right += 1.0f;

	Vector3f vForward, vRight, vUp;
	CCamera *pCam = C3dView::GetActive()->GetCamera();
	pCam->GetAbsAngles().GetMatrix3().GetRightUpForwardVectors( vRight, vUp, vForward );
	pCam->SetAbsPos( pCam->GetAbsPos()
		+ vForward * forward * elapsed * fSpeedForward
		+ vRight * right * elapsed * fSpeedRight );

	if( forward != 0.0f || right != 0.0f )
		return 1;
	return 0;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolCamera::OnKeyUp( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags )
{
	m_keys[nChar] = false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolCamera::OnRButtonDown( COpenGLView* view, UINT nFlags, CPoint point )
{
	m_LastMouse = point;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CALLBACK CToolCamera::CallbackTimerProc( HWND wnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
	CToolCamera *This = g_timerList[idEvent];
	This->OnTimerProc(wnd, uMsg, idEvent, dwTime);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolCamera::OnTimerProc( HWND wnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
	if( !C3dView::GetActive() || !C3dView::GetActive()->GetCamera() )
	{
		m_bFirstFrame = true;
		return;
	}

	float time = ((float)timeGetTime()) / 1000.0f;
	float elapsed = time - m_lastTime;
	m_lastTime = time;
	UpdateCameraPosition(elapsed);

	CCamera *cam = C3dView::GetActive()->GetCamera();
	if( m_bFirstFrame ||
		cam->GetAbsAngles() != m_lastCamRot ||
		cam->GetAbsPos() != m_lastCamPos )
	{
		m_lastCamPos = cam->GetAbsPos();
		m_lastCamRot = cam->GetAbsAngles();
		m_bFirstFrame = false;
	}
		
	//if( UpdateCameraPosition(elapsed) == 1 )
		//GetActiveChildFrame()->GetView(VIEW_3D)->DrawView();
}

