// filesystem/folder.h
// filesystem/folder.h
// filesystem/folder.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once

#include <string>
#include <set>
#include "file.h"

namespace filesystem
{



class Folder
{
friend class ArchiveFileWriter_impl;

private:
	class FolderNameComparator
	{
	public:
		bool operator()(const Folder &l, const Folder &r) const {
			return l.getName() < r.getName(); 
		}
	};

	class FileNameComparator
	{
	public:
		bool operator()(const File &l, const File &r) const {
			return l.getName() < r.getName();
		}
	};

public:
	typedef std::set<Folder, FolderNameComparator> FolderSet;
	typedef std::set<File, FileNameComparator> FileSet;


public:
	//Folder(std::istream &is);
	Folder(const std::string &name);
	Folder(const std::string &name, const FolderSet &folders, const FileSet &files);

	const std::string &getName() const {
		return m_name;
	}

	const FolderSet &getFolders() const {
		return m_folders;
	}

	const FileSet &getFiles() const {
		return m_files;
	}

	//void serialize(std::ostream &os) const;
	//void deserialize(std::istream &is);

	const Folder *getFolder(const std::string &name) const
	{
		FolderSet::iterator it = m_folders.find(Folder(name));
		if(it == m_folders.end())
			return NULL;
		return &(*it);
	}

	const File *getFile(const std::string &name) const
	{
		FileSet::iterator it = m_files.find(File(name));
		if(it == m_files.end())
			return NULL;
		return &(*it);
	}

	const Folder *getFolderRecursive(const std::string &path) const
	{
		size_t slash = path.find_first_of("/\\");
		std::string fn(path.begin(), slash == std::string::npos ? path.end() : path.begin() + slash);

		const Folder *f = NULL;

		if(fn == "..")
			f = NULL; //TODO: support parenting
		if(fn == ".")
			f = this;
		else
			f = getFolder(fn);

		if(!f)
			return NULL;

		if(slash == std::string::npos)
			return f;
		else
			return f->getFolderRecursive(std::string(path.begin()+slash+1, path.end()));
	}

	FolderSet::iterator addFolder(const Folder &folder);
	FileSet::iterator addFile(const File &file);

private:
	//void serializeFolders( std::ostream & os ) const;
	//void serializeFiles( std::ostream & os ) const;
	//void deserializeFiles( std::istream & is );
	//void deserializeFolders( std::istream & is );


private:
	std::string m_name;
	FolderSet m_folders;
	FileSet m_files;
};

}

