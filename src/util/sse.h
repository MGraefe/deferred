// util/sse.h
// util/sse.h
// util/sse.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__sse_H__
#define deferred__util__sse_H__

#ifdef _SSE_ENABLED
	#include <xmmintrin.h>
	#define USE_SSE2
	#include "sse_trig.h"
	typedef __m128 m128;
	#define __m128 ERROR_USE_m128_INSTEAD

	#ifdef _MSC_VER
	# define SSE_ALIGN16_BEG __declspec(align(16))
	# define SSE_ALIGN16_END 
	#else /* gcc or icc */
	# define SSE_ALIGN16_BEG
	# define SSE_ALIGN16_END __attribute__((aligned(16)))
	#endif
#endif

#endif // deferred__util__sse_H__