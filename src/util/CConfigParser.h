// util/CConfigParser.h
// util/CConfigParser.h
// util/CConfigParser.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CConfigParser.h

#pragma once
#ifndef deferred__util__CConfigParser_H__
#define deferred__util__CConfigParser_H__


class ConsoleHandler;

class CConfigParser
{
public:
	CConfigParser();
	bool ParseConfigFile(const char *filename, ConsoleHandler *consoleHandler);
};

#endif // deferred__util__CConfigParser_H__
