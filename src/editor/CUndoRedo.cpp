// editor/CUndoRedo.cpp
// editor/CUndoRedo.cpp
// editor/CUndoRedo.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "CUndoRedo.h"
#include "CEditorEntity.h"
#include "editorDoc.h"
#include "CEditorEntityList.h"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CUndoRedo::~CUndoRedo()
{
	for(IUndoRedoState *state : m_UndoList)
		delete state;
	for(IUndoRedoState *state : m_RedoList)
		delete state;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedo::Undo( void )
{
	if( !CanUndo() )
		return;

	IUndoRedoState *state = m_UndoList.front();
	m_RedoList.push_front( state );
	m_UndoList.pop_front();
	state->UndoState();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedo::Redo( void )
{
	if( !CanRedo() )
		return;

	//Save current state to undo list
	IUndoRedoState *state = m_RedoList.front();
	m_UndoList.push_back( state );
	m_RedoList.pop_front();
	state->RedoState();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedo::AddUndo( IUndoRedoState *state )
{
	InvalidateRedoList();

	m_UndoList.push_front( state );

	KeepMaximumListSize();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedo::InvalidateRedoList( void )
{
	for(IUndoRedoState *redo : m_RedoList)
		delete redo;
	m_RedoList.clear();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedo::KeepMaximumListSize( void )
{
	while( m_UndoList.size() > m_UndoMaxCount )
	{
		delete m_UndoList.back();
		m_UndoList.pop_back();
	}
	
	while( m_RedoList.size() > m_UndoMaxCount )
	{
		delete m_RedoList.back();
		m_RedoList.pop_back();
	}
}


//###############################################################
// PropertyChanged
//###############################################################
CUndoRedoStatePropertyChanged::CUndoRedoStatePropertyChanged( const std::vector<CEditorEntity*> &affectedEnts, const std::string &key, const std::string &oldVal, const std::string &newVal ) :
	m_affectedEnts(affectedEnts),
	m_key(key),
	m_newVal(newVal),
	m_oldVal(oldVal)
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedoStatePropertyChanged::UndoState()
{
	for(CEditorEntity *ent : m_affectedEnts)
		ent->SetValue(m_key, m_oldVal);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedoStatePropertyChanged::RedoState()
{
	for(CEditorEntity *ent : m_affectedEnts)
		ent->SetValue(m_key, m_newVal);
}


//###############################################################
// EntityCreated
//###############################################################
CUndoRedoStateEntityCreated::CUndoRedoStateEntityCreated( CEditorEntity *entity )
{
	m_entity = entity->Copy(-1);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CUndoRedoStateEntityCreated::~CUndoRedoStateEntityCreated()
{
	if( m_entity )
		delete m_entity;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedoStateEntityCreated::UndoState()
{
	//RemoveEntity calls m_entity->OnDestroy()
	GetActiveDocument()->GetEntityList()->RemoveEntity(m_entity->GetIndex());
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedoStateEntityCreated::RedoState()
{
	CEditorEntity *newEnt = GetActiveDocument()->GetEntityList()->DuplicateEntity(m_entity);
	newEnt->OnCreate();
}





//###############################################################
// EntitiesDeleted
//###############################################################
CUndoRedoStateEntitiesDeleted::CUndoRedoStateEntitiesDeleted( const std::vector<CEditorEntity*> &affectedEnts )
{
	for(CEditorEntity *ent : affectedEnts)
		m_entCopies.push_back(ent->Copy(-1));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CUndoRedoStateEntitiesDeleted::~CUndoRedoStateEntitiesDeleted()
{
	for(CEditorEntity *entCopy : m_entCopies)
		delete entCopy;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedoStateEntitiesDeleted::UndoState()
{
	for(CEditorEntity *entCopy : m_entCopies)
	{
		CEditorEntity *newEnt = GetActiveDocument()->GetEntityList()->DuplicateEntity(entCopy);
		newEnt->OnCreate();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedoStateEntitiesDeleted::RedoState()
{
	for(CEditorEntity *entCopy : m_entCopies)
		GetActiveDocument()->GetEntityList()->RemoveEntity(entCopy->GetIndex());
}


//###############################################################
// OutputListChanged
//###############################################################
CUndoRedoStateOutputListChanged::CUndoRedoStateOutputListChanged(const Outputs &oldOutputs, const Outputs &newOutputs) :
	m_oldOutputs(oldOutputs),
	m_newOutputs(newOutputs)
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CUndoRedoStateOutputListChanged::~CUndoRedoStateOutputListChanged()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedoStateOutputListChanged::UndoState()
{
	restoreOutputList(m_newOutputs);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedoStateOutputListChanged::RedoState()
{
	restoreOutputList(m_oldOutputs);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CUndoRedoStateOutputListChanged::restoreOutputList( const Outputs &outputs ) const
{
	CEditorEntityList *list = GetActiveDocument()->GetEntityList();
	Assert(list);
	for(const Outputs::value_type &p : outputs)
	{
		CEditorEntity *ent = list->FindByIndex(p.first);
		Assert(ent);
		ent->SetOutputList(p.second);
	}
}


