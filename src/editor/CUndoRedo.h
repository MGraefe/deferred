// editor/CUndoRedo.h
// editor/CUndoRedo.h
// editor/CUndoRedo.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


//CUndoRedo.h

#pragma once
#ifndef deferred__editor__CUndoRedo_H__
#define deferred__editor__CUndoRedo_H__

#include "CEditorEntity.h"


class IUndoRedoState
{
public:
	virtual void UndoState() = 0;
	virtual void RedoState() = 0;
};


class CUndoRedo
{
public:
	static const int m_UndoMaxCount = 100;

public:
	~CUndoRedo();
	void Undo( void );
	void Redo( void );
	void AddUndo( IUndoRedoState *state );
	bool CanUndo( void ) { return m_UndoList.size() > 0; }
	bool CanRedo( void ) { return m_RedoList.size() > 0; }

private:
	void InvalidateRedoList( void );
	void KeepMaximumListSize( void );

private:
	std::list<IUndoRedoState*> m_UndoList;
	std::list<IUndoRedoState*> m_RedoList;
};


class CUndoRedoStatePropertyChanged : public IUndoRedoState
{
public:
	CUndoRedoStatePropertyChanged(const std::vector<CEditorEntity*> &affectedEnts, const std::string &key, const std::string &oldVal, const std::string &newVal);
	virtual void UndoState();
	virtual void RedoState();

private:
	std::vector<CEditorEntity*> m_affectedEnts;
	const std::string m_key;
	const std::string m_oldVal;
	const std::string m_newVal;
};


class CUndoRedoStateEntityCreated : public IUndoRedoState
{
public:
	CUndoRedoStateEntityCreated(CEditorEntity *entity);
	~CUndoRedoStateEntityCreated();
	virtual void UndoState();
	virtual void RedoState();

private:
	CEditorEntity *m_entity;
};


class CUndoRedoStateEntitiesDeleted : public IUndoRedoState
{
public:
	CUndoRedoStateEntitiesDeleted(const std::vector<CEditorEntity*> &affectedEnts);
	~CUndoRedoStateEntitiesDeleted();
	virtual void UndoState();
	virtual void RedoState();

private:
	std::vector<CEditorEntity*> m_entCopies;
};


class CUndoRedoStateOutputListChanged : public IUndoRedoState
{
public:
	typedef std::vector<std::pair<int, CEditorEntityOutputList>> Outputs;
	CUndoRedoStateOutputListChanged(const Outputs &oldOutputs, const Outputs &newOutputs);
	~CUndoRedoStateOutputListChanged();
	virtual void UndoState();
	virtual void RedoState();

private:
	void restoreOutputList(const Outputs &outputs) const;
	Outputs m_oldOutputs;
	Outputs m_newOutputs;
};


//TODO: ent selected / deselected

#endif // deferred__editor__CUndoRedo_H__
