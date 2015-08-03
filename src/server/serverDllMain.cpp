// server/serverDllMain.cpp
// server/serverDllMain.cpp
// server/serverDllMain.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include <util/error.h>

void initServerDll()
{
	//puts("Initializing server shared library");
	InitConsole();
}

#ifdef _WINDOWS
BOOL WINAPI DllMain(
	__in  HINSTANCE hinstDLL,
	__in  DWORD fdwReason,
	__in  LPVOID lpvReserved
	)
{
	initServerDll();
	return TRUE;
}
#else
#ifdef __GNUC__
void __attribute__ ((constructor)) ServerDllMain(void)
{
	initServerDll();
}
#else
#error unsupported compiler
#endif
#endif
