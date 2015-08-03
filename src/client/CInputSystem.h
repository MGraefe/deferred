// client/CInputSystem.h
// client/CInputSystem.h
// client/CInputSystem.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CInputSystem_H__
#define deferred__client__CInputSystem_H__


#include "KeyCodes.h"

class IWindow;
class CInputEventHandler;
class CEventManager;
namespace OIS
{
	class InputManager;
	class Keyboard;
	class Mouse;
} 

class CInputSystem
{
public:
	CInputSystem();
	~CInputSystem();
	void Init(IWindow *window, CEventManager *buttonEventReceiver);
	void Shutdown();
	void Update();

private:
	IWindow *m_window;
	OIS::InputManager *m_inputManager;
	OIS::Keyboard *m_keyboard;
	OIS::Mouse *m_mouse;
	CInputEventHandler *m_eventHandler;
};


#endif