// renderer/CWindowCreateInfo.h
// renderer/CWindowCreateInfo.h
// renderer/CWindowCreateInfo.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CWindowCreateInfo_H__
#define deferred__renderer__CWindowCreateInfo_H__

// Window creation info
// Constructor handles all default values EXCEPT eventCallback
class CWindowCreateInfo
{
public:
	CWindowCreateInfo();

public:
	int width;
	int height;
	void *eventCallback;
	const char *title;
	int colorBits;
	int alphaBits;
	int depthBits;
	bool fullscreen;
};

#endif