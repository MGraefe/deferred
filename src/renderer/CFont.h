// renderer/CFont.h
// renderer/CFont.h
// renderer/CFont.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__CFont_H__
#define deferred__renderer__CFont_H__

#include <util/basic_types.h>
#include "CGuiElement.h"

//Forward declarations
template<typename T> class CVertexIndexBuffer;
class CVertexBuffer;

#define FONTNAME_MAX_LENGTH 64


struct glyphdata_t
{
	float width; //Width in uv-coordinate space
	float u; //uv-coords left
	float v; //uv-coords bottom
	float yofs; //pixel-coords y offset (near-zero for 'o' or 'e', negative for 'g' or 'p')
	char ascii;
};

struct glyphvert_t
{
	Vector3f vPos;
	float u;
	float v;
};

struct glyphvertinfo_t
{
	glyphvert_t verts[4];
};


class CFont
{
private:
	EMPTY_COPY_CONSTRUCTOR( CFont )
public:
	CFont();
	~CFont();

	UINT GetTexIndex( void ) const { return m_uiTextureIndex; }
	float GetLineSpacing(void) const { return m_fLineSpacing; }

	bool HasGlyph(char c) const;

	//NOTENOTE: this shouldnt be called by anyone besides CFontManager
	bool LoadFont( const char *pFilename, float fHeight, bool bGuiFont = true );

	const glyphdata_t &GetGlyphData( char c ) const;
	const glyphvertinfo_t &GetGlyphVertInfo( char c ) const;
	float GetGlyphWidth(char c) const;
	float GetStringWidth(const std::string &str) const;
	float GetFontHeight() const { return m_fHeight; }
private:
	float m_fHeight;
	float m_fInternalHeight;
	UINT m_uiTextureIndex;
	char *m_pchFontName[FONTNAME_MAX_LENGTH];
	std::vector<glyphdata_t> m_glyphData;
	std::vector<glyphvertinfo_t> m_glyphVerts;
	int m_iGlyphDataSize;
	float m_fTexWidth;
	float m_fTexHeight;
	float m_fLineSpacing;
	bool m_bLoaded;
	bool m_bIsGuiFont;
};


#endif
