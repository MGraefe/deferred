// renderer/CGuiRealtimeDebugInterface.cpp
// renderer/CGuiRealtimeDebugInterface.cpp
//CGuiRealtimeDebugInterface.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CGuiRealtimeDebugInterface.h"
#include "CDynamicText.h"
#include "CRealtimeDebugInterface.h"
#include <util/CConVar.h>

CConVar debug_realtime("debug.realtime", "0");

DECLARE_GUI_ELEMENT( GUI_REALTIMEDEBUGINTERFACE, CGuiRealtimeDebugInterface );

DATADESC_GUI_BEGIN(gui::CGuiRealtimeDebugInterface)
	DATA_GUI_VECTOR3(m_textPos, "text_pos");
	DATA_GUI_FLOAT(m_textSpacing, "text_spacing");
	DATA_GUI_FLOAT(m_textCooldown, "text_cooldown");
DATADESC_GUI_END()

namespace gui
{



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CGuiRealtimeDebugInterface::CGuiRealtimeDebugInterface()
{
	m_textPos = Vector3f(10.0f, 10.0f, 0.0f);
	m_textSpacing = 10.0f;
	m_textCooldown = 1.0f;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiRealtimeDebugInterface::VInit( void )
{
	CGuiElement::VInit();

	m_text.SetInitPos( m_textPos );
	m_text.SetFont( "FontSmallText" );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiRealtimeDebugInterface::VRenderCustom( void )
{
	if( !debug_realtime.GetBool() )
		return;

	CRealtimeDebugInterface *rtdi = g_RenderInterf->GetRealtimeDebug();
	CRealtimeDebugInterface::ValueMap vals = rtdi->GetValues();

	int i = 0;
	for(auto p : vals)
	{
		m_text.SetInitPos(m_textPos + Vector3f(0.0f, (float)i * m_textSpacing, 0.0f));
		float elapsed = gpGlobals->curtime - p.second.timeChanged;
		float decay = m_textCooldown > 0.0f ? clamp(elapsed / m_textCooldown, 0.0f, 1.0f) : 1.0f;
		BYTE r = (BYTE)(lerpclamped(decay, 0.5f, 1.0f, 255.0f, 0.0f) + 0.5f);
		BYTE g = (BYTE)(lerpclamped(decay, 0.0f, 0.5f, 0.0f, 255.0f) + 0.5f);
		m_text.SetDrawColor(r, g, 0);
		m_text.SetText((p.first + " = " + p.second.value).c_str());
		m_text.Render();
		i++;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiRealtimeDebugInterface::VRenderBackground( void )
{

}




} // namespace gui