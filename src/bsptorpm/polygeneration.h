// bsptorpm/polygeneration.h
// bsptorpm/polygeneration.h
// bsptorpm/polygeneration.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

//polygeneration.h

#pragma once
#ifndef deferred__bsptorpm__polygeneration_H__
#define deferred__bsptorpm__polygeneration_H__

#include <util/maths.h>
#include <vector>
#include <util/rpm_file.h>
#include "bspfile.h"
#include <util/triangle_t.h>
#include "CBSPLumps.h"

void testPolygon(void);
void brushesToTriangles(const CBSPLumps *lumps, 
						std::vector<Triangle> &triangles);
#endif
