// editor/CKeyValList.cpp
// editor/CKeyValList.cpp
// editor/CKeyValList.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include <iostream>
#include "CKeyValList.h"
#include <util/CScriptParser.h>


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CKeyValList::~CKeyValList()
{
	for( int i = 0; i < size(); i++ )
		delete m_keyvals[i];
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CKeyValList::addKeyVal( const CEntityKeyValue &keyval )
{
	m_keyvals.push_back( new CEntityKeyValue(keyval) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CKeyValList::Serialize( std::ostream &str ) const
{
	for( int i = 0; i < size(); i++ )
	{
		str << "\"" << m_keyvals[i]->name << "\" \"" << m_keyvals[i]->val << "\"" << std::endl;
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CKeyValList::Deserialize( CScriptSubGroup *pGrp )
{
	//const KeyValueMap *map = pGrp->GetKeyValueMap();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityKeyValue * CKeyValList::getKeyVal( const std::string &name ) const
{
	return getKeyVal( name.c_str() );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityKeyValue *CKeyValList::getKeyVal( const char *name ) const
{
	for( int i = 0; i < size(); i++ )
		if( m_keyvals[i]->name.compare( name ) == 0 )
			return m_keyvals[i];
	return NULL;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityKeyValue * CKeyValList::getKeyVal( const wchar_t *name ) const
{
	return getKeyVal( CW2A(name) );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CKeyValList::setValue( const char *key, const char *val )
{
	CEntityKeyValue *pKeyVal = getKeyVal(key);
	if( !pKeyVal )
		return;

	pKeyVal->val.assign( val );
}


