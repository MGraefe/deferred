// util/collision.h
// util/collision.h
// util/collision.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once
#ifndef deferred__util__collision_H__
#define deferred__util__collision_H__

#include "maths.h"
#include "../util/rpm_file.h"
#include "basic_types.h"
#include <limits>

struct CTraceline
{
//Inputs:
	Vector3f vStart;
	Vector3f vDir;
	float fMaxLength;

//Outputs:
	Vector3f vEnd;
	Vector3f vNorm;
	bool bHit;
	float fResultDist;

};

extern void BaseTrace( rpm_file_t *pObject, CTraceline *pTrace );

extern bool LineTriangleIntersect( const Vector3f &vLineStart, const Vector3f &vLineDir, const float fLength,
									const Vector3f *vTriVerts, const Vector3f &vTriNorm, Vector3f *vResult, float *fResultLength );

extern bool LineTriangleIntersect( const Vector3f vLineStart, Vector3f vLineEnd, Vector3f *vTriVerts, Vector3f vTriNorm,
									Vector3f *vResult, float *fResultLength );

template<typename T>
extern bool RayTriangleIntersect( const CRay_t<T> &ray, const T fLength,
							const Vector3<T> &v0, const Vector3<T> &v1, const Vector3<T> &v2,
							const CPlane_t<T> &plane, Vector3<T> *vResult, T *fResultLength, bool doubleSided );

extern bool LineQuadIntersect( const Vector3f vLineStart, const Vector3f vLineDir, const float fLength, Vector3f *vQuadVerts,
									const Vector3f vQuadNorm, Vector3f *vResult, float *fResultLength );

extern bool LineQuadIntersect( const Vector3f vLineStart, const Vector3f vLineEnd, Vector3f *vQuadVerts, const Vector3f vQuadNorm,
									Vector3f *vResult, float *fResultLength );

extern bool RayCubeIntersect( const Vector3f vRayStart, const Vector3f vRayDir, const float fMaxLength, geometryquad_t *pQuad );

extern bool RayCubeIntersect( const Vector3f vRayStart, const Vector3f vRayEnd, geometryquad_t *pQuad);

template<typename T>
bool RaySphereIntersect( const Vector3<T> &vSphereCenter, T radius,
	const Vector3<T> &vRayStart, const Vector3<T> &vRayDir,
	T distMax, T &distHit, bool bDiscardInner = true );

template<typename T>
bool RayAABBIntersect( const Vector3<T> &rayorig, const Vector3<T> &raydir,
					  const Vector3<T> &boxmin, const Vector3<T> &boxmax, T &distHit, T distMax = std::numeric_limits<T>::max() );

extern float RayPointDistance( const Vector3f &rayorig, const Vector3f &raydir, const Vector3f &point );

extern bool IntersectAxisAlignedQuadQuad2D(const Vector2f &min0, const Vector2f &max0, const Vector2f &min1, const Vector2f &max1);

#endif
