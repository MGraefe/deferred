// renderer/texture_t.h
// renderer/texture_t.h
// renderer/texture_t.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__texture_t_H__
#define deferred__renderer__texture_t_H__

#include <string>


#define TEX_TYPE_TGA	0
#define TEX_TYPE_DXT1	1
#define TEX_TYPE_DXT3	2
#define TEX_TYPE_DXT5	3

struct texture_t
{
	std::string filename;
	int	index;
	short int width;
	short int height;
	bool srgb;
};

struct materialprops_t
{
	float smoothness;
	float specular[4];
	bool bTranslucent;
	bool bDoubleSided;
	bool bShadowsOnly;
	std::string decoration;
};

struct material_t
{
	std::string name;
	texture_t diffuseTex;
	texture_t normalMap;
	materialprops_t props;
};
	

#endif
