// util/CmdLineParser.h
// util/CmdLineParser.h
//CmdLineParser.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__CmdLineParser_H__
#define deferred__util__CmdLineParser_H__

#include "basic_types.h"

class CmdLineParser
{
public:
	CmdLineParser();
	CmdLineParser(const char *cmdLine);
	void Parse(const char *cmdLine);

	const char *GetCharParam(const char *name, const char *def);
	int GetIntParam(const char *name, int def);
	float GetFloatParam(const char *name, float def);

private:
	std::map<std::string, std::string, LessThanCppStringObj> m_params;
};

#endif // deferred__util__CmdLineParser_H__