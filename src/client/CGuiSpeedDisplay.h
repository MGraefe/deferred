// client/CGuiSpeedDisplay.h
// client/CGuiSpeedDisplay.h
// client/CGuiSpeedDisplay.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__client__CGuiSpeedDisplay_H__
#define deferred__client__CGuiSpeedDisplay_H__

#include "CGuiHudElement.h"

class CDynamicText;

namespace gui
{
	
class CGuiSpeedDisplay : public CGuiHudElement
{
	DECLARE_GUI_DATADESC( CGuiHudElement, CGuiSpeedDisplay );
public:
	void VRenderCustom( void );
	void VInit( void );
	//void VParseAdditionalVars( CScriptSubGroup *vars );

private:
	Vector3f m_vText1Pos;
	Vector3f m_vText2Pos;
	Vector3f m_vText3Pos;
	CDynamicText *m_pText1;
	CDynamicText *m_pText2;
	CDynamicText *m_pText3;
};

} // namespace gui

#endif
