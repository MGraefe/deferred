// client/CGuiPingDisplay.cpp
// client/CGuiPingDisplay.cpp
// client/CGuiPingDisplay.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "CGuiPingDisplay.h"
#include <renderer/CDynamicText.h>
#include <util/StringUtils.h>
#include "CClientInterf.h"
#include <util/networking.h>
#include <renderer/COceanFFTSolverCPU.h>

namespace gui
{

DECLARE_GUI_ELEMENT( HUD_PING_DISPLAY, CGuiPingDisplay );

DATADESC_GUI_BEGIN(CGuiPingDisplay)
	DATA_GUI_VECTOR3( m_vTextPos, "text_pos" )
DATADESC_GUI_END()


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiPingDisplay::VRenderCustom( void )
{
	if( gpGlobals->curtime > m_fLastUpdateTime + 0.3f && gpGlobals->framecount > 0 )
	{
		float elapsed = gpGlobals->curtime - m_fLastUpdateTime;
		m_fps = ROUND( (float)(gpGlobals->framecount - m_lastFrameCount) / elapsed );

		UINT totalBytesRecv = g_ClientInterf->GetClientSocket()->getBytesReceived();
		UINT totalBytesSent = g_ClientInterf->GetClientSocket()->getBytesSent();
		m_bytesRecv = (float)(totalBytesRecv - m_lastBytesReceived) / elapsed;
		m_bytesSent = (float)(totalBytesSent - m_lastBytesSent) / elapsed;
		m_lastBytesReceived = totalBytesRecv;
		m_lastBytesSent = totalBytesSent;

		m_fLastUpdateTime = gpGlobals->curtime;
		m_lastFrameCount = gpGlobals->framecount;
	}

	char pchMsg[128];
	snprintf( pchMsg, 128, "rtt: %i ms, ping: %i ms, fps: %i, in: %.2f kb/s, out: %.2f kb/s, water: %.2f ms",
		ROUND(gpGlobals->rtt*1000.0f),
		ROUND(gpGlobals->ping*1000.0f),
		m_fps,
		m_bytesRecv/1024.0f,
		m_bytesSent/1024.0f,
		g_oceanFFTSolverTimeMs);

	m_pText->SetDrawColor( (BYTE)255, 255, 255, 255 );
	m_pText->SetText( pchMsg );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiPingDisplay::VInit( void )
{
	m_pText = new CDynamicText();
	m_pText->SetInitPos( m_vTextPos );
	m_pText->SetFont( "FontSmallText" );
	AddChild( m_pText );
	m_fLastUpdateTime = 0.0f;
	m_fps = 0;
	m_lastFrameCount = 0;
	m_lastBytesSent = 0;
	m_lastBytesReceived = 0;
}


} // namespace gui
