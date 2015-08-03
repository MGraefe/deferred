// editor/CEntityTypeSunLight.h
// editor/CEntityTypeSunLight.h
// editor/CEntityTypeSunLight.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//CEntityTypeSunLight.h

#pragma once
#ifndef deferred__editor__CEntityTypeSunLight_H__
#define deferred__editor__CEntityTypeSunLight_H__

#include "CEditorEntity.h"

class CEntityTypeSunLight : public CEditorEntity
{
	DECLARE_EDITORENT(CEditorEntity, CEntityTypeSunLight)
	DECLARE_BASECLASS(CEditorEntity)

public:
	//Use the base class constructors
	//using CEditorEntity::CEditorEntity;
	CEntityTypeSunLight( const CScriptSubGroup *pGrp, int index ) : BaseClass(pGrp,index) { m_lightIndex = -1; }
	CEntityTypeSunLight( const std::string &classname, int index ) : BaseClass(classname,index) { m_lightIndex = -1; }
	CEntityTypeSunLight( const CEntityTypeSunLight &other, int index) : BaseClass(other, index) { m_lightIndex = -1; }

public:
	virtual void OnCreate( void );
	virtual void OnDestroy( void );
	virtual void OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval );
	virtual void Render( void );
private:
	void UpdateLightParams( void );
	int m_lightIndex;
};

#endif // CEntityTypeSpotLight_H__
