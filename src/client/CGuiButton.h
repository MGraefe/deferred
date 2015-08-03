// client/CGuiButton.h
// client/CGuiButton.h
// client/CGuiButton.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CGuiButton_H__
#define deferred__client__CGuiButton_H__

#include "CGuiTextLocalized.h"
#include "GuiCallbacks.h"
#include "CGuiClientElement.h"

class CDynamicText;

namespace gui
{

class CGuiButton : public CGuiClientElement
{
	DECLARE_GUI_DATADESC( CGuiClientElement, CGuiButton );

public:
	CGuiButton();

	virtual void VInit( void ) override;
	virtual void VRenderCustom( void ) override;
	virtual void VRenderBackground( void ) override;

	virtual void OnMouseMove( const Vector2f &absPos, const Vector2f &relPos ) override;
	virtual void OnMousePressed( KeyCodes::KeyCode key, const Vector2f &absPos, const Vector2f &relPos ) override;
	virtual void OnMouseReleased( KeyCodes::KeyCode key, const Vector2f &absPos, const Vector2f &relPos ) override;
	void OnClicked();
	enum buttonstate_e
	{
		STATE_DEFAULT,
		STATE_HOVERED,
		STATE_PRESSED,
	};

private:
	// default / hovered / pressed
	Color m_colorBackground[3];
	Color m_colorBorder[3];
	Color m_colorText[3];

	std::string m_text;
	std::string m_textFont;

	float m_borderWidth;

	buttonstate_e m_state;	
	CGuiTextLocalized m_textObj;
};

}

#endif // deferred__client__CGuiButton_H__