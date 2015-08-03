// util/CNetVar.h
// util/CNetVar.h
// util/CNetVar.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//CNetVar.h

#pragma once
#ifndef deferred__util__CNetVar_H__
#define deferred__util__CNetVar_H__

class CNetVarBase
{

};

template<typename T, typename ChangerT>
class CNetVar : public CNetVarBase
{
public:
	inline static size_t getSize() { return sizeof(T); }
	inline static size_t getCount() { return 1; }

	CNetVar() {}

	CNetVar(const T &val) :
		m_val(val)
	{
		//Changed();
	}

	inline const T &set(const T &val) {
		if(m_val != val)
		{
			Changed();
			m_val = val;
		}
		return m_val;
	}

	inline const T &get() const {
		return m_val;
	}

	inline T &getMod() {
		Changed();
		return m_val;
	}

	inline const T &operator=(const T &other) {
		return set(other);
	}

	inline const T &operator=(const CNetVar<T, ChangerT> &other) {
		return set(other.get());
	}

	//prefix
	inline const T &operator++() {
		Changed();
		return ++m_val;
	}

	//prefix
	inline const T &operator--() {
		Changed();
		return --m_val;
	}

	//postfix
	inline T operator++( int ) {
		Changed();
		T old = m_val;
		m_val++;
		return old;
	}

	//postfix
	inline T operator--( int ) {
		Changed();
		T old = m_val;
		m_val--;
		return old;
	}

	//cast
	operator const T&() const {
		return m_val;
	}

	inline T operator+() const {
		return +m_val;
	}

	inline T operator-() const {
		return -m_val;
	}


	#define NETVAR_COMPOUND_OPERATOR(op) inline const CNetVar<T, ChangerT> &operator op(const T &v) { m_val op v; Changed(); return *this; }
	NETVAR_COMPOUND_OPERATOR(+=)
	NETVAR_COMPOUND_OPERATOR(-=)
	NETVAR_COMPOUND_OPERATOR(*=)
	NETVAR_COMPOUND_OPERATOR(/=)
	NETVAR_COMPOUND_OPERATOR(%=)
	NETVAR_COMPOUND_OPERATOR(&=)
	NETVAR_COMPOUND_OPERATOR(|=)
	NETVAR_COMPOUND_OPERATOR(^=)
	NETVAR_COMPOUND_OPERATOR(<<=)
	NETVAR_COMPOUND_OPERATOR(>>=)

	inline T operator~() const {
		return ~m_val;
	}

	const T* operator->() const
	{
		return &m_val; 
	}

	T* operator->()
	{
		Changed();
		return &m_val;
	}

	const bool operator!() const
	{
		return !m_val;
	}

protected:
	//do not make this const to protect from call from non-const function
	inline void Changed() 
	{
		ChangerT::Changed(this);
	}

private:
	CNetVar(const CNetVar<T, ChangerT> &other) {}

protected:
	T m_val;
};




template<typename T, size_t elemCount, typename ChangerT>
class CNetVarArray : public CNetVarBase
{
public:
	inline static size_t getSize() { return sizeof(T); }
	inline static size_t getCount() { return elemCount; }

	CNetVarArray() {}

	inline const T &operator[](size_t i) const
	{
		Assert(i < elemCount);
		return m_vals[i];
	}

	inline T &operator[](size_t i)
	{
		Changed();
		return m_vals[i];
	}

protected:
	//do not make this const to protect from call from non-const function
	inline void Changed() 
	{
		ChangerT::Changed(this);
	}

private:
	CNetVarArray(const CNetVarArray<T, elemCount, ChangerT> &other) {}

protected:
	T m_vals[elemCount];
};


template<typename T, typename ChangerT>
class CNetVarVector3 : public CNetVar<Vector3<T>, ChangerT>
{
private:
	typedef CNetVar<Vector3<T>, ChangerT> BaseClass;

public:
	CNetVarVector3() {}
	CNetVarVector3(const T *vals) : CNetVar<Vector3<T>, ChangerT>(Vector3<T>(vals)) {}
	CNetVarVector3(T x, T y, T z) : CNetVar<Vector3<T>, ChangerT>(Vector3<T>(x, y, z)) {}
	CNetVarVector3(const Vector3<T> &other) : CNetVar<Vector3<T>, ChangerT>(other) {}

	inline const Vector3<T> &operator=(const Vector3<T> &other) { return BaseClass::set(other); }
	inline const Vector3<T> &operator=(const CNetVarVector3<T, ChangerT> &other) { return BaseClass::set(other.get()); }

	inline T GetX() const { return BaseClass::m_val.x; }
	inline T GetY() const { return BaseClass::m_val.y; }
	inline T GetZ() const { return BaseClass::m_val.z; }

	inline void SetX(T x) { 
		if( BaseClass::m_val.x != x )
		{
			BaseClass::m_val.x = x;
			BaseClass::Changed();
		}
	}

	inline void SetY(T y) { 
		if( BaseClass::m_val.y != y )
		{
			BaseClass::m_val.y = y;
			BaseClass::Changed();
		}
	}

	inline void SetZ(T z) { 
		if( BaseClass::m_val.z != z )
		{
			BaseClass::m_val.z = z;
			BaseClass::Changed();
		}
	}

#define NETVAR_VECTOR_ARITHMETIC_OPERATOR(op) inline Vector3<T> operator op(const Vector3<T> &v) const { return BaseClass::m_val op v; }
	NETVAR_VECTOR_ARITHMETIC_OPERATOR(+)
	NETVAR_VECTOR_ARITHMETIC_OPERATOR(-)
	NETVAR_VECTOR_ARITHMETIC_OPERATOR(*)
	NETVAR_VECTOR_ARITHMETIC_OPERATOR(/)

#define NETVAR_VECTOR_COMPARISON_OPERATOR(op) inline bool operator op(const Vector3<T> &v) const { return BaseClass::m_val op v; }
	NETVAR_VECTOR_COMPARISON_OPERATOR(==)
	NETVAR_VECTOR_COMPARISON_OPERATOR(!=)
	NETVAR_VECTOR_COMPARISON_OPERATOR(>)
	NETVAR_VECTOR_COMPARISON_OPERATOR(<)
	NETVAR_VECTOR_COMPARISON_OPERATOR(>=)
	NETVAR_VECTOR_COMPARISON_OPERATOR(<=)
};



//template<typename _T>
//class CNetVar
//{
//public:
//	CNetVar() { }
//	CNetVar( const _T &v ) : t(v) { }
//	CNetVar( const CNetVar<_T> &nv ) : t(nv.t) { }
//
//	inline CNetVar<_T> &operator=( const _T &v ) {
//		t = v;
//		return *this;
//	}
//
//	//#####################
//	// Arithmetic operators
//	//#####################
//
//	//Suffix (a++)
//	inline CNetVar<_T> operator++( int ) {
//		CNetVar<_T> old(*this);
//		++t;
//		return old;
//	}
//
//	//Prefix (++a)
//	inline CNetVar<_T> &operator++() {
//		t++;
//		return *this;
//	}
//
//	//Suffix (a--)
//	inline CNetVar<_T> operator--( int ) {
//		CNetVar<_T> old(*this);
//		t--;
//		return old;
//	}
//
//	//Prefix (--a)
//	inline CNetVar<_T> &operator--() {
//		t--;
//		return *this;
//	}
//
//	inline CNetVar<_T> operator+( const _T &v ) const {
//		return CNetVar<_T>(t + v);
//	}
//
//	inline CNetVar<_T> operator-( const _T &v ) const {
//		return CNetVar<_T>( t - v );
//	}
//
//	inline CNetVar<_T> operator*( const _T &v ) const {
//		return CNetVar<_T>( t * v );
//	}
//
//	inline CNetVar<_T> operator/( const _T &v ) const {
//		return CNetVar<_T>( t / v );
//	}
//
//	inline CNetVar<_T> operator%( const _T &v ) const {
//		return CNetVar<_T>( t % v );
//	}
//
//	inline CNetVar<_T> operator+() {
//		return CNetVar<_T>( +t );
//	}
//
//	inline CNetVar<_T> operator-() {
//		return CNetVar<_T>( -t );
//	}
//
//
//	//#####################
//	// Comparison operators
//	//#####################
//#define NETVAR_COMPARISON_OPERATOR(op) inline bool operator op( const _T &v ) const { return t op v; }
//
//	NETVAR_COMPARISON_OPERATOR(==)
//	NETVAR_COMPARISON_OPERATOR(!=)
//	NETVAR_COMPARISON_OPERATOR(>)
//	NETVAR_COMPARISON_OPERATOR(<)
//	NETVAR_COMPARISON_OPERATOR(>=)
//	NETVAR_COMPARISON_OPERATOR(<=)
//
//
//	//#####################
//	// Logical operators
//	//#####################
//	inline bool operator!() const {
//		return !t;
//	}
//
//	inline bool operator&&( const _T &v ) const {
//		return t && v;
//	}
//
//	inline bool operator||( const _T &v ) const {
//		return t || v;
//	}
//
//
//	//#####################
//	// Bitwise operators
//	//#####################
//	inline CNetVar<_T> operator~() {
//		return CNetVar<_T>(~t);
//	}
//
//#define NETVAR_BITWISE_OPERATOR(op) inline CNetVar operator op( const _T &v ) const { return CNetVar<_T>( t op v ); }
//
//	NETVAR_BITWISE_OPERATOR(&)
//	NETVAR_BITWISE_OPERATOR(|)
//	NETVAR_BITWISE_OPERATOR(^)
//	NETVAR_BITWISE_OPERATOR(<<)
//	NETVAR_BITWISE_OPERATOR(>>)
//
//	//#####################
//	// Member and pointer operators
//	//#####################
//	template<typename U>
//	inline _T &operator[]( const U &v ) {
//		return t[v];
//	}
//
//	template<typename U>
//	inline const _T &operator[]( const U &v ) const {
//		return t[v];
//	}
//
//	//We also can overload *a, &a, a->b, a->*b, but why?
//	
//	//#####################
//	// Compound operators
//	//#####################
//#define NETVAR_COMPOUND_OPERATOR(op) inline CNetVar<_T> &operator op( const _T &v ) { t op v; return *this; }
//
//	NETVAR_COMPOUND_OPERATOR(+=)
//	NETVAR_COMPOUND_OPERATOR(-=)
//	NETVAR_COMPOUND_OPERATOR(*=)
//	NETVAR_COMPOUND_OPERATOR(/=)
//	NETVAR_COMPOUND_OPERATOR(%=)
//	NETVAR_COMPOUND_OPERATOR(&=)
//	NETVAR_COMPOUND_OPERATOR(|=)
//	NETVAR_COMPOUND_OPERATOR(^=)
//	NETVAR_COMPOUND_OPERATOR(<<=)
//	NETVAR_COMPOUND_OPERATOR(>>=)
//
//	
//	//#############################
//	// others
//	//#############################
//
//	//Type conversion operator
//	template<typename T>
//	inline operator T() {
//		return T(t);
//	}
//	
//	inline const _T &get() const {
//		return t;
//	}
//
//private:
//	_T t;
//};

#endif // deferred__util__CNetVar_H__
