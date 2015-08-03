// renderer/consoleCallback.cpp
// renderer/consoleCallback.cpp
// renderer/consoleCallback.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "consoleCallback.h"
#include <util/ConsoleHandler.h>
#include <util/CConVar.h>

NOINLINE DLL void InitConsoleInputCallbackRenderer( ConsoleHandler *pConsoleHandler )
{
	pConsoleHandler->AddCallback( &CConVarList::HandleConsoleInput, &CConVarList::GetConVarBase );
}
