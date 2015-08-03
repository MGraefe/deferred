// renderer/CModel.h
// renderer/CModel.h
// renderer/CModel.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CModel_H__
#define deferred__renderer__CModel_H__

#include "CBaseGeometry.h"

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING

class DLL CModel: public CBaseGeometry
{
	DECLARE_DATADESC_EDITOR(CBaseGeometry, CModel)
public: 
	CModel( int index = -1 );
	~CModel();
	void Init( const char *filename, const char *shadername, const Vector3f &AbsOrigin, const Quaternion &AbsAngles );
	virtual void VRender( const CRenderParams &params );
	virtual void VOnRestore( void );
	virtual void OnSpawn( void );

private:
	bool m_bLoadedModel;
	std::string m_filename;
};

POP_PRAGMA_WARNINGS

#endif
