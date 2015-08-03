// util/basicfuncs.cpp
// util/basicfuncs.cpp
// util/basicfuncs.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "basic_types.h"
#include "maths.h"
#include "rpm_file.h"


inline void swap_tri( rendertri_t &tri1, rendertri_t &tri2 )
{
	const rendertri_t buf = tri1;
	tri1 = tri2;
	tri2 = buf;
}


//simple devide & recursion quick sort algorithm
void sort_tris_by_tex_index( rendertri_t *tris, const int &left, const int &right )
{
	int pivot;
	int leftIdx = left;
	int rightIdx = right;
	if( right - left > 0 )
	{
		pivot = (left + right) / 2;
		while( leftIdx <= pivot && rightIdx >= pivot)
		{
			while( tris[leftIdx].iTexture < tris[pivot].iTexture && leftIdx <= pivot )
				leftIdx++;
			while( tris[rightIdx].iTexture > tris[pivot].iTexture && rightIdx >= pivot )
				rightIdx--;
			swap_tri( tris[leftIdx], tris[rightIdx] );
			leftIdx++;
			rightIdx--;
			if( leftIdx - 1 == pivot )
				pivot = rightIdx = rightIdx + 1;
			else if( rightIdx + 1 == pivot )
				pivot = leftIdx = leftIdx - 1;
		}
		sort_tris_by_tex_index( tris, left, pivot-1 );
		sort_tris_by_tex_index( tris, pivot+1, right );
	}
}


inline void orthonormalize_vectors( Vector3f &v1, Vector3f &v2, Vector3f &v3 )
{
	v1.Normalize();

	v2 = v2 - v1 * VectorDot(v1, v2);
	v2.Normalize();

	v3 = v3 - v2 * VectorDot(v2, v3) - v1 * VectorDot(v1, v3);
	v3.Normalize();
}


//For already normalized normal.
inline void orthonormalize_vectors( const Vector3f &v1, Vector3f &v2, Vector3f &v3 )
{
	v2 = v2 - v1 * VectorDot(v1, v2);
	v2.Normalize();

	v3 = v3 - v2 * VectorDot(v2, v3) - v1 * VectorDot(v1, v3);
	v3.Normalize();
}


inline void calc_tangent( const Vector3f &v1, const Vector3f &v2, const Vector3f &v3,
					 const Vector2f &uv1, const Vector2f &uv2, const Vector2f &uv3,
					 const Vector3f &normal, Vector3f &tangent, float &handiness )
{
	Vector3f bitangent;

	const Vector2f st1 = uv3-uv1;
	const Vector2f st2 = uv2-uv1;

	float denom = (st1.x * st2.y - st2.x * st1.y);
	float coef;
	if(denom != 0.0f)
	{
		coef = 1.0f / denom;
		tangent = ((((v3-v1) * st2.y) + ((v2-v1) * -st1.y)) * coef)/*.Normalize()*/;
		bitangent = ((((v3-v1) * st2.x) + ((v2-v1) * -st1.x)) * coef)/*.Normalize()*/;

		orthonormalize_vectors( normal, tangent, bitangent );
		handiness = VectorDot(VectorCross(tangent,bitangent),normal) < 0 ? -1.0f : 1.0f;
	}
	else
	{
		tangent = normal.GetPerpendicularVector();
		handiness = 1.0f;
	}	
}


void calc_tangent( rendertri_t *pTri )
{
	calc_tangent(	pTri->pVerts[0].vPos, pTri->pVerts[1].vPos, pTri->pVerts[2].vPos,
					pTri->pVerts[0].pUV, pTri->pVerts[1].pUV, pTri->pVerts[2].pUV, 
					pTri->pVerts[0].vNorm, pTri->pVerts[0].vTang, pTri->fHandiness );
	calc_tangent(	pTri->pVerts[1].vPos, pTri->pVerts[2].vPos, pTri->pVerts[0].vPos,
					pTri->pVerts[1].pUV, pTri->pVerts[2].pUV, pTri->pVerts[0].pUV, 
					pTri->pVerts[1].vNorm, pTri->pVerts[1].vTang, pTri->fHandiness );
	calc_tangent(	pTri->pVerts[2].vPos, pTri->pVerts[0].vPos, pTri->pVerts[1].vPos,
					pTri->pVerts[2].pUV, pTri->pVerts[0].pUV, pTri->pVerts[1].pUV, 
					pTri->pVerts[2].vNorm, pTri->pVerts[2].vTang, pTri->fHandiness );
}


void rpm_tri_to_rendertri( const rpm_triangle_t &pRPMTri, rendertri_t &pRenderTri, RTEX tex )
{
	for( int i = 0; i < 3; i++ )
	{
		pRenderTri.pVerts[i].vPos.Init( pRPMTri.vertex3[i].pos3 );
		pRenderTri.pVerts[i].vNorm.Init( pRPMTri.vertex3[i].norm3 );
		pRenderTri.pVerts[i].pUV.Init( pRPMTri.vertex3[i].uv2 );
	}
	pRenderTri.vTriNorm.Init( pRPMTri.norm3 );
	pRenderTri.iTexture = tex;

	calc_tangent( &pRenderTri );
}


//returns vertexes in the following order:
//	 2_____________6
//   /|			  /|
// 3/____________/7|
//	| |			 | |
//  | |			 | |		ISN'T THAT NICE?! ASCII ROCKS!
//  |0|_ _ _ _ _ |_|4		Front Face is 1,5,7,3
//  | /			 | /
// 1|/___________|/5
//
void GetCubeVerts( const Vector3f &vCubeMins, const Vector3f &vCubeMaxs, Vector3f * const vVerts )
{
	vVerts[0].Init( vCubeMins.x, vCubeMins.y, vCubeMins.z ); //Left Bottom Back		000
	vVerts[1].Init( vCubeMins.x, vCubeMins.y, vCubeMaxs.z ); //Left Bottom Front	001
	vVerts[2].Init( vCubeMins.x, vCubeMaxs.y, vCubeMins.z ); //Left Top Back		010
	vVerts[3].Init( vCubeMins.x, vCubeMaxs.y, vCubeMaxs.z ); //Left Top Front		011
	vVerts[4].Init( vCubeMaxs.x, vCubeMins.y, vCubeMins.z ); //Right Bottom Back	100
	vVerts[5].Init( vCubeMaxs.x, vCubeMins.y, vCubeMaxs.z ); //Right Bottom Front	101
	vVerts[6].Init( vCubeMaxs.x, vCubeMaxs.y, vCubeMins.z ); //Right Top Back		110
	vVerts[7].Init( vCubeMaxs.x, vCubeMaxs.y, vCubeMaxs.z ); //Right Top Front		111
}


//i ranges from 0 to 5.
//faces are usually referenced this way:
//	3____________2
//	|			 |
//	|	FACING	 |
//	|	 YOU!	 |
//	|			 |
//	|____________|
//	0			 1
//
geometryquad_t GetCubeQuad( const int &i, const Vector3f &vCubeMins, const Vector3f &vCubeMaxs )
{
	Vector3f vVerts[8];
	GetCubeVerts( vCubeMins, vCubeMaxs, vVerts );

	geometryquad_t q;
	if( i == 0 ) //Left quad
	{
		q.vVerts[0] = vVerts[0];
		q.vVerts[1] = vVerts[1];
		q.vVerts[2] = vVerts[3];
		q.vVerts[3] = vVerts[2];
	}
	else if( i == 1 ) //Front Quad
	{
		q.vVerts[0] = vVerts[1];
		q.vVerts[1] = vVerts[5];
		q.vVerts[2] = vVerts[7];
		q.vVerts[3] = vVerts[3];
	}
	else if( i == 2 ) //Right Quad
	{
		q.vVerts[0] = vVerts[5];
		q.vVerts[1] = vVerts[4];
		q.vVerts[2] = vVerts[6];
		q.vVerts[3] = vVerts[7];
	}
	else if( i == 3 ) //Back Quad
	{
		q.vVerts[0] = vVerts[4];
		q.vVerts[1] = vVerts[0];
		q.vVerts[2] = vVerts[2];
		q.vVerts[3] = vVerts[6];
	}
	else if( i == 4 ) //Top Quad
	{
		q.vVerts[0] = vVerts[3];
		q.vVerts[1] = vVerts[7];
		q.vVerts[2] = vVerts[6];
		q.vVerts[3] = vVerts[2];
	}
	else if( i == 5 ) //Bottom Quad
	{
		q.vVerts[0] = vVerts[0];
		q.vVerts[1] = vVerts[4];
		q.vVerts[2] = vVerts[5];
		q.vVerts[3] = vVerts[1];
	}

	//compute normal
	q.vNorm = VectorCross( q.vVerts[1]-q.vVerts[0], q.vVerts[3]-q.vVerts[0] ).Normalize();

	return q;
}



//In: c = constant, l = linear, q = quadratic, b = brightness scale
//Out: distance at which the given attentuation is reached
//This is actually just the solve for d in the attentuation formula:
// A = b / (c + l*d + q*d*d).
float CalcMaxDistPointLight( float c, float l, float q, float b, float fDesiredAtt )
{
	//pure constant lights are not supported.
	Assert( l > 0.0f || q > 0.0f );
	if( l <= 0.0f && q <= 0.0f )
		return 1.0f;
	Assert( c >= 0.0f && l >= 0.0f && q >= 0.0f && b >= 0.0f );

	float A = fDesiredAtt; //The minimum attentuation

	if( q <= 0.0f ) //Solve for the attent. formula when q == 0
		return (b - A*c) / (A*l);
	
	float l_2q = l / (2.0f*q);
	float sqrtres = sqrt( l_2q*l_2q - (A*c-b)/(A*q) );

	float d1 = -l_2q + sqrtres;
	float d2 = -l_2q - sqrtres;
	return max(d1,d2);
}


