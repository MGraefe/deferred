// renderer/scenetree.cpp
// renderer/scenetree.cpp
// renderer/scenetree.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


//Der Scenetree sollte nicht wirklich rendern.

#include "scenetree.h"
#include <util/basicfuncs.h>
#include <util/collision.h>
#include <vector>
#include <util/error.h>
#include "glheaders.h"
#include "CViewFrustum.h"
//#include "clientmain.h"
#include <util/globalvars.h>
#include "CRenderInterf.h"
#include "CShaders.h"
#include <util/debug.h>
#include "CCamera.h"
#include "renderer.h"
#include "glfunctions.h"
#include "CVertexIndexBuffer.h"
#include "CLoadingMutex.h"
#include <util/collisionTriangleAABB.h>

void CalcCubeCoords( CSTreeCube **pCubes, const Vector3f vCubeMin, const Vector3f vCubeMax )
{
	Vector3f vCubeMid = vCubeMin + (vCubeMax-vCubeMin) * 0.5f;
	pCubes[0]->m_vCubeBounds[0].Init( vCubeMin.x, vCubeMin.y, vCubeMin.z ); //000
	pCubes[0]->m_vCubeBounds[1].Init( vCubeMid.x, vCubeMid.y, vCubeMid.z );

	pCubes[1]->m_vCubeBounds[0].Init( vCubeMin.x, vCubeMin.y, vCubeMid.z ); //001
	pCubes[1]->m_vCubeBounds[1].Init( vCubeMid.x, vCubeMid.y, vCubeMax.z ); 

	pCubes[2]->m_vCubeBounds[0].Init( vCubeMin.x, vCubeMid.y, vCubeMin.z ); //010
	pCubes[2]->m_vCubeBounds[1].Init( vCubeMid.x, vCubeMax.y, vCubeMid.z ); 

	pCubes[3]->m_vCubeBounds[0].Init( vCubeMin.x, vCubeMid.y, vCubeMid.z ); //011
	pCubes[3]->m_vCubeBounds[1].Init( vCubeMid.x, vCubeMax.y, vCubeMax.z ); 

	pCubes[4]->m_vCubeBounds[0].Init( vCubeMid.x, vCubeMin.y, vCubeMin.z ); //100
	pCubes[4]->m_vCubeBounds[1].Init( vCubeMax.x, vCubeMid.y, vCubeMid.z ); 

	pCubes[5]->m_vCubeBounds[0].Init( vCubeMid.x, vCubeMin.y, vCubeMid.z ); //101
	pCubes[5]->m_vCubeBounds[1].Init( vCubeMax.x, vCubeMid.y, vCubeMax.z );

	pCubes[6]->m_vCubeBounds[0].Init( vCubeMid.x, vCubeMid.y, vCubeMin.z ); //110
	pCubes[6]->m_vCubeBounds[1].Init( vCubeMax.x, vCubeMax.y, vCubeMid.z );

	pCubes[7]->m_vCubeBounds[0].Init( vCubeMid.x, vCubeMid.y, vCubeMid.z ); //111
	pCubes[7]->m_vCubeBounds[1].Init( vCubeMax.x, vCubeMax.y, vCubeMax.z );
}



bool TriangleInCube( Vector3f *pTriVerts, const Vector3f vTriNorm, const Vector3f vCubeMins, const Vector3f vCubeMaxs )
{
#if 1
	//CASE 1: Check for Vertexes inside cube
	if( VertexInCube( pTriVerts[0], vCubeMins, vCubeMaxs ) ||
		VertexInCube( pTriVerts[1], vCubeMins, vCubeMaxs ) ||
		VertexInCube( pTriVerts[2], vCubeMins, vCubeMaxs ) )
		return true;
#endif

#if 1
	//CASE 2: Check for Triangle-Edges Intersecting Cube Sides by Tracing Triangle-Edges against Cube-Sides
	for( int i = 0; i < 6; i++ )
	{
		geometryquad_t quad = GetCubeQuad( i, vCubeMins, vCubeMaxs );
		if( LineQuadIntersect( pTriVerts[0], pTriVerts[1], quad.vVerts, quad.vNorm, NULL, NULL ) ||
			LineQuadIntersect( pTriVerts[1], pTriVerts[2], quad.vVerts, quad.vNorm, NULL, NULL ) ||
			LineQuadIntersect( pTriVerts[2], pTriVerts[0], quad.vVerts, quad.vNorm, NULL, NULL ) )
			return true;
	}
#endif

#if 1
	//CASE 3: Check for Cube-Corners Intersecting the Triangle by tracing.
	Vector3f vCubeVerts[8];
	GetCubeVerts( vCubeMins, vCubeMaxs, vCubeVerts );
	if( LineTriangleIntersect( vCubeVerts[0], vCubeVerts[2], pTriVerts, vTriNorm, NULL, NULL ) || //Left Back
		LineTriangleIntersect( vCubeVerts[2], vCubeVerts[3], pTriVerts, vTriNorm, NULL, NULL ) || //Left Top
		LineTriangleIntersect( vCubeVerts[3], vCubeVerts[1], pTriVerts, vTriNorm, NULL, NULL ) || //Left Front
		LineTriangleIntersect( vCubeVerts[1], vCubeVerts[0], pTriVerts, vTriNorm, NULL, NULL ) || //Left Bottom
		LineTriangleIntersect( vCubeVerts[4], vCubeVerts[5], pTriVerts, vTriNorm, NULL, NULL ) || //Right Bottom
		LineTriangleIntersect( vCubeVerts[5], vCubeVerts[7], pTriVerts, vTriNorm, NULL, NULL ) || //Right Front
		LineTriangleIntersect( vCubeVerts[7], vCubeVerts[6], pTriVerts, vTriNorm, NULL, NULL ) || //Right Top
		LineTriangleIntersect( vCubeVerts[6], vCubeVerts[4], pTriVerts, vTriNorm, NULL, NULL ) || //Right Back
		LineTriangleIntersect( vCubeVerts[0], vCubeVerts[4], pTriVerts, vTriNorm, NULL, NULL ) || //Bottom Back
		LineTriangleIntersect( vCubeVerts[1], vCubeVerts[5], pTriVerts, vTriNorm, NULL, NULL ) || //Bottom Front
		LineTriangleIntersect( vCubeVerts[2], vCubeVerts[6], pTriVerts, vTriNorm, NULL, NULL ) || //Top Back
		LineTriangleIntersect( vCubeVerts[3], vCubeVerts[7], pTriVerts, vTriNorm, NULL, NULL )  ) //Top Front
		return true;
#endif
	return false;
}


CSTreeCube::CSTreeCube()
{
	this->m_pIBOAllTris = NULL;
	this->m_iChildCubes = 0;
	this->m_iTris = -1;
	this->m_pChildCubes = NULL;
	this->m_pTris = NULL;
	this->m_iVisibleTris = 0;
	this->m_bVisible = false;
}


CSTreeCube::~CSTreeCube()
{
	{
		if( m_pIBOAllTris )
			delete m_pIBOAllTris;

		//delete all index buffers (if any)
		IndexBufferMap::iterator it = this->m_IndexMap.begin();
		for( ; it != m_IndexMap.end(); ++it )
			delete it->second;
	
		if( m_pTris )
			delete m_pTris;
	}
	//delete sub-cubes (if any)
	for( int i = 0; i < this->m_iChildCubes; i++ )
		delete m_pChildCubes[i];
	delete[] m_pChildCubes;
}

#define USE_NEW_TRI_AABB_INTERS

void CSTreeCube::SplitCube( void )
{
	{
		CSTreeCube **pNewCubes = new CSTreeCube*[8]; //List of Pointers
		for( int i = 0; i < 8; i++ ) //Cubes;
			pNewCubes[i] = new CSTreeCube;

		CalcCubeCoords( pNewCubes, m_vCubeBounds[0], m_vCubeBounds[1] );
		int iValidSubCubes = 0;

		for( int i = 0; i < 8; i++ )
		{
			CSTreeCube *pSubCube = pNewCubes[i];
			std::vector<int> pvTrisInCube;

			//Check for triangles in cube:
			//Iterate through all Triangles of the sub-cube's parent (this object actually)
			for( int k = 0; k < this->m_iTris; k++ )
			{
				int iTriIndex = m_pTris[k]; //Triangle index in m_pAllTris

				Vector3f pvVerts[3];
				pvVerts[0] = m_pAllTris[iTriIndex].pVerts[0].vPos;
				pvVerts[1] = m_pAllTris[iTriIndex].pVerts[1].vPos;
				pvVerts[2] = m_pAllTris[iTriIndex].pVerts[2].vPos;

#ifdef USE_NEW_TRI_AABB_INTERS
				if( triangle_aabb_inters( pvVerts, pSubCube->m_vCubeBounds[0], pSubCube->m_vCubeBounds[1] ) )
#else
				if( TriangleInCube( pvVerts, m_pAllTris[iTriIndex].vTriNorm, pSubCube->m_vCubeBounds[0], pSubCube->m_vCubeBounds[1] ) )
#endif
					pvTrisInCube.push_back( iTriIndex );
			}

			//Ok now we got all triangles that are inside our sub-cube.
			//Add them to the cube, since we only have em in a local list
			pSubCube->m_iTris = pvTrisInCube.size();
			if( pSubCube->m_iTris > 0 )
				pSubCube->m_pTris = new int[pSubCube->m_iTris]; //possible memory leak here.
			for( int k = 0; k < pSubCube->m_iTris; k++ )
			{
				pSubCube->m_pTris[k] = pvTrisInCube[k];
			}

			//simply propagate global indexes and pointers
			pSubCube->m_pAllTris = m_pAllTris;
			pSubCube->m_iAllTris = m_iAllTris;

			if( pSubCube->m_iTris > 0 )
				iValidSubCubes++;
		}

		//ok now we have a valid list of sub-cubes.
		//add them as children if they contain any triangles.
		if( iValidSubCubes == 0 )
			error( "No valid sub-cubes inside valid parent-cube" );

		this->m_pChildCubes = new CSTreeCube*[iValidSubCubes];
		this->m_iChildCubes = iValidSubCubes;
		//iterate through all sub-cubes and add the ones that are valid

		int k = 0;
		for( int i = 0; i < 8; i++ )
		{
			if( pNewCubes[i]->m_iTris > 0 )
			{
				this->m_pChildCubes[k] = pNewCubes[i];
				k++;
			}
			else
			{
				delete pNewCubes[i];
			}
		}

		//delete list of pointers
		delete[] pNewCubes;
	}

	//ok everything valid added.
	//now we can go into recursion if needed.
	for( int i = 0; i < this->m_iChildCubes; i++ )
	{
		if( m_pChildCubes[i]->m_iTris > MAX_TRIANGLES_PER_CUBE )
		{
			m_pChildCubes[i]->SplitCube();
		}
		else
		{
			m_pChildCubes[i]->CreateIndexBuffers();
		}

		delete[] m_pChildCubes[i]->m_pTris;
		m_pChildCubes[i]->m_pTris = NULL;
	}
}

int g_iGlobalIndexCount = 0;


//Creates IBOs of triangles with the same texture index and inserts them into a map.
void CSTreeCube::CreateIndexBuffers( void )
{
	Assert( m_iTris > 0 );
	g_LoadingMutex->SetOrWait();
	//the triangle list of this cube should be already sorted by texture indexes.
	//so here we go:

	int iLastIndex = -515422;
	CVertexIndexBuffer<USHORT> *pLastBuffer = NULL;

	m_pIBOAllTris = new CVertexIndexBuffer<USHORT>();
	m_pIBOAllTris->Init();

	for( int i = 0; i < m_iTris; i++ )
	{
		const int iTexIndex = m_pAllTris[m_pTris[i]].iTexture;
		if( iTexIndex != iLastIndex )
		{
			//submit the indexes of the last buffer
			if( pLastBuffer )
				pLastBuffer->SubmitIndexList(true);

			//Create a new index-buffer
			CVertexIndexBuffer<USHORT> *pBuffer = new CVertexIndexBuffer<USHORT>();
			pBuffer->Init();
			pBuffer->AddIndexToList( m_pTris[i]*3 );
			pBuffer->AddIndexToList( m_pTris[i]*3+1 );
			pBuffer->AddIndexToList( m_pTris[i]*3+2 );
			g_iGlobalIndexCount += 3;

			//add the new buffer to the map
			m_IndexMap.insert( IndexBufferPair( iTexIndex, pBuffer ) );

			pLastBuffer = pBuffer;
			iLastIndex = iTexIndex;
		}
		else
		{
			//just add the new triangle to the existing buffer
			pLastBuffer->AddIndexToList( m_pTris[i]*3 );
			pLastBuffer->AddIndexToList( m_pTris[i]*3+1 );
			pLastBuffer->AddIndexToList( m_pTris[i]*3+2 );
		}

		//Add the triangle also to our all-tris index buffer,
		//mainly to reduce draw-calls for shadow mapping
		m_pIBOAllTris->AddIndexToList( m_pTris[i]*3 );
		m_pIBOAllTris->AddIndexToList( m_pTris[i]*3+1 );
		m_pIBOAllTris->AddIndexToList( m_pTris[i]*3+2 );
	}

	//Submit data of the last used index list.
	if( pLastBuffer )
		pLastBuffer->SubmitIndexList(true);

	m_pIBOAllTris->SubmitIndexList(true);

	g_LoadingMutex->Release();
}


void CSTreeCube::DebugRender( void )
{
	//Vector3f vCenter = m_vCubeBounds[0] + (m_vCubeBounds[1]-m_vCubeBounds[0])*0.5f;
	//float fWidth = m_vCubeBounds[1].x - m_vCubeBounds[0].x;
	//Debug::DrawCube( vCenter, fWidth, Color(50,255,50), 1.0f );

	for( int i = 0; i < m_iChildCubes; i++ )
	{
		m_pChildCubes[i]->DebugRender();
	}
}


int g_DrawnCubes = 0;
void CSTreeCube::Render( int texindex, std::vector<USHORT> &indexes )
{
	if( !IsVisible() )
		return;

	if( IsLeaf() )
	{
		if( texindex == 0 ) //Shadow pass
		{
#ifdef USE_DRAW_CALL_REDUCTION
			const std::vector<USHORT> &newInds = m_pIBOAllTris->GetIndexList();
			indexes.insert(indexes.end(), newInds.begin(), newInds.end());
#else
			m_pIBOAllTris->Bind();
			GetGLStateSaver()->EnableClientState(GL_INDEX_ARRAY);
			m_pIBOAllTris->Render();
#endif
		}
		else
		{
			IndexBufferMapIterator it = m_IndexMap.find(texindex);
			if( it != m_IndexMap.end() )
			{
				CVertexIndexBuffer<USHORT> * const pIndBuf = it->second;
#ifdef USE_DRAW_CALL_REDUCTION
				const std::vector<USHORT> &newInds = pIndBuf->GetIndexList();
				indexes.insert(indexes.end(), newInds.begin(), newInds.end());
#else
				pIndBuf->Bind();
				GetGLStateSaver()->EnableClientState(GL_INDEX_ARRAY);
				pIndBuf->Render();
#endif
			}
		}
		g_DrawnCubes++;
	}
	else
	{
		for( int i = 0; i < m_iChildCubes; i++ )
			m_pChildCubes[i]->Render(texindex, indexes);
	}
}


int CSTreeCube::UpdateVisibility( void )
{
	{
		CViewFrustum *pFrustum = g_RenderInterf->GetRenderer()->GetActiveFrustum();
		m_bVisible = g_bNoFrustumCull || pFrustum->AxisAlignedCubeInFrustum( m_vCubeBounds[0], m_vCubeBounds[1] );
	}

	m_iVisibleTris = 0;

	if( IsVisible() )
	{
		if( IsNode() )
		{
			for( int i = 0; i < m_iChildCubes; i++ )
				m_iVisibleTris += m_pChildCubes[i]->UpdateVisibility();
		}
		else
		{
			m_iVisibleTris = m_iTris;
		}
	}
	//else
	//	SetVisible(false);

	return m_iVisibleTris;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CSTreeCube::GetNumVisibleTris( int texindex )
{
	if( !IsVisible() )
		return 0;

	if( texindex == 0)
		return m_iVisibleTris;
	
	int triCount = 0;
	if( IsLeaf() )
	{
		IndexBufferMapIterator it = m_IndexMap.find(texindex);
		if( it != m_IndexMap.end() )
			triCount = it->second->GetIndexList().size()/3;
	}
	else
	{
		for( int i = 0; i < m_iChildCubes; i++ )
			triCount += m_pChildCubes[i]->GetNumVisibleTris(texindex);
	}

	return triCount;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CSTreeCube::SetVisible( bool visible )
{
	m_bVisible = visible;

	for( int i = 0; i < m_iChildCubes; i++ )
		m_pChildCubes[i]->SetVisible(visible);
}

