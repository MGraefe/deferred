// util/dll.h
// util/dll.h
// util/dll.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__dll_H__
#define deferred__util__dll_H__

#if defined _WIN32 || defined __CYGWIN__ || defined _WINDOWS
    #ifdef __GNUC__
      #define DLLEXPORT __attribute__ ((dllexport))
	  #define DLLIMPORT __attribute__ ((dllimport))
    #else
      #define DLLEXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
	  #define DLLIMPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
#else
  #if __GNUC__ >= 4
    #define DLLEXPORT __attribute__ ((visibility ("default")))
    #define DLLIMPORT  __attribute__ ((visibility ("default")))
  #else
    #define DLLEXPORT
    #define DLLIMPORT
  #endif
#endif


#if defined(_RENDERER) || defined(_SERVER)
	#define DLL DLLEXPORT
#else
	#define DLL DLLIMPORT
#endif

/*
 *  #define PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING \
 *	#pragma warning( push ) \
 *	#pragma warning( disable : 4251)
 *
 *  #define POP_PRAGMA_WARNINGS #pragma warning( push )
 */

#ifdef _MSC_VER
#define PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING \
	__pragma( warning( push ) ) \
	__pragma( warning( disable : 4251 ) )
#else
#define PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING
#endif

#ifdef _MSC_VER
#define POP_PRAGMA_WARNINGS __pragma( warning( pop ) )
#else
#define POP_PRAGMA_WARNINGS
#endif

#endif
