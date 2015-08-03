// util/collision.cpp
// util/collision.cpp
// util/collision.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "collision.h"
#include "rpm_file.h"


//algorithm by http://www.blackpawn.com/texts/pointinpoly/default.html
//Does not work properly right now!
bool PointInTriangleBary( Vector3f *vTriVerts, Vector3f vTriNorm, Vector3f vPoint )
{
	Vector3f v0 = vTriVerts[0];
	Vector3f v1 = vTriVerts[1] - vTriVerts[0];
	Vector3f v2 = vPoint - vTriVerts[0];

	// Compute dot products
	float dot00 = VectorDot(v0, v0);
	float dot01 = VectorDot(v0, v1);
	float dot02 = VectorDot(v0, v2);
	float dot11 = VectorDot(v1, v1);
	float dot12 = VectorDot(v1, v2);

	// Compute barycentric coordinates
	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return ( (u > 0) && (v > 0) && (u + v < 1.0f) );
}


//algorithm by http://www.blackpawn.com/texts/pointinpoly/default.html
template<typename T>
inline bool SameSide( const Vector3<T> &p1, const Vector3<T> &p2, const Vector3<T> &a, const Vector3<T> &b )
{
	return ((b-a).Cross(p1-a)).Dot((b-a).Cross(p2-a)) > 0;
}
bool PointInTriangleSS( const Vector3f *pTri, const Vector3f p )
{
    return (SameSide( p, pTri[2], pTri[0], pTri[1] ) &&
			SameSide( p, pTri[0], pTri[1], pTri[2] ) &&
			SameSide( p, pTri[1], pTri[2], pTri[0] ));
}

template<typename T>
bool PointInTriangleSS( const Vector3<T> &v0, const Vector3<T> &v1, const Vector3<T> &v2, const Vector3<T> &p )
{
    return (SameSide<T>( p, v2, v0, v1 ) &&
			SameSide<T>( p, v0, v1, v2 ) &&
			SameSide<T>( p, v1, v2, v0 ));
}


bool LineTriangleIntersect( const Vector3f &vLineStart, const Vector3f &vLineDir, const float fLength,
							const Vector3f *vTriVerts, const Vector3f &vTriNorm, Vector3f *vResult, float *fResultLength )
{
	//float dotprod = VectorDot( vLineDir, vTriNorm );

	if( VectorDot( vLineDir, vTriNorm ) > 0 ) //we are facing the back of our triangle
		return false;
	
	//Calc Distance to penetration point
	//float t = -VectorDot( vTriNorm, vLineStart-vTriVerts[0] ) / VectorDot( vTriNorm, vLineDir );
	float t = - (	vTriNorm.x * (vLineStart.x - vTriVerts->x) +
					vTriNorm.y * (vLineStart.y - vTriVerts->y) +	//more speedy version
					vTriNorm.z * (vLineStart.z - vTriVerts->z) ) /
					( vTriNorm.x*vLineDir.x + vTriNorm.y*vLineDir.y + vTriNorm.z*vLineDir.z );

	//Line started behind triangle or is too long?
	if( t < 0 || t > fLength )
		return false;

	//Point to test:
	Vector3f vPoint = vLineStart + vLineDir * t;


	// Check if point is in triangle
	if( PointInTriangleSS( vTriVerts, vPoint ) )
	{
		if( vResult )
			*vResult = vPoint;
		if( fResultLength )
			*fResultLength = t;
		return true;
	}
	else
		return false;

}

template<typename T>
bool RayTriangleIntersect( const CRay_t<T> &ray, const T fLength,
							const Vector3<T> &v0, const Vector3<T> &v1, const Vector3<T> &v2,
							const CPlane_t<T> &plane, Vector3<T> *vResult, T *fResultLength, bool doubleSided )
{
	//are we facing the back of our triangle?
	if( !doubleSided && ray.dir.Dot(plane.m_vNorm) > 0 ) 
		return false;
	
	//Calc Distance to penetration point
	T t = -1.0;
	if( !ray.intersect(plane, t) )
		return false;

	//Line started behind triangle or is too long?
	if( t < 0 || t > fLength )
		return false;

	//Point to test:
	Vector3<T> vPoint = ray.orig + ray.dir * t;

	// Check if point is in triangle
	if( PointInTriangleSS( v0, v1, v2, vPoint ) )
	{
		if( vResult )
			*vResult = vPoint;
		if( fResultLength )
			*fResultLength = t;
		return true;
	}
	else
		return false;

}

//template instantiation
template bool RayTriangleIntersect<float>( const CRayf &ray, const float fLength,
							const Vector3f &v0, const Vector3f &v1, const Vector3f &v2,
							const CPlanef &plane, Vector3f *vResult, float *fResultLength, bool doubleSided );
template bool RayTriangleIntersect<double>( const CRayd &ray, const double fLength,
							const Vector3d &v0, const Vector3d &v1, const Vector3d &v2,
							const CPlaned &plane, Vector3d *vResult, double *fResultLength, bool doubleSided );


//Do collision test for all entities in the geometry manager exept those to avoid
void BaseTrace( rpm_file_t *pObject, CTraceline *pTrace )
{
	//maybe do a check if we can reach bounding box?

	//normalizations are expensive!
	pTrace->vDir.Normalize();
	pTrace->bHit = false;
	pTrace->vEnd = pTrace->vStart + pTrace->vDir * pTrace->fMaxLength; //pre-set end point, it is overwritten in case we hit something

	float fShortestDist = pTrace->fMaxLength;

	for( UINT i = 0; i < pObject->header.num_of_triangles; i++ )
	{
		//Ineffective!
		Vector3f vTriVerts[3];
		vTriVerts[0].Init( pObject->getTriangles()[i].vertex3[0].pos3 );
		vTriVerts[1].Init( pObject->getTriangles()[i].vertex3[1].pos3 );
		vTriVerts[2].Init( pObject->getTriangles()[i].vertex3[2].pos3 );

		//vTriVerts[0] -= pTrace->vStart;
		//vTriVerts[1] -= pTrace->vStart;
		//vTriVerts[2] -= pTrace->vStart;

		Vector3f vNorm( pObject->getTriangles()[i].norm3 );

		if( LineTriangleIntersect(	pTrace->vStart,
									pTrace->vDir,
									fShortestDist,		//use shortest dist because we only want to find points that are nearer than
														//the one we already found!
									vTriVerts, 
									vNorm,
									&pTrace->vEnd,
									&pTrace->fResultDist ) )
		{
			if( pTrace->fResultDist < fShortestDist )
			{
				pTrace->bHit = true;
				pTrace->vNorm = vNorm;
				fShortestDist = pTrace->fResultDist;
			}
		}
	}
}


bool PointInQuadSS( Vector3f *pQuad, const Vector3f p )
{
    return (SameSide( p, pQuad[2], pQuad[0], pQuad[1] ) && 
			SameSide( p, pQuad[3], pQuad[1], pQuad[2] ) &&
			SameSide( p, pQuad[0], pQuad[2], pQuad[3] ) && 
			SameSide( p, pQuad[1], pQuad[3], pQuad[0]) );
}


bool LineQuadIntersect( const Vector3f vLineStart, const Vector3f vLineDir, const float fLength, Vector3f *vQuadVerts, const Vector3f vQuadNorm, Vector3f *vResult, float *fResultLength )
{
	//float dotprod = VectorDot( vLineDir, vTriNorm );

	//NOTE: Maybe remove Culling?
	if( VectorDot( vLineDir, vQuadNorm ) > 0 ) //we are facing the back of our triangle
		return false;
	
	//Calc Distance to penetration point
	//float t = -VectorDot( vTriNorm, vLineStart-vTriVerts[0] ) / VectorDot( vTriNorm, vLineDir );
	float t = - (	vQuadNorm.x * (vLineStart.x - vQuadVerts->x) +
					vQuadNorm.y * (vLineStart.y - vQuadVerts->y) +	//more speedy version
					vQuadNorm.z * (vLineStart.z - vQuadVerts->z) ) /
					( vQuadNorm.x*vLineDir.x + vQuadNorm.y*vLineDir.y + vQuadNorm.z*vLineDir.z );

	//Line started behind triangle or is too long?
	if( t < 0 || t > fLength )
		return false;

	//Point to test:
	Vector3f vPoint = vLineStart + vLineDir * t;


	// Check if point is in triangle
	if( PointInQuadSS( vQuadVerts, vPoint ) )
	{
    	if( vResult )
			*vResult = vPoint;
		if( fResultLength )
			*fResultLength = t;
		return true;
	}
	else
		return false;
}

bool LineQuadIntersect( const Vector3f vLineStart, const Vector3f vLineEnd, Vector3f *vQuadVerts, const Vector3f vQuadNorm, Vector3f *vResult, float *fResultLength )
{
	Vector3f vDir = vLineEnd - vLineStart;
	float fDist = VectorLength(vDir);
	vDir /= fDist;

	return LineQuadIntersect( vLineStart, vDir, fDist, vQuadVerts, vQuadNorm, vResult, fResultLength );
}


bool LineTriangleIntersect( const Vector3f vLineStart, Vector3f vLineEnd, Vector3f *vTriVerts, Vector3f vTriNorm, Vector3f *vResult, float *fResultLength )
{
	Vector3f vDir = vLineEnd - vLineStart;
	float fDist = VectorLength(vDir);
	vDir /= fDist;

	return LineTriangleIntersect( vLineStart, vDir, fDist, vTriVerts, vTriNorm, vResult, fResultLength );
}


//we are not interested in the distance.
bool RayCubeIntersect( const Vector3f vRayStart, const Vector3f vRayDir, const float fMaxLength, geometryquad_t *pQuad )
{
	for( int i = 0; i < 6; i++ )
	{
		if( LineQuadIntersect( vRayStart, vRayDir, fMaxLength, pQuad[i].vVerts, pQuad[i].vNorm, NULL, NULL ) )
			return true;
	}
	return false;
}

//we are not interested in the distance.
bool RayCubeIntersect( const Vector3f vRayStart, const Vector3f vRayEnd, geometryquad_t *pQuad )
{
	Vector3f vRayDir = vRayEnd - vRayStart;
	float fDist = VectorLength( vRayDir );
	if( fDist == 0.0f )
		return false;
	vRayDir *= 1.0f/fDist;

	return RayCubeIntersect( vRayStart, vRayDir, fDist, pQuad );
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
template<typename T>
bool RaySphereIntersect( const Vector3<T> &vSphereCenter, T radius,
	const Vector3<T> &vRayStart, const Vector3<T> &vRayDir,
	T distMax, T &distHit, bool bDiscardInner )
{
	T a,b,c;
	Vector3<T> vRayOrig = vRayStart - vSphereCenter;
	T fRayOrigLengthSq = vRayOrig.LengthSq();
	T radiusSq = radius*radius;

	if( bDiscardInner && fRayOrigLengthSq < radiusSq )
	{
		distHit = T(-1.0);
		return false;
	}


	a = vRayDir.Dot(vRayDir);
	b = T(2.0) * vRayDir.Dot(vRayOrig);
	c = vRayOrig.Dot(vRayOrig) - radiusSq;

	T d = b*b - T(4.0)*a*c;
	if( d < T(0.0) )
	{
		distHit = T(-1.0);
		return false;
	}

	//Solution for p = eye + view * t
	T t = ( -b - sqrt(d) ) / (T(2.0) * a);
	if( t < 0.0f )
		t = ( -b + sqrt(d) ) / (T(2.0) * a);
	
	distHit = t;
	return distHit >= T(0.0) && distHit <= distMax;
}


//explicit template instantiation for float and double
template bool RaySphereIntersect( const Vector3f &vSphereCenter, float radius,
	const Vector3f &vRayStart, const Vector3f &vRayDir,
	float distMax, float &distHit, bool bDiscardInner );
template bool RaySphereIntersect( const Vector3d &vSphereCenter, double radius,
	const Vector3d &vRayStart, const Vector3d &vRayDir,
	double distMax, double &distHit, bool bDiscardInner );


template<typename T>
bool RayAABBIntersect( const Vector3<T> &rayorig, const Vector3<T> &raydir,
	const Vector3<T> &boxmin, const Vector3<T> &boxmax, T &distHit, T distMax )
{
	T lowt = T(0.0);
	T t;
	bool hit = false;
	Vector3<T> hitpoint;
	const Vector3<T>& min = boxmin;
	const Vector3<T>& max = boxmax;

	// Check origin inside first
	if ( rayorig > min && rayorig < max )
	{
		distHit = 0;
		return true;
	}

	// Check each face in turn, only check closest 3
	// Min x
	if (rayorig.x <= min.x && raydir.x > 0)
	{
		t = (min.x - rayorig.x) / raydir.x;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max x
	if (rayorig.x >= max.x && raydir.x < 0)
	{
		t = (max.x - rayorig.x) / raydir.x;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Min y
	if (rayorig.y <= min.y && raydir.y > 0)
	{
		t = (min.y - rayorig.y) / raydir.y;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max y
	if (rayorig.y >= max.y && raydir.y < 0)
	{
		t = (max.y - rayorig.y) / raydir.y;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Min z
	if (rayorig.z <= min.z && raydir.z > 0)
	{
		t = (min.z - rayorig.z) / raydir.z;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.y >= min.y && hitpoint.y <= max.y &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max z
	if (rayorig.z >= max.z && raydir.z < 0)
	{
		t = (max.z - rayorig.z) / raydir.z;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = rayorig + raydir * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.y >= min.y && hitpoint.y <= max.y &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}

	distHit = lowt;
	return distHit < distMax && distHit >= 0.0f ? hit : false;
}

//explicit template instantiation for float and double
template bool RayAABBIntersect<float>( const Vector3f &rayorig, const Vector3f &raydir,
	const Vector3f &boxmin, const Vector3f &boxmax, float &distHit, float distMax );
template bool RayAABBIntersect<double>( const Vector3d &rayorig, const Vector3d &raydir,
	const Vector3d &boxmin, const Vector3d &boxmax, double &distHit, double distMax );


//---------------------------------------------------------------
// Purpose: raydir must be normalized!
//---------------------------------------------------------------
float RayPointDistance( const Vector3f &rayorig, const Vector3f &raydir, const Vector3f &point )
{
	return raydir.Cross(point - rayorig).Length();
}


inline bool PointInQuad2D_XZ(const Vector2f &aabbMin, const Vector2f &aabbMax, const Vector3d &p)
{
	return	p.x > aabbMin.x &&
			p.x < aabbMax.x &&
			p.z > aabbMin.y &&
			p.z < aabbMax.y;
}

bool LineQuadIntersect2D_XZ(const Vector2f &aabbMin, const Vector2f &aabbMax, const Vector3d &lineStart, const Vector3d &lineEnd)
{
	if(PointInQuad2D_XZ(aabbMin, aabbMax, lineStart) || PointInQuad2D_XZ(aabbMin, aabbMax, lineEnd))
		return true;

	//TODO: finish
	throw std::exception();
}

bool IntersectAxisAlignedQuadQuad2D(const Vector2f &min0, const Vector2f &max0, const Vector2f &min1, const Vector2f &max1)
{
	//center points
	float c0x = (min0.x + max0.x) * 0.5f;
	float c0y = (min0.y + max0.y) * 0.5f;
	float c1x = (min1.x + max1.x) * 0.5f;
	float c1y = (min1.y + max1.y) * 0.5f;

	//distances of centers (always positive)
	float dx = abs(c1x - c0x);
	float dy = abs(c1y - c0y);

	//half sizes of boxes (always positive)
	float sh0x = c0x - min0.x;
	float sh0y = c0y - min0.y;
	float sh1x = c1x - min1.x;
	float sh1y = c1y - min1.y;

	return (dx <= (sh0x + sh1x) && dy <= (sh0y + sh1y));
}


// Calculate the projection of a polygon on an axis
// and returns it as a [min, max] interval
void ProjectTriangle2D(Vector2f axis, Vector2f *triangle, float &min, float &max) {
	float dotProduct = axis.Dot(triangle[0]);
	min = dotProduct;
	max = dotProduct;
	for (int i = 0; i < 3; i++)
	{
		float d = triangle[i].Dot(axis);
		if (d < min)
			min = d;
		else if(d > max)
			max = d;
	}
}


// Calculate the distance between [minA, maxA] and [minB, maxB]
// The distance will be negative if the intervals overlap
float IntervalDistance(float minA, float maxA, float minB, float maxB)
{
	if (minA < minB)
		return minB - maxA;
	else
		return minA - maxB;
}

bool TriangleTriangleIntersect2D(const Vector2f *tri0, const Vector2f *tri1)
{
	for(int i = 0; i < 6; i++)
	{
		
	}

	//TODO: finish

	throw std::exception();
}

