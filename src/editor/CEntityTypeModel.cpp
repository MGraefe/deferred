// editor/CEntityTypeModel.cpp
// editor/CEntityTypeModel.cpp
// editor/CEntityTypeModel.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "IEditorEntityFactory.h"
#include "CEntityTypeModel.h"
#include <CModel.h>
#include <renderer.h>
#include <CRenderList.h>
#include <util/StringUtils.h>

DECLARE_EDITOR_ENTITY_TYPE("MODEL", CEntityTypeModel)


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityTypeModel::CEntityTypeModel( const CScriptSubGroup *pGrp, int index ) 
	: BaseClass(pGrp,index)
{
	m_model = NULL;
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityTypeModel::CEntityTypeModel( const std::string &classname, int index ) 
	: BaseClass(classname,index)
{
	m_model = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityTypeModel::CEntityTypeModel( const CEntityTypeModel &other, int index )
	: BaseClass(other, index)
{
	m_model = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeModel::OnCreate( void )
{
	loadModel();
	BaseClass::OnCreate();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeModel::OnDestroy( void )
{
	destroyModel();
	BaseClass::OnDestroy();
}	


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeModel::OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval )
{
	BaseClass::OnParamChanged(key, oldval, newval);

	if( key.compare("modelname") == 0 && oldval.compare(newval) != 0 )
		loadModel();
	
	if( m_model )
	{
		m_model->SetAbsAngles(GetAbsAngles());
		m_model->SetAbsPos(GetAbsCenter());
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeModel::loadModel()
{		
	destroyModel();

	std::string modelname = GetValue("modelname");
	if( modelname.length() > 0) 
	{
		m_model = new CModel(-1);
		m_model->Init(GetValue("modelname").c_str(), "GBufferOut", GetAbsCenter(), GetAbsAngles());
		g_RenderInterf->GetRenderer()->GetRenderList()->AddStaticEntity(m_model);
		SetSpriteTexture("");
	}
	else
		SetSpriteTexture("icon_light");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeModel::destroyModel()
{
	if( m_model != NULL )
	{
		g_RenderInterf->GetRenderer()->GetRenderList()->DeleteStaticEntity(m_model, true);
		m_model = NULL;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CEntityTypeModel::TraceLine( tracelineinfo_t &info )
{
	AABoundingBox aabb = m_model->GetBoundingBoxRaw();
	aabb.Rotate(m_model->GetAbsAngles());
	aabb.Translate(m_model->GetAbsPos());

	return RayAABBIntersect(info.start, info.dir, aabb.min, aabb.max, info.distHit);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeModel::Render( void )
{
	if( IsSelected() )
	{
		GetShaderManager()->GetActiveShader()->EnableTexturing(false);
		set_draw_color( 1.0f, 1.0f, 0.0f, 1.0f );
		AABoundingBox aabb = m_model->GetBoundingBoxRaw().GetRotated(m_model->GetAbsAngles());
		Vector3f center = (aabb.min + aabb.max)*0.5f + m_model->GetAbsPos();
		Vector3f size = aabb.max - aabb.min;
		draw_line_cube(center, size);
		set_draw_color( 1.0f, 1.0f, 1.0f, 1.0f );
	}
}
