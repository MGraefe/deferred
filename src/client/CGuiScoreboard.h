// client/CGuiScoreboard.h
// client/CGuiScoreboard.h
// client/CGuiScoreboard.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CGuiScoreboard_H__
#define deferred__client__CGuiScoreboard_H__

#include "CGuiHudElement.h"
#include <util/IEvent.h>

class CDynamicText;

namespace gui
{

class CGuiScoreboard : public CGuiHudElement, public IEventListener
{
	DECLARE_GUI_DATADESC(CGuiHudElement, CGuiScoreboard);
public:
	CGuiScoreboard();
	void VRenderCustom();
	void VInit();
	void UpdateText();

	virtual bool IsVisible();

	virtual bool HandleEvent( const IEvent *evt );
	virtual void UnregisterEvents( void );

private:
	CDynamicText *m_text;
	Vector3f m_textPos;
	bool m_isVisible;
};

} // namespace gui

#endif // deferred__client__CGuiScoreboard_H__