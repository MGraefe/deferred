// util/hash.h
// util/hash.h
//hash.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__hash_H__
#define deferred__util__hash_H__

#include <stdint.h>

uint32_t hashFnv(const char *str);

bool testFnv1a();

#endif // deferred__util__hash_H__
