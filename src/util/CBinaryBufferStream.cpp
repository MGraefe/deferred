// util/CBinaryBufferStream.cpp
// util/CBinaryBufferStream.cpp
// util/CBinaryBufferStream.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CBinaryBufferStream.h"

//#########################################
// CBinaryBufferBase
//#########################################
CBinaryBufferBase::CBinaryBufferBase(bool dummyMode) : 
	m_pBuf(NULL),
	m_iSize(0),
	m_bDummyMode(dummyMode),
	m_error(ERROR_NONE) 
{}

CBinaryBufferBase::CBinaryBufferBase(char *buffer, UINT size) : 
	m_pBuf(buffer),
	m_iSize(size),
	m_bDummyMode(false),
	m_error(ERROR_NONE) 
{}


CBinaryBufferBase::error_e CBinaryBufferBase::GetError() const
{
	return m_error;
}


char *CBinaryBufferBase::GetBuffer() const
{
	return m_pBuf;
}


void CBinaryBufferBase::SetBuffer( char *buffer, UINT size )
{
	m_pBuf = buffer;
	m_iSize = size;
}




//#########################################
// CBinaryBufferInputStream
//#########################################
CBinaryBufferInputStream::CBinaryBufferInputStream(bool dummyMode /*= false*/) :
	CBinaryBufferBase(dummyMode),
	m_iGetPtr(0)
{
}

CBinaryBufferInputStream::CBinaryBufferInputStream(char *buffer, UINT size) :
	CBinaryBufferBase(buffer, size),
	m_iGetPtr(0)
{
}

bool CBinaryBufferInputStream::read( char *data, UINT size )
{
	Assert( m_bDummyMode || m_pBuf );
	Assert( m_bDummyMode || m_iGetPtr + size <= m_iSize );
	if( !m_bDummyMode )
	{
		if( m_iGetPtr + size <= m_iSize )
			memcpy( data, m_pBuf+m_iGetPtr, size );
		else
		{
			m_error = ERROR_READ;
			return false;
		}
	}
	m_iGetPtr += size;
	m_error = ERROR_NONE;
	return true;
}

UINT CBinaryBufferInputStream::tellg( void ) const
{
	return m_iGetPtr;
}

bool CBinaryBufferInputStream::seekg( UINT pos )
{
	Assert( m_bDummyMode || pos <= m_iSize );
	if(m_bDummyMode || pos <= m_iSize)
	{
		m_iGetPtr = pos;
		m_error = ERROR_NONE;
		return true;
	}
	else
	{
		m_error = ERROR_SEEKG;
		return false;
	}
}

void CBinaryBufferInputStream::SetBuffer( char *buffer, UINT size )
{
	CBinaryBufferBase::SetBuffer(buffer, size);
	m_iGetPtr = 0;
}




//#########################################
// CBinaryBufferOutputStream
//#########################################
CBinaryBufferOutputStream::CBinaryBufferOutputStream(bool dummyMode /*= false*/) :
	CBinaryBufferBase(dummyMode),
	m_iPutPtr(0)
{
}

CBinaryBufferOutputStream::CBinaryBufferOutputStream(char *buffer, UINT size) :
	CBinaryBufferBase(buffer, size),
	m_iPutPtr(0)
{
}

bool CBinaryBufferOutputStream::write( const char *data, UINT size )
{
	Assert( m_bDummyMode || m_pBuf );
	Assert( m_bDummyMode || m_iPutPtr + size <= m_iSize );
	if( !m_bDummyMode )
	{
		if( m_iPutPtr + size <= m_iSize )
			memcpy(m_pBuf+m_iPutPtr, data, size );
		else
		{
			m_error = ERROR_WRITE;
			return false;
		}
	}
	m_iPutPtr += size;
	m_error = ERROR_NONE;
	return true;
}

UINT CBinaryBufferOutputStream::tellp( void ) const
{
	return m_iPutPtr;
}

bool CBinaryBufferOutputStream::seekp( UINT pos )
{
	Assert( m_bDummyMode || pos <= m_iSize );
	if(m_bDummyMode || pos <= m_iSize)
	{
		m_iPutPtr = pos;
		m_error = ERROR_NONE;
		return true;
	}
	else
	{
		m_error = ERROR_SEEKP;
		return false;
	}
}

void CBinaryBufferOutputStream::SetBuffer( char *buffer, UINT size )
{
	CBinaryBufferBase::SetBuffer(buffer, size);
	m_iPutPtr = 0;
}





//#########################################
// CBinaryBufferStream
//#########################################
CBinaryBufferStream::CBinaryBufferStream( bool dummyMode /*= false*/ ) :
	CBinaryBufferInputStream(dummyMode),
	CBinaryBufferOutputStream(dummyMode),
	CBinaryBufferBase(dummyMode)
{

}

CBinaryBufferStream::CBinaryBufferStream( char *buffer, UINT size ) :
	CBinaryBufferInputStream(buffer, size),
	CBinaryBufferOutputStream(buffer, size),
	CBinaryBufferBase(buffer, size)
{

}

void CBinaryBufferStream::SetBuffer( char *buffer, UINT size )
{
	CBinaryBufferInputStream::SetBuffer(buffer, size);
	CBinaryBufferOutputStream::SetBuffer(buffer, size);
}
