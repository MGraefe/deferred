// util/CAlignedAllocator.cpp
// util/CAlignedAllocator.cpp
// util/CAlignedAllocator.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "maths.h"
#include <vector>

void testAlignedAllocator( void )
{
	std::vector<Matrix4, CAlignedAllocator<Matrix4> > vec;
	vec.push_back(Matrix4());
	std::vector<Matrix4, Matrix4f::Allocator> vec2;
	vec2.push_back(Matrix4());
}

#ifdef MATHS_SSE
	#include <xmmintrin.h>
#endif

void *aligned_malloc( size_t bytes )
{
#ifdef MATHS_SSE
	return _mm_malloc(bytes,16);
#else
	return malloc(bytes);
#endif
}

void aligned_free( void *ptr )
{
#ifdef MATHS_SSE
	_mm_free(ptr);
#else
	free(ptr);
#endif
}

//template<class _Ty>
//class CAlignedAllocator
//{
//public:
//	typedef _Ty value_type;
//	//typedef _Allocator_base<_Ty> _Mybase;
//	//typedef typename _Mybase::value_type value_type;
//
//	typedef value_type _FARQ *pointer;
//	typedef value_type _FARQ& reference;
//	typedef const value_type _FARQ *const_pointer;
//	typedef const value_type _FARQ& const_reference;
//
//	typedef size_t size_type;
//	typedef ptrdiff_t difference_type;
//
//	template<class _Other>
//	struct rebind
//	{	// convert this type to _ALLOCATOR<_Other>
//		typedef CAlignedAllocator<_Other> other;
//	};
//
//	pointer address(reference _Val) const
//	{	// return address of mutable _Val
//		return ((pointer) &(char&)_Val);
//	}
//
//	const_pointer address(const_reference _Val) const
//	{	// return address of nonmutable _Val
//		return ((const_pointer) &(char&)_Val);
//	}
//
//	CAlignedAllocator() throw()
//	{	// construct default allocator (do nothing)
//	}
//
//	CAlignedAllocator(const CAlignedAllocator<_Ty>&) throw()
//	{	// construct by copying (do nothing)
//	}
//
//	template<class _Other>
//	CAlignedAllocator(const CAlignedAllocator<_Other>&) throw()
//	{	// construct from a related allocator (do nothing)
//	}
//
//	template<class _Other>
//	CAlignedAllocator<_Ty>& operator=(const CAlignedAllocator<_Other>&)
//	{	// assign from a related allocator (do nothing)
//		return (*this);
//	}
//
//	void deallocate(pointer _Ptr, size_type)
//	{	// deallocate object at _Ptr, ignore size
//		::operator delete(_Ptr);
//	}
//
//	pointer allocate(size_type _Count)
//	{	// allocate array of _Count elements
//		return (_Allocate(_Count, (pointer)0));
//	}
//
//	pointer allocate(size_type _Count, const void _FARQ *)
//	{	// allocate array of _Count elements, ignore hint
//		return (allocate(_Count));
//	}
//
//	void construct(pointer _Ptr, const _Ty& _Val)
//	{	// construct object at _Ptr with value _Val
//		_Construct(_Ptr, _Val);
//	}
//
//	void construct(pointer _Ptr, _Ty&& _Val)
//	{	// construct object at _Ptr with value _Val
//		::new ((void _FARQ *)_Ptr) _Ty(_STD forward<_Ty>(_Val));
//	}
//
//	template<class _Other>
//	void construct(pointer _Ptr, _Other&& _Val)
//	{	// construct object at _Ptr with value _Val
//		::new ((void _FARQ *)_Ptr) _Ty(_STD forward<_Other>(_Val));
//	}
//
//	void destroy(pointer _Ptr)
//	{	// destroy object at _Ptr
//		_Destroy(_Ptr);
//	}
//
//	_SIZT max_size() const throw()
//	{	// estimate maximum array size
//		_SIZT _Count = (_SIZT)(-1) / sizeof (_Ty);
//		return (0 < _Count ? _Count : 1);
//	}
//};
