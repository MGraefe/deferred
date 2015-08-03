// renderer/CVertexIndexBuffer.h
// renderer/CVertexIndexBuffer.h
// renderer/CVertexIndexBuffer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__CVertexIndexBuffer_H__
#define deferred__renderer__CVertexIndexBuffer_H__

#include <util/basic_types.h>
#include <vector>
//class CVertexBuffer;

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING

template<typename T>
class DLL CVertexIndexBuffer
{
public:
	CVertexIndexBuffer();
	~CVertexIndexBuffer();

	void SetVertexIndexes( const T *pIndexes, int iIndexes );
	void AddIndexToList( T index );
	void AddTriangleToList( T index );
	void AddIndexRangeToList( T start, T end );
	void SubmitIndexList( bool bKeepIndexList = false );
	const std::vector<T> &GetIndexList( void ) { return m_vIndexList; }

	void Init( void );
	void Bind( void );
	void UnBind( void );
	void Render( void );

private:
	inline GLenum GetSizeType() const;

	bool m_bInitialized;
	UINT m_iIboID;
	UINT m_iIndexes;
	//CVertexBuffer *m_pVertexBuffer;

	std::vector<T> m_vIndexList;
};

template<>
inline GLenum CVertexIndexBuffer<UINT>::GetSizeType() const { return GL_UNSIGNED_INT; }
template<>
inline GLenum CVertexIndexBuffer<USHORT>::GetSizeType() const { return GL_UNSIGNED_SHORT; }

POP_PRAGMA_WARNINGS

#endif
