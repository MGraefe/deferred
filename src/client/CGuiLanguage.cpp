// client/CGuiLanguage.cpp
// client/CGuiLanguage.cpp
//CGuiLanguage.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "CGuiLanguage.h"
#include <util/CScriptParser.h>
#include <filesystem/include.h>
#include <util/StringUtils.h>

namespace gui
{

static const char *g_languageFile = "scripts/languages.rps";

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool _parseLangInfo( CScriptSubGroup &group, const std::string &groupName, std::string &fileName, std::string &realName )
{
	if( !group.GetString("name", realName) ||
		!group.GetString("file", fileName) )
	{
		error("Error parsing language file '%s', Group '%s' is not complete.", g_languageFile, groupName.c_str());
		return false;
	}

	fileName = StrUtils::getPathFromFilePath(g_languageFile, true) + fileName;
	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CGuiLanguage & CGuiLanguage::GetInst()
{
	static CGuiLanguage lang;
	return lang;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CGuiLanguage::CGuiLanguage()
{

}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::vector<CGuiLanguage::language_desc_t> CGuiLanguage::getLanguageList() const
{
	filesystem::Filesystem &fs = filesystem::Filesystem::GetInstance();
	filesystem::findFileInfo_t info;

	std::vector<CGuiLanguage::language_desc_t> result;

	CScriptParser langlist;
	if(!langlist.ParseFile(g_languageFile))
		error("Unable to read language file '%s'", g_languageFile);
	else
	{
		for(auto &p : *langlist.GetSubGroupMap())
		{
			CGuiLanguage::language_desc_t desc;
			desc.internalName = p.first;
			if(_parseLangInfo(*p.second, p.first, desc.fileName, desc.realName))
				result.push_back(desc);
		}
	}

	return result;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CGuiLanguage::parseLang( const std::string &internalName )
{
	m_stringMap.clear();
	m_currentLang.fileName.clear();
	m_currentLang.internalName.clear();
	m_currentLang.realName.clear();

	CScriptParser listParser;
	if(!listParser.ParseFile(g_languageFile))
	{
		error("Error parsing '%s'", g_languageFile);
		return false;
	}
	
	CScriptSubGroup *langGroup = listParser.GetSubGroup(internalName.c_str());
	if(!langGroup)
	{
		error("Language '%s' not found!", internalName.c_str());
		return false;
	}

	std::string filename, realname;
	if(!_parseLangInfo(*langGroup, internalName, filename, realname))
		return false;

	CScriptParser parser;
	if(!parser.ParseFile(filename))
	{
		error("Error parsing language file '%s'", filename.c_str());
		return false;
	}

	m_stringMap.insert(parser.GetKeyValueMap()->begin(), parser.GetKeyValueMap()->end());
	m_currentLang.fileName = filename;
	m_currentLang.internalName = internalName;
	m_currentLang.realName = realname;
	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const std::string & CGuiLanguage::localize( const std::string &name ) const
{
	auto it = m_stringMap.find(name);
	if(it != m_stringMap.end())
		return it->second;
	else
		return name;
}

}