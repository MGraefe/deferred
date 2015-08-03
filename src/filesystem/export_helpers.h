// filesystem/export_helpers.h
// filesystem/export_helpers.h
// filesystem/export_helpers.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once

#if defined _WIN32 || defined __CYGWIN__ || defined _WINDOWS
#ifdef __GNUC__
#define RPAF_DLLEXPORT __attribute__ ((dllexport))
#define RPAF_DLLIMPORT __attribute__ ((dllimport))
#else
#define RPAF_DLLEXPORT __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#define RPAF_DLLIMPORT __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#if __GNUC__ >= 4
#define RPAF_DLLEXPORT __attribute__ ((visibility ("default")))
#define RPAF_DLLIMPORT  __attribute__ ((visibility ("default")))
#else
#define RPAF_DLLEXPORT
#define RPAF_DLLIMPORT
#endif
#endif

#ifdef RPAF_EXPORT
#define RPAF_DLL RPAF_DLLEXPORT
#else
#define RPAF_DLL RPAF_DLLIMPORT
#endif
