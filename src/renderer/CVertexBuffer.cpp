// renderer/CVertexBuffer.cpp
// renderer/CVertexBuffer.cpp
// renderer/CVertexBuffer.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "glheaders.h"
#include "CVertexBuffer.h"
#include <util/error.h>

CVertexBuffer::CVertexBuffer(eBufferType usage /*= BT_STATIC_DRAW*/) :
	m_usage(usage)
{
	m_bInitialized = false;
	m_vertexCount = 0;
	m_bModifying = false;
}

CVertexBuffer::~CVertexBuffer()
{
	if( m_bInitialized )
		glDeleteBuffers( 1, &m_iVboID );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CVertexBuffer::Init( void )
{
	Assert( !m_bInitialized );

	glGenBuffers( 1, &m_iVboID );

	m_bInitialized = true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CVertexBuffer::SetVertexes( const rendervert_t *pVerts, int iVerts )
{
	Assert( m_bInitialized );
	Assert( !m_bModifying );

	Bind();

	GLenum usage = GL_STATIC_DRAW;
	switch(m_usage)
	{
		case BT_STATIC_DRAW: usage = GL_STATIC_DRAW; break;
		case BT_DYNAMIC_DRAW: usage = GL_DYNAMIC_DRAW; break;
		case BT_STREAM_DRAW: usage = GL_DYNAMIC_DRAW; break;
		default: error_fatal("CVertexBuffer#SetVertexes: No such usage type");
		return;
	}

	glBufferData(	GL_ARRAY_BUFFER,				//Target
					sizeof(rendervert_t)*iVerts,	//Buffer Size
					pVerts,							//Data
					usage );						//Usage

	UnBind();

	m_vertexCount = iVerts;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CVertexBuffer::AddTrianglesToList( const rendertri_t *pTri, int iTris )
{
	m_vVerts.reserve( m_vVerts.size() + iTris*3 );

	for( int i = 0; i < iTris; i++ )
	{
		m_vVerts.push_back( pTri[i].pVerts[0] );
		m_vVerts.push_back( pTri[i].pVerts[1] );
		m_vVerts.push_back( pTri[i].pVerts[2] );
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CVertexBuffer::AddVertexesToList( const rendervert_t *verts, size_t numVerts )
{
	m_vVerts.insert(m_vVerts.end(), verts, verts + numVerts);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CVertexBuffer::SubmitTriangleList( void )
{
	this->SetVertexes( m_vVerts.data(), m_vVerts.size() );
	m_vVerts.clear();
	m_vVerts.shrink_to_fit();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CVertexBuffer::Bind( void )
{
	Assert( m_bInitialized );
	glBindBuffer( GL_ARRAY_BUFFER, m_iVboID );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CVertexBuffer::UnBind( void )
{
	Assert( m_bInitialized );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CVertexBuffer::Render( void )
{
	Assert(!m_bModifying);
	Assert(GetVertexCountSubmitted() > 2);
	glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
rendervert_t * CVertexBuffer::StartModification(bool bindBuffer /*= true*/)
{
	rendervert_t *ret = NULL;
	if(m_usage == BT_DYNAMIC_DRAW || m_usage == BT_STREAM_DRAW)
	{
		m_bModifying = true;
		if(bindBuffer)
			Bind();
		ret = (rendervert_t*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	}
	else
	{
		error_fatal("CVertexBuffer#StartModification(): Trying to modify static vertex buffer");
		return NULL;
	}

	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CVertexBuffer::EndModification(bool unbindBuffer /*= true*/)
{
	if(m_usage == BT_DYNAMIC_DRAW || m_usage == BT_STREAM_DRAW)
	{
		glUnmapBuffer(GL_ARRAY_BUFFER);
		if(unbindBuffer)
			UnBind();
		m_bModifying = false;
	}
	else
	{
		error_fatal("CVertexBuffer#EndModification(): Trying to unmap static vertex buffer");
	}
}
