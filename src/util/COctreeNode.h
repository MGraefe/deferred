// util/COctreeNode.h
// util/COctreeNode.h
// util/COctreeNode.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//COctreeNode.h

#pragma once
#ifndef deferred__util__COctreeNode_H__
#define deferred__util__COctreeNode_H__

#include <vector>
#include "triangle_t.h"
#include "maths.h"
#include "collisionTriangleAABB.h"
#include "collision.h"


class COctreeNode //can be knot or leaf
{
public:
	COctreeNode(std::vector<Triangle> *allTriangles, CPlane_t<real> *allPlanes,
		std::vector<int> triangleIndices, const AABoundingBox_t<real> &bounds, size_t maxTrianglesPerLeaf, int recursionDepth, int maxRecursionDepth) :
		m_allTriangles(allTriangles),
		m_allPlanes(allPlanes),
		m_triangleIndices(triangleIndices),
		m_bounds(bounds),
		m_treeHeight(1)
	{
		if( recursionDepth < maxRecursionDepth && getTriangleCount() > maxTrianglesPerLeaf ) //generate child-cubes
		{
			std::vector<std::vector<int>> childTriangles(8);
			std::vector<AABoundingBox_t<real>> childBounds(8);

			//calculate children bounds
			Vector3<real> verts[3] = {m_bounds.min, m_bounds.GetCenter(), m_bounds.max};
			for( size_t x = 0; x < 2; x++ )
			{
				for( size_t y = 0; y < 2; y++ )
				{
					for( size_t z = 0; z < 2; z++ )
					{
						childBounds[x*4+y*2+z].min = Vector3<real>(verts[x].x, verts[y].y, verts[z].z);
						childBounds[x*4+y*2+z].max = Vector3<real>(verts[x+1].x, verts[y+1].y, verts[z+1].z);
					}
				}
			}

			//collect children triangles
			for( int i = 0; i < 8; i++ )
			{
				for( size_t k = 0; k < getTriangleCount(); k++ )
				{
					size_t triIndex = m_triangleIndices[k];
					const Triangle &tri = m_allTriangles->at(triIndex);
					if( triangle_aabb_inters_t<real>(tri.vertex3[0].pos3, tri.vertex3[1].pos3,
							tri.vertex3[2].pos3, childBounds[i].min, childBounds[i].max) )
						childTriangles[i].push_back(triIndex);
				}
			}

			//since we do not split the triangles it is not guaranteed that the child-cubes
			//have less triangles than the parent one, in that case the algorithm would not
			//converge but produce a stack overflow
			//check if the algorithm does converge here, and if not stop splitting cubes
			bool doesConverge = false;
			for( int i = 0; i < 8; i++ )
				if( childTriangles[i].size() < getTriangleCount() )
					doesConverge = true;

			if( doesConverge )
			{
				int maxChildHeight = 0;
				for( int i = 0; i < 8; i++ )
				{
					COctreeNode *node = new COctreeNode(m_allTriangles, m_allPlanes,
						childTriangles[i], childBounds[i], maxTrianglesPerLeaf, recursionDepth+1, maxRecursionDepth);
					if(node->getTriangleCount() > 0)
					{
						m_children.push_back(node);
						maxChildHeight = max(maxChildHeight, node->getTreeHeight());
					}
					else
						delete node;
				}
				m_treeHeight = 1 + maxChildHeight;
			}
		} //getTriangleCount() > maxTrianglesPerLeaf
	}


	inline bool rayTriangleCollision(const CRay_t<real> &ray) const {
		double distHit = 0.0;
		if(!ray.intersectAABB(m_bounds, distHit, 100000000.0))
			return false;

		if( isLeaf() )
		{
			for( size_t i = 0; i < m_triangleIndices.size(); i++ )
			{
				size_t index = m_triangleIndices[i];
				const Triangle &tri = m_allTriangles->at(index);
				if( RayTriangleIntersect<real>(ray,
						real(100000000.0),
						tri.vertex3[0].pos3,
						tri.vertex3[1].pos3,
						tri.vertex3[2].pos3,
						m_allPlanes[index],
						NULL,
						NULL,
						true ) )
					return true;
			}
		}
		else //No leaf
		{
			for( size_t i = 0; i < m_children.size(); i++ )
				if(m_children[i]->rayTriangleCollision(ray))
					return true;
		}

		return false; //nothing returned true :(
	}

	~COctreeNode() {
		for( size_t i = 0; i < m_children.size(); i++ )
			delete m_children[i];
		m_children.clear();
	}

	inline bool isLeaf() const { return m_children.size() == 0; }
	inline size_t getTriangleCount() const { return m_triangleIndices.size(); }
	inline int getTreeHeight() const { return m_treeHeight; }

private:
	AABoundingBox_t<real> m_bounds;
	std::vector<COctreeNode*> m_children;
	std::vector<int> m_triangleIndices;
	std::vector<Triangle> *m_allTriangles;
	CPlane_t<real> *m_allPlanes;
	int m_treeHeight;
};

#endif
