// bsptorpm/CPolygon.cpp
// bsptorpm/CPolygon.cpp
// bsptorpm/CPolygon.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "CPolygon.h"
#include <iostream>

using std::vector;
using std::cout;
using std::endl;


bool pointEqual( const Vector3d &a, const Vector3d &b )
{
	const double eps = 0.001;
	return	abs(a.x-b.x) < eps &&
			abs(a.y-b.y) < eps &&
			abs(a.z-b.z) < eps;
}

//------------------------------------------------------------------------
//Purpose: Compares vertices order independent
//------------------------------------------------------------------------
bool CPolygon::equal(const CPolygon &other) const
{
	int pointCount = points.size();
	if(pointCount != other.points.size() )
		return false;
	
	int sameVerts = 0;
	for( int i = 0; i < pointCount; i++ )
		for( int k = 0; k < pointCount; k++ )
			if(pointEqual(points[i], other.points[k]))
				sameVerts++;

	return sameVerts == pointCount;
}


//------------------------------------------------------------------------
//Purpose: Create a 4-sided, extremely large Polygon which is on a plane
//------------------------------------------------------------------------
void CPolygon::fromBasePlane(const CPlaned &plane)
{
	//Find the major axis of the plane's normal
	//int axisIndex = -1;
	//double axisMax = 0.0;
	//for( int i = 0; i < 3; i++ )
	//{
	//	double val = abs(plane.m_vNorm[i]);
	//	if(val > axisMax)
	//	{
	//		axisIndex = i;
	//		axisMax = val;
	//	}
	//}

	//AssertMsg(axisIndex != -1, "No Base Axis Found");
	//
	////Create new vector which is linearly independent to the planes normal
	//Vector3d v1Raw = vec3d_null;
	//v1Raw[(axisIndex+1)%3] = 1.0;

	////Project new vector onto plane
	//Vector3d v1 = v1Raw - plane.m_vNorm * v1Raw.Dot(plane.m_vNorm);
	Vector3d v1 = plane.m_vNorm.GetPerpendicularVector();
	v1.Normalize();

	//Create perpedicular vector
	Vector3d v2 = plane.m_vNorm.Cross(v1);
	v2.Normalize();

	//Create 4 vertices
	Vector3d planeCenter = plane.m_vNorm * -plane.m_d;
	double d = 100000.0;
	points.clear();
	points.push_back( planeCenter + v1 *  d + v2 *  d );
	points.push_back( planeCenter + v1 * -d + v2 *  d );
	points.push_back( planeCenter + v1 * -d + v2 * -d );
	points.push_back( planeCenter + v1 *  d + v2 * -d );

	normal = plane.m_vNorm;

	Assert(plane.m_vNorm.LengthSq() > 0.001);
	Assert( (points[0]-planeCenter).Cross(points[1]-planeCenter).Dot(normal) > 0.0 );
	Assert( (points[1]-planeCenter).Cross(points[2]-planeCenter).Dot(normal) > 0.0 );
	Assert( (points[2]-planeCenter).Cross(points[3]-planeCenter).Dot(normal) > 0.0 );
	Assert( (points[3]-planeCenter).Cross(points[0]-planeCenter).Dot(normal) > 0.0 );
}


//------------------------------------------------------------------------
//Purpose: Chop of a edge of this polygon, return false if the polygon
//		   is invalid after the operation
//------------------------------------------------------------------------
bool CPolygon::chopPolygon(const CPlaned &plane)
{
	vector<Vector3d> newPoints;
	const double eps = 0.00001;
	for( size_t i = 0; i < points.size(); i++ )
	{
		const Vector3d &thisVert = points[i];
		const Vector3d &nextVert = points[(i+1)%points.size()];
		double thisDist = plane.distToPointSigned(thisVert);
		double nextDist = plane.distToPointSigned(nextVert);
		bool thisOutside = thisDist > eps;
		bool nextInside = nextDist < eps;

		if( !thisOutside )
			newPoints.push_back(thisVert);

		if( thisOutside == nextInside ) //intersection
		{
			//Generate split point
			Vector3d split;
			CRayd edgeRay(thisVert, (nextVert-thisVert).Normalize());
			if( !edgeRay.intersect(plane, split) )
			{
				cout << "Warning: fp precision" << endl;
			}

			newPoints.push_back(split);
		}
	}

	points = newPoints;
	return points.size() > 2;
}

void BSPVectorToRPMVectorV2( const Vector3<real> &src, Vector3<real> &dst )
{
	//dst = src;
	dst.x = src.x;
	dst.y = -src.z;
	dst.z = -src.y;
}

//------------------------------------------------------------------------
//Purpose: triangulate the polygon, add triangles to vector triangles
//			returns the number of resulting triangles
//------------------------------------------------------------------------
int CPolygon::triangulate(vector<Triangle> &triangles) const
{
	AssertMsg(points.size() > 2, "Invalid Polygon");

	//This is particulary easy since the polygon is convex and
	//all corners are given in CCW order, so just do a triangle-fan
	size_t numTris = points.size()-2;

	for( size_t i = 0; i < numTris; i++ )
	{
		Triangle tri;
		BSPVectorToRPMVectorV2( points[0], tri.vertex3[0].pos3 );
		BSPVectorToRPMVectorV2( points[i+2], tri.vertex3[1].pos3 );
		BSPVectorToRPMVectorV2( points[i+1], tri.vertex3[2].pos3 );
		BSPVectorToRPMVectorV2( normal, tri.norm3 );
		triangles.push_back(tri);
	}

	return (int)numTris;
}

