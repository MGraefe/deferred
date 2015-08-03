// renderer/CGuiRealtimeDebugInterface.h
// renderer/CGuiRealtimeDebugInterface.h
//CGuiRealtimeDebugInterface.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CGuiRealtimeDebugInterface_H__
#define deferred__renderer__CGuiRealtimeDebugInterface_H__

#include "CGuiElement.h"
#include "CDynamicText.h"

class CDynamicText;

namespace gui
{

class CGuiRealtimeDebugInterface : public CGuiElement
{
public:
	DECLARE_GUI_DATADESC( CGuiElement, CGuiRealtimeDebugInterface );
public:
	CGuiRealtimeDebugInterface();

	virtual void VInit( void );
	virtual void VRenderCustom( void );
	virtual void VRenderBackground( void );

private:
	Vector3f m_textPos;
	float m_textSpacing;
	float m_textCooldown;
	CDynamicText m_text;
};

}

#endif // deferred__renderer__CGuiRealtimeDebugInterface_H__