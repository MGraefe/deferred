// editor/CToolCamera.h
// editor/CToolCamera.h
// editor/CToolCamera.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CToolCamera.h

#pragma once
#ifndef deferred__editor__CToolCamera_H__
#define deferred__editor__CToolCamera_H__


#include "IEditorTool.h"

class CToolCamera : public IEditorTool
{

public:
	CToolCamera();
	virtual ~CToolCamera();

public:
	virtual void OnMouseMove( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnLButtonDown( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnRButtonDown( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnMButtonDown( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnKeyDown( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags );
	virtual void OnLButtonUp( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnRButtonUp( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnMButtonUp( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnKeyUp( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags );
	virtual void OnRButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnMButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnLButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point ) {}
	virtual void OnWheelScroll( COpenGLView* view, UINT nFlags, short zDelta, CPoint pt ) {}
	virtual void OnVScroll( COpenGLView* view, UINT code, int pos ) {}
	virtual void OnHScroll( COpenGLView* view, UINT code, int pos ) {}

	virtual void Render3d( void ) {}
	virtual void Init( void );

	int UpdateCameraPosition( float elapsed );
	void OnTimerProc( HWND wnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );
	static void CALLBACK CallbackTimerProc( HWND wnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );
private:
	Angle3d m_camAngles;
	POINT m_LastMouse;
	bool m_keys[128];
	bool m_timerActive;
	float m_lastTime;
	UINT m_timer;

	Quaternion m_lastCamRot;
	Vector3f m_lastCamPos;
	bool m_bFirstFrame;
};

#endif // deferred__editor__CToolCamera_H__
