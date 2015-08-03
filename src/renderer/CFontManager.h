// renderer/CFontManager.h
// renderer/CFontManager.h
// renderer/CFontManager.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__CFontManager_H__
#define deferred__renderer__CFontManager_H__

#include <util/basic_types.h>
#include "CFont.h"
#include "Interfaces.h"

struct fontdata_t
{
	std::string name;
	std::string face;
	CFont *pFont;
	float height;
};

typedef std::map<std::string, fontdata_t, LessThanCppStringObj> FontNameMap;
class CFontManager : public IFontManager
{
public:
	CFontManager();
	~CFontManager();
	void SetScaleFactor( float fScale ) { m_fScale = fScale; }
	void LoadFonts( void );
	CFont *GetFontObject( const std::string &name );
	const fontdata_t *GetFontData( const std::string &name );

private:
	FontNameMap m_FontNameMap;
	float m_fScale;
};

#endif
