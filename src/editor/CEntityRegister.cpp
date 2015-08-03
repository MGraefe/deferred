// editor/CEntityRegister.cpp
// editor/CEntityRegister.cpp
// editor/CEntityRegister.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"

#include "CEntityRegister.h"
#include <util/CScriptParser.h>
#include "OutputWnd.h"


CEntityRegister g_EntityRegister;
CEntityRegister *GetEntityRegister( void )
{
	return &g_EntityRegister;
}

paramtypes_e GetParameterType( const std::string &type )
{
	if( type == "ANGLE" )
		return PT_ANGLE;
	if( type == "ORIGIN" )
		return PT_ORIGIN;
	if( type == "COLOR" )
		return PT_COLOR;
	return PT_UNKNOWN;
}


CEntityKeyValue * CEntityRegisterEntity::Get( const std::string &name ) const
{
	for( UINT i = 0; i < m_KeyVals.size(); i++ )
	{
		if( name == m_KeyVals[i]->name )
			return m_KeyVals[i];
	}

	return NULL;
}


CEntityKeyValue * CEntityRegisterEntity::Get( const CString &name ) const
{
	return Get( std::string( CT2A(name) ) );
}


CEntityKeyValue * CEntityRegisterEntity::Get( const char *name ) const
{
	return Get( std::string(name) );
}


CEntityKeyValue * CEntityRegisterEntity::Get( int i ) const
{
	Assert(i >= 0 && i < (int)m_KeyVals.size());
	if( i < 0 || i >= (int)m_KeyVals.size() )
		return NULL;
	return m_KeyVals[i];
}


void CEntityRegisterEntity::AddKeyVal( const std::string &name, const std::string &desc, const std::string &val, const std::string &type )
{
	CEntityKeyValue *pKeyVal = new CEntityKeyValue( name, desc, val, GetParameterType(type) );
	m_KeyVals.push_back(pKeyVal);
}

void CEntityRegisterEntity::AddKeyVal( const std::string &name, const std::string &desc, const std::string &val, paramtypes_e type )
{
	CEntityKeyValue *pKeyVal = new CEntityKeyValue( name, desc, val, type );
	m_KeyVals.push_back(pKeyVal);
}

void CEntityRegisterEntity::AddInput( const std::string &name, const std::string &desc )
{
	AddInput(CEntityRegisterInput(name, desc));
}

void CEntityRegisterEntity::AddInput( const CEntityRegisterInput &input )
{
	m_Inputs.push_back(input);
}

void CEntityRegisterEntity::AddOutput( const std::string &name, const std::string &desc )
{
	AddOutput(CEntityRegisterOutput(name, desc));
}

void CEntityRegisterEntity::AddOutput( const CEntityRegisterOutput &output )
{
	m_Outputs.push_back(output);
}

void CEntityRegisterEntity::Set( const std::string &name, const std::string &val )
{
	CEntityKeyValue *pKeyVal = Get(name);
	Assert(pKeyVal);	
	pKeyVal->val = val;
}


void CEntityRegisterEntity::Set( const CString &name, const CString &val )
{
	Set( std::string(CT2A(name)), std::string(CT2A(val)) );
}


void CEntityRegisterEntity::Set( const char *name, const char *val )
{
	Set( std::string(name), std::string(val) );
}


void CEntityRegisterEntity::Set( int i, const std::string &val )
{
	if( i < 0 || i >= (int)m_KeyVals.size() )
		return;
	
	m_KeyVals[i]->val = val;
}


void CEntityRegisterEntity::Set( int i, const CString &val )
{
	Set(i, std::string(CT2A(val)) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityRegisterEntity::CEntityRegisterEntity( const std::string &classname ) :
	m_classname(classname)
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const std::string &CEntityRegisterEntity::GetBaseVal( const string &key )
{
	return m_baseVals[key];
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityRegisterEntity * CEntityRegister::GetEntity( const std::string &name )
{
	EntityRegisterMap::const_iterator it = m_EntMap.find( name );
	if( it == m_EntMap.end() )
		return NULL;
	
	return it->second;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityRegisterEntity * CEntityRegister::GetEntity( const CString &name )
{
	return GetEntity( std::string(CT2A(name)) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityRegister::Init( void )
{
	m_parser.ParseFile( "scripts/entities.txt" );

	const SubGroupMap *groupMap = m_parser.GetSubGroupMap();
	SubGroupMap::const_iterator it = groupMap->begin();
	SubGroupMap::const_iterator itEnd = groupMap->end();

	for( ; it != itEnd; it++ )
	{
		std::string classname = it->first;
		CEntityRegisterEntity *pEnt = new CEntityRegisterEntity(classname);
		ParseEntity( pEnt, it->second );
		m_EntMap.insert( std::make_pair(classname, pEnt ) );
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityRegister::ResolveBaseEntities( CEntityRegisterEntity *pEnt, std::string bases )
{
	//Strip spaces
	std::string::iterator it = std::remove( bases.begin(), bases.end(), ' ' );
	bases.erase( it, bases.end() );
	
	UINT pos = 0;
	UINT oldpos = 0;
	while( pos < bases.length() )
	{
		pos = bases.find_first_of( ",\0", oldpos+1 );
		if( pos == std::string::npos )
			pos = bases.length();

		string b(bases.begin()+oldpos, bases.begin()+pos);
		CEntityRegisterEntity *pBase = GetEntity(b);
		if( !pBase )
		{
			//base could be there but is not yet loaded, try finding it
			CScriptSubGroup *baseGroup = m_parser.GetSubGroup(b.c_str());
			if( !baseGroup ) 
			{
				//nope, definitely not there
				PostOutput( _T("scripts\\entities.txt: %S -> unknown base class \"%S\""), 
					(LPCTSTR)pEnt->GetClassname().c_str(), (LPCTSTR)b.c_str() );
				return;
			}
			else
			{
				//entity is there but not loaded, so load the damn thing!
				CEntityRegisterEntity *baseEnt = new CEntityRegisterEntity(b);
				ParseEntity(baseEnt, baseGroup);
				m_EntMap.insert(std::make_pair(b, baseEnt));
				pBase = baseEnt;
			}
		}

		//add base class keyvals
		for( int i = 0; i < pBase->size(); i++ )
		{
			CEntityKeyValue *v = pBase->Get(i);
			pEnt->AddKeyVal( v->name, v->desc, v->val, v->type );
		}

		//add base class outputs
		for( size_t i = 0; i < pBase->GetNumOutputs(); i++ )
			pEnt->AddOutput(*pBase->GetOutput(i));

		//add base class inputs
		for( size_t i = 0; i < pBase->GetNumInputs(); i++ )
			pEnt->AddInput(*pBase->GetInput(i));

		oldpos = pos+1;
	}
}


//---------------------------------------------------------------
// Purpose: returns the index of the char after the delimiting
// character ] or string::npos if something went wrong
//---------------------------------------------------------------
size_t readBracketBoxedValue( const string &str, size_t off, string &out )
{
	//find first [
	size_t openBracket = str.find('[', off);
	if( openBracket == string::npos )
		return string::npos;

	//find first ] which is not preceeded by a backslash
	size_t closedBracket = openBracket;
	do {
		closedBracket++;
		closedBracket = str.find(']', closedBracket);
		if( closedBracket == string::npos || closedBracket <= openBracket )
			return string::npos;
	} while ( str[closedBracket-1] == '\\' );

	out.assign(str, openBracket+1, closedBracket-openBracket-1);
	return openBracket+1;
}


//---------------------------------------------------------------
// Purpose: splits keyvalue in the form "[type] [init] [desc]" into the parts
//---------------------------------------------------------------
bool getBracketSeperatedKeyvalue( const string &str, string &type, string &init, string &desc )
{
	size_t off;
	off = readBracketBoxedValue(str, 0, type);
	if( off == string::npos )
		return false;
	
	off = readBracketBoxedValue(str, off, init);
	if( off == string::npos )
		return false;

	off = readBracketBoxedValue(str, off, desc);
	if( off == string::npos )
		return false;
	return true;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityRegister::ParseEntity( CEntityRegisterEntity *pEnt, CScriptSubGroup *pGrp )
{
	//parse base-vals
	for(KeyValueMap::const_iterator it = pGrp->GetKeyValueMap()->begin();
			it != pGrp->GetKeyValueMap()->end(); it++ )
		pEnt->m_baseVals[it->first] = it->second;

	//parse classtype
	if( !pGrp->GetString( "classtype", pEnt->m_internalType ) )
	{
		if( pEnt->GetClassname().size() == 0 || pEnt->GetClassname()[0] != '@' )
			PostOutput( _T("Could not find internal entity type of \"%S\""), pEnt->GetClassname().c_str() );
	}

	//parse baseclasses
	std::string base;
	if( pGrp->GetString( "base", base ) )
		ResolveBaseEntities( pEnt, base );

	//parse describtion
	pGrp->GetString( "desc", pEnt->m_describtion );

	//parse keyvals
	const CScriptSubGroup *kvGroup = pGrp->GetSubGroup("keyvals");
	if( !kvGroup )
		PostOutput( _T("Notification: Couldn't find keyvals-section in entity \"%s\" in entities.txt, ")
			_T("this could be intentional if all kv's are contained in the base-classes."),
			CA2T(pEnt->GetClassname().c_str()) );
	else
	{
		const KeyValueMap *kvMap = kvGroup->GetKeyValueMap();
		KeyValueMap::const_iterator it = kvMap->begin();
		KeyValueMap::const_iterator itEnd = kvMap->end();

		for( ; it != itEnd; ++it )
		{
			string name = it->first;
			string type, init, desc;
			if( getBracketSeperatedKeyvalue(it->second, type, init, desc) )
				pEnt->AddKeyVal(name, desc, init, type);
			else
				PostOutput(_T("Error parsing keyval \"%s\" in entity \"%s\" in entities.txt"), CA2T(name.c_str()), CA2T(pEnt->GetClassname().c_str()));
		}
	}

	//Parse inputs
	const CScriptSubGroup *inGroup = pGrp->GetSubGroup("inputs");
	if( inGroup )
	{
		const KeyValueMap *kvMap = inGroup->GetKeyValueMap();	
		for( auto it = kvMap->begin(); it != kvMap->end(); ++it )
			pEnt->AddInput(it->first, it->second);
	}

	//Parse outputs
	const CScriptSubGroup *outGroup = pGrp->GetSubGroup("outputs");
	if( outGroup )
	{
		const KeyValueMap *kvMap = outGroup->GetKeyValueMap();
		for( auto it = kvMap->begin(); it != kvMap->end(); ++it )
			pEnt->AddOutput(it->first, it->second);
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityRegister::GetEntityNames( std::vector<std::string> &vec )
{
	EntityRegisterMap::const_iterator it = m_EntMap.begin();
	EntityRegisterMap::const_iterator itEnd = m_EntMap.end();

	for( ; it != itEnd; it++ )
		vec.push_back(it->first);
}
