// util/error.cpp
// util/error.cpp
// util/error.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "error.h"
#include <stdio.h>
#include <string>
#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <cstdarg>

#include "../version.h"

#ifdef _WINDOWS
#include <io.h>
#endif

#ifndef _WINDOWS
void ShowCursor(UINT dummyVal)
{
	//TODO: maybe implement this?
}
#endif


#define DECLARE_ERR(error, message) case error: return verboseMessage ? message : #error
const char *ToString(error_e error, bool verboseMessage) {
	switch(error)
	{
		DECLARE_ERR(ERR_NONE, "No Error");
		DECLARE_ERR(ERR_FILENOTFOUND, "File not found");
		DECLARE_ERR(ERR_EOF, "End of file");
		DECLARE_ERR(ERR_CORRUPT_FILE, "Corrupt file");
		DECLARE_ERR(ERR_WRONG_VERSION, "Wrong version");
		DECLARE_ERR(ERR_ALREADY_DONE, "Already done");

		DECLARE_ERR(ERR_LAST, "Last Error, blame the coder");
	};
	return "Unknown Error";
}

bool bConsoleOpen = false;

#ifdef _WINDOWS
HANDLE __hStdOut = NULL;
HANDLE __hStdIn = NULL;
#endif

//-----------------------------------------------------------------------
// Purpose: Post an Error-Message-Box
//-----------------------------------------------------------------------
void error( const char *TextAsPrintf, ... )
{
	char msg[2048];
	va_list argptr;

	va_start( argptr, TextAsPrintf );
		vsprintf_s( msg, TextAsPrintf, argptr );
	va_end( argptr );

	ConsoleMessage(msg);

#ifdef _DEBUG
	ShowCursor(TRUE);
	MessageBoxA( NULL, msg, "Error", MB_OK | MB_ICONWARNING );
	ShowCursor(FALSE);
#endif
}


//-----------------------------------------------------------------------
// Purpose: Post an Error-Message-Box
//-----------------------------------------------------------------------
void error_msg( const char *TextAsPrintf, ... )
{
	char msg[2048];
	va_list argptr;

	va_start( argptr, TextAsPrintf );
		vsprintf_s( msg, TextAsPrintf, argptr );
	va_end( argptr );

	ConsoleMessage(msg);

	ShowCursor(TRUE);
	MessageBoxA( NULL, msg, "Error", MB_OK | MB_ICONWARNING );
	ShowCursor(FALSE);
}


//-----------------------------------------------------------------------
// Purpose: Post an Error-Message-Box and quit the program
//-----------------------------------------------------------------------
void error_fatal( const char *TextAsPrintf, ... )
{
	char msg[2048];
	va_list argptr;

	va_start( argptr, TextAsPrintf );
		vsprintf_s( msg, TextAsPrintf, argptr );
	va_end( argptr );

	ConsoleMessage( msg );

	ShowCursor(TRUE);
	MessageBoxA( NULL, msg, "Fatal Error", MB_OK | MB_ICONERROR );
	AssertMsg( false, msg );

	exit(1);
}


//-----------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------
#ifdef _WINDOWS
void InitConsoleInternal( void )
{
	int hConHandle;
	FILE *fp;

	// redirect unbuffered STDOUT to the console
	hConHandle = _open_osfhandle((long)__hStdOut, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	std::cout.clear();

	// redirect unbuffered STDIN to the console
	hConHandle = _open_osfhandle((long)__hStdIn, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	std::cin.clear();
	setvbuf( stdin, NULL, _IONBF, 0 );

	// redirect unbuffered STDERR to the console
	HANDLE __hStdError = GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle((long)__hStdError, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
}
#endif

//-----------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------
void InitConsole( void )
{
#ifdef _WINDOWS
	bool bFirstThread = AllocConsole() == TRUE;
	if( bFirstThread )
		SetConsoleTitleA("Deferred Console");

	__hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	__hStdIn = GetStdHandle(STD_INPUT_HANDLE);

	if( !__hStdOut || !__hStdIn )
		return;

	if( bFirstThread )
		InitConsoleInternal();

	bConsoleOpen = true;
#else
	pid_t pid = getpid();
	char pchPid[16];
	_itoa_s(pid, pchPid, 10);
	char pchPidString[128] = "deferred-pid-envvar-";
	strncat(pchPidString, pchPid, 64);
	bool bFirstThread = getenv(pchPidString) == NULL;
	if(bFirstThread)
		setenv(pchPidString, pchPid, 1);
#endif
	
	//Write some basic infos in the console!
	if( bFirstThread )
	{
		ConsoleMessage( "Deferred Console" );
		ConsoleMessage( "Version: " DEFERREDVERSION_S ", build: %s %s", __DATE__, __TIME__ );
		ConsoleMessage( "Copyright 2008-2014 by Marius \"RedPuma\" Graefe\n]\n" );
	}
}

//-----------------------------------------------------------------------
// Purpose: Post an Error-Message-Box
//-----------------------------------------------------------------------
void ConsoleMessage( const char *TextAsPrintf, ... )
{
	char msg[2048];
	va_list argptr;

	va_start( argptr, TextAsPrintf );
		vsprintf_s( msg, TextAsPrintf, argptr );
	va_end( argptr );

#ifdef _WINDOWS
	OutputDebugStringA( msg );
	OutputDebugStringA( "\n" );
	//if( bConsoleOpen && __hStdOut )
	//{	
	//	DWORD cCharsWritten;
	//	WriteConsole(__hStdOut, msg, strlen(msg), &cCharsWritten, NULL);
	//	WriteConsole(__hStdOut, "\n", 1, NULL, NULL);
	//}
	puts(msg);
#else
	puts(msg);
#endif
}



//-----------------------------------------------------------------------
// Purpose: Assertion Dialog
//-----------------------------------------------------------------------
bool OpenAssertionDialog( const char *pchFile, int line, const char *pchExpression, const char *pchMessage )
{
	char pchLine[32];
	_itoa_s( line, pchLine, 10 );

	std::string errorstr;
	errorstr = std::string("File") + pchFile + "\nLine: " + pchLine + "\nExpression:" + pchExpression;
	if( pchMessage != NULL )
		errorstr = errorstr + "\nMessage: " + pchMessage;

	errorstr += "\n\nWould you like to break the debugger?";

	ConsoleMessage( "Assertion Failed:\n%s", errorstr.c_str() );
	int ret = MessageBoxA( NULL, errorstr.c_str(), "Assertion failed", MB_YESNO | MB_ICONERROR );

	return ret == IDYES;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const char * GetDeferredVersion()
{
	return DEFERREDVERSION_S;
}
