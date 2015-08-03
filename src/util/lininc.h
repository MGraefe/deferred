// util/lininc.h
// util/lininc.h
// util/lininc.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__lininc_H__
#define deferred__util__lininc_H__

#ifdef _LINUX
#include <unistd.h>
#include <stdlib.h>
//#include <X11/X.h>
#define FALSE 0
#define TRUE 1
typedef unsigned char BOOL;
#define CAST_WCHAR_CONST(v) ((const char *)(v))
#define CAST_WCHAR(v) ((char*)(v))
#else
	#error THIS IS A LINUX ONLY FILE, INCLUDE "wininc.h" FOR WINDOWS
#endif

#endif /* LININC_H_ */
