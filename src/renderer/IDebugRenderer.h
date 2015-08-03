// renderer/IDebugRenderer.h
// renderer/IDebugRenderer.h
// renderer/IDebugRenderer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//IDebugRenderer.h

#pragma once
#ifndef deferred__renderer__IDebugRenderer_H__
#define deferred__renderer__IDebugRenderer_H__

//#include "dll.h"

class /*DLL*/ IDebugRenderer
{
public:
	virtual void RenderDebug( void ) = 0;
};

#endif // deferred__renderer__IDebugRenderer_H__
