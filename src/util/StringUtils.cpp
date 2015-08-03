// util/StringUtils.cpp
// util/StringUtils.cpp
// util/StringUtils.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "StringUtils.h"
#include <string>

namespace StrUtils
{
using std::string;

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::string getPathFromFilePath( const std::string &filepath, bool includeLastSlash )
{
	size_t lastSlash = filepath.find_last_of( "/\\" );
	if( lastSlash != filepath.npos )
		return string( filepath, 0, lastSlash + (includeLastSlash?1:0) );
	else
		return string( "" );
}


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


////---------------------------------------------------------------
//// Purpose: 
////---------------------------------------------------------------
//template<typename T>
//bool GetSpaceDividedValues( const char *str, int iNum, T *value, T (*ConversionFunc)(const char*) )
//{
//	int ptr = 0;
//	for( int i = 0; i < iNum; i++ )
//	{
//		char chVal[32];
//		char delimiter = ' ';
//		if( i == iNum-1 )
//			delimiter = '\0';
//
//		ptr += ReadUntilDelimiter( str+ptr, chVal, delimiter, 32 );
//		if( ptr < 0 )
//		{
//			error( "Error parsing floats" );
//			return false;
//		}
//
//		value[i] = ConversionFunc( chVal );
//	}
//
//	return true;
//}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector4f GetVector4( const char *str )
{
	Vector4f ret;
	GetFloats( str, 4, (float*)&ret );
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector3f GetVector3( const char *str )
{
	Vector3f ret;
	GetFloats( str, 3, (float*)&ret );
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector2f GetVector2( const char *str )
{
	Vector2f ret;
	GetFloats( str, 2, (float*)&ret );
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int GetInt( const char *str )
{
	return atoi(str);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float GetFloat( const char *str )
{
	return (float)atof(str);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool GetFloat4( const char *str, float *value )
{
	return GetFloats( str, 4, value );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool GetFloats( const char *str, int iNum, float *value )
{
	return GetSpaceDividedValues( std::string(str), iNum, value, static_cast<float (*)(const std::string &)>(GetFloat) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool GetInts( const char *str, int iNum, int *value )
{
	return GetSpaceDividedValues( std::string(str), iNum, value, static_cast<int (*)(const std::string &)>(GetInt) );
}



//moved to header
////---------------------------------------------------------------
//// Purpose: 
////---------------------------------------------------------------
//template<typename T, typename FunctorT>
//bool GetSpaceDividedValues( const std::string &str, int iNum, T *value, FunctorT ConversionFunctor )
//{
//	int ptr = 0;
//	for( int i = 0; i < iNum; i++ )
//	{
//		char chVal[32];
//		char delimiter = ' ';
//		if( i == iNum-1 )
//			delimiter = '\0';
//
//		ptr += ReadUntilDelimiter( str.c_str()+ptr, chVal, delimiter, 32 );
//		if( ptr < 0 )
//		{
//			error( "Error parsing floats" );
//			return false;
//		}
//
//		value[i] = ConversionFunctor( chVal );
//	}
//
//	return true;
//}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector4f GetVector4( const std::string &str )
{
	Vector4f ret;
	GetFloats( str, 4, (float*)&ret );
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector3f GetVector3( const std::string &str )
{
	Vector3f ret;
	GetFloats( str, 3, (float*)&ret );
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector2f GetVector2( const std::string &str )
{
	Vector2f ret;
	GetFloats( str, 2, (float*)&ret );
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int GetInt( const std::string &str )
{
	return atoi(str.c_str());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float GetFloat( const std::string &str )
{
	return (float)atof(str.c_str());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool GetFloat4( const std::string &str, float *value )
{
	return GetFloats( str, 4, value );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool GetFloats( const std::string &str, int iNum, float *value )
{
	return GetSpaceDividedValues( str, iNum, value, static_cast<float (*)(const std::string &)>(GetFloat) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool GetInts( const std::string &str, int iNum, int *value )
{
	return GetSpaceDividedValues( str, iNum, value, static_cast<int (*)(const std::string &)>(GetInt) );
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int strip_leading_spaces_or_tabs( std::string &str )
{
	size_t length = str.length();

	size_t found = str.find_first_not_of( " \t" );
	if( found != std::string::npos )
		str.erase( 0, found );
	else
		str.resize(0);

	return length - str.length();
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int strip_trailing_spaces_or_tabs( std::string &str )
{
	size_t length = str.length();

	size_t found = str.find_last_not_of( " \t" );
	if( found != std::string::npos )
		str.resize( found+1 );
	else
		str.resize(0);

	return length - str.length();
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int trim( std::string &str )
{
	int lead = strip_leading_spaces_or_tabs(str);
	int trail = strip_trailing_spaces_or_tabs(str);
	return lead+trail;
}


//---------------------------------------------------------------
// Purpose: [-]{0-9}[.]{0-9}
//---------------------------------------------------------------
bool is_numeric( const std::string &str )
{
	bool dotValid = false;
	bool hadDot = false;
	bool signValid = true;

	for( size_t i = 0; i < str.length(); i++ )
	{
		if( str[i] == '-' )
		{
			if( signValid )
				signValid = false;
			else
				return false;
		}
		else if( str[i] == '.' )
		{
			if( dotValid )
			{
				dotValid = false;
				hadDot = true;
			}
			else
				return false;
		}
		else if( str[i] >= '0' && str[i] <= '9' )
		{
			signValid = false;
			if( !hadDot )
				dotValid = true;
		}
		else
			return false;
	}

	return true;
}


void replaceFileExtension(string &str, const string &newExt)
{
	size_t dotPos = str.find_last_of('.');
	if( dotPos != string::npos )
		str.erase(str.begin()+dotPos, str.end());
	if(!newExt.empty() && newExt[0] != '.')
		str.append(".");
	str.append(newExt);
}


void binarizeString(std::ostream &os, const std::string &str)
{
	Assert(str.length() < std::numeric_limits<USHORT>::max());
	USHORT size = str.length();
	WRITE_BINARY(os, size);
	os.write(&str[0], size);
}

void debinarizeString(std::istream &is, std::string &str)
{
	USHORT size = 0;
	READ_BINARY(is, size);
	str.resize(size);
	is.read(&str[0], size);
}

}
