// renderer/CGeometryManager.cpp
// renderer/CGeometryManager.cpp
// renderer/CGeometryManager.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "glheaders.h"
#include "CGeometryManager.h"
#include "CVertexBuffer.h"
#include "CVertexIndexBuffer.h"

//The model-loading functionality should be moved here.



CGeometryManager::CGeometryManager()
{

}

CGeometryManager::~CGeometryManager()
{
	DeleteAllModels();
}


bool CGeometryManager::PrecacheModel( modelinfo_t *pInfo, const char *filename )
{
	//look in list if we have loaded the model before!
	for( unsigned int i = 0; i < m_vModels.size(); ++i )
	{
		if( strcmp( filename, m_vModels[i].filename.c_str() ) == 0 )
		{
			*pInfo = m_vModels[i];
			return true;
		}
	}

	//We have not loaded the model before!
	return false;
}


void CGeometryManager::DeleteAllModels( void )
{
	for( UINT i = 0; i < m_vModels.size(); i++ )
	{
		delete m_vModels[i].pVBO;

		for( int k = 0; k < m_vModels[i].iNumOfTexIndexes; k++ )
			delete m_vModels[i].pIBO[k];
	}

	m_vModels.clear();

	//models are reloaded by VOnRestore inside CBaseGeometry and/or other classes
}


void CGeometryManager::AddModelToList( modelinfo_t info )
{
	m_vModels.push_back( info );
}
