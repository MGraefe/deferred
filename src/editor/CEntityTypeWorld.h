// editor/CEntityTypeWorld.h
// editor/CEntityTypeWorld.h
// editor/CEntityTypeWorld.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CEntityTypeWorld.h

#pragma once
#ifndef deferred__editor__CEntityTypeWorld_H__
#define deferred__editor__CEntityTypeWorld_H__

#include "CEditorEntity.h"

class CRenderWorld;
class CSkyBox;

class CEntityTypeWorld : public CEditorEntity
{
	DECLARE_EDITORENT(CEditorEntity, CEntityTypeWorld)
	DECLARE_BASECLASS(CEditorEntity)

public:
	//Use the base class constructors
	//using CEditorEntity::CEditorEntity;
	CEntityTypeWorld( const CScriptSubGroup *pGrp, int index );
	CEntityTypeWorld( const std::string &classname, int index );
	CEntityTypeWorld( const CEntityTypeWorld &other, int index );

public:
	virtual std::string GetSerializationHeaderString( void ) const;
	virtual void OnCreate( void );
	virtual void OnDestroy( void );
	virtual void OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval );
private:
	void loadWorld( void );
private:
	CRenderWorld *m_renderworld;
};




#endif // deferred__editor__CEntityTypeWorld_H__
