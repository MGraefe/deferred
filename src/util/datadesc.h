// util/datadesc.h
// util/datadesc.h
// util/datadesc.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//datadesc.h

#pragma once
#ifndef deferred__util__datadesc_H__
#define deferred__util__datadesc_H__

#include "basic_types.h"
#include "CScriptParser.h"
#include "CNetVar.h"
#include "networkStreams.h"

enum datadesc_type_e
{
	//single int
	DATA_INT = 0,

	//array of ints
	DATA_INT_ARRAY,

	//single bool
	DATA_BOOL,

	//single float
	DATA_FLOAT,

	//array of floats
	DATA_FLOAT_ARRAY,

	//Array of floats with 4 elements
	DATA_FLOAT4,

	//Vector2d
	DATA_VECTOR2,

	//Vector3f
	DATA_VECTOR3,

	//Vector3f is normalized after read
	DATA_VECTOR3_NORMALIZED,

	//Vector4d
	DATA_VECTOR4,

	//Vector4d is normalized after read
	DATA_VECTOR4_NORMALIZED,

	//x, y and z are divided by 255
	DATA_COLOR3,

	//x, y and z are divided by 255
	DATA_COLOR4,

	//C++ style string (std::string)
	DATA_STRING,

	//C style string (char[])
	DATA_CHAR_ARRAY, 

	//Rotation stored as euler angles in script, but as Quaternion inside CPP
	DATA_ANGLES_TO_QUATERNION,

	DATA_INPUTFUNC,
};


template<typename T>
class inputdata_t
{
public:
	inputdata_t(T *activator, T *caller, const std::string &param) :
		activator(activator),
		caller(caller),
		param(param)
	{}

	T *GetActivator() const { return activator; }
	T *GetCaller() const { return caller; }
	const std::string &GetParam() const { return param; }

private:
	T *activator;
	T *caller;
	std::string param;
};


template<typename EntityT>
class outputdata_t
{
public:
	outputdata_t(EntityT *source, const inputdata_t<EntityT> &inputData,
		const std::string &target, const std::string &input, float processTime) :
		m_source(source),
		m_inputData(inputData),
		m_target(target),
		m_input(input),
		m_processTime(processTime)
	{}

	//less operator for priority queue
	bool operator<(const outputdata_t<EntityT> &other) const
	{
		return m_processTime > other.m_processTime;
	}

	inline EntityT *getSource() const { return m_source; }
	inline const inputdata_t<EntityT> &getInputData() const { return m_inputData; }
	inline const std::string &getTarget() const { return m_target; }
	inline const std::string &getInput() const { return m_input; }
	inline float getProcessTime() const { return m_processTime; }

private:
	EntityT *m_source;
	inputdata_t<EntityT> m_inputData;
	std::string m_target;
	std::string m_input;
	float m_processTime;	
};


class CDatadescEntry
{
public:
	CDatadescEntry( datadesc_type_e Type, int Count, size_t DataOfs, int Flags, const char *Varname );
	bool ReadFromScript( CScriptSubGroup *pScript, void *dataPtr );

public:
	datadesc_type_e type;
	int count; //for array types, otherwise = 1
	size_t dataOfs; //member offset into data
	int flags;
	std::string varname; //name as in script
};

//										stream  member  size  count
typedef bool (*netvarproxyreadfunc_t)(InStream&, void*, UINT, UINT, UINT);
typedef bool (*netvarproxywritefunc_t)(OutStream&, void*, UINT, UINT, UINT);
typedef size_t (*netvarproxyreadsizefunc_t)(InStream&);
#ifdef _SERVER
	typedef netvarproxywritefunc_t netvarproxyfunc_t;
	typedef void* netvarproxysizefunc_t;
#else
	typedef netvarproxyreadfunc_t netvarproxyfunc_t;
	typedef netvarproxyreadsizefunc_t netvarproxysizefunc_t;
#endif

enum netvarinterptype_e
{
	INTERP_NONE = 0,
	INTERP_FLOAT,
	INTERP_VECTOR,
	INTERP_QUATERNION,
};


class CNetworkTableEntry
{
public:
	typedef unsigned short Idtype;
	CNetworkTableEntry(size_t offset, size_t memberSize, size_t networkSize, int count, int flags, Idtype id,
		netvarproxyfunc_t proxyFunc, netvarproxysizefunc_t proxySizeFunc = NULL, netvarinterptype_e interpType = INTERP_NONE) :
		offset(offset),
		memberSize(memberSize),
		networkSize(networkSize),
		count(count),
		flags(flags),
		id(id),
		proxyFunc(proxyFunc),
		proxySizeFunc(proxySizeFunc),
		interpType(interpType)
	{}

public:
	size_t offset; //member offset into data
	size_t memberSize;
	size_t networkSize; //network size is size_t(-1) for dynamic sized types
	int count; //for array types
	int flags;
	Idtype id;
	netvarproxyfunc_t proxyFunc;
	netvarproxysizefunc_t proxySizeFunc; //only valid if networkSize is size_t(-1)
	netvarinterptype_e interpType;
};


template<typename T>
struct inputfunc_t
{
	typedef void (T::*type)(const inputdata_t<T>&);
};


template<typename T>
class CDatadescInputEntry
{
public:
	CDatadescInputEntry( void(T::*func)(const inputdata_t<T>&), const char *varname ) :
		func(func),
		varname(varname)
	{}

public:
	void(T::*func)(const inputdata_t<T>&);
	const char *varname;
};


template<typename T>
class DatadescList
{
public:
	typedef std::map<size_t, CNetworkTableEntry> NetTable;

public:
	DatadescList(const char *entityClassName) {
		m_baseList = NULL;
		m_entityClassName = entityClassName;
		m_initialized = false;
	}

	inline void AddInput( const CDatadescInputEntry<T> &input ) {
		m_inputs.push_back(input);
	}

	inline void AddEntry( const CDatadescEntry &entry ) {
		m_entries.push_back(entry); 
	}

	inline void AddNetworkEntry( CNetworkTableEntry entry ) {
		entry.id = m_networkTable.size();
		if( m_networkTable.insert(NetTable::value_type(entry.offset, entry)).second == false )
			error_fatal("Error in networking table of entity %s", m_entityClassName.c_str());
	}

	inline const CNetworkTableEntry *GetNetworkEntryByOffset(size_t offset) const {
		auto it = m_networkTable.find(offset);
		return it != m_networkTable.end() ? &it->second : NULL;
	}

	inline const CNetworkTableEntry *GetNetworkEntryById(CNetworkTableEntry::Idtype id) const {
		for(auto it = m_networkTable.begin(); it != m_networkTable.end(); ++it)
			if(it->second.id == id)
				return &it->second;
		return NULL;
	}

	inline const NetTable &GetNetworkTable() const {
		return m_networkTable;
	}

	inline size_t size( void ) const {
		return m_entries.size();
	}

	inline void ReadFromScript( CScriptSubGroup *pScript, void *dataPtr ) {
		CScriptSubGroup *grpKeyVals = pScript->GetSubGroup("keyvals");
		for( UINT i = 0; i < m_entries.size(); i++ )
			m_entries[i].ReadFromScript(grpKeyVals, dataPtr);
		if( m_baseList )
			m_baseList->ReadFromScript(pScript, dataPtr);
	}

	inline void SetBase( DatadescList *base ) {
		if(m_baseList != NULL)
			return;
		m_baseList = base;
		if( base )
			m_networkTable = base->m_networkTable;
	}

	inline const CDatadescInputEntry<T> *GetInputEntry( const std::string &name ) const {
		const char *pchName = name.c_str();
		for( const CDatadescInputEntry<T> &entry : m_inputs )
			if( strcmp(entry.varname, pchName) == 0 )
				return &entry;

		if( m_baseList )
			return m_baseList->GetInputEntry(name);
		else
			return NULL;
	}

	inline const std::string &GetEntityClassName() const {
		return m_entityClassName;
	}

	inline bool IsInitialized() const {
		return m_initialized;
	}

	inline void SetInitialized() {
		m_initialized = true;
	}

private:
	std::vector<CDatadescInputEntry<T>> m_inputs;
	std::vector<CDatadescEntry> m_entries;
	NetTable m_networkTable;
	DatadescList *m_baseList; //List of base class, NULL if no base class
	std::string m_entityClassName;
	bool m_initialized;
};



//_T is the base class type (IRenderEntity for Client, CBaseEntity for Server)
template<typename _T>
class IEntityFactoryEditor
{
public:
	typedef std::map<std::string, IEntityFactoryEditor<_T>*, LessThanCppStringObj> EditorFactoryMap;

public:
	IEntityFactoryEditor( EditorFactoryMap *map, const std::string &className ) :
		m_className(className)
	{
		(*map)[className] = this;
	}

	virtual _T *CreateEntity( CScriptSubGroup *pScript, int index ) const = 0;

	const std::string &GetEntityClassName() const { return m_className; }
private:
	const std::string m_className;
};

	
#endif // deferred__util__datadesc_H__
