// util/CConfigParser.cpp
// util/CConfigParser.cpp
// util/CConfigParser.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CScriptParser.h"
#include "ConsoleHandler.h"
#include "CConVar.h"
#include "CConfigParser.h"


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CConfigParser::CConfigParser()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CConfigParser::ParseConfigFile( const char *filename, ConsoleHandler *consoleHandler )
{
	CScriptParser parser;
	if( !parser.ParseFile(filename) )
	{
		error("CConfigParser: File not found: %s", filename);
		return false;
	}

	const KeyValueMap *kvMap = parser.GetKeyValueMap();
	KeyValueMap::const_iterator it = kvMap->begin();
	KeyValueMap::const_iterator itEnd = kvMap->end();

	int count = 0;

	for(; it != itEnd; ++it)
	{
		const std::string &key = it->first;
		const std::string &val = it->second;

		CConVarBase *baseVar = consoleHandler->GetConVarBase(key);
		if( !baseVar )
		{
			new CConVar(key, val);
#ifdef _DEBUG
			ConsoleMessage("CConfigParser: adding previously unknown ConVar \"%s\" with val \"%s\"",
				key.c_str(), val.c_str() );
#endif
			count++;
		}
		else if( !baseVar->IsConVar() )
		{
			error("CConfigParser: \"%s\" is already defined but not as ConVar (most likely as ConCommand), ignoring...",
				key.c_str());
		}
		else
		{
			CConVar *conVar = (CConVar*)baseVar;
			conVar->SetValue(val);
			count++;
		}
	}

	ConsoleMessage("CConfigParser: Successfully parsed %i Console-Variables from \"%s\".", count, filename);
	return true;
}
