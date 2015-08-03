// editor/IRenderView.h
// editor/IRenderView.h
// editor/IRenderView.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once

class IRenderView
{
public:
	IRenderView(viewtypes_e viewtype) : m_viewType(viewtype) { }

public:
	virtual void Init( HDC hDC, int resx, int resy ) = 0;
	virtual void Resize( int resx, int resy ) = 0;
	virtual void Render( void ) = 0;
	virtual void SetViewPosition( const Vector2f &pos ) { }
	virtual Vector2f GetViewPosition( void ) { return vec2_null; }
	virtual int HandleMouseMove( UINT nFlags, CPoint point ) { return 0; }
	virtual int HandleLButtonDown( UINT nFlags, CPoint point ) { return 0; }
	virtual int HandleRButtonDown( UINT nFlags, CPoint point ) { return 0; }
	virtual int HandleZoom( int dir ) { return 0; }
	virtual int HandleVScroll( UINT code, int pos ) { return 0; }
	virtual int HandleHScroll( UINT code, int pos ) { return 0; }
	virtual int HandleKeyDown( UINT nChar, UINT nRepCnt, UINT flags ) { return 0; }
	virtual int HandleKeyUp( UINT nChar, UINT nRepCnt, UINT flags ) { return 0; }
	virtual int UpdateCameraPosition( float elapsed ) { return 0; }
	virtual int GetWidth( void ) = 0;
	virtual int GetHeight( void ) = 0;

	virtual void CreateMouseRay( const POINT &mousepos, Vector3f &rayorig, Vector3f &raydir ) { }
	virtual Vector3f MouseMoveToWorldMove( const Vector2f &mousemove, float camDist ) { return vec3_null; }
public:
	viewtypes_e GetViewType(void) { return m_viewType; }

private:
	viewtypes_e m_viewType;
};

