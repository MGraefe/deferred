// util/CScriptParser.h
// util/CScriptParser.h
// util/CScriptParser.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__CScriptParser_H__
#define deferred__util__CScriptParser_H__

#include <map>
#include <string>
#include "maths.h"
#include "CColor.h"
#include "basic_types.h"

#define SP_INVALID_INT INT_MAX
#define SP_INVALID_FLOAT (-98765432.1f)

using std::string;

class CScriptSubGroup;

typedef std::multimap<std::string, std::string, LessThanCppStringObj> KeyValueMap;
typedef std::multimap<std::string, CScriptSubGroup*, LessThanCppStringObj> SubGroupMap;

class CScriptSubGroup
{
public:
	~CScriptSubGroup();

	bool GetString( const char *key, char *value, int valueBufSize ) const;
	bool GetString( const char *key, std::string &value ) const;
	bool GetString( const std::string &key, std::string &value ) const;
	int  GetInt( const char *key, int iDefault = SP_INVALID_INT ) const;
	float GetFloat( const char *key, float fDefault = SP_INVALID_FLOAT ) const;
	bool GetFloat4( const char *key, float *value ) const;
	bool GetFloats( const char *key, int iNum, float *value ) const;
	bool GetInts( const char *key, int iNum, int *value ) const;
	Vector4f GetVector4d( const char *key, Vector4f vDefault = vec4_null ) const;
	Vector3f GetVector3f( const char *key, Vector3f vDefault = vec3_null ) const;
	Vector2f GetVector2d( const char *key, Vector2f vDefault = vec2_null ) const;
	Color GetColor( const char *key, Color cDefault = Color(0,0,0,0) ) const;

	const SubGroupMap *GetSubGroupMap( void ) const { return &m_SubGroupMap; }
	const KeyValueMap *GetKeyValueMap( void ) const { return &m_KeyValueMap; }

	//Wrapper around GetStringMapPrefixRange
	std::pair<KeyValueMap::const_iterator, KeyValueMap::const_iterator>
		GetPrefixRange( const std::string &prefix, int &count ) const;

	//Wrapper around GetStringMapPrefixRange
	std::pair<KeyValueMap::iterator, KeyValueMap::iterator> 
		GetPrefixRange( const std::string &prefix, int &count );

	template<typename T>
	bool GetSpaceDividedValues( const char *key, int iNum, T *value,
		T (*ConversionFunc)(const char*) ) const;

	CScriptSubGroup *GetSubGroup( const char *key ) const;
	
protected:
	error_e ParseSubGroup( string &str, int &ptr );
	KeyValueMap m_KeyValueMap;
	SubGroupMap m_SubGroupMap;
};


class CScriptParser : public CScriptSubGroup
{
public:
	CScriptParser();
	bool ParseFile( const char *filename );
	bool ParseFile( const std::string &filename );
	bool ParseFileString(const char *str);
	void FindSubString( const char *key );

private:
	error_e _ParseFile( const char *filename );
	error_e _ParseFileString( std::string &str );

	bool m_bOpened;
	char m_fileName[256];
	error_e m_error;
};

#endif
