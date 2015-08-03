// renderer/TextureManager.cpp
// renderer/TextureManager.cpp
// renderer/TextureManager.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "glheaders.h"
#include "TextureManager.h"
#include "TGA.h"
#include "DDS.h"
//#include "main.h"
#include <util/error.h>
#include <util/string_t.h>
#include <util/CScriptParser.h>
#include <util/instream.h>
#include "CLoadingMutex.h"
#include "CShaders.h"
#include "renderer.h"
#include "CRenderInterf.h"
#include "convars.h"
#include <util/StringUtils.h>

DECLARE_CONSOLE_COMMAND(materialprops_reload, "materialprops.reload")
{
	g_RenderInterf->GetTextureManager()->ParseMaterialGroups();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CTextureManager::CTextureManager()
{
	m_iUsedTextureMem = 0;
	m_pMaterialGroups = NULL;
}


CTextureManager::~CTextureManager()
{
	for( unsigned int i = 0; i < m_vTextures.size(); i++ )
	{
		GLuint tex = (GLuint)m_vTextures[i].index;
		glDeleteTextures( 1, &tex );
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CTextureManager::Init( void )
{
	SetTextureFiltering( GL_LINEAR_MIPMAP_LINEAR );
	//SetTextureFiltering( GL_LINEAR );
	ParseMaterialGroups();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CTextureManager::ParseMaterialGroups( void )
{
	if( m_pMaterialGroups )
		delete m_pMaterialGroups;
	m_pMaterialGroups = new CScriptParser();
	if( !m_pMaterialGroups->ParseFile("scripts/materialgroups.txt") )
		error("Could not open material-group file \"scripts/materialgroups.txt\"");

	for( size_t i = 0; i < m_vMaterials.size(); i++ )
	{
		CScriptParser parser;
		string filename = "textures/";
		filename += m_vMaterials[i].name;
		filename += ".rps";
		if( parser.ParseFile(filename.c_str()) )
			ParseMaterialProps(&parser, m_vMaterials[i]);
		else
			ConsoleMessage("ParseMaterialGroups(): couldnt find rps file for %s", m_vMaterials[i].name.c_str());
	}
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CTextureManager::IsMaterialTranslucent( int index )
{
	//Assert( index >= 0 && index < (int)m_vMaterials.size() );
	if( index < 0 || index >= (int)m_vMaterials.size() )
		return false;

	return m_vMaterials[index].props.bTranslucent;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CTextureManager::BindMaterial( int index )
{
	if( index < 0 )
		return;

	material_t *mat = &m_vMaterials[index];
	CShader *pShader = GetShaderManager()->GetActiveShader();

	bool transparencyShadows = render_shadows_transparent_materials.GetBool();
	int currentRenderpass = g_RenderInterf->GetRenderer()->GetActiveRenderpass();
	if( currentRenderpass == RENDERPASS_SHADOW )
	{
		if( !transparencyShadows )
			return;

		if( !mat->props.bTranslucent )
		{
			materialprops_t props = mat->props;
			props.bTranslucent = false;
			pShader->SetMaterialAttributes( props );
			return;
		}
	}

	GetGLStateSaver()->ActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, mat->diffuseTex.index );

	bool bNormalMap = mat->normalMap.index != 0;

	//Set the shader's translucency variable
	pShader->SetMaterialAttributes( mat->props );
	pShader->SetNormalMapEnabled( bNormalMap );

	if( bNormalMap && currentRenderpass != RENDERPASS_SHADOW )
	{
		GetGLStateSaver()->ActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, mat->normalMap.index );
		GetGLStateSaver()->ActiveTexture( GL_TEXTURE0 );
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CTextureManager::CreateDefaultMaterialFile( const char *name )
{
	string filePath( "textures/" );
	filePath += name;
	filePath += ".rps";

	size_t relPathLength = filePath.find_last_of("/\\");
	string fileName(filePath.begin()+relPathLength+1, filePath.end()-4);
	string fileNameNormal(fileName);
	fileName += ".dds";
	fileNameNormal += "_normal.dds";

	std::ofstream os( filePath );
	if( !os.is_open() )
		return;

	os << "\n\"diffuse\"	\"" << fileName.c_str() << "\"\n";
	os << "//\"normal\"	\"" << fileNameNormal.c_str() << "\"\n";
	os << "\"materialgroup\" \"default\"\n";
	os << "\"translucent\"	\"0\"\n";

	os.close();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CTextureManager::ParseMaterialProps(CScriptParser *parser, material_t &mat)
{
	string materialgroup, specular, smoothness;
	bool bMatGroup = parser->GetString( "materialgroup", materialgroup );
	bool bSpecular = parser->GetString( "specular", specular );
	bool bSmoothness = parser->GetString( "smoothness", smoothness );

	CScriptSubGroup *pSubGroup = NULL;
	if( bMatGroup )
	{
		pSubGroup = m_pMaterialGroups->GetSubGroup(materialgroup.c_str());
		if( !pSubGroup )
			ConsoleMessage("Unknown material group: \"%s\"", materialgroup.c_str());
	}

	if( bSpecular )
		mat.props.specular[3] = StrUtils::GetFloat(specular);
	else if( pSubGroup )
		mat.props.specular[3] = pSubGroup->GetFloat("specular", 0.5f);
	else
		mat.props.specular[3] = 0.0f;
	
	if( bSmoothness )
		mat.props.smoothness = StrUtils::GetFloat(smoothness);
	else if( pSubGroup )
		mat.props.smoothness = pSubGroup->GetFloat("smoothness", 0.5f);
	else
		mat.props.smoothness = 0.5f;

	mat.props.bTranslucent = parser->GetInt("translucent", 0) == 1;
	mat.props.bDoubleSided = parser->GetInt("doublesided", 0) == 1;
	mat.props.bShadowsOnly = parser->GetInt("shadowsonly", 0) == 1;
	parser->GetString("decoration", mat.props.decoration);
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
int CTextureManager::LoadMaterial( const char *name )
{
	Assert( name != NULL && name[0] != '\0' );

	//Does the material already exist?
	for( unsigned int i = 0; i < m_vMaterials.size(); i++ )
	{
		if( m_vMaterials[i].name.compare(name) == 0 )
			return i;
	}

	string fileName("textures/");
	fileName += name;
	fileName += ".rps";

	CScriptParser parser;
	if( !parser.ParseFile( fileName.c_str() ) )
	{
		ConsoleMessage( "Material not found: \"%s\", creating default material file.", name );
		CreateDefaultMaterialFile( name );
		return -1;
	}

	string diffuseTex;
	string normalMap;
	if( !parser.GetString( "diffuse", diffuseTex ) )
	{
		ConsoleMessage( "No diffuse texture in \"%s\".", name );
		return -1;
	}
	bool bUseNormal = parser.GetString( "normal", normalMap );

	size_t relPathLength = fileName.find_last_of("/\\");
	string relativePath(fileName.begin(), fileName.begin()+relPathLength+1);

	material_t mat;
	mat.name = name;
	mat.diffuseTex.filename = relativePath;
	mat.diffuseTex.filename += diffuseTex;
	mat.diffuseTex.srgb = parser.GetInt("srgb", 0) != 0;
	LoadTexture( mat.diffuseTex.filename.c_str(), mat.diffuseTex, false, true, false, mat.diffuseTex.srgb );

	ParseMaterialProps(&parser, mat);

	if( bUseNormal )
	{
		mat.normalMap.filename = relativePath;
		mat.normalMap.filename += normalMap;
		//mat.normalMap.srgb = false;
		LoadTexture( mat.normalMap.filename.c_str(), mat.normalMap );
	}
	else
		mat.normalMap.index = 0;

	m_vMaterials.push_back( mat );
	return m_vMaterials.size()-1;
}



//-----------------------------------------------------------------------
// Purpose: Caller guarantees that pFilename's length is >= 5
//-----------------------------------------------------------------------
bool CTextureManager::LoadImageFromFile( const char *pFilename, int index, texture_t &tex, int textureTarget /*= GL_TEXTURE_2D*/, bool flipY /*= false*/, bool srgb /*= false */ )
{
	bool bLoaded = false;
	int pFnLength = strlen( pFilename );
	
	if( strcmp( &pFilename[pFnLength-4], ".tga" ) == 0 )
	{
		bLoaded = PrecacheTgaImage( pFilename, index, tex, textureTarget, flipY, srgb );
	}
	else if( strcmp( &pFilename[pFnLength-4], ".dds" ) == 0 )
	{
		bLoaded = LoadDDSFromFile( pFilename, index, tex, textureTarget, flipY, srgb );
	}
	else
	{
		ConsoleMessage(	"Could not load texture \"%s\" due to an unsupported file format.\n"\
				"Supported file formats are .tga and .dds", pFilename );
		return false;
	}

	if( !bLoaded )
	{
		ConsoleMessage( "Could not load texture \"%s\". File not found.", pFilename );
		return bLoaded;
	}

	return bLoaded;
}



//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CTextureManager::LoadTexture( const char *pFilename, texture_t &tex,
								  bool bForceLoadFromDisk /*= false*/, bool bPutInTextureList /*= true*/, bool flipY /*= false*/, bool srgb /*= false */ )
{
	int iFnLength = strlen( pFilename );
	if( iFnLength < 5 )
	{
		ConsoleMessage(	"Could not load a texture because its filename was too short.\n"
						"Minimum is 5 (e.g. \"a.dds\")." );
		tex.index = 0;
		return false;
	}

	//Look first if Texture does not already exist, but only if we are not forced to load from disk
	if( !bForceLoadFromDisk )
	{
		for( unsigned int i = 0; i < m_vTextures.size(); i++ )
		{
			if( m_vTextures[i].filename.compare(pFilename) == 0 && m_vTextures[i].srgb == srgb )
			{
				tex = m_vTextures[i];
				return true;
			}
		}
	}
    
	//The texture does not already exist, so create a new one!
	GLuint texindex;
	g_LoadingMutex->SetOrWait();
	glGenTextures( 1, &texindex );
	g_LoadingMutex->Release();
	tex.srgb = srgb;
	
	if( LoadImageFromFile( pFilename, (int)texindex, tex, GL_TEXTURE_2D, flipY, srgb ) )
	{
		//This is currently only true when we reload all textures since we reload them only
		//by index, their references actually stay inside m_vTextures.
		if( bPutInTextureList )
			m_vTextures.push_back( tex );
		return true;
	}
	else
	{
		tex.index = 0;
		return false;
	}
}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CTextureManager::GetTexShininess( int iTexID, float *pShine4 )
{
	pShine4[0] = 0.1f;
	pShine4[1] = 0.1f;
	pShine4[2] = 0.1f;
	pShine4[3] = 1.0f;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CTextureManager::GetTexSpecular( int iTexID, float *pSpec4 )
{
	pSpec4[0] = 0.5f;
	pSpec4[1] = 0.5f;
	pSpec4[2] = 0.5f;
	pSpec4[3] = 1.0f;
}



//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
const materialprops_t &CTextureManager::GetMaterialProps( int materialId )
{
	return m_vMaterials[materialId].props;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CTextureManager::DeleteTexture( const texture_t &tex )
{
	//Find the entry in the texture-list!
	for( unsigned int i = 0; i < m_vTextures.size(); i++ )
	{
		if( m_vTextures[i].index == tex.index )
		{
			unsigned int index = tex.index;
			glDeleteTextures( 1, &index );
			m_vTextures.erase( m_vTextures.begin()+i );
		}
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CTextureManager::ReloadAllTextures( void )
{
	for( unsigned int i = 0; i < m_vTextures.size(); i++ )
	{
		unsigned int index = m_vTextures[i].index;
		glDeleteTextures( 1, &index );

		//force re-load from disk, and dont put the loaded textures inside the texture list
		//since they are already in there.
		LoadTexture( m_vTextures[i].filename.c_str(), m_vTextures[i], true, false ); 

		//update material list
		for( unsigned int k = 0; k < m_vMaterials.size(); k++ )
		{
			if( m_vMaterials[k].diffuseTex.filename.compare(m_vTextures[i].filename) == 0 )
				m_vMaterials[k].diffuseTex = m_vTextures[i];
			if( m_vMaterials[k].normalMap.index != 0 && 
					m_vMaterials[k].normalMap.filename.compare(m_vTextures[i].filename) == 0 )
				m_vMaterials[k].normalMap = m_vTextures[i];
		}
	}
}

