// server/stdafx.h
// server/stdafx.h
// server/stdafx.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__server__stdafx_H__
#define deferred__server__stdafx_H__


#ifndef _LINUX
#include <util/wininc.h>
#else
#include <util/lininc.h>
#endif

#include <util/stdincludes.h>
#include <util/maths.h>
#include <util/os_specific.h>
#include <util/sse.h>

#endif
