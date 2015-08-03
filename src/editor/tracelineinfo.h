// editor/tracelineinfo.h
// editor/tracelineinfo.h
// editor/tracelineinfo.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//tracelineinfo.h

#pragma once
#ifndef deferred__editor__tracelineinfo_H__
#define deferred__editor__tracelineinfo_H__

struct tracelineinfo_t
{
	//IN:
	Vector3f start;
	Vector3f dir;
	float distMax; //negative if infinite distance
	
	//Out:
	float distHit; //negative if not hit
};

#endif // deferred__editor__tracelineinfo_H__
