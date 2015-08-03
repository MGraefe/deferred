// renderer/CSkyBox.h
// renderer/CSkyBox.h
// renderer/CSkyBox.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CSkyBox_H__
#define deferred__renderer__CSkyBox_H__

#include "texture_t.h"
#include <util/maths.h>
#include <util/dll.h>

class CShaderUnlit;
class CShaderSky;

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING

class DLL CSkyBox
{
public:
	CSkyBox();
	~CSkyBox();
	void LoadSkyTextures( const char *pSkyName );
	void DrawSky( Vector3f vSunDir, bool bReflection = false );
	void OnRestore( void );
	UINT GetCubeMapTexture( void ) { return m_iCubeMapTex; }
private:
	bool m_bSkyLoaded;
	//texture_t m_SkyTexs[6];
	CShaderUnlit *m_pShader;
	CShaderSky *m_pShaderSky;
	char m_pSkyName[64];
	UINT m_iCubeMapTex;
};

POP_PRAGMA_WARNINGS

#endif
