// client/CGuiTextLocalized.cpp
// client/CGuiTextLocalized.cpp
//CGuiTextLocalized.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "CGuiTextLocalized.h"
#include "CGuiLanguage.h"

namespace gui
{
	
DECLARE_GUI_ELEMENT(TEXT, CGuiTextLocalized)

DATADESC_GUI_BEGIN(CGuiTextLocalized)
	SetText(GetText());
DATADESC_GUI_END()

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiTextLocalized::SetText( const std::string &text )
{
	m_textLoc = text;
	CDynamicText::SetText(CGuiLanguage::GetInst().localize(text));
}


}