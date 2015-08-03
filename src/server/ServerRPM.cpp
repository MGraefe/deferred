// server/ServerRPM.cpp
// server/ServerRPM.cpp
// server/ServerRPM.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include <util/basic_types.h>
#include <util/rpm_file.h>
#include <util/instream.h>
#include <util/basicfuncs.h>

#include "ServerInterfaces.h"

rendertri_t *LoadRPMFileServer( const char *filename, int &iTris )
{
	rpm_file_t rpm;
	if( rpm.LoadFromFile(filename) != rpm_file_t::RPM_LOAD_OK )
		return NULL;

	rendertri_t *pTris = new rendertri_t[ rpm.getTriangleCount() ];
	for( int i = 0; i < rpm.getTriangleCount(); i++ )
		rpm_tri_to_rendertri( rpm.getTriangles()[i], pTris[i], 0 );

	iTris = rpm.getTriangleCount();
	return pTris;
}

//rendertri_t *LoadRPMFileServer( const char *filename, int &iTris, int **pMaterials = NULL, int *pNumOfMaterials = NULL )
//{
//	fs::ifstream is;
//	is.open( filename, std::ios::binary );
//	if( !is.is_open() )
//		return NULL;
//
//	rpm_file_t rpm;
//	rpm_file_t *rpmfile = &rpm;
//
//	is.read( (char*)&rpmfile->header, RPM_HEADER_SIZE );
//	if( rpmfile->header.version != RPM_VERSION )
//		return NULL;
//
//	rpmfile->triangles = new rpm_triangle_t[ rpmfile->header.num_of_triangles ];
//	rpmfile->texofs = new int[ rpmfile->header.sTexOfsetts ];
//	rpmfile->texnames = new char[ rpmfile->header.iTexdataLength ];
//
//	is.read( (char*)rpmfile->triangles, rpmfile->header.num_of_triangles * RPM_TRIANGLE_SIZE );
//	is.read( (char*)rpmfile->texofs, rpmfile->header.sTexOfsetts * sizeof(int) );
//	is.read( (char*)rpmfile->texnames, rpmfile->header.iTexdataLength * sizeof(char) );
//	is.close();
//
//	bool bDeleteMaterialsAfterExecution = false;
//	if( pMaterials == NULL )
//	{
//		bDeleteMaterialsAfterExecution = true;
//		pMaterials = new (int*);
//	}
//
//	*pMaterials = new int[rpmfile->header.sTexOfsetts];
//	//for( int i = 0; i < rpmfile->header.sTexOfsetts; i++ )
//	//{
//	//(*pMaterials)[i] = g_GraphicInterf->GetTextureManager()->LoadMaterial( rpmfile->texnames + rpmfile->texofs[i] );
//	//}
//
//	if( pNumOfMaterials )
//		*pNumOfMaterials = (int)rpmfile->header.sTexOfsetts;
//
//	rendertri_t *pTris = new rendertri_t[ rpmfile->header.num_of_triangles ];
//	for( int i = 0; i < rpmfile->header.num_of_triangles; i++ )
//	{
//		rpm_tri_to_rendertri( rpmfile->triangles[i], pTris[i], (*pMaterials)[rpmfile->triangles[i].iTexID] );
//	}
//
//	delete[] rpmfile->triangles;
//	delete[] rpmfile->texofs;
//	delete[] rpmfile->texnames;
//
//	if( bDeleteMaterialsAfterExecution )
//	{
//		delete[] *pMaterials;
//		delete pMaterials;
//	}
//
//	iTris = rpmfile->header.num_of_triangles;
//	return pTris;
//}
