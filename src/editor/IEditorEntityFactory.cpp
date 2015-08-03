// editor/IEditorEntityFactory.cpp
// editor/IEditorEntityFactory.cpp
// editor/IEditorEntityFactory.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "IEditorEntityFactory.h"
#include <string>

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
EditorEntityFactoryList &IEditorEntityFactory::GetList( void )
{
	static EditorEntityFactoryList list;
	return list;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
IEditorEntityFactory::IEditorEntityFactory( const char *type )
{
	std::string sType(type);
	auto list = GetList();
	if( list.find(sType) != list.end() )
		error_fatal( "Type %s is already there, have a look into your Factory Registrations!!" );

	GetList()[sType] = this;
}

