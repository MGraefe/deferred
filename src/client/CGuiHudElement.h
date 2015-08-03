// client/CGuiHudElement.h
// client/CGuiHudElement.h
// client/CGuiHudElement.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__client__CGuiHudElement_H__
#define deferred__client__CGuiHudElement_H__

#include "CGuiClientElement.h"

namespace gui
{

//Base class for hud-elements
class CGuiHudElement : public CGuiClientElement
{
	DECLARE_GUI_DATADESC( CGuiClientElement, CGuiHudElement );
public:
	virtual void VInit( void );
	virtual void VRenderCustom( void );
	virtual void VRenderBackground( void );
};

}

#endif
