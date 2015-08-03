// renderer/CGuiBuilder.h
// renderer/CGuiBuilder.h
// renderer/CGuiBuilder.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__CGuiBuilder_H__
#define deferred__renderer__CGuiBuilder_H__

#include "CGuiFactory.h"
#include <util/CScriptParser.h>

namespace gui
{

class CGuiBuilder
{
public:
	bool BuildGui( const char *pScriptPath, CGuiElement *pParent );
		
private:
	Vector3f GetGuiElementPos( CScriptSubGroup *elem, CGuiElement *guiElem );
	Vector2f GetGuiElementSize( CScriptSubGroup *elem, CGuiElement *guiElem );
	void UpdateParamsFromScript( CScriptSubGroup *group, CGuiElement *guiElem );
	void BuildGuiElements( CScriptSubGroup *group, CGuiElement *pParent, CScriptParser *defaultParams );
	void BuildSingleGuiElement( const std::string &strType, CScriptSubGroup *elem, CGuiElement *pParent, CScriptParser *defaultParams );
	Anchors ParseAnchors( CScriptSubGroup * elem );
};

} //namespace gui

#endif
