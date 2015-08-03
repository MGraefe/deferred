// renderer/CGuiFactory.cpp
// renderer/CGuiFactory.cpp
// renderer/CGuiFactory.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CGuiFactory.h"
#include <util/error.h>

namespace gui
{

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
GuiDictionary &GetGuiDictionary( void )
{
	static GuiDictionary Static_guiDict;
	return Static_guiDict;
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
IGuiFactory *GetGuiFactory( const std::string &strName )
{
	GuiDictionary::const_iterator it = GetGuiDictionary().find( strName );
	if( it == GetGuiDictionary().end() )
	{
		error(	"Could not find Gui-Element \"%s\"." \
				"Did you forget DECLARE_GUI_ELEMENT inside the .cpp?",
				strName.c_str() );
		return NULL;
	}

	return it->second;
}

} //namespace gui