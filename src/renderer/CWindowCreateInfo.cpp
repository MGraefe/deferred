// renderer/CWindowCreateInfo.cpp
// renderer/CWindowCreateInfo.cpp
// renderer/CWindowCreateInfo.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CWindowCreateInfo.h"

CWindowCreateInfo::CWindowCreateInfo()
{
	width = 640;
	height = 480;
	eventCallback = NULL;
	title = "Deferred";
	colorBits = 24;
	alphaBits = 8;
	depthBits = 24;
	fullscreen = false;
}