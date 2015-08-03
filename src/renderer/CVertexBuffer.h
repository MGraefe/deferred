// renderer/CVertexBuffer.h
// renderer/CVertexBuffer.h
// renderer/CVertexBuffer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__CVertexBuffer_H__
#define deferred__renderer__CVertexBuffer_H__

#include <util/basic_types.h>
#include <vector>

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING

class DLL CVertexBuffer
{
public:
	enum eBufferType
	{
		BT_STATIC_DRAW, //Data changes never
		BT_DYNAMIC_DRAW, //Data changes sometimes
		BT_STREAM_DRAW, //Data changes every frame
	};

public:
	CVertexBuffer(eBufferType usage = BT_STATIC_DRAW);
	~CVertexBuffer();
	void Init( void );
	void SetVertexes( const rendervert_t *pVerts, int iVerts );
	void AddTrianglesToList( const rendertri_t *pTri, int iTris );
	void AddVertexesToList( const rendervert_t *verts, size_t numVerts );
	void SubmitTriangleList( void );
	void Bind( void );
	void UnBind( void );
	void Render( void );

	rendervert_t *StartModification(bool bindBuffer = true);
	void EndModification(bool unbindBuffer = true);

	inline size_t GetVertexCountSubmitted() const { return m_vertexCount; }
	inline size_t GetVertexCountUnsubmitted() const { return m_vVerts.size(); }

private:
	unsigned int m_iVboID;
	bool m_bInitialized;
	size_t m_vertexCount;
	std::vector<rendervert_t> m_vVerts;
	const eBufferType m_usage;
	bool m_bModifying;
};

POP_PRAGMA_WARNINGS

#endif
