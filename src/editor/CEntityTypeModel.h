// editor/CEntityTypeModel.h
// editor/CEntityTypeModel.h
// editor/CEntityTypeModel.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CEntityTypeModel.h

#pragma once
#ifndef deferred__editor__CEntityTypeModel_H__
#define deferred__editor__CEntityTypeModel_H__

#include "CEditorEntity.h"

class CModel;
class CEntityTypeModel : public CEditorEntity
{
	DECLARE_EDITORENT(CEditorEntity, CEntityTypeModel)
	DECLARE_BASECLASS(CEditorEntity)

public:
	//Use the base class constructors
	//using CEditorEntity::CEditorEntity;
	CEntityTypeModel( const CScriptSubGroup *pGrp, int index );
	CEntityTypeModel( const std::string &classname, int index );
	CEntityTypeModel( const CEntityTypeModel &other, int index );

public:
	virtual void OnCreate( void );
	virtual void OnDestroy( void );
	virtual void OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval );
	virtual bool TraceLine( tracelineinfo_t &info );
	virtual void Render( void );

private:
	void loadModel();
	void destroyModel();
	
private:
	CModel *m_model;
};

#endif // deferred__editor__CEntityTypeModel_H__
