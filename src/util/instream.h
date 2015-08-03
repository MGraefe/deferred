// util/instream.h
// util/instream.h
// util/instream.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__instream_H__
#define deferred__util__instream_H__

#include <string>
#include <vector>
#include <filesystem/include.h>


//#define USE_NATIVE_INSTREAM
#ifndef USE_NATIVE_INSTREAM

namespace fs
{
	typedef ::filesystem::InStream ifstream;

	inline size_t getFileSize(fs::ifstream &is)
	{
		return is.getFileSize();
	}
}

#else
#include <fstream>

namespace fs
{
	typedef ::std::ifstream ifstream;
}
#endif

namespace fs
{
	inline size_t getFileSize(std::ifstream &is)
	{
		std::ifstream::pos_type pos = is.tellg();
		is.seekg(0, std::ios::end);
		size_t size = (size_t)is.tellg();
		is.seekg(pos);
		return size;
	}

	template<class T>
	inline T readTextUnixLineEnding(fs::ifstream &is)
	{
		size_t size = getFileSize(is);
		is.seekg(0, std::ios::beg);
		T out;
		out.reserve(size);
		while(true)
		{
			char ch = is.get();
			if(!is.good())
				break;
			if(ch != '\r')
				out.push_back(ch);
		}
		return out;
	}

	inline std::string readTextUnixLineEndingString(fs::ifstream &is)
	{
		return readTextUnixLineEnding<std::string>(is);
	}

	inline std::vector<char> readTextUnixLineEndingVector(fs::ifstream &is)
	{
		return readTextUnixLineEnding<std::vector<char> >(is);
	}
}


#endif // deferred__util__instream_H__
