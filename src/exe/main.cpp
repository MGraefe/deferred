// exe/main.cpp
// exe/main.cpp
// exe/main.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include <util/dll.h>
extern "C" DLLIMPORT int clientMain(const char *cmdLine);


#if defined _WINDOWS

#include <Windows.h>

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	return clientMain(lpCmdLine);
}

#elif defined _LINUX

#include <string>

int main(int argc, const char *argv[])
{
	//clutter argv's together
	std::string args;
	for(int i = 1; i < argc; i++)
	{
		args += argv[i];
		if(i < argc-1)
			args += " ";
	}

	return clientMain(args.c_str());
}

#else
#error unsupported op-sys
#endif


