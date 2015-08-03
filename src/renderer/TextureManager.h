// renderer/TextureManager.h
// renderer/TextureManager.h
// renderer/TextureManager.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__TextureManager_H__
#define deferred__renderer__TextureManager_H__

#include <vector>
#include "texture_t.h"
#include "Interfaces.h"
#include "glheaders.h"

class CScriptParser;
class CTextureManager : public ITextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	bool LoadTexture( const char *pFilename, texture_t &tex, bool bForceLoadFromDisk = false, bool bPutInTextureList = true, bool flipY = false, bool srgb = false );
	void	DeleteTexture( const texture_t &tex );
	void	Init( void );
	void	ReloadAllTextures( void );
	void	AddUsedTextureMem( int mem ) { m_iUsedTextureMem += mem; }
	void	SubtractUsedTextureMem( int mem ) { m_iUsedTextureMem -= mem; }
	void	SetUsedTextureMem( int mem ) { m_iUsedTextureMem = mem; }
	int		GetUsedTextureMem( void ) { return m_iUsedTextureMem; }
	void	GetTexSpecular( int iTexID, float *pSpec4 );
	void	GetTexShininess( int iTexID, float *pShine4 );
	const materialprops_t &GetMaterialProps( int id );
	int		LoadMaterial( const char *name );
	void	BindMaterial( int index );
	void	CreateDefaultMaterialFile( const char *name );
	bool	IsMaterialTranslucent( int index );
	bool	LoadImageFromFile( const char *pFilename, int index, texture_t &tex, int textureTarget = GL_TEXTURE_2D, bool flipY = false, bool srgb = false );
	void	ParseMaterialGroups( void );
private:
	void ParseMaterialProps(CScriptParser *parser, material_t &mat);

	std::vector<texture_t>	m_vTextures;
	std::vector<material_t> m_vMaterials;
	int			m_iUsedTextureMem;
	CScriptParser *m_pMaterialGroups;
};

#endif
