// editor/C2dView.h
// editor/C2dView.h
// editor/C2dView.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once

#include "IRenderView.h"


class C2dView : public IRenderView
{
public:
	C2dView( viewtypes_e direction );
	virtual void Render( void );
	virtual void Resize( int resx, int resy );
	virtual void Init( HDC hDC, int resx, int resy );
	virtual void SetViewPosition( const Vector2f &pos );

	void SetupProjection( void );
	void DrawGrid( void );
	Vector2f GetViewableArea( void );
	Vector2f GetViewPosition( void );
	
	virtual int HandleMouseMove( UINT nFlags, CPoint point );
	virtual int HandleZoom( int dir );
	virtual int HandleVScroll( UINT code, int pos );
	virtual int HandleHScroll( UINT code, int pos );

	int GetWidth( void ) { return m_resx; }
	int GetHeight( void ) { return m_resy; }
private:
	viewtypes_e m_viewDir;
	Vector2f m_viewPos;
	float m_zoomFactor;
	HDC m_hDC;
	int m_resx;
	int m_resy;
};

