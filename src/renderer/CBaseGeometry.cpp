// renderer/CBaseGeometry.cpp
// renderer/CBaseGeometry.cpp
// renderer/CBaseGeometry.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "glheaders.h"
#include "CBaseGeometry.h"
#include <util/instream.h>
#include "TextureManager.h"
#include <util/rpm_file.h>
#include "CShaders.h"
#include <util/debug.h>
#include <util/error.h>
#include "RendererRPM.h"
#include "CVertexBuffer.h"
#include "CVertexIndexBuffer.h"
#include "CRenderInterf.h"
#include <util/basicfuncs.h>
#include "CLoadingMutex.h"
#include "renderer.h"
#include <util/CConVar.h>
#include "CViewFrustum.h"
#include "CRenderWorld.h"

CConVar render_cull_models( "render.cull_models", "1" );
CConVar render_probe_lights( "render.probe_lights", "1");

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CBaseGeometry::CBaseGeometry( int index ) : 
	CRenderEntity(index),
	m_skyVisInterp(1.0f/60.0f, 10, 0.05f, 0.5f)
{
	m_pShader = NULL;
	m_vScale.Init( 1.0f, 1.0f, 1.0f );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CBaseGeometry::~CBaseGeometry()
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CBaseGeometry::LoadGeometry( const char *filename, const char *shadername )
{
	if( !GetGeometryManager()->PrecacheModel( &m_ModelInfo, filename ) )
	{
		if( LoadGeometryFromFile( filename, shadername ) )
		{
			GetGeometryManager()->AddModelToList( m_ModelInfo );
			return true;
		}
		else
		{
			error( "Could not load model:\n\"%s\"", filename );
			return false;
		}
	}
	else
	{
		m_pShader = GetShaderManager()->GetShader( shadername );
		strcpy( m_pchShaderName, shadername );
	}
	
	return true;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CBaseGeometry::LoadGeometryFromFile( const char *filename, const char *shadername )
{
	//Load Shader!
	strcpy( m_pchShaderName, shadername );
	m_pShader = GetShaderManager()->GetShader( shadername );

	int *pMaterialIndexes = NULL;
	int iMaterialIndexes = -1;

	int iTris = 0;
	rendertri_t *pTris = LoadRPMFileRenderer( filename, iTris, &pMaterialIndexes, &iMaterialIndexes, &m_ModelInfo.rpm );

	m_ModelInfo.rpm.clearHeap();

	if( !pTris )
		return false;

	m_ModelInfo.filename = filename;

	//NOTE: need to measure how long this takes.
	sort_tris_by_tex_index( pTris, 0, iTris-1 );

	if( iMaterialIndexes < 0 || iMaterialIndexes > MAX_MATERIALS_PER_MODEL )
	{
		if( pMaterialIndexes )
			delete[] pMaterialIndexes;
		if( pTris )
			delete[] pTris;

		error(	"Model \"%s\" refers %i Materials but the limit is %i Materials per Model.\n" \
				"You maybe want to adjust MAX_MATERIALS_PER_MODEL inside \"%s\".",
				filename, iMaterialIndexes, MAX_MATERIALS_PER_MODEL, MAX_MATERIALS_PER_MODEL_FILE );
		return false;
	}

	for( int i = 0; i < iMaterialIndexes; i++ )
		m_ModelInfo.pTexIndexes[i] = pMaterialIndexes[i];
	delete[] pMaterialIndexes;


	//Make a display List! (deprecated)
	//m_ModelInfo.iVBOIndex = create_displaylist( pTris, iTris, iTangAttribLoc, false, false );
	g_LoadingMutex->SetOrWait();

	m_ModelInfo.pVBO = new CVertexBuffer();
	m_ModelInfo.pVBO->Init();
	m_ModelInfo.pVBO->AddTrianglesToList( pTris, iTris );
	m_ModelInfo.pVBO->SubmitTriangleList();

	int lk = 0;
	int k = 0;
	for( int i = 0; i < iMaterialIndexes; i++ )
	{
		//find next changing texture index
		int iLastTexture = pTris[lk].iTexture;
		while( k+1 < iTris && pTris[k+1].iTexture == iLastTexture ) //next texture equal to current texture?
			k++;

		m_ModelInfo.pIBO[i] = new CVertexIndexBuffer<USHORT>();
		m_ModelInfo.pIBO[i]->Init();
		m_ModelInfo.pIBO[i]->AddIndexRangeToList( lk*3, k*3+2 );
		m_ModelInfo.pIBO[i]->SubmitIndexList();

		k++;
		lk = k;
	}

	g_LoadingMutex->Release();

	m_ModelInfo.iNumOfTexIndexes = iMaterialIndexes;

	delete[] pTris;

	return true;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseGeometry::DrawGeometry( const CRenderParams &params )
{	
	if( render_cull_models.GetBool() )
	{
		AABoundingBox aabb = this->GetBoundingBoxRaw();
		aabb.Scale( m_vScale );
		aabb.Rotate( GetAbsAngles() );
		aabb.Translate( GetAbsPos() );
		CViewFrustum *pFrustum = g_RenderInterf->GetRenderer()->GetActiveFrustum();
		if( !pFrustum->AxisAlignedCubeInFrustum( aabb.min, aabb.max ) )
			return;
	}

	UpdateSkyVisibility();

	trans_rot_model_start( GetAbsPos(), GetAbsAngles() );
	if( m_vScale != Vector3f(1.0f, 1.0f, 1.0f) )
		GetMVStack().Scale( m_vScale.x, m_vScale.y, m_vScale.z );

	GetShaderManager()->GetActiveShader()->UpdateMatrices();

	m_ModelInfo.pVBO->Bind();

	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars( VBF_RENDERVERT );

	for( int i = 0; i < m_ModelInfo.iNumOfTexIndexes; i++ )
	{
		int matIndex = m_ModelInfo.pTexIndexes[i];
		bool bDisableCulling = false;
		if( matIndex != -1 )
		{
			g_RenderInterf->GetTextureManager()->BindMaterial(matIndex);
			bDisableCulling = g_RenderInterf->GetTextureManager()->GetMaterialProps(matIndex).bDoubleSided;
		}

		if( bDisableCulling )
			GetGLStateSaver()->Disable(GL_CULL_FACE);

		m_ModelInfo.pIBO[i]->Bind();
		m_ModelInfo.pIBO[i]->Render();

		if( bDisableCulling )
			GetGLStateSaver()->Enable(GL_CULL_FACE);
	}

	rglPopClientAttrib();
	m_ModelInfo.pVBO->UnBind();
	m_ModelInfo.pIBO[0]->UnBind();

	trans_rot_model_end();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseGeometry::VOnRestore( void )
{
	//since Vertex and Index-Buffer objects are not managed
	//in a single place we have to reload them here

	//TODO: this is not correct yet.

	LoadGeometry( this->m_ModelInfo.filename.c_str(), m_pchShaderName );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseGeometry::VRender( const CRenderParams &params )
{
	if( params.renderpass == RENDERPASS_NORMAL_TRANSLUCENT )
		return;

	DrawGeometry(params);
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CBaseGeometry::VPreDelete( void )
{

}


//---------------------------------------------------------------
// Purpose: used for the 2d rendering of the editor
//---------------------------------------------------------------
void CBaseGeometry::RenderNoTextureBufferOnly( bool bindWhiteTexture )
{
	if( bindWhiteTexture )
		glBindTexture(GL_TEXTURE_2D, Debug::g_iWhiteTexture);

	trans_rot_model_start( GetAbsPos(), GetAbsAngles() );
	if( m_vScale != Vector3f(1.0f, 1.0f, 1.0f) )
		GetMVStack().Scale(m_vScale.x, m_vScale.y, m_vScale.z);
	GetShaderManager()->GetActiveShader()->UpdateMatrices();

	m_ModelInfo.pVBO->Bind();

	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars( VBF_RENDERVERT );

	int num = m_ModelInfo.iNumOfTexIndexes;
	for( int i = 0; i < num; i++ )
	{
		m_ModelInfo.pIBO[i]->Bind();
		m_ModelInfo.pIBO[i]->Render();
		//m_ModelInfo.pIBO[i]->UnBind();
	}
	m_ModelInfo.pIBO[num-1]->UnBind(); //Unbind last used index buffer

	rglPopClientAttrib();
	m_ModelInfo.pVBO->UnBind();

	trans_rot_model_end();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CBaseGeometry::UpdateSkyVisibility()
{
	float skyVis = 0.5f;
	if( render_probe_lights.GetBool() )
	{
		CRenderer *renderer = g_RenderInterf->GetRenderer();
		if(renderer)
		{
			CRenderWorld *world = renderer->GetWorld();
			if(world)
				skyVis = world->GetLightprobeSkyVis(GetAbsPos());
		}
	}
	m_skyVisInterp.SetNew(skyVis);
	float interpSkyVis = m_skyVisInterp.Update(gpGlobals->curtime);
	GetShaderManager()->GetActiveShader()->SetSkyVisFactor(interpSkyVis);
}
