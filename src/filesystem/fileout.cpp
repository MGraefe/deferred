// filesystem/fileout.cpp
// filesystem/fileout.cpp
// filesystem/fileout.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "fileout.h"

namespace filesystem
{
	

FileOut::FileOut( const std::string &path ) :
	m_realPath(path),
	m_virtualPath(path)
{

}


FileOut::FileOut( const std::string &realPath, const std::string &virtualPath ) :
	m_realPath(realPath),
	m_virtualPath(virtualPath)
{

}


}