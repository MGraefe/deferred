// editor/CEditorEntityList.h
// editor/CEditorEntityList.h
// editor/CEditorEntityList.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//CEditorEntityList.h

#pragma once
#ifndef deferred__editor__CEditorEntityList_H__
#define deferred__editor__CEditorEntityList_H__

#include <vector>
#include "IDebugRenderer.h"

class CEditorEntity;
class CScriptSubGroup;
class IEditorEntityFactory;

class CEditorEntityList : public IDebugRenderer
{
	typedef std::ostream outstr;
public:
	CEditorEntityList();
	CEditorEntityList( const CEditorEntityList &list );
	~CEditorEntityList();
	CEditorEntity* InsertNewEntity( const std::string &classname );
	void InsertExistingEntity( CEditorEntity *ent );
	void Deserialize( const CScriptSubGroup *pGrp, std::vector<CEditorEntity*> *outAddedEnts = NULL );
	void Serialize( outstr &str );
	void RemoveEntity( CEditorEntity *pEnt, bool bDelete = true );
	void RemoveEntity( int entIndex, bool bDelete = true );
	void ReplaceEntity( CEditorEntity *pOld, CEditorEntity *pNew );
	CEditorEntity *ChangeEntityClassname( CEditorEntity *pEnt, const std::string &cnNew );
	CEditorEntity *TraceLine( const Vector3f &start, const Vector3f &dir );
	const std::vector<CEditorEntity*> &GetVector( void ) const { return m_ents; }
	virtual void RenderDebug( void );
	int FindNumberByName(const std::string &name) const;
	std::vector<CEditorEntity*> FindByName(const std::string &name) const;
	CEditorEntity* FindByIndex( int index ) const;
	CEditorEntity *DuplicateEntity( const CEditorEntity * oldEnt, bool insertInList = true );

private:
	IEditorEntityFactory *GetFactory( const std::string &classname );
	int GetEntListIndex( CEditorEntity *pEnt );
	int GetEntListIndex( int entIndex );
	void _RemoveEntity( int listIndex, bool bDelete );

private:
	std::vector<CEditorEntity*> m_ents;
	int m_indexNumerator;
};

#endif // deferred__editor__CEditorEntityList_H__
