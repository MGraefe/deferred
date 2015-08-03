// renderer/CPostProcessor.h
// renderer/CPostProcessor.h
// renderer/CPostProcessor.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

//CPostProcessor.h

#pragma once
#ifndef deferred__renderer__CPostProcessor_H__
#define deferred__renderer__CPostProcessor_H__

typedef unsigned int UINT;

class CShaderBlur;
class CShaderBloomComplete;
class CShaderTonemap;
class CShaderBrightpass;
class CShaderExposure;

class CPostProcessor
{
public:
	CPostProcessor();
	~CPostProcessor();

	void ApplyPostProcessing( UINT sceneTex, UINT sceneDepthTex, UINT bufferTex, UINT destTex);
	void Init( void );
	void OnResolutionChange(void);
private:
	void calcExposure(UINT sourceTex);
	void brightpass( UINT sourceTex, UINT destTex, UINT lumTex );
	void blur( bool hori, UINT sourceTex, UINT destTex, int lod );
	void tonemap( UINT sceneTex, UINT sceneDepthTex, UINT bloomTex, UINT lumTex, UINT destTex );
	void bloomComplete( UINT sourceTex, UINT destTex );
private:
	//CShaderBrightpass *m_shaderBrightpass;
	CShaderBlur *m_shaderBlur;
	CShaderBloomComplete *m_shaderBloomComplete;
	CShaderTonemap *m_shaderTonemap;
	CShaderBrightpass *m_shaderBrightpass;
	CShaderExposure *m_shaderExposure;

	UINT m_bufTex2;
	UINT m_iFramebuffer;
	UINT m_LumTexs[2];
	int m_LumTexIndex;

	static const int m_lodLevels = 4;
};

#endif // deferred__renderer__CPostProcessor_H__
