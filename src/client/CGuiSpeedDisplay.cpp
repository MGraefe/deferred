// client/CGuiSpeedDisplay.cpp
// client/CGuiSpeedDisplay.cpp
// client/CGuiSpeedDisplay.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CGuiSpeedDisplay.h"
#include <renderer/CFont.h>
#include "CClientPlayerCar.h"
#include <renderer/CDynamicText.h>
#include "CClientInterf.h"
#include <util/IRealtimeDebugInterface.h>

namespace gui
{

DECLARE_GUI_ELEMENT( HUD_SPEED_DISPLAY, CGuiSpeedDisplay );


//-----------------------------------------------------------------------
// Purpose: All custom drawing code here
//-----------------------------------------------------------------------
void CGuiSpeedDisplay::VRenderCustom( void )
{
	SetDrawColor( 1.0f, 1.0f, 1.0f, 1.0f );
	
	//Set Texts
	CClientPlayerCar *pCar = GetLocalPlayerCar();
	if( pCar )
	{
		int rpm = (int)pCar->GetRpm();
		char pchRpm[8];
		_itoa( rpm, pchRpm, 10 );
		m_pText1->SetText( pchRpm );

		int gear = pCar->GetGear();
		char pchGear[4];
		_itoa( gear, pchGear, 10 );
		m_pText2->SetText( pchGear );

		float velKmh = pCar->GetSpeedKmh();
		g_RenderInterf->GetRealtimeDebugInterface()->SetValue( "car.velocity", std::to_string(velKmh).c_str() );
		char pchVelKmh[8];
		_itoa( (int)velKmh, pchVelKmh, 10 );
		m_pText3->SetText( pchVelKmh );
	}
}


//-----------------------------------------------------------------------
// Purpose: Called after ParseAdditionalVars()
//-----------------------------------------------------------------------
void CGuiSpeedDisplay::VInit( void )
{
	m_pText1 = new CDynamicText();
	m_pText1->SetInitPos( m_vText1Pos );
	m_pText1->SetFont( "FontLargeText" );
	AddChild( m_pText1 );

	m_pText2 = new CDynamicText();
	m_pText2->SetInitPos( m_vText2Pos );
	m_pText2->SetFont( "FontLargeText" );
	AddChild( m_pText2 );

	m_pText3 = new CDynamicText();
	m_pText3->SetInitPos( m_vText3Pos );
	m_pText3->SetFont( "FontLargeText" );
	AddChild( m_pText3 );
}


//-----------------------------------------------------------------------
// Purpose: Called right after this element's creation
//-----------------------------------------------------------------------
// void CGuiSpeedDisplay::VParseAdditionalVars( CScriptSubGroup *vars )
// {
// 	m_vText1Pos = vars->GetVector3f( "text1_pos" );
// 	m_vText2Pos = vars->GetVector3f( "text2_pos" );
// 
// 	//Dont forget this!
// 	BaseClass::VParseAdditionalVars( vars );
// }

DATADESC_GUI_BEGIN(CGuiSpeedDisplay)
	DATA_GUI_VECTOR3( m_vText1Pos, "text1_pos" )
	DATA_GUI_VECTOR3( m_vText2Pos, "text2_pos" )
	DATA_GUI_VECTOR3( m_vText3Pos, "text3_pos" )
DATADESC_GUI_END()


} // namespace gui
