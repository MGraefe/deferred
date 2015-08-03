// client/CGuiEventHandler.cpp
// client/CGuiEventHandler.cpp
//CGuiEventHandler.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CGuiEventHandler.h"
#include "CClientInterf.h"
#include <util/CEventManager.h>
#include <util/eventlist.h>
#include "CGuiClientElement.h"

namespace gui
{

#define CALL_ON_GUI_CHILDREN(func, ...) \
	do { \
		CGuiElement *guiRoot = GetGuiRoot(); \
		if(!guiRoot) \
			break; \
		for(CGuiElement *child : guiRoot->GetChildren()) \
		{ \
			CGuiClientElement *clientChild = dynamic_cast<CGuiClientElement*>(child); \
			if(clientChild) \
				clientChild->func(__VA_ARGS__); \
		} \
	} while(false)

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CGuiEventHandler::HandleEvent( const IEvent *evt )
{
	if(evt->GetType() == ev::MOUSE_POS_UPDATE)
	{
		const CMouseMoveEvent *mmevt = dynamic_cast<const CMouseMoveEvent*>(evt);
		Assert(mmevt);
		Vector2f pos(mmevt->GetX(), mmevt->GetY());
		CALL_ON_GUI_CHILDREN(HandleMouseMove, pos, pos);
		return true;
	}

	if(evt->GetType() == ev::MOUSE_PRESSED || evt->GetType() == ev::MOUSE_RELEASED)
	{
		const CMouseButtonEvent *mbevt = dynamic_cast<const CMouseButtonEvent*>(evt);
		Assert(mbevt);
		Vector2f pos(mbevt->GetX(), mbevt->GetY());
		KeyCodes::KeyCode kc = (KeyCodes::KeyCode)mbevt->GetKeyCode();
		if(evt->GetType() == ev::MOUSE_PRESSED)
			CALL_ON_GUI_CHILDREN(HandleMousePressed, kc, pos, pos);
		else
			CALL_ON_GUI_CHILDREN(HandleMouseReleased, kc, pos, pos);
	}
	return false;
}

const EventType g_events[] = {ev::MOUSE_POS_UPDATE, ev::MOUSE_PRESSED, ev::MOUSE_RELEASED};

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiEventHandler::UnregisterEvents( void )
{
	for(size_t i = 0; i < ARRAY_LENGTH(g_events); ++i)
		g_ClientInterf->GetClientEventMgr()->UnregisterListener(g_events[i], this);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiEventHandler::Init()
{
	for(size_t i = 0; i < ARRAY_LENGTH(g_events); ++i)
		g_ClientInterf->GetClientEventMgr()->RegisterListener(g_events[i], this);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CGuiElement *CGuiEventHandler::GetGuiRoot()
{
	IRenderer *renderer = g_ClientInterf->GetRenderer();
	if(!renderer)
		return NULL;
	return renderer->GetGuiRoot();
}

}