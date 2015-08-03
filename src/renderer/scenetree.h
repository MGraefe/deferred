// renderer/scenetree.h
// renderer/scenetree.h
// renderer/scenetree.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__scenetree_H__
#define deferred__renderer__scenetree_H__

#include <util/maths.h>
#include <util/basic_types.h>
#include <map>

#define MAX_TRIANGLES_PER_CUBE 64000
//#define USE_DRAW_CALL_REDUCTION

template<typename T> class CVertexIndexBuffer;
typedef std::map< RTEX, CVertexIndexBuffer<USHORT>* > IndexBufferMap;
typedef IndexBufferMap::iterator IndexBufferMapIterator;
typedef std::pair<int, CVertexIndexBuffer<USHORT>*> IndexBufferPair;

//Basic Octree Knot.
//All Knots are actually Cubes.
//If a Cube has Child-Cubes it is a Node and does not contain a Display List.
//If a Cube does not have Child-Cubes it is a Leaf and contains a Display List.
//Real Triangle Coords are not stored inside a cube.

//Addition in version 2:
//Every Tree Leaf has multiple CVertexIndexBuffers (IBOs).
//These IBOs are stored inside a map, the key value is actually the texture-index
//all triangles/vertexes/indexes belong to.

class CSTreeCube
{
public:
	CSTreeCube();
	~CSTreeCube();

	Vector3f m_vCubeBounds[2]; //1: Left-Bottom-Back-Corner, 2: Right-Top-Front-Corner.

	rendertri_t *m_pAllTris; //pointer to a array of all triangles (the same on all cubes of one tree)
	int m_iAllTris; //number of all triangles in the m_pAllTris Array

	bool m_bVisible; //This Variable is updated each frame, just before rendering

	///////Node Characteristics:
	int m_iChildCubes;			//iChildKnots == 0 means we are a leaf, which means there must be triangles
	CSTreeCube **m_pChildCubes;	//Maybe do this as a standard array? Might be data waste but grants the cache some speed

	//An Index-Buffer which contains the triangles belonging to this node
	IndexBufferMap m_IndexMap;
	CVertexIndexBuffer<USHORT> *m_pIBOAllTris;

	int m_iTris; //Number of Triangle References
	int *m_pTris; //Triangle References

	int m_iVisibleTris;

	//int iEntities;			//Number of Entities that are in this Cube or are touching this Cube.
	//int *pEntities;			//Indexes in the global Entity List
	//NOTE: Too Difficult atm, so not supported.
	//But: Tracing and Rendering moving Geometry should be cheap because:
	//	1. havent got that much objects
	//	2. we can quickly trace/frustrum-cull the bounding box.
	//Putting entitys into the tree would be very complicated because we must know which
	//Cubes the Entity is touching AND we need to maintain cubes that do not contain world geometry.

public:
	bool IsNode( void ) { return m_iChildCubes != 0; }
	bool IsLeaf( void ) { return m_iChildCubes == 0; }

	bool IsVisible(void) { return m_bVisible; }
	void SetVisible(bool visible);

	//returns the number of visible triangles
	int UpdateVisibility( void );
	void SplitCube( void );

	void CreateIndexBuffers( void );

	void Render( int texindex, std::vector<USHORT> &indexes );

	int GetNumVisibleTris( int texindex );

	void DebugRender( void );
};

extern int g_DrawnCubes;

#endif
