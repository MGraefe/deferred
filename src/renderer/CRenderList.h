// renderer/CRenderList.h
// renderer/CRenderList.h
// renderer/CRenderList.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__renderer__CRenderList_H__
#define deferred__renderer__CRenderList_H__

#include "CRenderEntity.h"
#include <list>
#include <util/IEvent.h>
#include <util/CEventManager.h>
#include <util/dll.h>

typedef std::list< CRenderEntity* > RenderEntList;

PUSH_PRAGMA_DISABLE_DLL_MEMBER_WARNING

class DLL CRenderList : public IEventListener
{
public:
	CRenderList();
	virtual ~CRenderList();
	void RegisterEvents( CEventManager* const pMgr );
	void SetAppLayerEventMgr( CEventManager *const pMgr ) { m_pAppLayerEvtMgr = pMgr; }
	CEventManager *GetAppLayerEventMgr( void ) { return m_pAppLayerEvtMgr; }

	void AddStaticEntity( CRenderEntity* const pEnt );
	bool DeleteStaticEntity( CRenderEntity* const pEnt, bool bCleanup = true );
	bool DeleteStaticEntity( const int &index, bool bCleanup = true );

	CRenderEntity* GetEntityByLogicIndex( int index );

	void RenderStatics( const CRenderParams &params );
	bool HandleEvent( const IEvent *evt );
	void UnregisterEvents( void ) { }

	void RestoreEntities( void );

	void UpdateInterpolation( void );

private:
	RenderEntList m_StaticEnts;
	CEventManager* m_pAppLayerEvtMgr;
	float m_fSnapshotStart;
	float m_fSnapshotRate;
	float m_fLastSnapshotRate;
};

POP_PRAGMA_WARNINGS

#endif
