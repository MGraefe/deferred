// editor/C3dView.h
// editor/C3dView.h
// editor/C3dView.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once

#include "stdafx.h"
#include <CRenderEntity.h>
#include "IRenderView.h"

class C3dView : public IRenderView
{
public:
	static C3dView *GetActive( void );

	C3dView();
	~C3dView();

	void Init( HDC hDC, int resx, int resy );
	void Resize( int resx, int resy );
	void Render( void );
	CCamera *GetCamera(void) { return m_Camera; }
	int GetWidth( void ) { return m_resx; }
	int GetHeight( void ) { return m_resy; }
	void CreateMouseRay( const POINT &mousepos, Vector3f &rayorig, Vector3f &raydir );
	Vector3f MouseMoveToWorldMove( const Vector2f &mousemove, float camDist );

private:
	CCamera *m_Camera;
	HDC m_hDC;
	int m_resx;
	int m_resy;
};



