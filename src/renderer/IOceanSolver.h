// renderer/IOceanSolver.h
// renderer/IOceanSolver.h
// renderer/IOceanSolver.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__IOceanSolver_H__
#define deferred__renderer__IOceanSolver_H__

#include <util/maths.h>

class IOceanSolver
{
public:
	virtual void Init(int N, float L, const ComplexNf *h0, ComplexNf *in, ComplexNf *out) = 0;
	virtual void *malloc(size_t N) = 0;
	virtual void free(void *p) = 0;
	virtual void Execute(float t) = 0;
};

#endif // deferred__renderer__IOceanSolver_H__
