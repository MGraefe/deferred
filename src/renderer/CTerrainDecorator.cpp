// renderer/CTerrainDecorator.cpp
// renderer/CTerrainDecorator.cpp
// renderer/CTerrainDecorator.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CTerrainDecorator.h"
#include <util/collisionTriangleAABB.h>
#include "CRenderWorld.h"
#include <util/CConVar.h>
#include <util/CScriptParser.h>
#include "RendererRPM.h"
#include "TextureManager.h"
#include "CVertexBuffer.h"
#include <util/collision.h>
#include "CShaders.h"
#include "CVertexIndexBuffer.h"
#include "renderer.h"
#include "CCamera.h"
#include "CViewFrustum.h"
#include <fstream>

const float CTerrainDecorationQuadtreeNode::minSizeVertexBuffer = 750.0f;
const float CTerrainDecorationQuadtreeNode::minSizeIndexBuffer = 100.0f;
const float CTerrainDecorationQuadtreeNode::minSizeRayTrace = 100.0f;
const float CTerrainDecorationQuadtreeNode::rayHeight = 3200.0f;

size_t g_raysGood = 0;
size_t g_raysBad = 0;

CConVar terrain_decoration_file("terrain.decoration.file", "scripts/decoration.txt");

//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
CTerrainDecorator::CTerrainDecorator() :
	m_rndGen(25418348)
{
	m_treeRoot = NULL;
	m_vertexBufferMemory = 0;
	m_maxVertexBufferMemory = 0;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
CTerrainDecorator::~CTerrainDecorator()
{
	if( m_treeRoot )
		delete m_treeRoot;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
bool CTerrainDecorator::LoadDecorationFile()
{
	const char *filename = terrain_decoration_file.GetString().c_str();

	if( !m_decorFile.ParseFile(filename) )
	{
		error("Error parsing terrain decoration-file \"%s\"", filename);
		return false;
	}

	//Load all models which are referenced in this decor-file
	for( auto it = m_decorFile.GetSubGroupMap()->begin(); it != m_decorFile.GetSubGroupMap()->end(); ++it )
	{
		for( auto modelIt = it->second->GetSubGroupMap()->begin(); modelIt != it->second->GetSubGroupMap()->end(); ++modelIt )
		{
			if(m_decorModels.find(modelIt->first) == m_decorModels.end()) //model not yet loaded?
			{
				CTerrainDecorationModel *model = new CTerrainDecorationModel();
				if( model->Init(this, modelIt->first) )
					m_decorModels[modelIt->first] = model;
				else
					delete model;
			}
		}
	}

	//Load decorations
	int decorIndex = 0;
	for( auto it = m_decorFile.GetSubGroupMap()->begin(); it != m_decorFile.GetSubGroupMap()->end(); ++it )
	{
		CTerrainDecoration *decor = new CTerrainDecoration(it->first, decorIndex, it->second);
		if( decor->Init(this) )
		{
			m_decorNameIndexMap[it->first] = decorIndex;
			m_decorIndexMap[decorIndex] = decor;
			decorIndex++;
		}
		else
			delete decor;
	}

	return true;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorator::PreprocessTerrain(CRenderWorld *world)
{
	if( !LoadDecorationFile() )
		return;

	UINT start = g_RenderInterf->GetTimer()->GetTimeMs();

	CTextureManager *texMgr = g_RenderInterf->GetTextureManager();
	const rendertri_t *allTris = world->GetTriangles();
	const AABoundingBox &aabb = world->GetBoundingBox();

	DecorTriangleMap decorTriangleMap;
	DecorAvoidanceList avoidanceTriangles;
	for(int i = 0; i < world->GetTriangleCount(); i++)
	{
		if(allTris[i].iTexture >= 0)
		{
			const std::string &decorName = texMgr->GetMaterialProps(allTris[i].iTexture).decoration;
			if(!decorName.empty())
			{
				auto it = m_decorNameIndexMap.find(decorName);
				if( it != m_decorNameIndexMap.end() )
				{
					CTerrainDecoration *decor = m_decorIndexMap[it->second];
					decorTriangleMap[decor].push_back(i);
				}
			}
			else
			{
				avoidanceTriangles.push_back(i);
			}
		}
	}

	m_treeRoot = new CTerrainDecorationQuadtreeNode(Vector2f(aabb.min.x, aabb.min.z), Vector2f(aabb.max.x, aabb.max.z), allTris);
	m_treeRoot->CalculateTriangles(decorTriangleMap, avoidanceTriangles, false);
	m_treeRoot->SplitCube(1, NULL, NULL);
	m_treeRoot->FilterAvoidanceList();

	UINT end = g_RenderInterf->GetTimer()->GetTimeMs();
	ConsoleMessage("CTerrainDecorator: splitting into quadtree needed %u ms", end-start);

	Populate(Vector3f(), 1000000.0f); //TODO: write intelligent update method
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorator::Update()
{

}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorator::VRender( const CRenderParams &params, const Vector3f &camPos, float radius )
{
	if(m_treeRoot && !m_decorModels.empty() && m_decorModels.begin()->second &&
		m_decorModels.begin()->second->GetTriangleCount() > 0)
	{
		//Bind the first material. Our system is restricted to one single material.
		int matIndex = m_decorModels.begin()->second->GetTriangles()[0].iTexture;
		g_RenderInterf->GetTextureManager()->BindMaterial(matIndex);
		bool bDisableCulling = g_RenderInterf->GetTextureManager()->GetMaterialProps(matIndex).bDoubleSided;
		if( bDisableCulling )
			GetGLStateSaver()->Disable(GL_CULL_FACE);
		Vector2f rad(radius, radius);
		Vector2f cam(camPos.x, camPos.z);
		m_treeRoot->Render(cam - rad, cam + rad);
		glBindBuffer( GL_ARRAY_BUFFER, 0 ); //Unbind last used vertex buffer
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); //And last used index buffer
		if( bDisableCulling )
			GetGLStateSaver()->Enable(GL_CULL_FACE);
	}
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorator::VOnRestore( void )
{

}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorator::Populate( const Vector3f &camPos, float radius )
{
	Assert(radius > 0.0f);
	Vector2f rad(radius, radius);
	Vector2f quadMin = Vector2f(camPos.x, camPos.z) - rad;
	Vector2f quadMax = Vector2f(camPos.x, camPos.z) + rad;

	UINT startTime = g_RenderInterf->GetTimer()->GetTimeMs();
	m_treeRoot->Populate(this, NULL, NULL, quadMin, quadMax);
	UINT endTime = g_RenderInterf->GetTimer()->GetTimeMs();

	ConsoleMessage("CTerrainDecorator: Population took %u ms and %u KB of vertex-memory,"
		"with a maximum size of a single vertex-buffer of %u KB",
		endTime-startTime, m_vertexBufferMemory/1024, m_maxVertexBufferMemory/1024);
	ConsoleMessage("Good rays: %u, Bad rays: %u", g_raysGood, g_raysBad);
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
CTerrainDecorationModel *CTerrainDecorator::GetDecorModel(const std::string &filename)
{
	auto it = m_decorModels.find(filename);
	if( it != m_decorModels.end() )
		return it->second;
	else
		return NULL;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
CTerrainDecoration::CTerrainDecoration(const DecorName &name, DecorIndex index, const CScriptSubGroup *group) :
	m_name(name),
	m_index(index),
	m_group(group)
{

}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
bool CTerrainDecoration::Init(CTerrainDecorator *decorator)
{
	for( auto it = m_group->GetSubGroupMap()->begin(); it != m_group->GetSubGroupMap()->end(); ++it )
	{
		CTerrainDecorationSingleModel *singleModel =
			new CTerrainDecorationSingleModel(it->first, it->second);
		if( singleModel->Init(decorator) )
			m_modelList.push_back(singleModel);
		else
			delete singleModel;
	}

	return !m_modelList.empty();
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
CTerrainDecorationSingleModel::CTerrainDecorationSingleModel(const std::string &filename, const CScriptSubGroup *data) :
	m_filename(filename)
{
	m_density = data->GetFloat("density", 1.0f);
	m_randomness = data->GetFloat("randomness", 0.5f);
	m_minScale = data->GetFloat("minScale", 0.8f);
	m_maxScale = data->GetFloat("maxScale", 1.2f);	
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
bool CTerrainDecorationSingleModel::Init(CTerrainDecorator *decorator)
{
	m_model = decorator->GetDecorModel(m_filename);
	return m_model != NULL;
}


CTerrainDecorationModel::CTerrainDecorationModel()
{
	m_triangles = NULL;
	m_triangleCount = 0;
}

//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
bool CTerrainDecorationModel::Init(CTerrainDecorator *decorator, const std::string &filename)
{
	m_triangles = LoadRPMFileRenderer(filename.c_str(), m_triangleCount, NULL, NULL, NULL);
	if(!m_triangles)
	{
		error("Error loading geometry-decoration-model \"%s\"", filename.c_str());
		return false;
	}
	return true;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorator::AddVertexBufferMemory(size_t mem)
{
	m_vertexBufferMemory += mem;
	if( mem > m_maxVertexBufferMemory )
		m_maxVertexBufferMemory = mem;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
CTerrainDecorationQuadtreeNode::CTerrainDecorationQuadtreeNode(const Vector2f &min, const Vector2f &max, const rendertri_t *allTris) :
	m_min(min),
	m_max(max),
	m_allTris(allTris)
{
	m_children.reserve(4);
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;
	Assert(minSizeRayTrace <= minSizeIndexBuffer && minSizeIndexBuffer <= minSizeVertexBuffer);
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
CTerrainDecorationQuadtreeNode::~CTerrainDecorationQuadtreeNode()
{
	if(m_vertexBuffer)
		delete m_vertexBuffer;
	if(m_indexBuffer)
		delete m_indexBuffer;
	for(CTerrainDecorationQuadtreeNode *child : m_children)
		delete child;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorationQuadtreeNode::CalculateTriangles(const DecorTriangleMap &parentIndices,
														const DecorAvoidanceList &avoidanceList,
														bool checkIntersection /*= true*/)
{
	if(checkIntersection)
	{
		for(auto decorIt = parentIndices.begin(); decorIt != parentIndices.end(); ++decorIt)
		{
			for(int index : decorIt->second)
			{
				const rendertri_t &tri = m_allTris[index];
				bool intersect = triangle_aabb_inters_2d_xz(m_min, m_max, tri.pVerts[0].vPos, tri.pVerts[1].vPos, tri.pVerts[2].vPos);
				if( intersect )
					m_triangleIndices[decorIt->first].push_back(index);
			}
		}
		for(int avoidanceIndex : avoidanceList)
		{
			const rendertri_t &tri = m_allTris[avoidanceIndex];
			bool intersect = triangle_aabb_inters_2d_xz(m_min, m_max, tri.pVerts[0].vPos, tri.pVerts[1].vPos, tri.pVerts[2].vPos);
			if( intersect )
				m_avoidanceList.push_back(avoidanceIndex);
		}
	}
	else
	{
		m_triangleIndices = parentIndices;
		m_avoidanceList = avoidanceList;
	}
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorationQuadtreeNode::SplitCube(int depth, CTerrainDecorationQuadtreeNode *vertexBufferParent,
											   CTerrainDecorationQuadtreeNode *indexBufferParent)
{
	if( !vertexBufferParent && GetMaxSize() <= minSizeVertexBuffer)
	{
		m_vertexBuffer = new CVertexBuffer();
		m_vertexBuffer->Init();
		vertexBufferParent = this;
	}
	if( !indexBufferParent && GetMaxSize() <= minSizeIndexBuffer)
	{
		m_indexBuffer = new CVertexIndexBuffer<UINT>();
		m_indexBuffer->Init();
		indexBufferParent = this;
	}

	if( GetMaxSize() > minSizeRayTrace )
	{
		Vector2f coords[8];
		GetSplitCubesCoords(coords);

		for( int i = 0; i < 4; i++ )
		{
			CTerrainDecorationQuadtreeNode *child = new CTerrainDecorationQuadtreeNode(coords[2*i], coords[2*i+1], m_allTris);
			child->CalculateTriangles(m_triangleIndices, m_avoidanceList);
			if( child->HasTriangles() )
			{
				child->SplitCube(depth+1, vertexBufferParent, indexBufferParent);
				m_children.push_back(child);
			}
			else
				delete child;
		}
	}

	//clear unnecessary data
	if( IsNode() )
		m_triangleIndices.clear();
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorationQuadtreeNode::GetSplitCubesCoords( Vector2f *coords8 ) const
{
	Vector2f mid = (m_min + m_max) * 0.5f;

	coords8[0] = m_min;
	coords8[1] = mid;

	coords8[2] = Vector2f(mid.x, m_min.y);
	coords8[3] = Vector2f(m_max.x, mid.y);

	coords8[4] = Vector2f(m_min.x, mid.y);
	coords8[5] = Vector2f(mid.x, m_max.y);

	coords8[6] = mid;
	coords8[7] = m_max;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorationQuadtreeNode::GetChildLeafs(std::vector<CTerrainDecorationQuadtreeNode*> &leafs)
{
	if( IsLeaf() )
		leafs.push_back(this);
	else
		for(CTerrainDecorationQuadtreeNode *child : m_children)
			child->GetChildLeafs(leafs);
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorationQuadtreeNode::Populate(CTerrainDecorator *decorator,
											  CTerrainDecorationQuadtreeNode *vertexBufferParent,
											  CTerrainDecorationQuadtreeNode *indexBufferParent,
											  const Vector2f &quadMin, const Vector2f &quadMax)
{
	//when we have a vertexbuffer-parent we need to generate geometry for all children in order to have it filled completely.
	//we do not update existing vertex-buffers.
	if( !vertexBufferParent && !Intersect(quadMin, quadMax) )
		return;
	if( m_vertexBuffer )
		vertexBufferParent = this;
	if( m_indexBuffer )
		indexBufferParent = this;

	if(IsLeaf())
	{
		Assert(vertexBufferParent);
		Assert(indexBufferParent);
		for(DecorTriangleMap::const_iterator it = m_triangleIndices.begin(); it != m_triangleIndices.end(); ++it)
			for( const CTerrainDecorationSingleModel *modelInfo : it->first->GetModelList() )
				Populate(decorator, vertexBufferParent, indexBufferParent, modelInfo, it->second);
	}
	else
	{
		for(CTerrainDecorationQuadtreeNode *child : m_children)
			child->Populate(decorator, vertexBufferParent, indexBufferParent, quadMin, quadMax);
	}

	if( m_vertexBuffer )
	{
		m_vertexBuffer->SubmitTriangleList();
		decorator->AddVertexBufferMemory(m_vertexBuffer->GetVertexCountSubmitted() * sizeof(rendervert_t));
	}
	if( m_indexBuffer )
		m_indexBuffer->SubmitIndexList();
}



//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
bool CTerrainDecorationQuadtreeNode::Raytrace(const CRay &ray, DecorTriIndex triIndex, Vector3f *basePos, float *hitDist) const
{
	const rendertri_t &tri = m_allTris[triIndex];
	CPlane plane(tri.pVerts[0].vPos, tri.vTriNorm);
	return RayTriangleIntersect<float>(ray, rayHeight*2.0f,
		tri.pVerts[0].vPos, tri.pVerts[1].vPos, tri.pVerts[2].vPos,
		plane, basePos, hitDist, false);
}



//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorationQuadtreeNode::Populate(CTerrainDecorator *decorator,
											  CTerrainDecorationQuadtreeNode *vertexBufferParent,
											  CTerrainDecorationQuadtreeNode *indexBufferParent,
											  const CTerrainDecorationSingleModel *model,
											  const std::vector<DecorTriIndex> &triIndices)
{
	Assert(IsLeaf());
	float eps = 0.000001f;
	float spacing = 10.0f / max(model->GetDensity(), eps);
	float x0 = round(m_min.x / spacing) * spacing - eps;
	float x1 = round(m_max.x / spacing) * spacing + eps;
	float y0 = round(m_min.y / spacing) * spacing - eps;
	float y1 = round(m_max.y / spacing) * spacing + eps;

	std::uniform_real_distribution<float> distr(-spacing * model->GetRandomness(), spacing * model->GetRandomness());
	auto &gen = decorator->GetRndGen();
	
	//ConsoleMessage("x0 = %f, x1 = %f, y0 = %f, y1 = %f, spacing = %f", x0, x1, y0, y1, spacing);
	for(float x = x0; x <= x1; x += spacing)
	{
		for(float y = y0; y <= y1; y += spacing)
		{
			Vector3f basePos;
			float randomX = distr(gen);
			float randomY = distr(gen);
			CRay ray(Vector3f(x+randomX, rayHeight, y+randomY), Vector3f(0.0f, -1.0f, 0.0f));
			bool inters = false;
			DecorTriIndex hitTri;
			float hitDist = 0.0f;
			for(DecorTriIndex i : triIndices)
			{
				if(Raytrace(ray, i, &basePos, &hitDist))
				{
					inters = true;
					hitTri = i;
					g_raysGood++;
					break;
				}
				else
					g_raysBad++;
			}
			if(inters && !IntersectWithAvoidance(ray, hitTri, hitDist))
			{
				float skyVisibility = m_allTris[hitTri].pVerts[0].fSkyVisibility; //TODO: interpolate
				SubmitModel(decorator, vertexBufferParent, indexBufferParent, model, basePos, skyVisibility);
			}
		}
	}
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorationQuadtreeNode::SubmitModel(CTerrainDecorator *decorator,
												 CTerrainDecorationQuadtreeNode *vertexBufferParent,
												 CTerrainDecorationQuadtreeNode *indexBufferParent,
												 const CTerrainDecorationSingleModel *modelInfo,
												 const Vector3f &pos, float skyVisibility)
{
	//TODO: consider randomness
	const CTerrainDecorationModel *model = modelInfo->GetModel();
	size_t numVerts = model->GetTriangleCount()*3;
	if( numVerts < 2 )
		return;

    std::uniform_real_distribution<float> distr(modelInfo->GetMinScale(), modelInfo->GetMaxScale());
	float scale = distr(decorator->GetRndGen());
	rendervert_t *transformedVerts = new rendervert_t[model->GetTriangleCount()*3];
	const rendertri_t *tris = model->GetTriangles();

	for(int i = 0; i < model->GetTriangleCount(); i++)
	{
		for(int k = 0; k < 3; k++)
		{
			transformedVerts[i*3+k] = tris[i].pVerts[k];
			transformedVerts[i*3+k].vPos *= scale;
			transformedVerts[i*3+k].vPos += pos;
			transformedVerts[i*3+k].fSkyVisibility = skyVisibility;
		}
	}

	size_t indexStart = vertexBufferParent->m_vertexBuffer->GetVertexCountUnsubmitted();
	size_t indexEnd = indexStart + numVerts - 1;
	vertexBufferParent->m_vertexBuffer->AddVertexesToList(transformedVerts, numVerts);
	indexBufferParent->m_indexBuffer->AddIndexRangeToList(indexStart, indexEnd);

	delete[] transformedVerts;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorationQuadtreeNode::Unpopulate()
{
	//if( m_buffer )
	//	delete m_buffer;
	//m_buffer = NULL;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorationQuadtreeNode::Render(const Vector2f &quadMin, const Vector2f &quadMax)
{
	if( !Intersect(quadMin, quadMax) )
		return;

	if( m_vertexBuffer )
	{
		m_vertexBuffer->Bind();
		g_RenderInterf->GetShaderManager()->InitVertexbufferVars(VBF_RENDERVERT, NULL);
	}

	if( m_indexBuffer )
	{
		Vector3f min3(m_min.x, -32000.0f, m_min.y);
		Vector3f max3(m_max.x, 32000.0f, m_max.y);
		CViewFrustum *frustum = g_RenderInterf->GetRenderer()->GetActiveCam()->GetFrustum();
		if(frustum->AxisAlignedCubeInFrustum(min3, max3))
		{
			m_indexBuffer->Bind();
			m_indexBuffer->Render();
		}
	}
	else
	{
		for(CTerrainDecorationQuadtreeNode *child : m_children)
			child->Render(quadMin, quadMax);
	}
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
bool CTerrainDecorationQuadtreeNode::Intersect(const Vector2f &quadMin, const Vector2f &quadMax) const
{
	return IntersectAxisAlignedQuadQuad2D(m_min, m_max, quadMin, quadMax);
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
void CTerrainDecorationQuadtreeNode::FilterAvoidanceList()
{
	if(IsLeaf())
	{
		for(auto it = m_triangleIndices.begin(); it != m_triangleIndices.end(); it++)
			for(DecorTriIndex decorTri : it->second)
				for(DecorTriIndex avoidTri : m_avoidanceList)
					if(ShouldAvoidTriangle(decorTri, avoidTri))
						m_avoidanceMap[decorTri].push_back(avoidTri);
	}
	else
	{
		for(CTerrainDecorationQuadtreeNode *child : m_children)
			child->FilterAvoidanceList();
	}

	m_avoidanceList.clear();
	m_avoidanceList.shrink_to_fit();
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
bool CTerrainDecorationQuadtreeNode::ShouldAvoidTriangle(DecorTriIndex avoidTriIndex) const
{
	for(auto it = m_triangleIndices.begin(); it != m_triangleIndices.end(); ++it)
		for(int decorId : it->second)
			if( ShouldAvoidTriangle(avoidTriIndex, decorId) )
				return true;
	return false;
}



inline Vector2f ToVector2D_XZ(const Vector3f &vec)
{
	return Vector2f(vec.x, vec.z);
}


inline void GetTriangleAABB2D_XZ(const rendertri_t &tri, Vector2f &min, Vector2f &max)
{
	min = max = ToVector2D_XZ(tri.pVerts[0].vPos);
	//TODO: finish
}

const float maxNormalDiff = acos(DEG2RAD(45.0f));
const float maxYDiff = 10.0f; //50 cm
//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
bool CTerrainDecorationQuadtreeNode::ShouldAvoidTriangle(DecorTriIndex decorTriIndex, DecorTriIndex avoidanceTriIndex) const
{
	const rendertri_t &decorTri = m_allTris[decorTriIndex];
	const rendertri_t &avoidTri = m_allTris[avoidanceTriIndex];

	if(decorTri.vTriNorm.Dot(avoidTri.vTriNorm) < maxNormalDiff)
		return false;

	float centerDecorY = (decorTri.pVerts[0].vPos.y + decorTri.pVerts[1].vPos.y + decorTri.pVerts[2].vPos.y) / 3.0f;
	float centerAvoidY = (avoidTri.pVerts[0].vPos.y + avoidTri.pVerts[1].vPos.y + avoidTri.pVerts[2].vPos.y) / 3.0f;
	if(centerAvoidY < centerDecorY - maxYDiff || centerAvoidY > centerDecorY + maxYDiff)
		return false;

	//TODO: check for collision in a 2D xz style
	return true;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
bool CTerrainDecorationQuadtreeNode::IntersectWithAvoidance(const CRay &ray, DecorTriIndex hitTri, float hitDist)
{
	DecorAvoidanceMap::const_iterator it = m_avoidanceMap.find(hitTri);
	if(it == m_avoidanceMap.end())
		return false;

	const std::vector<DecorTriIndex> &indexList = it->second;
	float avoidHitDist = 0.0f;
	for(DecorTriIndex i : indexList)
	{
		if(Raytrace(ray, i, NULL, &avoidHitDist) && avoidHitDist < hitDist && avoidHitDist > hitDist - maxYDiff)
			return true;
	}

	return false;
}
