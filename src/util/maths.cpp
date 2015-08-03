// util/maths.cpp
// util/maths.cpp
// util/maths.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "maths.h"

#ifdef MATHS_SSE
#include <xmmintrin.h>
#endif

#include <cstdlib>
//#include <ctime>
#include <limits.h>
#include "error.h"
#include "collision.h"
//#include "../client/glheaders.h"
//#ifndef _PANICGAME
//	#include "..\studiorpm\stdafx.h"
//#endif

//Random
//void InitRandom( void )
//{
//	time_t now = time( NULL );
//	unsigned char *p = (unsigned char *)&now;
//	unsigned seed = 0;
//	size_t i;
//	for ( i = 0; i < sizeof now; i++ )
//		seed = seed * ( 0xFF + 2U ) + p[i];
//
//	srand( seed );
//}



//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
int RandomInt( int min, int max )
{
	//InitRandom();
	return min + rand() / ( RAND_MAX / ( (max+1) - min ) + 1 );
	//return rand() % (max - min + 1) + min;
}


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
Vector2f RandomVector( int MinX, int MinY, int MaxX, int MaxY )
{
	return Vector2f( (float)RandomInt( MinX, MaxX ), (float)RandomInt( MinY, MaxY ) );
}


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
float RandomFloat( float min, float max )
{
	return lerp( (float)rand(), 0.0f, (float)RAND_MAX, min, max );
}


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
float VectorLength( Vector2f v )
{
	return sqrt( v.x*v.x + v.y*v.y );
}

//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
float VectorLength( const Vector3f &v )
{
	return v.Length();
}

//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
float VectorLengthSq( const Vector3f &v )
{
	return v.LengthSq();
}

//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
void VectorNormalize( Vector2f &v )
{
	v *= 1.0f/VectorLength(v);
}

//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
void VectorNormalize( Vector3f &v )
{
	v.Normalize();
}


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
Vector3f VectorCross( const Vector3f &a, const Vector3f &b )
{
	return a.Cross(b);
}


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
float VectorDot( const Vector3f &a, const Vector3f &b )
{
	return a.Dot(b);
}


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
extern void GetAngleMatrix3x3( const Angle3d &Angle, Matrix3 &m )
{
	const float s1 = SIN( Angle.z );
	const float s2 = SIN( Angle.y );
	const float s3 = SIN( Angle.x );
	const float c1 = COS( Angle.z );
	const float c2 = COS( Angle.y );
	const float c3 = COS( Angle.x );

	m[0][0] = c1*c2;  m[0][1] = c1*s2*s3-s1*c3; m[0][2] = s1*s3+c1*s2*c3;
	m[1][0] = s1*c2;  m[1][1] = s1*s2*s3+c1*c3; m[1][2] = s1*s2*c3-c1*s3;
	m[2][0] = -s2;	  m[2][1] = c2*s3;          m[2][2] = c2*c3;
}


//-----------------------------------------------------------------------------
// purpose: returns row-major rotation matrix
//-----------------------------------------------------------------------------
void GetAngleMatrix3x3( const Angle3d &Angle, float *m )
{
	const float s1 = SIN( Angle.z );
	const float s2 = SIN( Angle.y );
	const float s3 = SIN( Angle.x );
	const float c1 = COS( Angle.z );
	const float c2 = COS( Angle.y );
	const float c3 = COS( Angle.x );

	m[0] = c1*c2; m[1] = c1*s2*s3-s1*c3; m[2] = s1*s3+c1*s2*c3;
	m[3] = s1*c2; m[4] = s1*s2*s3+c1*c3; m[5] = s1*s2*c3-c1*s3;
	m[6] = -s2;	  m[7] = c2*s3;          m[8] = c2*c3;
}


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
void AngleToVector( const Vector3f &Angle, Vector3f &Vec )
{
	const float s1 = SIN( Angle.z );
	const float s2 = SIN( Angle.y );
	const float s3 = SIN( Angle.x );
	const float c1 = COS( Angle.z );
	const float c2 = COS( Angle.y );
	const float c3 = COS( Angle.x );

	Vec.x = -s1*s3 - c1*c3*s2;
	Vec.y = c1*s3 - c3*s1*s2;
    Vec.z = -c2*c3;
}


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
#define ANGTOVEC_TESTS 8
void AngleToVectorTest( void )
{
	Angle3d angs[ANGTOVEC_TESTS];
	angs[0].Init( 0, 0, 0 ); //should be 0, 0, -1
	angs[1].Init( 0, 180, 0 ); //should be 0, 0, 1
	angs[2].Init( 180, 0, 0 ); //should be 0, 0, -1
	angs[3].Init( 180, 180, 0 ); //should be 0, 0, -1
	angs[4].Init( 0, 90, 0 ); //should be -1, 0, 0
	angs[5].Init( 90, 0, 0 ); //should be 0, 1, 0
	angs[6].Init( 0, 45, 0 ); //should be -0.7, 0, -0.7
	angs[7].Init( 45, 45, 0 ); //should be -1, 1, -1 * sqrt(3)

	Vector3f vecs[ANGTOVEC_TESTS];
	Angle3d angs2[ANGTOVEC_TESTS];
	for( int i = 0; i < ANGTOVEC_TESTS; i++ )
	{
		AngleToVector( angs[i], vecs[i] );
		VectorToAngle( vecs[i], angs2[i] );
	}
}



//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
void VectorToAngle( const Vector3f &Vec, Vector3f &Angle )
{
	float tmp, yaw, pitch;

	if (Vec.x == 0.0f && Vec.z == 0.0f)
	{
		yaw = 0.0f;
		if (Vec.y > 0)
			pitch = 270.0f;
		else
			pitch = 90.0f;
	}
	else if( Vec.y == 0.0f && Vec.z == 0.0f )
	{
		yaw = 0.0f;
		if( Vec.x < 0.0f )
			pitch = 90.0f;
		else
			pitch = 270.0f;
	}
	else
	{

		yaw = -atan2( Vec.z, Vec.x ) * (180.0f / PI) - 90.0f;
		if (yaw < 0)
			yaw += 360.0f;

		tmp = sqrt( Vec.x*Vec.x + Vec.z*Vec.z );
		pitch = -atan2(-Vec.y, tmp) * (180.0f / PI);
		if (pitch < 0)
			pitch += 360;
	}

	Angle.x = pitch;
	Angle.y = yaw;
	Angle.z = 0;

	Assert( Angle.IsValid() );
}


//-----------------------------------------------------------------------------
// make sure quaternions are within 180 degrees of one another, if not, reverse q
//-----------------------------------------------------------------------------
void QuaternionAlign( const Quaternion &p, const Quaternion &q, Quaternion &qt )
{
	// decide if one of the quaternions is backwards
	Quaternion qa( p-q );
	Quaternion qb( p+q );
	float a = qa.Dot(qa);
	float b = qb.Dot(qb);

	if (a > b)
	{
		qt = -q;
	}
	else if (&qt != &q)
	{
		qt = q;
	}
}

//-----------------------------------------------------------------------------
// Quaternion sphereical linear interpolation
//-----------------------------------------------------------------------------
void QuaternionSlerp( const Quaternion &p, const Quaternion &q, const float &t, Quaternion &qt )
{
	Quaternion q2;
	// 0.0 returns p, 1.0 return q.

	// decide if one of the quaternions is backwards
	QuaternionAlign( p, q, q2 );

	QuaternionSlerpNoAlign( p, q2, t, qt );
	Assert(q.IsValid());
}


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
void QuaternionSlerpNoAlign( const Quaternion &p, const Quaternion &q, const float &t, Quaternion &qt )
{
	float omega, cosom, sinom, sclp, sclq;

	// 0.0 returns p, 1.0 return q.

	cosom = p.Dot(q);

	if ((1.0f + cosom) > 0.000001f)
	{
		if ((1.0f - cosom) > 0.000001f)
		{
			omega = acos( cosom );
			sinom = sin( omega );
			//Assert(abs(sinom) > std::numeric_limits<float>::epsilon());
			if( sinom == 0.0f )
				sinom = std::numeric_limits<float>::epsilon();
			sclp = sin( (1.0f - t)*omega) / sinom;
			sclq = sin( t*omega ) / sinom;
		}
		else
		{
			// TODO: add short circuit for cosom == 1.0f?
			sclp = 1.0f - t;
			sclq = t;
		}
		qt = p * sclp + q * sclq;
	}
	else
	{
		Assert( &qt != &q );

		qt.x = -q.y;
		qt.y =  q.x;
		qt.z = -q.w;
		qt.w =  q.z;
		sclp = sin( (1.0f - t) * (0.5f * PI));
		sclq = sin( t * (0.5f * PI));
		qt.x = sclp * p.x + sclq * qt.x;
		qt.y = sclp * p.y + sclq * qt.y;
		qt.z = sclp * p.z + sclq * qt.z;
	}

	Assert( qt.IsValid() );
}


const float Math::EPSILON = 1e-6f;

const Matrix3 Matrix3::IDENTITY(1.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 1.0f);


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
Matrix3 Matrix3::GetInverse() const
{
    // If the inverse doesn't exist for this matrix, then the identity
    // matrix will be returned.

    Matrix3 tmp;
    float d = determinant();

    if (Math::closeEnough(d, 0.0f))
    {
        tmp.identity();
    }
    else
    {
        d = 1.0f / d;

        tmp.mtx[0][0] = d * (mtx[1][1] * mtx[2][2] - mtx[1][2] * mtx[2][1]);
        tmp.mtx[0][1] = d * (mtx[0][2] * mtx[2][1] - mtx[0][1] * mtx[2][2]);
        tmp.mtx[0][2] = d * (mtx[0][1] * mtx[1][2] - mtx[0][2] * mtx[1][1]);

        tmp.mtx[1][0] = d * (mtx[1][2] * mtx[2][0] - mtx[1][0] * mtx[2][2]);
        tmp.mtx[1][1] = d * (mtx[0][0] * mtx[2][2] - mtx[0][2] * mtx[2][0]);
        tmp.mtx[1][2] = d * (mtx[0][2] * mtx[1][0] - mtx[0][0] * mtx[1][2]);

        tmp.mtx[2][0] = d * (mtx[1][0] * mtx[2][1] - mtx[1][1] * mtx[2][0]);
        tmp.mtx[2][1] = d * (mtx[0][1] * mtx[2][0] - mtx[0][0] * mtx[2][1]);
        tmp.mtx[2][2] = d * (mtx[0][0] * mtx[1][1] - mtx[0][1] * mtx[1][0]);
    }

    return tmp;
}


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
Matrix3 TransposeMatrix( Matrix3 mat )
{
	Matrix3 ret;
	ret[0][0] = mat[0][0];
	ret[1][0] = mat[0][1];
	ret[2][0] = mat[0][2];
	ret[0][1] = mat[1][0];
	ret[1][1] = mat[1][1];
	ret[2][1] = mat[1][2];
	ret[0][2] = mat[2][0];
	ret[1][2] = mat[2][1];
	ret[2][2] = mat[2][2];
	return ret;
}


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
float max3f( float a, float b, float c )
{
	float max = a;
	if( b > max )
		max = b;
	if( c > max )
		max = c;

	return max;
}


//-----------------------------------------------------------------------------
// purpose:
//-----------------------------------------------------------------------------
#define SQRT_2PI 2.506628274631f
void GaussDistribution( const int number, const float sigma, float *values )
{
	for( int i = 0; i < number; i++ )
	{
		float x = (float)i - (float)(number/2);
		float exponent = (-x*x)/(2.0f*sigma*sigma);
		values[i] = (1.0f/(SQRT_2PI*sigma)) * exp( exponent );
	}
}


//-----------------------------------------------------------------------------
//							3		5		7	9	  11    13    15	17
//-----------------------------------------------------------------------------
const float fSigmas[] = {0.417079f, 1.54f, 1.78f, 2.12f, 2.54f, 2.95f, 3.34f, 4.95f};
void BlurValues( const int number, float *values )
{
	GaussDistribution( number, fSigmas[number/2], values );
}

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4700 )
#endif

template<typename T>
void Inverse_4x4(T *m)
{
    T inv[16];
    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    T det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == T(0.0))
        return;

    det = T(1.0) / det;

    for (int i = 0; i < 16; i++)
        m[i] = inv[i] * det;
}

//Matrix4 inverse with MATHS_SSE
#ifdef MATHS_SSE
void PIII_Inverse_4x4(float* src)
{
	__m128 minor0, minor1, minor2, minor3;
	__m128 row0, row1, row2, row3;
	__m128 det, tmp1;
#ifdef __MSVC_RUNTIME_CHECKS //overcome nasty runtime check breaks
	for( int i = 0; i < 4; i++ )
		row0.m128_f32[i] = row1.m128_f32[i] = row2.m128_f32[i] = row3.m128_f32[i] = tmp1.m128_f32[i] = 0.0f;
#endif
	tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src)), (__m64*)(src+ 4));
	row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (__m64*)(src+8)), (__m64*)(src+12));
	row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
	row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);
	tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64*)(src+ 2)), (__m64*)(src+ 6));
	row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (__m64*)(src+10)), (__m64*)(src+14));
	row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
	row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row2, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor0 = _mm_mul_ps(row1, tmp1);
	minor1 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
	minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
	minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row1, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
	minor3 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
	minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	row2 = _mm_shuffle_ps(row2, row2, 0x4E);
	minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
	minor2 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
	minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
	minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);
	// -----------------------------------------------
	det = _mm_mul_ps(row0, minor0);
	det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
	det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
	tmp1 = _mm_rcp_ss(det);
	det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
	det = _mm_shuffle_ps(det, det, 0x00);
	minor0 = _mm_mul_ps(det, minor0);
	_mm_storel_pi((__m64*)(src), minor0);
	_mm_storeh_pi((__m64*)(src+2), minor0);
	minor1 = _mm_mul_ps(det, minor1);
	_mm_storel_pi((__m64*)(src+4), minor1);
	_mm_storeh_pi((__m64*)(src+6), minor1);
	minor2 = _mm_mul_ps(det, minor2);
	_mm_storel_pi((__m64*)(src+ 8), minor2);
	_mm_storeh_pi((__m64*)(src+10), minor2);
	minor3 = _mm_mul_ps(det, minor3);
	_mm_storel_pi((__m64*)(src+12), minor3);
	_mm_storeh_pi((__m64*)(src+14), minor3);
}
#endif /* MATHS_SSE */

#ifdef MSCVER
#pragma warning( pop )
#endif /* MSCVER */

void Matrix4Inverse( float *m16 )
{
#ifdef MATHS_SSE
	PIII_Inverse_4x4(m16);
#else
	//throw std::exception();
	Inverse_4x4<float>(m16);
#endif /* MATHS_SSE */
}


#include <iostream>
void Matrix4InverseTest( void )
{
	//float mat[] = { 0.25f, 0.0f, 0.0f, 10.0f,
	//				0.0f,  0.5f, 0.0f, 10.0f,
	//				0.0f,  0.0f, 1.0f, 10.0f,
	//				0.0f,  0.0f, 0.0f,  1.0f };
	//Matrix4f m = Matrix4f(mat);
	//Matrix4f mi = m.GetInverse();
	//ConsoleMessage( "%f	%f	%f	%f\n"
	//				"%f	%f	%f	%f\n"
	//				"%f	%f	%f	%f\n"
	//				"%f	%f	%f	%f",
	//				mi[0], mi[1], mi[2], mi[3],
	//				mi[4], mi[5], mi[6], mi[7],
	//				mi[8], mi[9], mi[10], mi[11],
	//				mi[12], mi[13], mi[14], mi[15] );
	//std::allocator<Matrix4> asd;
}

void Matrix4MultTest( void )
{
	//float mat[] = { 0.999f, 0.0f, 0.0f, 0.01f,
	//				0.0f,  0.999f, 0.0f, 0.01f,
	//				0.0f,  0.0f, 1.0f, 0.01f,
	//				0.0f,  0.0f, 0.0f,  1.0f };
	//Matrix4f m1(mat);
	//Matrix4f m2(m1);

	//DWORD start = timeGetTime();
	//UINT i = 0;
	//while( i < 10*1000*1000 ) { m1 = m1 * m2; i++; }
	//

	//DWORD dur = timeGetTime() - start;

	//ConsoleMessage( "10 Mil. matrix multiplications needed %i msecs.", int(dur) );
}



template<typename T>
bool CRay_t<T>::intersectSphere( const Vector3<T> &sphereCenter, T radius,
	T distMax, T &distHit, bool bDiscardInner ) const
{
	return RaySphereIntersect<T>(sphereCenter, radius, orig, dir,
		distMax, distHit, bDiscardInner);
}

template<typename T>
bool CRay_t<T>::intersectAABB( const AABoundingBox_t<T> &box, T &distHit, T distMax ) const {
	return RayAABBIntersect<T>(orig, dir, box.min, box.max, distHit, distMax);
}


template class CRay_t<float>;
template class CRay_t<double>;
