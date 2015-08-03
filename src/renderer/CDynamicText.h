// renderer/CDynamicText.h
// renderer/CDynamicText.h
// renderer/CDynamicText.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__CDynamicText_H__
#define deferred__renderer__CDynamicText_H__

#include "CGuiFactory.h"
#include <string>
#include <util/dll.h>

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING

class CFont;
class DLL CDynamicText : public gui::CGuiElement
{
	DECLARE_GUI_DATADESC(gui::CGuiElement, CDynamicText)

public:
	CDynamicText();
	~CDynamicText();

	void Init( void );

	void SetText( const char *pText );
	void SetText( const std::string &str );
	inline const std::string &GetText( void ) const { return m_Text; }

	inline void SetFont( CFont *pFont ) { m_pFont = pFont; }
	void SetFont( const std::string &name );
	inline const CFont *GetFont( void ) const { return m_pFont; }
	float GetStringWidth() const;

	virtual void VRenderCustom( void );
	float GetStringHeight() const;
	/*virtual void VParseAdditionalVars( CScriptSubGroup *vars );*/

public:
	CFont *m_pFont;
	std::string m_Text;
};

POP_PRAGMA_WARNINGS

#endif
