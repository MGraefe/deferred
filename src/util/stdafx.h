// util/stdafx.h
// util/stdafx.h
// util/stdafx.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__stdafx_H__
#define deferred__util__stdafx_H__

#ifndef _UTIL
#error YOU INCLUDED THE WRONG FILE (<stdafx.h> instead of "stdafx.h")
#endif

#ifndef _LINUX
#include "wininc.h"
#else
#include "lininc.h"
#endif

#include "stdincludes.h"
#include "maths.h"
#include "readbinary.h"
#include "os_specific.h"
#include "sse.h"

#endif
