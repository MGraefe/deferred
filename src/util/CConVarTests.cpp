// util/CConVarTests.cpp
// util/CConVarTests.cpp
// util/CConVarTests.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "CUnitTest.h"
#include "CConVar.h"
#include "error.h"


class CConVarTester : public CUnitTest
{
public:
	CConVarTester();
	bool RunTests( void );
private:
	static void PrintCVarInfo( const CConVar &var );
};

CConVarTester g_convartester;

//---------------------------------------------------------------
// Purpose: TESTS
//--------------------------------------------------------------
CConVarTester::CConVarTester() :
	CUnitTest( string("CConVar Test") )
{

}



bool CConVarTester::RunTests( void )
{
	ConsoleMessage( "\n-----------------------------\nCConVar Tests\n-----------------------------" );
	CConVar &cvar_test1 = *CConVarList::GetConVar( "cvar_test1" );
	CConVar &cvar_test2 = *CConVarList::GetConVar( "cvar_test2" );
	CConVar &cvar_test3 = *CConVarList::GetConVar( "cvar_test3" );
	CConVar &cvar_test4 = *CConVarList::GetConVar( "cvar_test4" );

	PrintCVarInfo( cvar_test1 );
	PrintCVarInfo( cvar_test2 );
	PrintCVarInfo( cvar_test3 );
	PrintCVarInfo( cvar_test4 );

	ConsoleMessage( "Setting cvar_test1 to string \"asdf\"..." );
	cvar_test1.SetValue( "asdf" );
	PrintCVarInfo( cvar_test1 );

	ConsoleMessage( "Setting cvar_test1 to string \"123.256\"..." );
	cvar_test1.SetValue( "123.256" );
	PrintCVarInfo( cvar_test1 );

	ConsoleMessage( "Setting cvar_test1 to float 25.12f..." );
	cvar_test1.SetValue( 25.12f );
	PrintCVarInfo( cvar_test1 );

	ConsoleMessage( "Setting cvar_test1 to int 23..." );
	cvar_test1.SetValue( 23 );
	PrintCVarInfo( cvar_test1 );

	ConsoleMessage( "Setting cvar_test1 to bool FALSE" );
	cvar_test1.SetValue( false );
	PrintCVarInfo( cvar_test1 );

	ConsoleMessage( "Setting cvar_test1 to bool TRUE" );
	cvar_test1.SetValue( true );
	PrintCVarInfo( cvar_test1 );

	ConsoleMessage( "Parsing Command cvar_test1 = 2" );
	CConVarList::ParseConsoleInputString( "\t cvar_test1 2" );
	PrintCVarInfo( cvar_test1 );

	ConsoleMessage( "Parsing Command cvar_test1 = \"3\"" );
	CConVarList::ParseConsoleInputString( "cvar_test1 \"3\"" );
	PrintCVarInfo( cvar_test1 );

	return true;
}


void CConVarTester::PrintCVarInfo( const CConVar &var )
{
	ConsoleMessage( "ConVar \"%s\" (\"%s\") has string: \"%s\", int %i, float %f, bool %s\n	and was %s (default: %s)", 
		var.GetName().c_str(),
		var.GetHelpString().c_str(),
		var.GetString().c_str(),
		var.GetInt(),
		var.GetFloat(),
		var.GetBool() ? "TRUE" : "FALSE",
		var.IsNumeric() ? "numeric" : "not numeric",
		var.IsDefaultNumeric() ? "numeric" : "not numeric" );
}
