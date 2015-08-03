// filesystem/fileout.h
// filesystem/fileout.h
// filesystem/fileout.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__filesystem__fileout_H__
#define deferred__filesystem__fileout_H__

#include <string>

namespace filesystem
{

//FileOut, represents abstract file, used when creating an archive file
class FileOut
{
public:
	FileOut(const std::string &path);
	FileOut(const std::string &realPath, const std::string &virtualPath);

private:
	std::string m_realPath;
	std::string m_virtualPath;
};

}
#endif // deferred__filesystem__fileout_H__