// renderer/CFontManager.cpp
// renderer/CFontManager.cpp
// renderer/CFontManager.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CFontManager.h"
#include <util/CScriptParser.h>

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CFontManager::CFontManager()
{
	m_fScale = 1.0f;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CFontManager::~CFontManager()
{
	FontNameMap::iterator it = m_FontNameMap.begin();
	FontNameMap::iterator itEnd = m_FontNameMap.end();

	for( ; it != itEnd; it++ )
	{
		delete it->second.pFont;
		it->second.pFont = NULL;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CFontManager::LoadFonts( void )
{
	CScriptParser parser;
	parser.ParseFile( "scripts/fonts.rps" );

	const SubGroupMap *pMap = parser.GetSubGroupMap();
	SubGroupMap::const_iterator it = pMap->begin();
	
	for( ; it != pMap->end(); it++ )
	{
		CScriptSubGroup *pSubGrp = it->second;
		
		char pchFace[32];
		pSubGrp->GetString( "face", pchFace, ArrayLength(pchFace) );
		float fHeight = pSubGrp->GetFloat( "height", 12.0f );
		fHeight *= m_fScale;

		char pchFileName[64];
		strcpy( pchFileName, "misc/" );
		strcat( pchFileName, pchFace );
		strcat( pchFileName, ".ttf" );

		CFont *pFont = new CFont();
		if( !pFont->LoadFont( pchFileName, fHeight, true ) )
		{
			error( "Error loading font face \"%s\".", pchFileName );
			delete pFont;
			continue;
		}
		
		fontdata_t fontdata;
		fontdata.face = pchFace;
		fontdata.height = fHeight;
		fontdata.name = it->first;
		fontdata.pFont = pFont;
		m_FontNameMap.insert( std::make_pair( it->first, fontdata ) );
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CFont *CFontManager::GetFontObject( const std::string &name )
{
	FontNameMap::const_iterator it = m_FontNameMap.find( name );
	if( it == m_FontNameMap.end() )
		return NULL;

	return it->second.pFont;	
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const fontdata_t *CFontManager::GetFontData( const std::string &name )
{
	FontNameMap::const_iterator it = m_FontNameMap.find( name );
	if( it == m_FontNameMap.end() )
		return NULL;

	return &it->second;
}
