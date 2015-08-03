// renderer/CRenderWorld.cpp
// renderer/CRenderWorld.cpp
// renderer/CRenderWorld.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include <util/timer.h>
#include "CRenderWorld.h"
#include <util/instream.h>
#include "TextureManager.h"
#include "glheaders.h"
#include <util/error.h>
#include "CShaders.h"
#include "RendererRPM.h"
#include "scenetree.h"
#include <util/basic_types.h>
#include "CVertexBuffer.h"
#include "CVertexIndexBuffer.h"
#include "CRenderInterf.h"
#include "renderer.h"
#include <util/basicfuncs.h>
#include "CLoadingMutex.h"
#include <util/debug.h>
#include "CTerrainDecorator.h"
#include "CRenderInterf.h"
#include "CCamera.h"
#include <util/CConVar.h>
#include <util/CLightProbe.h>
#include <util/CSkyVisibilityCalculator.h>

#define SHADER_NAME "GBufferOut"
CConVar render_decoration_distance("render.decoration.distance", "500.0");

CRenderWorld::CRenderWorld() : CRenderEntity(0) //NOTENOTE: this should be changed
{
	m_iTextures = 0;
	m_pMaterials = NULL;
	pTreeRoot = NULL;
}

CRenderWorld::~CRenderWorld()
{
	Cleanup();
}


//TODO: Write
void CRenderWorld::VPreDelete( void )
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderWorld::VRender( const CRenderParams &params )
{
	if( !pTreeRoot )
		return;

	CShader *pShader = GetShaderManager()->GetActiveShader();
	pShader->SetSkyVisFactor(1.0f);

	//RENDERPASS NORMAL
	if( params.renderpass == RENDERPASS_NORMAL || params.renderpass == RENDERPASS_WATER_REFL )
	{
		pTreeRoot->UpdateVisibility();

		pShader->UpdateMatrices();
		

		m_pVertexBuffer->Bind();

		rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
		GetShaderManager()->InitVertexbufferVars( VBF_RENDERVERT );
		CTextureManager *texMgr = g_RenderInterf->GetTextureManager();

		//Render non-translucent things
		for( UINT i = 0; i < m_vTexIndexes.size(); i++ )
		{
			if( m_vTexIndexes[i] != -1 && !texMgr->GetMaterialProps(m_vTexIndexes[i]).bShadowsOnly )
			{
				texMgr->BindMaterial(m_vTexIndexes[i]);
				RenderSingleTexIndex(m_vTexIndexes[i]);
			}
		}

		rglPopClientAttrib();
		m_pVertexBuffer->UnBind();
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); //unbind the last used index buffer
	}
	//RENDERPASS SHADOW
	else if( params.renderpass == RENDERPASS_SHADOW )
	{
		pShader->UpdateMatrices();
		materialprops_t props;
		props.bTranslucent = false;
		props.bDoubleSided = false;
		pShader->SetMaterialAttributes(props);

		m_pVertexBuffer->Bind();

		rglPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
		GetShaderManager()->InitVertexbufferVars( VBF_RENDERVERT );

		pTreeRoot->UpdateVisibility();
		RenderSingleTexIndex(0);

		RenderTranslucentThings();

		rglPopClientAttrib();
		m_pVertexBuffer->UnBind();
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); //unbind the last used index buffer
	}

	//RENDERPASS NORMAL TRANSLUCENT
	else if( params.renderpass == RENDERPASS_NORMAL_TRANSLUCENT )
	{
		pShader->UpdateMatrices();

		m_pVertexBuffer->Bind();

		rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
		GetShaderManager()->InitVertexbufferVars( VBF_RENDERVERT );

		RenderTranslucentThings();

		rglPopClientAttrib();
		m_pVertexBuffer->UnBind();
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); //unbind the last used index buffer

		if( m_terrainDecorator )
			m_terrainDecorator->VRender(params,
				g_RenderInterf->GetRenderer()->GetActiveCam()->GetAbsPos(),
				render_decoration_distance.GetFloat());
	}	
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderWorld::Cleanup( void )
{
	//nothing to clean?
	if( !pTreeRoot )
		return; 

	//delete the old tree structure:
	//this should call all destructors in a recursive manner.
	delete pTreeRoot;
	pTreeRoot = NULL;

	//delete the vertex buffer
	delete m_pVertexBuffer;
	m_pVertexBuffer = NULL;

	//delete the old triangle array.
	delete m_pTris;
	m_pTris = NULL;

	m_vTexIndexes.clear();

	for( UINT i = 0; i < m_TranslucentIndexBuffers.size(); i++ )
	{
		delete m_TranslucentIndexBuffers[i]->pIndexBuffer;
		delete m_TranslucentIndexBuffers[i];
	}
	m_TranslucentIndexBuffers.clear();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderWorld::VOnRestore( void )
{
	Cleanup();

	//reload the world...
	LoadWorld( m_pchFilename );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderWorld::LoadWorld( const char *pFileName )
{
	//copy filename
	strcpy_s( m_pchFilename, pFileName );

	rpm_file_t rpm;
	m_pTris = LoadRPMFileRenderer( pFileName, m_iTris, NULL, NULL, &rpm );
	if( m_pTris == NULL )
	{
		error( "Could not load world!\n\"%s\"", pFileName );
	}
	else
	{
		LoadWorld(rpm);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderWorld::LoadWorld( const CScriptParser &worldFile )
{
	CScriptSubGroup *worldgroup = worldFile.GetSubGroup("WORLD");
	if( !worldgroup ) 
	{
		error("Renderer: Could not find subgroup \"WORLD\" in world-script.");
		return;
	}

	CScriptSubGroup *keyvalsGroup = worldgroup->GetSubGroup("keyvals");
	if( !keyvalsGroup )
	{
		error("Renderer: Could not find subgroup \"keyvals\" in Group \"WORLD\"");
		return;
	}

	std::string worldModel;
	if( !keyvalsGroup->GetString("filename", worldModel) )
	{
		error("Renderer: Could not find keyval \"filename\" in subgroup \"keyvals\" in subgroup \"WORLD\"");
		return;
	}

	strcpy_s(m_pchFilename, worldModel.c_str());

	rpm_file_t rpm;
	m_pTris = LoadRPMFileRenderer( m_pchFilename, m_iTris, NULL, NULL, &rpm );
	if( m_pTris == NULL )
	{
		error( "Could not load world!\n\"%s\"", m_pchFilename );
		return;
	}
	
	//parse lightprobes from mapfile
	std::vector<CLightProbe> probes;
	const SubGroupMap *entMap = worldFile.GetSubGroupMap();
	for(auto it = entMap->begin(); it != entMap->end(); ++it)
	{
		std::string classname;
		it->second->GetString("classname", classname);
		if(classname == "lightprobe")
		{
			CScriptSubGroup *kvgroup = it->second->GetSubGroup("keyvals");
			if(!kvgroup)
				continue;
			Vector3f origin = kvgroup->GetVector3f("origin");
			CLightProbe probe;
			probe.SetPosition(origin);
			probes.push_back(probe);
		}
	}

	if( LightProbesChanged(probes, rpm) )
	{
		ConsoleMessage("---------------------------------------------");
		ConsoleMessage("Lightprobes changed, calculating lightprobes");
		ConsoleMessage("---------------------------------------------");

		//transform triangles into Triangle_t format
		std::vector<Triangle> tris;
		for(int i = 0; i < rpm.getTriangleCount(); i++)
		{
			Triangle tri;
			memset(&tri, 0, sizeof(Triangle));
			for(int j = 0; j < 3; j++)
			{
				tri.vertex3[j].pos3.setFromOther(rpm.getTriangles()[i].vertex3[j].pos3);
				tri.vertex3[j].norm3.setFromOther(rpm.getTriangles()[i].vertex3[j].norm3);
			}
			tri.norm3.setFromOther(rpm.getTriangles()[i].norm3);
			tris.push_back(tri);
		}

		AABoundingBox_t<double> aabb(rpm.getBounds().min.toVec3d(), rpm.getBounds().max.toVec3d());
		CSkyVisibilityCalculator skyVisCalculator(tris, aabb);
		skyVisCalculator.calculateLightProbeSkyVis(probes);

		rpm_lump *lump = rpm.findLump(LUMP_LIGHTPROBES);
		if(!lump)
			lump = rpm.createLump(LUMP_LIGHTPROBES);
		lump->alloc(sizeof(CLightProbe) * probes.size());
		memcpy(lump->getData(), &probes[0], lump->getSize());
		std::ofstream os(m_pchFilename, std::ios::binary);
		if(os.is_open())
		{
			if(rpm.Serialize(os) != rpm_file_t::RPM_WRITE_OK)
				error("Error serializing rpm file \"%s\".", m_pchFilename);
			else
				ConsoleMessage("Successfully wrote Lightprobes to \"%s\"", m_pchFilename);
		}
		else
			error("Error opening file \"%s\" for output.", m_pchFilename);
		ConsoleMessage("Done.\n");
	}

	LoadWorld(rpm);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderWorld::LoadWorld(rpm_file_t &rpm)
{
	//Load lightprobes (if any)
	rpm_lump *lightProbeLump = rpm.findLump(LUMP_LIGHTPROBES);
	if(lightProbeLump)
	{
		size_t numProbes = lightProbeLump->getSize() / sizeof(CLightProbe);
		ConsoleMessage("Found %i LightProbes", (int)numProbes);
		m_lightProbes.resize(numProbes);
		memcpy(&m_lightProbes[0], lightProbeLump->getData(), lightProbeLump->getSize());
	}
	else
		m_lightProbes.clear();

	//sort the triangle array by texture indexes:
	sort_tris_by_tex_index( m_pTris, 0, m_iTris-1 );

	//determine indexes that are translucent and indexes that are not
	std::vector<int> texIndexesNormal;
	std::vector<int> texIndexesTranslucent;
	split_trianglelist_translucency( m_pTris, m_iTris, texIndexesNormal, texIndexesTranslucent );

	//build texindex-list for non-translucent triangles
	//this is easy since the tris are already sorted by texindex.
	for( UINT i = 0; i < texIndexesNormal.size(); i++ )
	{
		int index = texIndexesNormal[i];
		if( m_vTexIndexes.size() == 0 || m_vTexIndexes.back() != m_pTris[index].iTexture )
			m_vTexIndexes.push_back( m_pTris[index].iTexture );
	}

	g_LoadingMutex->SetOrWait();

	//create vertex buffer
	m_pVertexBuffer = new CVertexBuffer();
	m_pVertexBuffer->Init();
	m_pVertexBuffer->AddTrianglesToList( m_pTris, m_iTris );
	m_pVertexBuffer->SubmitTriangleList();

	//Store all translucent triangles into our special index-buffers
	StoreTranslucentTriangles( m_pTris, texIndexesTranslucent );

	g_LoadingMutex->Release();

	//find minimum and maximum vertex coords
	//NOTENOTE: this should be done inside bsptorpm
	m_boundingBox.min = m_boundingBox.max = m_pTris[0].pVerts[0].vPos;

	for( int i = 0; i < m_iTris; i++ )
	{
		for( int k = 0; k < 3; k++ )
		{
			Vector3f vVert = m_pTris[i].pVerts[k].vPos;
			for( int l = 0; l < 3; l++ )
			{
				if( vVert[l] < m_boundingBox.min[l] )
					m_boundingBox.min[l] = vVert[l];
				else if( vVert[l] > m_boundingBox.max[l] )
					m_boundingBox.max[l] = vVert[l];
			}
		}
	}

	//build first cube here...
	Vector3f vMinMaxDist = m_boundingBox.max - m_boundingBox.min;
	Vector3f vCenter = m_boundingBox.min + vMinMaxDist * 0.5f;
	float fCubeLengthHalf = max3f( vMinMaxDist.x, vMinMaxDist.y, vMinMaxDist.z ) * 0.5f;
	Vector3f vCubeMin = vCenter - fCubeLengthHalf;
	Vector3f vCubeMax = vCenter + fCubeLengthHalf;

	this->pTreeRoot = new CSTreeCube;
	pTreeRoot->m_vCubeBounds[0] = vCubeMin;
	pTreeRoot->m_vCubeBounds[1] = vCubeMax;

	//store only the non-translucent triangles in the root.
	pTreeRoot->m_iTris = texIndexesNormal.size();
	pTreeRoot->m_pTris = new int[pTreeRoot->m_iTris];
	memcpy( pTreeRoot->m_pTris, &texIndexesNormal.front(), sizeof(int)*pTreeRoot->m_iTris );

	pTreeRoot->m_pAllTris = m_pTris;
	pTreeRoot->m_iAllTris = m_iTris;

	g_pTimer->UpdateTime();
	float time = gpGlobals->curtime;

	if( m_iTris > MAX_TRIANGLES_PER_CUBE )
		pTreeRoot->SplitCube();
	else
		pTreeRoot->CreateIndexBuffers();

	g_pTimer->UpdateTime();
	time = gpGlobals->curtime - time;

	ConsoleMessage( "Splitting the World into an Octree took us %i ms", (int)(time*1000.0f));

	//Init terrain decorator
	m_terrainDecorator = new CTerrainDecorator();
	m_terrainDecorator->PreprocessTerrain(this);
}


//Note: This function assumes that the triangles are sorted by texture-index.
void CRenderWorld::StoreTranslucentTriangles( rendertri_t *pTris, const std::vector<int> &transTriIndexes )
{
	int iLastTexIndex = -1;

	for( UINT i = 0; i < transTriIndexes.size(); i++ )
	{
		int iTriIndex = transTriIndexes[i];
		int iTexIndex = pTris[iTriIndex].iTexture;

		//new texture-index?
		if( iTexIndex != iLastTexIndex )
		{
			//create a new index buffer for the new texture index
			translucency_indexbuffer_t *pNewBuf = new translucency_indexbuffer_t;
			pNewBuf->iTexIndex = iTexIndex;
			pNewBuf->pIndexBuffer = new CVertexIndexBuffer<USHORT>();
			pNewBuf->pIndexBuffer->Init();
			m_TranslucentIndexBuffers.push_back( pNewBuf );
		}

		//add the current triangle to our index-buffer
		m_TranslucentIndexBuffers.back()->pIndexBuffer->AddTriangleToList(iTriIndex);

		iLastTexIndex = iTexIndex;
	}

	//now all needed triangles where added to the index-buffers,
	//but we need to submit them to the graphics card:
	for( UINT i = 0; i < m_TranslucentIndexBuffers.size(); i++ )
		m_TranslucentIndexBuffers[i]->pIndexBuffer->SubmitIndexList();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderWorld::RenderNoTextureBufferOnly( void )
{
	//RENDERPASS NORMAL
	//pTreeRoot->UpdateVisibility();
	glBindTexture(GL_TEXTURE_2D, Debug::g_iWhiteTexture);

	GetShaderManager()->GetActiveShader()->UpdateMatrices();

	m_pVertexBuffer->Bind();

	rglPushClientAttrib( GL_CLIENT_VERTEX_ARRAY_BIT );
	GetShaderManager()->InitVertexbufferVars( VBF_RENDERVERT );

	//Render non-translucent things
	for( UINT i = 0; i < m_vTexIndexes.size(); i++ )
	{
		//if( m_vTexIndexes[i] != -1 )
		//	g_RenderInterf->GetTextureManager()->BindMaterial( m_vTexIndexes[i] );
		//pTreeRoot->Render( 0 );
	}

	rglPopClientAttrib();
	m_pVertexBuffer->UnBind();
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ); //unbind the last used index buffer
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderWorld::RenderTranslucentThings()
{
	//Render translucent things
	for( UINT i = 0; i < m_TranslucentIndexBuffers.size(); i++ )
	{
		int iTexIndex = m_TranslucentIndexBuffers[i]->iTexIndex;
		if( iTexIndex != -1 )
		{
			g_RenderInterf->GetTextureManager()->BindMaterial( iTexIndex );
			m_TranslucentIndexBuffers[i]->pIndexBuffer->Bind();
			GetGLStateSaver()->EnableClientState(GL_INDEX_ARRAY);
			m_TranslucentIndexBuffers[i]->pIndexBuffer->Render();
		}
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CRenderWorld::RenderSingleTexIndex( int texIndex )
{
	std::vector<USHORT> indexes;
#ifdef USE_DRAW_CALL_REDUCTION
	size_t reserved = (size_t)(3 * pTreeRoot->GetNumVisibleTris(texIndex));
	indexes.reserve( reserved );
#endif

	pTreeRoot->Render(texIndex, indexes );

#ifdef USE_DRAW_CALL_REDUCTION
	Assert(reserved == indexes.size());
	if( indexes.size() > 0 )
		glDrawElements( GL_TRIANGLES, indexes.size(), GL_UNSIGNED_SHORT, &indexes[0] );	
#endif

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CRenderWorld::GetLightprobeSkyVis( const Vector3f &pos )
{
	//find the probe clostest to pos
	float minDist = FLOAT_MAX;
	size_t minIndex = -1;

	for(size_t i = 0; i < m_lightProbes.size(); i++)
	{
		float dist = m_lightProbes[i].GetPosition().distToSq(pos);
		if(dist < minDist)
		{
			minDist = dist;
			minIndex = i;
		}
	}

	if(minIndex != size_t(-1))
	{
		return m_lightProbes[minIndex].GetLight();
	}
	else
	{
		return 0.5f;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CRenderWorld::LightProbesChanged( const std::vector<CLightProbe> &parsedProbes, const rpm_file_t &rpmfile )
{
	rpm_lump *lump = rpmfile.findLump(LUMP_LIGHTPROBES);
	Assert(lump->getSize() % sizeof(CLightProbe) == 0);
	size_t numRpmProbes = lump ? lump->getSize() / sizeof(CLightProbe) : 0;

	if(parsedProbes.size() == 0 && numRpmProbes == 0) //no probes at all?
		return false;
	if(parsedProbes.size() != numRpmProbes) //different probe count?
		return true;

	//when we get here we got the same count of lumps parsed an in the rpm, check
	//if they differ in positions
	CLightProbe *rpmProbes = (CLightProbe*)lump->getData();
	for(size_t i = 0; i < numRpmProbes; i++)
		if(parsedProbes[i].GetPosition().distToSq(rpmProbes[i].GetPosition()) > (0.01f*0.01f))
			return true;
	return false;
}

