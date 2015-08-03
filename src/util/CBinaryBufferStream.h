// util/CBinaryBufferStream.h
// util/CBinaryBufferStream.h
// util/CBinaryBufferStream.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__CBinaryBufferStream_H__
#define deferred__util__CBinaryBufferStream_H__

#include "error.h"
#include "maths.h"

typedef unsigned int UINT;

class CBinaryBufferBase
{
public:
	enum error_e
	{
		ERROR_NONE = 0,
		ERROR_READ,
		ERROR_WRITE,
		ERROR_SEEKP,
		ERROR_SEEKG,
	};

	error_e GetError() const;
	char *GetBuffer() const;
	virtual void SetBuffer(char *buffer, UINT size);

protected:
	CBinaryBufferBase(bool dummyMode);
	CBinaryBufferBase(char *buffer, UINT size);

protected:
	char *m_pBuf;
	UINT m_iSize;
	error_e m_error;
	bool m_bDummyMode;
};


class CBinaryBufferInputStream : public virtual CBinaryBufferBase
{
public:
	CBinaryBufferInputStream(bool dummyMode = false);
	CBinaryBufferInputStream(char *buffer, UINT size);

	bool read( char *data, UINT size );
	UINT tellg( void ) const;
	bool seekg( UINT pos );
	virtual void SetBuffer( char *buffer, UINT size );

private:
	UINT m_iGetPtr;

public:
	template <typename T>
	inline CBinaryBufferInputStream& operator>>( T &val )
	{
		Assert( m_bDummyMode || m_pBuf );
		Assert( m_bDummyMode || m_iGetPtr + sizeof(T) <= m_iSize );
		if( !m_bDummyMode )
		{
			if( m_iGetPtr + sizeof(T) <= m_iSize )
				val = *((T*)(m_pBuf+m_iGetPtr));
			else
			{
				m_error = ERROR_READ;
				return *this;
			}
		}
		m_error = ERROR_NONE;
		m_iGetPtr += sizeof(T);
		return *this;
	}

	//This one is dangerous
	inline CBinaryBufferInputStream& operator>>(char *str)
	{
		int length = strlen(m_pBuf+m_iGetPtr);
		read( str, length+1 );
		return *this;
	}

	//This one is dangerous
	inline CBinaryBufferInputStream& operator>>( unsigned char *str)
	{
		int length = strlen(m_pBuf+m_iGetPtr);
		read( (char*)str, length+1 );
		return *this;
	}

	//This one is dangerous
	inline CBinaryBufferInputStream& operator>>(signed char *str)
	{
		int length = strlen(m_pBuf+m_iGetPtr);
		read( (char*)str, length+1 );
		return *this;
	}
};



class CBinaryBufferOutputStream : public virtual CBinaryBufferBase
{
public:
	CBinaryBufferOutputStream(bool dummyMode = false);
	CBinaryBufferOutputStream(char *buffer, UINT size);

	bool write( const char *data, UINT size );

	UINT tellp( void ) const;
	bool seekp( UINT pos );
	virtual void SetBuffer( char *buffer, UINT size );

private:
	UINT m_iPutPtr;

public:
	template <typename T>
	inline typename std::enable_if<std::is_arithmetic<T>::value, CBinaryBufferOutputStream>::type&
	operator<<( const T &val )
	{
		Assert( m_bDummyMode || m_pBuf );
		Assert( m_bDummyMode || m_iPutPtr + sizeof(T) <= m_iSize );
		if( !m_bDummyMode )
		{
			if( m_iPutPtr + sizeof(T) <= m_iSize )
			{
				memcpy(m_pBuf+m_iPutPtr, &val, sizeof(val));
				//*((T*)(m_pBuf+m_iPutPtr)) = val;
			}
			else 
			{
				m_error = ERROR_WRITE;
				return *this;
			}
		}
		m_error = ERROR_NONE;
		m_iPutPtr += sizeof(T);
		return *this;
	}

	inline CBinaryBufferOutputStream& operator<<(const char *str)
	{
		int length = strlen(str);
		write( str, length+1 );
		return *this;
	}

	inline CBinaryBufferOutputStream& operator<<(const unsigned char *str)
	{
		int length = strlen((char*)str);
		write( (char*)str, length+1 );
		return *this;
	}

	inline CBinaryBufferOutputStream& operator<<(const signed *str)
	{
		int length = strlen((char*)str);
		write( (char*)str, length+1 );
		return *this;
	}
};


class CBinaryBufferStream : public CBinaryBufferInputStream, public CBinaryBufferOutputStream
{
public:
	CBinaryBufferStream(bool dummyMode = false);
	CBinaryBufferStream(char *buffer, UINT size);
	void SetBuffer(char *buffer, UINT size);
};


//Vector3f
inline CBinaryBufferOutputStream &operator<<(CBinaryBufferOutputStream &st, const Vector3f &vec)
{
	Assert(vec.IsValid());
	st.write( (char*)(&vec), sizeof(Vector3f) );
	return st;
}

inline CBinaryBufferInputStream &operator>>(CBinaryBufferInputStream &st, Vector3f &vec)
{
	st.read( (char*)(&vec), sizeof(Vector3f) );
	return st;
}


//Angle3dCompressed
inline CBinaryBufferOutputStream &operator<<(CBinaryBufferOutputStream &st, const Angle3dCompressed &ang)
{
	st.write( (char*)(&ang), Angle3dCompressed::sNetSize );
	return st;
}

inline CBinaryBufferInputStream &operator>>(CBinaryBufferInputStream &st, Angle3dCompressed &ang)
{
	st.read( (char*)(&ang), Angle3dCompressed::sNetSize );
	return st;
}


//Vector3fCompressed
inline CBinaryBufferOutputStream &operator<<(CBinaryBufferOutputStream &st, const Vector3fCompressed &vec)
{
	st.write( (char*)(&vec), sizeof(Vector3fCompressed) );
	return st;
}

inline CBinaryBufferInputStream &operator>>(CBinaryBufferInputStream &st, Vector3fCompressed &vec)
{
	st.read( (char*)(&vec), sizeof(Vector3fCompressed) );
	return st;
}


//Quaternion
inline CBinaryBufferOutputStream &operator<<(CBinaryBufferOutputStream &st, const Quaternion &quat)
{
	Assert(quat.IsValid());
	st.write( (char*)(&quat), sizeof(Quaternion) );
	return st;
}

inline CBinaryBufferInputStream &operator>>(CBinaryBufferInputStream &st, Quaternion &quat)
{
	st.read( (char*)(&quat), sizeof(Quaternion) );
	return st;
}

//std::string
inline CBinaryBufferOutputStream &operator<<(CBinaryBufferOutputStream &st, const std::string &str)
{
	Assert(str.size() < std::numeric_limits<USHORT>::max());
	st << (USHORT)str.size();
	st.write(str.c_str(), str.size());
	return st;
}

inline CBinaryBufferInputStream &operator>>(CBinaryBufferInputStream &st, std::string &str)
{
	USHORT size = 0;
	st >> size;
	char *buf = new char[size];
	st.read(buf, size);
	str.assign(buf, size);
	delete[] buf;
	return st;
}




//class CBinaryBufferStream
//{
//public:
//	enum error_e
//	{
//		ERROR_NONE = 0,
//		ERROR_READ,
//		ERROR_WRITE,
//		ERROR_SEEKP,
//		ERROR_SEEKG,
//	};
//
//	enum openmode_e
//	{
//		MODE_READ = 0,
//		MODE_WRITE,
//		MODE_BOTH,
//	};
//
//public:
//	CBinaryBufferStream(bool dummyMode = false, openmode_e mode = MODE_BOTH);
//	CBinaryBufferStream(char *buffer, UINT size, openmode_e mode = MODE_BOTH);
//
//	bool write( const char *data, UINT size );
//	bool read( char *data, UINT size );
//
//	UINT tellg( void ) const;
//	UINT tellp( void ) const;
//	bool seekg( UINT pos );
//	bool seekp( UINT pos );
//	void SetBuffer( char *buffer, UINT size );
//	char *GetBuffer( void ) const;
//	inline openmode_e GetMode() const { return m_mode; }
//	inline bool IsReadValid() const { return m_mode == MODE_READ || m_mode == MODE_BOTH; }
//	inline bool IsWriteValid() const { return m_mode == MODE_WRITE || m_mode == MODE_BOTH; }
//private:
//	char *m_pBuf;
//	UINT m_iSize;
//	UINT m_iPutPtr;
//	UINT m_iGetPtr;
//	error_e m_error;
//	const openmode_e m_mode;
//	bool m_bDummyMode;
//
//public:
//
//	inline error_e GetError() const {
//		return m_error;
//	}
//
//	template <typename T>
//	inline CBinaryBufferStream& operator<<( const T &val )
//	{
//		Assert( IsWriteValid() );
//		if( !IsWriteValid() )
//		{
//			m_error = ERROR_WRITE;
//			return *this;
//		}
//
//		Assert( m_bDummyMode || m_pBuf );
//		Assert( m_bDummyMode || m_iPutPtr + sizeof(T) <= m_iSize );
//		if( !m_bDummyMode )
//		{
//			if( m_iPutPtr + sizeof(T) <= m_iSize )
//				*((T*)(m_pBuf+m_iPutPtr)) = val;
//			else 
//			{
//				m_error = ERROR_WRITE;
//				return *this;
//			}
//		}
//		m_error = ERROR_NONE;
//		m_iPutPtr += sizeof(T);
//		return *this;
//	}
//
//	template <typename T>
//	inline CBinaryBufferStream& operator>>( T &val )
//	{
//		Assert( IsReadValid() );
//		if( !IsReadValid() )
//		{
//			m_error = ERROR_READ;
//			return *this;
//		}
//
//		Assert( m_bDummyMode || m_pBuf );
//		Assert( m_bDummyMode || m_iGetPtr + sizeof(T) <= m_iSize );
//		if( !m_bDummyMode )
//		{
//			if( m_iPutPtr + sizeof(T) <= m_iSize )
//				val = *((T*)(m_pBuf+m_iGetPtr));
//			else
//			{
//				m_error = ERROR_READ;
//				return *this;
//			}
//		}
//		m_error = ERROR_NONE;
//		m_iGetPtr += sizeof(T);
//		return *this;
//	}
//
//	inline CBinaryBufferStream& operator<<(const char *str)
//	{
//		int length = strlen(str);
//		write( str, length+1 );
//		return *this;
//	}
//
//	//This one is dangerous
//	inline CBinaryBufferStream& operator>>(char *str)
//	{
//		int length = strlen(m_pBuf+m_iGetPtr);
//		read( str, length+1 );
//		return *this;
//	}
//
//	inline CBinaryBufferStream& operator<<(const unsigned char *str)
//	{
//		int length = strlen((char*)str);
//		write( (char*)str, length+1 );
//		return *this;
//	}
//
//	//This one is dangerous
//	inline CBinaryBufferStream& operator>>( unsigned char *str)
//	{
//		int length = strlen(m_pBuf+m_iGetPtr);
//		read( (char*)str, length+1 );
//		return *this;
//	}
//
//	inline CBinaryBufferStream& operator<<(const signed *str)
//	{
//		int length = strlen((char*)str);
//		write( (char*)str, length+1 );
//		return *this;
//	}
//
//	//This one is dangerous
//	inline CBinaryBufferStream& operator>>(signed char *str)
//	{
//		int length = strlen(m_pBuf+m_iGetPtr);
//		read( (char*)str, length+1 );
//		return *this;
//	}
//};


#endif
