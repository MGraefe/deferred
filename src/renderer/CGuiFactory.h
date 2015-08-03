// renderer/CGuiFactory.h
// renderer/CGuiFactory.h
// renderer/CGuiFactory.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__CGuiFactory_H__
#define deferred__renderer__CGuiFactory_H__

#include <map>
#include "CGuiElement.h"
#include <util/basic_types.h>
#include <util/dll.h>

namespace gui
{

class DLL IGuiFactory
{
public:
	IGuiFactory() { }
	virtual ~IGuiFactory() {}
	virtual CGuiElement *Create( void ) = 0;
};


typedef std::map<std::string, IGuiFactory*, LessThanCppStringObj> GuiDictionary;
extern DLL GuiDictionary &GetGuiDictionary( void );
extern IGuiFactory *GetGuiFactory( const std::string &strName );


template<class T>
class CGuiFactory : public IGuiFactory
{
public:
	CGuiFactory( const char *name )
	{
		GetGuiDictionary().insert( std::make_pair(std::string(name), (IGuiFactory*)this) );
	}

	CGuiElement *Create( void )
	{
		T *pObj = new T();
		return (CGuiElement*)pObj;
	}
};




#define DECLARE_GUI_ELEMENT(name,classname) \
	namespace gui { \
		static CGuiFactory<classname> classname##_Factory( #name ); \
	}


} //namespace gui

#endif
