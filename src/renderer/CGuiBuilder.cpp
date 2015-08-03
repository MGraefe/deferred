// renderer/CGuiBuilder.cpp
// renderer/CGuiBuilder.cpp
// renderer/CGuiBuilder.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CGuiBuilder.h"
#include <util/CScriptParser.h>
#include "CRenderInterf.h"
#include "renderer.h"
#include <util/StringUtils.h>

namespace gui
{

//-----------------------------------------------------------------------
// Purpose: Build a gui from a script file
//-----------------------------------------------------------------------
bool CGuiBuilder::BuildGui( const char *pScriptPath, CGuiElement *pParent )
{
	CScriptParser parser;
	if( !parser.ParseFile( pScriptPath ) )
		return false;

	CScriptParser defaultParser;
	if( !defaultParser.ParseFile("scripts/gui_defaults.rps") )
		return false;

	BuildGuiElements( &parser, pParent, &defaultParser );

	return true;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
Vector3f CGuiBuilder::GetGuiElementPos( CScriptSubGroup *elem, CGuiElement *guiElem )
{
	std::string str;
	if( !elem->GetString("position", str))
		return guiElem->GetPosition();

	std::string vals[3];
	StrUtils::GetSpaceDividedValues(str, 3, vals);


	bool r = vals[0].length() > 0 && vals[0][0] == 'r';
	bool b = vals[1].length() > 0 && vals[1][0] == 'b';

	Vector3f res;
	res.x = (float)atof(std::string(vals[0], r ? 1 : 0).c_str());
	res.y = (float)atof(std::string(vals[1], b ? 1 : 0).c_str());
	res.z = (float)atof(vals[2].c_str());

	if(r)
		res.x = guiElem->GetParentSize().x - res.x;
	if(b)
		res.y = guiElem->GetParentSize().y - res.y;

	return res;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
Vector2f CGuiBuilder::GetGuiElementSize( CScriptSubGroup *elem, CGuiElement *guiElem )
{
	std::string str;
	if( !elem->GetString("size", str))
		return guiElem->GetSize();

	std::string vals[2];
	if(! StrUtils::GetSpaceDividedValues(str, 2, vals) )
		return guiElem->GetSize();

	bool r = vals[0].length() > 0 && vals[0][0] == 'r';
	bool b = vals[1].length() > 0 && vals[1][0] == 'b';

	Vector2f res;
	res.x = (float)atof(std::string(vals[0], r ? 1 : 0).c_str());
	res.y = (float)atof(std::string(vals[1], b ? 1 : 0).c_str());
	
	if(r)
		res.x = guiElem->GetParentSize().x - res.x - guiElem->GetInitPos().x;
	if(b)
		res.y = guiElem->GetParentSize().y - res.y - guiElem->GetInitPos().y;

	return res;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiBuilder::UpdateParamsFromScript( CScriptSubGroup *group, CGuiElement *guiElem )
{
	//Get element name
	std::string name;
	if( group->GetString( "name", name) )
		guiElem->SetName( name );

	guiElem->SetInitPos( GetGuiElementPos(group, guiElem) );
	guiElem->SetInitSize( GetGuiElementSize( group, guiElem ) );
	guiElem->SetAnchors( ParseAnchors(group) );

	//Handle additional vars the element might need
	guiElem->VParseAdditionalVars( group );
}



//-----------------------------------------------------------------------
// Purpose: Create the single gui element and create its children as well
//-----------------------------------------------------------------------
void CGuiBuilder::BuildSingleGuiElement( const std::string &strType, CScriptSubGroup *elem, CGuiElement *pParent, CScriptParser *defaultParams )
{
	IGuiFactory *pFactory = GetGuiFactory( strType );
	if( !pFactory )
		return;

	//Let the factory create our element
	CGuiElement *pElem = pFactory->Create();

	//Parse defaults first
	CScriptSubGroup *defaultGroup = defaultParams->GetSubGroup(strType.c_str());
	if(defaultGroup)
		UpdateParamsFromScript(defaultGroup, pElem);

	//Update real params
	UpdateParamsFromScript(elem, pElem);

	//Add child automatically sets the child's parent
	pParent->AddChild( pElem );

	//Init the element
	pElem->VInit();

	//Build child gui elements(if any)
	BuildGuiElements( elem, pElem, defaultParams );
}


//-----------------------------------------------------------------------
// Purpose: Iterate through our sub-groups and add them
//-----------------------------------------------------------------------
void CGuiBuilder::BuildGuiElements( CScriptSubGroup *group, CGuiElement *pParent, CScriptParser *defaultParams )
{
	const SubGroupMap *pGrpMap = group->GetSubGroupMap();
	for( auto &p : *pGrpMap )
		if(p.first != "anchors")
			BuildSingleGuiElement(p.first, p.second, pParent, defaultParams );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
gui::Anchors CGuiBuilder::ParseAnchors( CScriptSubGroup * elem )
{
	Anchors anchors(ANCHOR_LEFT, ANCHOR_TOP, ANCHOR_LEFT, ANCHOR_TOP);
	CScriptSubGroup *anchorGrp = elem->GetSubGroup("anchors");
	if(!anchorGrp)
		return anchors;
	
	std::string val;
	if(anchorGrp->GetString("left", val) && val == "right")
		anchors.left = ANCHOR_RIGHT;
	if(anchorGrp->GetString("top", val) && val == "bottom")
		anchors.top = ANCHOR_BOTTOM;
	if(anchorGrp->GetString("right", val) && val == "right")
		anchors.right = ANCHOR_RIGHT;
	if(anchorGrp->GetString("bottom", val) && val == "bottom")
		anchors.bottom = ANCHOR_BOTTOM;
	return anchors;
}


} //namespace gui