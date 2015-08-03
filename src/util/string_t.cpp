// util/string_t.cpp
// util/string_t.cpp
// util/string_t.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "string_t.h"
#include <string.h>
#include "error.h"

#if 0
#ifdef DEBUG_STRING_T
	int g_iStringTs = 0;
#endif

string_t::string_t()
{
	buf = NULL;

#ifdef DEBUG_STRING_T
	g_iStringTs++;
	error( "constructed string_t (%i)", g_iStringTs );
#endif
}


string_t::~string_t()
{
	//error( "calling deconstructor of %i", this );
	if( buf != NULL )
	{
		int length = strlen( *this );
		delete[] buf;
		buf = NULL;
	}
#ifdef DEBUG_STRING_T
	g_iStringTs--;
	error( "destructed  string_t (%i)", g_iStringTs );
#endif
}

string_t::string_t( const string_t &s )
{
	//error( "calling constructor (copying) of %i", this );
	buf = NULL;
	SetString( s.buf );

#ifdef DEBUG_STRING_T
	g_iStringTs++;
	error( "constructed string_t (%i)", g_iStringTs );
#endif
}


string_t::string_t( const char *pStr )
{
	//error( "calling constructor of %i with %s", this, pStr );
	if( pStr != NULL )
	{
		buf = new char[strlen(pStr)+1];
		if( buf != NULL )
			strcpy( buf, pStr );
	}
	else
		buf = NULL;
#ifdef DEBUG_STRING_T
	g_iStringTs++;
	error( "constructed string_t (%i)", g_iStringTs );
#endif
}


char *string_t::GetString( void )
{
	return buf;
}


void string_t::SetString( const char *pStr )
{
	if( buf != NULL )
		delete[] buf;
    if( pStr != NULL )
	{
		buf = new char[strlen(pStr)+1];
		strcpy( buf, pStr );
	}
	else
		buf = NULL;
}


void string_t::AddString( const char *pStr )
{
	char *buf2 = new char[strlen(buf)+strlen(pStr)+1];
	memcpy( buf2, buf, strlen(buf)+1 );
	strcat( buf2, pStr );
   	delete[] buf;
	buf = buf2;
}

int string_t::Length( void )
{
	if( buf == NULL )
		return 0;
	
	return strlen(buf);
}


//---------------------------------------------------------------
//ASSIGNMENT OPERATORS
//---------------------------------------------------------------
string_t string_t::operator+=(string_t str)
{
	AddString( str.GetString() );
	return *this;
}

string_t string_t::operator+=(char *str)
{
	AddString( str );
	return *this;
}

string_t string_t::operator=(string_t str)
{
	SetString( str.GetString() );
	return *this;
}

string_t string_t::operator=(char *str)
{
	SetString( str );
	return *this;
}

//---------------------------------------------------------------
//MATHEMATICAL OPERATORS
//---------------------------------------------------------------
string_t string_t::operator+( char *pStr )
{
	string_t ret( this->GetString() );
	ret += pStr;
	return ret;
}

string_t string_t::operator+( string_t str )
{
	string_t ret( this->GetString() );
	ret += str;
	return ret;
}


//---------------------------------------------------------------
//COMPARISON OPERATORS
//---------------------------------------------------------------
bool string_t::operator==( string_t str )
{
	return ( strcmp( buf, str ) == 0 );
}

bool string_t::operator==( char *pStr )
{
	if( pStr == NULL )
		return buf == NULL;

	return ( strcmp( buf, pStr ) == 0 );
}

bool string_t::operator!=( string_t str )
{
	return ( strcmp( buf, str ) != 0 );
}

bool string_t::operator!=( char *pStr )
{
	if( pStr == NULL )
		return buf != NULL;

	return ( strcmp( buf, pStr ) != 0 );
}


//---------------------------------------------------------------
//OTHER OPERATORS
//---------------------------------------------------------------
void string_t::operator delete[]( void *p )
{
	delete p;
}

#endif /* 0 */

