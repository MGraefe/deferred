// editor/CEntityTypeBlock.cpp
// editor/CEntityTypeBlock.cpp
// editor/CEntityTypeBlock.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "IEditorEntityFactory.h"
#include "CEntityTypeBlock.h"
#include <CRenderInterf.h>
#include <TextureManager.h>
#include <CShaders.h>
#include <CVertexBuffer.h>
#include <CVertexIndexBuffer.h>
#include <util/StringUtils.h>

DECLARE_EDITOR_ENTITY_TYPE("BLOCK", CEntityTypeBlock)

#define KEY_WIDTH "width"
#define KEY_HEIGHT "height"
#define KEY_DEPTH "depth"
#define KEY_TEX_SCALE "textureScale"
#define KEY_MATERIAL "material"


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityTypeBlock::CEntityTypeBlock( const CScriptSubGroup *pGrp, int index )
	: BaseClass(pGrp, index)
{
	m_vertBuffer = NULL;
	m_vertIndexBuffer = NULL;
	updateInternals();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityTypeBlock::CEntityTypeBlock( const std::string &classname, int index )
	: BaseClass(classname, index)
{
	m_vertBuffer = NULL;
	m_vertIndexBuffer = NULL;
	updateInternals();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityTypeBlock::CEntityTypeBlock( const CEntityTypeBlock &other, int index ) :
	BaseClass(other, index)
{
	m_vertBuffer = NULL;
	m_vertIndexBuffer = NULL;
	updateInternals();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeBlock::updateInternals()
{
	m_texScale = StrUtils::GetFloat(GetValue(KEY_TEX_SCALE));
	m_size.x = StrUtils::GetFloat(GetValue("width"));
	m_size.y = StrUtils::GetFloat(GetValue("height"));
	m_size.z = StrUtils::GetFloat(GetValue("depth"));
	for(int i = 0; i < 3; i++)
		if( m_size[i] == 0.0f )
			m_size[i] = 0.01f;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeBlock::OnCreate( void )
{
	updateInternals();
	loadMaterial();
	loadVertices();
	resize();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeBlock::OnDestroy( void )
{
	destroyVertices();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeBlock::OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval )
{
	BaseClass::OnParamChanged(key, oldval, newval);

	if( key == KEY_TEX_SCALE || key == KEY_WIDTH || key == KEY_HEIGHT || key == KEY_DEPTH )
	{
		updateInternals();
		resize();
	}
	else if(key == KEY_MATERIAL)
		loadMaterial();
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeBlock::resize()
{
	sizeVertices(m_size);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CEntityTypeBlock::TraceLine( tracelineinfo_t &info )
{
	AABoundingBox aabb = getAABB();
	return RayAABBIntersect(info.start, info.dir, aabb.min, aabb.max, info.distHit);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeBlock::Render( void )
{
	set_draw_color(1.0f, 1.0f, 1.0f, 1.0f);
	trans_rot_model_start(GetAbsCenter(), GetAbsAngles());
	GetShaderManager()->GetActiveShader()->UpdateMatrices();
	GetShaderManager()->GetActiveShader()->EnableTexturing(true);
	g_RenderInterf->GetTextureManager()->BindMaterial(m_materialId);
	m_vertBuffer->Bind();
	m_vertIndexBuffer->Bind();
	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars(VBF_RENDERVERT);
	m_vertIndexBuffer->Render();
	m_vertIndexBuffer->UnBind();
	m_vertBuffer->UnBind();
	rglPopClientAttrib();

	if(IsSelected())
	{
		GetShaderManager()->GetActiveShader()->EnableTexturing(false);
		set_draw_color(1.0f, 1.0f, 0.0f, 1.0f);
		draw_line_cube(vec3_null, m_size);
		set_draw_color(1.0f, 1.0f, 1.0f, 1.0f);
	}
	trans_rot_model_end();
}



const float s = 0.5f;
const Vector3f nx(1.0f, 0.0f, 0.0f);
const Vector3f tx(0.0f, 0.0f, -1.0f);
const Vector3f ny(0.0f, 1.0f, 0.0f);
const Vector3f ty(-1.0f, 0.0f, 0.0f);
const Vector3f nz(0.0f, 0.0f, 1.0f);
const Vector3f tz(1.0f, 0.0f, 0.0f);

const rendervert_t g_BlockVerts[] = {

	//bottom
	rendervert_t(Vector3f(-s, -s, -s), -ny, -ty, Vector2f(0,0), 1.0f), //0
	rendervert_t(Vector3f(-s, -s,  s), -ny, -ty, Vector2f(0,1), 1.0f), //1
	rendervert_t(Vector3f( s, -s, -s), -ny, -ty, Vector2f(1,0), 1.0f), //2
	rendervert_t(Vector3f( s, -s,  s), -ny, -ty, Vector2f(1,1), 1.0f), //3

	//top
	rendervert_t(Vector3f(-s,  s, -s),  ny,  ty, Vector2f(0,0), 1.0f), //4
	rendervert_t(Vector3f(-s,  s,  s),  ny,  ty, Vector2f(0,1), 1.0f), //5
	rendervert_t(Vector3f( s,  s, -s),  ny,  ty, Vector2f(1,0), 1.0f), //6
	rendervert_t(Vector3f( s,  s,  s),  ny,  ty, Vector2f(1,1), 1.0f), //7

	//back
	rendervert_t(Vector3f(-s, -s, -s), -nz, -tz, Vector2f(0,0), 1.0f), //8
	rendervert_t(Vector3f(-s,  s, -s), -nz, -tz, Vector2f(0,1), 1.0f), //9
	rendervert_t(Vector3f( s, -s, -s), -nz, -tz, Vector2f(1,0), 1.0f), //10
	rendervert_t(Vector3f( s,  s, -s), -nz, -tz, Vector2f(1,1), 1.0f), //11

	//front
	rendervert_t(Vector3f(-s, -s,  s),  nz,  tz, Vector2f(0,0), 1.0f), //12
	rendervert_t(Vector3f(-s,  s,  s),  nz,  tz, Vector2f(0,1), 1.0f), //13
	rendervert_t(Vector3f( s, -s,  s),  nz,  tz, Vector2f(1,0), 1.0f), //14
	rendervert_t(Vector3f( s,  s,  s),  nz,  tz, Vector2f(1,1), 1.0f), //15

	//left
	rendervert_t(Vector3f(-s, -s, -s), -nx, -tx, Vector2f(0,0), 1.0f), //16
	rendervert_t(Vector3f(-s, -s,  s), -nx, -tx, Vector2f(0,1), 1.0f), //17
	rendervert_t(Vector3f(-s,  s, -s), -nx, -tx, Vector2f(1,0), 1.0f), //18
	rendervert_t(Vector3f(-s,  s,  s), -nx, -tx, Vector2f(1,1), 1.0f), //19

	//right
	rendervert_t(Vector3f( s, -s, -s),  nx,  tx, Vector2f(0,0), 1.0f), //20
	rendervert_t(Vector3f( s, -s,  s),  nx,  tx, Vector2f(0,1), 1.0f), //21
	rendervert_t(Vector3f( s,  s, -s),  nx,  tx, Vector2f(1,0), 1.0f), //22
	rendervert_t(Vector3f( s,  s,  s),  nx,  tx, Vector2f(1,1), 1.0f), //23
};


const USHORT g_BlockIndices[] = {
	0,  2,  3,  0,  3,  1,	//bottom
	4,  5,  7,  4,  7,  6,	//top
	8,  9,  11, 8,  11, 10,	//back
	12, 14, 15, 12, 15, 13, //front
	16, 17, 19, 16, 19, 18, //left
	20, 22, 23, 20, 23, 21	//right
};


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeBlock::loadVertices()
{
	float s = 0.5f;
	Vector3f n(1.0f, 0.0f, 0.0f);
	Vector3f t(0.0f, 1.0f, 0.0f);

	m_vertBuffer = new CVertexBuffer(CVertexBuffer::BT_DYNAMIC_DRAW);
	m_vertBuffer->Init();
	m_vertBuffer->SetVertexes(g_BlockVerts, ARRAY_LENGTH(g_BlockVerts));

	m_vertIndexBuffer = new CVertexIndexBuffer<USHORT>();
	m_vertIndexBuffer->Init();
	m_vertIndexBuffer->SetVertexIndexes(g_BlockIndices, ARRAY_LENGTH(g_BlockIndices));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeBlock::destroyVertices()
{
	delete m_vertBuffer;
	m_vertBuffer = NULL;
	delete m_vertIndexBuffer;
	m_vertIndexBuffer = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeBlock::sizeVertices( const Vector3f &size )
{
	Assert(m_vertBuffer);
	Assert(m_vertBuffer->GetVertexCountSubmitted() == ARRAY_LENGTH(g_BlockVerts));

	rendervert_t *buf = m_vertBuffer->StartModification();
	for(int i = 0; i < ARRAY_LENGTH(g_BlockVerts); i++)
		buf[i].vPos = g_BlockVerts[i].vPos * size;
	calcUvs(buf, ARRAY_LENGTH(g_BlockVerts));
	m_vertBuffer->EndModification();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeBlock::calcUvs( rendervert_t * buf, int numVerts )
{
	for(int i = 0; i < numVerts; i++)
	{
		Vector3f tx = buf[i].vTang;
		Vector3f ty = buf[i].vNorm.Cross(buf[i].vTang);
		buf[i].pUV.x = buf[i].vPos.Dot(tx) * m_texScale;
		buf[i].pUV.y = buf[i].vPos.Dot(ty) * m_texScale;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
AABoundingBox CEntityTypeBlock::getAABB()
{
	AABoundingBox aabb(m_size * -0.5f, m_size * 0.5f);
	aabb.Rotate(GetAbsAngles());
	aabb.Translate(GetAbsCenter());
	return aabb;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeBlock::loadMaterial()
{
	m_materialId = g_RenderInterf->GetTextureManager()->LoadMaterial(GetValue(KEY_MATERIAL).c_str());
}




//static Vector2d g_BlockUvs[] = {
//	Vector2d(0,0), Vector2d(0,1), Vector2d(1,0), Vector2d(1,1),
//	Vector2d(0,0), Vector2d(0,1), Vector2d(1,0), Vector2d(1,1),
//	Vector2d(0,0), Vector2d(0,1), Vector2d(1,0), Vector2d(1,1),
//	Vector2d(0,0), Vector2d(0,1), Vector2d(1,0), Vector2d(1,1),
//	Vector2d(0,0), Vector2d(0,1), Vector2d(1,0), Vector2d(1,1),
//	Vector2d(0,0), Vector2d(0,1), Vector2d(1,0), Vector2d(1,1)
//};
//
//float s = 0.5f;
//static Vector3f g_BlockVerts[] = {
//	//bottom
//	Vector3f(-s, -s, -s),
//	Vector3f(-s, -s,  s),
//	Vector3f( s, -s, -s),
//	Vector3f( s, -s,  s),
//
//	//top
//	Vector3f(-s,  s, -s),
//	Vector3f(-s,  s,  s),
//	Vector3f( s,  s, -s),
//	Vector3f( s,  s,  s),
//
//	//front
//	Vector3f(-s, -s,  s),
//	Vector3f(-s,  s,  s),
//	Vector3f( s, -s,  s),
//	Vector3f( s,  s,  s),
//
//	//back
//	Vector3f(-s, -s, -s),
//	Vector3f(-s,  s, -s),
//	Vector3f( s, -s, -s),
//	Vector3f( s,  s, -s),
//
//	//left
//	Vector3f(-s, -s, -s),
//	Vector3f(-s, -s,  s),
//	Vector3f(-s,  s, -s),
//	Vector3f(-s,  s,  s),
//
//	//right
//	Vector3f( s, -s, -s),
//	Vector3f( s, -s,  s),
//	Vector3f( s,  s, -s),
//	Vector3f( s,  s,  s),
//};


