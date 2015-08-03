// util/stdincludes.h
// util/stdincludes.h
// util/stdincludes.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__stdincludes_H__
#define deferred__util__stdincludes_H__

#include <vector>
#include <map>
#include <queue>
#include <list>
#include <algorithm>
#include <iterator>
#include <set>
#include <limits>
#include <string>
#include <math.h>
#include <cmath>
#include <cstdlib>
#include <ostream>
#include <istream>
#include <iostream>
#include <sstream>
#include <exception>
#include <limits.h>
#include <thread>
#include <chrono>

#ifndef _T
	#ifdef _UNICODE
		typedef wchar_t TCHAR;
		#define _T(x) L ## x
		#define _tcscmp wcscmp
	#else
		typedef char TCHAR;
		#define _T(x) x
		#define _tcscmp strcmp
	#endif
#endif


#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(x[0]))

#define STRINGIFYX(s) STRINGIFY(s)
#define STRINGIFY(s) #s


#endif
