// editor/CToolTranslateEffector.cpp
// editor/CToolTranslateEffector.cpp
// editor/CToolTranslateEffector.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "CToolTranslateEffector.h"

#include <CModel.h>
#include "editorDoc.h"
#include "CEditorEntity.h"

#define ARROW_MODEL_FILENAME "models\\editor\\editorarrow.rpm"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolTranslateEffector::CToolTranslateEffector( const Vector3f &color ) : 
	CToolEffector(color),
	m_position(vec3_null)
{
	m_arrowModel = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolTranslateEffector::~CToolTranslateEffector()
{
	if( m_arrowModel )
		delete m_arrowModel;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolTranslateEffector::Init( void )
{
	m_arrowModel = new CModel(-1);
	m_arrowModel->Init( ARROW_MODEL_FILENAME, "Unlit", m_position, GetLocalRotation() );

	AABoundingBox arrowBox = m_arrowModel->GetBoundingBoxRaw();
	m_modelSize = arrowBox.max - arrowBox.min;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolTranslateEffector::Render( void )
{
	CShader *pShd = GetShaderManager()->GetActiveShader();

	float r = m_baseColor.x;
	float g = m_baseColor.y;
	float b = m_baseColor.z;
	float alphagreater = m_selected ? 0.15f : 0.075f;
	float alphaless = m_selected ? 1.0f : 0.6f;

	if( b > 0.5f ) //boost blue a bit
	{
		alphaless *= 1.5f;
		alphagreater *= 1.5f;
	}

	GetGLStateSaver()->DepthFunc(GL_GREATER);
	pShd->SetDrawColor(r, g, b, clamp(alphagreater, 0.0f, 1.0f));
	m_arrowModel->RenderNoTextureBufferOnly(false);

	GetGLStateSaver()->DepthFunc(GL_LESS);
	pShd->SetDrawColor(r, g, b, clamp(alphaless, 0.0f, 1.0f));
	m_arrowModel->RenderNoTextureBufferOnly(false);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CToolTranslateEffector::TraceLine( tracelineinfo_t &info )
{
	float length = m_modelSize.x * m_scale;
	float width = m_modelSize.y * m_scale * 0.3f;

	//AABoundingBox aabb;
	//aabb.min.Init( 0, -width, -width );
	//aabb.max.Init( length, width, width );

	//aabb.Rotate(GetLocalRotation());
	//aabb.Translate(GetPosition());

	//return RayAABBIntersect( info.start, info.dir, aabb.min, aabb.max, info.distHit, info.distMax );

	CRay axisRay(m_position, m_axis);
	CRay traceRay(info.start, info.dir);
	
	Vector3f hitPoint;
	float d = axisRay.distToRay(traceRay, hitPoint, Vector3f());

	if( d > width || m_position.distTo(hitPoint) > length || (m_position+m_axis*length).distTo(hitPoint) > length )
		return false;
	
	info.distHit = info.start.distTo(hitPoint);
	return info.distHit <= info.distMax;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolTranslateEffector::SetPosition( const Vector3f &pos )
{
	m_position = pos;
	if( m_arrowModel )
		m_arrowModel->SetAbsPos( pos );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolTranslateEffector::UpdateSize( float cameraDistance, float camFov )
{
	float fTan = TAN(camFov*0.5f);
	m_scale = fTan * cameraDistance * 0.035f;
	if( m_arrowModel )
		m_arrowModel->SetScale( m_scale );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const Vector3f & CToolTranslateEffector::GetPosition( void )
{
	return m_position;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolTranslateEffector::DragStart( const CRay &clickRay, bool active )
{
	if( !active )
		return;

	CeditorDoc *pDoc = GetActiveDocument();
	m_startPos = m_position;
	m_entityStartPos.clear();
	for( size_t i = 0; i < pDoc->GetSelectedEntityList().size(); i++ )
		m_entityStartPos.push_back(pDoc->GetSelectedEntity(i)->GetAbsCenter());
	
	CRay axisRay(m_position, m_axis);
	axisRay.distToRay(clickRay, m_clickStart, Vector3f() );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolTranslateEffector::DragUpdate( const CRay &clickRay, bool active )
{
	if( !active )
		return;

	CRay axisRay(m_position, m_axis);
	Vector3f newDrag;
	axisRay.distToRay(clickRay, newDrag, Vector3f());
	Vector3f relDrag = newDrag - m_clickStart;

	CeditorDoc *pDoc = GetActiveDocument();
	for( size_t i = 0; i < pDoc->GetSelectedEntityList().size(); i++ )
	{
		CEditorEntity *pEnt = pDoc->GetSelectedEntity(i);
		if( pEnt->IsTranslatable() )
			pEnt->SetAbsCenter(m_entityStartPos[i] + relDrag);
	}

	SetPosition(m_startPos + relDrag);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolTranslateEffector::DragEnd(bool active)
{
	m_entityStartPos.clear();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Quaternion CToolTranslateEffector::GetLocalRotation()
{
	//Get rotation relative to the x-Axis
	Vector3f xAxis(1.0f, 0.0f, 0.0f);

	if( m_axis == xAxis )
		return Quaternion();

	Vector3f rotAxis = xAxis.Cross(m_axis).Normalize();
	float rotAngle = ACOS(m_axis.Dot(xAxis));

	return Quaternion(rotAxis, rotAngle);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolTranslateEffector::SetAxis( const Vector3f &axis )
{
	CToolEffector::SetAxis(axis);
	
	if( m_arrowModel )
		m_arrowModel->SetAbsAngles(GetLocalRotation());
}
