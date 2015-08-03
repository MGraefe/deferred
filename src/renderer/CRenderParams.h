// renderer/CRenderParams.h
// renderer/CRenderParams.h
// renderer/CRenderParams.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CRenderParams_H__
#define deferred__renderer__CRenderParams_H__

//Renderer constants:
enum renderpasses_e
{
	RENDERPASS_NORMAL = 0, //normal non-translucent geometry
	RENDERPASS_NORMAL_TRANSLUCENT, //normal translucent geometry
	RENDERPASS_WATER_REFL, //render water reflections
	RENDERPASS_SHADOW, //render into shadow map
};


class CRenderParams
{
public:
	renderpasses_e renderpass;
};

#endif
