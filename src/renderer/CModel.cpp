// renderer/CModel.cpp
// renderer/CModel.cpp
// renderer/CModel.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CModel.h"
#include "CRenderInterf.h"
#include <util/CEventManager.h>
#include <util/debug.h>


BEGIN_DATADESC_EDITOR(CModel)
	NETTABLE_ENTRY_PROXY(m_filename, CNetVarReadProxyString)
END_DATADESC_EDITOR()

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CModel::CModel( int index ) : CBaseGeometry(index)
{
	m_bLoadedModel = false;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CModel::~CModel()
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CModel::VOnRestore( void )
{
	CBaseGeometry::VOnRestore();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CModel::Init( const char *filename, const char *shadername, const Vector3f &AbsOrigin, const Quaternion &AbsAngles )
{
	SetAbsPos( AbsOrigin );
	SetAbsAngles( AbsAngles );

	m_bLoadedModel = LoadGeometry( filename, shadername );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CModel::VRender( const CRenderParams &params )
{
	if( m_bLoadedModel )
		CBaseGeometry::VRender(params);

	//Vector3f vDir;
	//AngleToVector( GetAbsAngles(), vDir );
	//Debug::DrawLine( GetAbsPos(), GetAbsPos() + vDir * 30.0f, Color(255,0,0) );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CModel::OnSpawn( void )
{
	BaseClass::OnSpawn();

	if( !m_bLoadedModel )
		Init(m_filename.c_str(), "GBufferOut", GetAbsPos(), GetAbsAngles());
}

