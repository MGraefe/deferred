// filesystem/file.h
// filesystem/file.h
// filesystem/file.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once

#include <string>
#include <ostream>
#include <istream>
#include <stdint.h>

namespace filesystem
{

class Folder;

//File object, used for (de)serializing the archive header
class File
{
public:
	friend class Folder;
	friend class ArchiveFileWriter_impl;

public:
	File(const std::string &name, uint32_t start, uint32_t size) :
		m_start(start),
		m_size(size),
		m_name(name)
	{}

	const std::string getName() const {
		return m_name;
	}

	uint32_t getStart() const {
		return m_start;
	}

	uint32_t getSize() const {
		return m_size;
	}

private:
	File(const std::string &name) : 
		m_name(name) 
	{}

	uint32_t m_start;
	uint32_t m_size;
	std::string m_name;
};

};

