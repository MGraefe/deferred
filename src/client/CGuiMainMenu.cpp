// client/CGuiMainMenu.cpp
// client/CGuiMainMenu.cpp
//CGuiMainMenu.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------



#include "stdafx.h"

#include "CGuiMainMenu.h"
#include "CClientInterf.h"
#include <renderer/CGuiFactory.h>

namespace gui
{

DECLARE_GUI_ELEMENT(GUI_MAINMENU, CGuiMainMenu)

GUI_MSGMAP_START(CGuiMainMenu)
	GUI_BUTTON_CLICKED("mm_b_newgame", &CGuiMainMenu::OnClickedNewGame)
	GUI_BUTTON_CLICKED("mm_b_joingame", &CGuiMainMenu::OnClickedConnect)
	GUI_BUTTON_CLICKED("mm_b_disconnect", &CGuiMainMenu::OnClickedDisconnect)
	GUI_BUTTON_CLICKED("mm_b_options", &CGuiMainMenu::OnClickedOptions)
	GUI_BUTTON_CLICKED("mm_b_quitgame", &CGuiMainMenu::OnClickedQuitGame)
GUI_MSGMAP_END()


DATADESC_GUI_BEGIN(CGuiMainMenu)
	DATA_GUI_COLOR(m_bgColor, "color_background");
DATADESC_GUI_END()


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiMainMenu::VInit( void )
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiMainMenu::VRenderCustom()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiMainMenu::VRenderBackground()
{
	SetDrawColor(m_bgColor);
	DrawQuad(0.0f, 0.0f, GetSizeX(), GetSizeY());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiMainMenu::OnClickedNewGame()
{
	ConsoleMessage("New game was pressed");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiMainMenu::OnClickedConnect()
{
	ConsoleMessage("Connect was pressed");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiMainMenu::OnClickedDisconnect()
{
	ConsoleMessage("Disconnect was pressed");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiMainMenu::OnClickedOptions()
{
	ConsoleMessage("Options was pressed");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiMainMenu::OnClickedQuitGame()
{
	g_ClientInterf->SetQuitGameState(true);
}


}