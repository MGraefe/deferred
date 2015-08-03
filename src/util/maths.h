// util/maths.h
// util/maths.h
// util/maths.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__maths_H__
#define deferred__util__maths_H__

typedef unsigned short int USHORT;
typedef unsigned int UINT;
typedef short int SHORT;
typedef unsigned char BYTE;

//#define MATHS_SSE

#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>
#include <cmath>
#include <cstdlib>
#include <ostream>
#include <istream>
#include <algorithm>
#include <vector>
#include <cstring>
#include "error.h"
#include "CAlignedAllocator.h"

#undef max
#undef min

inline bool IsAligned( void *ptr, unsigned int align ) {
	Assert( (align & (align-1)) == 0 ); //power of two?
	return (reinterpret_cast<unsigned int>(ptr) & (align-1)) == 0;
}

#ifdef MATHS_SSE
	#include <xmmintrin.h>
	#define ALIGN_16 __declspec(align(16))
	#define ASSERT_ALIGNMENT(bound) \
		AssertMsg( IsAligned(this,bound), "The Data Type is not aligned!!" )

	#define DECLARE_ALIGNED(classname) \
		public: \
			typedef CAlignedAllocator<classname> Allocator; \
			void *classname::operator new(size_t bytes) { \
				return aligned_malloc(bytes); \
			} \
			void *classname::operator new (size_t size, void *p) { \
				return p; \
			} \
			void *classname::operator new[](size_t bytes) { \
				AssertMsg( sizeof(classname) % __alignof(classname) == 0, \
					"Trying to create an array of members " \
					"with a size that cant be divided by the alignment for this class." \
					"Prepare for alignment errors and/or crashs." ); \
				return aligned_malloc(bytes); \
			} \
			void classname::operator delete( void* ptr ) { \
				aligned_free(ptr); \
			} \
			void classname::operator delete( void *ptr1, void *ptr2 ) \
			{ \
				aligned_free(ptr1); \
			} \
			void classname::operator delete[]( void *ptr ) { \
				aligned_free(ptr); \
			}

#else
	#define ALIGN_16
	#define ASSERT_ALIGNMENT(bound)
	#define DECLARE_ALIGNED(classname) \
		public: \
		typedef std::allocator<classname> Allocator;
#endif



#include "mmult.h"

static const float float_null(0.0f);

template<typename T>
inline int ROUND(T a) {
	return a > 0.0f ? (int)(a+0.5f) : (int)(a-0.5f);
}
#define CLAMP(a,min,max) ((a) < (min) ? (min) :( (a) > (max) ? (max) : (a) ) )
#define WRAP(a,min,max) ( (a) % ((max)-(min)+1) + (min) )

#define ASIN_MAX_HALF 0.499999f

#define PI 3.141592653589793f
#define SIN(a) (sin((a)*(PI/180.0f)))
#define COS(a) (cos((a)*(PI/180.0f)))
#define TAN(a) (tan((a)*(PI/180.0f)))
#define ASIN(a) (asin(a)*(180.0f/PI))
#define ACOS(a)	(acos(a)*(180.0f/PI))
#define ATAN(a)	(atan(a)*(180.0f/PI))
#define ATAN2(a,b) (atan2((a),(b))*(180.0f/PI))
#define RAD2DEG(r) ((r)*(180.0f/PI))
#define DEG2RAD(a) ((a)*(PI/180.0f))

inline float round(float v)
{
	return v > 0.0f ? floor(v + 0.5f) : ceil(v - 0.5f);
}

inline double round(double v)
{
	return v > 0.0 ? floor(v + 0.5) : ceil(v - 0.5);
}

template<typename T>
inline const T &max( const T &a, const T &b )
{
	return std::max<T>(a,b);
}

template<typename T>
inline const T &min( const T &a, const T &b )
{
	return std::min<T>(a,b);
}


inline float RoundToNearestMultiple( float val, float multiple )
{
	if( multiple != 0.0f )
		return floor(val / multiple + 0.5f) * multiple;
	else
		return val;
}

inline double RoundToNearestMultiple( double val, double multiple )
{
	if( multiple != 0.0 )
		return floor(val / multiple + 0.5) * multiple;
	else
		return val;
}


static const float MAX_FLOAT = std::numeric_limits<float>::max();
static const float FLOAT_MAX = MAX_FLOAT;
static const float MIN_FLOAT = std::numeric_limits<float>::min();
static const float FLOAT_MIN = MIN_FLOAT;
static const float INFINITY_FLOAT = std::numeric_limits<float>::infinity();
static const float FLOAT_INFINITY = INFINITY_FLOAT;

struct rendercoords_t
{
	float x1;
	float y1;
	float x2;
	float y2;
};


template<typename T>
inline T rsqrt( T a )
{
	return T(1) / sqrt(a);
}

#ifdef MATHS_SSE
inline float rsqrt( float a )
{
	//dangerous but fast
	float ret;
	_mm_store_ss(&ret , _mm_rsqrt_ss( _mm_set_ss(a) ) );
	return ret;
}
#endif

inline const unsigned long& FloatBits( const float& f )
{
	return *reinterpret_cast<const unsigned long*>(&f);
}

template<typename T>
inline bool isNaN( T t )
{
	//dont remove this even if it seems useless,
	//comparison with NaN always results in false
	return t != t;
}

template<typename T>
inline bool isInfinite( T t )
{
	return std::numeric_limits<T>::has_infinity &&
		(t == std::numeric_limits<T>::infinity() ||
		t == -std::numeric_limits<T>::infinity() );
}

//This checks for +/- Infinitiy and NaN
template<typename T>
inline bool IsFinite( T f )
{
	return !isNaN(f) && !isInfinite(f);
}

template<typename T>
inline bool FloatEqual( T val, T parentRef, T maxDifference )
{
	return val >= (parentRef-maxDifference) && val <= (parentRef+maxDifference);
}


template<typename T>
inline T absMax( const T &a, const T &b )
{
	return abs(a) > abs(b) ? a : b;
}

template<typename T>
inline T absMin( const T &a, const T &b )
{
	return abs(a) < abs(b) ? a : b;
}

inline float sign( float f )
{
	return f >= 0.0f ? 1.0f : -1.0f;
}

inline int sign( int i )
{
	return i >= 0 ? 1 : -1;
}
//template<typename T>
//inline T sign( const T &v )
//{
//	if( v > 0 )
//		return 1;
//	else
//		return -1;
//}

void Matrix4Inverse( float *m16 );
void Matrix4InverseTest( void );
void Matrix4MultTest( void );



//////////////////////////////////////////////////////////////////////////
//	Complex number
//////////////////////////////////////////////////////////////////////////
template<typename T>
class ComplexN
{
public:
	ComplexN() {}
	ComplexN(T r, T i) : r(r), i(i) {}
	
	inline ComplexN &operator=(const ComplexN &v) {
		r = v.r;
		i = v.i;
		return *this;
	}

	inline ComplexN GetConj() const {
		return ComplexN(r, -i);
	}

	inline ComplexN operator-() const {
		return ComplexN(-r, -i);
	}

	inline ComplexN operator+(const ComplexN &v) const {
		return ComplexN(r+v.r, i+v.i);
	}

	inline ComplexN &operator+=(const ComplexN &v) {
		r += v.r;
		i += v.i;
		return *this;
	}

	inline ComplexN operator-(const ComplexN &v) const {
		return ComplexN(r-v.r, i-v.i);
	}

	inline ComplexN &operator-=(const ComplexN &v) {
		r -= v.r;
		i -= v.i;
		return *this;
	}

	inline ComplexN operator*(const ComplexN &v) const {
		return ComplexN( r*v.r - i*v.i, r*v.i + i*v.r );
	}

	inline ComplexN &operator*=(const ComplexN &v) {
		*this = *this * v;
	}

	//returns exp(v)
	inline static ComplexN exp(const ComplexN &v) {
		T er = ::exp(v.r);
		return ComplexN(er*cos(v.i), er*sin(v.i));
	}

	//returns exp(i*imag);
	static ComplexN expImag(T imag) {
		return ComplexN(cos(imag), sin(imag));
	}

public:
	T r;
	T i;
};

typedef ComplexN<double> ComplexNd;
typedef ComplexN<float> ComplexNf;



//////////////////////////////////////////////////////////////////////////
//	Vector 2d 
//////////////////////////////////////////////////////////////////////////
template<typename T>
class Vector2
{
public:
	Vector2() : x(T(0)), y(T(0)) {	}
	Vector2( T X, T Y ) { x = X; y = Y; }
	Vector2( T *a ) { x = a[0]; y = a[1]; }
	Vector2( int X, int Y ) { x = (T)X; y = (T)Y; }

public:
	inline void Init( T *a )
	{ 
		x = a[0]; 
		y = a[1]; 
	}

	inline void Init( T X, T Y ) 
	{ 
		x = X; 
		y = Y; 
	}

	inline void Init( const Vector2<T> &v ) 
	{ 
		x = v.x; 
		y = v.y; 
	}

	inline T length( void ) const
	{
		return sqrt( x*x + y*y );
	}

	inline Vector2<T> &Normalize( void )
	{
		T rl = rsqrt( x*x + y*y );
		if( !isNaN(rl) )
		{
			x *= rl;
			y *= rl;
		}
		return *this;
	}

	inline Vector2<T> GetNormalized( void ) const
	{
		T rl = rsqrt( x*x + y*y );
		if( rl == rl )
			return Vector2<T>( x*rl, y*rl );
		else
			return *this;
	}

	inline T Dot( const Vector2<T> &v ) const
	{
		return x*v.x + y*v.y;
	}

	inline Vector2<T> GetRotate( T deg ) const
	{
		T s = SIN(deg);
		T c = COS(deg);
		return Vector2<T>(	c*x - s*y,
			s*x + c*y );
	}

	inline Vector2<T> operator+(const Vector2<T>& v) const
	{
		return Vector2<T>( x + v.x, y + v.y ); 
	}

	inline Vector2<T> operator+(T a) const
	{
		return Vector2<T>( x + a, y + a ); 
	}

	inline Vector2<T> operator-(const Vector2<T>& v) const
	{
		return Vector2<T>( x - v.x, y - v.y ); 
	}

	inline Vector2<T> operator*(T a) const
	{
		return Vector2<T>( x * a, y * a ); 
	}

	inline Vector2<T> operator/(T a) const
	{
		return Vector2<T>( x / a, y / a ); 
	}

	inline Vector2<T>& operator=(const Vector2<T>& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	inline bool operator!=(const Vector2<T>& v)
	{
		return ( x != v.x || y != v.y );
	}

	inline bool operator==(const Vector2<T>& v)
	{
		return ( x == v.x && y == v.y );
	}

	inline Vector2<T>& operator+=( const Vector2<T> &v )
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	inline Vector2<T>& operator+=( T i )
	{
		x += i;
		y += i;
		return *this;
	}

	inline Vector2<T>& operator-=( const Vector2<T> &v )
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	inline Vector2<T>& operator*=(T a)
	{
		x *= a;
		y *= a;
		return *this;
	}

	inline Vector2<T>& operator/=(T a)
	{
		x /= a;
		y /= a;
		return *this;
	}

	inline T operator[](size_t index) const
	{
		return ((T*)this)[index];
	}

	inline T &operator[](size_t index)
	{
		return ((T*)this)[index];
	}

public:
	T x;
	T y;
};

typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;

const Vector2f vec2_null(0.0f, 0.0f);


//////////////////////////////////////////////////////////////////////////
//	Matrix 2d
//////////////////////////////////////////////////////////////////////////
//Column major!
class Matrix2
{
public:
	Matrix2() : v00(1.0f), v10(0.0f), v01(0.0f), v11(1.0f) { }
	Matrix2( float V00, float V01, float V10, float V11 ) :
		v00(V00), v10(V01), v01(V10), v11(V11) { }
	//Column major!
	Matrix2( float *vals ) : v00(vals[0]), v10(vals[1]), v01(vals[2]), v11(vals[3]) { }
	Matrix2( float rotationRads ) {
		float c = cos(rotationRads);
		float s = sin(rotationRads);
		v00 = c; v01 = -s;
		v10 = s; v11 = c;
	}

	inline Vector2f operator*(const Vector2f &vec) const {
		return Vector2f( v00*vec.x + v01*vec.y, 
						 v10*vec.x + v11*vec.y );
	}

	inline Matrix2 operator*(const Matrix2 &m ) const {
		return Matrix2( v00*m.v00 + v01*m.v10, v00*m.v01 + v01*m.v11,
						v10*m.v00 + v11*m.v10, v10*m.v01 + v11*m.v11 );
	}

public:
	//Column major!
	union {
		float v[4];
		struct {float v00, v10, v01, v11;};
	};
};

//////////////////////////////////////////////////////////////////////////
//	Vector 3d 
//////////////////////////////////////////////////////////////////////////
//#define VECTOR3_PARANOIA
#ifdef VECTOR3_PARANOIA
#define CHECK_VECTOR3() Assert(IsValid())
#else
#define CHECK_VECTOR3()
#endif

template<typename T>
class Vector3
{
public:
	Vector3() : 
		x( T(0) ), y( T(0) ), z( T(0) )
	{}
	
	Vector3( T a ) : 
		x(a), y(a), z(a) 
	{
		CHECK_VECTOR3();
	}
	
	Vector3( T X, T Y, T Z )
		: x(X), y(Y), z(Z)
	{
		CHECK_VECTOR3();
	}
	
	Vector3( const T* const a )
	{
		x = a[0]; y = a[1]; z = a[2];
	}
	
	Vector3( const Vector3<T> &other )
	{
		x = other.x; y = other.y; z = other.z;
		CHECK_VECTOR3();
	}

public:
	//Returns (0,0,0)
	inline static Vector3<T> null(void) {
		return Vector3<T>(T(0));
	}

	//Assign from other template-parameter-type
	//Use like this:
	// Vector3<float> foo;
	// bar.setFromOther<float>(foo);
	template<typename U>
	inline Vector3 &setFromOther(const Vector3<U> &vec) {
		x = (T)vec.x;
		y = (T)vec.y;
		z = (T)vec.z;
		CHECK_VECTOR3();
		return *this;
	}

	//Gets vector of other template-parameter-type
	//Use like this:
	// Vector3<float> = someVector.getOther<float>();
	template<typename U>
	inline Vector3<U> getOther(void) const {
		return Vector3<U>( U(x), U(y), U(z) );
	}

	//Converts to float-vector
	inline Vector3<float> toVec3f( void ) const {
		return getOther<float>();
	}

	//Converts to double-vector
	inline Vector3<double> toVec3d( void ) const {
		return getOther<double>();
	}

	//Gets normalized vector
	inline Vector3 GetNormalized( void ) const {
		return Vector3(*this).Normalize();
	}

	//makes absolute in place
	inline Vector3 &makeAbs( void ) {
		x = ::abs(x);
		y = ::abs(y);
		z = ::abs(z);
		return (*this);
	}

	//returns component-wise absolute
	inline Vector3 abs( void ) const {
		return Vector3( ::abs(x), ::abs(y), ::abs(z) );
	}

	//returns ||this||
	inline T Length( void ) const {
		return sqrt( x*x + y*y + z*z );
	}

	//returns ||this||^2 = this.Dot(this);
	inline T LengthSq( void ) const {
		return x*x + y*y + z*z;
	}

	//returns ||vec-this||
	inline T distTo(const Vector3 &vec) const {
		return (*this-vec).Length();
	}

	//returns ||vec-this||^2
	inline T distToSq(const Vector3 &vec) const {
		return (*this-vec).LengthSq();
	}

	//returns <this, v>
	inline T Dot( const Vector3 &v ) const {
		return x*v.x + y*v.y + z*v.z;
	}

	//returns abs(x*v.x)+abs(y*v.y)+abs(z*v.z)
	T AbsDotComponentWise( const Vector3 &v ) const {
		return ::abs(x*v.x) + ::abs(y*v.y) + ::abs(z*v.z);
	}

	//Cross product
	inline Vector3 Cross( const Vector3 &v ) const {
		return Vector3(	( y * v.z ) - ( z * v.y ),
							( z * v.x ) - ( x * v.z ),
							( x * v.y ) - ( y * v.x ) );
	}

	//returns true if no component is +/- infinity or NaN
	inline bool IsValid( void ) const {
		return (IsFinite(x) && IsFinite(y) && IsFinite(z));
	}

	//non-constant array-access operator
	inline T& operator[](int i) {
		return ((T*)this)[i];
	}

	//Constent array-access operator
	inline T operator[](int i) const {
		return ((T*)this)[i];
	}

	//Add Vectors
	inline Vector3 operator+(const Vector3& v) const {
		return Vector3( x + v.x, y + v.y, z + v.z );
	}

	//Add a to all components
	inline Vector3 operator+(const T a) const {
		return Vector3( x + a, y + a, z + a ); 
	}

	//Subtract vector
	inline Vector3 operator-(const Vector3& v) const {
		return Vector3( x - v.x, y - v.y, z - v.z ); 
	}

	//Return negative
	inline Vector3 operator-( void ) const {
		return Vector3( -x, -y, -z );
	}

	//Subtract a from all components
	inline Vector3 operator-(const T a) const {
		return Vector3( x - a, y - a, z - a ); 
	}

	//Multiply all components by a
	inline Vector3 operator*(const T a) const {
		return Vector3( x * a, y * a, z * a );
	}

	inline Vector3 operator*(const Vector3 &v) const {
		return Vector3( x * v.x, y * v.y, z * v.z );
	}

	//Divide all components by a
	inline Vector3 operator/( const T a) const {
		T f = 1.0f / a;
		return Vector3( x * f, y * f, z * f ); 
	}

	//divide component wise
	inline Vector3 operator/(const Vector3 &v) const {
		return Vector3( x / v.x, y / v.y, z / v.z );
	}

	//Assign component-wise
	inline Vector3& operator=(const Vector3& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		CHECK_VECTOR3();
		return *this;
	}

	//Assign a to all components
	inline Vector3& operator=(const T a) {
		x = a;
		y = a;
		z = a;
		CHECK_VECTOR3();
		return *this;
	}

	//Compare component-wise
	inline bool operator!=(const Vector3& v) const {
		return ( x != v.x || y != v.y || z != v.z );
	}

	//Compare component-wise
	inline bool operator==(const Vector3& v) const {
		return ( x == v.x && y == v.y && z == v.z );
	}

	//Compare component-wise
	inline bool operator> (const Vector3& v) const {
		return ( x > v.x && y > v.y && z > v.z );
	}

	//Compare component-wise
	inline bool operator>=(const Vector3& v) const {
		return ( x >= v.x && y >= v.y && z >= v.z );
	}

	//Compare component-wise
	inline bool operator< (const Vector3& v) const {
		return ( x < v.x && y < v.y && z < v.z );
	}

	//Compare component-wise
	inline bool operator<=(const Vector3& v) const {
		return ( x <= v.x && y <= v.y && z <= v.z );
	}

	//add vector
	inline Vector3& operator+=( const Vector3 &v ) {
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	//add i to all components
	inline Vector3& operator+=( T i ) {
		x += i;
		y += i;
		z += i;
		return *this;
	}

	//subtract vector
	inline Vector3& operator-=( const Vector3 &v ) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	//Multiply all components by a
	inline Vector3& operator*=( const T a ) {
		x *= a;
		y *= a;
		z *= a;
		return *this;
	}

	//Mulitply all components component wise in place
	inline Vector3& operator *=( const Vector3 &v ) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	//Divide all components by a
	inline Vector3& operator/=( const T a ) {
		T f = 1.0f/a;
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	//Divide all components component wise in place
	inline Vector3& operator/=( const Vector3 &v ) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	//Reziproc length
	inline T rlength(void) const {
		return rsqrt( x*x + y*y + z*z );
	}

	//Normalize in place
	inline Vector3& Normalize( void ) {
		T rl = rlength();
		if( rl == rl )
		{
			x *= rl;
			y *= rl;
			z *= rl;
		}
		return *this;
	}

	//Assign by values
	inline void	Init( T X, T Y, T Z ) {
		x = X;
		y = Y;
		z = Z;
		Assert( IsValid() );
	}

	//Assign by array of floats.
	//Array has at least 3 elements
	inline void Init( const T* const a ) {
		x = a[0];
		y = a[1];
		z = a[2];
		CHECK_VECTOR3();
	}

	//Assign vector3
	inline void	Init( const Vector3 &vec ) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
		CHECK_VECTOR3();
	}

	//returns (min(x, v.x), min(y, v.y), min(z, v.z))
	inline Vector3 MinComponentWise( const Vector3 &v ) const {
		return Vector3( ::min(x, v.x), ::min(y, v.y), ::min(z, v.z) );
	} 

	//returns (max(x, v.x), max(y, v.y), max(z, v.z))
	inline Vector3 MaxComponentWise( const Vector3 &v ) const {
		return Vector3( ::max(x, v.x), ::max(y, v.y), ::max(z, v.z) );
	} 


	//Returns the index of the axis in which this vector extends most
	//returns -1 if no major axis is found (0,0,0)
	inline int GetMajorAxis( void ) const {
		Vector3<T> absVec(this->abs());
		T max = T(0);
		int index = -1;

		for( int i = 0; i < 3; i++ )
		{
			if( absVec[i] > max )
			{
				max = absVec[i];
				index = i;
			}
		}
		return index;
	}


	//Return a Vector perpendicular to this one.
	//The Vector returned might point into any direction and might be of any length (NOT normalized),
	//the only assertion you can make is that it is perpendicular (dot-product = 0) to this vector
	//Returns (0,0,0) if this vector is (0,0,0)
	inline Vector3 GetPerpendicularVector(void) const {
		Vector3<T> a(T(0), T(0), T(0));
		int axisIndex = GetMajorAxis();
		if( axisIndex < 0 )
			return a;
		a[(axisIndex+1)%3] = T(1);

		//Make new vector perpendicular by removing non-perpendicular fraction,
		//This is called Vector rejection
		//http://en.wikipedia.org/w/index.php?title=Vector_projection&oldid=506911455#Vector_rejection_2
		const Vector3<T> &b = *this;
		return a - b * (a.Dot(b) / b.Dot(b));
	}

	inline Vector3 pow(T e) const {
		return Vector3(::pow(x, e), ::pow(y, e), ::pow(z, e));
	}

public:
	T x;
	T y;
	T z;
};

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;
typedef Vector3f Angle3d;

const Vector3f vec3_null(0.0f,0.0f,0.0f);
const Vector3f vec3f_null(0.0f,0.0f,0.0f);
const Vector3d vec3d_null(0.0,0.0,0.0);

template<typename T>
inline Vector3<T> abs( const Vector3<T> &v )
{
	return v.abs();
}



//Vector serialization
template<typename T>
inline std::ostream& operator<<( std::ostream &os, const Vector3<T> &val )
{
	return os.write((const char*)&val.x, sizeof(T)*3);
}

//Vector deserialization
template<typename T>
inline std::istream& operator>>( std::istream &is, Vector3<T> &val )
{
	return is.read((char*)&val.x, sizeof(T)*3);
}

//wraps degrees to (-360, 360)
inline float wrapDegrees(float deg)
{
	//return fmod( deg, 360.0f );
	return deg - 360.0f * floor(deg/360.0f);
}

inline void wrapDegrees(Angle3d &ang)
{
	ang.x = wrapDegrees(ang.x);
	ang.y = wrapDegrees(ang.y);
	ang.z = wrapDegrees(ang.z);
}


const float angle3dCompressed_fact = 180.0f; //360*180 = 64.800 < 2^16-1 = 65.535
const float angle3dCompressed_fact_r = 1.0f/180.0f;
class Angle3dCompressed
{
public:
	Angle3dCompressed() {}
	Angle3dCompressed(const Angle3d &ang)
	{
		Assert(ang.IsValid());
		x = (USHORT)(wrapDegrees(ang.x) * angle3dCompressed_fact + 0.5f);
		y = (USHORT)(wrapDegrees(ang.y) * angle3dCompressed_fact + 0.5f);
		z = (USHORT)(wrapDegrees(ang.z) * angle3dCompressed_fact + 0.5f);
	}

	inline Angle3d toAngle()
	{
		return Angle3d(	((float)x)*angle3dCompressed_fact_r,
						((float)y)*angle3dCompressed_fact_r,
						((float)z)*angle3dCompressed_fact_r );
	}

public:
	USHORT x, y, z;
	static const size_t sNetSize = sizeof(USHORT)*3;
};


class Vector3fCompressed
{
public: 
	Vector3fCompressed() {}
	Vector3fCompressed(const Vector3f &vec, float maxRange)
	{
		Assert(vec.IsValid());
		float prec_r = 32767.0f / maxRange;
		x = (SHORT)(vec.x * prec_r + 0.5f);
		y = (SHORT)(vec.y * prec_r + 0.5f);
		z = (SHORT)(vec.z * prec_r + 0.5f);
	}

	inline Vector3f toVector(float maxRange)
	{
		float prec = maxRange / 32767.0f;
		return Vector3f( ((float)x)*prec, ((float)y)*prec, ((float)z)*prec );
	}
public:
	SHORT x, y, z;
};


//#define clamp(a,low,up) ((a) < (low)? (low): (a) > (up)? (up): (a))
template<typename T>
inline T clamp(T val, T min, T max)
{
	if( val < min )
		return min;
	else if( val > max )
		return max;
	else
		return val;
}


// Remap a value in the range [a,b] to [c,d].
template<typename T>
inline T lerp( T val, T a, T b, T c, T d)
{
	return c + (d - c) * (val - a) / (b - a);
}

// Remap a value in the range [a,b] to [c,d] and clamp the val to [c,d]
template<typename T>
inline T lerpclamped( T val, T a, T b, T c, T d )
{
	float lerped = lerp(val, a, b, c, d);
	if( c < d )
		return clamp( lerped, c, d );
	else
		return clamp( lerped, d, c );
}

//#define EPSILON 0.000001f;

class Math
{
public:
    static const float EPSILON;

    static bool closeEnough(float f1, float f2)
    {
        // Determines whether the two floating-point values f1 and f2 are
        // close enough together that they can be considered equal.

        return fabsf((f1 - f2) / ((f2 == 0.0f) ? 1.0f : f2)) < EPSILON;
    }
};







//-----------------------------------------------------------------------------
// A row-major 3x3 matrix class.
//
// Matrices are concatenated in a left to right order.
// Multiplies vectors to the left of the matrix.
//-----------------------------------------------------------------------------
class Matrix3
{
    friend Vector3f operator*(const Vector3f &lhs, const Matrix3 &rhs);
    friend Matrix3 operator*(float scalar, const Matrix3 &rhs);

public:
    static const Matrix3 IDENTITY;

    Matrix3() {}
	Matrix3( const float *fv ) {
		std::memcpy(mtx, fv, sizeof(float)*9);
	}

    Matrix3(float m11, float m12, float m13,
            float m21, float m22, float m23,
            float m31, float m32, float m33);
    ~Matrix3() {}

    float *operator[](int row);
    const float *operator[](int row) const;

    bool operator==(const Matrix3 &rhs) const;
    bool operator!=(const Matrix3 &rhs) const;

	Matrix3 &operator=(float *f4x4Matrix);

    Matrix3 &operator+=(const Matrix3 &rhs);
    Matrix3 &operator-=(const Matrix3 &rhs);
    Matrix3 &operator*=(const Matrix3 &rhs);
    Matrix3 &operator*=(float scalar);
    Matrix3 &operator/=(float scalar);

    Matrix3 operator+(const Matrix3 &rhs) const;
    Matrix3 operator-(const Matrix3 &rhs) const;
    Matrix3 operator*(const Matrix3 &rhs) const;
    Matrix3 operator*(float scalar) const;
	Vector3f operator*(const Vector3f &v) const;
    Matrix3 operator/(float scalar) const;

    float determinant() const;
    void identity();
    Matrix3 GetInverse() const;
    Matrix3 GetTranspose() const;
	Matrix3 &transpose();

	void ToOpenGlMatrix( float *mat, const Vector3f &vOffset = vec3_null ) const;
	void ToOpenGlMatrixTranspose( float *mat, const Vector3f &vOffset = vec3_null ) const;

	Vector3f GetDirectionVector( void );
	void GetRightUpForwardVectors( Vector3f &vRight, Vector3f &vUp, Vector3f &vForward ) const;

private:
    float mtx[3][3];
};

inline Vector3f Matrix3::GetDirectionVector( void )
{
	return Vector3f( -mtx[0][2], -mtx[1][2], -mtx[2][2] );
}

inline void Matrix3::GetRightUpForwardVectors( Vector3f &vRight, Vector3f &vUp, Vector3f &vForward ) const
{
	vRight.x =		mtx[0][0];
	vRight.y =		mtx[0][1];
	vRight.z =		mtx[0][2];
	vUp.x =			mtx[1][0];
	vUp.y =			mtx[1][1];
	vUp.z =			mtx[1][2];
	vForward.x =	-mtx[2][0];
	vForward.y =	-mtx[2][1];
	vForward.z =	-mtx[2][2];
}


inline void Matrix3::ToOpenGlMatrix( float *mat, const Vector3f &vOffset ) const
{
	mat[0] = mtx[0][0]; mat[1] = mtx[0][1]; mat[2] = mtx[0][2]; mat[3] = 0.0f;
	mat[4] = mtx[1][0]; mat[5] = mtx[1][1]; mat[6] = mtx[1][2]; mat[7] = 0.0f;
	mat[8] = mtx[2][0]; mat[9] = mtx[2][1]; mat[10] = mtx[2][2]; mat[11] = 0.0f;
	mat[12] = vOffset.x; mat[13] = vOffset.y; mat[14] = vOffset.z; mat[15] = 1.0f;
}

inline void Matrix3::ToOpenGlMatrixTranspose( float *mat, const Vector3f &vOffset ) const
{
	mat[0] = mtx[0][0]; mat[1] = mtx[1][0]; mat[2] = mtx[2][0]; mat[3] = 0.0f;
	mat[4] = mtx[0][1]; mat[5] = mtx[1][1]; mat[6] = mtx[2][1]; mat[7] = 0.0f;
	mat[8] = mtx[0][2]; mat[9] = mtx[1][2]; mat[10] = mtx[2][2]; mat[11] = 0.0f;
	mat[12] = vOffset.x; mat[13] = vOffset.y; mat[14] = vOffset.z; mat[15] = 1.0f;
}


inline Vector3f operator*(const Vector3f &lhs, const Matrix3 &rhs)
{
    return Vector3f((lhs.x * rhs.mtx[0][0]) + (lhs.y * rhs.mtx[1][0]) + (lhs.z * rhs.mtx[2][0]),
					(lhs.x * rhs.mtx[0][1]) + (lhs.y * rhs.mtx[1][1]) + (lhs.z * rhs.mtx[2][1]),
					(lhs.x * rhs.mtx[0][2]) + (lhs.y * rhs.mtx[1][2]) + (lhs.z * rhs.mtx[2][2]));
}

inline Vector3f Matrix3::operator*(const Vector3f &v) const
{
	return Vector3f(	mtx[0][0]*v.x + mtx[0][1]*v.y + mtx[0][2]*v.z,
						mtx[1][0]*v.x + mtx[1][1]*v.y + mtx[1][2]*v.z,
						mtx[2][0]*v.x + mtx[2][1]*v.y + mtx[2][2]*v.z );
}

inline Matrix3 operator*(float scalar, const Matrix3 &rhs)
{
    return rhs * scalar;
}

inline Matrix3::Matrix3(float m11, float m12, float m13,
                        float m21, float m22, float m23,
                        float m31, float m32, float m33)
{
    mtx[0][0] = m11, mtx[0][1] = m12, mtx[0][2] = m13;
    mtx[1][0] = m21, mtx[1][1] = m22, mtx[1][2] = m23;
    mtx[2][0] = m31, mtx[2][1] = m32, mtx[2][2] = m33;
}

inline float *Matrix3::operator[](int row)
{
    return mtx[row];
}

inline const float *Matrix3::operator[](int row) const
{
    return mtx[row];
}


inline Matrix3 &Matrix3::operator=(float *f4x4Matrix)
{
	mtx[0][0] = f4x4Matrix[0];
	mtx[1][0] = f4x4Matrix[1];
	mtx[2][0] = f4x4Matrix[2];

	mtx[0][1] = f4x4Matrix[4];
	mtx[1][1] = f4x4Matrix[5];
	mtx[2][1] = f4x4Matrix[6];

	mtx[0][2] = f4x4Matrix[8];
	mtx[1][2] = f4x4Matrix[9];
	mtx[2][2] = f4x4Matrix[10];

	return *this;
}

inline bool Matrix3::operator==(const Matrix3 &rhs) const
{
    return Math::closeEnough(mtx[0][0], rhs.mtx[0][0])
        && Math::closeEnough(mtx[0][1], rhs.mtx[0][1])
        && Math::closeEnough(mtx[0][2], rhs.mtx[0][2])
        && Math::closeEnough(mtx[1][0], rhs.mtx[1][0])
        && Math::closeEnough(mtx[1][1], rhs.mtx[1][1])
        && Math::closeEnough(mtx[1][2], rhs.mtx[1][2])
        && Math::closeEnough(mtx[2][0], rhs.mtx[2][0])
        && Math::closeEnough(mtx[2][1], rhs.mtx[2][1])
        && Math::closeEnough(mtx[2][2], rhs.mtx[2][2]);
}

inline bool Matrix3::operator!=(const Matrix3 &rhs) const
{
    return !(*this == rhs);
}

inline Matrix3 &Matrix3::operator+=(const Matrix3 &rhs)
{
    mtx[0][0] += rhs.mtx[0][0], mtx[0][1] += rhs.mtx[0][1], mtx[0][2] += rhs.mtx[0][2];
    mtx[1][0] += rhs.mtx[1][0], mtx[1][1] += rhs.mtx[1][1], mtx[1][2] += rhs.mtx[1][2];
    mtx[2][0] += rhs.mtx[2][0], mtx[2][1] += rhs.mtx[2][1], mtx[2][2] += rhs.mtx[2][2];
    return *this;
}

inline Matrix3 &Matrix3::operator-=(const Matrix3 &rhs)
{
    mtx[0][0] -= rhs.mtx[0][0], mtx[0][1] -= rhs.mtx[0][1], mtx[0][2] -= rhs.mtx[0][2];
    mtx[1][0] -= rhs.mtx[1][0], mtx[1][1] -= rhs.mtx[1][1], mtx[1][2] -= rhs.mtx[1][2];
    mtx[2][0] -= rhs.mtx[2][0], mtx[2][1] -= rhs.mtx[2][1], mtx[2][2] -= rhs.mtx[2][2];
    return *this;
}

inline Matrix3 &Matrix3::operator*=(const Matrix3 &rhs)
{
    Matrix3 tmp;

    // Row 1.
    tmp.mtx[0][0] = (mtx[0][0] * rhs.mtx[0][0]) + (mtx[0][1] * rhs.mtx[1][0]) + (mtx[0][2] * rhs.mtx[2][0]);
    tmp.mtx[0][1] = (mtx[0][0] * rhs.mtx[0][1]) + (mtx[0][1] * rhs.mtx[1][1]) + (mtx[0][2] * rhs.mtx[2][1]);
    tmp.mtx[0][2] = (mtx[0][0] * rhs.mtx[0][2]) + (mtx[0][1] * rhs.mtx[1][2]) + (mtx[0][2] * rhs.mtx[2][2]);

    // Row 2.
    tmp.mtx[1][0] = (mtx[1][0] * rhs.mtx[0][0]) + (mtx[1][1] * rhs.mtx[1][0]) + (mtx[1][2] * rhs.mtx[2][0]);
    tmp.mtx[1][1] = (mtx[1][0] * rhs.mtx[0][1]) + (mtx[1][1] * rhs.mtx[1][1]) + (mtx[1][2] * rhs.mtx[2][1]);
    tmp.mtx[1][2] = (mtx[1][0] * rhs.mtx[0][2]) + (mtx[1][1] * rhs.mtx[1][2]) + (mtx[1][2] * rhs.mtx[2][2]);

    // Row 3.
    tmp.mtx[2][0] = (mtx[2][0] * rhs.mtx[0][0]) + (mtx[2][1] * rhs.mtx[1][0]) + (mtx[2][2] * rhs.mtx[2][0]);
    tmp.mtx[2][1] = (mtx[2][0] * rhs.mtx[0][1]) + (mtx[2][1] * rhs.mtx[1][1]) + (mtx[2][2] * rhs.mtx[2][1]);
    tmp.mtx[2][2] = (mtx[2][0] * rhs.mtx[0][2]) + (mtx[2][1] * rhs.mtx[1][2]) + (mtx[2][2] * rhs.mtx[2][2]);

    *this = tmp;
    return *this;
}

inline Matrix3 &Matrix3::operator*=(float scalar)
{
    mtx[0][0] *= scalar, mtx[0][1] *= scalar, mtx[0][2] *= scalar;
    mtx[1][0] *= scalar, mtx[1][1] *= scalar, mtx[1][2] *= scalar;
    mtx[2][0] *= scalar, mtx[2][1] *= scalar, mtx[2][2] *= scalar;
    return *this;
}

inline Matrix3 &Matrix3::operator/=(float scalar)
{
    mtx[0][0] /= scalar, mtx[0][1] /= scalar, mtx[0][2] /= scalar;
    mtx[1][0] /= scalar, mtx[1][1] /= scalar, mtx[1][2] /= scalar;
    mtx[2][0] /= scalar, mtx[2][1] /= scalar, mtx[2][2] /= scalar;
    return *this;
}

inline Matrix3 Matrix3::operator+(const Matrix3 &rhs) const
{
    Matrix3 tmp(*this);
    tmp += rhs;
    return tmp;
}

inline Matrix3 Matrix3::operator-(const Matrix3 &rhs) const
{
    Matrix3 tmp(*this);
    tmp -= rhs;
    return tmp;
}

inline Matrix3 Matrix3::operator*(const Matrix3 &rhs) const
{
    Matrix3 tmp(*this);
    tmp *= rhs;
    return tmp;
}

inline Matrix3 Matrix3::operator*(float scalar) const
{
    Matrix3 tmp(*this);
    tmp *= scalar;
    return tmp;
}

inline Matrix3 Matrix3::operator/(float scalar) const
{
    Matrix3 tmp(*this);
    tmp /= scalar;
    return tmp;
}

inline float Matrix3::determinant() const
{
    return (mtx[0][0] * (mtx[1][1] * mtx[2][2] - mtx[1][2] * mtx[2][1]))
        - (mtx[0][1] * (mtx[1][0] * mtx[2][2] - mtx[1][2] * mtx[2][0]))
        + (mtx[0][2] * (mtx[1][0] * mtx[2][1] - mtx[1][1] * mtx[2][0]));
}

inline void Matrix3::identity()
{
    mtx[0][0] = 1.0f, mtx[0][1] = 0.0f, mtx[0][2] = 0.0f;
    mtx[1][0] = 0.0f, mtx[1][1] = 1.0f, mtx[1][2] = 0.0f;
    mtx[2][0] = 0.0f, mtx[2][1] = 0.0f, mtx[2][2] = 1.0f;
}

inline Matrix3 Matrix3::GetTranspose() const
{
    Matrix3 tmp;

    tmp[0][0] = mtx[0][0], tmp[0][1] = mtx[1][0], tmp[0][2] = mtx[2][0];
    tmp[1][0] = mtx[0][1], tmp[1][1] = mtx[1][1], tmp[1][2] = mtx[2][1];
    tmp[2][0] = mtx[0][2], tmp[2][1] = mtx[1][2], tmp[2][2] = mtx[2][2];

    return tmp;
}

inline Matrix3 &Matrix3::transpose()
{
	*this = GetTranspose();
	return *this;
}

extern void GetAngleMatrix3x3( const Angle3d &Angle, float *m );
extern void GetAngleMatrix3x3( const Angle3d &Angle, Matrix3 &m );

//-----------------------------------------------------------------------------
//
// A Quaternion
// 
//
//-----------------------------------------------------------------------------
#define QUAT_PARANOIA
#ifdef QUAT_PARANOIA
#define CHECK_OP_BRACKETS Assert( x == (*this)[0] && y == (*this)[1] && z == (*this)[2] && w == (*this)[3] )
#else
#define CHECK_OP_BRACKETS
#endif

class Quaternion
{
public:
	//Quaternion( const Quaternion &q ) : val(q.val) { }
	Quaternion( const float &X, const float &Y, const float &Z, const float &W ) :
		x(X), y(Y), z(Z), w(W) { }
	Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) { }

	Quaternion( const Angle3d &ang )
	{
		SetFromAngle( ang );
	}

	Quaternion( const Vector3f &axis, const float &angle )
	{
		SetFromAxisRot( axis, angle );
	}

	inline bool IsValid( void ) const {
		return (IsFinite(x) && IsFinite(y) && IsFinite(z) && IsFinite(w));
	}

	inline void SetFromAngle( const Angle3d &ang )
	{
		//Quaternion qx( Vector3f(1,0,0), ang.x );
		//Quaternion qy( Vector3f(0,1,0), ang.y );
		//Quaternion qz( Vector3f(0,0,1), ang.z );

		//*this = (qz*qy)*qx;

		float h1 = DEG2RAD( ang.x ) * 0.5f;
		float h2 = DEG2RAD( ang.y ) * 0.5f;
		float h3 = DEG2RAD( ang.z ) * 0.5f;
		float s1 = sinf(h1);
		float s2 = sinf(h2);
		float s3 = sinf(h3);
		float c1 = cosf(h1);
		float c2 = cosf(h2);
		float c3 = cosf(h3);

		x = s1*c2*c3 - c1*s2*s3;
		y = c1*s2*c3 + s1*c2*s3;
		z = c1*c2*s3 - s1*s2*c3;
		w = c1*c2*c3 + s1*s2*s3;

		//Normalize();

		Assert( IsValid() );
	}


	inline Quaternion &Normalize( void )
	{
		float rs = 1.0f/sqrt( x*x + y*y + z*z + w*w );
		Assert( IsFinite(rs) );
		x *= rs;
		y *= rs;
		z *= rs;
		w *= rs;
		Assert(IsValid());
		return *this;
	}

	inline void ToAngle( Angle3d &ang ) const
	{
		//float sqw = w*w;
		float sqx = x*x;
		float sqy = y*y;
		float sqz = z*z;

#if 1
		float test = w*y - z*x;
		if( test > ASIN_MAX_HALF )
		{
			ang.x = 0.0f;
			ang.y = 90.0f;
			ang.z = -2.0f * ATAN2(x,w);
			return;
		}
		if( test < -ASIN_MAX_HALF )
		{
			ang.x = 0.0f;
			ang.y = -90.0f;
			ang.z = 2.0f * ATAN2(x,w);
			return;
		}
#endif
		ang.x = ATAN2( 2.0f*(w*x + y*z), 1.0f - 2.0f*(sqx + sqy) );
		ang.y = ASIN( 2.0f * test );
		ang.z = ATAN2( 2.0f * (w*z + x*y), 1.0f - 2.0f*(sqy + sqz) );

		Assert( ang.IsValid() );
	}

	inline Angle3d ToAngle( void ) const
	{
		Angle3d ang;
		ToAngle(ang);
		return ang;
	}

	inline void SetValue( const float &X, const float &Y, const float &Z, const float &W )
	{
		x = X;
		y = Y;
		z = Z;
		w = W;
		Assert( IsValid() );
	}

	inline void SetValue( const Quaternion &q ) {
		//val = q.val;
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
		Assert( IsValid() );
	}

	inline void SetFromAxisRot( const Vector3f &axis, const float &angle ){
		float ah = DEG2RAD(angle) * 0.5f;
		float sa = sinf(ah);
		float ca = cosf(ah);

		x = sa * axis.x;
		y = sa * axis.y;
		z = sa * axis.z;
		w = ca;

		Assert( IsValid() );
	}

	inline float GetNorm() const {
		return sqrt(x*x + y*y + z*z + w*w);
	}

	inline Quaternion GetInverse() const {
		return Quaternion(*this).invert();
	}

	inline Quaternion &invert() {
		//float norm = GetNorm();
		return conjugate() /= (x*x + y*y + z*z + w*w);
	}

	inline Quaternion		operator+(const Quaternion& v) const {
		return Quaternion( x + v.x, y + v.y, z + v.z, w + v.w );
	}

	inline Quaternion		operator-(const Quaternion& v) const {
		return Quaternion( x - v.x, y - v.y, z - v.z, w - v.w );
	}

	inline Quaternion		operator-( void ) const {
		return Quaternion( -x, -y, -z, -w );
	}

	inline Quaternion		operator*(const float &a) const {
		return Quaternion( x*a, y*a, z*a, w*a );
	}

	inline Quaternion		operator*(const Quaternion &q) const {
		return Quaternion(	w * q.x + x * q.w + y * q.z - z * q.y,
							w * q.y + y * q.w + z * q.x - x * q.z,
							w * q.z + z * q.w + x * q.y - y * q.x,
							w * q.w - x * q.x - y * q.y - z * q.z ); //correct.
	}

	// Multiplying a quaternion q with a vector v applies the q-rotation to v
	inline Vector3f operator* (const Vector3f &v) const
	{
#if 0
		Vector3f vn(v);
		vn.Normalize();
 
		Quaternion q( vn.x, vn.y, vn.z, 0.0f );
		q = *this * (q*GetConjugate());
 
		return Vector3f(q.x, q.y, q.z);
#else
		// nVidia SDK implementation
		Vector3f uv, uuv;
		Vector3f qvec(x, y, z);
		uv = qvec.Cross(v);
		uuv = qvec.Cross(uv);
		uv *= (2.0f * w);
		uuv *= 2.0f;

		return v + uv + uuv;
#endif
	}

	inline Quaternion GetConjugate( void ) const {
		return Quaternion( -x, -y, -z, w );
	}

	inline Quaternion &conjugate( void ) {
		x = -x;
		y = -y;
		z = -z;
		return *this;
	}

	inline void GetOpenglMatrix4x4( float *mat ) const {
		float x2 = x * x;
		float y2 = y * y;
		float z2 = z * z;
		float xy = x * y;
		float xz = x * z;
		float yz = y * z;
		float wx = w * x;
		float wy = w * y;
		float wz = w * z;
 
		//Column-major!
		mat[0] = 1.0f - 2.0f * (y2 + z2);	mat[4] = 2.0f * (xy - wz);			mat[8] = 2.0f * (xz + wy);			mat[12] = 0.0f;
		mat[1] = 2.0f * (xy + wz);			mat[5] = 1.0f - 2.0f * (x2 + z2);	mat[9] = 2.0f * (yz - wx);			mat[13] = 0.0f;
		mat[2] = 2.0f * (xz - wy);			mat[6] = 2.0f * (yz + wx);			mat[10] = 1.0f - 2.0f * (x2 + y2);	mat[14] = 0.0f;
		mat[3] = 0.0f;						mat[7] = 0.0f;						mat[11] = 0.0f;						mat[15] = 1.0f;
	}

	inline void GetOpenglMatrix4x4( const Vector3f &pos, float *mat ) const {
		GetOpenglMatrix4x4( mat );
		mat[12] = pos.x;
		mat[13] = pos.y;
		mat[14] = pos.z;
	}


	inline Matrix3 GetMatrix3( void ) const {
		float x2 = x * x;
		float y2 = y * y;
		float z2 = z * z;
		float xy = x * y;
		float xz = x * z;
		float yz = y * z;
		float wx = w * x;
		float wy = w * y;
		float wz = w * z;

		//row-major
		return Matrix3( 1.0f - 2.0f * (y2 + z2),	2.0f * (xy + wz),			2.0f * (xz - wy),
						2.0f * (xy - wz),			1.0f - 2.0f * (x2 + z2),	2.0f * (yz + wx),
						2.0f * (xz + wy),			2.0f * (yz - wx),			1.0f - 2.0f * (x2 + y2) );
	}

	inline Quaternion		operator/(float a) const {
		Assert( a != 0.0f );
		float r = 1.0f/a;
		return Quaternion( x*r, y*r, z*r, w*r );
	}

	inline Quaternion&		operator=(const Quaternion& v) {
		//val = v.val;
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		Assert( IsValid() );
		return *this;
	}

	inline Quaternion&	operator+=(const Quaternion& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
		Assert( IsValid() );
		return *this;
	}

	inline Quaternion&	operator-=(const Quaternion& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
		Assert( IsValid() );
		return *this;
	}

	inline Quaternion&	operator*=(float a) {
		x *= a;
		y *= a;
		z *= a;
		w *= a;
		Assert( IsValid() );
		return *this;
	}

	inline Quaternion&	operator*=(const Quaternion &q) {
		SetValue( *this * q );
		return *this;
	}

	inline Quaternion&	operator/=(float a) {
		Assert( a != 0.0f );
		float r = 1.0f/a;
		x *= r;
		y *= r;
		z *= r;
		w *= r;
		return *this;
	}

	inline bool compare(const Quaternion &q) const {
		return x == q.x && y == q.y && z == q.z && w == q.w;
	}

	inline bool operator==(const Quaternion &other) const {
		return compare(other);
	}

	inline bool operator!=(const Quaternion &other) const {
		return !compare(other);
	}

	inline float Dot( const Quaternion &q ) const {
		return x * q.x + y * q.y + z * q.z + w * q.w;
	}

	template<typename T>
	inline float &operator[]( const T &i ) {
		Assert( i >= 0 && i < 4 );
		return ((float*)this)[i];
	}

	template<typename T>
	inline const float &operator[]( const T &i ) const {
		Assert( i >= 0 && i < 4 );
		return ((float*)this)[i];
	}

	//union{
	//	struct{ 
			float x,y,z,w;
		//};
		//__m128 val;
	//};
};


void QuaternionAlign( const Quaternion &p, const Quaternion &q, Quaternion &qt );
void QuaternionSlerp( const Quaternion &p, const Quaternion &q, const float &t, Quaternion &qt );
void QuaternionSlerpNoAlign( const Quaternion &p, const Quaternion &q, const float &t, Quaternion &qt );

inline void InterpolateEulers( const Angle3d &a, const Angle3d &b, const float &alpha, Angle3d &out )
{
	if( a == b )
		out = a;
	else
	{
		Quaternion q;
		QuaternionSlerp( Quaternion(a), Quaternion(b), alpha, q );
		q.ToAngle( out );
	}
}


template<typename T>
class Vector4
{
public:
	Vector4() { x = y = z = w = 0.0f; }
	Vector4( T X, T Y, T Z, T W ) 
		: x(X), y(Y), z(Z), w(W) { }

	Vector4( const Vector3f &v, T W ) {
		x = v.x; y = v.y; z = v.z; w = W;
	}

	Vector4 operator+(const Vector4 &v) const {
		return Vector4( x+v.x, y+v.y, z+v.z, w+v.w );
	}

	Vector4 operator*(T a) const {
		return Vector4( x*a, y*a, z*a, w*a );
	}

	Vector4 &Normalize( void ) {
		T rl = rsqrt( x*x + y*y + z*z + w*w );
		if( rl == rl )
		{
			x *= rl;
			y *= rl;
			z *= rl;
			w *= rl;
		}
		return *this;
	}

	bool operator==(const Vector4 &v) const {
		return x == v.x && y == v.y && z == v.z && w == v.w;
	}

	bool operator!=(const Vector4 &v) const {
		return x != v.x || y != v.y || z != v.z || w != v.w;
	}

public:
	T x,y,z,w;
};

typedef Vector4<float> Vector4f;

const Vector4f vec4_null(0.0f, 0.0f, 0.0f, 0.0f);


//calculates r = a * b^T
#ifdef MATHS_SSE
inline void Matrix4MultTranspose( const __m128 *a, const float *b, float *r )
{
	__m128 b_line, r_line;
	for( int i = 0; i < 4; i++ )
	{
		b_line = _mm_set_ps1( b[i+0] );
		r_line = _mm_mul_ps( a[0], b_line );

		b_line = _mm_set_ps1( b[i+4] );
		r_line = _mm_add_ps(r_line, _mm_mul_ps(a[1],b_line));

		b_line = _mm_set_ps1( b[i+8] );
		r_line = _mm_add_ps(r_line, _mm_mul_ps(a[2],b_line));

		b_line = _mm_set_ps1( b[i+12] );
		r_line = _mm_add_ps(r_line, _mm_mul_ps(a[3],b_line));

		_mm_store_ps( &r[i*4], r_line );
	}
}
#endif


//calculates r = a * b^T
inline void Matrix4MultTranspose( const float *a, const float *b, float *r )
{
	AssertMsg( (a != r && b != r), "Matrix4MultTranspose(): need different storage places." );
#ifdef MATHS_SSE
	__m128 cols[4] = { _mm_load_ps(&a[0]), _mm_load_ps(&a[4]), _mm_load_ps(&a[8]), _mm_load_ps(&a[12]) };
	Matrix4MultTranspose( cols, b, r );
#else
	r[0] = a[0]*b[0] + a[4]*b[4] + a[8]*b[8] + a[12]*b[12];
	r[1] = a[1]*b[0] + a[5]*b[4] + a[9]*b[8] + a[13]*b[12];
	r[2] = a[2]*b[0] + a[6]*b[4] + a[10]*b[8] + a[14]*b[12];
	r[3] = a[3]*b[0] + a[7]*b[4] + a[11]*b[8] + a[15]*b[12];

	r[4] = a[0]*b[1] + a[4]*b[5] + a[8]*b[9] + a[12]*b[13];
	r[5] = a[1]*b[1] + a[5]*b[5] + a[9]*b[9] + a[13]*b[13];
	r[6] = a[2]*b[1] + a[6]*b[5] + a[10]*b[9] + a[14]*b[13];
	r[7] = a[3]*b[1] + a[7]*b[5] + a[11]*b[9] + a[15]*b[13];

	r[8] = a[0]*b[2] + a[4]*b[6] + a[8]*b[10] + a[12]*b[14];
	r[9] = a[1]*b[2] + a[5]*b[6] + a[9]*b[10] + a[13]*b[14];
	r[10] = a[2]*b[2] + a[6]*b[6] + a[10]*b[10] + a[14]*b[14];
	r[11] = a[3]*b[2] + a[7]*b[6] + a[11]*b[10] + a[15]*b[14];

	r[12] = a[0]*b[3] + a[4]*b[7] + a[8]*b[11] + a[12]*b[15];
	r[13] = a[1]*b[3] + a[5]*b[7] + a[9]*b[11] + a[13]*b[15];
	r[14] = a[2]*b[3] + a[6]*b[7] + a[10]*b[11] + a[14]*b[15];
	r[15] = a[3]*b[3] + a[7]*b[7] + a[11]*b[11] + a[15]*b[15];
#endif
}

#ifdef MATHS_SSE
inline void Matrix4Mult( const __m128 *a, const float *b, float *r )
{
	__m128 b_line, r_line;
	for( int i = 0; i < 16; i += 4 )
	{
		b_line = _mm_set1_ps(b[i]); 
		r_line = _mm_mul_ps(a[0], b_line);

		b_line = _mm_set1_ps(b[i+1]);
		r_line = _mm_add_ps(_mm_mul_ps(a[1], b_line), r_line);

		b_line = _mm_set1_ps(b[i+2]);
		r_line = _mm_add_ps(_mm_mul_ps(a[2], b_line), r_line);

		b_line = _mm_set1_ps(b[i+3]);
		r_line = _mm_add_ps(_mm_mul_ps(a[3], b_line), r_line);

		_mm_store_ps(&r[i], r_line);
	}
}
#endif

//calculates r = a * b
inline void Matrix4Mult( const float *a, const float *b, float *r )
{
	AssertMsg( (a != r && b != r), "Matrix4MultTranspose(): need different storage places." );
#ifdef MATHS_SSE
	__m128 cols[4] = { _mm_load_ps(&a[0]), _mm_load_ps(&a[4]), _mm_load_ps(&a[8]), _mm_load_ps(&a[12]) };
	Matrix4Mult( cols, b, r );
#else
	r[0] = a[0]*b[0] + a[4]*b[1] + a[8]*b[2] + a[12]*b[3];
	r[1] = a[1]*b[0] + a[5]*b[1] + a[9]*b[2] + a[13]*b[3];
	r[2] = a[2]*b[0] + a[6]*b[1] + a[10]*b[2] + a[14]*b[3];
	r[3] = a[3]*b[0] + a[7]*b[1] + a[11]*b[2] + a[15]*b[3];

	r[4] = a[0]*b[4] + a[4]*b[5] + a[8]*b[6] + a[12]*b[7];
	r[5] = a[1]*b[4] + a[5]*b[5] + a[9]*b[6] + a[13]*b[7];
	r[6] = a[2]*b[4] + a[6]*b[5] + a[10]*b[6] + a[14]*b[7];
	r[7] = a[3]*b[4] + a[7]*b[5] + a[11]*b[6] + a[15]*b[7];

	r[8] = a[0]*b[8] + a[4]*b[9] + a[8]*b[10] + a[12]*b[11];
	r[9] = a[1]*b[8] + a[5]*b[9] + a[9]*b[10] + a[13]*b[11];
	r[10] = a[2]*b[8] + a[6]*b[9] + a[10]*b[10] + a[14]*b[11];
	r[11] = a[3]*b[8] + a[7]*b[9] + a[11]*b[10] + a[15]*b[11];

	r[12] = a[0]*b[12] + a[4]*b[13] + a[8]*b[14] + a[12]*b[15];	
	r[13] = a[1]*b[12] + a[5]*b[13] + a[9]*b[14] + a[13]*b[15];
	r[14] = a[2]*b[12] + a[6]*b[13] + a[10]*b[14] + a[14]*b[15];
	r[15] = a[3]*b[12] + a[7]*b[13] + a[11]*b[14] + a[15]*b[15];
#endif
}


inline void Matrix4MultVector4( const float *m, const float *v, float *r )
{
	AssertMsg( (v != r), "Matrix4MultVector4(): Need different storage locations." );
#ifdef MATHS_SSE
	//What we basically do here can be seen in the non-sse version.
	//Imagine you turn the non-sse version by 90 degrees.

	__m128 a_line, b_line, r_line;
	a_line = _mm_load_ps(&m[0]);
	b_line = _mm_set_ps1(v[0]);
	r_line = _mm_mul_ps(a_line, b_line); // r_line is zero in the start, so no add.

	a_line = _mm_load_ps(&m[4]);
	b_line = _mm_set1_ps(v[1]);
	r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

	a_line = _mm_load_ps(&m[8]);
	b_line = _mm_set1_ps(v[2]);
	r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

	a_line = _mm_load_ps(&m[12]);
	b_line = _mm_set1_ps(v[3]);
	r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

	_mm_store_ps(r, r_line);     // r = r_line
#else
	r[0] = m[0]*v[0] + m[4]*v[1] + m[8]*v[2] + m[12]*v[3];
	r[1] = m[1]*v[0] + m[5]*v[1] + m[9]*v[2] + m[13]*v[3];
	r[2] = m[2]*v[0] + m[6]*v[1] + m[10]*v[2] + m[14]*v[3];
	r[3] = m[3]*v[0] + m[7]*v[1] + m[11]*v[2] + m[15]*v[3];
#endif
}


#ifdef MATHS_SSE
inline void Matrix4MultVector4( const __m128 *m, const float *v, float *r )
{
	__m128 b_line, r_line;

	b_line = _mm_set_ps1(v[0]);
	r_line = _mm_mul_ps(m[0], b_line); // r_line = a_line * b_line
	b_line = _mm_set1_ps(v[1]);
	r_line = _mm_add_ps(_mm_mul_ps(m[1], b_line), r_line);
	b_line = _mm_set1_ps(v[2]);
	r_line = _mm_add_ps(_mm_mul_ps(m[2], b_line), r_line);
	b_line = _mm_set1_ps(v[3]);
	r_line = _mm_add_ps(_mm_mul_ps(m[3], b_line), r_line);
	_mm_store_ps(r, r_line); 
}
#endif

// | 0 4 8  12 |
// | 1 5 9  13 |
// | 2 6 10 14 |
// | 3 7 11 15 |
#ifdef MATHS_SSE
inline void Matrix4Transpose( float *a )
{
	__m128 A = _mm_load_ps( &a[0] );
	__m128 B = _mm_load_ps( &a[4] );
	__m128 C = _mm_load_ps( &a[8] );
	__m128 D = _mm_load_ps( &a[12] );
	_MM_TRANSPOSE4_PS(A,B,C,D);
}

inline void Matrix4Transpose( __m128 *a )
{
	_MM_TRANSPOSE4_PS( a[0], a[1], a[2], a[3] );
}
#else
inline void Matrix4Transpose( float *a )
{
	float b[16];
	memcpy( b, a, sizeof(float)*16 );

	a[1] = b[4];
	a[2] = b[8];
	a[3] = b[12];

	a[4] = b[1];
	a[6] = b[9];
	a[7] = b[13];

	a[8] = b[2];
	a[9] = b[6];
	a[11] = b[14];

	a[12] = b[3];
	a[13] = b[7];
	a[14] = b[11];
}
#endif

	
//Memory layout:
// | 0 4 8  12 |
// | 1 5 9  13 |
// | 2 6 10 14 |
// | 3 7 11 15 |
ALIGN_16 class Matrix4
{
	DECLARE_ALIGNED(Matrix4)

public:
	Matrix4() { ASSERT_ALIGNMENT(16); }
	Matrix4( const float *fv16 ) {
		Set(fv16);
		ASSERT_ALIGNMENT(16);
	}
	
	Matrix4( const Matrix4 &m ) {
		Set(m);
		ASSERT_ALIGNMENT(16);
	}

	Matrix4(float f11, float f12, float f13, float f14,
			float f21, float f22, float f23, float f24,
			float f31, float f32, float f33, float f34,
			float f41, float f42, float f43, float f44) {
		fv[0] = f11; fv[4] = f12; fv[8] = f13; fv[12] = f14;
		fv[1] = f21; fv[5] = f22; fv[9] = f23; fv[13] = f24;
		fv[2] = f31; fv[6] = f32; fv[10] = f33; fv[14] = f34;
		fv[3] = f41; fv[7] = f42; fv[11] = f43; fv[15] = f44;
		ASSERT_ALIGNMENT(16);
	}

	inline void Set( const float *v16 ) {
		memcpy(fv, v16, sizeof(float)*16);
	}

	inline void Set( float f11, float f12, float f13, float f14,
			float f21, float f22, float f23, float f24,
			float f31, float f32, float f33, float f34,
			float f41, float f42, float f43, float f44) {
		fv[0] = f11; fv[4] = f12; fv[8] = f13; fv[12] = f14;
		fv[1] = f21; fv[5] = f22; fv[9] = f23; fv[13] = f24;
		fv[2] = f31; fv[6] = f32; fv[10] = f33; fv[14] = f34;
		fv[3] = f41; fv[7] = f42; fv[11] = f43; fv[15] = f44;
	}

	inline void Set( const Matrix4 &mat ) {
		Set( mat.Get() );
	}

	inline const float *Get( void ) const {
		return fv;
	}

	inline float *Get( void ) {
		return fv;
	}

	inline Matrix4 operator*( const Matrix4 &m ) const {
		Matrix4 res;
		#ifdef MATHS_SSE
			Matrix4Mult( cols, m.fv, res.fv );
		#else
			Matrix4Mult( fv, m.fv, res.fv );
		#endif
		return res;
	}

	inline Matrix4 &operator*=( const Matrix4 &m ) {
		return Mult( m.fv );
	}

	//Multiply a row-major memory layout with this matrix
	//		*this			*this			b
	// | 0 4 8  12 |   | 0 4 8  12 |   | 0  1  2  3  |
	// | 1 5 9  13 |   | 1 5 9  13 |   | 4  5  6  7  |
	// | 2 6 10 14 | = | 2 6 10 14 | x | 8  9  10 11 |
	// | 3 7 11 15 |   | 3 7 11 15 |   | 12 13 14 15 |
	inline Matrix4 &MultRowMajor( const float *b ) {
		Matrix4 a(fv);
		#ifdef MATHS_SSE
			Matrix4MultTranspose( a.cols, b, fv );
		#else
			Matrix4MultTranspose( a.fv, b, fv );
		#endif
		return *this;
	}

	//Multiply a column-major memory layout with this matrix
	//		*this			*this			b
	// | 0 4 8  12 |   | 0 4 8  12 |   | 0 4 8  12 |
	// | 1 5 9  13 |   | 1 5 9  13 |   | 1 5 9  13 |
	// | 2 6 10 14 | = | 2 6 10 14 | x | 2 6 10 14 |
	// | 3 7 11 15 |   | 3 7 11 15 |   | 3 7 11 15 |
	inline Matrix4 &Mult( const float *b ) {
		Matrix4 a(fv);
		#ifdef MATHS_SSE
			Matrix4Mult( a.cols, b, fv );
		#else
			Matrix4Mult( a.fv, b, fv );
		#endif
		return *this;
	}

	inline Vector4f operator*( const Vector4f &v ) const {
		return Vector4f(	fv[0]*v.x + fv[4]*v.y + fv[8]*v.z + fv[12]*v.w,
							fv[1]*v.x + fv[5]*v.y + fv[9]*v.z + fv[13]*v.w,
							fv[2]*v.x + fv[6]*v.y + fv[10]*v.z + fv[14]*v.w,
							fv[3]*v.x + fv[7]*v.y + fv[11]*v.z + fv[15]*v.w );
	}

	inline Matrix4 &operator=( const Matrix4 &m ) {
		Set(m);
		return *this;
	}

	inline Matrix4 &SetIdentity( void ) {
		fv[0] = 1.0f; fv[1] = 0.0f; fv[2] = 0.0f; fv[3] = 0.0f;
		fv[4] = 0.0f; fv[5] = 1.0f; fv[6] = 0.0f; fv[7] = 0.0f;
		fv[8] = 0.0f; fv[9] = 0.0f; fv[10] = 1.0f; fv[11] = 0.0f;
		fv[12] = 0.0f; fv[13] = 0.0f; fv[14] = 0.0f; fv[15] = 1.0f;
		return *this;
	}

	inline Matrix4 &invert( void ) {
		Matrix4Inverse(fv);
		return *this;
	}

	inline Matrix4 GetInverse( void ) const {
		return Matrix4(*this).invert();
	}

	inline Matrix4 GetTransposed( void ) const {
		#ifdef MATHS_SSE
			Matrix4 res(*this);
			Matrix4Transpose( res.cols );
			return res;
		#else
			return Matrix4( fv[0], fv[1], fv[2], fv[3],
							fv[4], fv[5], fv[6], fv[7],
							fv[8], fv[9], fv[10], fv[11],
							fv[12], fv[13], fv[14], fv[15] );
		#endif
	}

	inline Matrix4 &transpose( void ) {
		#ifdef MATHS_SSE
			Matrix4Transpose( cols );
		#else
			Matrix4Transpose( fv );
		#endif
		return *this;
	}

	inline float& operator[]( int i ) {
		return fv[i];
	}

	inline const float& operator[]( int  i ) const {
		return fv[i];
	}

	inline static Matrix4 GetTranslation(const Vector3f &of)
	{
		ALIGN_16 float m[] = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			of.x, of.y, of.z, 1.0f };
		return Matrix4(m);
	}

	inline static Matrix4 GetPerspective( float fovy, float aspect, float zNear, float zFar ) {
		float f = 1.0f/TAN(fovy*0.5f);
		float a1 = (zFar+zNear) / (zNear-zFar);
		float a2 = (2.0f*zFar*zNear)/(zNear-zFar);

		ALIGN_16 float fMat[] = {
			f/aspect, 0.0f, 0.0f, 0.0f,
			0.0f,     f,    0.0f, 0.0f,
			0.0f,     0.0f, a1,   -1.0f,
			0.0f,     0.0f, a2,   0.0f };
		return Matrix4(fMat);
	}

public:
	#ifdef MATHS_SSE
		union{
	#endif
			float fv[16];
	#ifdef MATHS_SSE
			__m128 cols[4];
		};
	#endif
};

typedef Matrix4 Matrix4f;
//typedef Matrix4<double> Matrix4d;


//////////////////////////////////////////////////////////////////////////
//	Plane
//////////////////////////////////////////////////////////////////////////
template<typename T>
class CPlane_t
{
public:
	CPlane_t() : m_vNorm(), m_d(T(0)) { }
	CPlane_t( const Vector3<T> &norm, T d ) : m_vNorm(norm), m_d(d) { 
		Assert( FloatEqual(norm.Length(), T(1), T(0.001) ) ); 
	}

	CPlane_t( const Vector3<T> &vRef, const Vector3<T> &vNorm ) { 
		redefine(vRef,vNorm);
	}

	CPlane_t( const CPlane_t &plane ) {
		m_vNorm = plane.m_vNorm;
		m_d = plane.m_d;
	}

	inline void redefine( const Vector3<T> &point, const Vector3<T> &norm ) {
		Assert( FloatEqual(norm.Length(), T(1.0), T(0.001) ) );
		m_vNorm = norm;
		m_d = -norm.Dot(point);
	}

	inline bool IsVertexAbove( const Vector3<T> &vVert ) const { 
		return distToPointSigned(vVert) > T(0); 
	}

	inline bool IsBoxAbove( const Vector3<T> &boxCenter, const Vector3<T> &boxSizeHalf ) const {
		float dist = distToPointSigned(boxCenter);
		float maxDist = m_vNorm.AbsDotComponentWise(boxSizeHalf);
		return dist > maxDist;
	}

	inline bool IsNonCenteredBoxAbove( const Vector3<T> &boxMins, const Vector3<T> &boxMaxs ) const {
		Vector3<T> boxCenter = (boxMaxs + boxMins) * T(0.5);
		Vector3<T> boxSizeHalf = (boxMaxs - boxMins) * T(0.5);
		return IsBoxAbove( boxCenter, boxSizeHalf );
	}

	inline T distToPointSigned( const Vector3<T> &p ) const {
		return m_vNorm.Dot(p) + m_d;
	}

	inline T distToPointAbs( const Vector3<T> &p ) const {
		return abs( distToPointSigned(p) );
	}

public:
	Vector3<T> m_vNorm;
	T m_d;
};

typedef CPlane_t<float> CPlane;
typedef CPlane_t<float> CPlanef;
typedef CPlane_t<double> CPlaned;


//////////////////////////////////////////////////////////////////////////
//	AABB
//////////////////////////////////////////////////////////////////////////
template<typename T>
class AABoundingBox_t
{
public:
	AABoundingBox_t() { }
	AABoundingBox_t( const AABoundingBox_t &box ) : min(box.min), max(box.max) { }
	AABoundingBox_t( const Vector3<T> &Min, const Vector3<T> &Max ) : min(Min), max(Max) { }
	inline void CalculateVertices( Vector3<T> *pVerts8 ) const {
		pVerts8[0].Init( min.x, min.y, min.z );
		pVerts8[1].Init( min.x, min.y, max.z );
		pVerts8[2].Init( min.x, max.y, min.z );
		pVerts8[3].Init( min.x, max.y, max.z );
		pVerts8[4].Init( max.x, min.y, min.z );
		pVerts8[5].Init( max.x, min.y, max.z );
		pVerts8[6].Init( max.x, max.y, min.z );
		pVerts8[7].Init( max.x, max.y, max.z );
	}

	//Note that this rotates the aabb around the world-origin (0 0 0),
	//not around the box's center
	inline AABoundingBox_t GetRotated( const Quaternion &rot ) const {
		AABoundingBox_t aabb(*this);
		return aabb.Rotate(rot);
	}

	inline AABoundingBox_t &Rotate( const Quaternion &rot ) {
		Vector3<T> verts[8];
		CalculateVertices( verts );

		for( int i = 0; i < 8; i++ )
			verts[i] = rot * verts[i];

		return CalcFromVertices( verts, 8 );
	}

	inline AABoundingBox_t GetScaled( T scale ) const {
		AABoundingBox_t aabb(*this);
		return aabb.Scale(scale);
	}

	inline AABoundingBox_t GetScaled( const Vector3<T> &scale ) {
		AABoundingBox_t aabb(*this);
		return aabb.Scale(scale);
	}

	inline AABoundingBox_t &Scale( T scale ) {
		min *= scale;
		max *= scale;
		return *this;
	}

	inline AABoundingBox_t &Scale( const Vector3<T> &scale ) {
		min.x *= scale.x;
		min.y *= scale.y;
		min.z *= scale.z;
		max.x *= scale.x;
		max.y *= scale.y;
		max.z *= scale.z;
		return *this;
	}

	inline AABoundingBox_t GetTranslated( const Vector3<T> &trans ) const {
		AABoundingBox_t aabb(*this);
		return aabb.Translate(trans);
	}

	inline AABoundingBox_t &Translate( const Vector3<T> &trans ) {
		min += trans;
		max += trans;
		return *this;
	}

	inline AABoundingBox_t &CalcFromVertices( const Vector3<T> *verts, int count ) {
		Assert( count > 0 );
		min = max = verts[0];
		for( int i = 1; i < count; i++ )
		{
			const Vector3<T> &v = verts[i];
			min = min.MinComponentWise(v);
			max = max.MaxComponentWise(v);
		}
		return *this;
	}

	//For interleaved vertex-data, stride is the difference of vertex-adresses
	//(i.e. reinterpret_cast<size_t>(&vert[1]-&vert[0]) )
	inline AABoundingBox_t &CalcFromVertices( void *data, size_t count, size_t stride ) {
		Assert( count > 0 );
		min = max = *(Vector3<T>*)data;
		for( size_t i = 1; i < count; i++ )
		{
			data = reinterpret_cast<void*>( reinterpret_cast<size_t>(data) + stride );
			const Vector3<T> &v = *(Vector3<T>*)data;
			min = min.MinComponentWise(v);
			max = max.MaxComponentWise(v);
		}
		return *this;
	}

	//Get the center of the aabb
	inline Vector3<T> GetCenter(void) const {
		return (min+max)*T(0.5);
	}

public:
	Vector3<T> min;
	Vector3<T> max;
};

typedef AABoundingBox_t<float> AABoundingBox;
typedef AABoundingBox_t<double> AABoundingBoxd;


//////////////////////////////////////////////////////////////////////////
//	RAY
//////////////////////////////////////////////////////////////////////////
template<typename T>
class CRay_t
{
public:
	CRay_t(const Vector3<T> &rayorig, const Vector3<T> &raydir)
		: orig(rayorig), dir(raydir) { }
	CRay_t()
		:orig(Vector3<T>::null()), dir(Vector3<T>(T(1), T(0), T(0))) { }

	//Returns the minimum distance to the point
	inline T distToPoint(const Vector3<T> &point) const {
		return dist(*this, point);
	}

	//Returns the minimum distance to the point and the ray-parameter t.
	//The point on the ray which is closest to the point is P(t) = orig + dir*t;
	inline T distToPoint(const Vector3<T> &point, T &t) const {
		return dist(*this, point, t);
	}

	//Returns the minimum distance to the point and the point on the ray in which the distance
	//is minimal
	inline T distToPoint(const Vector3<T> &point, Vector3<T> &distPoint) const {
		return dist(*this, point, distPoint);
	}

	bool intersectSphere( const Vector3<T> &sphereCenter, T radius,
		T distMax, T &distHit, bool bDiscardInner = true ) const;

	bool intersectAABB( const AABoundingBox_t<T> &box, T &distHit, T distMax = std::numeric_limits<T>::max() ) const;

	//Returns the minimum distance between this ray and the other one
	inline T distToRay(const CRay_t &other) const {
		return dist(*this, other);
	}

	//Returns the minimum distance between this ray and the other one
	//pointThis is the point which is nearest to this ray,
	//pointOther is the point which is neares to the other ray.
	inline T distToRay(const CRay_t &other, Vector3<T> &pointThis, Vector3<T> &pointOther) const {
		return dist(*this, other, pointThis, pointOther);
	}

	inline bool intersect( const CPlane_t<T> &plane, T &t ) const {
		T denom = plane.m_vNorm.Dot(dir);
		if( abs(denom) < std::numeric_limits<T>::epsilon() )
			return false;

		T nom = plane.m_vNorm.Dot(orig) + plane.m_d;
		t = -(nom/denom);
		return true;
	}

	inline bool intersect( const CPlane_t<T> &plane, Vector3<T> &intersPoint ) const {
		T t;
		return intersect(plane, intersPoint, t);
	}

	inline bool intersect( const CPlane_t<T> &plane, Vector3<T> &intersPoint, T &t ) const {
		if( !intersect(plane, t) )
			return false;
		intersPoint = orig + dir * t;
		return true;
	}



	//Returns the minimum distance between p and q.
	//pp is the point on p which is nearest to q,
	//pq is the point on q which is nearest to p.
	static T dist(const CRay_t &p, const CRay_t &q, Vector3<T> &pp, Vector3<T> &pq) {
		//Ray-Ray distance calculation from http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
		const Vector3<T> &u = p.dir;
		const Vector3<T> &v = q.dir;
		const Vector3<T> w0 = p.orig - q.orig;
		const T a = u.Dot(u);
		const T b = u.Dot(v);
		const T c = v.Dot(v);
		const T d = u.Dot(w0);
		const T e = v.Dot(w0);

		const T div = (a*c - b*b);
		const T tp = (b*e - c*d) / div;
		const T tq = (a*e - b*d) / div;

		pp = p.orig + p.dir * tp;
		pq = q.orig + q.dir * tq;
		return pp.distTo(pq);
	}

	inline static T dist(const CRay_t &r, const Vector3<T> &p) {
		return r.dir.Cross(r.orig - p).Length() / r.dir.Length();
	}

	inline static T dist(const CRay_t<T> &r, const Vector3<T> &p, Vector3<T> &distPoint) {
		T t;
		T d = dist(r,p,t);
		distPoint = r.orig + r.dir * t;
		//T d2 = distPoint.distTo(p);
		return d;
	}

	//Returns the minimum distance between p and q
	inline static T dist(const CRay_t<T> &p, const CRay_t<T> &q) {
		Vector3<T> dummy1, dummy2;
		return dist(p, q, dummy1, dummy2);
	}

	inline static T dist(const CRay_t<T> &r, const Vector3<T> &p, T &t) {
		t = (p - r.orig).Dot(r.dir) / r.dir.LengthSq();
		return dist(r,p);
	}

public:
	Vector3<T> orig;
	Vector3<T> dir;
};

typedef CRay_t<float> CRay;
typedef CRay_t<float> CRayf;
typedef CRay_t<double> CRayd;


//////////////////////////////////////////////////////////////////////////
//	Convex Shape
//////////////////////////////////////////////////////////////////////////
class ConvexShape
{
public:
	ConvexShape(bool normalsFacingOutside = true) {
		m_normalsFacingOutside = normalsFacingOutside;
	}

	void addPlane(const CPlane &plane) {
		m_planes.push_back(plane);
	}

	bool isPointInside(const Vector3f &p) {
		for( UINT i = 0; i < m_planes.size(); i++ )
			if( m_planes[i].IsVertexAbove(p) == m_normalsFacingOutside )
				return false;
		return true;
	}

	bool isPointInside(const Vector3f &p, float tolerance) {
		for( UINT i = 0; i < m_planes.size(); i++ ) {
			float dist = m_planes[i].distToPointSigned(p);
			if( abs(dist) > tolerance && ((dist > 0.0f) == m_normalsFacingOutside) )
				return false;
		}
		return true;
	}
private:
	std::vector<CPlane> m_planes;
	bool m_normalsFacingOutside;
};


//////////////////////////////////////////////////////////////////////////
//	General Math Functions
//////////////////////////////////////////////////////////////////////////


inline int IntPow( int base, int exp )
{
	Assert( exp >= 0 );
	int res = 1;
	for( int i = 0; i < exp; i++ )
		res *= base;

	return res;
}


inline Vector3f ValveVectorToVector3f( const Vector3f &v )
{
	return (Vector3f( -v.x, v.z, v.y ) * (2.54f/10.0f));
}

//Calculates Sphere coordinates from cartesian coordinates
//
// IN: 
// point - point on the sphere relative to the sphere's center
//
// OUT:
// r - sphere radius
// phi - Azimuth angle (between the x-axis and the projection of the point onto the xz-plane, from -pi to pi)
// theta - Polar angle (between the y-axis and the point, from 0 to pi)
template<typename T>
inline void PointToSphereCoords(const Vector3<T> &point, T &r, T &phi, T &theta)
{
	r = point.Length();
	Assert( IsFinite(r) && r >= 0);

	if( point.z == 0.0f && point.x == 0 )
		phi = 0;
	else
		phi = atan2(point.z, point.x);
		
	Assert( IsFinite(phi) );

	T a = point.y / r;
	theta = acos(clamp(a, T(-1), T(1)));
}


//Random
extern void InitRandom( void );
extern int RandomInt( int min, int max );
extern float RandomFloat( float min, float max );
extern Vector2f RandomVector( int MinX, int MinY, int MaxX, int MaxY );

extern float VectorLength( Vector2f v );
extern float VectorLength( const Vector3f &v );
extern float VectorLengthSq( const Vector3f &v );

extern void VectorNormalize( Vector2f &v );
extern void VectorNormalize( Vector3f &v );

extern Vector3f VectorCross( const Vector3f &a, const Vector3f &b );
extern float VectorDot( const Vector3f &a, const Vector3f &b );


extern void AngleToVector( const Vector3f &Angle, Vector3f &Vec );
extern void VectorToAngle( const Vector3f &Vec, Vector3f &Angle );
//extern void Matrix3ToAngle( const Matrix3 &m, Angle3d &a );
extern void AngleToVectorTest( void );

extern Matrix3 TransposeMatrix( Matrix3 mat );

extern void BlurValues( const int number, float *values );

extern float max3f( float a, float b, float c );

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

//#ifndef __TOOL
//#ifndef _SERVER
//extern Matrix3 g_CameraMatrix;
//extern Matrix3 g_CameraMatrixTrans;
//extern inline void RotateCameraMatrix( float ang, const int axis );
//extern inline void RotateWorldMatrix( float ang, const int axis );
//extern inline void RotateWorld( Vector3f angles );
//extern Matrix3 GetModelViewMatrix3( void );
//#endif
//#endif

template<class T>
inline void removeFromVector(std::vector<T> &vec, const T &val)
{
	vec.erase(std::remove(vec.begin(), vec.end(), val), vec.end());
}



#endif
