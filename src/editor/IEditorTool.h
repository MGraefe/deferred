// editor/IEditorTool.h
// editor/IEditorTool.h
// editor/IEditorTool.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//IEditorTool.h

#pragma once
#ifndef deferred__editor__IEditorTool_H__
#define deferred__editor__IEditorTool_H__

class COpenGLView;

class IEditorTool
{
public:
	enum returncodes_e
	{
		DO_NOTHING = 0,
		REDRAW_VIEW,
		START_CAM_MOVE,
		STOP_CAM_MOVE,
	};

	static IEditorTool* GetToolById( tools_e tool );

	IEditorTool( tools_e tool ) : m_toolId(tool) { }
	tools_e GetToolId(void) { return m_toolId; }
	//static IRenderView* GetView( viewtypes_e type );
	//static COpenGLView* GetViewContainer( viewtypes_e type );

public:
	virtual void OnMouseMove( COpenGLView* view, UINT nFlags, CPoint point ) = 0;
	virtual void OnLButtonDown( COpenGLView* view, UINT nFlags, CPoint point ) = 0;
	virtual void OnRButtonDown( COpenGLView* view, UINT nFlags, CPoint point ) = 0;
	virtual void OnMButtonDown( COpenGLView* view, UINT nFlags, CPoint point ) = 0;
	virtual void OnKeyDown( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags ) = 0;
	virtual void OnLButtonUp( COpenGLView* view, UINT nFlags, CPoint point ) = 0;
	virtual void OnRButtonUp( COpenGLView* view, UINT nFlags, CPoint point ) = 0;
	virtual void OnMButtonUp( COpenGLView* view, UINT nFlags, CPoint point ) = 0;
	virtual void OnKeyUp( COpenGLView* view, UINT nChar, UINT nRepCnt, UINT flags ) = 0;
	virtual void OnRButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point ) = 0;
	virtual void OnMButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point ) = 0;
	virtual void OnLButtonDblClk( COpenGLView* view, UINT nFlags, CPoint point ) = 0;
	virtual void OnWheelScroll( COpenGLView* view, UINT nFlags, short zDelta, CPoint pt ) = 0;
	virtual void OnVScroll( COpenGLView* view, UINT code, int pos ) = 0;
	virtual void OnHScroll( COpenGLView* view, UINT code, int pos ) = 0;
	
	virtual void Render3d( void ) = 0;
	virtual void Init( void ) = 0;

private:
	const tools_e m_toolId;
};

#endif // deferred__editor__IEditorTool_H__
