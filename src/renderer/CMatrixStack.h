// renderer/CMatrixStack.h
// renderer/CMatrixStack.h
// renderer/CMatrixStack.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__CMatrixStack_H__
#define deferred__renderer__CMatrixStack_H__

#include <util/basic_types.h>
#include <stack>
#include <util/dll.h>
#include "CRenderInterf.h"

typedef std::stack<Matrix4,std::vector<Matrix4,CAlignedAllocator<Matrix4>>> MatrixStack_t;
PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING;

class DLL CMatrixStack
{
public:
	CMatrixStack() {
		GetStack().push( Matrix4f().SetIdentity() );
		SetChanged();
	}

	inline void PushMatrix() {
		Assert( GetStack().size() < 32 );
		GetStack().push( GetStack().top() );
		SetChanged();
	}

	inline void PopMatrix() {
		Assert( GetStack().size() > 1 );
		GetStack().pop();
		SetChanged();
	}

	inline void Translate( float x, float y, float z ) {
		ALIGN_16 float m[] = { //row-major
			1.0f, 0.0f, 0.0f, x,
			0.0f, 1.0f, 0.0f, y,
			0.0f, 0.0f, 1.0f, z,
			0.0f, 0.0f, 0.0f, 1.0f };
		MultMatrixRowMajor(m);
	}

	inline void Translate( const Vector3f &v ) {
		Translate(v.x, v.y, v.z);
	}

	inline void Rotate( const Quaternion &quat ) {
		ALIGN_16 float m[16];
		quat.GetOpenglMatrix4x4( m );
		MultMatrix(m);
	}

	//Performance sucks.
	inline void Rotate( const Angle3d &angle ) {
		Rotate( angle.x, 1.0f, 0.0f, 0.0f );
		Rotate( angle.y, 0.0f, 1.0f, 0.0f );
		Rotate( angle.z, 0.0f, 0.0f, 1.0f );
	}

	//maybe optimize a bit?
	inline void Rotate( float deg, float x, float y, float z )
	{
		deg = DEG2RAD(deg);
		float c = cosf(deg);
		float s = sinf(deg);
		float lengthsq = x*x + y*y + z*z;

		//normalized?
		if( lengthsq < 0.999f || lengthsq > 1.0001f )
		{
			float lengthr = rsqrt(lengthsq);
			x *= lengthr;
			y *= lengthr;
			z *= lengthr;
		}

		float omc = 1.0f - c; //one minus c
		ALIGN_16 float m[] = { //row major
			x*x*omc+c,   x*y*omc-z*s, x*z*omc+y*s, 0.0f,
			y*x*omc+z*s, y*y*omc+c,   y*z*omc-x*s, 0.0f,
			x*z*omc-y*s, y*z*omc+x*s, z*z*omc+c,   0.0f,
			0.0f,		 0.0f,		  0.0f,		   1.0f };

		MultMatrixRowMajor(m);
	}

	inline void SetPerspective( float fovy, float aspect, float zNear, float zFar ) {
		float f = 1.0f/TAN(fovy*0.5f);
		float a1 = (zFar+zNear) / (zNear-zFar);
		float a2 = (2.0f*zFar*zNear)/(zNear-zFar);

		ALIGN_16 float fMat[] = { //column major
			f/aspect, 0.0f, 0.0f, 0.0f,
			0.0f,     f,    0.0f, 0.0f,
			0.0f,     0.0f, a1,   -1.0f,
			0.0f,     0.0f, a2,   0.0f };
		GetStack().top().Set( fMat );
		SetChanged();
	}


	inline void SetOrthographic( float left, float right, float bottom,
		float top, float nearVal, float farVal )
	{
		float tx = -(right+left)/(right-left);
		float ty = -(top+bottom)/(top-bottom);
		float tz = -(farVal+nearVal)/(farVal-nearVal);
		float f1 = 2.0f/(right-left);
		float f2 = 2.0f/(top-bottom);
		float f3 = -2.0f/(farVal-nearVal);

		ALIGN_16 float m[] = { //Column major!
			f1,		0.0f,	0.0f,	0.0f,
			0.0f,	f2,		0.0f,	0.0f,
			0.0f,	0.0f,	f3,		0.0f,
			tx,		ty,		tz,		1.0f };
		GetStack().top().Set( m );
		SetChanged();
	}


	inline void Scale( float x, float y, float z ) {
		ALIGN_16 float m[] = {	x,0.0f,0.0f,0.0f,
								0.0f,y,0.0f,0.0f,
								0.0f,0.0f,z,0.0f,
								0.0f,0.0f,0.0f,1.0f };
		MultMatrix(m);
	}

	inline void MultMatrix( const Matrix3 &mat ) {
		ALIGN_16 float fMat[16];
		mat.ToOpenGlMatrix(fMat);
		GetStack().top().Mult( fMat );
		SetChanged();
	}

	inline void MultMatrix( const Matrix4f &mat ) {
		GetStack().top() *= mat;
		SetChanged();
	}

	inline void MultMatrix( const float *fv16 ) {
		GetStack().top().Mult( fv16 );
		SetChanged();
	}

	inline void MultMatrixRowMajor( const float *fv16 ) {
		GetStack().top().MultRowMajor( fv16 );
		SetChanged();
	}

	inline void SetMatrix( const float *fv16 ) {
		GetStack().top().Set( fv16 );
		SetChanged();
	}

	inline void SetMatrix( const Matrix4f &m ) {
		GetStack().top().Set( m );
		SetChanged();
	}

	inline void SetIdentity( void ) {
		GetStack().top().SetIdentity();
		SetChanged();
	}

	inline const float *GetMatrix( void ) const {
		return GetStack().top().Get();
	}

	inline const Matrix4f &GetMatrixObj( void ) const {
		return GetStack().top();
	}

	inline bool HasChanged( void ) const {
		return m_bChanged;
	}

	inline void ResetChanged( void ) {
		m_bChanged = false;
	}

	inline void SetChanged( void ) {
		m_bChanged = true;
	}

private:
	inline MatrixStack_t &GetStack( void ) {
		return m_Stack;
	}

	inline const MatrixStack_t &GetStack( void ) const {
		return m_Stack;
	}

private:
	MatrixStack_t m_Stack;
	bool m_bChanged;
};


POP_PRAGMA_WARNINGS;



inline CMatrixStack &GetMVStack( void ) { return *(g_RenderInterf->GetMVStack()); }
inline CMatrixStack &GetProjStack( void ) { return *(g_RenderInterf->GetProjStack()); }


#endif
