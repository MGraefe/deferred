// renderer/DDS.h
// renderer/DDS.h
// renderer/DDS.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__DDS_H__
#define deferred__renderer__DDS_H__


#include "texture_t.h"
#include "glheaders.h"

extern bool LoadDDSFromFile( const char *pFilename, int index, texture_t &tex, int textureTarget = GL_TEXTURE_2D, bool flipY = false, bool srgb = false );

#endif
