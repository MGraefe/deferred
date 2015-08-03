// util/CScriptParser.cpp
// util/CScriptParser.cpp
// util/CScriptParser.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CScriptParser.h"
#include "instream.h"
#include "error.h"

#define MAX_LINE_LENGTH 1024


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int ReadUntilDelimiter( const char *source, char *dest, char delimiter, int maxSize )
{
	int i = 0;
	while( i < maxSize-1 )
	{
		if( source[i] == delimiter )
		{
			dest[i] = '\0';
			return i+1;
		}

		dest[i] = source[i];
		i++;
	}

	//when we get here we reached maxSize
	dest[maxSize-1] = '\0';
	return maxSize;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void RemoveComments( string &str )
{
	while( true )
	{
		//Find beginning of comment
		//BUGBUG: // inside double brackets is recognized as a comment
		size_t iCmt = str.find( "//" );
		if( iCmt == string::npos )
			return;

		//Find end of comment
		size_t iEoC = str.find( '\n', iCmt );
		if( iEoC == string::npos )
			iEoC = str.length();

		str.erase( iCmt, iEoC-iCmt+1 );
	}
}



CScriptParser::CScriptParser()
{
	m_bOpened = false;
}

//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
bool CScriptParser::ParseFile( const char *filename )
{
	m_error = _ParseFile(filename);
	if(m_error != ERR_NONE)
		ConsoleMessage("CScriptParser: Error parsing File \"%s\": %s", filename, ToString(m_error));
	return m_error == ERR_NONE;
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
bool CScriptParser::ParseFile( const std::string &filename )
{
	return ParseFile(filename.c_str());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
error_e CScriptParser::_ParseFile( const char *filename )
{
	strcpy( m_fileName, filename );

	fs::ifstream is( filename );
	if( !is.is_open() )
		return ERR_FILENOTFOUND; //File not found

	//Read the whole file into our string
	string str;
	std::getline( is, str, '\0' );
	is.close();

	return _ParseFileString(str);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CScriptParser::ParseFileString( const char *str )
{
	std::string s(str);
	m_error = _ParseFileString(s);
	if(m_error != ERR_NONE)
		ConsoleMessage("CScriptParser: Error parsing File String: %s", ToString(m_error));
	return m_error == ERR_NONE;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
error_e CScriptParser::_ParseFileString( std::string &str )
{
	int ptr = 0;
	RemoveComments(str);

	error_e err = ParseSubGroup( str, ptr );
	m_bOpened = err == ERR_NONE;
	return err;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CScriptParser::FindSubString( const char *key )
{
	int count = 0;
	auto rangePair = this->GetPrefixRange( string("Max"), count ); //This needs aprox. 60 ns

	ConsoleMessage( "Found %i items:", count );

	KeyValueMap::const_iterator it = rangePair.first;
	KeyValueMap::const_iterator itend = rangePair.second;
	for(; it != itend; it++ )
		ConsoleMessage( "Found key: \"%s\"", it->first.c_str() );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CScriptSubGroup::~CScriptSubGroup()
{
	
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CScriptSubGroup::GetString( const char *key, char *value, int valueBufSize ) const
{
	string sValue;
	if(GetString( key, sValue ))
	{
		int charsWritten = sValue.copy( value, valueBufSize-1 );
		value[charsWritten] = '\0';
		return true;
	}
	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CScriptSubGroup::GetString( const char *key, std::string &value ) const
{
	return GetString( string(key), value );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CScriptSubGroup::GetString( const std::string &key, std::string &value ) const
{
	KeyValueMap::const_iterator it;
	it = m_KeyValueMap.find( key );

	if( it == m_KeyValueMap.end() )
		return false;

	value = it->second;
	return true;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
error_e CScriptSubGroup::ParseSubGroup( string &str, int &ptr )
{
	while( true )
	{
		//Get beginning of key(or group) name
		size_t keystart = str.find_first_of( "\"}", ptr );

		//End of file?
		if( keystart == string::npos )
			return ERR_NONE;

		//End of group?
		if( str[keystart] == '}' )
		{
			ptr = keystart+1;
			return ERR_NONE;
		}

		//Find end of key (or group) name
		size_t keyend = str.find( '"', keystart+1 );
		if( keyend == string::npos )
			return ERR_CORRUPT_FILE;

		//Find beginning of value or group
		size_t valstart = str.find_first_of( "\"{", keyend+1 );
		if( valstart == string::npos )
			return ERR_CORRUPT_FILE;

		//A group started
		if( str[valstart] == '{' )
		{
			//This is a sub-group
			ptr = valstart+1;
			const string sKey( str, keystart+1, keyend-keystart-1 ); //group-name
			CScriptSubGroup *pSubGrp = new CScriptSubGroup;
			error_e err = pSubGrp->ParseSubGroup( str, ptr );//parse sub-group
			if(err != ERR_NONE)
				return err;
			m_SubGroupMap.insert( std::make_pair( sKey, pSubGrp ) ); //insert subgroup into our map
		}
		else //normal parameter
		{
			//Get end of value
			size_t valend = str.find( '"', valstart+1 );
			if( valend == string::npos )
				return ERR_CORRUPT_FILE;

			//Fill key/value pair into map
			const string sKey( str, keystart+1, keyend-keystart-1 );
			const string sVal( str, valstart+1, valend-valstart-1 );

			m_KeyValueMap.insert( std::pair<string,string>( sKey, sVal ) );

			ptr = valend+1;
		}
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CScriptSubGroup::GetInt( const char *key, int iDefault ) const
{
	auto it = m_KeyValueMap.find( string(key) );
	return it == m_KeyValueMap.end() ? iDefault : atoi(it->second.c_str());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CScriptSubGroup::GetFloat( const char *key, float fDefault ) const
{
	auto it = m_KeyValueMap.find(string(key));
	return it == m_KeyValueMap.end() ? fDefault : (float)atof(it->second.c_str());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CScriptSubGroup::GetFloat4( const char *key, float *value ) const
{
	return GetFloats( key, 4, value );
}

float atoff( const char *string )
{
	return (float)atof(string);
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CScriptSubGroup::GetFloats( const char *key, int iNum, float *value ) const
{
	return GetSpaceDividedValues<float>( key, iNum, value, &atoff );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector3f CScriptSubGroup::GetVector3f( const char *key, Vector3f vDefault ) const
{
	Vector3f ret;
	if( !GetFloats( key, 3, (float*)(&ret) ) )
		return vDefault;
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector4f CScriptSubGroup::GetVector4d( const char *key, Vector4f vDefault ) const
{
	Vector4f ret;
	if( !GetFloats( key, 4, (float*)(&ret) ) )
		return vDefault;
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector2f CScriptSubGroup::GetVector2d( const char *key, Vector2f vDefault ) const
{
	Vector2f ret;
	if( !GetFloats( key, 2, (float*)(&ret) ) )
		return vDefault;
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Color CScriptSubGroup::GetColor( const char *key, Color cDefault ) const
{
	int vals[4];
	if( GetInts(key, 4, vals) )
		return Color( (BYTE)vals[0], (BYTE)vals[1], (BYTE)vals[2], (BYTE)vals[3] );
	else if( GetInts(key, 3, vals) )
		return Color( (BYTE)vals[0], (BYTE)vals[1], (BYTE)vals[2] );
	else
		return cDefault;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CScriptSubGroup::GetInts( const char *key, int iNum, int *value ) const
{
	return GetSpaceDividedValues<int>( key, iNum, value, &atoi );
}


template<typename T>
bool CScriptSubGroup::GetSpaceDividedValues( const char *key, int iNum, T *value,
	T (*ConversionFunc)(const char*) ) const
{
	string str(key);
	KeyValueMap::const_iterator it = m_KeyValueMap.find( str );

	if( it == m_KeyValueMap.end() )
		return false;

	int ptr = 0;
	for( int i = 0; i < iNum; i++ )
	{
		char chFloat[32];
		char delimiter = ' ';
		if( i == iNum-1 )
			delimiter = '\0';
		
		ptr += ReadUntilDelimiter( it->second.c_str()+ptr, chFloat, delimiter, 32 );
		if( ptr < 0 )
		{
			error( "Error parsing floats" );
			return false;
		}

		value[i] = ConversionFunc( chFloat );
	}

	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CScriptSubGroup *CScriptSubGroup::GetSubGroup( const char *key ) const
{
	SubGroupMap::const_iterator it = m_SubGroupMap.find( string(key) );
	
	if( it == m_SubGroupMap.end() )
		return NULL;

	return it->second;
}


//---------------------------------------------------------------
// Purpose: Wrapper around GetStringMapPrefixRange
//---------------------------------------------------------------
std::pair<KeyValueMap::const_iterator, KeyValueMap::const_iterator> 
	CScriptSubGroup::GetPrefixRange( const std::string &prefix, int &count ) const
{
	return GetStringMapPrefixRange( m_KeyValueMap, prefix, count );
}


//---------------------------------------------------------------
// Purpose: Wrapper around GetStringMapPrefixRange
//---------------------------------------------------------------
std::pair<KeyValueMap::iterator, KeyValueMap::iterator> 
	CScriptSubGroup::GetPrefixRange( const std::string &prefix, int &count )
{
	return GetStringMapPrefixRange( m_KeyValueMap, prefix, count );
}
