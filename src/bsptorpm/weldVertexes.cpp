// bsptorpm/weldVertexes.cpp
// bsptorpm/weldVertexes.cpp
// bsptorpm/weldVertexes.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include <map>
#include <vector>
#include <iostream>
#include <util/triangle_t.h>
#include <util/maths.h>
#include "bsptorpm.h"

using std::map;
using std::vector;
using std::cout;
using std::endl;

//------------------------------------------------------------------------
// Purpose: Sort vertexes first by x, then by y and then by z coordinate,
//			returning false if vertexes are equal regarding to a small offset
//			named epsilon
//------------------------------------------------------------------------
class CWeldVertexComparatorLess
{
public:
	CWeldVertexComparatorLess(real epsilon) : eps(epsilon) {}
	bool operator() (	const Vector3<real> &left,
						const Vector3<real> &right ) const
	{
		if( abs(left.x - right.x) > eps )
			return left.x < right.x;
		if( abs(left.y - right.y) > eps )
			return left.y < right.y;
		if( abs(left.z - right.z) > eps )
			return left.z < right.z;
		return false;
	}

private:
	const real eps;
};

typedef map<Vector3<real>, vector<Vector3<real>*>, CWeldVertexComparatorLess> VertexMap;

//------------------------------------------------------------------------
// Purpose: remove very small differences in vertex-positions which cause
//			flickering along triangle-edges
//------------------------------------------------------------------------
void weldVertexes(vector<Triangle> &triangles)
{
	//generate a map
	VertexMap vertexMap(CWeldVertexComparatorLess(0.001));
	
	//fill vertexes into map, collecting vertexes which compare equally
	//according to the comparator
	for( size_t i = 0; i < triangles.size(); ++i )
	{
		for( size_t k = 0; k < 3; ++k )
		{
			Vector3<real> *vert = &triangles[i].vertex3[k].pos3;
			vertexMap[*vert].push_back(vert);
		}
	}

	//calculate average of all vertexes which compared equally according to
	//the comparator and assin the average to each one of them
	VertexMap::const_iterator it = vertexMap.begin();
	VertexMap::const_iterator itEnd = vertexMap.end();
	for( ; it != itEnd; ++it )
	{
		const vector<Vector3<real>*> &verts = it->second;
		Vector3<real> sum(real(0.0));
		for( size_t k = 0; k < verts.size(); ++k )
			sum += *verts[k];
		sum /= (real)verts.size(); //verts can never be empty, so this is safe
		for( size_t k = 0; k < verts.size(); ++k )
			*verts[k] = sum;
	}

	cout << "\tWelded " << triangles.size()*3 << " to " << vertexMap.size() << " vertex-positions." << endl;
}
