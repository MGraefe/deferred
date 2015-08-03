// renderer/COceanRenderer.h
// renderer/COceanRenderer.h
// renderer/COceanRenderer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__COceanRenderer_H__
#define deferred__renderer__COceanRenderer_H__

#include <util/maths.h>
#include "CNormalRnd.h"

class COceanFFTSolverCPU;
class COceanQuadtreeNode;
class CViewFrustum;
class CShaderWaterPretty;
class COceanRenderer
{
	friend class COceanQuadtreeNode;

public:
	COceanRenderer();
	~COceanRenderer();

	void Init();
	void ReInit();
	void UploadAndBindTexture();
	float GetWaterHeight() const { return m_waterHeight; }
	void SetWaterHeight(float height);
	void Render(CShaderWaterPretty *shader, const CViewFrustum *frustum, const Vector3f &center);
	void Pause();

private:
	void RenderMesh( Vector2f pos, float size );
	void InitConvars();
	void InitMesh();
	void RecreateBuffer(UINT &buf);
	size_t GetDataSize() const { return m_N * m_N * 4; }

private:
	int m_N;
	float m_L;
	COceanFFTSolverCPU *m_solver;
	UINT m_texture;
	UINT m_meshVertBuffer;
	UINT m_meshIndexBuffer;
	int  m_meshSize;
	UINT m_meshIndexCount;
	float m_waterHeight;
	COceanQuadtreeNode *m_quadtreeParent;
	float m_renderSize;
	float m_quadtreeMinSize;
	CShaderWaterPretty *m_shader;
	UINT m_pixelBuffers[2];
	UINT m_pixelBufferIndex;
};

#endif // deferred__renderer__COceanRenderer_H__