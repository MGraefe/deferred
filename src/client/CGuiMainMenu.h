// client/CGuiMainMenu.h
// client/CGuiMainMenu.h
//CGuiMainMenu.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CGuiMainMenu_H__
#define deferred__client__CGuiMainMenu_H__

#include "CGuiClientElement.h"
#include "GuiCallbacks.h"

namespace gui
{
class CGuiMainMenu : public CGuiClientElement
{
	GUI_MSGMAP_DECL();
	DECLARE_GUI_DATADESC(CGuiClientElement, CGuiMainMenu);

public:	
	virtual void VInit( void ) override;
	virtual void VRenderCustom() override;
	virtual void VRenderBackground() override;

	void OnClickedNewGame();
	void OnClickedConnect();
	void OnClickedDisconnect();
	void OnClickedOptions();
	void OnClickedQuitGame();

private:
	Color m_bgColor;
};
}
#endif // deferred__client__CGuiMainMenu_H__