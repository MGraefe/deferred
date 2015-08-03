// util/IInterfaces.h
// util/IInterfaces.h
// util/IInterfaces.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__util__IInterfaces_H__
#define deferred__util__IInterfaces_H__

#include "dll.h"

#define DECL_INTERFACE(Class,ClassName)	public: Class *Get##ClassName( void ) const { return m_p##ClassName; } \
										private: Class *m_p##ClassName;

#define DECL_INTERFACE_GET_ABSTRACT(Class,ClassName) public: virtual Class *Get##ClassName( void ) const = 0;
#define INIT_INTERFACE(Class,ClassName) m_p##ClassName = new Class()
#define CLEANUP_INTERFACE(ClassName) if(m_p##ClassName) { delete m_p##ClassName; m_p##ClassName = NULL; }
#define NULL_INTERFACE(ClassName) m_p##ClassName = NULL


class DLL IInterfaces
{
public:
	IInterfaces() { }
	~IInterfaces() { }
	virtual void Init( void ) = 0;
	virtual void Cleanup( void ) = 0;
};


#endif
