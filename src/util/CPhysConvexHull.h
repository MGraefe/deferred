// util/CPhysConvexHull.h
// util/CPhysConvexHull.h
// util/CPhysConvexHull.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__CPhysConvexHull_H__
#define deferred__util__CPhysConvexHull_H__

#include <istream>
#include <ostream>
#include <vector>
#include "maths.h"
#include "readbinary.h"

// Represents a convex hull
// NOTE: At the moment vertexes are not filtered in this object (they may be filtered in the physics-library),
// so there may be vertices which are fully inside the hull that can be ignored.
// Maybe we filter these in the future
class CPhysConvexHull
{
public:
	typedef Vector3f Vertex;
	typedef std::vector<Vertex> VertexList;

	CPhysConvexHull() : m_materialIndex(-1) { }

	//Get list of vertices
	inline const VertexList &GetVertexList(void) const { return m_verts; }

	//Get a single vertex by id
	inline const Vertex &GetVertex(UINT i) const { Assert(i < m_verts.size()); return m_verts[i]; }

	//Get the number of vertices
	inline UINT GetSize(void) const { return m_verts.size(); }

	//Add a vertex to the shape. NOTE: vertices are not filtered inside this object.
	inline void AddVertex(const Vertex &vert) { m_verts.push_back(vert); }

	inline bool containsVertex(const Vertex &vert, float maxDist = 0.001f) {
		float maxDistSq = maxDist*maxDist;
		for(size_t i = 0; i < m_verts.size(); i++)
			if(m_verts[i].distToSq(vert) <= maxDistSq)
				return true;
		return false;
	}

	inline int GetMaterialIndex(void) const { return m_materialIndex; }
	inline void SetMaterialIndex(int index) { m_materialIndex = (short int)index; }

	//serialize to an ostream
	inline error_e Serialize(std::ostream &os) const {
		UINT verts = m_verts.size();
		WRITE_BINARY(os, m_materialIndex);
		WRITE_BINARY(os, verts);
		os.write( (char*)&m_verts[0], sizeof(Vertex)*verts);
		return os.good() ? ERR_NONE : ERR_CORRUPT_FILE;
	}

	//deserialize from an ostream
	inline error_e Deserialize(std::istream &is) {
		UINT verts;
		READ_BINARY(is, m_materialIndex);
		READ_BINARY(is, verts);
		m_verts.resize(verts);
		is.read( (char*)&m_verts[0], sizeof(Vertex)*verts);
		return is.good() ? ERR_NONE : ERR_CORRUPT_FILE;
	}

private:
	short int m_materialIndex;
	VertexList m_verts;
};

#endif