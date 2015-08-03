// bsptorpm/CPolygon.h
// bsptorpm/CPolygon.h
// bsptorpm/CPolygon.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__bsptorpm__CPolygon_H__
#define deferred__bsptorpm__CPolygon_H__

#include <util/maths.h>
#include <vector>
#include "bspfile.h"
#include <util/triangle_t.h>

class CPolygon
{
public:
	void fromBasePlane(const CPlaned &plane);
	bool chopPolygon(const CPlaned &plane);
	int triangulate(std::vector<Triangle> &triangles) const;
	bool equal(const CPolygon &other) const;
public:
	Vector3d normal;
	std::vector<Vector3d> points;
	const dbrush_t *parentBrush;
	const dbrushside_t *parentBrushSide;
};


#endif
