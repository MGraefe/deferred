// studiorpm/CInputFile.h
// studiorpm/CInputFile.h
// studiorpm/CInputFile.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__studiorpm__CInputFile_H__
#define deferred__studiorpm__CInputFile_H__

#include <string>
#include <util/error.h>
#include <util/rpm_file.h>
#include <vector>
#include "CBone.h"

class CInputFile
{
public:
	virtual ~CInputFile() {}

	virtual error_e parse(std::string filename) = 0;
	virtual std::vector<rpm_triangle_t> &triangles() = 0;
	virtual std::vector<int> parentBones() = 0;
	virtual std::vector<std::string> &textures() = 0;
	virtual std::vector<CBone> &bones() = 0;
	
};

#endif // deferred__studiorpm__CInputFile_H__