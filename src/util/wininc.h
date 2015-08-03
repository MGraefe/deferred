// util/wininc.h
// util/wininc.h
// util/wininc.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__wininc_H__
#define deferred__util__wininc_H__

#ifdef _WINDOWS
	#define NOMINMAX
	#include <WinSock2.h>
	#include <ws2tcpip.h>
	#include <Windows.h>

	#define CAST_WCHAR_CONST(v) (v)
	#define CAST_WCHAR(v) (v)
#else
	#error THIS IS A WINDOWS ONLY FILE, INCLUDE "lininc.h" FOR LINUX
#endif

#endif
