// filesystem/filesystem.h
// filesystem/filesystem.h
// filesystem/filesystem.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once

#include <stdint.h>
#include "safe_strings.h"
#include "export_helpers.h"


namespace filesystem
{

	class fileinfo_t
	{
	public:
		fileinfo_t() :
			archiveName(NULL),
			archiveSize(0),
			fileSize(0),
			fileStart(0)
		{}

		const char *archiveName;
		uint32_t archiveSize;
		uint32_t fileSize;
		uint32_t fileStart;
	};

	typedef void* HANDLE;

	class findFileInfo_t
	{
	public:
		findFileInfo_t() :
			isRegularFile(false),
			isFolder(false),
			isVirtual(false),
			fileInfo()
		{
			name[0] = '\0';
		}

		findFileInfo_t(const char *name, bool isRegularFile, bool isFolder, bool isVirtual, const fileinfo_t &info)
		{
			strcpys(this->name, name);
			this->isRegularFile = isRegularFile;
			this->isFolder = isFolder;
			this->isVirtual = isVirtual;
			this->fileInfo = info;
		}

		char name[256];
		bool isRegularFile;
		bool isFolder;
		bool isVirtual; //Is the file a virtual file? (mounted in archive)
		fileinfo_t fileInfo; //Only valid if isVirtual is true
	};

	//Access to virtual file system
	class RPAF_DLL Filesystem
	{
	public:
		virtual bool mountArchive(const char *path) = 0;

		//Find a file
		virtual bool findFile(const char *path, fileinfo_t *fileInfo = NULL ) const = 0;
		
		//Returns a handle for calls to findNextFile and closeFindFile
		virtual HANDLE findFirstFile(const char *path, const char *filename, findFileInfo_t *info = NULL, bool fullRegex = false) = 0;

		//Supply handle returned by findFirstFile
		virtual bool findNextFile(HANDLE handle, findFileInfo_t *info = NULL) = 0;

		//Closes handle returned by findFirstFile
		virtual bool closeFindFile(HANDLE handle) = 0;

		//Get filesystem instance
		static Filesystem &GetInstance();
	};

}
