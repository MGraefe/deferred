// client/CEntityCreateListener.h
// client/CEntityCreateListener.h
// client/CEntityCreateListener.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CEntityCreateListener_H__
#define deferred__client__CEntityCreateListener_H__

#include <util/IEvent.h>
class CEntityCreateListener
{
public:
	static bool HandleEventStatic( const IEvent *evt );
};


#endif