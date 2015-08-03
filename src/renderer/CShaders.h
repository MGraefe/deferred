// renderer/CShaders.h
// renderer/CShaders.h
// renderer/CShaders.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CShaders_H__
#define deferred__renderer__CShaders_H__

#include "CLightList.h"
#include "glheaders.h"
#include "texture_t.h"
#include <util/string_t.h>
#include "mesh_vertex_t.h"
#include <util/maths.h>
#include "vertexbuffer_formats.h"
#include <util/dll.h>

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING;

//Shader variable bits
#define SHADER_VERTEX			1
#define SHADER_NORMAL			2
#define SHADER_TANGENT			4
#define SHADER_UVCOORD			8
#define SHADER_SKY_VISIBILITY	16



//Pure Virtual Base Class for all Shaders
class DLL CShader
{
public:
	CShader();
	virtual ~CShader();
	virtual void ReloadShader( void );
	virtual bool LoadShader( void );
	virtual void DeleteShader( void );
	virtual void Enable( void );
	virtual void Init( void );
	virtual int GetAttribLoc( int attrib );
	virtual void UpdateMatrices( bool forceUpdate = false );
	virtual UINT GetNeededVars( void ) = 0;

	const char *GetName( void ) const { return m_name.c_str(); }
	void SetName( const std::string &name ) { m_name = name; }
	void SetFileNames( const std::string &vs_name, const std::string &ps_name );
	void CheckForError( int prog, const char *pFilename, const std::string &shaderSource );
	void SetReverseNormals( bool bReverse );

	virtual void SetMaterialAttributes( const materialprops_t &props ) { }
	virtual void SetNormalMapEnabled( bool bEnabled ) { }
	virtual void SetDrawColor( float r, float g, float b, float a );
	virtual void EnableTexturing( bool enabled = true ) { }
	virtual void SetPointSize( float size ) { }
	virtual void SetSkyVisFactor( float factor ) { }

	void SetUniform1f(const char *name, float v);
	void SetUniform2f(const char *name, float v1, float v2);

	int LoadUniformLoc(const char *name);
	int LoadAttributeLoc(const char *name);

	inline bool LoadUniformLoc(const char *name, int &loc) {
		loc = LoadUniformLoc(name);
		return loc != -1;
	}

	inline bool LoadAttributeLoc(const char *name, int &loc) {
		loc = LoadAttributeLoc(name);
		return loc != -1;
	}
public:
	int m_iProgram;
	
private:
	std::string m_name;

protected:
	std::string m_fragProgName;
	std::string m_vertProgName;
	bool	bIsActive;
	bool	bReverseNormals;
	int		m_iVertexShader;
	int		m_iFragmentShader;
	int		m_iLocModelviewMatrix;
	int		m_iLocProjectionMatrix;
	int		m_iLocModelviewProjectionMatrix;
	int		m_iLocNormalMatrix;
	int		m_iLocAttribPos;
	int		m_iLocAttribNormal;
	int		m_iLocAttribUvCoord;
	int		m_iLocAttribTangent;
	int		m_iLocAttribSkyVisibility;
};


//Manager for all shaders
class DLL CShaderManager
{
public:
	CShaderManager();
	~CShaderManager();
	void LoadAllShaders( void );
	void ReloadShaders( void );
	CShader *GetShader( const char *pchName );
	//CGcontext cgContext;
	void SetShaderStatus( bool bActive );
	CShader *GetActiveShader( void ) { return m_ActiveShader; }
	void SetActiveShader( CShader *shd );
	bool GetShaderStatus( void ) { return m_bShadersActive; }
	void InitializeShaders( void );

	void InitVertexbufferVars(vertexbuffer_format_e format, const void* buffer = NULL);
	void UnbindAllVertexbufferVars();
private:
	void SetVertexAttribPointer(UINT attrib, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);
	void init_vertexbuffer_vars_rendervert_t( const void *buffer );
	void init_vertexbuffer_vars_pos_normal_uvcoord( const void *buffer );
	void init_vertexbuffer_vars_pos_uvcoord( const void *buffer );
	void init_vertexbuffer_vars_uvcoord_pos( const void *buffer );
	void init_vertexbuffer_vars_pos( const void *buffer );

	std::set<int> m_activeVertexbufferAttribs;

	void LoadShader( const char *name, const CScriptParser &parser );
	std::vector<CShader*> m_shaders;
	CShader *m_ActiveShader;
	bool m_bShadersActive;
	vertexbuffer_format_e m_currentFormat;
	CShader *m_currentFormatSetShader;
	const void *m_lastBuffer;
};

//extern CShaderManager ShaderManager;
DLL CShaderManager *GetShaderManager(void);


//Declares a shader, name must match to entry in file "shaders/shaders.txt"
#define DECLARE_SHADER(name,classname) namespace shader{ \
		static CShaderFactory<classname> classname##_Factory( name ); \
	}


//SHADER FACTORIES
class IShaderFactory;
typedef std::map<std::string, IShaderFactory*, LessThanCppStringObj> ShaderFactoryMap;

ShaderFactoryMap &GetShaderFactoryMap(void);

class IShaderFactory
{
public:
	virtual CShader *Create( void ) = 0;
};

template<typename T>
class CShaderFactory : public IShaderFactory
{
public:
	CShaderFactory( const char *shaderName ) {
		GetShaderFactoryMap()[std::string(shaderName)] = this;
	}

	inline CShader *Create( void ) {
		return new T();
	}
};


//All Shaders go here!
class DLL  CShaderUnlit : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX | SHADER_UVCOORD; }
	virtual void Init( void );
	void EnableTexturing( bool bEnabled );
	void SetDrawColor( float r, float g, float b, float a );
	void SetSingleChannelAlpha(bool status);
private:
	int m_iLocationTextureEnabled;
	int m_iLocationDrawColor;
	int m_iLocationSingleChannelAlpha;
};


class DLL CShaderGBufferOut : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX |
		SHADER_TANGENT | SHADER_NORMAL | SHADER_UVCOORD | SHADER_SKY_VISIBILITY; }
	virtual void Init( void );
	int GetTangAttribLoc( void ) { return m_iTangAttribLoc; }
	virtual void SetNormalMapEnabled( bool bEnabled );
	virtual void SetMaterialAttributes( const materialprops_t &props );
	virtual void SetSkyVisFactor(float factor);
private:
	int m_iTangAttribLoc;
	int m_iNormalMapEnabledUniformLoc;
	int m_iTranslucencyEnabledUniformLoc;
	int m_iDoubleSided;
	int m_iSpecular;
	int m_iSmoothness;
	int m_iSkyVisFactor;
};


class DLL CShaderDeferred : public CShader
{
public:
	virtual UINT GetNeededVars( void ) { return SHADER_VERTEX; }
	virtual void Init( void );
};


class DLL CShaderDeferredLighting : public CShaderDeferred
{
public:
	virtual void Init( void );
	void UpdateLightingData( const CSunLight &light,
		const Matrix3 &mViewSpace,
		const Matrix4f &mViewSpace4x4,
		const Matrix4f *mShadowSpaces,
		int iShadowCascades );
private:
	int m_iSunLightDir;
	int m_iSunLightColor;
	int m_iShadowSpaceMatLoc;
	int m_iAmbientColor1;
	int m_iAmbientColor2;
	int m_iSunLightSpecularColor;
};

class DLL CShaderWater : public CShaderDeferred
{
public:
	virtual void Init( void );
	UINT GetNeededVars( void ) { return SHADER_VERTEX | SHADER_UVCOORD | SHADER_NORMAL | SHADER_TANGENT; }
};


class DLL CShaderWaterPretty : public CShaderDeferred
{
public:
	virtual void Init( void );
	UINT GetNeededVars( void ) { return SHADER_VERTEX; }
	void SetTexFactor(float factor);
	void SetMeshPosScale(const Vector3f &pos, float scale);
	void SetCamPos(const Vector3f &pos);
	void SetSunDir(const Vector3f &sunDir);
	void SetSeaColorDark(const Vector3f &color);
	void SetSeaColorBright(const Vector3f &color);
private:
	int m_iSeaColorBright;
	int m_iSeaColorDark;
	int m_iTexFactor;
	int m_iSunDir;
	int m_iMeshPosScale;
	int m_iCamPos;
};


class DLL CShaderShadowMap : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX | SHADER_UVCOORD; }
	void SetMaterialAttributes( const materialprops_t &props );
	void Init( void );

private:
	//void Init( void );
	int m_iTransparencyEnabledLoc;
};

class DLL CShaderPointLight : public CShaderDeferred
{
public:
	virtual void Init( void );
	void SetLightData( const CPointLight &light, const Matrix3 &mViewSpace, const Vector3f &mViewTransl  ); 

private:
	int m_iPointLightPos;
	int m_iPointLightColor;
	int m_iPointLightAttent;
};


class DLL CShaderSpotLight : public CShaderDeferred
{
public:
	virtual void Init( void );
	void SetLightData( const CSpotLight &light, const Matrix3 &mViewSpace, const Vector3f &mViewTransl  ); 

private:
	int m_iLightPos;
	int m_iLightDir;
	int m_iLightColor;
	int m_iLightAttent;
	int m_iLightAngle;
};


class DLL CShaderSpotLightTex : public CShaderDeferred
{
public:
	virtual void Init( void );
	void SetLightData( const CSpotTexLight &light, const Matrix3 &mViewSpace, const Vector3f &mViewTransl );
	int GetLightTexIndex();

private:
	int m_iLightColor;
	int m_iLightMatrix;
	int m_iLightPos;
	int m_iLightAttent;
};


class DLL CShaderPointSprite : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX; }
	virtual void Init( void );
	virtual void SetPointSize( float size );

private:
	int m_iPointSizeUniformLoc;
};


class DLL CShaderBlur : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX | SHADER_UVCOORD; }
	virtual void Init( void );
	void SetBlurDirection( bool horizontal );
	void SetLodLevel(int lod);
private:
	int m_iBlurDir;
	int m_iLodLevel;
};



class DLL CShaderBloomComplete : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX | SHADER_UVCOORD; }
	virtual void Init( void );
	void SetBrightpassSubstract(const Vector4f &sub);
	void SetLodLevel(int lod);
private:
	int m_iBrightpassSubstract;
	int m_iLodLevel;
};


class DLL CShaderTonemap : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX | SHADER_UVCOORD; }
	virtual void Init( void );
	void SetBloomLodLevels(int lodStart, int lodLevels);
	void SetZBounds(float nearZ, float farZ);
private:
	int m_iBloomLodStart;
	int m_iBloomLodLevels;
	int m_iNearZ;
	int m_iFarZ;
};


class DLL CShaderBrightpass : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX | SHADER_UVCOORD; }
	virtual void Init( void );
	void SetLodLevel(int lod);

private:
	int m_iLodLevel;
};


class DLL CShaderExposure : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX | SHADER_UVCOORD; }
	virtual void Init( void );
	void SetFrametime(float frametime);

private:
	int m_iFrametime;	
};


class DLL CShaderShadowMapVisualize : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX | SHADER_UVCOORD; }
	virtual void Init( void );
	void SetSlice(float slice);
	
private:
	int m_iSlice;
};


class DLL CShaderSky : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX; }
	virtual void Init( void );
	void SetBrightness(float brightness);
private:
	int m_iBrightness;
};


class DLL CShaderAntiAlias : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX | SHADER_UVCOORD; }
	virtual void Init( void );
};


class DLL CShaderDepthCopy : public CShader
{
public:
	UINT GetNeededVars( void ) { return SHADER_VERTEX | SHADER_UVCOORD; }
	virtual void Init( void );
};

POP_PRAGMA_WARNINGS;

#endif
