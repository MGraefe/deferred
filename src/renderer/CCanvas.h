// renderer/CCanvas.h
// renderer/CCanvas.h
// renderer/CCanvas.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//==============================================================================\
// PANICGAME Source code														|
// Author: Marius Gräfe															|
// Copyright(c) 2008-2009: Marius Gräfe											|
//																				|
// Do not use this code(or parts of it) without my prior agreement.				|
// Mail: graefemarius@web.de													|
//==============================================================================/

//---------------------------------------------------------------------------

#pragma once
#ifndef deferred__renderer__CCanvas_H__
#define deferred__renderer__CCanvas_H__
//#include "Lines.h"
//#include "clientmain.h"

#if 0

#include "CColor.h"
#include "maths.h"
class CCanvas
{
	public:
		CCanvas();
		//void DrawLine( line_t *line );
		void DrawTexturedRect( int iTextureIndex, int x1, int y1, int x2, int y2 );
		void DrawTexturedRectEx( int iTextureIndex, Vector *Coords4, Vector *TexCoords4 );
		void DrawRect( int x1, int y1, int x2, int y2 );
		void DrawTriangle( int x1, int y1, int x2, int y2, int x3, int y3 );
		//void DrawMouse( void );

	public:
		void SetDrawColor( float r, float g, float b, float a );
		void SetDrawColor( int r, int g, int b, int a );
		void SetDrawColor( Color col );
		void SetWindowSize( int width, int height );

	public:
		void GetWindowSize( int *width, int *height );
		void GetMousePos( int *x, int *y );

	public:
		bool IsMouseOver( int x1, int y1, int x2, int y2 );

	private:
		inline void TransformVertex2( int &x, int &y );

	public:
		int m_iWidth;
		int m_iHeight;
		int m_iOldHeight;

	private:
		unsigned char m_bTexture2d;
		inline bool IsTexture2dEnabled( void );
		inline bool IsCurrentColor( float r, float g, float b, float a );
};

#define RESX(x) ((float)Canvas.m_iWidth/640.0f)*x
#define RESY(y) ((float)Canvas.m_iHeight/480.0f)*y

#endif

//---------------------------------------------------------------------------
#endif
