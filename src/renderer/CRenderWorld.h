// renderer/CRenderWorld.h
// renderer/CRenderWorld.h
// renderer/CRenderWorld.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CRenderWorld_H__
#define deferred__renderer__CRenderWorld_H__

#include "CRenderEntity.h"
#include <vector>
#include <util/dll.h>
#include <util/CLightProbe.h>
//#include "basic_types.h"
//#include "scenetree.h"
//#include "texture_t.h"

class CSTreeCube;
struct rendertri_t;
class CVertexBuffer;
template<typename T> class CVertexIndexBuffer;
class CTerrainDecorator;
class rpm_file_t;

struct translucency_indexbuffer_t
{
	int iTexIndex;
	CVertexIndexBuffer<USHORT> *pIndexBuffer;
};

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING

class DLL CRenderWorld : public CRenderEntity
{
public:
	CRenderWorld();
	~CRenderWorld();

	void LoadWorld( const char *pFileName );
	void LoadWorld( const CScriptParser &worldFile );

	void VRender(const CRenderParams &params);

	void RenderNoTextureBufferOnly();
	void VOnRestore();
	void VPreDelete();
	
	void Cleanup( void );

	inline const rendertri_t *GetTriangles(void) const { return m_pTris; }
	inline int GetTriangleCount() const { return m_iTris; }
	inline const AABoundingBox &GetBoundingBox() const { return m_boundingBox; }

	inline const std::vector<CLightProbe> &GetLightProbes() const { return m_lightProbes; }
	float GetLightprobeSkyVis(const Vector3f &pos);
	bool LightProbesChanged(const std::vector<CLightProbe> &parsedProbes, const rpm_file_t &rpmfile);

private:
	void LoadWorld(rpm_file_t &rpm);
	void RenderSingleTexIndex( int index );
	void RenderTranslucentThings();
	void StoreTranslucentTriangles( rendertri_t *pTris, const std::vector<int> &transTriIndexes );

	int m_iTextures;
	int *m_pMaterials;
	int m_iTris;
	char m_pchFilename[128];
	rendertri_t *m_pTris;
	CSTreeCube *pTreeRoot;
	CVertexBuffer *m_pVertexBuffer;
	std::vector<translucency_indexbuffer_t*> m_TranslucentIndexBuffers;
	std::vector<int> m_vTexIndexes;
	AABoundingBox m_boundingBox;
	CTerrainDecorator *m_terrainDecorator;
	std::vector<CLightProbe> m_lightProbes;
};

POP_PRAGMA_WARNINGS

#endif
