// util/CConVar.cpp
// util/CConVar.cpp
// util/CConVar.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CConVar.h"
#include "error.h"
#include "StringUtils.h"


using std::string;
static const char g_stdHelpString[] = "No help string...the programmer was too lazy...";


void ConCommand_findCallback( const CConCommand &sender, const string &params )
{
	CConVarList::FindConVarBaseCommand(params);
}

CConCommand ConCommand_find( "find", &ConCommand_findCallback );


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
ConVarMap &CConVarList::GetConVarMap( void )
{
	static ConVarMap cvarmap;
	return cvarmap;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const ConVarMap &CConVarList::GetVarMap( void )
{
	return GetConVarMap();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CConVarBase *CConVarList::GetConVarBase( const string &name )
{
	ConVarMap::iterator it = GetConVarMap().find( name );
	if( it != GetConVarMap().end() )
		return it->second;
	else
		return NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CConVar * CConVarList::GetConVar( const string &name )
{
	CConVarBase *varBase = GetConVarBase( name );

	if( varBase && varBase->IsConVar() )
		return (CConVar*)varBase;

	return NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConVarList::AddConVar( CConVarBase *pConVar )
{
	ConVarMap::iterator it = GetConVarMap().find( pConVar->GetName() );
	if( it != GetConVarMap().end() )
	{
		error( "Tried to add the already existing ConVar \"%s\". Ignoring.",
			pConVar->GetName().c_str() );
		return;
	}

	GetConVarMap().insert( std::make_pair( pConVar->GetName(), pConVar ) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CConVarList::TryParseConsoleInput( string str )
{
	//StringUtils::trimstrip_leading_spaces_or_tabs( str );
	StrUtils::trim(str);
	
	//Parse until space or tab
	char spacetab[] = { ' ', '\t', 0x0 };
	size_t found = str.find_first_of( spacetab );
	if( found == string::npos )
		found = str.length();
	//if( found == string::npos )
	//	return EchoConVarValue( str );

	string varname( str.begin(), str.begin() + found );

	CConVarBase *pVarBase = GetConVarBase( varname );
	if( !pVarBase )
		return false;

	if( found == str.length() && pVarBase->IsConVar() ) //are there any parameters or values?
	{
		EchoConVarValue((CConVar*)pVarBase);
		return true;
	}

	string value;
	if( found == str.length() )
		value.assign( "" );
	else
		value.assign( str.begin()+found+1, str.end() );

	if( value.length() > 0 )
	{
		StrUtils::trim( value );

		if( *value.begin() == '"' && *value.rbegin() == '"' )
		{
			value.erase( 0, 1 );
			value.pop_back();
		}
	}

	if( pVarBase->IsConVar() )
	{
		CConVar *pVar = (CConVar*)pVarBase;
		pVar->SetValue( value );
		EchoConVarValue( pVar );
	}
	else if( pVarBase->IsConCmd() )
	{
		CConCommand *pCmd = (CConCommand*)pVarBase;
		pCmd->CallCommand( value );
	}
	
	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConVarList::ParseConsoleInputString( string str )
{
	if( !TryParseConsoleInput(str) )
		ConsoleMessage( "Couldn't find variable \"%s\"", str.c_str() );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConVarList::EchoConVarValue( CConVar *pVar )
{
	if( pVar != NULL )
		ConsoleMessage( "\"%s\" = \"%s\"", pVar->GetName().c_str(), pVar->GetString().c_str() );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CConVarList::EchoConVarValue( const string &name )
{
	CConVar *pVar = GetConVar( name );
	if( !pVar )
		return false;
	
	EchoConVarValue( pVar );
	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CConVarList::HandleConsoleInput( const char *input )
{
	return TryParseConsoleInput( string(input) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConVarList::FindConVarBaseCommand( const string &str )
{
	ConVarMap &map = GetConVarMap();
	ConVarMap::const_iterator it = map.begin();
	ConVarMap::const_iterator itEnd = map.end();

	bool foundOne = false;
	for( ; it != itEnd; it++ )
	{
		if( it->first.find( str ) != string::npos )
		{
			foundOne = true;
			string outstr( it->first );
			if( it->second->IsConVar() )
				outstr += " = \"" + ((CConVar*)it->second)->GetString() + "\"";
			else
				outstr += " (command)";
			ConsoleMessage( outstr.c_str() );
		}
	}

	if( !foundOne )
		ConsoleMessage( "Nothing found." );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CConVar * CConVarList::FindConVar( const string &name )
{
	ConVarMap &map = GetConVarMap();
	auto it = map.find(name);
	if(it == map.end())
		return NULL;
	if(it->second->IsConCmd())
		return NULL;
	return dynamic_cast<CConVar*>(it->second);
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CConVar::CConVar(	const string &name,
					const string &defaultValue,
					const string &helpString,
					int flags /*= 0*/,
					ConVarChangeCallback *pCallback /*= NULL*/ ) 
	: CConVarBase(true)
{
	Create( name, defaultValue, flags, helpString, pCallback );
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CConVar::CConVar( const string &name, const string &defaultValue, int flags /*= 0 */, ConVarChangeCallback *pCallback /*= NULL*/ )
	: CConVarBase(true)
{
	Create( name, defaultValue, flags, g_stdHelpString, pCallback );
}


//---------------------------------------------------------------
// Purpose: Private copy constructor
//---------------------------------------------------------------
CConVar::CConVar( const CConVar &var ) 
	: CConVarBase(true),
	m_ValueFloat( var.m_ValueFloat ),
	m_ValueInt( var.m_ValueInt ),
	m_pConVarChangeCallback( var.m_pConVarChangeCallback ),
	m_defaultValue( var.m_defaultValue ),
	m_ValueString( var.m_ValueString ),
	m_ValueBool( var.m_ValueBool ),
	m_isNumeric( var.m_isNumeric ),
	m_isDefaultNumeric( var.m_isDefaultNumeric )
{
	m_name = var.m_name;
	m_helpString = var.m_helpString;
	m_flags = var.m_flags;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConVar::Create( const string &name, const string &defaultValue, int flags, const string &helpString, ConVarChangeCallback *pCallback )
{
	m_name = name;
	m_defaultValue = defaultValue;
	m_flags = flags;
	m_helpString = helpString;
	m_pConVarChangeCallback = pCallback;

	m_isDefaultNumeric = m_isNumeric = StrUtils::is_numeric(defaultValue);

	if( m_isDefaultNumeric )
	{
		SetValue( defaultValue );
	}
	else
	{
		m_ValueString = defaultValue;
		m_ValueFloat = 0.0f;
		m_ValueBool = false;
		m_ValueInt = 0;
	}

	CConVarList::AddConVar( this );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConVar::PrintConsoleWarning( const char *retrievedTypeName ) const
{
	ConsoleMessage( "Warning: retrieving %s from non-numeric ConVar \"%s\", returning zero.",
		retrievedTypeName, m_name.c_str() );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const string &CConVar::GetString( void ) const
{
	return m_ValueString;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
float CConVar::GetFloat( void ) const
{
	if( !m_isDefaultNumeric )
		PrintConsoleWarning( "float" );

	return m_ValueFloat;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CConVar::GetInt( void ) const
{
	if( !m_isDefaultNumeric )
		PrintConsoleWarning( "int" );

	return m_ValueInt;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CConVar::GetBool( void ) const
{
	if( !m_isDefaultNumeric )
		PrintConsoleWarning( "bool" );

	return m_ValueBool;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConVar::SetValue( const string &sVal )
{
	CConVar oldVar(*this);

	m_ValueString = sVal;
	
	if( StrUtils::is_numeric(sVal) )
	{
		m_isNumeric = true;
		m_ValueFloat = (float)atof( sVal.c_str() );
		m_ValueInt = (int)(m_ValueFloat + 0.5f);
		m_ValueBool = m_ValueInt != 0;
	}
	else
	{
		if( m_isDefaultNumeric )
			ConsoleMessage( "Warning: setting numeric ConVar \"%s\" to non-numeric value, last numeric value was %f.",
				m_name.c_str(), m_ValueFloat );

		m_isNumeric = false;
	}

	if( m_pConVarChangeCallback )
		m_pConVarChangeCallback( oldVar, *this );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConVar::SetValue( const char *pchVal )
{
	SetValue( string(pchVal) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConVar::SetValue( float fVal )
{
	CConVar oldVar(*this);

	m_isNumeric = true;
	m_ValueFloat = fVal;
	m_ValueInt = (int)(fVal + 0.5f); //Do rounding
	m_ValueBool = m_ValueInt != 0;

	char str[32];
	sprintf( str, "%f", fVal );
	m_ValueString.assign( str );

	if( m_pConVarChangeCallback )
		m_pConVarChangeCallback( oldVar, *this );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConVar::SetValue( int iVal )
{
	CConVar oldVar(*this);

	m_isNumeric = true;
	m_ValueFloat = (float)iVal;
	m_ValueInt = iVal;
	m_ValueBool = iVal != 0;

	char str[32];
	_itoa( iVal, str, 10 );
	m_ValueString.assign( str );

	if( m_pConVarChangeCallback )
		m_pConVarChangeCallback( oldVar, *this );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConVar::SetValue( bool bVal )
{
	CConVar oldVar(*this);

	m_isNumeric = true;
	m_ValueFloat = bVal ? 1.0f : 0.0f;
	m_ValueInt = bVal ? 1 : 0;
	m_ValueBool = bVal;

	m_ValueString = bVal ? "1" : "0";

	if( m_pConVarChangeCallback )
		m_pConVarChangeCallback( oldVar, *this );
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const string &CConVar::GetDefaultValue( void ) const
{
	return m_defaultValue;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CConCommand::CConCommand( const char *name, ConCommandCallback *pCallback, int flags /*= 0 */ )
	: CConVarBase(false)
{
	Create( string(name), pCallback, g_stdHelpString, flags );
}


//---------------------------------------------------------------
// Purpose:
//---------------------------------------------------------------
CConCommand::CConCommand( const string &name, ConCommandCallback *pCallback, int flags /*= 0 */ )
	: CConVarBase(false)
{
	Create( name, pCallback, g_stdHelpString, flags );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CConCommand::CConCommand( const string &name, ConCommandCallback *pCallback, const string &helpString, int flags /*= 0 */ )
	: CConVarBase(false)
{
	Create( name, pCallback, helpString, flags );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConCommand::Create( const string &name, ConCommandCallback *pCallback, const string &helpString, int flags )
{
	m_name = name;
	m_pCallback = pCallback;
	m_helpString = helpString;
	m_flags = flags;

	CConVarList::AddConVar( this );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CConCommand::CallCommand( const string &params )
{
	if( m_pCallback )
		m_pCallback( *this, params );
	else
		ConsoleMessage( "No callback defined for command \"%s\"", GetName().c_str() );
}
