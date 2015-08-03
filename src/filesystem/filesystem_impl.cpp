// filesystem/filesystem_impl.cpp
// filesystem/filesystem_impl.cpp
// filesystem/filesystem_impl.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include <assert.h>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <fstream>
#include "util.h"
#include "zip.h"
#include "filesystem_impl.h"
#include "safe_strings.h"


namespace filesystem
{


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Filesystem_impl::Filesystem_impl()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Filesystem_impl::ArchiveFile::ArchiveFile() :
	m_path(),
	m_root(NULL)
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Filesystem_impl::ArchiveFile::~ArchiveFile()
{
	if(m_root)
		delete m_root;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool Filesystem_impl::ArchiveFile::open( const std::string &path )
{
	if(m_root)
	{
		delete m_root;
		m_root = NULL;
	}

	m_path = getNormalizedPath(path);

	std::ifstream is(m_path, std::ios::binary);
	if(!is.is_open())
		return false;

	is.seekg(0, std::ios::end);
	m_archiveSize = (uint32_t)is.tellg();
	is.seekg(0, std::ios::beg);

	return readZip(is);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const File* Filesystem_impl::ArchiveFile::findFile( const std::string &normPath ) const
{
	if(!m_root)
		return NULL;

	const Folder *currentFolder = m_root;
	size_t slash, lastSlash = 0;
	while( (slash = normPath.find('/', lastSlash)) != std::string::npos )
	{
		const std::string folderName(normPath, lastSlash, slash-lastSlash);
		currentFolder = currentFolder->getFolder(folderName);
		if(!currentFolder)
			return NULL;
		lastSlash = slash + 1;
	}

	const std::string filename(normPath, lastSlash);
	return currentFolder->getFile(filename);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool Filesystem_impl::ArchiveFile::readZip( std::ifstream &is )
{
	//read end of cd structure
	zip::eocd_t eocd;
	if(!eocd.deserialize(is))
		return false;
	
	//read central directory entries
	std::vector<zip::cdh_t> cdhs(eocd.numOfCDs);
	is.seekg(eocd.offsetOfCD, std::ios::beg);
	for(zip::cdh_t &cdh : cdhs)
		if(!cdh.deserialize(is))
			return false;

	assert(!m_root);
	m_root = new Folder("");
	
	for(const zip::cdh_t &cdh : cdhs)
	{
		zip::lfh_t lfh;
		is.seekg(cdh.fileOffset, std::ios::beg);
		if(!lfh.deserialize(is, cdh))
			return false;
		if(lfh.fileName.empty()) //filename empty?
			continue;
		bool isDirectory = lfh.fileName.back() == '/';

		std::string filePath = getNormalizedPath(std::string(lfh.fileName.begin(), lfh.fileName.end() - (isDirectory ? 1 : 0)));
		std::vector<std::string> splitPath = splitFilename(filePath);
		if(splitPath.empty())
			continue;

		Folder *currentFolder = m_root;
		for(size_t i = 0; i < splitPath.size() - (isDirectory ? 0 : 1); ++i)
		{
			auto it = currentFolder->addFolder(Folder(splitPath[i]));
			currentFolder = const_cast<Folder*>(&*it); //hacky but ok since we do not change the key of the folder (filename)
		}
		if(!isDirectory)
			currentFolder->addFile(File(splitPath.back(), lfh.fileStart, lfh.sizeCompressed));
	}

	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool Filesystem_impl::mountArchive( const char *path )
{
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string normPath = getNormalizedPath(path);

	//is the archive already mounted?
	for(const ArchiveFile *archive : m_archives)
		if(archive->getPath() == normPath)
			return false;

	ArchiveFile *archive = new ArchiveFile();
	if(archive->open(normPath))
	{
		m_archives.push_back(archive);
		return true;
	}
	else
	{
		delete archive;
		return false;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool Filesystem_impl::findFile( const char *path,
	fileinfo_t *fileInfo /*= NULL*/ ) const
{
	std::string normPath(getNormalizedPath(path));

	for(const ArchiveFile *archive : m_archives)
	{
		const File *file = archive->findFile(normPath);
		if(file)
		{
			if(fileInfo)
			{
				fileInfo->archiveName = archive->getPath().c_str();
				fileInfo->archiveSize = archive->getArchiveSize();
				fileInfo->fileSize = file->getSize();
				fileInfo->fileStart = file->getStart();
			}
			return true;
		}
	}

	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Filesystem_impl::~Filesystem_impl()
{
	for(ArchiveFile *archive : m_archives)
		if(archive)
			delete archive;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const std::vector<const Filesystem_impl::ArchiveFile*> Filesystem_impl::getArchives()
{
	return std::vector<const Filesystem_impl::ArchiveFile*>(m_archives.begin(), m_archives.end());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Filesystem_impl &Filesystem_impl::GetInstance()
{
	static Filesystem_impl fs;
	return fs;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::string regex_escape(const std::string& string_to_escape) {
	static const boost::regex re_boostRegexEscape( "[\\^\\.\\$\\|\\(\\)\\[\\]\\*\\+\\?\\/\\\\]" );
	const std::string rep( "\\\\\\1&" );
	std::string result = regex_replace(string_to_escape, re_boostRegexEscape, rep, boost::match_default | boost::format_sed);
	return result;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
HANDLE Filesystem_impl::findFirstFile( const char *path, const char *filename, findFileInfo_t *info /*= NULL*/, bool fullRegex /*= false*/ )
{
	m_mutex.lock();
		
	FindFileHandleData *handle = new FindFileHandleData();
	FindFileHandleData &data = *handle;

	//prepare path
	data.path = path;
	if(!fullRegex)
	{
		std::string sNewFn = regex_escape(filename);
		static const boost::regex re_boostRegexReplaceAsterix("\\\\\\*");
		std::string sNewFn2 = regex_replace(sNewFn, re_boostRegexReplaceAsterix,
			"[^\\\\^/]*", boost::match_default | boost::format_sed);
		data.regexFilename = sNewFn2;
	}
	else
		data.regexFilename = filename;

	try{
		data.dirIt = boost::filesystem::directory_iterator(data.path);
	} catch(boost::filesystem::filesystem_error e) {
		data.dirIt = boost::filesystem::directory_iterator(); //invalidate iterator
	}

	m_mutex.unlock();
	if(!findNextFile(handle, info))
	{
		closeFindFile(handle);
		return 0;
	}

	return handle;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool Filesystem_impl::findNextFile( HANDLE handle, findFileInfo_t *info /*= NULL*/ )
{
	std::lock_guard<std::mutex> l(m_mutex);

	FindFileHandleData *pdata = static_cast<FindFileHandleData*>(handle);
	FindFileHandleData &data = *pdata;

	//Search local fs
	boost::filesystem::directory_iterator itEnd;
	while(data.dirIt != itEnd)
	{
		std::string filename = data.dirIt->path().filename().string();
		if(regex_match(filename, data.regexFilename))
		{
			if(info)
			{
				strcpys(info->name, filename.c_str());
				info->isRegularFile = boost::filesystem::is_regular_file(data.dirIt->status());
				info->isFolder = boost::filesystem::is_directory(data.dirIt->status());
				info->isVirtual = false;
				info->fileInfo.archiveName = NULL;
				info->fileInfo.archiveSize = 0;
				info->fileInfo.fileSize = 0;
				info->fileInfo.fileStart = 0;
			}
			++data.dirIt; //increment before returning so we get another file next time
			return true;
		}
		++data.dirIt;
	}

	//Nothing found in local fs, check virtual one
	if(data.virtualFilesIndex < 0)
	{
		//Create virtual archive index
		data.virtualFilesIndex = 0;
		data.virtualFiles.clear();
		for(ArchiveFile *archive : m_archives)
		{
			const Folder *folder = archive->getRoot()->getFolderRecursive(data.path);
			if(folder)
			{
				for(const Folder &subfolder : folder->getFolders())
					if(regex_match(subfolder.getName(), data.regexFilename))
						data.virtualFiles.push_back(
							findFileInfo_t(subfolder.getName().c_str(), false, true, true,
								buildFileInfo(archive, &subfolder)));
				for(const File &subfile : folder->getFiles())
					if(regex_match(subfile.getName(), data.regexFilename))
						data.virtualFiles.push_back(
							findFileInfo_t(subfile.getName().c_str(), true, false, true,
								buildFileInfo(archive, &subfile)));
			}
		}
	}

	//No virtual files?
	if((size_t)data.virtualFilesIndex < data.virtualFiles.size())
	{
		if(info)
			*info = data.virtualFiles[data.virtualFilesIndex];
		data.virtualFilesIndex++;
		return true;
	}

	if(info)
	{
		info->isFolder = false;
		info->isRegularFile = false;
		info->isVirtual = false;
		info->name[0] = 0;
		info->fileInfo.archiveName = NULL;
		info->fileInfo.archiveSize = 0;
		info->fileInfo.fileSize = 0;
		info->fileInfo.fileStart = 0;
	}
	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool Filesystem_impl::closeFindFile( HANDLE handle )
{
	if(!handle)
		return false;

	std::lock_guard<std::mutex> l(m_mutex);

	FindFileHandleData *pdata = static_cast<FindFileHandleData*>(handle);
	delete pdata;

	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
fileinfo_t Filesystem_impl::buildFileInfo( const ArchiveFile *archive, const Folder *folder )
{
	fileinfo_t info;
	info.archiveName = archive->getPath().c_str();
	info.archiveSize = archive->getArchiveSize();
	info.fileSize = 0;
	info.fileStart = 0;
	return info;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
filesystem::fileinfo_t Filesystem_impl::buildFileInfo( const ArchiveFile *archive, const File *file )
{
	fileinfo_t info;
	info.archiveName = archive->getPath().c_str();
	info.archiveSize = archive->getArchiveSize();
	info.fileSize = file->getSize();
	info.fileStart = file->getStart();
	return info;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Filesystem &Filesystem::GetInstance()
{
	Filesystem_impl& (*fct)() = Filesystem_impl::GetInstance; //avoid inlining
	return fct();
}

}