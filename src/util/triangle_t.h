// util/triangle_t.h
// util/triangle_t.h
// util/triangle_t.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__triangle_t_H__
#define deferred__util__triangle_t_H__


#include "maths.h"
typedef double real; //TODO: get rid of this

template<typename T>
struct Vertex_t
{
	template<typename U>
	void fromOther(const Vertex_t<U> &v) {
		pos3 = v.pos3.template getOther<T>();
		norm3 = v.norm3.template getOther<T>();
		uv2[0] = (T)v.uv2[0];
		uv2[1] = (T)v.uv2[1];
	}

	Vector3<T> pos3;
	Vector3<T> norm3;
	T uv2[2];
	T skyVisibility;
};

typedef Vertex_t<real> Vertex;

template<typename T>
struct Triangle_t
{
	Triangle_t()
		: iTexID(-1), iDispVert(0) { }

	template<typename U>
	void fromOther(const Triangle_t<U> &tri) {
		iDispVert = tri.iDispVert;
		iTexID = tri.iTexID;
		norm3 = tri.norm3.template getOther<T>();
		for( int i = 0; i < 3; i++ )
			vertex3[i].fromOther<U>(tri.vertex3[i]);
	}

	Vertex_t<T> vertex3[3];
	Vector3<T> norm3;
	int iTexID;
	int iDispVert;
	double skyVisibility;
};

typedef Triangle_t<real> Triangle;


#endif
