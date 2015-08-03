// filesystem/folder.cpp
// filesystem/folder.cpp
// filesystem/folder.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "folder.h"
#include "util.h"

namespace filesystem
{


////---------------------------------------------------------------
//// Purpose: 
////---------------------------------------------------------------
//Folder::Folder( std::istream &is )
//{
//	deserialize(is);
//}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Folder::Folder( const std::string &name ) :
	m_name(name)
{
	
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Folder::Folder( const std::string &name, const FolderSet &folders, const FileSet &files ) :
	m_name(name),
	m_folders(folders),
	m_files(files)
{

}


////---------------------------------------------------------------
//// Purpose: 
////---------------------------------------------------------------
//void Folder::serialize( std::ostream &os ) const
//{
//	serializeString(m_name, os);
//	serializeFolders(os);
//	serializeFiles(os);
//}


////---------------------------------------------------------------
//// Purpose: 
////---------------------------------------------------------------
//void Folder::deserialize( std::istream &is )
//{
//	deserializeString(is, m_name);
//	deserializeFolders(is);
//	deserializeFiles(is);
//}


////---------------------------------------------------------------
//// Purpose: 
////---------------------------------------------------------------
//void Folder::deserializeFolders( std::istream & is )
//{
//	uint32_t lengthFolders = 0;
//	deserializeBinary(is, lengthFolders);
//	m_folders.clear();
//	for(uint32_t i = 0; i < lengthFolders; i++)
//		m_folders.insert(Folder(is));
//}


////---------------------------------------------------------------
//// Purpose: 
////---------------------------------------------------------------
//void Folder::deserializeFiles( std::istream & is )
//{
//	uint32_t lengthFiles = 0;
//	deserializeBinary(is, lengthFiles);
//	m_files.clear();
//	for(uint32_t i = 0; i < lengthFiles; i++)
//		m_files.insert(File(is));
//}


////---------------------------------------------------------------
//// Purpose: 
////---------------------------------------------------------------
//void Folder::serializeFiles( std::ostream & os ) const
//{
//	serializeBinary((uint32_t)m_files.size(), os);
//	for(const File &file : m_files)
//		file.serialize(os);
//}


////---------------------------------------------------------------
//// Purpose: 
////---------------------------------------------------------------
//void Folder::serializeFolders( std::ostream & os ) const
//{
//	serializeBinary((uint32_t)m_folders.size(), os);
//	for(const Folder &folder : m_folders)
//		folder.serialize(os);
//}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Folder::FolderSet::iterator Folder::addFolder( const Folder &folder )
{
	auto it = m_folders.find(folder);
	if(it != m_folders.end())
		return it;
	else
		return m_folders.insert(folder).first;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Folder::FileSet::iterator Folder::addFile( const File &file )
{
	auto it = m_files.find(file);
	if(it != m_files.end())
		return it;
	else
		return m_files.insert(file).first;
}


}