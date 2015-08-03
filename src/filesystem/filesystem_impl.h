// filesystem/filesystem_impl.h
// filesystem/filesystem_impl.h
// filesystem/filesystem_impl.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once

#include <string>
#include <vector>
#include <map>
#include "folder.h"
#include "filesystem.h"
#include <mutex>


namespace filesystem
{

class Filesystem_impl : public Filesystem
{

private:
	class ArchiveFile
	{
	public:
		ArchiveFile();
		~ArchiveFile();
		bool open(const std::string &path);
		const File* findFile(const std::string &normPath) const;
		const std::string &getPath() const {
			return m_path;
		}

		const Folder *getRoot() const {
			return m_root;
		}

		uint32_t getArchiveSize() const {
			return m_archiveSize;
		}

	private:
		bool readZip( std::ifstream &is );

		std::string m_path;
		Folder *m_root;
		uint32_t m_archiveSize;
	};

	class FindFileHandleData
	{
	public:
		FindFileHandleData() :
			virtualFiles(),
			virtualFilesIndex(-1),
			dirIt()
		{}

		std::vector<findFileInfo_t> virtualFiles;
		int virtualFilesIndex;
		boost::filesystem::directory_iterator dirIt;
		boost::regex regexFilename;
		std::string path;
	};

public:
	Filesystem_impl();
	~Filesystem_impl();
	bool mountArchive(const char *path);
	bool findFile(const char *path, fileinfo_t *fileInfo = NULL ) const;
	const std::vector<const ArchiveFile*> getArchives();
	static Filesystem_impl &GetInstance();

	static fileinfo_t buildFileInfo(const ArchiveFile *archive, const Folder *folder);
	static fileinfo_t buildFileInfo(const ArchiveFile *archive, const File *file);

	//Returns a handle for calls to findNextFile and closeFindFile
	virtual HANDLE findFirstFile(const char *path, const char *filename, findFileInfo_t *info = NULL, bool fullRegex = false) override;

	//Supply handle returned by findFirstFile
	virtual bool findNextFile(HANDLE handle, findFileInfo_t *info = NULL) override;

	//Closes handle returned by findFirstFile
	virtual bool closeFindFile(HANDLE handle) override;

	std::mutex m_mutex;

private:
	std::vector<ArchiveFile*> m_archives;
};

}
