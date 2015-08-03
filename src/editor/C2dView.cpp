// editor/C2dView.cpp
// editor/C2dView.cpp
// editor/C2dView.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "C2dView.h"
#include <gl\glew.h>		// Header File For The OpenGL Extension Wrangler
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include "CGLResources.h"
#include <CModel.h>
#include "CRenderInterf.h"


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
C2dView::C2dView( viewtypes_e direction ) : IRenderView(direction)
{
	m_viewDir = direction;
	m_viewPos.Init(0.0f,0.0f);
	m_zoomFactor = 10.0f;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void C2dView::Render( void )
{
	GetShaderManager()->SetActiveShader( GetShaderManager()->GetShader( "Unlit" ) );

	glDisable(GL_CULL_FACE);
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	SetupProjection();

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	//glDisable( GL_DEPTH_TEST );

	//glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	//glBegin( GL_LINES );
	//glVertex3f( -10.0f, 0.0, 0.0f );
	//glVertex3f( 10.0f, 0.0, 0.0f );
	//glEnd();
	if( GetGLResources()->m_world )
		GetGLResources()->m_world->RenderNoTextureBufferOnly();
	if( GetGLResources()->m_model )
		GetGLResources()->m_model->RenderNoTextureBufferOnly();

	DrawGrid();
	SwapBuffers(m_hDC);
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable(GL_CULL_FACE);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void C2dView::Resize( int resx, int resy )
{
	m_resx = resx;
	m_resy = resy;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void C2dView::Init( HDC hDC, int resx, int resy )
{
	m_hDC = hDC;
	m_resx = resx;
	m_resy = resy;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void C2dView::SetupProjection( void )
{
	Vector2f va = GetViewableArea() * 0.5f;

	GetProjStack().SetOrthographic( -va.x, va.x, -va.y, va.y, -500.0f, 500.0f );
	GetMVStack().SetIdentity();

	Assert( m_viewDir == VIEW_FRONT || m_viewDir == VIEW_TOP || m_viewDir == VIEW_RIGHT );

	if( m_viewDir == VIEW_TOP )
	{
		GetMVStack().Rotate( 90.0f, 1.0f, 0.0f, 0.0f );
		GetMVStack().Translate( Vector3f(m_viewPos.x, 0.0f, -m_viewPos.y) );
	}
	else if( m_viewDir == VIEW_RIGHT )
	{
		GetMVStack().Rotate( -90.0f, 0.0f, 1.0f, 0.0f );
		GetMVStack().Translate( Vector3f(0.0f, m_viewPos.y, -m_viewPos.x) );
	}
	else if( m_viewDir == VIEW_FRONT )
	{
		GetMVStack().Translate( Vector3f(m_viewPos.x, m_viewPos.y, 0.0f) );
	}

	//no else because view is properly aligned for VIEW_FRONT
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int C2dView::HandleMouseMove( UINT nFlags, CPoint point )
{
	return 0;
}

struct testvert_t
{
	Vector3f v;
	Vector2f uv;
};

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void C2dView::DrawGrid( void )
{
	GetMVStack().SetIdentity();

	float gridScale = 10.0f;
	float posx = fmod(m_viewPos.x, gridScale);
	float posy = fmod(m_viewPos.y, gridScale);
	Vector2f va = GetViewableArea();
	
	const int iNumLinesX = (int)(va.x/gridScale) + 1;
	const int iNumLinesY = (int)(va.y/gridScale) + 1;
	const int iVertexes = iNumLinesX*2 + iNumLinesY*2;
	testvert_t *pV = new testvert_t[iVertexes];
	for( int i = 0; i < iNumLinesX; i++ )
	{
		float x = (i-iNumLinesX/2)*gridScale + posx;
		int k = i*2;
		pV[k].v.Init(x, va.y*0.5f, 0.0f);
		pV[k+1].v.Init(x, va.y*-0.5f, 0.0f);
	}
	for( int i = 0; i < iNumLinesY; i++ )
	{
		float y = (i-iNumLinesY/2)*gridScale + posy;
		int k = (i+iNumLinesX)*2;
		pV[k].v.Init(va.x*0.5f, y, 0.0f );
		pV[k+1].v.Init(va.x*-0.5f, y, 0.0f );
	}

	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars(VBF_POS_UV, pV);
	//glDisableVertexAttribArray( GetShaderManager()->GetActiveShader()->GetAttribLoc(SHADER_UVCOORD) );
	GetShaderManager()->GetActiveShader()->UpdateMatrices(true);
	glDrawArrays( GL_LINES, 0, iVertexes );
	rglPopClientAttrib();

	delete[] pV;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector2f C2dView::GetViewableArea( void )
{
	float sizeFactor = 1.0f / m_zoomFactor;
	return Vector2f( ((float)m_resx) * sizeFactor,
				     ((float)m_resy) * sizeFactor );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector2f C2dView::GetViewPosition( void )
{
	return m_viewPos;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void C2dView::SetViewPosition( const Vector2f &pos )
{
	m_viewPos = pos;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int C2dView::HandleZoom( int dir )
{
	float inc = 1.25f;
	float zoom = dir < 0 ? m_zoomFactor / inc : m_zoomFactor * inc;
	zoom = clamp(zoom, 0.1f, 10000.0f);

	if( m_zoomFactor != zoom )
	{
		m_zoomFactor = zoom;
		return 1;
	}
	return 0;	
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int C2dView::HandleVScroll( UINT code, int pos )
{
	Vector2f vPos = GetViewPosition();
	float scrollDist = 7.5f / m_zoomFactor;

	if( code == SB_LINEUP )
		vPos.y -= scrollDist;
	else if( code == SB_LINEDOWN )
		vPos.y += scrollDist;
	else if( code == SB_THUMBPOSITION )
		vPos.y = (float)pos;

	SetViewPosition(vPos);

	return (int)vPos.y;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int C2dView::HandleHScroll( UINT code, int pos )
{
	Vector2f vPos = GetViewPosition();
	float scrollDist = 7.5f / m_zoomFactor;

	if( code == SB_LINERIGHT )
		vPos.x -= scrollDist;
	else if( code == SB_LINELEFT )
		vPos.x += scrollDist;
	else if( code == SB_THUMBPOSITION )
		vPos.x = (float)pos;

	SetViewPosition(vPos);

	return (int)vPos.x;
}
