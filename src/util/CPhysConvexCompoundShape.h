// util/CPhysConvexCompoundShape.h
// util/CPhysConvexCompoundShape.h
// util/CPhysConvexCompoundShape.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__CPhysConvexCompoundShape_H__
#define deferred__util__CPhysConvexCompoundShape_H__

#include "maths.h"
#include "basic_types.h"

class CPhysConvexHull;

// Represents a compound-shape, a set of convex-hulls which form a single object
// This way, objects can be "concave"
class CPhysConvexCompoundShape
{
public:
	typedef std::vector<CPhysConvexHull*> ShapeList;
	typedef std::map<std::string, int, LessThanCppStringObj> MaterialIndexMap;
	const static int max_shapes;

	CPhysConvexCompoundShape();
	~CPhysConvexCompoundShape();

	//Get the list of all shapes
	inline const ShapeList &GetShapeList(void) const { return m_shapes; }

	//Get a single shape by id
	inline const CPhysConvexHull &GetShape(UINT i) const { Assert(i < m_shapes.size()); return *m_shapes[i]; }

	//Get number of children
	inline UINT GetSize(void) const { return m_shapes.size(); }

	//Get Axis-aligned bounding box
	inline const AABoundingBox &GetAABB(void) const { return m_aabb; }

	//Set Axis-aligned bounding box
	inline void SetAABB(const AABoundingBox &box) { m_aabb = box; m_aabbValid = true; }

	//Is the AABB valid?
	inline bool IsAABBValid(void) const { return m_aabbValid; }

	//Calculate AABB from children
	void CalculateAABB(void);

	//Add a shape to this compound shape
	//Warning: convexHull will be deleted if this compound-shape is deleted
	//Warning: the material-index of the shape is overwritten by this function
	void AddShape(CPhysConvexHull *shape, const std::string &material);

	//serialize to an ostream
	error_e Serialize(std::ostream &os) const;

	//deserialize from an istream
	error_e Deserialize(std::istream &is);

	const MaterialIndexMap &GetMaterialIndexMap() const { return m_materialIndexMap; }

	//Get a material name by its id, returns empty string if not found
	std::string GetMaterialNameById(int id) const;

private:
	AABoundingBox m_aabb;
	bool m_aabbValid;
	ShapeList m_shapes;

	MaterialIndexMap m_materialIndexMap;
	int m_materialIndexCounter;
};


#endif