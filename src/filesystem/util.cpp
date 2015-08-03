// filesystem/util.cpp
// filesystem/util.cpp
// filesystem/util.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "util.h"
#include <stdint.h>
#include <assert.h>
#include <vector>
#include <algorithm>

namespace filesystem
{

void serializeString( const std::string &str, std::ostream &os )
{
	uint32_t length = (uint32_t)str.length();
	os.write((char*)&length, 4);
	os.write(str.c_str(), str.length());
}

//#define USE_UNSAFE_STRING_DESERIALZATION
void deserializeString( std::istream &is, std::string &str )
{
	uint32_t length = 0;
	is.read((char*)&length, 4);
#ifdef USE_UNSAFE_STRING_DESERIALZATION
	str.resize(length);
	is.read(&str[0], length);
#else
	std::vector<char> data(length);
	is.read(&data[0], length);
	str.assign(data.begin(), data.end());
#endif
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::vector<std::string> splitFilename( const std::string &fn )
{
	size_t slash, lastBegin = 0;
	std::vector<std::string> result;
	do {
		slash = fn.find_first_of("/\\", lastBegin);
		result.push_back(std::string(fn, lastBegin, slash-lastBegin));
		lastBegin = slash + 1;
	} while (slash != std::string::npos);
	return result;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void replaceBackslash( std::string &str )
{
	std::replace(str.begin(), str.end(), '\\', '/');
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::string getNormalizedPath( const std::string &path )
{
	std::string norm(path);
	std::transform(norm.begin(), norm.end(), norm.begin(), ::tolower);
	replaceBackslash(norm);
	return norm;
}

}