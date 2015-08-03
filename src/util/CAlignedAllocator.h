// util/CAlignedAllocator.h
// util/CAlignedAllocator.h
// util/CAlignedAllocator.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CAlignedAllocator.h
#pragma once
#ifndef deferred__util__CAlignedAllocator_H__
#define deferred__util__CAlignedAllocator_H__

#include <exception>
#include <cstddef>

void *aligned_malloc( size_t bytes );
void aligned_free( void *ptr );

template<class _Ty>
class CAlignedAllocator
{
public:

	typedef _Ty						value_type;	
	typedef	value_type*				pointer;
	typedef	value_type&				reference;
	typedef const	value_type*		const_pointer;
	typedef const	value_type&		const_reference;
	typedef size_t					size_type;
	typedef ptrdiff_t				difference_type;

	template<class _Other>
	struct rebind
	{
		typedef CAlignedAllocator<_Other> other;
	};

	_Ty *address(_Ty &_Val) const
	{
		return ((_Ty*) (&((char&)_Val)));
	}

	const _Ty *address(const _Ty &_Val) const
	{
		return ((const _Ty*) (&((char&)_Val)));
	}

	CAlignedAllocator() throw() 
	{
	}
	
	template<class _Other>
	CAlignedAllocator(const CAlignedAllocator<_Other>&) throw()
	{
	}

	template<class _Other>
	CAlignedAllocator<_Ty>& operator=(const CAlignedAllocator<_Other>&)
	{
		return *this;
	}

	void deallocate(_Ty *ptr, size_t size)
	{
		aligned_free(ptr);
	}

	_Ty* allocate(size_t _Count )
	{
		size_t maxElements = (size_t)(-1) / sizeof(_Ty);
		void *ptr = NULL;

		if( _Count > maxElements || (ptr = aligned_malloc(_Count * sizeof(_Ty))) == NULL )
#ifdef USE_OLD_VS
			throw std::bad_alloc( "Bad Alloc in CAlignedAllocator" );
#else
			throw std::bad_alloc();
#endif

		return (_Ty*)ptr;
	}

	_Ty* allocate(size_t _Count, const void *hint )
	{
		return allocate(_Count);
	}

	void construct(_Ty *ptr, const _Ty& val )
	{
		new ((void*)ptr) _Ty(val);
		//::operator new ((void*)ptr) _Ty(val);
	}

	void construct(_Ty *ptr, _Ty&& val )
	{
		new ((void*)ptr) _Ty(val);
		//::operator new ((void*)ptr) _Ty(val);
	}

	template<class _Other>
	void construct( _Ty *ptr, _Other&& val )
	{
		new ((void*)ptr) _Ty(val);
		//::operator new ((void*)ptr) _Ty(val);
	}

	void destroy( _Ty *ptr )
	{
		ptr->~_Ty();
	}

	size_t max_size() const throw()
	{
		size_t count = (size_t)(-1) / sizeof(_Ty);
		return count > 0 ? count : 1;
	}

	bool operator==(const CAlignedAllocator<_Ty> &other)
	{
		return this == &other;
	}
};



#endif // deferred__util__CAlignedAllocator_H__
