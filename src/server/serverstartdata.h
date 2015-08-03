// server/serverstartdata.h
// server/serverstartdata.h
// server/serverstartdata.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__server__serverstartdata_H__
#define deferred__server__serverstartdata_H__


enum servertypes_e
{
	SERVERTYPE_LOCAL = 0,
	SERVERTYPE_LAN,
	SERVERTYPE_INET,
};

struct serverstartdata_t
{
	serverstartdata_t()
	{
		type = SERVERTYPE_LOCAL;
		pbShutdown = 0;
		pbIsDown = 0;
		pbIsUp = 0;
		port = 0;
		tickrate = 66;
		mapname[0] = 0;
		bThreaded = false;
		pDebugInterface = 0;
	}

	servertypes_e type;
	volatile int *pbShutdown;
	volatile int *pbIsDown;
	volatile int *pbIsUp;
	unsigned short int port;
	int tickrate;
	char mapname[64];
	bool bThreaded;
	void *pDebugInterface;
};


#endif
