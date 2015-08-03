// client/CInputSystem.cpp
// client/CInputSystem.cpp
// client/CInputSystem.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CInputSystem.h"
#include <renderer/IWindow.h>
#include <util/CEventManager.h>
#include <util/CConVar.h>
#include <ostream>
#include <OIS.h>

CConVar input_debug("input.debug", "0");

const char *g_DeviceType[6] = {"OISUnknown", "OISKeyboard", "OISMouse", "OISJoyStick",
							 "OISTablet", "OISOther"};


KeyCodes::KeyCode translateKey(OIS::KeyCode oisKey)
{
	Assert(oisKey >= OIS::KeyCode::KC_UNASSIGNED);
	Assert(oisKey <= OIS::KeyCode::KC_MEDIASELECT);
	return (KeyCodes::KeyCode)oisKey;
}


KeyCodes::KeyCode translateKey(OIS::MouseButtonID oisKey)
{
	static_assert(OIS::MouseButtonID::MB_Left == 0, "Rewrite OIS Mouse-Button Translation!");
	Assert(oisKey >= 0 && oisKey <= 7);
	return (KeyCodes::KeyCode)((int)oisKey + (int)KeyCodes::KC_MOUSEL);
}


//#################################################################
// CInputSystem
//#################################################################
class CInputEventHandler : public OIS::KeyListener, public OIS::MouseListener
{
public:
	CInputEventHandler(CEventManager *manager)
	{
		m_manager = manager;
	}

	bool keyPressed( const OIS::KeyEvent &arg )
	{
		m_manager->AddEventToQueue(new CIntegerDataEvent(ev::KEYBOARD_PRESSED, translateKey(arg.key)));

		if(input_debug.GetBool())
			std::cout << " KeyPressed {" << arg.key
				<< ", " << ((OIS::Keyboard*)(arg.device))->getAsString(arg.key)
				<< "} || Character (" << (char)arg.text << ")" << std::endl;
		return true;
	}

	bool keyReleased( const OIS::KeyEvent &arg )
	{
		m_manager->AddEventToQueue(new CIntegerDataEvent(ev::KEYBOARD_RELEASED, translateKey(arg.key)));

		if(input_debug.GetBool())
			std::cout << "KeyReleased {" << ((OIS::Keyboard*)(arg.device))->getAsString(arg.key) << "}\n";
		return true;
	}

	bool mouseMoved( const OIS::MouseEvent &arg )
	{
		const OIS::MouseState& s = arg.state;
		m_manager->AddEventToQueue(new CMouseMoveEvent(ev::MOUSE_POS_UPDATE, s.X.abs, s.Y.abs));

		if(input_debug.GetBool())
			std::cout << "\nMouseMoved: Abs("
					  << s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
					  << s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
		return true;
	}

	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		const OIS::MouseState& s = arg.state;
		m_manager->AddEventToQueue(new CMouseButtonEvent(ev::MOUSE_PRESSED, translateKey(id), s.X.abs, s.Y.abs));

		if(input_debug.GetBool())
			std::cout << "\nMouse button #" << id << " pressed. Abs("
					  << s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
					  << s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
		return true;
	}

	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
	{
		const OIS::MouseState& s = arg.state;
		m_manager->AddEventToQueue(new CMouseButtonEvent(ev::MOUSE_RELEASED, translateKey(id), s.X.abs, s.Y.abs));

		if(input_debug.GetBool())
			std::cout << "\nMouse button #" << id << " released. Abs("
					  << s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
					  << s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
		return true;
	}

private:
	CEventManager *m_manager;
};


//#################################################################
// CInputSystem
//#################################################################
CInputSystem::CInputSystem()
{
	m_window = NULL;
	m_inputManager = NULL;
	m_keyboard = NULL;
	m_mouse = NULL;
	m_eventHandler = NULL;
}


CInputSystem::~CInputSystem()
{
	Shutdown();
}


void CInputSystem::Shutdown()
{
	if( m_inputManager )
	{
		OIS::InputManager::destroyInputSystem(m_inputManager);
		m_inputManager = NULL;
	}

	if( m_eventHandler )
	{
		delete m_eventHandler;
		m_eventHandler = NULL;
	}

	m_keyboard = NULL;
	m_mouse = NULL;
	m_window = NULL;
}


void CInputSystem::Init(IWindow *window, CEventManager *buttonEventReceiver)
{
	m_window = window;
	m_eventHandler = new CInputEventHandler(buttonEventReceiver);

	OIS::ParamList pl;
	std::ostringstream wndStr;

	size_t sysRef = (size_t)m_window->GetWindowSystemReference();
	wndStr << sysRef;
	pl.insert(std::make_pair(std::string("WINDOW"), wndStr.str()));
	
	if(!window->IsFullscreen())
	{
#ifdef _WINDOWS
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
#else
		pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
		//pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
		pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
	}

	m_inputManager = OIS::InputManager::createInputSystem(pl);
	
	if(input_debug.GetBool())
	{
		//Print debugging information
		unsigned int v = m_inputManager->getVersionNumber();
		std::cout << "OIS Version: " << (v>>16 ) << "." << ((v>>8) & 0x000000FF) << "." << (v & 0x000000FF)
			<< "\nRelease Name: " << m_inputManager->getVersionName()
			<< "\nManager: " << m_inputManager->inputSystemName()
			<< "\nTotal Keyboards: " << m_inputManager->getNumberOfDevices(OIS::OISKeyboard)
			<< "\nTotal Mice: " << m_inputManager->getNumberOfDevices(OIS::OISMouse);

		//List all devices
		OIS::DeviceList list = m_inputManager->listFreeDevices();
		for( OIS::DeviceList::iterator i = list.begin(); i != list.end(); ++i )
			std::cout << "\n\tDevice: " << g_DeviceType[i->first] << " Vendor: " << i->second;
	}

	m_keyboard = (OIS::Keyboard*)m_inputManager->createInputObject(OIS::OISKeyboard, true);
	m_keyboard->setEventCallback(m_eventHandler);

	m_mouse = (OIS::Mouse*)m_inputManager->createInputObject(OIS::OISMouse, true);
	m_mouse->setEventCallback(m_eventHandler);
	m_mouse->getMouseState().width = m_window->GetWidth();
	m_mouse->getMouseState().height = m_window->GetHeight();
}


void CInputSystem::Update()
{
	if( m_keyboard )
		m_keyboard->capture();
	if( m_mouse )
	{
		m_mouse->getMouseState().width = m_window->GetWidth();
		m_mouse->getMouseState().height = m_window->GetHeight();
		m_mouse->capture();
	}
}