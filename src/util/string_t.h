// util/string_t.h
// util/string_t.h
// util/string_t.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__string_t_H__
#define deferred__util__string_t_H__

#define string_t std::string

#if 0
class string_t
{

public:
	string_t();
	string_t( const string_t &s );
	string_t( const char *pStr );
	~string_t();

	void SetString( const char *pStr );
	char *GetString( void );
	void AddString( const char* pStr );
	int Length( void );
	
	//Assignment operators:
	string_t operator+=( string_t );
	string_t operator+=( char* );
	string_t operator=( string_t );
	string_t operator=( char* );

	//Mathematical operators:
	string_t operator+( string_t );
	string_t operator+( char* );
	
	//boolean operators
	bool operator==( char* );
	bool operator==( string_t );
	bool operator!=( char* );
	bool operator!=( string_t );

	//convertion operators
	operator char*() { return buf; }
	operator const char*() { return buf; }

	//binary operators
	char operator[] ( int i ) { return buf[i]; }

	//others
	void operator delete[]( void *p );

private:
	char *buf;
};
#endif

#endif
