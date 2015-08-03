// client/CGuiHudElement.cpp
// client/CGuiHudElement.cpp
// client/CGuiHudElement.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "CGuiHudElement.h"

namespace gui
{

DATADESC_GUI_BEGIN(CGuiHudElement)
DATADESC_GUI_END()


//-----------------------------------------------------------------------
// Purpose: Called after ParseAdditionalVars()
//-----------------------------------------------------------------------
void CGuiHudElement::VInit( void )
{

}


//-----------------------------------------------------------------------
// Purpose: All custom drawing code here
//-----------------------------------------------------------------------
void CGuiHudElement::VRenderCustom( void )
{

}


//-----------------------------------------------------------------------
// Purpose: Called before VRenderCustom
//-----------------------------------------------------------------------
void CGuiHudElement::VRenderBackground( void )
{
	BaseClass::VRenderBackground();

	SetDrawColor( 0.0f, 0.0f, 0.0f, 0.4f );
	
	const Vector2f &v = GetSize();

	DrawQuad( 0.0f, 0.0f, v.x, v.y );
}


} // namespace gui
