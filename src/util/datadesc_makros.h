// util/datadesc_makros.h
// util/datadesc_makros.h
// util/datadesc_makros.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__datadesc_makros_H__
#define deferred__util__datadesc_makros_H__

#ifndef BASE_ENTITY_TYPE
	#error BASE_ENTITY_TYPE has to be declared before using this file, use CBaseEntity for the server.
#endif

#include "datadesc.h"

#define TypedDatadescList DatadescList<BASE_ENTITY_TYPE>

#define CLASS_MEMBER_OFFSET(classname,member) (reinterpret_cast<size_t>(&(((classname*)NULL)->member)))
#define CLASS_MEMBER_SIZE(classname, member) (sizeof(((classname*)NULL)->member))

//######################################################
// Basic datadesc
//######################################################
#define DECLARE_DATADESC_EDITOR(BaseClassName, ClassName) \
	DECLARE_CLASS( BaseClassName, ClassName ); \
	public: static TypedDatadescList* GetDatadescListEditor(void); /*{ return m_datadescListEditor; }*/ \
	static TypedDatadescList* GetBaseDatadescListEditor(void) { return BaseClassName::GetDatadescListEditor(); } \
	static void InitDatadescListEditor(void); \
	virtual const TypedDatadescList *GetDatadescList(void) const { return ThisClass::GetDatadescListEditor(); }


#define DECLARE_DATADESC_INITIALIZER( ClassName ) \
	namespace internals { \
		class CDatadescInitializer_##ClassName { \
			public: CDatadescInitializer_##ClassName() { ClassName::InitDatadescListEditor(); } \
		}; \
		static const CDatadescInitializer_##ClassName datadescInitializer_##ClassName; \
	}

#define BEGIN_DATADESC_EDITOR( ClassName ) \
	DECLARE_DATADESC_INITIALIZER( ClassName ) \
	TypedDatadescList* ClassName::GetDatadescListEditor(void) { static TypedDatadescList l(#ClassName); return &l; } \
	void ClassName::InitDatadescListEditor(void) \
	{ \
		TypedDatadescList *dataList = ClassName::GetDatadescListEditor(); \
		if( dataList->IsInitialized() ) return; \
		BaseClass::InitDatadescListEditor(); \
		dataList->SetBase( ClassName::GetBaseDatadescListEditor() );


#define DECLARE_DATADESC_EDITOR_NOBASE(ClassName) \
	private: typedef ClassName ThisClass; \
	public: static TypedDatadescList* GetDatadescListEditor(void); /*{ return m_datadescListEditor; }*/ \
		static TypedDatadescList* GetBaseDatadescListEditor(void) { return NULL; } \
		static void InitDatadescListEditor(void); \
		virtual const TypedDatadescList *GetDatadescList(void) const { return ThisClass::GetDatadescListEditor(); }


#define BEGIN_DATADESC_EDITOR_NOBASE( ClassName ) \
	DECLARE_DATADESC_INITIALIZER( ClassName ) \
	TypedDatadescList* ClassName::GetDatadescListEditor(void) { static TypedDatadescList l(#ClassName); return &l; } \
	void ClassName::InitDatadescListEditor(void) \
	{ \
		TypedDatadescList *dataList = ClassName::GetDatadescListEditor(); \
		if( dataList->IsInitialized() ) return; \
		dataList->SetBase( NULL );


#define END_DATADESC_EDITOR() dataList->SetInitialized(); }
#define END_DATEDESC_EDITOR_NOBASE() dataList->SetInitialized(); }


//######################################################
// Datadesc fields
//######################################################
#define _DEFINE_FIELD(type, count, member, flags, varname ) \
	dataList->AddEntry( CDatadescEntry(type, count, CLASS_MEMBER_OFFSET(ThisClass, member), flags, varname) );

//single int
#define FIELD_INT(varname, member) \
	_DEFINE_FIELD( DATA_INT, 1, member, 0, varname )

//array of integers
#define FIELD_INT_ARRAY(varname, member, count) \
	_DEFINE_FIELD( DATA_INT_ARRAY, count, member, 0, varname )

//single bool
#define FIELD_BOOL(varname, member) \
	_DEFINE_FIELD( DATA_BOOL, 1, member, 0, varname )

//single float
#define FIELD_FLOAT(varname, member) \
	_DEFINE_FIELD( DATA_FLOAT, 1, member, 0, varname )

//array of floats
#define FIELD_FLOAT_ARRAY(varname, member, count) \
	_DEFINE_FIELD( DATA_FLOAT_ARRAY, count, member, 0, varname )

//array of floats with 4 elements
#define FIELD_FLOAT4(varname, member) \
	_DEFINE_FIELD( DATA_FLOAT4, 4, member, 0, varname )

//Vector2d
#define FIELD_VECTOR2(varname, member) \
	_DEFINE_FIELD( DATA_VECTOR2, 2, member, 0, varname )

//Vector3f
#define FIELD_VECTOR3(varname, member) \
	_DEFINE_FIELD( DATA_VECTOR3, 3, member, 0, varname )

#define FIELD_ANGLES FIELD_VECTOR3

//Vector3f is normalized after read
#define FIELD_VECTOR3_NORMALIZED(varname, member) \
	_DEFINE_FIELD( DATA_VECTOR3_NORMALIZED, 3, member, 0, varname )

//Vector4d
#define FIELD_VECTOR4(varname, member) \
	_DEFINE_FIELD( DATA_VECTOR4, 4, member, 0, varname )

//Vector4d is normalized after read
#define FIELD_VECTOR4_NORMALIZED(varname, member) \
	_DEFINE_FIELD( DATA_VECTOR4_NORMALIZED, 4, member, 0, varname )

//C++ style string (std::string)
#define FIELD_STRING(varname, member) \
	_DEFINE_FIELD( DATA_STRING, 1, member, 0, varname )

//C style string (char[])
#define FIELD_CHAR_ARRAY(varname, member, count) \
	_DEFINE_FIELD( DATA_CHAR_ARRAY, count, member, 0, varname )

//Rotation stored as euler angles in script, but as Quaternion inside CPP
#define FIELD_ANGLE_TO_QUATERNION(varname, member) \
	_DEFINE_FIELD( DATA_ANGLES_TO_QUATERNION, 3, member, 0, varname )

#define FIELD_COLOR3(varname, member) \
	_DEFINE_FIELD( DATA_COLOR3, 3, member, 0, varname )

#define FIELD_COLOR4(varname, member) \
	_DEFINE_FIELD( DATA_COLOR4, 4, member, 0, varname )


//######################################################
// Inputfunction field
//######################################################
#ifdef _SERVER
#define FIELD_INPUTFUNC(name, func) \
	dataList->AddInput( CDatadescInputEntry<BASE_ENTITY_TYPE>(static_cast<inputfunc_t<BASE_ENTITY_TYPE>::type>(&ThisClass::func), name) );
#endif /* _SERVER */


//######################################################
// Network table macros
//######################################################
#ifdef _SERVER
	#define NETPROXYDEFAULT CNetVarWriteProxyDefault
#else
	#define NETPROXYDEFAULT CNetVarReadProxyDefault
#endif

#define _NETVAR_OFFSET(member) \
	(reinterpret_cast<size_t>(static_cast<CNetVarBase*>(&(((ThisClass*)NULL)->member))))

/* #define NETTABLE_ENTRY_ARRAY_RAW_PROXY_INTERP(member, proxy, interp) \
 *	dataList->AddNetworkEntry( CNetworkTableEntry(CLASS_MEMBER_OFFSET(ThisClass, member), \
 *		sizeof(ThisClass::member[0]), ARRAY_LENGTH(member), 0, 0, &proxy::NETPROXYFUNCNAME) )
 */

#ifdef _SERVER
#define NETTABLE_ENTRY_PROXY(member, proxy) \
	dataList->AddNetworkEntry( CNetworkTableEntry(_NETVAR_OFFSET(member), \
		NetVarType_##member::getSize(), proxy::GetNetworkSize(NetVarType_##member::getSize()), \
		NetVarType_##member::getCount(), 0, 0, &proxy::write) );

	#define NETTABLE_ENTRY(member) \
		NETTABLE_ENTRY_PROXY(member, CNetVarWriteProxyDefault)

	#define NETTABLE_ENTRY_ARRAY(member, count) \
		NETTABLE_ENTRY_PROXY(member, CNetVarWriteProxyDefault)

	//For std::string
	#define NETTABLE_ENTRY_STRING(member) \
		NETTABLE_ENTRY_PROXY(member, CNetVarWriteProxyString)

#else /* _SERVER */
	#define NETTABLE_ENTRY_RAW_PROXY_INTERP(member, proxy, interp) \
		dataList->AddNetworkEntry( CNetworkTableEntry(CLASS_MEMBER_OFFSET(ThisClass, member), \
			CLASS_MEMBER_SIZE(ThisClass, member), proxy::GetNetworkSize(CLASS_MEMBER_SIZE(ThisClass, member)), \
			1, 0, 0, &proxy::read, &proxy::GetDynamicSize, interp) );

	#define NETTABLE_ENTRY(member) \
		NETTABLE_ENTRY_RAW_PROXY_INTERP(member, CNetVarReadProxyDefault, INTERP_NONE)

	#define NETTABLE_ENTRY_ARRAY_PROXY(member, count, proxy) \
		dataList->AddNetworkEntry( CNetworkTableEntry(CLASS_MEMBER_OFFSET(ThisClass, member[0]), \
		CLASS_MEMBER_SIZE(ThisClass, member[0]), proxy::GetNetworkSize(CLASS_MEMBER_SIZE(ThisClass, member[0])), \
		count, 0, 0, &proxy::read, &proxy::GetDynamicSize, INTERP_NONE) );

	#define NETTABLE_ENTRY_ARRAY(member, count) \
		NETTABLE_ENTRY_ARRAY_PROXY(member, count, CNetVarReadProxyDefault)

	#define NETTABLE_ENTRY_INTERP(member, interp) \
		NETTABLE_ENTRY_RAW_PROXY_INTERP(member, CNetVarReadProxyDefault, interp)

	#define NETTABLE_ENTRY_PROXY(member, proxy) \
		NETTABLE_ENTRY_RAW_PROXY_INTERP(member, proxy, INTERP_NONE)	

	//For std::string
	#define NETTABLE_ENTRY_STRING(member) \
		NETTABLE_ENTRY_PROXY(member, CNetVarReadProxyString)

#endif /* else _SERVER */


#ifdef _SERVER
#define _NetVar(member) \
	friend class NetVarChanger_##member; \
	class NetVarChanger_##member \
	{ \
		public: \
		inline static void Changed(void *netVar) \
		{ \
			(reinterpret_cast<ThisClass*>(reinterpret_cast<size_t>(netVar) - CLASS_MEMBER_OFFSET(ThisClass, member)))->SetNetworkChanged( \
				CLASS_MEMBER_OFFSET(ThisClass, member)); \
		} \
	}; \
	NetVarType_##member member;

	#define NetVar(type, member) \
		class NetVarChanger_##member; \
		typedef CNetVar<type, NetVarChanger_##member> NetVarType_##member; \
		_NetVar(member)

	#define NetVarVector3f(member) \
		class NetVarChanger_##member; \
		typedef CNetVarVector3<float, NetVarChanger_##member> NetVarType_##member; \
		_NetVar(member)

	#define NetVarArray(type, member, count) \
		class NetVarChanger_##member; \
		typedef CNetVarArray<type, count, NetVarChanger_##member> NetVarType_##member; \
		_NetVar(member)
#endif /* _SERVER */

#endif // deferred__util__datadesc_makros_H__
