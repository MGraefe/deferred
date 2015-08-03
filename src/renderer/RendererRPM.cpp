// renderer/RendererRPM.cpp
// renderer/RendererRPM.cpp
// renderer/RendererRPM.cpp
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
#include "glfunctions.h"
#include "TextureManager.h"
#include "CRenderInterf.h"


rendertri_t *LoadRPMFileRenderer( const char *filename, int &iTris, int **pMaterials = NULL, int *pNumOfMaterials = NULL, rpm_file_t *rpm = NULL )
{
	bool deleteRpm = false;
	if( !rpm )
	{
		deleteRpm = true;
		rpm = new rpm_file_t;
	}

	if( rpm->LoadFromFile(filename) != rpm_file_t::RPM_LOAD_OK )
		return NULL;
	
	bool bDeleteMaterialsAfterExecution = false;
	if( pMaterials == NULL )
	{
		bDeleteMaterialsAfterExecution = true;
		pMaterials = new (int*);
	}

	*pMaterials = new int[rpm->getTextureCount()];
	for( int i = 0; i < rpm->getTextureCount(); i++ )
	{
		const char *pTexName = rpm->getTexName(i);
		if( pTexName != NULL && pTexName[0] != '\0' )
			(*pMaterials)[i] = g_RenderInterf->GetTextureManager()->LoadMaterial( pTexName );
		else
			(*pMaterials)[i] = -1;
	}

	if( pNumOfMaterials )
		*pNumOfMaterials = rpm->getTextureCount();

	//check for unrenderable faces
	int iRenderables = rpm->getTriangleCount();
	bool *bIsRenderable = new bool[rpm->getTriangleCount()];
	for( int i = 0; i < rpm->getTriangleCount(); i++ )
	{
		const char *texname = rpm->getTexName(rpm->getTriangles()[i]);
		if( strstr( texname, "toolsphysclip" ) != NULL )
		{
			iRenderables--;
			bIsRenderable[i] = false;
		}
		else
			bIsRenderable[i] = true;
	}

	rendertri_t *pTris = new rendertri_t[iRenderables];
	int k = 0;
	for( int i = 0; i < iRenderables; i++ )
	{
		while( !bIsRenderable[k] )
			k++;
		Assert( k < rpm->getTriangleCount() );
		rpm_tri_to_rendertri( rpm->getTriangles()[k], pTris[i], (*pMaterials)[rpm->getTriangles()[k].iTexID] );
		for( int l = 0; l < 3; l++ )
			pTris[i].pVerts[l].fSkyVisibility = rpm->getSkyVisibility(k, l);
		k++;
	}

	delete[] bIsRenderable;

	if( bDeleteMaterialsAfterExecution )
	{
		delete[] *pMaterials;
		delete pMaterials;
	}

	if( deleteRpm )
		delete rpm;

	iTris = iRenderables;
	return pTris;
}


//rendertri_t *LoadRPMFileRenderer( const char *filename, int &iTris, int **pMaterials = NULL, int *pNumOfMaterials = NULL )
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
//	//std::vector<int> viUnrenderableIndexes;
//	*pMaterials = new int[rpmfile->header.sTexOfsetts];
//	for( int i = 0; i < rpmfile->header.sTexOfsetts; i++ )
//	{
//		char *pTexName = rpmfile->texnames + rpmfile->texofs[i];
//		(*pMaterials)[i] = g_RenderInterf->GetTextureManager()->LoadMaterial( rpmfile->texnames + rpmfile->texofs[i] );
//		//if( strstr( pTexName, "toolsphysclip" ) == NULL )
//		//	viUnrenderableIndexes.push_back(i);
//	}
//
//	if( pNumOfMaterials )
//		*pNumOfMaterials = (int)rpmfile->header.sTexOfsetts;
//
//	//check for unrenderable faces
//	int iRenderables = rpmfile->header.num_of_triangles;
//	bool *bIsRenderable = new bool[rpmfile->header.num_of_triangles];
//	for( int i = 0; i < rpmfile->header.num_of_triangles; i++ )
//	{
//		int texid = rpmfile->triangles[i].iTexID;
//		char *texname = rpmfile->texnames + rpmfile->texofs[texid];
//		if( strstr( texname, "toolsphysclip" ) != NULL )
//		{
//			iRenderables--;
//			bIsRenderable[i] = false;
//		}
//		else
//			bIsRenderable[i] = true;
//	}
//
//	rendertri_t *pTris = new rendertri_t[ iRenderables ];
//	int k = 0;
//	for( int i = 0; i < iRenderables; i++ )
//	{
//		while( !bIsRenderable[k] )
//			k++;
//		Assert( k < rpmfile->header.num_of_triangles );
//		rpm_tri_to_rendertri( rpmfile->triangles[k], pTris[i], (*pMaterials)[rpmfile->triangles[k].iTexID] );
//		k++;
//	}
//
//	delete[] bIsRenderable;
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
//	iTris = iRenderables;
//	return pTris;
//}
