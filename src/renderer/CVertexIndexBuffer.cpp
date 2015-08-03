// renderer/CVertexIndexBuffer.cpp
// renderer/CVertexIndexBuffer.cpp
// renderer/CVertexIndexBuffer.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "glheaders.h"
#include "CVertexIndexBuffer.h"
#include <util/error.h>
#include "CVertexBuffer.h"

template<typename T>
CVertexIndexBuffer<T>::CVertexIndexBuffer()
{
	m_bInitialized = false;
	m_iIndexes = -1;
}

template<typename T>
CVertexIndexBuffer<T>::~CVertexIndexBuffer()
{
	if( m_bInitialized )
		glDeleteBuffers( 1, &m_iIboID );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
template<typename T>
void CVertexIndexBuffer<T>::Init( void )
{
	Assert( !m_bInitialized );

	glGenBuffers( 1, &m_iIboID );

	//m_pVertexBuffer = pVertexBuffer;
	m_bInitialized = true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
template<typename T>
void CVertexIndexBuffer<T>::AddIndexToList( T index )
{
	m_vIndexList.push_back( index );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
template<typename T>
void CVertexIndexBuffer<T>::AddIndexRangeToList( T start, T end )
{
	Assert( start < end );

	const size_t count = end-start+1;

	m_vIndexList.reserve( m_vIndexList.size() + count );

	for( T i = start; i <= end; i++ )
	{
		m_vIndexList.push_back( i );
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
template<typename T>
void CVertexIndexBuffer<T>::SubmitIndexList( bool bKeepIndexList )
{
	this->SetVertexIndexes( m_vIndexList.data(), m_vIndexList.size() );

	if( !bKeepIndexList )
	{
		m_vIndexList.clear();
		m_vIndexList.shrink_to_fit();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
template<typename T>
void CVertexIndexBuffer<T>::SetVertexIndexes( const T *pIndexes, int iIndexes )
{
	Assert( m_bInitialized );

	Bind();

	glBufferData(	GL_ELEMENT_ARRAY_BUFFER,	//Target
					sizeof(T)*iIndexes,			//Buffer Size
					pIndexes,					//Data
					GL_STATIC_DRAW );			//Usage

	m_iIndexes = iIndexes;

	UnBind();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
template<typename T>
void CVertexIndexBuffer<T>::Bind( void )
{
	Assert( m_bInitialized );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iIboID );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
template<typename T>
void CVertexIndexBuffer<T>::UnBind( void )
{
	Assert( m_bInitialized );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
template<typename T>
void CVertexIndexBuffer<T>::Render( void )
{
	if( m_iIndexes > 2 )
		glDrawElements( GL_TRIANGLES, m_iIndexes, GetSizeType(), BUFFER_OFFSET(0) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
template<typename T>
void CVertexIndexBuffer<T>::AddTriangleToList( T index )
{
	T index3 = index*3;
	m_vIndexList.push_back( index3 );
	m_vIndexList.push_back( index3+1 );
	m_vIndexList.push_back( index3+2 );
}


template class CVertexIndexBuffer<USHORT>;
template class CVertexIndexBuffer<UINT>;

