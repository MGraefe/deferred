// client/CGuiLanguage.h
// client/CGuiLanguage.h
//CGuiLanguage.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__client__CGuiLanguage_H__
#define deferred__client__CGuiLanguage_H__


namespace gui
{

class CGuiLanguage
{
public:
	struct language_desc_t
	{
		std::string internalName;
		std::string realName;
		std::string fileName;
	};

public:
	static CGuiLanguage &GetInst();

	CGuiLanguage();
	std::vector<language_desc_t> getLanguageList() const;
	bool parseLang(const std::string &internalName);
	const std::string &localize(const std::string &name) const;
	const language_desc_t &GetCurrentLang() const { return m_currentLang; }

private:
	std::map<std::string, std::string> m_stringMap;
	language_desc_t m_currentLang;
};

}

#endif // deferred__client__CGuiLanguage_H__