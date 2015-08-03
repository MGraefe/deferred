// server/servermain.h
// server/servermain.h
// server/servermain.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#define SERVERCREATEERROR_NONE 0

#include <util/dll.h>
#include "serverstartdata.h"

#ifdef _SERVER
	#define SERVERDLL extern "C" DLLEXPORT
#else
	#define SERVERDLL extern "C" DLLIMPORT
#endif

SERVERDLL int servermain( const serverstartdata_t &data );
