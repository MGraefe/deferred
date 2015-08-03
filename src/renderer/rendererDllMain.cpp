// renderer/rendererDllMain.cpp
// renderer/rendererDllMain.cpp
// renderer/rendererDllMain.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include <util/error.h>

void initRendererDll()
{
	//puts("Initializing renderer shared library");
	InitConsole();
}

#ifdef _WINDOWS
BOOL WINAPI DllMain(
	__in  HINSTANCE hinstDLL,
	__in  DWORD fdwReason,
	__in  LPVOID lpvReserved
	)
{
	initRendererDll();
	return TRUE;
}
#endif

#ifdef __GNUC__
void __attribute__ ((constructor)) RendererDllMain(void)
{
	initRendererDll();
}
#endif

