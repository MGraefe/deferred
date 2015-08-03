// util/StringUtils.h
// util/StringUtils.h
// util/StringUtils.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//StringUtils.h

#pragma once
#ifndef deferred__util__StringUtils_H__
#define deferred__util__StringUtils_H__

#include "maths.h"
namespace StrUtils
{
	std::string getPathFromFilePath( const std::string &filepath, bool includeLastSlash = false );

	Vector4f GetVector4( const char *str );
	Vector3f GetVector3( const char *str );
	Vector2f GetVector2( const char *str );

	int  GetInt( const char *str );
	float GetFloat( const char *str );
	bool GetFloat4( const char *str, float *value );
	bool GetFloats( const char *str, int iNum, float *value );
	bool GetInts( const char *str, int iNum, int *value );

	Vector4f GetVector4( const std::string &str );
	Vector3f GetVector3( const std::string &str );
	Vector2f GetVector2( const std::string &str );

	int  GetInt( const std::string &str );
	float GetFloat( const std::string &str );
	bool GetFloat4( const std::string &str, float *value );
	bool GetFloats( const std::string &str, int iNum, float *value );
	bool GetInts( const std::string &str, int iNum, int *value );


	//returns number of erased chars
	int strip_leading_spaces_or_tabs( std::string &str );

	//returns number of erased chars
	int strip_trailing_spaces_or_tabs( std::string &str );

	//remove leading and trailing spaces or tabs
	//returns the number of erased chars
	int trim( std::string &str );

	//returns true if string is numeric: [-]{0-9}[.]{0-9}
	bool is_numeric( const std::string &str );

	void replaceFileExtension(std::string &str, const std::string &newExt);

	void binarizeString(std::ostream &os, const std::string &str);
	void debinarizeString(std::istream &is, std::string &str);

	template<typename T, typename FunctorT>
	inline bool GetSpaceDividedValues( const std::string &str, int iNum, T values, FunctorT ConversionFunctor )
	{
		size_t start = 0;
		for( int i = 0; i < iNum; i++ )
		{
			start = str.find_first_not_of(" \t", start);
			if(start == std::string::npos)
				return false;
			size_t spacePos = str.find_first_of(" \t\0", start);
			if(spacePos == str.npos)
				spacePos = str.size();
			std::string v(str, start, spacePos-start);
			values[i] = ConversionFunctor(v);
			start = spacePos;
		}
		return true;
	}

	template<typename T>
	inline bool GetSpaceDividedValues( const std::string &str, int iNum, T values )
	{
		return GetSpaceDividedValues(str, iNum, values, [](const std::string &str){return str;});
	}
}


#endif // deferred__util__StringUtils_H__
