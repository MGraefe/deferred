// bsptorpm/bsptorpm.h
// bsptorpm/bsptorpm.h
// bsptorpm/bsptorpm.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__bsptorpm__bsptorpm_H__
#define deferred__bsptorpm__bsptorpm_H__

#include "..\..\src\util\maths.h"
#include <vector>

typedef double real;

void BSPVectorToRPMVector( const Vector3<real> &src, Vector3<real> &dst );
int CreateTextureIndex( SHORT texinfo );

#endif
