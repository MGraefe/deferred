// client/stdafx.h
// client/stdafx.h
// client/stdafx.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__client__stdafx_H__
#define deferred__client__stdafx_H__

#ifdef _WINDOWS
	#include <util/wininc.h>
#else
	#include <util/lininc.h>
#endif

#include <util/os_specific.h>
#include <util/stdincludes.h>
#include <util/maths.h>
#include <util/sse.h>

#endif
