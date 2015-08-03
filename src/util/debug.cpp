// util/debug.cpp
// util/debug.cpp
// util/debug.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



//#include <renderer/glheaders.h>
//#include "maths.h"
//#include <renderer/CCanvas.h>
//#include "debug.h"
//#include "error.h"

namespace Debug
{

//CRenderLineList RenderListObj;
//CRenderLineList *g_pRenderLineList = &RenderListObj; 
//
//CRenderLineList::~CRenderLineList()
//{
//	if( m_pMem )
//		free(m_pMem);
//}
//
//void CRenderLineList::AddLine( const Vector3f &vStart, const Vector3f &vEnd, const Color &color )
//{
//	//enough space?
//	if( m_iMemSize < m_iOccMem + sizeof(CRenderLine) )
//	{
//		m_iMemSize += CRenderLineList::BLOCKSIZE;
//		m_pMem = (CRenderLine*)realloc(m_pMem, m_iMemSize);
//	}
//
//	m_pMem[m_iLines].m_vStart = vStart;
//	m_pMem[m_iLines].colorStart = color;
//	m_pMem[m_iLines].m_vStop = vEnd;
//	m_pMem[m_iLines].colorEnd = color;
//
//	m_iOccMem += sizeof( CRenderLine );
//	m_iLines++;
//}
//
//
//void CRenderLineList::Render( void )
//{
//#ifdef _RENDERER
//	if( m_iLines < 1 )
//		return;
//
//	glBindBuffer( GL_ARRAY_BUFFER, 0 );
//	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); //unbind the last used index buffer
//	
//	glVertexPointer( 3, GL_FLOAT, sizeof(CRenderLine)/2, (char*)m_pMem );
//	glColorPointer( 4, GL_UNSIGNED_BYTE, sizeof(CRenderLine)/2, (char*)m_pMem + sizeof(Vector3f) );
//
//	StateSaver.EnableClientState( GL_VERTEX_ARRAY );
//	StateSaver.EnableClientState( GL_COLOR_ARRAY );
//	StateSaver.DisableClientState( GL_NORMAL_ARRAY );
//	StateSaver.DisableClientState( GL_TEXTURE_COORD_ARRAY );
//	StateSaver.DisableClientState( GL_INDEX_ARRAY );
//	StateSaver.DisableVertexAttribArray( 0, true );
//
//	glDrawArrays( GL_LINES, 0, m_iLines*2 );
//
//	StateSaver.DisableClientState( GL_COLOR_ARRAY );
//	StateSaver.EnableClientState( GL_INDEX_ARRAY );
//#endif
//}
//
//
//void CRenderLineList::Clear( void )
//{
//	//I commented this because we dont want to free the
//	//debug-line memory all the time and reallocate it again when we draw new lines.
//	//So debug memory reaches its maximum when the maximum count of 
//	//lines gets rendered (usually the first time physics-debugging is enabled),
//	//but it never decreases again. This might be wasted as soon
//	//as physical debugging is deactivated, but I can live with these 5 Megabytes or so.
//
//	//free( m_pMem );
//	//m_iMemSize = 0;
//	//m_pMem = NULL;
//
//	m_iLines = 0;
//	m_iOccMem = 0;
//}


UINT g_iWhiteTexture = 0;

void InitDebug( UINT iWhiteTexture )
{
	Debug::g_iWhiteTexture = iWhiteTexture;
}

//#define DEBUG_METHOD_DIRECT
//#ifdef DEBUG_METHOD_DIRECT
//void DrawLine( const Vector3f &vStart, const Vector3f &vEnd, const Color &color, const float &fLineWidth )
//{
	//Canvas.SetDrawColor( color );
	//glBindTexture( GL_TEXTURE_2D, g_iWhiteTexture );
	//glLineWidth( fLineWidth );
	//glBegin( GL_LINES );
	//	glVertex3f( vStart.x, vStart.y, vStart.z );
	//	glVertex3f( vEnd.x, vEnd.y, vEnd.z );
	//glEnd();
	//glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
//}
//#else
//void DrawLine( const Vector3f &vStart, const Vector3f &vEnd, const Color &color, const float &fLineWidth )
//{
	//RenderListObj.AddLine( vStart, vEnd, color );
//}
//#endif


//void DrawXZQuad( const Vector3f &vFrontLeft, const float &size, const Color &color, const float &fLineWidth )
//{
//#ifdef _RENDERER
//	glBegin( GL_LINE_STRIP );
//		glVertex3f( vFrontLeft.x,		vFrontLeft.y,	vFrontLeft.z );
//		glVertex3f( vFrontLeft.x,		vFrontLeft.y,	vFrontLeft.z-size );
//		glVertex3f( vFrontLeft.x+size,	vFrontLeft.y,	vFrontLeft.z-size );
//		glVertex3f( vFrontLeft.x+size,	vFrontLeft.y,	vFrontLeft.z );
//		glVertex3f( vFrontLeft.x,		vFrontLeft.y,	vFrontLeft.z );
//	glEnd();
//#endif
//}


//void DrawCube( const Vector3f &vCenter, const float &fWidth, const Color &color, const float &fLineWidth )
//{
//#ifdef _RENDERER
//	float fWh = fWidth * 0.5f;
//	float fW = fWidth;
//
//	//Top-Front-Left Corner
//	Vector3f vTFL = Vector3f( vCenter.x - fWh, vCenter.y + fWh, vCenter.z + fWh );
//
//	//Canvas.SetDrawColor( color );
//	glBindTexture( GL_TEXTURE_2D, g_iWhiteTexture );
//	glLineWidth( fLineWidth );
//
//	DrawXZQuad( vTFL, fWidth, color, fLineWidth );
//	DrawXZQuad( vTFL-Vector3f(0,fW,0), fWidth, color, fLineWidth );
//
//	glBegin( GL_LINES );
//		glVertex3f( vTFL.x,		vTFL.y,		vTFL.z );
//		glVertex3f( vTFL.x,		vTFL.y-fW,	vTFL.z );
//
//		glVertex3f( vTFL.x,		vTFL.y,		vTFL.z-fW );
//		glVertex3f( vTFL.x,		vTFL.y-fW,	vTFL.z-fW );
//
//		glVertex3f( vTFL.x+fW,	vTFL.y,		vTFL.z-fW );
//		glVertex3f( vTFL.x+fW,	vTFL.y-fW,	vTFL.z-fW );
//
//		glVertex3f( vTFL.x+fW,	vTFL.y,		vTFL.z );
//		glVertex3f( vTFL.x+fW,	vTFL.y-fW,	vTFL.z );
//	glEnd();
//#endif
//}

}
