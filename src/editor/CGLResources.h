// editor/CGLResources.h
// editor/CGLResources.h
// editor/CGLResources.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once

#include "stdafx.h"
#include <util/CScriptParser.h>


class CRenderWorld;
class CGLResources
{
public:
	CGLResources();
	~CGLResources();

	void Init( HDC hDC, HWND hWnd, int resx3d, int resy3d );
	CRenderWorld *LoadWorldFromFile( const char *filename );
	void SetActive( HDC hdc );
	HGLRC GetOglContext( void ) { return m_OglRenderCtx; }
	void UpdateAndDraw();
public:
	bool m_initialized;
	CModel *m_model;
	CRenderWorld *m_world;
	CCTime *m_timer;
	CEventManager *m_EvtMgr;
	gpGlobals_t *m_Globals;
	CRenderInterfaces *m_pRenderInterfaces;
	HGLRC m_OglRenderCtx;
};


CGLResources *GetGLResources(void);
