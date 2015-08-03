// util/mmult.h
// util/mmult.h
// util/mmult.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

//MATHS_SSE speedup version of the matrix multiplication.
//uses some overloading for easier code

#pragma once
#ifndef deferred__util__mmult_H__
#define deferred__util__mmult_H__

//#define MATHS_SSE

#ifdef MATHS_SSE

#include <xmmintrin.h>

struct sse_vec4
{
	__m128 xmm;

	sse_vec4 (__m128 v) : xmm (v) {}

	sse_vec4 (float v) { 
		xmm = _mm_set1_ps(v); 
	}

	sse_vec4 (float x, float y, float z, float w) { 
		xmm = _mm_set_ps(w,z,y,x); 
	}

	sse_vec4 (const float *v) { 
		xmm = _mm_load_ps(v);
	}

	sse_vec4 operator* (const sse_vec4 &v) const { 
		return sse_vec4(_mm_mul_ps(xmm, v.xmm)); 
	}

	sse_vec4 operator+ (const sse_vec4 &v) const { 
		return sse_vec4(_mm_add_ps(xmm, v.xmm)); 
	}

	sse_vec4 operator- (const sse_vec4 &v) const { 
		return sse_vec4(_mm_sub_ps(xmm, v.xmm)); 
	}

	sse_vec4 operator/ (const sse_vec4 &v) const { 
		return sse_vec4(_mm_div_ps(xmm, v.xmm)); 
	}

	void operator*= (const sse_vec4 &v) { 
		xmm = _mm_mul_ps(xmm, v.xmm); 
	}

	void operator+= (const sse_vec4 &v) { 
		xmm = _mm_add_ps(xmm, v.xmm); 
	}

	void operator-= (const sse_vec4 &v) { 
		xmm = _mm_sub_ps(xmm, v.xmm); 
	}

	void operator/= (const sse_vec4 &v) { 
		xmm = _mm_div_ps(xmm, v.xmm); 
	}

	void operator>> (float *v) { 
		_mm_store_ps(v, xmm); 
	}
};

#else //No SSE present

struct no_sse_vec4
{
	//__m128 xmm;
	float x,y,z,w;

	//no_sse_vec4 (__m128 v) : xmm (v) {}

	no_sse_vec4 (float v) { x = y = z = w = v; }

	no_sse_vec4 (float X, float Y, float Z, float W) { 
		x = X; y = Y; z = Z; w = W; 
	}

	no_sse_vec4 (const float *v) { x = v[0]; y = v[1]; z = v[2]; w = v[3]; }

	no_sse_vec4 operator* (const no_sse_vec4 &v) const { 
		return no_sse_vec4(x*v.x, y*v.y, z*v.z, w*v.w); 
	}

	no_sse_vec4 operator+ (const no_sse_vec4 &v) const { 
		return no_sse_vec4(x+v.x, y+v.y, z+v.z, w+v.w); 
	}

	no_sse_vec4 operator- (const no_sse_vec4 &v) const { 
		return no_sse_vec4(x-v.x, y-v.y, z-v.z, w-v.w); 
	}

	no_sse_vec4 operator/ (const no_sse_vec4 &v) const { 
		return no_sse_vec4(x/v.x, y/v.y, z/v.z, w/v.w); 
	}

	void operator*= (const no_sse_vec4 &v) { 
		x *= v.x; 
		y *= v.y; 
		z *= v.z; 
		w *= v.w;
	}

	void operator+= (const no_sse_vec4 &v) { 
		x += v.x; 
		y += v.y; 
		z += v.z; 
		w += v.w;
	}

	void operator-= (const no_sse_vec4 &v) { 
		x -= v.x; 
		y -= v.y; 
		z -= v.z; 
		w -= v.w;
	}

	void operator/= (const no_sse_vec4 &v) { 
		x /= v.x; 
		y /= v.y; 
		z /= v.z; 
		w /= v.w;
	}

	void operator>> (float *v) { 
		v[0] = x;
		v[1] = y;
		v[2] = z;
		v[3] = w;
	}
};
#endif //No MATHS_SSE

#ifdef MATHS_SSE
	typedef sse_vec4 mmult_vec4;
#else
	typedef no_sse_vec4 mmult_vec4;
#endif

#endif
