// bsptorpm/bsptorpm.cpp
// bsptorpm/bsptorpm.cpp
// bsptorpm/bsptorpm.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
// bsptorpm:
// Converts Valve BSP (http://developer.valvesoftware.com/wiki/Source_BSP_File_Format) into RedPuma Model (RPM).
// Copyright (c) 2010 Marius Gräfe.
#include "stdafx.h"
#include <util/wininc.h>
#include <direct.h>
#include <iostream>
#include <tchar.h>
#include <fstream>
#include <list>
#include <vector>
#include <map>
#include "bsptorpm.h"
#include "bspfile.h"
#include <util/rpm_file.h>
#include <util/maths.h>
#include "polygeneration.h"
#include "CBSPLumps.h"
#include <util/triangle_t.h>
#include <util/CSkyVisibilityCalculator.h>
#include "weldVertexes.h"
#include <util/CLightProbe.h>

#define RPM_VERSION 4

using namespace std;

//Inch->Decimetres Scaling factor
#define SCALE_FACTOR (2.54/10.0)
#define USE_ORIG_FACES
//#define SKIP_DISPLACEMENTS

bool g_bNoUserInput = false;


CBSPLumps lumps;
vector<Triangle> g_Triangles;
AABoundingBox g_mapBounds;

int g_iFixedTriangleNormals = 0;
int g_iFixedVertexNormals = 0;

int				*g_pTexOfs = NULL;
int				g_iTexOfs = 0;
char			*g_pTexNames = NULL;
char			*g_pTexNamesPtr = NULL;



//Stores for a given Vertex i the information if Vertex belongs to a Displacement
//char *m_vbDispFace; //1=true, 0=false

bool VectorsEqual( Vector3f v1, Vector3f v2, float fMaxOffset );
char *FixTextureName( char *pName );

const Vector3<real> fix_normal( 0, 1, 0 );

struct vertex_pos_t
{
	float pos3[3];
};

inline bool IsVertEqual( const rpm_vertex_t &vert1, const rpm_vertex_t &vert2 )
{
	return (vert1.pos3[0] == vert2.pos3[0] &&
			vert1.pos3[1] == vert2.pos3[1] &&
			vert1.pos3[2] == vert2.pos3[2] &&
			vert1.norm3[0] == vert2.norm3[0] && 
			vert1.norm3[1] == vert2.norm3[1] &&
			vert1.norm3[2] == vert2.norm3[2] );
}

void GetVertexList( rpm_triangle_t *pTris, int iTris )
{
	std::list<rpm_vertex_t> foundverts;
	std::list<rpm_vertex_t>::iterator foundverts_it;

	for( int i = 0; i < iTris; i++ ) //Iterate Tris
	{
		for( int k = 0; k < 2; k++ ) //Iterate Verts
		{
			bool bAlreadyInList = false;
			for( foundverts_it = foundverts.begin(); foundverts_it != foundverts.end(); foundverts_it++ ) //Iterate List
			{
				if( IsVertEqual( *foundverts_it, pTris[i].vertex3[k] ) )
				{
					bAlreadyInList = true;
					break;
				}
			}
			
			//When we havent found this vertex at all add it to the list
			if( !bAlreadyInList )
				foundverts.push_back( pTris[i].vertex3[k] );
		}
	}
}


//------------------------------------------------------------------------
//Purpose:
//------------------------------------------------------------------------
USHORT GetVertexIndex( const rpm_vertex_t &vert, vector<rpm_vertex_t> &Vector )
{
	for( UINT i = 0; i < Vector.size(); i++ )
	{
		if( IsVertEqual( vert, Vector[i] ) )
			return (USHORT)i;
	}

	//when we get here we havent found any matching verts.
	Vector.push_back(vert);
	return (USHORT)(Vector.size()-1);
}


//------------------------------------------------------------------------
//Purpose:
//------------------------------------------------------------------------
void StrToLower( char *pStr )
{
	const int l = strlen( pStr );
	for( int i = 0; i < l; i++ )
		pStr[i] = tolower(pStr[i]);
}


//------------------------------------------------------------------------
//Purpose:
//------------------------------------------------------------------------
int CreateTextureIndex( SHORT texinfo )
{
	const int texdata = lumps.texinfos[texinfo].texdata;
	const int TexStringTableID = lumps.texdatas[texdata].nameStringTableID;
	if( TexStringTableID < 0 || TexStringTableID > MAX_MAP_TEXDATA_STRING_TABLE )
		return -1;

	char *pTexNamePtr = lumps.texStringData.addr + lumps.texStringTable[TexStringTableID] * sizeof(char);
	char pTexName[128];
	if( strstr( pTexNamePtr, "maps/" ) == pTexNamePtr ) //Texture-Name needs fixup?
	{
		char *pFixedName = FixTextureName( pTexNamePtr );
		strcpy( pTexName, pFixedName );
		delete[] pFixedName;
	}
	else
	{
		strcpy( pTexName, pTexNamePtr );
	}
	
	//convert texture name to lower because I think its better
	StrToLower( pTexName );

	//search for already-there texture
	for( int i = 0; i < g_iTexOfs; i++ )
	{
		char *pName = g_pTexNames + g_pTexOfs[i] * sizeof(char);
		if( strcmp( pName, pTexName ) == 0 )
			return i; //Texture is already there!
	}

	//texture is not there, create new entry!
	strcpy( g_pTexNamesPtr, pTexName );
	//cout << "New Texture: \"" << pTexName << "\"" << endl;
	g_pTexOfs[g_iTexOfs] = g_pTexNamesPtr - g_pTexNames; //Generate Tex-Offset Entry
	g_iTexOfs++; //Adjust iTexOfs for next usage
	g_pTexNamesPtr += strlen(pTexName)+1; //Adjust the pointer for next usage
	return g_iTexOfs-1; //return index of just-generated-tex-ofs-entry
}


//------------------------------------------------------------------------
//Purpose:
//------------------------------------------------------------------------
void PressEnterToContinue( void )
{
	if( g_bNoUserInput )
		return;

	std::cout << "Press any key to continue..." << endl << std::flush;
	std::cin.sync();
	std::cin.ignore();
	//if( !g_bNoUserInput )
	//	system("PAUSE");
}


//------------------------------------------------------------------------
//Purpose: Does the user want to overwrite the existing file?
//------------------------------------------------------------------------
bool UserWantsToOverwrite( char *filepath )
{
	ifstream is;
	is.open( filepath, ios::binary );
	bool bFileExists = is.is_open();
	is.close();

	if( bFileExists )
	{
		if( g_bNoUserInput )
		{
			cout << "Overwriting existing file..." << endl;
			return true;
		}

		cout << "\nWarning: File already exists. Overwrite? (Y/N) " << std::flush;
		char overwrite[16];
		cin >> overwrite;
		if( overwrite[0] != 'Y' && overwrite[0] != 'y' )
		{
			cout << "\nNo. Ok, your choice." << endl;
			PressEnterToContinue();
			return false;
		}
	}

	return true;
}


//------------------------------------------------------------------------
// Purpose: Fixes a texture-name in the form
//			"maps/cc_coast1/concrete/concretefloor037b_-6176_-13456_280.vmt"
//			to "concrete/concretefloor037b"
//------------------------------------------------------------------------
char *FixTextureName( char* pName )
{
	char *pMapNameStart = pName + strlen( "maps/" );
	char *pTexNameStart = strchr( pMapNameStart, '/' ) + 1;
	
	//search for the last 3 '_'s
	int iUnderscores = 0;
	int iTexNameLength = 0;
	for( int i = strlen(pTexNameStart)-1; i >= 0; i-- )
	{
		if( pTexNameStart[i] == '_' )
		{
			iUnderscores++;
			if( iUnderscores == 3 )
			{
				iTexNameLength = i;
				break;
			}
		}
	}
	
	char *pRet = new char[iTexNameLength+1];
	strncpy( pRet, pTexNameStart, iTexNameLength );
	pRet[iTexNameLength] = 0;
	return pRet;
}


//------------------------------------------------------------------------
//Purpose: Is the Face renderable? (we do not need Trigger/Hint Brushes etc.)
//------------------------------------------------------------------------
bool IsRenderable( dface_t *pFace, texinfo_t *pTexinfos )
{
	if( pFace->numedges < 3 )
	{
		cout << "Error: face with less than 3 edges" << endl;
		return false;
	}

	if( pFace->texinfo == -1 )
		return false;

#ifdef	SKIP_DISPLACEMENTS
	if( pFace->dispinfo != -1 )
		return false;
#endif

	//displacements always have 4 edges
	if( pFace->dispinfo != -1 && pFace->numedges != 4 )
		return false;

	if( pFace->texinfo < 0 || pFace->texinfo > lumps.texinfos.elements-1 )
		return false;

	int texflags = pTexinfos[ pFace->texinfo ].flags;

	if( texflags != 0 && !(texflags & SURF_BUMPLIGHT) && !(texflags & SURF_LIGHT) && !(texflags & SURF_TRANS) )
		return false;
	//if( (texflags & SURF_NODRAW) ||
	//	(texflags & SURF_HINT) ||
	//	(texflags & SURF_SKIP) ||
	//	(texflags & SURF_SKY) ||
	//	(texflags & SURF_WARP) )
	//	return false;

	int texdata = pTexinfos[ pFace->texinfo ].texdata;
	int TexStringTableID = lumps.texdatas[texdata].nameStringTableID;
	if( TexStringTableID < 0 || TexStringTableID > MAX_MAP_TEXDATA_STRING_TABLE )
		return false;

#ifdef USE_ORIG_FACES
	if( texdata < 0 || texdata > MAX_MAP_TEXDATA )
		return false;
#endif


	return true;
}

//------------------------------------------------------------------------
//Purpose: Calculates the number of Triangles per Face
//------------------------------------------------------------------------
int NumberOfTriangles( dface_t *pFace )
{
	if( pFace->dispinfo == -1 )
		return pFace->numedges - 2;
	else
	{
		return lumps.dispinfos[pFace->dispinfo].NumTris(); // 2*(2^p)^2
	}
}


bool PointEqual( const Vector3<real> &a, const Vector3<real> &b, real tolerance = 0 )
{
	if( a[0] >= b[0] - tolerance &&
		a[0] <= b[0] + tolerance &&
		a[1] >= b[1] - tolerance &&
		a[1] <= b[1] + tolerance &&
		a[2] >= b[2] - tolerance &&
		a[2] <= b[2] + tolerance )
		return true;
	else
		return false;
}


void ComputeVertexNormal( Triangle &triangle, const vector<Triangle> &triangles, int v )
{
	///////////////////////////////////
	//Calculate Vertex normals out of Triangle normal!
	///////////////////////////////////
	//rpm_triangle_t * tri = file->triangles;
	int iTri = v / 3;
	int iVer = v % 3;

	Vector3<real> vSum = Vector3<real>( 0, 0, 0 );
	int iAverageDiv = 0;
	for( size_t i = 0; i < triangles.size(); i++ )
	{
		//Only handle Displacement Vertexes
		if( triangle.iDispVert == 0 )
			continue;

		real dot = triangles[i].norm3.Dot(triangle.norm3);

		if( dot < 0.5 ) //60°
			continue;

		if( PointEqual( triangles[i].vertex3[0].pos3, triangle.vertex3[iVer].pos3, 0.001f )||
			PointEqual( triangles[i].vertex3[1].pos3, triangle.vertex3[iVer].pos3, 0.001f )||
			PointEqual( triangles[i].vertex3[2].pos3, triangle.vertex3[iVer].pos3, 0.001f ) )
		{
			vSum += Vector3<real>( triangles[i].norm3 );
			iAverageDiv++;
		}
	}
	//All intersection with this vertex clear, calculate the normal!
	Vector3<real> vVertNorm = vSum / (real)iAverageDiv;

	if( !vVertNorm.IsValid() || vVertNorm.LengthSq() == 0.0 )
	{
		vVertNorm = fix_normal;
		//std::cout << "WARNING: Invalid vertex normal nr. "<<v<<", fixed normal to (0,1,0)."<<std::endl;
		g_iFixedVertexNormals++;
	}
	else
		vVertNorm.Normalize();

	//store
	triangle.vertex3[iVer].norm3 = vVertNorm;
}


//------------------------------------------------------------------------
// Purpose: Calculate Normals
//------------------------------------------------------------------------
void CalculateNormals( vector<Triangle> &triangles )
{
	int iTriangles = (int)triangles.size();

	//First pass, calculate per-face normals
	for( int i = 0; i < iTriangles; i++ )
	{
		Triangle *pTriangle = &triangles[i];

		//Dont calculate on normal faces!
		if( pTriangle->iDispVert == 0 )
		{
			for( int i = 0; i < 3; i++ )
				pTriangle->vertex3[i].norm3 = pTriangle->norm3;
			continue;
		}

		Vector3<real> v[3];
		for( int k = 0; k < 3; k++ )
		{
			v[k] = pTriangle->vertex3[k].pos3;
		}

		Vector3<real> btoa = v[0]-v[1];
		Vector3<real> btoc = v[2]-v[1];
		Vector3<real> normal = btoc.Cross(btoa);

		if( normal.LengthSq() == 0.0f )
		{
			normal = fix_normal;
			//std::cout << "WARNING: Line-shaped Triangle nr "<<i<<", fixed normal to (0,1,0)"<< std::endl;
			g_iFixedTriangleNormals++;
		}
		else
			normal.Normalize();

		Assert( normal.IsValid() );
		for( int k = 0; k < 3; k++ )
			pTriangle->vertex3[k].norm3 = normal;

		pTriangle->norm3 = normal;
	}

	//Second pass, calculate per-vertex normals on Displacements
	for( int i = 0; i < iTriangles*3; i++ ) //Triangle Loop
	{
		//Only Handle Displacement Vertexes
		if( triangles[i/3].iDispVert == 1 )
			ComputeVertexNormal( triangles[i/3], triangles, i );
	}
}



//------------------------------------------------------------------------
//Purpose: Transforms a Vector3f in BSPs Format into RPMs float3
//------------------------------------------------------------------------
void BSPVectorToRPMReal3( const Vector3<real> &v, real *real3 )
{
	real3[0] = -v.x;
	real3[1] = v.z;
	real3[2] = v.y;
}


//------------------------------------------------------------------------
//Purpose: Transforms a Vector3f in BSPs Format into RPMs float3
//------------------------------------------------------------------------
void BSPVectorToRPMVector( const Vector3<real> &src, Vector3<real> &dst )
{
	dst.x = -src.x;
	dst.y = src.z;
	dst.z = src.y;
}

//------------------------------------------------------------------------
//Purpose: Transforms BSP UV-Coordinates to RPM UV-Coordinates
//------------------------------------------------------------------------
void BspUvsToRpmUvs( real *bspuv2, real *rpmuvs2 )
{
	rpmuvs2[0] = -bspuv2[0];
	rpmuvs2[1] = -bspuv2[1];
}


//------------------------------------------------------------------------
//Purpose: Scales the whole Map
//------------------------------------------------------------------------
void ScaleRPM( vector<Triangle> &triangles )
{
	for( size_t i = 0; i < triangles.size(); i++ )
		for( size_t k = 0; k < 3; k++ )
			triangles[i].vertex3[k].pos3 *= SCALE_FACTOR;
}


//------------------------------------------------------------------------
//Purpose: Transforms a normal face (no Displacement)
//------------------------------------------------------------------------
void TransformFace( dface_t *pFace )
{
	for( int k = 0; k < pFace->numedges-2; k++ )
	{
		int iAbsEdges[3];
		int iNum[3];

		//pSurfEdges Values can be negative if edge is traced from the second to first index.
		//In this case we have to use the second edge instead of the first edge
		iNum[0] = lumps.surfedges[pFace->firstedge] < 0 ? 1 : 0;
		iNum[1] = lumps.surfedges[pFace->firstedge+k+2] < 0 ? 1 : 0; //opengl uses ccw culling but bsp is cw
		iNum[2] = lumps.surfedges[pFace->firstedge+k+1] < 0 ? 1 : 0;
		iAbsEdges[0] = abs( lumps.surfedges[pFace->firstedge] );
		iAbsEdges[1] = abs( lumps.surfedges[pFace->firstedge+k+2] ); //opengl uses ccw culling but bsp is cw
		iAbsEdges[2] = abs( lumps.surfedges[pFace->firstedge+k+1] );

		Triangle tri;
		for( int m = 0; m < 3; m++ )
		{
			BSPVectorToRPMVector(lumps.vertexes[lumps.edges[iAbsEdges[m]].v[iNum[m]]].point.getOther<real>(), tri.vertex3[m].pos3);
			
			//calculate uv-coordinates
			float (*pUVs)[4] = lumps.texinfos[pFace->texinfo].textureVecsTexelsPerWorldUnits;
			Vector3<real> vert( tri.vertex3[m].pos3 );
			real width = (real)lumps.texdatas[ lumps.texinfos[ pFace->texinfo ].texdata ].width;
			real height = (real)lumps.texdatas[ lumps.texinfos[ pFace->texinfo ].texdata ].height;

			tri.vertex3[m].uv2[0] = (real)pUVs[0][0]*(-vert.x) + (real)pUVs[0][1]*(vert.z) + 
				(real)pUVs[0][2]*(vert.y) + (real)pUVs[0][3];
			tri.vertex3[m].uv2[0] /= width;
			tri.vertex3[m].uv2[1] = (real)pUVs[1][0]*(-vert.x) + (real)pUVs[1][1]*(vert.z) + 
				(real)pUVs[1][2]*(vert.y) + (real)pUVs[1][3];
			tri.vertex3[m].uv2[1] /= -height;
		}
		
		tri.iTexID = CreateTextureIndex( pFace->texinfo );
		g_Triangles.push_back(tri);

		//t++;
	}
}

//------------------------------------------------------------------------
//Purpose: integer pow function
//------------------------------------------------------------------------
int pow( int a, int b )
{
	int c = 1;
	if( b < 0 )
		for( int i = 0; i < -b; i++ )
			c /= a;
	else
		for( int i = 0; i < b; i++ )
			c *= a;
	return c;
}


//------------------------------------------------------------------------
// Purpose: Transforms Vertexes from a Displacement into Triangles
//			e.g. 25 Vertexes -> 32 Triangles á 3 Vertexes
//------------------------------------------------------------------------
void VertexesToRPMTriangles( Vector3<real> *pVerts, int iVertsPerSide, real *pUVs, short texinfo )
{
	int iVerts = iVertsPerSide * iVertsPerSide;
	int iMax = (iVertsPerSide-1)*(iVertsPerSide-1);

	int iV0, iV1, iV2, iV3;
	bool bDir = false;

	for( int i = 0; i < iMax; i++ )
	{
		iV0 = i + i / (iVertsPerSide-1);
		iV1 = iV0 + 1;
		iV3 = iV0 + iVertsPerSide;
		iV2 = iV3 + 1;
		Triangle tri0, tri1;
		if( !bDir )
		{
			BSPVectorToRPMVector( pVerts[iV0], tri0.vertex3[0].pos3 );
			BSPVectorToRPMVector( pVerts[iV1], tri0.vertex3[1].pos3 );
			BSPVectorToRPMVector( pVerts[iV2], tri0.vertex3[2].pos3 );
			BspUvsToRpmUvs( &pUVs[iV0*2], tri0.vertex3[0].uv2 );
			BspUvsToRpmUvs( &pUVs[iV1*2], tri0.vertex3[1].uv2 );
			BspUvsToRpmUvs( &pUVs[iV2*2], tri0.vertex3[2].uv2 );
			tri0.iTexID = CreateTextureIndex( texinfo );
			BSPVectorToRPMVector( pVerts[iV0], tri1.vertex3[0].pos3 );
			BSPVectorToRPMVector( pVerts[iV2], tri1.vertex3[1].pos3 );
			BSPVectorToRPMVector( pVerts[iV3], tri1.vertex3[2].pos3 );
			BspUvsToRpmUvs( &pUVs[iV0*2], tri1.vertex3[0].uv2 );
			BspUvsToRpmUvs( &pUVs[iV2*2], tri1.vertex3[1].uv2 );
			BspUvsToRpmUvs( &pUVs[iV3*2], tri1.vertex3[2].uv2 );
			tri1.iTexID = CreateTextureIndex( texinfo );
		}
		else
		{
			BSPVectorToRPMVector( pVerts[iV0], tri0.vertex3[0].pos3 );
			BSPVectorToRPMVector( pVerts[iV1], tri0.vertex3[1].pos3 );
			BSPVectorToRPMVector( pVerts[iV3], tri0.vertex3[2].pos3 );
			BspUvsToRpmUvs( &pUVs[iV0*2], tri0.vertex3[0].uv2 );
			BspUvsToRpmUvs( &pUVs[iV1*2], tri0.vertex3[1].uv2 );
			BspUvsToRpmUvs( &pUVs[iV3*2], tri0.vertex3[2].uv2 );
			tri0.iTexID = CreateTextureIndex( texinfo );
			BSPVectorToRPMVector( pVerts[iV1], tri1.vertex3[0].pos3 );
			BSPVectorToRPMVector( pVerts[iV2], tri1.vertex3[1].pos3 );
			BSPVectorToRPMVector( pVerts[iV3], tri1.vertex3[2].pos3 );
			BspUvsToRpmUvs( &pUVs[iV1*2], tri1.vertex3[0].uv2 );
			BspUvsToRpmUvs( &pUVs[iV2*2], tri1.vertex3[1].uv2 );
			BspUvsToRpmUvs( &pUVs[iV3*2], tri1.vertex3[2].uv2 );
			tri1.iTexID = CreateTextureIndex( texinfo );
		}

		tri0.iDispVert = 1;
		tri1.iDispVert = 1;
		g_Triangles.push_back(tri0);
		g_Triangles.push_back(tri1);
		
		//Change Direction of Triangles so that they look crossed
		if( i % (iVertsPerSide-1) != iVertsPerSide-2 )
			bDir = !bDir;
	}
}


//------------------------------------------------------------------------
//Purpose: Not used atm.
//------------------------------------------------------------------------
void MirrorBSPVertexes( Vector3f *pVerts, int iVertsPerSide, bool bX, bool bY )
{
	int iVtx = iVertsPerSide * iVertsPerSide;
	Vector3f *pVertsOrig = new Vector3f[iVtx];

	if( bX )
	{
		memcpy( pVertsOrig, pVerts, iVtx * sizeof(Vector3f) );

		for( int i = 0; i < iVtx; i++ )
		{
			int column = (iVertsPerSide-1) - (i % iVertsPerSide);
			int row = i / iVertsPerSide;
			pVerts[i] = pVertsOrig[ row * iVertsPerSide + column ];
		}
	}

	if( bY )
	{
		memcpy( pVertsOrig, pVerts, iVtx * sizeof(Vector3f) );

		for( int i = 0; i < iVtx; i++ )
		{
			int column = i % iVertsPerSide;
			int row = (iVertsPerSide-1) - (i / iVertsPerSide);
			pVerts[i] = pVertsOrig[ row * iVertsPerSide + column ];
		}
	}

	delete[] pVertsOrig;
}


//------------------------------------------------------------------------
//Purpose: Compare two Vectors with tolerance
//------------------------------------------------------------------------
bool VectorsEqual( Vector3<real> v1, Vector3<real> v2, real fMaxOffset )
{
	Vector3<real> v = v2 - v1;
	return ( v.LengthSq() <= fMaxOffset*fMaxOffset );
}


//------------------------------------------------------------------------
//Purpose: fixup Corners, so that vCorners[0] == vStartPosition
//------------------------------------------------------------------------
void FixupDisplacementCorners( Vector3<real> *vCorners, Vector3<real> vStartPosition )
{
	int i;
	Vector3<real> vCornersCpy[4];
	memcpy( vCornersCpy, vCorners, 4*sizeof(Vector3<real>) );

	//Get the index of the starting edge
	for( i = 0; i < 4; i++ )
	{
		if( VectorsEqual( vCorners[i], vStartPosition, 0.1 ) )
			break;

		if( i == 3 )
			cout << "\n### FixupDisplacementCorners Error! ###" << endl; //Cannot find the starting edge
	}

	//Rotate Edges by i to the "right"
	for( int k = 0; k < 4; k++ )
	{
		int m = k+i;
		if( m > 3 ) 
			m -= 4;
		else if( m < 0 )
			m += 4;
		
		vCorners[k] = vCornersCpy[m];
	}
}


//------------------------------------------------------------------------
// Purpose: Transforms a Displacement into World-Coordinate-Triangles
//			This one is fairly complex, need a better describtion!
//------------------------------------------------------------------------
void TransformDisplacement( dface_t *pFace )
{
	ddispinfo_t *pDisp = &lumps.dispinfos[pFace->dispinfo];
	CDispVert *pVerts = &lumps.dispverts[pDisp->m_iDispVertStart];
	int iTris = pDisp->NumTris();
	int iVerts = pDisp->NumVerts();

	//get main corner-points of the original face
	Vector3<real> vCorners[4];
	for( int i = 0; i < 4; i++ )
	{	
		int iAbsEdge = abs( lumps.surfedges[pFace->firstedge+i] );
		int iNum = lumps.surfedges[pFace->firstedge+i] < 0 ? 1 : 0;
		vCorners[i].setFromOther(lumps.vertexes[lumps.edges[iAbsEdge].v[iNum]].point);
	}
	
	int iVtxPerSide = pow(2,pDisp->power) + 1; //Vertexes per side

	//Fixup corners because sometimes vCorners[0] != pDisp->vStartPoint
	FixupDisplacementCorners( vCorners, pDisp->startPosition.getOther<real>() );

	//Calculate the original Face's Edges
	Vector3<real> vEdgeX1, vEdgeX2, vEdgeY1, vEdgeY2;
	vEdgeX1 = (vCorners[3] - vCorners[0]); //Length of Edge == distance between corners
	vEdgeY1 = (vCorners[1] - vCorners[0]);
	vEdgeX2 = (vCorners[2] - vCorners[1]);
	vEdgeY2 = (vCorners[2] - vCorners[3]);

	//These are temporarily used
	Vector3<real> *vVertexes = new Vector3<real>[iVerts];
	real *fUVs = new real[iVerts*2];

	//calculate world positions of vertexes
	//in BSP each vertex is stored as direction and distance from its original point
	for( int i = 0; i < iVerts; i++ )
	{
		int VertexCoordX = i % iVtxPerSide; // = column
		int VertexCoordY = i / iVtxPerSide; // = row
		real WeightX = ( 1.0f / (iVtxPerSide-1) ) * VertexCoordX; //ranges from 0 to 1 depending on Column
		real WeightY = ( 1.0f / (iVtxPerSide-1) ) * VertexCoordY; //ranges from 0 to 1 depending on Row

		//Calculate the original point of vertex
		Vector3<real> vLineStart = vCorners[0] + vEdgeY1 * WeightY;
		Vector3<real> vLineEnd = vCorners[3] + vEdgeY2 * WeightY;
		Vector3<real> vLine = vLineEnd - vLineStart;
		Vector3<real> vOrigPoint = vLineStart + vLine * WeightX;

		//Add Offset to our original Point
		vVertexes[i] = vOrigPoint + pVerts[i].m_vVector.getOther<real>() * (real)pVerts[i].m_flDist;

		//calculate uv-coordinates
		float (*pUVs)[4] = lumps.texinfos[pFace->texinfo].textureVecsTexelsPerWorldUnits;
		Vector3<real> vert( vOrigPoint );
		real width = (real)lumps.texdatas[ lumps.texinfos[ pFace->texinfo ].texdata ].width;
		real height = (real)lumps.texdatas[ lumps.texinfos[ pFace->texinfo ].texdata ].height;
		fUVs[i*2] = (real)pUVs[0][0]*vert.x + (real)pUVs[0][1]*vert.y + (real)pUVs[0][2]*vert.z + (real)pUVs[0][3];
		fUVs[i*2] /= width;
		fUVs[i*2+1] = (real)pUVs[1][0]*vert.x + (real)pUVs[1][1]*vert.y + (real)pUVs[1][2]*vert.z + (real)pUVs[1][3];
		fUVs[i*2+1] /= height;
	}

	//Transform the resulting BSP World Coordinate Vertexes into RPM Format Triangles
	VertexesToRPMTriangles( vVertexes, iVtxPerSide, fUVs, pFace->texinfo );

	delete[] vVertexes;
	delete[] fUVs;
}


//------------------------------------------------------------------------
// Purpose: Somehow the texinfos on original faces are screwed,
//			so here we just go through all faces and set the texinfo of
//			the corresponding original face to the face's texinfo.
//			This isn't actually the nicest way, but since this is done only
//			once per convertion I can live with it.
//------------------------------------------------------------------------
void FixupOrigFaces( dface_t *pOrigFaces, int iOrigFaces, dface_t *pFaces, int iFaces )
{
	//First set all orig-face texinfo's to -1
	for( int i = 0; i < iOrigFaces; i++ )
	{
		pOrigFaces[i].texinfo = -1;
	}

	int iFacesWithoutOrig = 0;
	for( int i = 0; i < iFaces; i++ )
	{
		dface_t *pFace = &pFaces[i];
		if( pFace->origFace < 0 || pFace->origFace >= iOrigFaces )
		{
#ifdef _DEBUG
			cout << "ERROR: Wrong original face with index " << i << endl;
#endif
			iFacesWithoutOrig++;
			continue;
		}

		pOrigFaces[pFace->origFace].texinfo = pFace->texinfo;
	}

	cout << "Faces without corresponding original face: " << iFacesWithoutOrig << endl;
}


//------------------------------------------------------------------------
//Purpose:
//------------------------------------------------------------------------
void ReplaceAllOccurencesInString( std::string &str, const std::string &needle, const std::string &replacement )
{
	size_t pos = 0;
	while( (pos = str.find(needle,pos)) != std::string::npos )
	{
		str.replace( pos, needle.length(), replacement );
		pos += replacement.length();
	}
}


//------------------------------------------------------------------------
//Purpose: Store Entity-related data
//------------------------------------------------------------------------
bool ConvertAndStoreEntityDataFile( const char *pString, int iStringSize, const char *pFilePath )
{
	std::string str;
	str.assign( pString, iStringSize );
	
	ReplaceAllOccurencesInString( str, "{", "\n\"ENTITY\"\n{" );

	std::ofstream os;
	os.open( pFilePath, ios::trunc | ios::in );
	if( !os.is_open() )
		return false;

	os.write( str.c_str(), str.length() );
	os.close();

	if( !os.good() )
		return false;

	return true;
}


//------------------------------------------------------------------------
//Purpose: Calculate mins & maxs
//------------------------------------------------------------------------
AABoundingBox_t<real> calculateMapBounds(const std::vector<Triangle> triangles)
{
	AABoundingBox_t<real> box;

	if( triangles.size() > 0 )
	{
		box.max = box.min = triangles[0].vertex3[0].pos3;
		for( size_t i = 0; i < triangles.size(); i++ )
		{
			for( int k = 0; k < 3; k++ )
			{
				box.min = box.min.MinComponentWise(triangles[i].vertex3[k].pos3);
				box.max = box.max.MaxComponentWise(triangles[i].vertex3[k].pos3);
			}
		}
	}

	return box;
}


const bool bBrushMethod = true;

#define START_ACTION(name, action) cout << name << endl; start=timeGetTime(); action; stop=timeGetTime(); cout << "done. (" << (float)(stop-start)/1000.0f << "s)\n" << endl

//------------------------------------------------------------------------
//Purpose: Main Function for transforming a BSP File into RPM Triangles
//------------------------------------------------------------------------
void TransformBSPToRPM( bool calcSkyVis = true, std::vector<CLightProbe> *lightProbes = NULL )
{
	DWORD start, stop;
	cout << "\nConverting brushes to triangles..." << endl;
	start = timeGetTime();

	//do the real transforming
	if( bBrushMethod )
	{
		brushesToTriangles(&lumps, g_Triangles);

		for( int i = 0; i < lumps.origfaces.elements; i++ )
		{
			if( lumps.origfaces[i].dispinfo != -1 )
				TransformDisplacement( &lumps.origfaces[i] );
		}
	}
	else
	{
#ifdef USE_ORIG_FACES
		for( int i = 0; i < lumps.origfaces.elements; i++ )
		{
			dface_t *pFace = &lumps.origfaces[i];
#else
		for( int i = 0; i < lumps.faces.elements; i++ )
		{
			dface_t *pFace = &lumps.faces[i];
#endif
			if( !IsRenderable( pFace, lumps.texinfos.get() ) )
				continue;
		
			if( pFace->dispinfo == -1 )
				TransformFace( pFace );
#ifndef SKIP_DISPLACEMENTS
			else
				TransformDisplacement( pFace );
#endif
		}
	}
	stop = timeGetTime();
	cout << "done. (" << (float)(stop-start)/1000.0f << "s)\n" << endl;

	START_ACTION("Scaling Model...", ScaleRPM(g_Triangles)); 
	START_ACTION("Welding vertexes...", weldVertexes(g_Triangles));
	START_ACTION("Calculating map bounds...", AABoundingBox_t<real> bounds = calculateMapBounds(g_Triangles));
	cout << "\tmin = (" << bounds.min.x << ", " << bounds.min.y << ", " << bounds.min.z << ")" << endl;
	cout << "\tmax = (" << bounds.max.x << ", " << bounds.max.y << ", " << bounds.max.z << ")" << endl;

	g_mapBounds.min = bounds.min.toVec3f();
	g_mapBounds.max = bounds.max.toVec3f();

	START_ACTION("Calculating normals...", CalculateNormals(g_Triangles));
	if( g_iFixedTriangleNormals > 0 )
		cout << "\tWARNING: Fixed "<<g_iFixedTriangleNormals<<" triangle-normals because of line-shaped triangles." << endl;
	if( g_iFixedVertexNormals > 0 )
		cout << "\tWARNING: Fixed "<<g_iFixedVertexNormals<<" per-vertex normals because of line-shaped triangles." << endl;

	CSkyVisibilityCalculator skyVC(g_Triangles, bounds);
	if(calcSkyVis)
		skyVC.calculate();
	if(lightProbes)
		skyVC.calculateLightProbeSkyVis(*lightProbes);
}


//------------------------------------------------------------------------
//Purpose:
//------------------------------------------------------------------------
void GetOutputFilePath( char *OrigFilePath, char *OutputFilePath )
{
	//find the last "/" or "\" in the original file-path
	char *FileNameStart1 = strrchr( OrigFilePath, '\\' );
	char *FileNameStart2 = strrchr( OrigFilePath, '/' );
	char *FileNameStart = std::max( FileNameStart1, FileNameStart2 );
	if( FileNameStart == NULL )
		FileNameStart = OrigFilePath;

	//search the last occurence of ".bsp" in the original file-path
	char *BspStart = strstr( FileNameStart, ".bsp" );

	//is the occurence really at the end of the filepath? it might be somewhere in between
	if( BspStart - FileNameStart != strlen(FileNameStart) - 4 )
		BspStart = NULL;

	if( BspStart == NULL )
	{
		//no ".bsp" at the end of the original file-path, so just add ".rpm" to the total filename
		strcpy( OutputFilePath, OrigFilePath );
		strcat( OutputFilePath, ".rpm" );
	}
	else
	{
		//replace .bsp
		strcpy( OutputFilePath, OrigFilePath );
		OutputFilePath[ strlen(OutputFilePath)-4 ] = NULL;
		strcat( OutputFilePath, ".rpm" );
	}
}


//------------------------------------------------------------------------
//Purpose:
//------------------------------------------------------------------------
void ReplaceFilePath( std::string &oldpath, const std::string &replacement )
{
	size_t pos = oldpath.find_last_of( "/\\" );
	if( pos != std::string::npos )
		oldpath.erase(0,pos+1);
	oldpath.insert(0,replacement);
}


//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
void CopyFilesToGameDirectory( const char * filepath, const std::string &entityFilePath, const std::string &outDir ) 
{
	std::string sOutDir( outDir );

	//extract map filename without ending
	std::string sFilePath(filepath);
	std::string sFileNameWithoutEnding(sFilePath);
	size_t lastSlash = sFilePath.find_last_of("/\\");
	if( lastSlash != string::npos )
		sFileNameWithoutEnding.erase(sFileNameWithoutEnding.begin(), sFileNameWithoutEnding.begin()+(lastSlash+1));
	size_t endingDot = sFileNameWithoutEnding.find_last_of('.');
	if( endingDot != string::npos )
		sFileNameWithoutEnding.erase(sFileNameWithoutEnding.begin()+endingDot, sFileNameWithoutEnding.end());
		
	std::string sNewFilePath;
	sNewFilePath += sOutDir;
	sNewFilePath += "models\\";
	sNewFilePath += sFileNameWithoutEnding;

	std::string sRpmFilePath = sNewFilePath;
	sRpmFilePath += ".rpm";
	cout << "Copying .rpm file to \"" << sRpmFilePath.c_str() << "\"." << endl;
	CopyFile( filepath, sRpmFilePath.c_str(), FALSE );

	std::string sEntsFilePath = sNewFilePath;
	sEntsFilePath += ".ents";
	cout << "Copying .ents file to \"" << sEntsFilePath.c_str() << "\"." << endl;
	CopyFile( entityFilePath.c_str(), sEntsFilePath.c_str(), FALSE );
}


//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
void VectorToFloat3( float *float3, const Vector3<real> &v )
{
	float3[0] = (float)v.x;
	float3[1] = (float)v.y;
	float3[2] = (float)v.z;
}

void TriangleToRPMTriangle(rpm_triangle_t &rt, const Triangle &t)
{
	rt.iTexID = t.iTexID;
	if( rt.iTexID < 0 )
		cout << "WARNING: invalid texid on triangle" << endl;

	rt.norm3.setFromOther(t.norm3);
	for( int k = 0; k < 3; k++ )
	{
		rt.vertex3[k].pos3.setFromOther(t.vertex3[k].pos3);
		rt.vertex3[k].norm3.setFromOther(t.vertex3[k].norm3);
		rt.vertex3[k].uv2[0] = (float)t.vertex3[k].uv2[0];
		rt.vertex3[k].uv2[1] = (float)t.vertex3[k].uv2[1];
	}
}

//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
void createRpmFile( const vector<Triangle> &triangles, rpm_file_t &rpmfile )
{
	rpmfile.header.bounds = g_mapBounds;
	rpmfile.header.num_of_triangles = triangles.size();
	rpmfile.header.iTexdataLength = g_pTexNamesPtr - g_pTexNames;
	rpmfile.header.sTexOfsetts = g_iTexOfs;
	rpm_triangle_t *rpmtris = new rpm_triangle_t[triangles.size()];
	float *skyViss = new float[triangles.size()*3];
	rpmfile.addLump(rpm_lump(LUMP_TRIANGLES, triangles.size() * sizeof(rpm_triangle_t), (char*)rpmtris));
	rpmfile.addLump(rpm_lump(LUMP_SKYVIS, triangles.size() * 3 * sizeof(float), (char*)skyViss));
	rpmfile.addLump(rpm_lump(LUMP_TEXNAMES, rpmfile.header.iTexdataLength, (char*)g_pTexNames));
	rpmfile.addLump(rpm_lump(LUMP_TEXOFS, g_iTexOfs * sizeof(int), (char*)g_pTexOfs));

	cout << "Number of resulting triangles: " << rpmfile.header.num_of_triangles << endl;

	for( size_t i = 0; i < triangles.size(); i++ )
	{
		TriangleToRPMTriangle(rpmtris[i], triangles[i]);
		for( int k = 0; k < 3; k++ )
			skyViss[i*3+k] = (float)triangles[i].vertex3[k].skyVisibility;
	}
}


//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
void AllocateOutputSpaces() 
{
	//Allocate Space for Texture-Offset-Array and Texture-Name-Array
	g_pTexOfs = new int[512];
	g_pTexNames = new char[512*128];
	g_pTexNamesPtr = g_pTexNames;
}

//------------------------------------------------------------------------
//Purpose: 
//------------------------------------------------------------------------
void WriteMapOutputFile( char * filepath, rpm_file_t &rpmfile ) 
{
	ofstream os;
	os.open( filepath, ios::binary | ios::trunc );
	if( rpmfile.Serialize(os) != rpm_file_t::rpm_write_errors_e::RPM_WRITE_OK )
		cout << "Error while writing file!";
}



//------------------------------------------------------------------------
//Purpose: Read BSP Lumps -> Transform BSP into RPM -> Store RPM
//------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
	//Init windows stuff
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	cout << "###  BSP to RPM Converter\n"
			"###  Copyright (c) 2010-2011 Marius Graefe\n"
			"###  Build: " << __DATE__ << " " << __TIME__
#ifdef _DEBUG
		<< " DEBUG BUILD"
#endif
		<< endl << endl;

	string sGamePath;
	bool gamePathValid = false;
	{
		char gamePath[512];
		DWORD ret = GetEnvironmentVariable("deferredgame", gamePath, 512);
		gamePathValid = ret != 0 && ret <= 512;
		if( gamePathValid )
		{
			sGamePath = gamePath;
			size_t pos;
			while( (pos = sGamePath.find_first_of("\"'")) != string::npos)
				sGamePath.erase(pos);
			while( (pos = sGamePath.find_first_of("/")) != string::npos)
				sGamePath[pos] = '\\';
			if( sGamePath[sGamePath.length()-1] != '\\' )
				sGamePath.append("\\");
			cout << "Game Path: " << sGamePath.c_str() << endl << endl;
		}
		else
		{
			cout << "ERROR: Game Path NOT FOUND, create an environment-variable with "
				"name \"deferredgame\" and the path to the game-directory. "
				"You may have to restart your pc before the changes take place." << endl;
			cout << "Windows reported error: " << endl;
			char errormsg[512];
			DWORD err = GetLastError();
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, LANG_SYSTEM_DEFAULT, errormsg, 512, NULL);
			cout << errormsg << endl;
			PressEnterToContinue();
			return ERROR_ENVVAR_NOT_FOUND;
		}
	}

	if( argc <= 1 )
	{
		cout << "Usage: bsptorpm <filepath> [-nouserinput]" << endl;
		PressEnterToContinue();
		return ERROR_INVALID_FIELD_IN_PARAMETER_LIST;
	}

	if( argc > 2 && !strcmp( argv[2], "-nouserinput" ) )
	{
		cout << "No user input mode." << endl;
		g_bNoUserInput = true;
	}

	char pchWorkingDirectory[512];
	_getcwd( pchWorkingDirectory, 512 );
	cout << "Working directory: " << pchWorkingDirectory << endl;

	//open BSP File
	std::ifstream is;
	is.open( argv[1], ios::binary );
	if( !is.is_open() )
	{
		cout << "File not found." << endl;
		PressEnterToContinue();
		return ERROR_FILE_NOT_FOUND;
	}
	else
	{
		cout << "Opening file " << argv[1] << endl;
	}

	//Read File
	if( !lumps.read(is) )
	{
		PressEnterToContinue();
		return ERROR_INVALID_DATA;
	}

	//CalculateAndCoutNumberOfTriangles();
#ifdef USE_ORIG_FACES
	FixupOrigFaces(lumps.origfaces, lumps.origfaces.elements, lumps.faces, lumps.faces.elements);
#endif

	AllocateOutputSpaces();
	TransformBSPToRPM();

	rpm_file_t rpmfile;
	createRpmFile(g_Triangles, rpmfile);

	//Get the output file path.
	char filepath[512];
	GetOutputFilePath( argv[1], filepath );

	cout << "\nFinished convertion.\n\nWriting output file: \"" << filepath << "\" ..." << std::flush;

	if( !UserWantsToOverwrite( filepath ) )
		return 0;

	WriteMapOutputFile(filepath, rpmfile);
	cout << "\nFinished writing File!" << endl;

	cout << "Writing Entity-Data File...";
	std::string entityFilePath(filepath);
	entityFilePath.replace(entityFilePath.length()-3, 3, "ents" );
	if( !ConvertAndStoreEntityDataFile( lumps.entData, lumps.entData.elements, entityFilePath.c_str() ) )
		cout << "\nError writing Entity-File \"" << entityFilePath.c_str() << "\". Did not write Entity File." << endl;
	else
		cout << "finished successfully." << endl;
	
	CopyFilesToGameDirectory(filepath, entityFilePath, sGamePath);

	cout << "ok." << endl;
	PressEnterToContinue();

	if( argc > 3 && strcmp(argv[3], "-startgame") == 0)
	{
		cout << "Starting deferred.exe...";
		string exePath(sGamePath);
		exePath.append("deferred.exe");
		ShellExecute( NULL, "open", exePath.c_str(), NULL, sGamePath.c_str(), SW_SHOWNORMAL );
	}

	return 0;
}

