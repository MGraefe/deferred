// renderer/CFont.cpp
// renderer/CFont.cpp
// renderer/CFont.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H

#include <util/error.h>
#include "CFont.h"
#include "glheaders.h"
#include "CVertexBuffer.h"
#include "CVertexIndexBuffer.h"
#include "CShaders.h"
#include <util/instream.h>

#define CHAR_GLYPH_OFFSET 32

size_t charToGlyphIndex(char c)
{
	return (size_t)c - CHAR_GLYPH_OFFSET;
}


char glyphIndexToChar(size_t i)
{
	return (char)(i + CHAR_GLYPH_OFFSET);
}


CFont::CFont()
{
	m_fHeight = 0.0f;
	m_fInternalHeight = 0.0f;
	m_uiTextureIndex = 0;
	m_pchFontName[0] = '\0';
	m_iGlyphDataSize = 0;
	m_fTexWidth = 0.0f;
	m_fTexHeight = 0.0f;
	m_fLineSpacing = 10.0f;
	m_bLoaded = false;
	m_bIsGuiFont = true;	
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CFont::~CFont()
{
	if( !m_bLoaded )
		return;

	glDeleteTextures( 1, &m_uiTextureIndex );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const glyphdata_t &CFont::GetGlyphData( char c ) const
{
	return m_glyphData[charToGlyphIndex(c)];
}


//---------------------------------------------------------------
// Purpose: this shouldn't be called by anyone besides CFontManager
//---------------------------------------------------------------
bool CFont::LoadFont( const char *pFilename, float fHeight, bool bGuiFont )
{
	m_bIsGuiFont = bGuiFont;
	m_fHeight = fHeight;
	UINT timeStart = g_RenderInterf->GetTimer()->GetTimeMs();

	FT_Library ftlib;
	if( FT_Init_FreeType(&ftlib) != 0 )
	{
		error( "Couldn't init FreeType2 library!" );
		return false;
	}

	fs::ifstream is(pFilename, std::ios::in | std::ios::binary);
	if(!is.is_open())
	{
		error( "Couldn't open font file \"%s\".", pFilename );
		FT_Done_FreeType(ftlib);
		return false;
	}

	size_t fileSize = is.getFileSize();
	std::vector<char> fontFileMem(fileSize);
	is.read(fontFileMem.data(), fileSize);
	is.close();

	FT_Face ftface;
	if( FT_New_Memory_Face( ftlib, (FT_Byte*)fontFileMem.data(), fileSize, 0, &ftface ) != 0 )
	{
		error( "Couldn't load font file \"%s\".", pFilename );
		FT_Done_FreeType(ftlib);
		return false;
	}

	if( !(ftface->face_flags & FT_FACE_FLAG_SCALABLE) ||
		!(ftface->face_flags & FT_FACE_FLAG_HORIZONTAL) )
	{
		error( "Error setting font size in \"%s\".", pFilename );
		FT_Done_Face(ftface);
		FT_Done_FreeType(ftlib);
		return false;
	}

	FT_Set_Pixel_Sizes( ftface, 0, (FT_UInt)fHeight );

	//Define some glyphs we want to render
	UINT iGlyphs = '~' - ' ' + 1;
	std::vector<char> pchGlyphs(iGlyphs+1);
	for( UINT i = 0; i < iGlyphs; i++ )
		pchGlyphs[i] = glyphIndexToChar(i);
	pchGlyphs[iGlyphs] = (char)0xFF;

	// First we go over all the characters to find the max descent
	// and ascent (space required above and below the base of a
	// line of text) and needed image size. There are simpler methods
	// to obtain these with FreeType but they are unreliable.
	UINT iGlyphMargin = 1;
	UINT iImageHeight = 0;
	UINT iImageWidth = 512;
	int iMaxDescent = 0;
	int iMaxAscent = 0;
	UINT iSpaceOnLine = iImageWidth - iGlyphMargin;
	UINT iLines = 1;
	
	std::vector<FT_Bitmap> ftbitmaps(iGlyphs);
	std::vector<FT_Glyph_Metrics> ftmetrics(iGlyphs);
	std::vector<int> ftbitmaplefts(iGlyphs);

	for( UINT i = 0; i < iGlyphs; i++ )
	{
		UINT iCharIndex = FT_Get_Char_Index( ftface, (UINT)pchGlyphs[i] );
		if( pchGlyphs[i] == (char)0xFF )
			iCharIndex = 0;

		FT_Load_Glyph( ftface, iCharIndex, FT_LOAD_DEFAULT );
		FT_Render_Glyph( ftface->glyph, FT_RENDER_MODE_NORMAL );

		//copy for later usage
		ftmetrics[i] = ftface->glyph->metrics;
		FT_Bitmap_New( &ftbitmaps[i] );
		FT_Bitmap_Copy( ftlib, &ftface->glyph->bitmap, &ftbitmaps[i] );
		ftbitmaplefts[i] = ftface->glyph->bitmap_left;

		UINT iAdvance = ftface->glyph->metrics.horiAdvance / 64 + iGlyphMargin;
		
		//if the line is full go to the next line
		if( iAdvance > iSpaceOnLine )
		{
			iSpaceOnLine = iImageWidth - iGlyphMargin;
			iLines++;
		}
		iSpaceOnLine -= iAdvance;

		iMaxAscent = max( ftface->glyph->bitmap_top, iMaxAscent );
		iMaxDescent = max( ftface->glyph->bitmap.rows - ftface->glyph->bitmap_top,
							iMaxDescent );
	}

	//All glyph metrics have been read.
	//Now compute height of texture
	UINT iNeededImageHeight = (iMaxAscent + iMaxDescent + iGlyphMargin)*iLines + iGlyphMargin;
	iImageHeight = 16;
	while( iImageHeight < iNeededImageHeight )
		iImageHeight *= 2;

	m_fTexWidth = (float)iImageWidth;
	m_fTexHeight = (float)iImageHeight;

	float fXFactor = 1.0f/(float)iImageWidth;
	float fYFactor = 1.0f/(float)iImageHeight;

	std::vector<BYTE> pImage(iImageHeight*iImageWidth, 0);
	
	UINT x = iGlyphMargin;
	UINT y = iGlyphMargin;// + iMaxAscent;

	m_glyphData.resize(iGlyphs);
	m_iGlyphDataSize = iGlyphs;

	for( UINT i = 0; i < iGlyphs; i++ )
	{
		//metrics shortcut
		FT_Glyph_Metrics *metrics = &ftmetrics[i];
		FT_Bitmap *bitmap = &ftbitmaps[i];

		//See whether the character fits on the current line
		UINT iAdvance = metrics->horiAdvance / 64 + iGlyphMargin;
		if( iAdvance > iImageWidth - x )
		{
			x = iGlyphMargin;
			y += (iMaxAscent + iMaxDescent + iGlyphMargin);
		}

		//Fill in the glyph data
		m_glyphData[i].ascii = pchGlyphs[i];
		m_glyphData[i].width = ((float)(iAdvance - iGlyphMargin))*fXFactor;
		m_glyphData[i].u = ((float)x) * fXFactor;
		m_glyphData[i].v = ((float)(y - iGlyphMargin)) * fYFactor;
		m_glyphData[i].yofs = (float)(metrics->height - metrics->horiBearingY)*(-1.0f/64.0f);

		//Copy the image to our texture
		for( int row = 0; row < bitmap->rows; row++ )
		{
			int iByteCount =	bitmap->width;
			int iImageDest =	(x + ftbitmaplefts[i]) + (y + row)*iImageWidth;
			int iImageSrc =		(bitmap->rows-row-1) * bitmap->pitch;
			memcpy( pImage.data() + iImageDest, bitmap->buffer + iImageSrc, iByteCount );
		}

		FT_Bitmap_Done( ftlib, bitmap );
		x += iAdvance;
	}

	m_fLineSpacing = fHeight * 1.5f;

	FT_Done_Face( ftface );
	FT_Done_FreeType( ftlib );

	//BYTE *pImageExpanded = new BYTE[iImageHeight*iImageWidth*4];
	std::vector<BYTE> pImageExpanded(iImageWidth*iImageHeight*4, 255);
	for( UINT i = 0; i < iImageHeight*iImageWidth; i++ )
		pImageExpanded[i*4+3] = pImage[i];

	//Create an OpenGL texture for our font.
	glGenTextures( 1, &m_uiTextureIndex );
	glBindTexture( GL_TEXTURE_2D, m_uiTextureIndex );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, iImageWidth, iImageHeight, 0, 
				GL_RGBA, GL_UNSIGNED_BYTE, pImageExpanded.data() );

	m_glyphVerts.resize(iGlyphs);
	float fGlyphHeight = (iMaxAscent + iMaxDescent) * fYFactor;
	for( UINT i = 0; i < iGlyphs; i++ )
	{
		float fVertGlyphWidth = m_glyphData[i].width * m_fTexWidth;
		float fVertGlyphHeight = (float)(iMaxAscent + iMaxDescent);
		float fVertYOfs = m_glyphData[i].yofs;
		float fUVWidth = m_glyphData[i].width;

		//The GUI is flipped upside down
		if( bGuiFont )
		{
			fVertYOfs = -fVertYOfs;
			fVertGlyphHeight = -fVertGlyphHeight;
		}

		//Bottom-Left
		m_glyphVerts[i].verts[0].vPos.Init( 0, fVertYOfs, 0 );
		m_glyphVerts[i].verts[0].u = m_glyphData[i].u;
		m_glyphVerts[i].verts[0].v = m_glyphData[i].v;
		//Bottom-Right
		m_glyphVerts[i].verts[1].vPos.Init( fVertGlyphWidth, fVertYOfs, 0 );
		m_glyphVerts[i].verts[1].u = m_glyphData[i].u + fUVWidth;
		m_glyphVerts[i].verts[1].v = m_glyphData[i].v;
		//Top-Right
		m_glyphVerts[i].verts[2].vPos.Init( fVertGlyphWidth, fVertGlyphHeight+fVertYOfs, 0 );
		m_glyphVerts[i].verts[2].u = m_glyphData[i].u + fUVWidth;
		m_glyphVerts[i].verts[2].v = m_glyphData[i].v + fGlyphHeight;
		//Top-Left
		m_glyphVerts[i].verts[3].vPos.Init( 0, fVertGlyphHeight+fVertYOfs, 0 );
		m_glyphVerts[i].verts[3].u = m_glyphData[i].u;
		m_glyphVerts[i].verts[3].v = m_glyphData[i].v + fGlyphHeight;
	}
	

	UINT time = g_RenderInterf->GetTimer()->GetTimeMs() - timeStart;
	ConsoleMessage( "Loading font '%s' size %f took %i ms.", pFilename, fHeight, time );

	m_bLoaded = true;
	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CFont::HasGlyph( char c ) const
{
	size_t index = charToGlyphIndex(c);
	return index >= 0 && index < m_glyphData.size();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const glyphvertinfo_t & CFont::GetGlyphVertInfo( char c ) const
{
	return m_glyphVerts[charToGlyphIndex(c)];
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CFont::GetGlyphWidth( char c ) const
{
	return GetGlyphData(c).width * m_fTexWidth;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CFont::GetStringWidth( const std::string &str ) const
{
	float w = 0.0f;
	for(char c : str)
		if(HasGlyph(c))
			w += GetGlyphWidth(c);
	return w;
}

