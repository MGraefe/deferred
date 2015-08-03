// bsptorpm/polygeneration.cpp
// bsptorpm/polygeneration.cpp
// bsptorpm/polygeneration.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include <util/maths.h>
#include <util/rpm_file.h>
#include <vector>
#include <iostream>
#include "bspfile.h"
#include "bspflags.h"
#include <util/triangle_t.h>
#include "bsptorpm.h"
#include "CBSPLumps.h"
#include "CPolygon.h"
#include <map>

using std::vector;
using std::cout;
using std::endl;
using std::cerr;


class CLine
{
public:
	CLine() { }
	CLine(const Vector3d &vStart, const Vector3d &vEnd) :
		start(vStart), end(vEnd) { }

	bool intersect(const CPlaned &plane, Vector3d &intersPoint) const {
		CRayd ray(start, (end-start).Normalize());
		double t;
		if(!ray.intersect(plane, intersPoint, t))
			return false;
		if( t < 0.0 || t > start.distTo(end) )
			return false;
		return true;
	}
public:
	Vector3d start;
	Vector3d end;
};


//------------------------------------------------------------------------
// Purpose: Create a polygon from a set of intersecting planes.
//			The valid area of the polygon is always "under" the plane,
//			so the plane's normal points away from the polygon.
//			referencePlane is an index into planes and marks the plane
//			in which the polygon should be created.
//			Returns false if the the resulting polygon would be invalid
//			(i.e. area = 0)
//------------------------------------------------------------------------
bool polygonFromPlanes(vector<CPlaned> planes, int referencePlane, CPolygon &poly)
{
	//Create extremely large, 4-sided polygon on the referencePlane
	poly.fromBasePlane(planes[referencePlane]);

	for( size_t i = 0; i < planes.size(); i++ )
	{
		if( i == referencePlane ) //Don't use the plane used for creation again
			continue;
		if( !poly.chopPolygon(planes[i]) )
			return false;
	}

	return true;
}



//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
bool isTextureRenderable(const texinfo_t &texinfo, const CBSPLumps *lumps)
{
	if( texinfo.texdata == -1 )
		return false;

	if((texinfo.flags & SURF_SKY |
		texinfo.flags & SURF_SKIP |
		texinfo.flags & SURF_NODRAW) == 0 )
		return true;

	int texStringTableID = lumps->texdatas[texinfo.texdata].nameStringTableID;
	if( texStringTableID >= 0 || texStringTableID < MAX_MAP_TEXDATA_STRING_TABLE )
	{
		char *pTexNamePtr = lumps->texStringData.addr + lumps->texStringTable[texStringTableID] * sizeof(char);
		if( strcmp(pTexNamePtr, "TOOLS/TOOLSBLOCKLIGHT") == 0 )
			return true;
	}

	return false;
}


//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
int removeOverlayedPolygons(const vector<CPolygon> &in, vector<CPolygon> &out)
{
	out.reserve(in.size());
	bool *removePolys = new bool[in.size()];
	memset(removePolys, 0, in.size()*sizeof(bool));

	for( size_t i = 0; i < in.size(); i++ )
	{
		for( size_t k = i+1; k < in.size(); k++ )
		{
			if( in[i].equal(in[k]) && 
				in[i].normal.Dot(in[k].normal) < -0.9 )
			{
				removePolys[i] = true;
				removePolys[k] = true;
			}
		}
	}

	int removed = 0;
	for( size_t i = 0; i < in.size(); i++ )
		if(removePolys[i])
			removed++;
		else
			out.push_back(in[i]);
			
	cout << "removeOverlayedPolygons(): removed " << removed << " polygons." << endl;

	delete[] removePolys;
	return removed;
}



//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
void planeFromDPlane(const dplane_t &src, CPlaned &dst)
{
	dst.m_vNorm.setFromOther(-src.normal).Normalize();
	dst.m_d = (double)-src.dist;
}


//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
bool isValidForCulling(const dbrush_t &brush, const dbrushside_t *brushsides)
{
	//first check for displacements
	//for( int i = 0; i < brush.numsides; i++ )
	//{
	//	short dispinfo = brushsides[brush.firstside+i].dispinfo;
	//	if( dispinfo > -1 )
	//		return false;
	//}

	const UINT maskYes = CONTENTS_SOLID;
	const UINT maskNo = CONTENTS_WINDOW|CONTENTS_WATER|CONTENTS_MOVEABLE;
	return (brush.contents & maskYes) && !(brush.contents & maskNo);
}


//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
bool isPolyInsideBrush(	const CPolygon &poly, const dbrush_t *brush,
						const dbrushside_t *brushsides, const CPlaned *bspplanes)
{
	if( poly.parentBrush == brush )
		return false;

	for( size_t i = 0; i < poly.points.size(); i++ )
	{
		for( int k = 0; k < brush->numsides; k++ )
		{
			const CPlaned &plane = bspplanes[brushsides[brush->firstside+k].planenum];

			//Be more generous when the poly's normal is looking towards this plane.
			//double eps = plane.m_vNorm.Dot(poly.normal) < 0.5f ? 0.01 : 0.01;
			double eps = 0.01;

			//immediately return false if vertex is outside of any plane
			if( plane.distToPointSigned(poly.points[i]) > eps)
				return false;
		}
	}

	return true;
}


//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
bool isPolygonInsideAnyBrush(const CPolygon &poly, const dbrush_t *brushes, int brushCount,
							 const dbrushside_t *brushsides, const CPlaned *bspplanes)
{
	for( int i = 0; i < brushCount; i++ )
	{
		if( isValidForCulling(brushes[i], brushsides) && 
			isPolyInsideBrush(poly, &brushes[i], brushsides, bspplanes) )
			return true;
	}
	return false;
}

void PrintDispinfoStats( int brushCount, const dbrush_t * brushes, const dbrushside_t * brushsides );


//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
int removePolygonsInsideSolids(const vector<CPolygon> &in, vector<CPolygon> &out,
							   const dbrush_t *brushes, int brushCount,
							   const dbrushside_t *brushsides,
							   const CPlaned *bspplanes )
{
	int removed = 0;
	for( size_t i = 0; i < in.size(); i++ )
	{
		if( isPolygonInsideAnyBrush(in[i], brushes, brushCount, brushsides, bspplanes) )
			removed++;
		else
			out.push_back(in[i]);
	}

	int validForCulling = 0;
	for( int i = 0; i < brushCount; i++ )
		if( isValidForCulling(brushes[i], brushsides) )
			validForCulling++;

	cout << "Brushes valid for culling: " << validForCulling << " of " << brushCount << endl;
	cout << "removePolygonsInsideSolids(): removed " << removed << " polygons." << endl;
	PrintDispinfoStats(brushCount, brushes, brushsides);

	return removed;
}


//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
void brushesToTriangles(const CBSPLumps *lumps,
						vector<Triangle> &triangles)
{
	//Collect and convert all planes
	CPlaned *planes = new CPlaned[lumps->planes.elements];
	for( int i = 0; i < lumps->planes.elements; i++ )
		planeFromDPlane(lumps->planes[i], planes[i]);

	//Collect polygons
	vector<CPolygon> rawPolys;
	for( int i = 0; i < lumps->brushes.elements; i++ )
	{
		const dbrush_t *brush = &lumps->brushes[i];

		//Collect planes for this brush
		vector<CPlaned> localPlanes(brush->numsides);
		for( int j = 0; j < brush->numsides; j++ )
			localPlanes[j] = planes[lumps->brushsides[brush->firstside+j].planenum];

		//Create polygons from brushsides
		for( int j = 0; j < brush->numsides; j++ ) 
		{
			int sideId = brush->firstside+j;
			short texinfoId = lumps->brushsides[sideId].texinfo;

			//Dont bother textures that aren't visible
			if( texinfoId < 0 || !isTextureRenderable(lumps->texinfos[texinfoId], lumps) )
				continue;

			//Create a polygon
			CPolygon poly;
			poly.parentBrush = brush;
			poly.parentBrushSide = &lumps->brushsides[sideId];
			if( !polygonFromPlanes(localPlanes, j, poly) )
				continue;
			rawPolys.push_back(poly);
		}
	}

	//Remove overlayed polygons
	vector<CPolygon> polygons;

	//removeOverlayedPolygons(rawPolys, polygons);
	removePolygonsInsideSolids(rawPolys, polygons, lumps->brushes.addr,
		lumps->brushes.elements, lumps->brushsides.addr, planes);

	//Triangulate polygons
	for( size_t i = 0; i < polygons.size(); i++ )
	{
		//Create texture index
		int texinfoId = polygons[i].parentBrushSide->texinfo;
		int texIndex = CreateTextureIndex(texinfoId);

		//triangulate it, also does bsp -> rpm coordinate conversion
		int polyTris = polygons[i].triangulate(triangles);

		//calculate uv-coordinates
		const float (*pUVs)[4] = lumps->texinfos[texinfoId].textureVecsTexelsPerWorldUnits;
		for( int k = 0; k < polyTris; k++ )
		{
			Triangle &tri = triangles[triangles.size()-polyTris+k];
			tri.iTexID = texIndex;
			for( int l = 0; l < 3; l++ )
			{
				Vector3d vert( tri.vertex3[l].pos3 );
				real width = (real)lumps->texdatas[lumps->texinfos[texinfoId].texdata].width;
				real height = (real)lumps->texdatas[lumps->texinfos[texinfoId].texdata].height;
				tri.vertex3[l].uv2[0] = (real)pUVs[0][0]*(-vert.x) + (real)pUVs[0][1]*(vert.z) + (real)pUVs[0][2]*(vert.y) + (real)pUVs[0][3];
				tri.vertex3[l].uv2[0] /= width;
				tri.vertex3[l].uv2[1] = (real)pUVs[1][0]*(-vert.x) + (real)pUVs[1][1]*(vert.z) + (real)pUVs[1][2]*(vert.y) + (real)pUVs[1][3];
				tri.vertex3[l].uv2[1] /= -height;
			}
		}
	}

	delete[] planes;
}


//------------------------------------------------------------------------
//Purpose: tests
//------------------------------------------------------------------------
void testPolygon(void)
{
	CPlaned plane0( Vector3d(0, 1, 0), 10 );
	CPolygon poly0;
	poly0.fromBasePlane(plane0);
	poly0.chopPolygon( CPlaned(Vector3d(0.0, 0.0, -10.0), Vector3d( 0.0,  0.0, -1.0).Normalize()) );
	poly0.chopPolygon( CPlaned(Vector3d(0.0, 0.0, 10.0),  Vector3d( 0.0,  0.0,  1.0).Normalize()) );
	poly0.chopPolygon( CPlaned(Vector3d(-10.0, 0.0, 0.0), Vector3d(-1.0,  0.0,  0.0).Normalize()) );
	poly0.chopPolygon( CPlaned(Vector3d(10.0, 0.0, 0.0),  Vector3d( 1.0,  0.0,  0.0).Normalize()) );

	poly0.chopPolygon( CPlaned(Vector3d(8.0, 0.0, -8.0),  Vector3d( 1.0,  0.0, -1.0).Normalize()) );
	poly0.chopPolygon( CPlaned(Vector3d(-8.0, 0.0, -8.0), Vector3d(-1.0,  0.0, -1.0).Normalize()) );


	CPlaned plane1( Vector3d(1.0, 1.0, 0.0).GetNormalized(), 10.0 );
	CPolygon poly1;
	poly1.fromBasePlane(plane1);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void PrintDispinfoStats( int brushCount, const dbrush_t * brushes, const dbrushside_t * brushsides )
{
	{
		std::map<short, int> dispinfos;
		for( int i = 0; i < brushCount; i++ )
		{
			const dbrush_t &brush = brushes[i];
			for( int j = 0; j < brush.numsides; j++ )
			{
				short dispinfo = brushsides[brush.firstside+j].dispinfo;
				dispinfos[dispinfo]++;
			}
		}

		cout << "Dispinfo stats: " << endl;
		for(const std::pair<short, int> &p : dispinfos)
			cout << "  " << p.first << ": " << p.second << endl;
	}
}




