// editor/CEntityTypeSpotTexLight.h
// editor/CEntityTypeSpotTexLight.h
// editor/CEntityTypeSpotTexLight.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__editor__CEntityTypeSpotTexLight_H__
#define deferred__editor__CEntityTypeSpotTexLight_H__

#include "CEditorEntity.h"

class CEntityTypeSpotTexLight : public CEditorEntity
{
	DECLARE_EDITORENT(CEditorEntity, CEntityTypeSpotTexLight)
	DECLARE_BASECLASS(CEditorEntity)

public:
	//Use the base class constructors
	//using CEditorEntity::CEditorEntity;
	CEntityTypeSpotTexLight( const CScriptSubGroup *pGrp, int index ) : BaseClass(pGrp,index) { m_lightIndex = -1; }
	CEntityTypeSpotTexLight( const std::string &classname, int index ) : BaseClass(classname,index) { m_lightIndex = -1; }
	CEntityTypeSpotTexLight( const CEntityTypeSpotTexLight &other, int index) : BaseClass(other, index) { m_lightIndex = -1; }
public:
	virtual void OnCreate( void );
	virtual void OnDestroy( void );
	virtual void OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval );
	virtual void Render( void );

private:
	void UpdateLightParams( void );
	int m_lightIndex;
};

#endif // deferred__editor__CEntityTypeSpotTexLight_H__
