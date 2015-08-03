// client/CGuiClientElement.h
// client/CGuiClientElement.h
//CGuiClientElement.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CGuiClientElement_H__
#define deferred__client__CGuiClientElement_H__

#include <renderer/CGuiElement.h>
#include "KeyCodes.h"
#include "CGuiMessageIdent.h"

namespace gui
{

class CGuiClientElement : public CGuiElement
{
public:
	typedef void (CGuiClientElement::*methodptr_t)(void);
	typedef std::map<MessageIdent, methodptr_t> MessageMap;

	virtual const MessageMap &GetMessageMap() const;

	void HandleMouseMove( const Vector2f &absPos, const Vector2f &relPos );
	void HandleMousePressed( KeyCodes::KeyCode key, const Vector2f &absPos, const Vector2f &relPos );
	void HandleMouseReleased( KeyCodes::KeyCode key, const Vector2f &absPos, const Vector2f &relPos );
	Vector2f GetRelativePosition(const Vector2f &pos) const { return pos - Vector2f(GetPosition().x, GetPosition().y); }

	virtual void OnMouseMove( const Vector2f &absPos, const Vector2f &relPos ) { }
	virtual void OnMousePressed( KeyCodes::KeyCode key, const Vector2f &absPos, const Vector2f &relPos ) { }
	virtual void OnMouseReleased( KeyCodes::KeyCode key, const Vector2f &absPos, const Vector2f &relPos ) { }

private:
	
};

} // namespace gui

#endif // deferred__client__CGuiClientElement_H__