// client/CGuiPingDisplay.h
// client/CGuiPingDisplay.h
// client/CGuiPingDisplay.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CGuiPingDisplay.h

#pragma once
#ifndef deferred__client__CGuiPingDisplay_H__
#define deferred__client__CGuiPingDisplay_H__

#include "CGuiHudElement.h"

class CDynamicText;

namespace gui
{

class CGuiPingDisplay : public CGuiHudElement
{
	DECLARE_GUI_DATADESC( CGuiHudElement, CGuiPingDisplay );
public:
	void VRenderCustom( void );
	void VInit( void );

private:
	CDynamicText *m_pText;
	Vector3f m_vTextPos;
	float m_fLastUpdateTime;
	int m_fps;
	int m_lastFrameCount;
	UINT m_lastBytesReceived;
	UINT m_lastBytesSent;
	float m_bytesSent;
	float m_bytesRecv;
	float m_waterMs;
};

}

#endif // deferred__client__CGuiPingDisplay_H__
