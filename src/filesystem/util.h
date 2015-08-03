// filesystem/util.h
// filesystem/util.h
// filesystem/util.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once

#include <ostream>
#include <istream>
#include <vector>
#include <stdint.h>

namespace filesystem
{
template<typename T>
inline void serializeBinary(T data, std::ostream &os)
{
	static_assert(std::is_arithmetic<T>::value, "no integral type");
	os.write((char*)&data, sizeof(T));
}

template<typename T>
inline void deserializeBinary(std::istream &is, T &data)
{
	static_assert(std::is_arithmetic<T>::value, "no integral type");
	is.read((char*)&data, sizeof(T));
}

void serializeString(const std::string &str, std::ostream &os);
void deserializeString(std::istream &is, std::string &str);

std::vector<std::string> splitFilename(const std::string &fn);

void replaceBackslash(std::string &str);
std::string getNormalizedPath(const std::string &path);


//little endian "RPAF" for RedPuma Archive File
static const uint32_t sArchiveFileMagicNumber = (uint32_t)(('F' << 24) + ('A' << 16) + ('P' << 8) + 'R');
static const uint32_t sArchiveFileVersion = 1;
};

