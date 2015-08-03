// util/CUnitTest.cpp
// util/CUnitTest.cpp
// util/CUnitTest.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CUnitTest.h"
#include "error.h"

CUnitTest::CUnitTest( const string &name ):
	m_Name(name)
{
	CUnitTestList::AddTestToList( this );
}


UnitTestList &CUnitTestList::GetTestList( void )
{
	static UnitTestList testlist;
	return testlist;
}

void CUnitTestList::AddTestToList( CUnitTest *pTest )
{
	GetTestList().push_back(pTest);
}


void CUnitTestList::RunAllTests( void )
{
	int size = GetTestList().size();

	if( size <= 0 )
		return;

	ConsoleMessage( "\n----------------------------------\nUNIT TESTS\n----------------------------------" );

	for( int i = 0; i < size; i++ )
	{
		CUnitTest *pTest = GetTestList()[i];
		if( !pTest )
			continue;
		ConsoleMessage( "------------\nRunning Test: \"%s\"\n-----------", pTest->GetName().c_str() );

		bool bResult = pTest->RunTests();

		if( !bResult )
		{
			string errormsg = string("The test \"") + pTest->GetName() + "\" failed...";
			AssertMsg( bResult == true, errormsg.c_str() );
			ConsoleMessage(errormsg.c_str() );
		}
		else
			ConsoleMessage( "The test \"%s\" ran successfully\n", pTest->GetName().c_str() );
	}
}

