// editor/CKeyValList.h
// editor/CKeyValList.h
// editor/CKeyValList.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//CKeyValList.h

#pragma once
#ifndef deferred__editor__CKeyValList_H__
#define deferred__editor__CKeyValList_H__

#include "CEntityRegister.h"

class CKeyValList
{
public:
	CKeyValList() { }
	~CKeyValList();

	CEntityKeyValue *operator[]( int i ) const {
		return getKeyVal(i);
	}

	CEntityKeyValue *getKeyVal( int i ) const {
		return m_keyvals[i];
	}
	
	CEntityKeyValue *operator[]( const char *name ) const {
		return getKeyVal(name);
	}

	CEntityKeyValue *operator[]( const wchar_t *name ) const {
		return getKeyVal( name );
	}

	CEntityKeyValue *getKeyVal( const wchar_t *name ) const;
	CEntityKeyValue *getKeyVal( const std::string &name ) const;
	CEntityKeyValue *getKeyVal( const char *name ) const;

	void addKeyVal( const CEntityKeyValue &keyval );

	void Serialize( std::ostream &str ) const;
	void Deserialize( CScriptSubGroup *pGrp );

	int size( void ) const { return m_keyvals.size(); }

	void setValue( const char *key, const char *val );

private:
	std::vector<CEntityKeyValue*> m_keyvals;
};




#endif // deferred__editor__CKeyValList_H__
