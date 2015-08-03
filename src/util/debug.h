// util/debug.h
// util/debug.h
// util/debug.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__debug_H__
#define deferred__util__debug_H__

#include "basic_types.h"
//#include <renderer/CCanvas.h>
//#include <vector>
namespace Debug
{
	extern UINT g_iWhiteTexture;
	//extern void DrawLine( const Vector3f &vStart, const Vector3f &vEnd, const Color &color, const float &fLineWidth = 1.0f );
	//extern void DrawCube( const Vector3f &vCenter, const float &fWidth, const Color &color, const float &fLineWidth = 1.0f );
	extern void InitDebug( UINT iWhiteTexture );

	//class CRenderLine
	//{
	//public:
	//	Vector3f m_vStart;
	//	Color colorStart;
	//	Vector3f m_vStop;
	//	Color colorEnd;
	//};

	//class CRenderLineList
	//{
	//private:
	//	static const int BLOCKSIZE = 64*1024; //64 kb
	//	CRenderLineList( const CRenderLineList &c ) { }
	//public:
	//	CRenderLineList() : m_iLines(0), m_iMemSize(0), m_iOccMem(0), m_pMem(NULL) { }
	//	~CRenderLineList();
	//	void AddLine( const Vector3f &vStart, const Vector3f &vEnd, const Color &color );
	//	void Render( void );
	//	void Clear( void );
	//private:
	//	CRenderLine *m_pMem;
	//	unsigned int m_iMemSize;
	//	unsigned int m_iOccMem;
	//	unsigned int m_iLines;
	//};

	//extern CRenderLineList *g_pRenderLineList; 
};

#endif
