// util/hash.cpp
// util/hash.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//----------------------------------


#include "stdafx.h"
#include "hash.h"

//FNV-1a algorithm (http://www.isthe.com/chongo/tech/comp/fnv/)

#define FNV_PRIME32 16777619U;
#define FNV_BASIS32 2166136261U;

uint32_t hashFnv(const char *str)
{
	uint32_t hash = FNV_BASIS32;
	for(unsigned char *s = (unsigned char*)str; *s; ++s)
		hash = (hash ^ (uint32_t)*s) * FNV_PRIME32;
	return hash;
}
