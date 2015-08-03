// renderer/CCanvas.cpp
// renderer/CCanvas.cpp
// renderer/CCanvas.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#if 0

#include "CRenderInterf.h"
#include "glheaders.h"
#include "CCanvas.h"
//#include "main.h"
#include <stdio.h>
#include "maths.h"

#pragma warning( push )
#pragma warning( disable : 4244 ) //disable annoying typecast warnings


//Declare CCanvas!
//CCanvas Canvas;

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CCanvas::CCanvas()
{
	m_iHeight = 0;
	m_iWidth = 0;
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCanvas::TransformVertex2( int &x, int &y )
{
	//x = RESX(x);
	//y = RESY(y);
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
//void CCanvas::DrawLine( line_t *line )
//{
//	int x1 = RESX( line->x1 );
//	int y1 = RESY( line->y1 );
//	int x2 = RESX( line->x2 );
//	int y2 = RESY( line->y2 );
//
//	glBegin(GL_LINES);
//		glVertex2f( x1, y1 );
//		glVertex2f( x2, y2 );
//	glEnd();
//}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCanvas::DrawRect( int x1, int y1, int x2, int y2 )
{
	//x1 = RESX( x1 );
	//y1 = RESY( y1 );
	//x2 = RESX( x2 );
	//y2 = RESY( y2 );

	//if( IsTexture2dEnabled() )
	//	GetGLStateSaver()->Disable( GL_TEXTURE_2D );

	//glBegin( GL_QUADS );
	//	glVertex2i( x1, y1 );
	//	glVertex2i( x2, y1 );
	//	glVertex2i( x2, y2 );
	//	glVertex2i( x1, y2 );
	//glEnd();
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCanvas::DrawTriangle( int x1, int y1, int x2, int y2, int x3, int y3 )
{
	//TransformVertex2( x1, y1 );
	//TransformVertex2( x2, y2 );
	//TransformVertex2( x3, y3 );

	//if( IsTexture2dEnabled() )
	//	GetGLStateSaver()->Disable( GL_TEXTURE_2D );

	//glBegin( GL_TRIANGLES );
	//	glVertex2i( x1, y1 );
	//	glVertex2i( x2, y2 );
	//	glVertex2i( x3, y3 );
	//glEnd();
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCanvas::SetDrawColor( float r, float g, float b, float a )
{
	if( !IsCurrentColor( r, g, b, a ) )
		glColor4f( r, g, b, a );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCanvas::SetDrawColor( int r, int g, int b, int a )
{
	SetDrawColor( r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCanvas::SetDrawColor( Color col )
{
	SetDrawColor( col.r / 255.0f, col.g / 255.0f, col.b / 255.0f, col.a / 255.0f );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCanvas::DrawTexturedRect( int iTextureIndex, int x1, int y1, int x2, int y2 )
{
	//TransformVertex2( x1, y1 );
	//TransformVertex2( x2, y2 );

	//if( !IsTexture2dEnabled() ) 
	//	GetGLStateSaver()->Enable( GL_TEXTURE_2D );

	//glBindTexture( GL_TEXTURE_2D, iTextureIndex );
	//glBegin(GL_QUADS);
	//	glTexCoord2d( 0.0f, 1.0f );
	//	glVertex2f( x1, y1 );

	//	glTexCoord2d( 1.0f, 1.0f );
	//	glVertex2f( x2, y1 );

	//	glTexCoord2d( 1.0f, 0.0f );
	//	glVertex2f( x2, y2 );

	//	glTexCoord2d( 0.0f, 0.0f );
	//	glVertex2f( x1, y2 );
	//glEnd();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCanvas::SetWindowSize( int width, int height )
{
	m_iWidth = width;
	m_iHeight = height;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCanvas::GetWindowSize( int *width, int *height )
{
	*width = m_iWidth;
	*height = m_iHeight;
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCanvas::GetMousePos( int *x, int *y )
{
	POINT pos;
	GetCursorPos( &pos );
	
	//TITLEBARINFO titlebar;
	//titlebar.cbSize = sizeof( TITLEBARINFO );
	//GetTitleBarInfo( hWnd, &titlebar );

	WINDOWINFO window;
	window.cbSize = sizeof( WINDOWINFO );
	GetWindowInfo( (HWND)g_RenderInterf->GetWnd(), &window );
	
	*x = 640.0f/(float)m_iWidth*(pos.x - window.rcClient.left);
	*y = 480.0f/(float)m_iHeight*(pos.y - window.rcClient.top);

	//Debug:
	//char pText[256];
	//sprintf( pText, "real: %i %i, interpolated: %i %i", pos.x, pos.y, *x, *y );
	//SetWindowText( hWnd, pText );
}

#define MOUSE_SIZE 25
//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
//void CCanvas::DrawMouse( void )
//{
//	//Dont draw mouse if we dont have focus
//	//if( !main::HasWindowFocus() )
//		//return;
//
//	glPushMatrix();
//		glTranslatef( 0, 0, -riMouse*100 );
//		int x, y;
//		GetMousePos( &x, &y );
//
//		x += -1;
//		y += 3;
//		SetDrawColor( 0, 0, 0, 30 );
//		DrawTriangle( x, y, x+MOUSE_SIZE/2, y+MOUSE_SIZE, x+MOUSE_SIZE, y+MOUSE_SIZE/2 );
//
//		x -= -1;
//		y -= 3;
//		SetDrawColor( 138, 255, 0, 255 );
//		DrawTriangle( x, y, x+MOUSE_SIZE/2, y+MOUSE_SIZE, x+MOUSE_SIZE, y+MOUSE_SIZE/2 );
//	glPopMatrix();
//}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CCanvas::IsMouseOver( int x1, int y1, int x2, int y2 )
{
	if( GetForegroundWindow() != g_RenderInterf->GetWnd() ) //If our window does not have focus
		return false;

	int x, y;
	GetMousePos( &x, &y );
	return( (x >= x1) && (y >= y1) && (x < x2) && (y < y2) );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CCanvas::DrawTexturedRectEx( int iTextureIndex, Vector *Coords4, Vector *TexCoords4 )
{
	//Vector Coords4Ex[4];

	//for( int i = 0; i < 4; i++ )
	//{
	//	Coords4Ex[i] = Coords4[i] + 0.5f;
	//	Coords4Ex[i].x = RESX( Coords4Ex[i].x );
	//	Coords4Ex[i].y = RESY( Coords4Ex[i].y );
	//}

	//if( !IsTexture2dEnabled() )
	//	GetGLStateSaver()->Enable( GL_TEXTURE_2D );

	//glBindTexture( GL_TEXTURE_2D, iTextureIndex );
	//glBegin(GL_QUADS);
	//	glTexCoord2d( TexCoords4[0].x, TexCoords4[0].y );
	//	glVertex2f( Coords4Ex[0].x, Coords4Ex[0].y );

	//	glTexCoord2d( TexCoords4[1].x, TexCoords4[1].y );
	//	glVertex2f( Coords4Ex[1].x, Coords4Ex[1].y );

	//	glTexCoord2d( TexCoords4[2].x, TexCoords4[2].y );
	//	glVertex2f( Coords4Ex[2].x, Coords4Ex[2].y );

	//	glTexCoord2d( TexCoords4[3].x, TexCoords4[3].y );
	//	glVertex2f( Coords4Ex[3].x, Coords4Ex[3].y );
	//glEnd();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CCanvas::IsTexture2dEnabled( void )
{
	glGetBooleanv( GL_TEXTURE_2D, &m_bTexture2d );
	return (m_bTexture2d == 1);
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CCanvas::IsCurrentColor( float r, float g, float b, float a )
{
	float pColor[4];
	glGetFloatv( GL_CURRENT_COLOR, pColor );
	return (	r == pColor[0] && 
				g == pColor[1] && 
				b == pColor[2] && 
				a == pColor[3] );
}
//---------------------------------------------------------------------------


#pragma warning( pop )

#endif