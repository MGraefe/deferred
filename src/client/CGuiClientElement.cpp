// client/CGuiClientElement.cpp
// client/CGuiClientElement.cpp
//CGuiClientElement.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CGuiClientElement.h"
#include <renderer/CGuiFactory.h>
#include "GuiCallbacks.h"
#include "CClientInterf.h"


namespace gui
{

DECLARE_GUI_ELEMENT( HUD_PARENT, CGuiClientElement );

GUI_MSGMAP_START(CGuiClientElement)
GUI_MSGMAP_END()

#define CALL_FUNC_ON_CLIENT_CHILDREN(func, ...) \
	for(CGuiElement *child : GetChildren()) \
	{ \
		CGuiClientElement *clientChild = dynamic_cast<CGuiClientElement*>(child); \
		if(clientChild) \
			clientChild->func(__VA_ARGS__); \
	}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiClientElement::HandleMouseMove( const Vector2f &absPos, const Vector2f &relPos )
{
	Vector2f rel = GetRelativePosition(relPos);
	OnMouseMove(absPos, rel);
	CALL_FUNC_ON_CLIENT_CHILDREN( HandleMouseMove, absPos, rel );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiClientElement::HandleMousePressed( KeyCodes::KeyCode key, const Vector2f &absPos, const Vector2f &relPos )
{
	Vector2f rel = GetRelativePosition(relPos);
	OnMousePressed(key, absPos, rel);
	CALL_FUNC_ON_CLIENT_CHILDREN( HandleMousePressed, key, absPos, rel );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiClientElement::HandleMouseReleased( KeyCodes::KeyCode key, const Vector2f &absPos, const Vector2f &relPos )
{
	Vector2f rel = GetRelativePosition(relPos);
	OnMouseReleased(key, absPos, rel);
	CALL_FUNC_ON_CLIENT_CHILDREN( HandleMouseReleased, key, absPos, rel );
}


} // namespace gui