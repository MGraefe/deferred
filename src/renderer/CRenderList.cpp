// renderer/CRenderList.cpp
// renderer/CRenderList.cpp
// renderer/CRenderList.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"



#include "CRenderList.h"
#include "CModel.h"
#include <util/timer.h>
#include "CCamera.h"
#include "CRenderInterf.h"
#include "renderer.h"
#include "CRenderWorld.h"
//#include <networking.h>
//#include "CClientPlayerCar.h"

void CRenderList::AddStaticEntity( CRenderEntity *const pEnt )
{
	m_StaticEnts.push_back( pEnt );
}


bool CRenderList::DeleteStaticEntity( CRenderEntity *const pEnt, bool bCleanup )
{
	RenderEntList::iterator it = m_StaticEnts.begin();
	RenderEntList::iterator itEnd = m_StaticEnts.end();

	for( ; it != itEnd; it++ )
	{
		if( *it == pEnt )
		{
			m_StaticEnts.erase( it );
			if( bCleanup )
				delete pEnt;
			return true;
		}
	}

	return false;
}


CRenderEntity* CRenderList::GetEntityByLogicIndex( int index )
{
	RenderEntList::iterator it = m_StaticEnts.begin();
	RenderEntList::iterator itEnd = m_StaticEnts.end();
	for( ; it != itEnd; it++ )
	{
		CRenderEntity *pEnt = *it;
		if( pEnt->GetIndex() == index )
			return pEnt;
	}

	return NULL;
}


void CRenderList::RestoreEntities( void )
{
	RenderEntList::iterator it = m_StaticEnts.begin();
	RenderEntList::iterator itEnd = m_StaticEnts.end();

	for( ; it != itEnd; it++ )
	{
		CRenderEntity *pEnt = *it;
		pEnt->VOnRestore();
	}
}


bool CRenderList::DeleteStaticEntity( const int &index, bool bCleanup )
{
	RenderEntList::iterator it = m_StaticEnts.begin();
	RenderEntList::iterator itEnd = m_StaticEnts.end();

	for( ; it != itEnd; it++ )
	{
		if( (*it)->GetIndex() == index )
		{
			if( bCleanup )
			{
				(*it)->VPreDelete();
				delete (*it);
			}
			m_StaticEnts.erase( it );
			return true;
		}
	}

	return false;
}


void CRenderList::UpdateInterpolation( void )
{
	float fTime = gpGlobals->curtime;
	float fSnapshotProceedTime = fTime - m_fSnapshotStart;
	float alpha = fSnapshotProceedTime * m_fSnapshotRate;
	//alpha = clamp( alpha, 0.0f, 1.0f );
	//float oneminusalpha = 1.0f - alpha;

	RenderEntList::iterator it = m_StaticEnts.begin();
	RenderEntList::iterator itEnd = m_StaticEnts.end();

	for( ; it != itEnd; it++ )
	{
		(*it)->UpdateInterpolation( alpha );
	}
}


void CRenderList::RenderStatics( const CRenderParams &params )
{
	RenderEntList::iterator it = m_StaticEnts.begin();
	RenderEntList::iterator itEnd = m_StaticEnts.end();

	for( ; it != itEnd; it++ )
	{
		CRenderEntity *pEnt = *it;
		if( pEnt->IsVisible() )
		{
			pEnt->VRender(params);
		}
	}
}



bool CRenderList::HandleEvent( const IEvent *evt )
{
	if( evt->GetType() == ev::ENT_DELETE )
	{
		CEntityDeleteEvent *pEvt = (CEntityDeleteEvent*)evt;
		this->DeleteStaticEntity( pEvt->GetLogicEntity() );
	}
	else if( evt->GetType() == ev::TELL_SNAPSHOT_TIME )
	{
		float fTime = ((CFloatDataEvent*)evt)->GetValue();
		fTime = clamp( fTime, 0.01f, 0.25f );
		m_fSnapshotRate = 1.0f/fTime;
	}
	else if( evt->GetType() == ev::SERVER_SNAPSHOT_END )
	{
		m_fSnapshotStart = ToClientTime(gpGlobals->serveroffset, ((CFloatDataEvent*)evt)->GetValue()) + g_InterpVal;
	}

	return false;
}


void CRenderList::RegisterEvents( CEventManager* const pMgr )
{
	pMgr->RegisterListener( ev::ENT_DELETE, this );
	pMgr->RegisterListener( ev::SERVER_SNAPSHOT_END, this );
	pMgr->RegisterListener( ev::TELL_SNAPSHOT_TIME, this );
	m_fSnapshotStart = 0.0f;
	m_fSnapshotRate = 33.0f;
	m_fLastSnapshotRate = 33.0f;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CRenderList::CRenderList()
{
	m_fSnapshotStart = 0.0f;
	m_fSnapshotRate = 33.0f;
	m_fLastSnapshotRate = 33.0f;
	m_pAppLayerEvtMgr = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CRenderList::~CRenderList()
{
	RenderEntList::iterator it = m_StaticEnts.begin();
	RenderEntList::iterator itEnd = m_StaticEnts.end();

	for( ; it != itEnd; it++ )
	{
		CRenderEntity *pEnt = *it;
		if(pEnt)
		{
			delete pEnt;
			pEnt = NULL;
		}
	}
}

