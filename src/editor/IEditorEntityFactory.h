// editor/IEditorEntityFactory.h
// editor/IEditorEntityFactory.h
// editor/IEditorEntityFactory.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//IEditorEntityFactory.h

#pragma once
#ifndef deferred__editor__IEditorEntityFactory_H__
#define deferred__editor__IEditorEntityFactory_H__

#include <map>

class CEditorEntity;
class CScriptSubGroup;
class IEditorEntityFactory;

typedef std::map<std::string, IEditorEntityFactory*> EditorEntityFactoryList;


class IEditorEntityFactory
{
public:
	static EditorEntityFactoryList &GetList(void);

	IEditorEntityFactory( const char *type );

	virtual CEditorEntity *CreateEntity( const CScriptSubGroup *pGrp, int index ) = 0;
	virtual CEditorEntity *CreateEntity( const std::string &classname, int index ) = 0;
	virtual CEditorEntity *CopyEntity( const CEditorEntity *ent ) = 0;
};


template <class T>
class CBaseEditorEntityFactory : public IEditorEntityFactory
{
public:
	CBaseEditorEntityFactory( const char *type ) : IEditorEntityFactory(type) { }
	CEditorEntity *CreateEntity( const CScriptSubGroup *pGrp, int index ) { return new T(pGrp, index); }
	CEditorEntity *CreateEntity( const std::string &classname, int index ) { return new T(classname, index); }
	CEditorEntity *CopyEntity( const CEditorEntity *ent ) { return new T( *((T*)ent) ); }
};


#define DECLARE_EDITOR_ENTITY_TYPE(name, classname)							\
	namespace factories {													\
	static CBaseEditorEntityFactory<classname> classname##_Factory(name);	\
	}


#endif // deferred__editor__IEditorEntityFactory_H__
