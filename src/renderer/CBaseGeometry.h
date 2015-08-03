// renderer/CBaseGeometry.h
// renderer/CBaseGeometry.h
// renderer/CBaseGeometry.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CBaseGeometry_H__
#define deferred__renderer__CBaseGeometry_H__

#include "CRenderEntity.h"
#include "mesh_vertex_t.h"
#include "texture_t.h"
#include "CShaders.h"
#include "CGeometryManager.h"
#include <util/rpm_file.h>
#include <util/dll.h>
#include <util/CSteppedInterpolator.h>

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING;

class DLL CBaseGeometry : public CRenderEntity
{
public:
	CBaseGeometry( int index );
	virtual ~CBaseGeometry();

	virtual void VRender( const CRenderParams &params );
	virtual void RenderNoTextureBufferOnly( bool bindWhiteTexture = true );
	virtual void VPreDelete( void );
	virtual void VOnRestore( void );

	bool LoadGeometry( const char *filename, const char *shadername );
	void SetScale( float scale ) { m_vScale.x = m_vScale.y = m_vScale.z = scale; }
	void SetScale( const Vector3f &scale ) { m_vScale = scale; }
	const Vector3f &GetScale( void ) const { return m_vScale; }
	AABoundingBox GetBoundingBoxRaw(void) { 
		return m_ModelInfo.rpm.header.bounds;
	}

protected:
	bool LoadGeometryFromFile( const char *filename, const char *shadername );

private:
	void DrawGeometry( const CRenderParams &params );

	void UpdateSkyVisibility();

private:
	Vector3f m_vScale;
	mesh_vertex_t *m_pVertexes;
	modelinfo_t m_ModelInfo;
	int m_iVertexes;
	texture_t m_TexDiffuse;
	texture_t m_TexNormal;
	char m_pchShaderName[32];
	CShader *m_pShader;
	CSteppedInterpolator<float, float> m_skyVisInterp;
};

POP_PRAGMA_WARNINGS;

#endif
