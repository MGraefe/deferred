// client/CNettableUpdateHandler.h
// client/CNettableUpdateHandler.h
// client/CNettableUpdateHandler.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CNettableUpdateHandler_H__
#define deferred__client__CNettableUpdateHandler_H__

#include <util/eventlist.h>

class CNettableUpdateHandler
{
public:
	static void HandleEvent( const CNettableUpdate *evt );

private:
	static bool checkError(const InStream &is);
	CNettableUpdateHandler() {}
	CNettableUpdateHandler(const CNettableUpdateHandler &other) {}
};

#endif // deferred__client__CNettableUpdateHandler_H__