// editor/CToolEditCam.h
// editor/CToolEditCam.h
// editor/CToolEditCam.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__editor__CToolEditCam_H__
#define deferred__editor__CToolEditCam_H__

#include "IEditorTool.h"


class CToolEditCam : public IEditorTool
{
	DECLARE_CLASS(IEditorTool, CToolEditCam);

public:
	CToolEditCam();
	virtual ~CToolEditCam();

public:
	virtual void OnMouseMove( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnLButtonDown( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnRButtonDown( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnMButtonDown( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnKeyDown( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags );
	virtual void OnLButtonUp( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnRButtonUp( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnMButtonUp( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnKeyUp( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags );
	virtual void OnRButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnMButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnLButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point );
	virtual void OnWheelScroll( COpenGLView* view, UINT nFlags, short zDelta, CPoint pt );
	virtual void OnVScroll( COpenGLView* view, UINT code, int pos );
	virtual void OnHScroll( COpenGLView* view, UINT code, int pos );

	virtual void Render3d( void );
	virtual void Init( void );

private:
	IEditorTool *m_camera;
	IEditorTool *m_pointer;

};

#endif // deferred__editor__CToolEditCam_H__