// studiorpm/studiorpm.h
// studiorpm/studiorpm.h
// studiorpm/studiorpm.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once
#ifndef deferred__studiorpm__studiorpm_H__
#define deferred__studiorpm__studiorpm_H__

#include <tchar.h>

//Quick include hack
#ifndef _UTIL
#define _UTIL
#include <util/stdafx.h>
#undef _UTIL
#else
#include <util/stdafx.h>
#endif

#include "mmsystem.h"
#include <iostream>
#include <fstream>
#include <util/maths.h>
#include <util/StringUtils.h>
#include <util/error.h>
#include <util/CScriptParser.h>
#include <util/CPhysFile.h>
#include <util/CPhysConvexCompoundShape.h>
#include <util/CPhysConvexHull.h>
#include <util/wininc.h>

#include <util/rpm_file.h>


void calcTriangleNormal( rpm_triangle_t &tri );
char *CopyUntilOccurenceOfChar( char *out, char *in, char *delimiter );
int getTextureIndex( std::vector<std::string> &textures, const char *tex );

std::string &handleFilenameVariables( std::string &filename );

#define THROW_INVALID_FUNCTION_CALL() throw std::exception("Invalid function call")

#endif // deferred__studiorpm__studiorpm_H__