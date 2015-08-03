// util/IRealtimeDebugInterface.h
// util/IRealtimeDebugInterface.h
//IRealtimeDebugInterface.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__IRealtimeDebugInterface_H__
#define deferred__util__IRealtimeDebugInterface_H__

class IRealtimeDebugInterface
{
public:
	virtual void SetValue(const char *name, const char *value) = 0;
};


class CRealtimeDebugInterfaceDummy : public IRealtimeDebugInterface
{
public:
	virtual void SetValue(const char *name, const char *value) {}
};

#endif // deferred__util__IRealtimeDebugInterface_H__