// util/error.h
// util/error.h
// util/error.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__error_H__
#define deferred__util__error_H__

#include <stdarg.h>

#ifndef _WINDOWS
#include "MessageBox.h"
#endif /* _WINDOWS */

enum consolecolor_e
{
	COLOR_WHITE,
	COLOR_RED,
	COLOR_GREEN,
	COLOR_BLUE,
};

extern void error( const char *TextAsPrintf, ... );
extern void error_fatal( const char *TextAsPrintf, ... );
extern void error_msg( const char *TextAsPrintf, ... );
extern void InitConsole( void );
extern void ConsoleMessage( const char *TextAsPrintf, ... );
extern bool bConsoleOpen;
extern bool OpenAssertionDialog( const char *pchFile, int line, const char *pchExpression, const char *pchMessage );

#if defined _DEBUG || defined _RELEASE_ASSERT
	#ifdef _LINUX
		#include "signal.h"
		#define DEBUG_BREAK() raise(SIGTRAP)
	#else
		#include "wininc.h"
		#define DEBUG_BREAK() DebugBreak()
	#endif

	#define _AssertMsg( _expr, _msg, _bFatal )	\
		do {									\
			if( !(_expr) )						\
			{									\
				if( OpenAssertionDialog(__FILE__, __LINE__, #_expr, _msg ) ) \
					DEBUG_BREAK();				\
				if( _bFatal )					\
					error_fatal( "Assertion failed: %s", _msg ); \
			}									\
		} while(0)
#else
	#ifndef _LINUX
		#define _AssertMsg(a,b,c) ((void*)0)
	#else
		#define _AssertMsg(a,b,c) (static_cast<void>(0))
	#endif
#endif

#define Assert(expr)				_AssertMsg(expr,NULL,false)
#define AssertMsg(expr,msg)			_AssertMsg(expr,msg,false)
#define AssertFatal(expr)			_AssertMsg(expr,NULL,true)
#define AssertMsgFatal(expr,msg)	_AssertMsg(expr,msg,true)

//global error codes
enum error_e
{
	ERR_NONE = 0, //No error
	ERR_FILENOTFOUND, //file not found
	ERR_EOF, //end of file
	ERR_CORRUPT_FILE, //corrupt file
	ERR_WRONG_VERSION, //wrong version
	ERR_ALREADY_DONE, //already done?
	
	//feel free to add more

	ERR_LAST, //MUST BE THE LAST ONE!
};

extern const char *ToString(error_e error, bool verboseMessage = true);

const char *GetDeferredVersion();


#endif
