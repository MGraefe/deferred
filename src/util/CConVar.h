// util/CConVar.h
// util/CConVar.h
// util/CConVar.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//CConVar.h

#pragma once
#ifndef deferred__util__CConVar_H__
#define deferred__util__CConVar_H__

#include <string>
#include <map>
#include "ConsoleHandler.h"

using std::string;

class CConVarBase; //Forward declaration
class CConVar;

typedef std::map<string, CConVarBase*> ConVarMap;

class CConVarList
{
friend class CConVar;
friend class CConCommand;
public:
	static bool HandleConsoleInput( const char *input );
	static CConVarBase *GetConVarBase( const string &name );
	static CConVar *GetConVar( const string &name );
	static const ConVarMap &GetVarMap( void );
	static void ParseConsoleInputString( string str );
	static void FindConVarBaseCommand( const string &str );

	static CConVar *FindConVar(const string &name);

private:
	static void AddConVar( CConVarBase *pConVar );
	static ConVarMap &GetConVarMap( void );

	static bool EchoConVarValue( const string &name );
	static void EchoConVarValue( CConVar *pVar );
	static bool TryParseConsoleInput( string str );
};


class CConVarBase
{
public:
	CConVarBase( bool isVariable ) : m_isVariable(isVariable) { }
	virtual ~CConVarBase() {}
	const string &GetName( void ) const { return m_name; }
	const string &GetHelpString( void ) const { return m_helpString; }
	bool IsConVar( void ) { return m_isVariable; }
	bool IsConCmd( void ) { return !m_isVariable; }

protected:
	string m_name;
	string m_helpString;
	int m_flags;

private:
	const bool m_isVariable;
};


class CConVar : public CConVarBase
{
public:
	typedef void (ConVarChangeCallback)( const CConVar &oldVar, const CConVar &newVar );
private:
	CConVar( const CConVar &var );
public: 
	CConVar( const string &name, const string &defaultValue, int flags = 0, ConVarChangeCallback *pCallback = NULL );

	//flags = 0 is perfectly valid
	CConVar( const string &name, const string &defaultValue, const string &helpString, int flags = 0, ConVarChangeCallback *pCallback = NULL );

	const string &GetString( void ) const;
	const string &GetDefaultValue( void ) const;
	float  GetFloat( void ) const;
	int    GetInt( void ) const;
	bool   GetBool( void ) const;

	void SetValue( const string &sVal );
	void SetValue( const char *pchVal );
	void SetValue( float fVal );
	void SetValue( int iVal );
	void SetValue( bool bVal );

	bool IsNumeric( void ) const { return m_isNumeric; }
	bool IsDefaultNumeric( void ) const { return m_isDefaultNumeric; }
private:
	void Create( const string &name, const string &defaultValue, int flags, const string &helpString, ConVarChangeCallback *pCallback );
	void PrintConsoleWarning( const char *retrievedTypeName ) const;

private:
	float  m_ValueFloat;
	int    m_ValueInt;
	ConVarChangeCallback *m_pConVarChangeCallback;
	string m_defaultValue;
	string m_ValueString;
	bool   m_ValueBool;

	bool m_isNumeric;
	bool m_isDefaultNumeric;
};





class CConCommand : public CConVarBase
{
public:
	typedef void (ConCommandCallback)(const CConCommand &sender, const string &params);
private:
	CConCommand( const CConCommand &cmd ) : CConVarBase(false) { }
public:
	CConCommand( const char *name, ConCommandCallback *pCallback, int flags = 0);
	CConCommand( const string &name, ConCommandCallback *pCallback, int flags = 0 );
	CConCommand( const string &name, ConCommandCallback *pCallback, const string &helpString, int flags = 0 );

	void Create( const string &name, ConCommandCallback *pCallback, const string &helpString, int flags );
	void CallCommand( const string &params );

private:
	ConCommandCallback *m_pCallback;

};


// Declare a console command.
// Function has the header void <name>_concmd_callback(const CConCommand &sender, const std::string &params), so you can do something like:
// DECLARE_CONSOLE_COMMAND(config_parse, "config.parse")
// {
//	  CConfigParser parser;
//	  if( params.length() < 1 )
//		  parser.ParseConfigFile(CLIENT_CONFIG_FILE, g_pConsoleHandler);
//	  else
//		  parser.ParseConfigFile(params.c_str(), g_pConsoleHandler);
// }

//#define NO_CONCOMMAND

#ifndef NO_CONCOMMAND
#define DECLARE_CONSOLE_COMMAND(name, strname) void name##_concmd_callback(const CConCommand &sender, const std::string &params); \
	CConCommand name(strname, &(name##_concmd_callback)); \
	void name##_concmd_callback(const CConCommand &sender, const std::string &params)
#else
#define DECLARE_CONSOLE_COMMAND(name, strname) void name##_concmd_callback(const CConCommand &sender, const std::string &params)
#endif



#endif // deferred__util__CConVar_H__
