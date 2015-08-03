// filesystem/archiveFileWriter.h
// filesystem/archiveFileWriter.h
// filesystem/archiveFileWriter.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once

#if 0
#include "fileout.h"
#include <map>
#include <stdint.h>
#include "include.h"

namespace filesystem
{

class Folder;

class ArchiveFileWriter_impl : public ArchiveFileWriter
{
public:
	void addFile(const std::string &virtualPath, const std::string &realPath);

	void serialize(std::ostream &os);
	void serialize(std::string &filename);

private:
	uint32_t serialize(std::ostream &os, const std::string &realPath);

	struct fileinfo_t
	{
		uint32_t start;
		uint32_t size;
	};

	void buildTree(Folder &root, const std::map<std::string, fileinfo_t> *infomap);

	typedef std::map<std::string, std::string> FileMap;
	FileMap m_files; //key is virtual path, value is real path

	uint32_t calcHeaderSize();
};


	void testNullStream();
}

#endif
