// filesystem/archiveFileWriter.cpp
// filesystem/archiveFileWriter.cpp
// filesystem/archiveFileWriter.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#if 0

#include "archiveFileWriter.h"
#include <fstream>
#include <assert.h>
#include "folder.h"
#include "util.h"

#include <iostream>
#include <iomanip>
#include <stack>

class NullStreamBuf : public std::streambuf 
{
public:
	virtual std::streamsize xsputn (const char * s, std::streamsize n)
	{
		return n;
	}
	virtual int overflow (int c)
	{
		return 1;
	}
};


class NullStream : public std::ostream {
public:
	NullStream() : std::ostream (&buf) {}
private:
	NullStreamBuf buf;
};



namespace filesystem
{

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void ArchiveFileWriter_impl::addFile( const std::string &virtualPath, const std::string &realPath )
{
	m_files[virtualPath] = realPath;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void ArchiveFileWriter_impl::serialize( std::ostream &os )
{
	//write magic number
	serializeBinary(sArchiveFileMagicNumber, os);
	serializeBinary(sArchiveFileVersion, os);

	uint32_t headerSize = calcHeaderSize();

	//serialize files and save info regarding file size etc
	std::streampos headerStart = os.tellp();
	os.seekp(headerStart + std::streamoff(headerSize));
	std::map<std::string, fileinfo_t> infomap;
	for(FileMap::const_reference file : m_files)
	{
		fileinfo_t info;
		info.start = (uint32_t)os.tellp() + headerSize;
		info.size = serialize(os, file.second);
		infomap[file.first] = info;
	}
	os.seekp(headerStart);

	Folder root("");
	buildTree(root, &infomap);
	root.serialize(os);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void ArchiveFileWriter_impl::serialize( std::string &filename )
{
	std::ofstream os(filename, std::ios::binary);
	serialize(os);
}

#define GIGABYTE (1024*1024*1024)
//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
uint32_t ArchiveFileWriter_impl::serialize( std::ostream &os, const std::string &realPath )
{
	std::ifstream is(realPath, std::ios::binary);
	if(!is.is_open())
		return 0;

	is.seekg(std::ios::end);
	int64_t fileSize = is.tellg();
	if(fileSize < 0 || fileSize >= 4ULL*GIGABYTE || (int64_t)os.tellp() + (int64_t)fileSize >= 4ULL*GIGABYTE)
		return 0;

	//read/write data in chunks
	const uint32_t bufferSize = 1024 * 1024; //1 MB buffer size seems ok to me
	char *buffer = new char[bufferSize];
	uint32_t bytesRemaining = (uint32_t)fileSize;
	while(bytesRemaining > 0)
	{
		uint32_t chunkSize = std::min(bytesRemaining, bufferSize);
		is.read(buffer, chunkSize);
		os.write(buffer, chunkSize);
		bytesRemaining -= chunkSize;
	}

	assert(bytesRemaining == 0);
	delete[] buffer;

	return (uint32_t)fileSize;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void ArchiveFileWriter_impl::buildTree(Folder &root, const std::map<std::string, ArchiveFileWriter_impl::fileinfo_t> *infomap)
{
	for(FileMap::const_reference file : m_files)
	{
		std::vector<std::string> fn = splitFilename(file.first);
		Folder *parent = &root;
		for(size_t i = 1; i < fn.size(); i++)
		{
			const std::string &name = fn[i-1];
			parent = const_cast<Folder*>(&*parent->addFolder(Folder(name)));
		}

		uint32_t size = 0;
		uint32_t start = 0;

		if(infomap)
		{
			auto it = infomap->find(file.first);
			size = it->second.size;
			start = it->second.start;
		}
		
		parent->addFile(File(fn.back(), start, size));
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
uint32_t ArchiveFileWriter_impl::calcHeaderSize()
{
	Folder virtualRoot("");
	buildTree(virtualRoot, NULL);

	NullStream ns;
	virtualRoot.serialize(ns);
	return (uint32_t)ns.tellp();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
ArchiveFileWriter RPAF_DLL *ArchiveFileWriter::create()
{
	return new ArchiveFileWriter_impl();
}


void testNullStream(void)
{
	NullStream str;
	assert((int)str.tellp() == 0);
	uint32_t dummy = 'a';
	str.write((char*)&dummy, 4);
	assert((int)str.tellp() == 4);
	char *dummyStr = new char[1024*1024];
	str.write(dummyStr, 1024*1024);
	assert((int)str.tellp() == 1024*1024+4);
	delete dummyStr;
}

}

#endif
