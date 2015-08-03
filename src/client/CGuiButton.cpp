// client/CGuiButton.cpp
// client/CGuiButton.cpp
// client/CGuiButton.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CGuiButton.h"
#include "GuiCallbacks.h"


namespace gui
{

DECLARE_GUI_ELEMENT(GUI_BUTTON, CGuiButton)

DATADESC_GUI_BEGIN(CGuiButton)
	DATA_GUI_COLOR( m_colorBackground[0], "color_background" );
	DATA_GUI_COLOR( m_colorBackground[1], "color_background_h" );
	DATA_GUI_COLOR( m_colorBackground[2], "color_background_p" );
	DATA_GUI_COLOR( m_colorBorder[0], "color_border" );
	DATA_GUI_COLOR( m_colorBorder[1], "color_border_h" );
	DATA_GUI_COLOR( m_colorBorder[2], "color_border_p" );
	DATA_GUI_COLOR( m_colorText[0], "color_text" );
	DATA_GUI_COLOR( m_colorText[1], "color_text_h" );
	DATA_GUI_COLOR( m_colorText[2], "color_text_p" );

	DATA_GUI_FLOAT( m_borderWidth, "border_width" );

	DATA_GUI_STRING( m_text, "text" );
	DATA_GUI_STRING( m_textFont, "text_font" );
DATADESC_GUI_END()

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CGuiButton::CGuiButton() :
	m_borderWidth(1.0f),
	m_state(STATE_DEFAULT)
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiButton::VInit( void )
{
	CGuiElement::VInit();

	m_textObj.SetFont(m_textFont);
	m_textObj.SetText(m_text);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiButton::VRenderCustom( void )
{
	size_t st = (size_t)m_state;
	
	float sx = GetSizeX(), sy = GetSizeY(), bw = m_borderWidth;

	//Draw background
	SetDrawColor(m_colorBackground[st]);
	DrawQuad(0.0f, 0.0f, sx, sy);

	//Draw border
	SetDrawColor(m_colorBorder[st]);
	DrawQuad(0.0f, 0.0f,  sx, bw); //top
	DrawQuad(0.0f, sy-bw, sx, sy); //bottom
	DrawQuad(0.0f, 0.0f,  bw, sy); //left
	DrawQuad(sx-bw, 0.0f, sx, sy); //right

	//Text
	SetDrawColor(m_colorText[st]);
	float textX = (GetSizeX() - m_textObj.GetStringWidth()) * 0.5f;
	float textY = (GetSizeY() + m_textObj.GetStringHeight()) * 0.5f;
	m_textObj.SetInitPos(Vector3f(textX, textY, 0.0f));
	m_textObj.Render();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiButton::VRenderBackground( void )
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiButton::OnMouseMove( const Vector2f &absPos, const Vector2f &relPos )
{
	if(IsPointInside(relPos))
	{
		if(m_state != STATE_PRESSED)
			m_state = STATE_HOVERED;
	}
	else
		m_state = STATE_DEFAULT;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiButton::OnMousePressed( KeyCodes::KeyCode key, const Vector2f &absPos, const Vector2f &relPos )
{
	if(key == KeyCodes::KC_MOUSEL && IsPointInside(relPos))
	{
		m_state = STATE_PRESSED;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiButton::OnMouseReleased( KeyCodes::KeyCode key, const Vector2f &absPos, const Vector2f &relPos )
{
	if(key == KeyCodes::KC_MOUSEL && m_state == STATE_PRESSED && IsPointInside(relPos))
	{
		m_state = IsPointInside(relPos) ? STATE_HOVERED : STATE_DEFAULT;
		OnClicked();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CGuiButton::OnClicked()
{
	GUI_CALL_BUTTON_CLICKED(GetParent(), GetName());
}




} //namespace gui
