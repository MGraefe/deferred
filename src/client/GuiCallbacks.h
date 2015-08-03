// client/GuiCallbacks.h
// client/GuiCallbacks.h
// client/GuiCallbacks.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__GuiCallbacks_H__
#define deferred__client__GuiCallbacks_H__

#include <string>


#include "CGuiMessageIdent.h"
#include "CGuiClientElement.h"

namespace gui
{


enum messageid_e
{
	ID_NONE = 0,
	ID_BUTTON_CLICKED,
	ID_LAST,
};


typedef void (CGuiClientElement::*functype_void)();

#define GUI_MSGMAP_DECL() public: virtual const CGuiClientElement::MessageMap &GetMessageMap() const override;

#define GUI_MSGMAP_START(classname) \
	class classname##_messageMapConstructor { \
	public: \
		classname##_messageMapConstructor(); \
		CGuiClientElement::MessageMap m_msgmap; \
	}; \
	classname##_messageMapConstructor classname##_messageMapConstructor_inst; \
	const CGuiClientElement::MessageMap &classname::GetMessageMap() const { return classname##_messageMapConstructor_inst.m_msgmap; } \
	classname##_messageMapConstructor::classname##_messageMapConstructor() {

#define GUI_MSGMAP_END() }

#define GUI_COMMAND(name, msg, functype, func) \
	static_assert(sizeof(CGuiClientElement::methodptr_t) == sizeof(functype), "Unmatching pointer sizes..."); \
	m_msgmap[MessageIdent(name, (int)msg)] = reinterpret_cast<CGuiClientElement::methodptr_t>(static_cast<functype>(func));

#define GUI_CALL_COMMAND(name, msg, functype, obj, ...) \
	do { \
		if(!obj) break; \
		CGuiClientElement *clElem = dynamic_cast<CGuiClientElement*>(obj); \
		if(!clElem) break; \
		auto it = clElem->GetMessageMap().find(MessageIdent(name, (int)msg)); \
		if( it != clElem->GetMessageMap().end() ) \
		{ \
			functype ptr = reinterpret_cast<functype>(it->second); \
			(*clElem.*ptr)(__VA_ARGS__); \
		} \
	} \
	while(false)

#define GUI_BUTTON_CLICKED(name, func) GUI_COMMAND(name, ID_BUTTON_CLICKED, functype_void, func)
#define GUI_CALL_BUTTON_CLICKED(obj, name) GUI_CALL_COMMAND(name, ID_BUTTON_CLICKED, functype_void, obj)

}

#endif // deferred__client__GuiCallbacks_H__