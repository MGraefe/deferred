// util/CUnitTest.h
// util/CUnitTest.h
// util/CUnitTest.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__CUnitTest_H__
#define deferred__util__CUnitTest_H__

#include <string>
#include <vector>
using std::string;

class CUnitTest
{
public:
	CUnitTest( const string &name );
	const string &GetName( void ) { return m_Name; }
	virtual bool RunTests( void ) = 0;
private:
	string m_Name;
};


typedef std::vector<CUnitTest*> UnitTestList;
class CUnitTestList
{
public:
	static void RunAllTests( void );
	static void AddTestToList( CUnitTest *pTest );

private:
	static UnitTestList &GetTestList( void );
};

#endif
