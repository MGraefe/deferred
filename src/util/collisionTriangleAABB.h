// util/collisionTriangleAABB.h
// util/collisionTriangleAABB.h
// util/collisionTriangleAABB.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//collisionTriangleAABB.h

#pragma once
#ifndef deferred__util__collisionTriangleAABB_H__
#define deferred__util__collisionTriangleAABB_H__

template<typename T>
bool triangle_aabb_inters_t( Vector3<T> *pTriVerts, const Vector3<T> &vCubeMins, const Vector3<T> &vCubeMaxs );

template<typename T>
bool triangle_aabb_inters_t( const Vector3<T> &v0, const Vector3<T> &v1, const Vector3<T> &v2, const Vector3<T> &vCubeMins, const Vector3<T> &vCubeMaxs );

//default name
#define triangle_aabb_inters triangle_aabb_inters_t<float> 

bool triangle_aabb_inters_2d_xz(const Vector2f &aabbMin, const Vector2f &aabbMax,
								const Vector3f &triVertex1, const Vector3f &triVertex2, const Vector3f &triVertex3);

#endif // deferred__util__collisionTriangleAABB_H__

