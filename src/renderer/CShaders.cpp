// renderer/CShaders.cpp
// renderer/CShaders.cpp
// renderer/CShaders.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include <stdio.h>
#include "CShaders.h"
#include "CCamera.h"
#include <util/instream.h>
#include "CRenderInterf.h"
#include "renderer.h"
#include <util/StringUtils.h>
#include <util/CConVar.h>

CConVar render_shaders_debuglevel("render.shaders.debuglevel", "0");

#define SHADER_ATTRIB_NAME_VERTEX	"iv_Vertex"
#define SHADER_ATTRIB_NAME_UVCOORD	"iv_UvCoord"
#define SHADER_ATTRIB_NAME_NORMAL	"iv_Normal"
#define SHADER_ATTRIB_NAME_TANGENT	"iv_Tangent"
#define SHADER_ATTRIB_NAME_SKY_VISIBILITY "if_SkyVisibility"
#define SHADER_UNIFORM_NAME_MODELVIEW_MATRIX	"im_ModelViewMatrix"
#define SHADER_UNIFORM_NAME_NORMAL_MATRIX		"im_NormalMatrix"
#define SHADER_UNIFORM_NAME_PROJ_MATRIX			"im_ProjectionMatrix"
#define SHADER_UNIFORM_NAME_MODELVPROJ_MATRIX	"im_ModelViewProjectionMatrix"


void reload_all_shaders(const CConCommand &sender, const string &params) 
{
	if( !g_RenderInterf )
		return;

	CShaderManager *pMgr = g_RenderInterf->GetShaderManager();

	if( !pMgr )
		return;

	pMgr->ReloadShaders();
}

CConCommand cmd_reload_all_shaders( "render.shaders.reload", &reload_all_shaders );


CShaderManager *GetShaderManager(void)
{
	//return &ShaderManager;
	return g_RenderInterf->GetShaderManager();
}


ShaderFactoryMap &GetShaderFactoryMap(void)
{
	static ShaderFactoryMap factMap;
	return factMap;
}

IShaderFactory *GetShaderFactory( const char *name )
{
	ShaderFactoryMap fm = GetShaderFactoryMap();
	ShaderFactoryMap::iterator it = fm.find( std::string(name) );
	if( it == fm.end() )
	{
		error( "Could not find factory for shader \"%s\".", name );
		return NULL;
	}

	return it->second;	
}


bool textFileRead( const char *filename, std::string &out )
{
	fs::ifstream is( filename, std::ios::binary );
	if( !is.is_open() )
		return false;
	out = fs::readTextUnixLineEndingString(is);
	return true;
}


bool resolveShaderIncludes( std::string &shd, const std::string &path, int includeCount = 0 )
{
	//Cancel recursive includes
	if( includeCount > 1000 )
		return false;

	//First get rid of all /*...*/ comments
	size_t commentStart;
	while( (commentStart = shd.find("/*")) != shd.npos )
	{
		size_t commentEnd = shd.find( "*/", commentStart+2 );
		if( commentEnd == shd.npos )
			return false;
		shd.erase(commentStart, commentEnd+2-commentStart);
	}

	//Then get rid of //commentaries
	while( (commentStart = shd.find("//")) != shd.npos )
	{
		size_t commentEnd = shd.find_first_of( "\n\0", commentStart+2 );
		if( commentEnd == shd.npos )
			return false;
		shd.erase( commentStart, commentEnd-commentStart );
	}

	//Then find #include statements and replace them
	size_t includeStart;
	while( (includeStart = shd.find("#include")) != shd.npos )
	{
		size_t fnStart = shd.find( '"', includeStart );
		if( fnStart == shd.npos )
			return false;

		size_t fnStop = shd.find( '"', fnStart+1 );
		if( fnStop == shd.npos )
			return false;

		std::string fn( shd, fnStart+1, fnStop-fnStart-1 ); //the filename
		std::string fp( path+fn ); //the filepath

		//Read text file
		std::string incFile;
		if( !textFileRead(fp.c_str(), incFile) )
		{
			error( "Could not find shader include file \"%s\"", fp.c_str() );
			shd.erase( includeStart, fnStop-includeStart+1 );
			continue;
		}

		if( !resolveShaderIncludes( incFile, StrUtils::getPathFromFilePath(fp,true), includeCount+1 ) )
		{
			error( "Error parsing shader file \"%s\"", fp.c_str() );
			return false;
		}
		shd.replace( includeStart, fnStop-includeStart+1, incFile );
	}

	//Replace all {(...)} defines with their console variable equivalents
	size_t repStart;
	while( (repStart = shd.find("{(")) != shd.npos )
	{
		size_t repEnd = shd.find(")}", repStart);
		if(repEnd == shd.npos)
			return false;

		std::string varName(shd.begin() + repStart + 2, shd.begin() + repEnd);
		std::string varValue;
		CConVar *var = CConVarList::FindConVar(varName);
		if(var)
			varValue = var->GetString();
		else
			error("console variable \"%s\" is referenced in a shader but not existent", varName.c_str());
		
		shd.replace(shd.begin() + repStart, shd.begin() + repEnd + 2, varValue);
	}

	return true;
}


void ReadShaderFile( const char *filename, std::string &shd )
{
	if( !textFileRead(filename, shd) )
	{
		shd.clear();
		error("File not found or cannot open: \"%s\"", filename);
		return;
	}
	std::string path = StrUtils::getPathFromFilePath( filename, true );
	if( !resolveShaderIncludes( shd, path ) )
		error( "An error occured while reading shader file %s", filename );
}


const float g_gamma = 2.2f;

Vector3f gammaCorrect(const Vector3f &v)
{
	return Vector3f(pow(v.x, g_gamma), pow(v.y, g_gamma), pow(v.z, g_gamma));
}

Vector4f gammaCorrect(const Vector4f &v)
{
	return Vector4f(pow(v.x, g_gamma), pow(v.y, g_gamma), pow(v.z, g_gamma), v.w);
}


//#######################################################################
// CShader
//#######################################################################


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CShader::CShader()
{
	bIsActive = false;

	m_iLocModelviewMatrix = -1;
	m_iLocProjectionMatrix = -1;
	m_iLocAttribPos = -1;
	m_iLocAttribNormal = -1;
	m_iLocAttribUvCoord = -1;
	m_iLocAttribTangent = -1;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CShader::~CShader()
{
	DeleteShader();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CShader::SetReverseNormals( bool bReverse )
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CShader::CheckForError( int prog, const char *pFilename, const std::string &shaderSource )
{
	int infologLength = 0;
	int charsWritten  = 0;

	int iCompileStatus = 0;
	glGetShaderiv( prog, GL_COMPILE_STATUS, &iCompileStatus );

#ifndef _DEBUG
	if( iCompileStatus == GL_TRUE )
		return;
#endif

	glGetShaderiv( prog, GL_INFO_LOG_LENGTH, &infologLength);

	std::vector<char> infoLog;
	if (infologLength > 1)
	{
		infoLog.resize(infologLength);
		glGetShaderInfoLog( prog, infologLength, &charsWritten, infoLog.data() );
	}

	if( iCompileStatus != GL_TRUE )
	{
		std::string dumpFile = std::string(pFilename) + ".dumped";
		std::string message = std::string(infoLog.data()) + "\n\nWould you like to dump the preprocessed shader file to \"" + dumpFile + "\"?";
		if( MessageBox( NULL, message.c_str(), pFilename, MB_YESNO ) == IDYES )
		{
			std::ofstream os(dumpFile.c_str());
			if(!os.is_open())
				MessageBox( NULL, "Error dumping shader file.", "Error", MB_OK);
			else
			{
				os << shaderSource;
				os.close();
			}
		}
	}
	else if(infologLength > 1)
		ConsoleMessage( "Shader '%s', File '%s', Compile Message: %s",
			m_name.c_str(), pFilename, infoLog );
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CShader::DeleteShader( void )
{
	glDetachShader( m_iProgram, m_iVertexShader );
	glDetachShader( m_iProgram, m_iFragmentShader );
	glDeleteShader( m_iVertexShader );
	glDeleteShader( m_iFragmentShader );
	glDeleteProgram( m_iProgram );
	m_iProgram = 0;
	m_iVertexShader = 0;
	m_iFragmentShader = 0;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
bool CShader::LoadShader( void )
{
	std::string vs, fs;
	ReadShaderFile( m_vertProgName.c_str(), vs );
	ReadShaderFile( m_fragProgName.c_str(), fs );

	if( vs.length() == 0 || fs.length() == 0 )
		return false;

	m_iVertexShader = glCreateShader(GL_VERTEX_SHADER);
	m_iFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);	

	const char *vv = vs.c_str();
	const char *ff = fs.c_str();

	glShaderSource( m_iVertexShader, 1, &vv, NULL );
	glCompileShader( m_iVertexShader );
	CheckForError( m_iVertexShader, m_vertProgName.c_str(), vs );

	glShaderSource( m_iFragmentShader, 1, &ff, NULL );
	glCompileShader( m_iFragmentShader );
	CheckForError( m_iFragmentShader, m_fragProgName.c_str(), fs );

	int statusVs = 0;
	int statusFs = 0;
	glGetShaderiv( m_iVertexShader, GL_COMPILE_STATUS, &statusVs );
	glGetShaderiv( m_iFragmentShader, GL_COMPILE_STATUS, &statusFs );

	if( statusVs != GL_TRUE || statusFs != GL_TRUE )
		return false;

	m_iProgram = glCreateProgram();

	glAttachShader( m_iProgram, m_iVertexShader );
	glAttachShader( m_iProgram, m_iFragmentShader );

	glBindAttribLocation( m_iProgram, 0, SHADER_ATTRIB_NAME_VERTEX );

	glLinkProgram( m_iProgram );
	glUseProgram( m_iProgram );

	SetReverseNormals(false);

	return true;
}



//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
//void CShader::ComputeTangentBasis(
//      const float *P1, const float *P2, const float *P3, 
//      const float *UV1, const float *UV2, const float *UV3,
//      float *tangent, float *bitangent )
//{
//	float Edge1[3];
//	Edge1[0] = P2[0] - P1[0];
//	Edge1[1] = P2[1] - P1[1];
//	Edge1[2] = P2[2] - P1[2];
//
//	float Edge2[3];
//	Edge2[0] = P3[0] - P1[0];
//	Edge2[1] = P3[1] - P1[1];
//	Edge2[2] = P3[2] - P1[2];
//
//	float Edge1uv[2];
//	Edge1uv[0] = UV2[0] - UV1[0];
//	Edge1uv[1] = UV2[1] - UV1[1];
//
//	float Edge2uv[2];
//	Edge2uv[0] = UV3[0] - UV1[0];
//	Edge2uv[1] = UV3[1] - UV1[1];
//
//	float cp = Edge1uv[1] * Edge2uv[0] - Edge1uv[0] * Edge2uv[1];
//
//	if ( cp != 0.0f )
//	{
//		float mul = 1.0f / cp;
//		tangent[0] = (Edge1[0] * -Edge2uv[1] + Edge2[0] * Edge1uv[1]) * mul;
//		tangent[1] = (Edge1[1] * -Edge2uv[1] + Edge2[1] * Edge1uv[1]) * mul;
//		tangent[2] = (Edge1[2] * -Edge2uv[1] + Edge2[2] * Edge1uv[1]) * mul;
//
//		bitangent[0] = (Edge1[0] * -Edge2uv[0] + Edge2[0] * Edge1uv[0]) * mul;
//		bitangent[1] = (Edge1[1] * -Edge2uv[0] + Edge2[1] * Edge1uv[0]) * mul;
//		bitangent[2] = (Edge1[2] * -Edge2uv[0] + Edge2[2] * Edge1uv[0]) * mul;
//
//		float tangentlength = sqrt( tangent[0]*tangent[0] + tangent[1]*tangent[1] + tangent[2]*tangent[2] );
//		tangent[0] /= tangentlength;
//		tangent[1] /= tangentlength;
//		tangent[2] /= tangentlength;
//
//		float bitangentlength = sqrt( bitangent[0]*bitangent[0] + bitangent[1]*bitangent[1] + bitangent[2]*bitangent[2] );
//		bitangent[0] /= bitangentlength;
//		bitangent[1] /= bitangentlength;
//		bitangent[2] /= bitangentlength;
//	}
//}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CShader::Enable( void )
{
	if( GetShaderManager()->GetActiveShader() != this )
	{
		GetShaderManager()->SetActiveShader( this );
		if( GetShaderManager()->GetShaderStatus() == true )
		{
			glUseProgram( m_iProgram );

			//Set these to changed, so they get uploaded next draw-call
			GetMVStack().SetChanged();
			GetProjStack().SetChanged();
		}
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CShader::Init( void )
{
	LoadUniformLoc( SHADER_UNIFORM_NAME_MODELVIEW_MATRIX, m_iLocModelviewMatrix );
	LoadUniformLoc( SHADER_UNIFORM_NAME_PROJ_MATRIX, m_iLocProjectionMatrix );
	LoadUniformLoc( SHADER_UNIFORM_NAME_NORMAL_MATRIX, m_iLocNormalMatrix );
	LoadUniformLoc( SHADER_UNIFORM_NAME_MODELVPROJ_MATRIX, m_iLocModelviewProjectionMatrix );

	if( GetNeededVars() & SHADER_VERTEX )
		LoadAttributeLoc( SHADER_ATTRIB_NAME_VERTEX, m_iLocAttribPos );

	if( GetNeededVars() & SHADER_NORMAL )
		LoadAttributeLoc( SHADER_ATTRIB_NAME_NORMAL, m_iLocAttribNormal );

	if( GetNeededVars() & SHADER_UVCOORD )
		LoadAttributeLoc( SHADER_ATTRIB_NAME_UVCOORD, m_iLocAttribUvCoord );

	if( GetNeededVars() & SHADER_TANGENT )
		LoadAttributeLoc( SHADER_ATTRIB_NAME_TANGENT, m_iLocAttribTangent );

	if( GetNeededVars() & SHADER_SKY_VISIBILITY )
		LoadAttributeLoc( SHADER_ATTRIB_NAME_SKY_VISIBILITY, m_iLocAttribSkyVisibility );
}



//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CShader::UpdateMatrices( bool forceUpdate )
{
	if( forceUpdate || GetMVStack().HasChanged() )
	{
		const float *mv = GetMVStack().GetMatrix();
		if( m_iLocModelviewMatrix != -1 )
			glUniformMatrix4fv( m_iLocModelviewMatrix, 1, GL_FALSE, mv );

		if( m_iLocNormalMatrix != -1 )
		{
			Matrix3 mNormalMat( mv[0], mv[1], mv[2],
								mv[4], mv[5], mv[6],
								mv[8], mv[9], mv[10] );
			mNormalMat = mNormalMat.GetInverse();
			mNormalMat.transpose();

			glUniformMatrix3fv( m_iLocNormalMatrix, 1, GL_FALSE, &mNormalMat[0][0] );
		}
	}

	if( forceUpdate || GetProjStack().HasChanged() )
		if( m_iLocProjectionMatrix != -1 )
			glUniformMatrix4fv( m_iLocProjectionMatrix, 1, GL_FALSE, GetProjStack().GetMatrix() );
		

	if( forceUpdate || GetProjStack().HasChanged() || GetMVStack().HasChanged() )
	{
		if( m_iLocModelviewProjectionMatrix != -1 )
		{
			Matrix4f mvpmat = GetProjStack().GetMatrixObj() * GetMVStack().GetMatrixObj();
			glUniformMatrix4fv( m_iLocModelviewProjectionMatrix, 1, GL_FALSE, mvpmat.Get() );
		}
	}

	GetMVStack().ResetChanged();
	GetProjStack().ResetChanged();
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
int CShader::GetAttribLoc( int attrib )
{
	switch( attrib )
	{
	case SHADER_VERTEX:
		return m_iLocAttribPos;
	case SHADER_NORMAL:
		return m_iLocAttribNormal;
	case SHADER_UVCOORD:
		return m_iLocAttribUvCoord;
	case SHADER_TANGENT:
		return m_iLocAttribTangent;
	case SHADER_SKY_VISIBILITY:
		return m_iLocAttribSkyVisibility;
	};

	error( "CShader::GetAttribLoc(): couldnt find attribute %i.", attrib );
	return -1;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CShader::SetDrawColor( float r, float g, float b, float a )
{
	throw std::exception();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShader::ReloadShader( void )
{
	DeleteShader();
	LoadShader();
	Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShader::SetFileNames( const std::string &vs_name, const std::string &ps_name )
{
	m_vertProgName = vs_name;
	m_fragProgName = ps_name;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShader::SetUniform1f( const char *name, float v )
{
	int loc = glGetUniformLocation(m_iProgram, name);
	if( loc != -1 )
		glUniform1f(loc, v);
	else if (render_shaders_debuglevel.GetBool())
		error( "Could not find attribute \"%s\" in shader %s", name, GetName());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShader::SetUniform2f( const char *name, float v1, float v2 )
{
	int loc = glGetUniformLocation(m_iProgram, name);
	if( loc != -1 )
		glUniform2f(loc, v1, v2);
	else if (render_shaders_debuglevel.GetBool())
		error( "Could not find attribute \"%s\" in shader %s", name, GetName());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CShader::LoadUniformLoc( const char *name )
{
	int loc = glGetUniformLocation(m_iProgram, name);
	if( loc == -1 && render_shaders_debuglevel.GetInt() > 0 ) \
		ConsoleMessage( "%s: Couldn't find shader uniform \"%s\".", GetName(), name ); \
	return loc;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CShader::LoadAttributeLoc( const char *name )
{
	int var = glGetAttribLocation( m_iProgram, name );
	if( var == -1 )
	{
		if(render_shaders_debuglevel.GetInt() > 0)
			ConsoleMessage( "%s: Couldn't find shader attribute \"%s\".", GetName(), name );
	}
	else if(render_shaders_debuglevel.GetInt() > 1)
		ConsoleMessage( "%s: Binding %s to attrib Location %i", GetName(), name, var );
	return var;
}



//#######################################################################
// CShaderManager
//#######################################################################

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CShaderManager::CShaderManager()
{
	m_ActiveShader = NULL;
	m_currentFormat = VBF_RENDERVERT;
	m_currentFormatSetShader = NULL;
	m_bShadersActive = true;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CShaderManager::~CShaderManager() 
{
	for( UINT i = 0; i < m_shaders.size(); i++ )
		delete m_shaders[i];
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CShaderManager::LoadAllShaders( void )
{
	//delete old shaders if they exist
	for( UINT i = 0; i < m_shaders.size(); i++ )
		delete m_shaders[i];

	m_shaders.clear();

	CScriptParser parser;
	if( !parser.ParseFile( "shaders/shaders.txt" ) )
		error_fatal( "Could not find shader definition file \"shaders/shaders.txt\"" );
	const SubGroupMap *subGroups = parser.GetSubGroupMap();

	for( SubGroupMap::const_iterator it = subGroups->begin(); it != subGroups->end(); it++ )
	{
		const string &name = it->first;
		string file_ps, file_vs;
		it->second->GetString( "PS", file_ps );
		it->second->GetString( "VS", file_vs );
		file_ps.insert( 0, "shaders/" );
		file_vs.insert( 0, "shaders/" );

		IShaderFactory *fact = GetShaderFactory( name.c_str() );
		if( fact )
		{
			CShader *shd = fact->Create();
			shd->SetName( name.c_str() );
			shd->SetFileNames( file_vs, file_ps );
			if( !shd->LoadShader() )
				error_msg("Error loading shader \"%s\", shader is invalid, cannot recover, expect crash",
					name.c_str());
			else
				m_shaders.push_back(shd);
		}
		else
			error("No CPP-Entry for Shader \"%s\"", name.c_str());
	}

	InitializeShaders();
}




//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CShaderManager::InitializeShaders( void )
{
	for( UINT i = 0; i < m_shaders.size(); i++ )
	{
		m_shaders[i]->Enable();
		m_shaders[i]->Init();
	}
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CShader *CShaderManager::GetShader( const char *pchName )
{
	for( UINT i = 0; i < m_shaders.size(); i++ )
	{
		if( strcmp( pchName, m_shaders[i]->GetName() ) == 0 )
			return m_shaders[i];
	}

	return NULL;
}


void CShaderManager::SetShaderStatus( bool bActive )
{
	m_bShadersActive = bActive;
	if( !m_ActiveShader )
		return;

	if( bActive )
	{
		glUseProgram( m_ActiveShader->m_iProgram );
	}
	else
	{
		glUseProgram(0);
	}
}


void CShaderManager::SetActiveShader( CShader *shd )
{
	m_ActiveShader = shd;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderManager::ReloadShaders( void )
{
	for( UINT i = 0; i < m_shaders.size(); i++ )
		m_shaders[i]->ReloadShader();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderManager::UnbindAllVertexbufferVars()
{
	for(int attrib : m_activeVertexbufferAttribs)
		glDisableVertexAttribArray(attrib);
	m_activeVertexbufferAttribs.clear();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderManager::InitVertexbufferVars(vertexbuffer_format_e format, const void* buffer /*= NULL*/)
{
	//if(format == m_currentFormat && m_ActiveShader == m_currentFormatSetShader && m_lastBuffer == buffer && buffer == NULL)
	//	return; //Nothing to do here *flies away*

	std::set<int> oldActiveVertexAttribs = m_activeVertexbufferAttribs;
	m_activeVertexbufferAttribs.clear();

	switch(format)
	{
	case VBF_RENDERVERT:
		init_vertexbuffer_vars_rendervert_t(buffer);
		break;
	case VBF_POS_NORMAL_UV:
		init_vertexbuffer_vars_pos_normal_uvcoord(buffer);
		break;
	case VBF_POS_UV:
		init_vertexbuffer_vars_pos_uvcoord(buffer);
		break;
	case VBF_UV_POS:
		init_vertexbuffer_vars_uvcoord_pos(buffer);
		break;
	case VBF_POS:
		init_vertexbuffer_vars_pos(buffer);
		break;
	default:
		error_fatal("Unknown vertex format, have a look into CShaderManager::InitVertexbufferVars()");
	}

	//Disable attributes that are in the old but not in the new attrib-set
	for(int attrib : oldActiveVertexAttribs)
		if(m_activeVertexbufferAttribs.find(attrib) == m_activeVertexbufferAttribs.end())
			glDisableVertexAttribArray(attrib);

	//Enable attributes that are in the new but not in the old attrib-set
	for(int attrib : m_activeVertexbufferAttribs)
		if(oldActiveVertexAttribs.find(attrib) == oldActiveVertexAttribs.end())
			glEnableVertexAttribArray(attrib);

	m_currentFormat = format;
	m_currentFormatSetShader = m_ActiveShader;
	m_lastBuffer = buffer;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderManager::SetVertexAttribPointer(UINT attrib, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{
	int attribLoc = m_ActiveShader->GetAttribLoc(attrib);
	glVertexAttribPointer(attribLoc, size, type, normalized, stride, pointer);
	m_activeVertexbufferAttribs.insert(attribLoc);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderManager::init_vertexbuffer_vars_rendervert_t( const void *buffer )
{
	UINT iNeededVars = m_ActiveShader->GetNeededVars();
	int iStructureSize = sizeof(rendervert_t);

	//SHADER_VERTEX
	if( iNeededVars & SHADER_VERTEX )
		SetVertexAttribPointer( SHADER_VERTEX, 3, GL_FLOAT, GL_FALSE, iStructureSize, ((char*)buffer) + sizeof(float)*0 );

	//SHADER_NORMAL
	if( iNeededVars & SHADER_NORMAL )
		SetVertexAttribPointer( SHADER_NORMAL, 3, GL_FLOAT, GL_FALSE, iStructureSize, ((char*)buffer) + sizeof(float)*3 );
	
	//SHADER_TANGENT
	if( iNeededVars & SHADER_TANGENT )
		SetVertexAttribPointer( SHADER_TANGENT, 3, GL_FLOAT, GL_FALSE, iStructureSize, ((char*)buffer) + sizeof(float)*6 );

	//SHADER_UVCOORD
	if( iNeededVars & SHADER_UVCOORD )
		SetVertexAttribPointer( SHADER_UVCOORD, 2, GL_FLOAT, GL_FALSE, iStructureSize, ((char*)buffer) + sizeof(float)*9 );

	//SHADER_SKY_VISIBILITY
	if( iNeededVars & SHADER_SKY_VISIBILITY )
		SetVertexAttribPointer( SHADER_SKY_VISIBILITY, 1, GL_FLOAT, GL_FALSE, iStructureSize, ((char*)buffer) + sizeof(float)*11 );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderManager::init_vertexbuffer_vars_pos_normal_uvcoord( const void *buffer )
{
	int iStructureSize = sizeof(float)*8;
	SetVertexAttribPointer( SHADER_VERTEX, 3, GL_FLOAT, GL_FALSE, iStructureSize, ((char*)buffer) + 0 );
	SetVertexAttribPointer( SHADER_NORMAL, 3, GL_FLOAT, GL_FALSE, iStructureSize, ((char*)buffer) + sizeof(float)*3 );
	SetVertexAttribPointer( SHADER_UVCOORD, 2, GL_FLOAT, GL_FALSE, iStructureSize, ((char*)buffer) + sizeof(float)*6 );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderManager::init_vertexbuffer_vars_pos_uvcoord( const void *buffer )
{
	int iStructureSize = sizeof(float)*5;
	SetVertexAttribPointer( SHADER_VERTEX, 3, GL_FLOAT, GL_FALSE, iStructureSize, ((char*)buffer) + 0 );
	SetVertexAttribPointer( SHADER_UVCOORD, 2, GL_FLOAT, GL_FALSE, iStructureSize, ((char*)buffer) + sizeof(float)*3 );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderManager::init_vertexbuffer_vars_uvcoord_pos( const void *buffer )
{
	int iStructureSize = sizeof(float)*5;
	SetVertexAttribPointer( SHADER_UVCOORD, 2, GL_FLOAT, GL_FALSE, iStructureSize, ((char*)buffer) + 0 );
	SetVertexAttribPointer( SHADER_VERTEX, 3, GL_FLOAT, GL_FALSE, iStructureSize, ((char*)buffer) + sizeof(float)*2 );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderManager::init_vertexbuffer_vars_pos( const void *buffer )
{
	SetVertexAttribPointer( SHADER_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, ((char*)buffer) + 0 );
}


//#######################################################################
// CShaderUnlit
//#######################################################################
DECLARE_SHADER( "Unlit", CShaderUnlit )

void CShaderUnlit::Init( void )
{
	m_iLocationTextureEnabled = glGetUniformLocation( m_iProgram, "bTexEnabled" );
	glUniform1i( m_iLocationTextureEnabled, 1 );

	m_iLocationDrawColor = glGetUniformLocation( m_iProgram, "cDrawColor" );
	glUniform4f( m_iLocationDrawColor, 1.0f, 1.0f, 1.0f, 1.0f );

	m_iLocationSingleChannelAlpha = glGetUniformLocation( m_iProgram, "bSingleChannelAlpha" );
	glUniform1i( m_iLocationSingleChannelAlpha, 0 );

	CShader::Init();
}

void CShaderUnlit::EnableTexturing( bool bEnabled )
{
	glUniform1i( m_iLocationTextureEnabled, bEnabled ? 1 : 0 );
}

void CShaderUnlit::SetDrawColor( float r, float g, float b, float a )
{
	glUniform4f( m_iLocationDrawColor, r, g, b, a );
}

void CShaderUnlit::SetSingleChannelAlpha( bool status )
{
	glUniform1i( m_iLocationSingleChannelAlpha, status ? 1 : 0);
}


//#######################################################################
// CShaderGBufferOut
//#######################################################################
DECLARE_SHADER( "GBufferOut", CShaderGBufferOut )

void CShaderGBufferOut::Init( void )
{
	glUniform1i( glGetUniformLocation( m_iProgram, "tex0" ), 0 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex1" ), 1 );

	LoadAttributeLoc("vTang", m_iTangAttribLoc);
	LoadUniformLoc( "bTranslucent",		m_iTranslucencyEnabledUniformLoc );
	LoadUniformLoc( "bNormalMap",		m_iNormalMapEnabledUniformLoc );
	LoadUniformLoc( "fDoubleSided",		m_iDoubleSided );
	LoadUniformLoc( "fSmoothness",		m_iSmoothness );
	LoadUniformLoc( "fSpecular",		m_iSpecular );
	LoadUniformLoc( "fSkyVisFactor",	m_iSkyVisFactor );
	CShader::Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderGBufferOut::SetNormalMapEnabled( bool bEnabled )
{
	glUniform1i( m_iNormalMapEnabledUniformLoc, bEnabled );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderGBufferOut::SetMaterialAttributes( const materialprops_t &props )
{
	glUniform1i( m_iTranslucencyEnabledUniformLoc, props.bTranslucent );

	//This is tightly coupled with the shader, 
	//the shader calculates normal += normal * float(!gl_FrontFacing) * fDoubleSided;
	glUniform1f( m_iDoubleSided, props.bDoubleSided ? -2.0f : 0.0f );

	glUniform1f( m_iSmoothness, props.smoothness );
	glUniform1f( m_iSpecular, props.specular[3] );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderGBufferOut::SetSkyVisFactor( float factor )
{
	glUniform1f( m_iSkyVisFactor, factor );
}



//#######################################################################
// CShaderDeferred (abstract)
//#######################################################################
void CShaderDeferred::Init( void )
{
	CRenderer *pRenderer = g_RenderInterf->GetRenderer();

	float screenWidth = (float)pRenderer->GetScreenWidth();
	float screenHeight = (float)pRenderer->GetScreenHeight();

	SetUniform2f("vScreenSize",		screenWidth, screenHeight);
	SetUniform2f("vInvScreenSize",	1.0f/screenWidth, 1.0f/screenHeight);
	SetUniform1f("fTanHalfFov",		TAN(75.0f*0.5f));
	SetUniform1f("fFarZ",			pRenderer->GetFarClippingDist());
	SetUniform1f("fNearZ",			pRenderer->GetNearClippingDist());

	CShader::Init();
}


//#######################################################################
// CShaderDeferredLighting
//#######################################################################
DECLARE_SHADER("DeferredLighting", CShaderDeferredLighting)

CConVar render_speculars("render.speculars", "1");

void CShaderDeferredLighting::Init( void )
{
	glUniform1i( glGetUniformLocation( m_iProgram, "tex0" ), 0 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex1" ), 1 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex2" ), 2 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex3" ), 3 );

	CConVar *var = CConVarList::GetConVar("render.shadows.mapsize");
	glUniform1f( glGetUniformLocation( m_iProgram, "shadowMapSizeR" ), 
		var ? 1.0f/var->GetFloat() : 1.0f/1024.0f);

	LoadUniformLoc( "vSunLightDir",			m_iSunLightDir );
	LoadUniformLoc( "vSunLightColor",		m_iSunLightColor );
	LoadUniformLoc( "mShadowSpaceMatrix",	m_iShadowSpaceMatLoc );
	LoadUniformLoc( "cAmbientMin",			m_iAmbientColor1 );
	LoadUniformLoc( "cAmbientMax",			m_iAmbientColor2 );
	LoadUniformLoc( "vSunLightSpecularColor", m_iSunLightSpecularColor );
	CShaderDeferred::Init();
}


void CShaderDeferredLighting::UpdateLightingData( const CSunLight &light,
	const Matrix3 &mViewSpace,
	const Matrix4f &mViewSpace4x4,
	const Matrix4f *mShadowSpaces,
	int iShadowCascades )
{
	Vector3f vSunDir = mViewSpace * (-light.GetDirection());

	Vector4f c = gammaCorrect(light.GetColor());
	Vector3f a1 = gammaCorrect(light.GetAmbient1());
	Vector3f a2 = gammaCorrect(light.GetAmbient2());
	Vector4f s = gammaCorrect(light.GetSpecular());

	//Upload sun-data
	glUniform3f( m_iSunLightDir, vSunDir.x, vSunDir.y, vSunDir.z );
	glUniform4f( m_iSunLightColor, c.x, c.y, c.z, c.w );
	glUniform3f( m_iAmbientColor1, a1.x, a1.y, a1.z );
	glUniform3f( m_iAmbientColor2, a2.x, a2.y, a2.z );
	glUniform4f( m_iSunLightSpecularColor, s.x, s.y, s.z, s.w );

	//Upload matrices to shader
	glUniformMatrix4fv( m_iShadowSpaceMatLoc, iShadowCascades, GL_FALSE, mShadowSpaces[0].Get() );
	//glUniformMatrix4fv( m_iViewSpaceMat, 1, GL_FALSE, mViewSpace4x4.Get() );
}



//#######################################################################
// CShaderShadowMap
//#######################################################################
DECLARE_SHADER( "ShadowMap", CShaderShadowMap )

void CShaderShadowMap::SetMaterialAttributes( const materialprops_t &props )
{
	if( m_iTransparencyEnabledLoc != -1 )
		glUniform1i( m_iTransparencyEnabledLoc, props.bTranslucent );
}

void CShaderShadowMap::Init( void )
{
	m_iTransparencyEnabledLoc = glGetUniformLocation( m_iProgram, "bTranslucent" );
	CShader::Init();
}

//#######################################################################
// CShaderPointLight
//#######################################################################
DECLARE_SHADER( "DeferredPoint", CShaderPointLight );

void CShaderPointLight::Init( void )
{
	glUniform1i( glGetUniformLocation( m_iProgram, "tex0" ), 0 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex1" ), 1 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex2" ), 2 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex3" ), 3 );

	LoadUniformLoc( "vPointLightPos", m_iPointLightPos );
	LoadUniformLoc( "vPointLightColor", m_iPointLightColor );
	LoadUniformLoc( "vPointLightAttent", m_iPointLightAttent );

	CShaderDeferred::Init();
}


void CShaderPointLight::SetLightData( const CPointLight &light, const Matrix3 &mViewSpace, const Vector3f &mViewTransl )
{
	Vector3f pos = light.GetAbsPos();
	pos = mViewSpace * (pos - mViewTransl);

	Vector4f c = gammaCorrect(light.GetColor());

	glUniform3f( m_iPointLightPos, pos.x, pos.y, pos.z );
	glUniform4f( m_iPointLightColor, c.x, c.y, c.z, c.w );
	glUniform4f( m_iPointLightAttent, light.GetConstant(), light.GetLinear(), light.GetQuadratic(), light.GetDist() );
}


//#######################################################################
// CShaderWater
//#######################################################################
DECLARE_SHADER( "Water", CShaderWater )

void CShaderWater::Init( void )
{
	glUniform1i( glGetUniformLocation( m_iProgram, "tex0" ), 0 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex1" ), 1 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex2" ), 2 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex3" ), 3 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex4" ), 4 );

	CShaderDeferred::Init();
}



//#######################################################################
// CShaderWaterPretty
//#######################################################################

DECLARE_SHADER("WaterPretty", CShaderWaterPretty)

void CShaderWaterPretty::Init( void )
{
	glUniform1i( glGetUniformLocation( m_iProgram, "tex0" ), 0 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex1" ), 1 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex2" ), 2 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex3" ), 3 );
	LoadUniformLoc("texFactor",		m_iTexFactor);
	LoadUniformLoc("sunDir",		m_iSunDir);
	LoadUniformLoc("meshPosScale",	m_iMeshPosScale);
	LoadUniformLoc("camPos",		m_iCamPos);
	LoadUniformLoc("seaBright",		m_iSeaColorBright);
	LoadUniformLoc("seaDark",		m_iSeaColorDark);
	CShaderDeferred::Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderWaterPretty::SetTexFactor( float factor )
{
	if(m_iTexFactor != -1)
		glUniform1f(m_iTexFactor, factor);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderWaterPretty::SetMeshPosScale( const Vector3f &pos, float scale )
{
	if(m_iMeshPosScale != -1)
		glUniform4f(m_iMeshPosScale, pos.x, pos.y, pos.z, scale);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderWaterPretty::SetCamPos( const Vector3f &pos )
{
	if(m_iCamPos != -1)
		glUniform3f(m_iCamPos, pos.x, pos.y, pos.z);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderWaterPretty::SetSunDir( const Vector3f &sunDir )
{
	if(m_iSunDir != -1)
		glUniform3f(m_iSunDir, sunDir.x, sunDir.y, sunDir.z);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderWaterPretty::SetSeaColorDark(const Vector3f &color)
{
	Vector3f c = color.pow(2.2f);
	if(m_iSeaColorDark != -1)
		glUniform3f(m_iSeaColorDark, c.x, c.y, c.z);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderWaterPretty::SetSeaColorBright(const Vector3f &color)
{
	Vector3f c = color.pow(2.2f);
	if(m_iSeaColorBright != -1)
		glUniform3f(m_iSeaColorBright, c.x, c.y, c.z);
}


//#######################################################################
// CShaderPointSprite
//#######################################################################
DECLARE_SHADER( "PointSprite", CShaderPointSprite )

void CShaderPointSprite::Init( void )
{
	m_iPointSizeUniformLoc = glGetUniformLocation( m_iProgram, "PointSize" );
	if( m_iPointSizeUniformLoc == -1 )
		ConsoleMessage( "%s: Could not find uniform \"PointSize\"", GetName() );

	CShader::Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderPointSprite::SetPointSize( float size )
{
	if( m_iPointSizeUniformLoc != -1 )
		glUniform1f( m_iPointSizeUniformLoc, size );
}




//#######################################################################
// CShaderSpotLight
//#######################################################################
DECLARE_SHADER( "DeferredSpot", CShaderSpotLight )

void CShaderSpotLight::Init( void )
{
	glUniform1i( glGetUniformLocation( m_iProgram, "tex0" ), 0 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex1" ), 1 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex2" ), 2 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex3" ), 3 );

	LoadUniformLoc( "vLightPos", m_iLightPos );
	LoadUniformLoc( "vLightDir", m_iLightDir );
	LoadUniformLoc( "vLightColor", m_iLightColor );
	LoadUniformLoc( "vLightAttent", m_iLightAttent );
	LoadUniformLoc( "vLightAngle", m_iLightAngle );
	
	CShaderDeferred::Init();
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderSpotLight::SetLightData( const CSpotLight &light, const Matrix3 &mViewSpace, const Vector3f &mViewTransl )
{
	Vector3f pos = light.GetAbsPos();
	pos = mViewSpace * (pos - mViewTransl);

	Vector3f dir = mViewSpace * (light.GetAbsAngles() * Vector3f(0.0f, 0.0f, 1.0f));
	dir.Normalize();

	Vector4f c = gammaCorrect(light.GetColor());

	glUniform3f( m_iLightPos, pos.x, pos.y, pos.z );
	glUniform3f( m_iLightDir, dir.x, dir.y, dir.z );
	glUniform4f( m_iLightColor, c.x, c.y, c.z, c.w );
	glUniform4f( m_iLightAttent, light.GetConstant(), light.GetLinear(), light.GetQuadratic(), light.GetDist() );
	glUniform2f( m_iLightAngle, COS(light.GetDegrees()*0.5f), 0.0f );
}



//#######################################################################
// CShaderSpotLightTex
//#######################################################################

DECLARE_SHADER( "DeferredSpotTex", CShaderSpotLightTex )

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderSpotLightTex::Init( void )
{
	glUniform1i( glGetUniformLocation( m_iProgram, "tex0" ), 0 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex1" ), 1 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex2" ), 2 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex3" ), 3 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex4" ), 4 );

	LoadUniformLoc( "vLightColor", m_iLightColor );
	LoadUniformLoc( "mLightSpace", m_iLightMatrix );
	LoadUniformLoc( "vLightPos", m_iLightPos );
	LoadUniformLoc( "vLightAttent", m_iLightAttent );

	CShaderDeferred::Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderSpotLightTex::SetLightData( const CSpotTexLight &light, const Matrix3 &mViewSpace, const Vector3f &mViewTransl )
{
	Vector3f pos = mViewSpace * (light.GetAbsPos() - mViewTransl);
	Matrix3 lightSysRot = mViewSpace * light.GetAbsAngles().GetMatrix3().GetTranspose();
	Matrix4 lightSys;
	lightSysRot.ToOpenGlMatrix(&lightSys[0]);
	lightSys *= Matrix4::GetTranslation(-pos);
	
	Matrix4 lightProj = Matrix4::GetPerspective(light.GetFov(), light.GetAspect(), light.GetNear(), light.GetFar());
	Matrix4 lightMat = lightProj * lightSys;

	Vector4f c = gammaCorrect(light.GetColor());

	glUniformMatrix4fv(m_iLightMatrix, 1, GL_FALSE, lightMat.Get());
	glUniform4f(m_iLightColor, c.x, c.y, c.z, c.w);
	glUniform3f(m_iLightPos, pos.x, pos.y, pos.z);
	glUniform4f(m_iLightAttent, light.GetConstant(), light.GetLinear(), light.GetQuadratic(), light.GetDist() );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CShaderSpotLightTex::GetLightTexIndex()
{
	return 4;
}


//#######################################################################
// CShaderBlur
//#######################################################################
DECLARE_SHADER( "Blur", CShaderBlur )


void CShaderBlur::Init( void )
{
	LoadUniformLoc( "vBlurDir", m_iBlurDir );
	LoadUniformLoc( "lodLevel", m_iLodLevel );
	CShader::Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderBlur::SetBlurDirection( bool horizontal )
{
	if( horizontal )
		glUniform2f( m_iBlurDir, 1.0f, 0.0f );
	else
		glUniform2f( m_iBlurDir, 0.0f, 1.0f );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderBlur::SetLodLevel( int lod )
{
	glUniform1f(m_iLodLevel, (float)lod);
}



//#######################################################################
// CShaderBloomComplete
//#######################################################################
DECLARE_SHADER( "BloomComplete", CShaderBloomComplete )

void CShaderBloomComplete::Init( void )
{
	LoadUniformLoc( "cBrightpassSubstract", m_iBrightpassSubstract );
	LoadUniformLoc( "fLodLevel", m_iLodLevel );

	CShader::Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderBloomComplete::SetBrightpassSubstract( const Vector4f &sub )
{
	glUniform4f( m_iBrightpassSubstract, sub.x, sub.y, sub.z, sub.w );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderBloomComplete::SetLodLevel( int lod )
{
	glUniform1f( m_iLodLevel, (float)lod );
}


//#######################################################################
// CShaderTonemap
//#######################################################################
DECLARE_SHADER( "Tonemap", CShaderTonemap )


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderTonemap::Init( void )
{
	glUniform1i( glGetUniformLocation( m_iProgram, "tex0" ), 0 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex1" ), 1 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex2" ), 2 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex3" ), 3 );

	LoadUniformLoc( "iBloomLodStart", m_iBloomLodStart );
	LoadUniformLoc( "iBloomLodLevels", m_iBloomLodLevels );
	LoadUniformLoc( "fNearZ", m_iNearZ );
	LoadUniformLoc( "fFarZ", m_iFarZ );

	CShader::Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderTonemap::SetBloomLodLevels( int lodStart, int lodLevels )
{
	glUniform1i( m_iBloomLodStart, lodStart );
	glUniform1i( m_iBloomLodLevels, lodLevels );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderTonemap::SetZBounds(float nearZ, float farZ)
{
	glUniform1f( m_iNearZ, nearZ );
	glUniform1f( m_iFarZ, farZ );
}


//#######################################################################
// CShaderBrightpass
//#######################################################################
DECLARE_SHADER("Brightpass", CShaderBrightpass)


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderBrightpass::Init( void )
{
	LoadUniformLoc( "sourceLod", m_iLodLevel );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex0" ), 0 );
	glUniform1i( glGetUniformLocation( m_iProgram, "tex1" ), 1 );
	CShader::Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderBrightpass::SetLodLevel( int lod )
{
	glUniform1i(m_iLodLevel, lod);
}


//#######################################################################
// CShaderExposure
//#######################################################################
DECLARE_SHADER("Exposure", CShaderExposure)

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderExposure::Init( void )
{
	glUniform1i( glGetUniformLocation( m_iProgram, "texLastLum" ), 0 );
	glUniform1i( glGetUniformLocation( m_iProgram, "texScene" ), 1 );
	LoadUniformLoc( "frametime", m_iFrametime );
	CShader::Init();
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderExposure::SetFrametime(float frametime)
{
	glUniform1f(m_iFrametime, frametime);
}


//#######################################################################
// CShaderShadowMapVisualize
//#######################################################################
DECLARE_SHADER("ShadowMapVisualize", CShaderShadowMapVisualize)

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderShadowMapVisualize::Init( void )
{
	LoadUniformLoc( "slice", m_iSlice );
	CShader::Init();
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderShadowMapVisualize::SetSlice(float slice)
{
	glUniform1f(m_iSlice, slice);
}


//#######################################################################
// CShaderSky
//#######################################################################
DECLARE_SHADER("Sky", CShaderSky)


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderSky::Init( void )
{
	LoadUniformLoc( "brightness", m_iBrightness );
	CShader::Init();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderSky::SetBrightness(float brightness)
{
	glUniform1f(m_iBrightness, brightness);
}


//#######################################################################
// CShaderAntiAlias
//#######################################################################
DECLARE_SHADER("AntiAlias", CShaderAntiAlias)


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CShaderAntiAlias::Init( void )
{
	glUniform1i( glGetUniformLocation( m_iProgram, "tex0" ), 0 ); //Scene Color
	glUniform1i( glGetUniformLocation( m_iProgram, "tex1" ), 1 ); //Scene Depth
	glUniform1i( glGetUniformLocation( m_iProgram, "tex2" ), 2 ); //GBuffer Normals

	CShader::Init();
}


//#######################################################################
// CShaderDepthCopy
//#######################################################################

DECLARE_SHADER("DepthCopy", CShaderDepthCopy)

void CShaderDepthCopy::Init( void )
{
	glUniform1i( glGetUniformLocation( m_iProgram, "tex0" ), 0 );

	CShader::Init();
}

