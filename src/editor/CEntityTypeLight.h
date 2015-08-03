// editor/CEntityTypeLight.h
// editor/CEntityTypeLight.h
// editor/CEntityTypeLight.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//CEntityTypeLight.h

#pragma once
#ifndef deferred__editor__CEntityTypeLight_H__
#define deferred__editor__CEntityTypeLight_H__

#include "CEditorEntity.h"

class CEntityTypeLight : public CEditorEntity
{
	DECLARE_EDITORENT(CEditorEntity, CEntityTypeLight)
	DECLARE_BASECLASS(CEditorEntity)

public:
	//Use the base class constructors
	//using CEditorEntity::CEditorEntity;
	CEntityTypeLight( const CScriptSubGroup *pGrp, int index ) : BaseClass(pGrp,index) { m_lightIndex = -1; }
	CEntityTypeLight( const std::string &classname, int index ) : BaseClass(classname,index) { m_lightIndex = -1; }
	CEntityTypeLight( const CEntityTypeLight &other, int index ) : BaseClass(other, index) { m_lightIndex = -1; }

public:
	virtual void OnCreate( void );
	virtual void OnDestroy( void );
	virtual void OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval );
	virtual void Render( void );
private:
	void UpdateLightParams( void );
	int m_lightIndex;
};

#endif // deferred__editor__CEntityTypeLight_H__
