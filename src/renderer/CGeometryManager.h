// renderer/CGeometryManager.h
// renderer/CGeometryManager.h
// renderer/CGeometryManager.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CGeometryManager_H__
#define deferred__renderer__CGeometryManager_H__

#include <vector>
#include "mesh_vertex_t.h"
#include "CRenderInterf.h"
#include <util/rpm_file.h>

class CVertexBuffer;
template<typename T> class CVertexIndexBuffer;

//note: currently a maximum of 4 textures per model
#define MAX_MATERIALS_PER_MODEL 4
#define MAX_MATERIALS_PER_MODEL_FILE __FILE__
class modelinfo_t
{
public:
	modelinfo_t() :
		pVBO(NULL),
		iNumOfTexIndexes(0)
	{}

	modelinfo_t(const modelinfo_t &other) :
		filename(other.filename),
		pVBO(other.pVBO),
		iNumOfTexIndexes(other.iNumOfTexIndexes),
		rpm(other.rpm)
	{
		for(int i = 0; i < MAX_MATERIALS_PER_MODEL; i++)
		{
			pIBO[i] = other.pIBO[i];
			pTexIndexes[i] = other.pTexIndexes[i];
		}
	}

	modelinfo_t &operator=(const modelinfo_t &other)
	{
		filename = other.filename;
		pVBO = other.pVBO;
		iNumOfTexIndexes = other.iNumOfTexIndexes;
		for(int i = 0; i < MAX_MATERIALS_PER_MODEL; i++)
		{
			pIBO[i] = other.pIBO[i];
			pTexIndexes[i] = other.pTexIndexes[i];
		}
		rpm = other.rpm;
		return *this;
	}

public:
	std::string filename;
	CVertexBuffer *pVBO;
	CVertexIndexBuffer<USHORT> *pIBO[MAX_MATERIALS_PER_MODEL];
	int pTexIndexes[MAX_MATERIALS_PER_MODEL];
	int iNumOfTexIndexes;
	rpm_file_t rpm;
};

class CGeometryManager
{
public:
	CGeometryManager();
	~CGeometryManager();
	bool PrecacheModel( modelinfo_t *pInfo, const char *filename );
	void AddModelToList( modelinfo_t info );
	void DeleteAllModels( void );
private:
	std::vector<modelinfo_t>	m_vModels;
};


inline CGeometryManager *GetGeometryManager( void )
{
	return g_RenderInterf->GetGeometryManager();
}


#endif
