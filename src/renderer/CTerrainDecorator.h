// renderer/CTerrainDecorator.h
// renderer/CTerrainDecorator.h
// renderer/CTerrainDecorator.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CTerrainDecorator_H__
#define deferred__renderer__CTerrainDecorator_H__

#include "CRenderEntity.h"
#include <random>

class CRenderWorld;
class CTerrainDecorator;
class CTerrainDecoration;
class CTerrainDecorationSingleModel;
class CTerrainDecorationModel;
class CVertexBuffer;
template<typename T> class CVertexIndexBuffer;

typedef int DecorIndex;
typedef int DecorTriIndex;
typedef std::string DecorName;
typedef std::map<CTerrainDecoration*, std::vector<DecorTriIndex>> DecorTriangleMap;
typedef std::vector<DecorTriIndex> DecorAvoidanceList;
typedef std::map<DecorTriIndex, std::vector<DecorTriIndex>> DecorAvoidanceMap;

//class DecorTriIndex
//{
//public:
//	DecorTriIndex(int TriIndex, CTerrainDecoration *Decor) :
//		triIndex(TriIndex),
//		decor(Decor)
//	{}
//
//	int triIndex;
//	CTerrainDecoration *decor;
//};

class CTerrainDecorationQuadtreeNode
{
public:
	CTerrainDecorationQuadtreeNode(const Vector2f &min, const Vector2f &max, const rendertri_t *allTris);
	~CTerrainDecorationQuadtreeNode();
	void CalculateTriangles(const DecorTriangleMap &parentIndices,
		const DecorAvoidanceList &avoidanceList, bool checkIntersection = true);
	void SplitCube(int depth, CTerrainDecorationQuadtreeNode *vertexBufferParent,
		CTerrainDecorationQuadtreeNode *indexBufferParent);
	void GetSplitCubesCoords( Vector2f *coords8 ) const;

	inline bool HasTriangles() const { return !m_triangleIndices.empty(); }
	inline bool IsNode() const { return m_children.size() > 0; }
	inline bool IsLeaf() const { return m_children.size() == 0; }
	inline float GetSizeX() const { return m_max.x - m_min.x; }
	inline float GetSizeY() const { return m_max.y - m_min.y; }
	inline float GetMaxSize() const { return max(GetSizeX(), GetSizeY()); }
	void GetChildLeafs(std::vector<CTerrainDecorationQuadtreeNode*> &leafs);
	void Populate(CTerrainDecorator *decorator,
		CTerrainDecorationQuadtreeNode *vertexBufferParent,
		CTerrainDecorationQuadtreeNode *indexBufferParent,
		const Vector2f &quadMin, const Vector2f &quadMax);
	void Unpopulate();
	void Render(const Vector2f &quadMin, const Vector2f &quadMax);
	bool Intersect(const Vector2f &quadMin, const Vector2f &quadMax) const;
	void FilterAvoidanceList();
	bool ShouldAvoidTriangle(DecorTriIndex avoidTriIndex) const;
	bool ShouldAvoidTriangle(DecorTriIndex decorTriIndex, DecorTriIndex avoidanceTriIndex) const;
	bool IntersectWithAvoidance(const CRay &ray, DecorTriIndex hitTri, float hitDist);
	bool Raytrace(const CRay &ray, DecorTriIndex triIndex, Vector3f *basePos, float *hitDist) const;
private:
	void Populate(CTerrainDecorator *decorator,
		CTerrainDecorationQuadtreeNode *vertexBufferParent,
		CTerrainDecorationQuadtreeNode *indexBufferParent,
		const CTerrainDecorationSingleModel *model,
		const std::vector<DecorTriIndex> &triIndices);
	void SubmitModel(CTerrainDecorator *decorator,
		CTerrainDecorationQuadtreeNode *vertexBufferParent,
		CTerrainDecorationQuadtreeNode *indexBufferParent,
		const CTerrainDecorationSingleModel *modelInfo,
		const Vector3f &pos, float skyVisibility);

	const static float minSizeVertexBuffer;
	const static float minSizeIndexBuffer;
	const static float minSizeRayTrace;
	const static float rayHeight;

	DecorTriangleMap m_triangleIndices;
	DecorAvoidanceList m_avoidanceList;
	DecorAvoidanceMap m_avoidanceMap;
	const rendertri_t *m_allTris;
	std::vector<CTerrainDecorationQuadtreeNode*> m_children;
	Vector2f m_min;
	Vector2f m_max;
	CVertexIndexBuffer<UINT> *m_indexBuffer;
	CVertexBuffer *m_vertexBuffer;
};


class CTerrainDecorationModel
{
public:
	CTerrainDecorationModel();
	bool Init(CTerrainDecorator *decorator, const std::string &filename);

	inline const std::string &GetName() const { return m_name; }
	inline const rendertri_t *GetTriangles() const { return m_triangles; }
	inline int GetTriangleCount() const { return m_triangleCount; }
private:
	std::string m_name;
	rendertri_t *m_triangles;
	int m_triangleCount;
};


class CTerrainDecorationSingleModel
{
public:
	CTerrainDecorationSingleModel(const std::string &filename, const CScriptSubGroup *data);
	bool Init(CTerrainDecorator *decorator);

	inline const CTerrainDecorationModel *GetModel() const { return m_model; }
	inline float GetDensity() const { return m_density; }
	inline float GetMinScale() const { return m_minScale; }
	inline float GetMaxScale() const { return m_maxScale; }
	inline float GetRandomness() const { return m_randomness; }

private:
	std::string m_filename;
	CTerrainDecorationModel *m_model;
	float m_density;
	float m_randomness;
	float m_minScale;
	float m_maxScale;
};


class CTerrainDecoration
{
public:
	CTerrainDecoration(const DecorName &name, DecorIndex index, const CScriptSubGroup *group);
	bool Init(CTerrainDecorator *decorator);
	inline const std::vector<CTerrainDecorationSingleModel*> &GetModelList() const { return m_modelList; }
private:
	DecorName m_name;
	DecorIndex m_index;
	const CScriptSubGroup *m_group;
	std::vector<CTerrainDecorationSingleModel*> m_modelList;
};


class CTerrainDecorator
{
	friend class CTerrainDecorationQuadtreeNode;

public:
	CTerrainDecorator();
	~CTerrainDecorator();

	void PreprocessTerrain(CRenderWorld *world);
	void Update();
	void VRender( const CRenderParams &params, const Vector3f &camPos, float radius );
	void VOnRestore( void );
	void Populate( const Vector3f &camPos, float radius );
	void AddVertexBufferMemory(size_t mem);
	CTerrainDecorationModel *GetDecorModel(const std::string &filename);

    std::mt19937 &GetRndGen() { return m_rndGen; }
private:
	CVertexIndexBuffer<UINT> *SubmitVertices(rendervert_t *verts, size_t count);
	bool LoadDecorationFile();
	CTerrainDecorationQuadtreeNode *m_treeRoot;
	CScriptParser m_decorFile;

	typedef std::map<DecorName, DecorIndex, LessThanCppStringObj> DecorNameIndexMap;
	typedef std::map<DecorIndex, CTerrainDecoration*> DecorIndexMap;
	typedef std::map<std::string, CTerrainDecorationModel*, LessThanCppStringObj> ModelNameIndexMap;

	DecorNameIndexMap m_decorNameIndexMap;
	DecorIndexMap m_decorIndexMap;
	ModelNameIndexMap m_decorModels;
	size_t m_vertexBufferMemory;
	size_t m_maxVertexBufferMemory;
	std::mt19937 m_rndGen;
};


#endif
