// util/CSkyVisibilityCalculator.h
// util/CSkyVisibilityCalculator.h
// util/CSkyVisibilityCalculator.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__CSkyVisibilityCalculator_H__
#define deferred__util__CSkyVisibilityCalculator_H__

#include "triangle_t.h"
#include "CLightProbe.h"

class COctreeNode;

class CSkyVisibilityCalculator
{
public:
	typedef double ftype;
	CSkyVisibilityCalculator(std::vector<Triangle> &triangles, const AABoundingBox_t<real> &mapBounds);
	~CSkyVisibilityCalculator();
	void calculate();
	void calculateLightProbeSkyVis(std::vector<CLightProbe> &probes);

private:
	void weldSkyVisibility(std::vector<Triangle> &triangles);
	void calcOctree();

private:
	std::vector<Triangle> &m_triangles;
	const AABoundingBox_t<real> m_mapBounds;
	COctreeNode *m_octree;
	CPlane_t<ftype> *m_planes;

};

#endif
