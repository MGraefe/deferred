// renderer/CDynamicText.cpp
// renderer/CDynamicText.cpp
// renderer/CDynamicText.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "CDynamicText.h"
#include "CFont.h"
#include "CRenderInterf.h"
#include "CFontManager.h"
#include "CShaders.h"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CDynamicText::CDynamicText()
{
	m_pFont = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CDynamicText::~CDynamicText()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CDynamicText::Init( void )
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CDynamicText::VParseAdditionalVars( CScriptSubGroup *vars )
{
	char pchFontName[64];
	if( vars->GetString( "font", pchFontName, ArrayLength(pchFontName) ) )
	{
		std::string str( pchFontName );
		SetFont( str );
	}

	char pchText[256];
	if( vars->GetString( "text", pchText, ArrayLength(pchText) ) )
	{
		SetText( pchText );
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CDynamicText::SetText( const char *pText )
{
	m_Text.assign( pText );
}


void CDynamicText::SetText( const std::string &str )
{
	m_Text = str;
}


//---------------------------------------------------------------
// Purpose: All real drawing code goes here!
//---------------------------------------------------------------
void CDynamicText::VRenderCustom( void )
{
	//m_pFont->BindForRendering();
	//m_pFont->RenderString( m_Text.c_str() );
	//m_pFont->UnBindFromRendering();
	glBindTexture(GL_TEXTURE_2D, m_pFont->GetTexIndex());
	std::vector<glyphvertinfo_t> verts;
	verts.reserve(m_Text.length());

	float lineSpacing = m_pFont->GetLineSpacing();
	Vector3f pos = vec3_null;
	for(size_t i = 0; i < m_Text.length(); ++i)
	{
		char c = m_Text[i];
		if(c == '\n')
		{
			pos.x = 0.0f;
			pos.y += lineSpacing;
		}
		else
		{
			if(!m_pFont->HasGlyph(c))
				c = '?';
			glyphvertinfo_t info = m_pFont->GetGlyphVertInfo(c);
			for(size_t i = 0; i < 4; ++i)
				info.verts[i].vPos += pos;
			verts.push_back(info);
			pos.x += m_pFont->GetGlyphWidth(c);
			pos.z += 0.0001f;
		}
	}

	GetShaderManager()->InitVertexbufferVars(VBF_POS_UV, verts.data());
	GetShaderManager()->GetActiveShader()->UpdateMatrices();
	glDrawArrays(GL_QUADS, 0, verts.size()*4);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CDynamicText::SetFont( const std::string &name )
{
	m_pFont = g_RenderInterf->GetFontManager()->GetFontObject( name );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CDynamicText::GetStringWidth() const
{
	if(!m_pFont)
		return 0.0f;
	return m_pFont->GetStringWidth(m_Text);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CDynamicText::GetStringHeight() const
{
	if(!m_pFont)
		return 0.0f;
	return m_pFont->GetFontHeight();
}
