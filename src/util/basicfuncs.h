// util/basicfuncs.h
// util/basicfuncs.h
// util/basicfuncs.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__basicfuncs_H__
#define deferred__util__basicfuncs_H__

#include "basic_types.h"
#include "rpm_file.h"
#include <string>

/*extern*/ void sort_tris_by_tex_index( rendertri_t *tris, const int &left, const int &right );

/*extern*/ inline void calc_tangent( const Vector3f &v1, const Vector3f &v2, const Vector3f &v3,
					 const Vector2f &uv1, const Vector2f &uv2, const Vector2f &uv3,
					 const Vector3f normal, Vector3f &tangent, float &handiness );

/*extern*/ void calc_tangent( rendertri_t *pTri );

/*extern*/ void rpm_tri_to_rendertri( const rpm_triangle_t &pRPMTri, rendertri_t &pRenderTri, RTEX tex );

/*extern*/ void GetCubeVerts( const Vector3f &vCubeMins, const Vector3f &vCubeMaxs, Vector3f * const vVerts );
/*extern*/ geometryquad_t GetCubeQuad( const int &i, const Vector3f &vCubeMins, const Vector3f &vCubeMaxs );

/*extern*/ 

float CalcMaxDistPointLight( float c, float l, float q, float b, float fDesiredAtt = 0.01 );

inline bool VertexInCube( const Vector3f &vVert, const Vector3f &vCubeMins, const Vector3f &vCubeMaxs )
{
	return (vVert >= vCubeMins && vVert <= vCubeMaxs);
}


#endif
