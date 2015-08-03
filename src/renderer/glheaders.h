// renderer/glheaders.h
// renderer/glheaders.h
// renderer/glheaders.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__glheaders_H__
#define deferred__renderer__glheaders_H__

#ifdef _WINDOWS
#include <util/wininc.h>
#else
#include <util/lininc.h>
#endif

#include <GL/glew.h>	// Header File Fot The OpenGL Extension Wrangler
#include <GL/gl.h>			// Header File For The OpenGL32 Library
#include <GL/glu.h>			// Header File For The GLu32 Library

#include "glfunctions.h"
#include "CGLStateSaver.h"
#include "CMatrixStack.h"

#endif
