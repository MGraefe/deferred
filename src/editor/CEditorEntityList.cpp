// editor/CEditorEntityList.cpp
// editor/CEditorEntityList.cpp
// editor/CEditorEntityList.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "CEditorEntityList.h"
#include "CEditorEntity.h"
#include "OutputWnd.h"
#include "IEditorEntityFactory.h"
#include <util/CScriptParser.h>
#include "CEntityRegister.h"
#include "OutputWnd.h"
#include "editorDoc.h"
#include "IEditorTool.h"
#include "C3dView.h"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntityList::CEditorEntityList()
{
	m_indexNumerator = 0;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntityList::CEditorEntityList( const CEditorEntityList &list ) :
	m_indexNumerator(list.m_indexNumerator),
	m_ents(list.m_ents.size(), NULL)
{
	for( UINT i = 0; i < list.m_ents.size(); i++ )
		m_ents[i] = list.m_ents[i]->Clone();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntity* CEditorEntityList::InsertNewEntity( const std::string &classname )
{
	IEditorEntityFactory *pFactory = GetFactory(classname);
	if( !pFactory )
		return NULL;

	CEditorEntity *pEnt = pFactory->CreateEntity( classname, m_indexNumerator++ );
	Assert(pEnt);

	m_ents.push_back(pEnt);
	pEnt->OnCreate();
	return pEnt;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntityList::InsertExistingEntity( CEditorEntity *ent )
{
	m_ents.push_back(ent);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntityList::Deserialize( const CScriptSubGroup *pGrp, std::vector<CEditorEntity*> *outAddedEnts /*= NULL*/ )
{
	SubGroupMap::const_iterator it = pGrp->GetSubGroupMap()->begin();
	SubGroupMap::const_iterator itEnd = pGrp->GetSubGroupMap()->end();

	for( ; it != itEnd; it++ )
	{
		const CScriptSubGroup *pEntGrp = it->second;
		std::string classname;
		if( !pEntGrp->GetString("classname", classname) )
		{
			PostOutput( _T("could not find classname of entity") );
			continue;
		}

		IEditorEntityFactory *pFactory = GetFactory(classname);
		if( !pFactory )
			continue;

		CEditorEntity *pEnt = pFactory->CreateEntity( pEntGrp, m_indexNumerator++ );
		if( !pEnt )
			continue;

		m_ents.push_back(pEnt);
		if(outAddedEnts)
			outAddedEnts->push_back(pEnt);
		pEnt->OnCreate();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntityList::Serialize( outstr &str )
{
	for( UINT i = 0; i < m_ents.size(); i++ )
	{
		if( m_ents[i] )
			m_ents[i]->Serialize( str );
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntityList::RemoveEntity( CEditorEntity *pEnt, bool bDelete )
{
	_RemoveEntity( GetEntListIndex(pEnt), bDelete );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntityList::RemoveEntity( int entIndex, bool bDelete )
{
	_RemoveEntity( GetEntListIndex(entIndex), bDelete );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntity *CEditorEntityList::ChangeEntityClassname( CEditorEntity *pEnt, const std::string &cnNew )
{
	//TODO: write this one
	return NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
IEditorEntityFactory *CEditorEntityList::GetFactory( const std::string &classname )
{
	//Get internal entity type for classname
	CEntityRegisterEntity *ent = GetEntityRegister()->GetEntity(classname);
	if( !ent )
	{
		PostOutput( _T("Could not find entity \"%S\""), classname.c_str() );
		return NULL;
	}

	std::string type = ent->GetInternalType();
	if( type.empty() )
	{
		PostOutput( _T("Could not retrieve internal type of \"%S\""), classname.c_str() );
		return NULL;
	}

	//Get factory for entity type
	auto it = IEditorEntityFactory::GetList().find( type );
	if( it == IEditorEntityFactory::GetList().end() )
	{
		PostOutput( _T("Could not find internal type \"%S\" of \"%S\""), type.c_str(), classname.c_str() );
		return NULL;
	}

	Assert(it->second);
	return it->second;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CEditorEntityList::GetEntListIndex( CEditorEntity *pEnt )
{
	for( UINT i = 0; i < m_ents.size(); i++ )
		if( m_ents[i] == pEnt )
			return i;
	return -1;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CEditorEntityList::GetEntListIndex( int entIndex )
{
	for( UINT i = 0; i < m_ents.size(); i++ )
		if( m_ents[i] && m_ents[i]->GetIndex() == entIndex )
			return i;
	return -1;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntityList::_RemoveEntity( int listIndex, bool bDelete )
{
	if( listIndex < 0 )
		return;

	if( bDelete )
	{
		m_ents[listIndex]->OnDestroy();
		delete m_ents[listIndex];
	}

	m_ents.erase( m_ents.begin()+listIndex );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntityList::~CEditorEntityList()
{
	for( UINT i = 0; i < m_ents.size(); i++ )
		if( m_ents[i] )
			delete m_ents[i];
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntityList::ReplaceEntity( CEditorEntity *pOld, CEditorEntity *pNew )
{
	int index = GetEntListIndex(pOld);
	Assert(index >= 0);
	m_ents[index] = pNew;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntity * CEditorEntityList::TraceLine( const Vector3f &start, const Vector3f &dir )
{
	float hitDist = std::numeric_limits<float>::infinity();
	CEditorEntity *pEnt = NULL;

	tracelineinfo_t info;
	info.start = start;
	info.dir = dir;
	info.distMax = std::numeric_limits<float>::infinity();

	for( UINT i = 0; i < m_ents.size(); i++ )
	{
		if( m_ents[i]->TraceLine( info ) && info.distHit > 1.0f && info.distHit < hitDist )
		{
			hitDist = info.distHit;
			pEnt = m_ents[i];
		}
	}

	return pEnt;
}


bool sortEditorEntityByZ (const CEditorEntity *l, const CEditorEntity *r)
{
	return l->GetZVal() > r->GetZVal(); //sort descending, highest z gets drawn first
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntityList::RenderDebug( void )
{
	Assert(C3dView::GetActive() && C3dView::GetActive()->GetCamera());
	CCamera *cam =  C3dView::GetActive()->GetCamera();
	Vector3f camPos = cam->GetAbsPos();
	Vector3f camDir = cam->GetAbsAngles() * Vector3f(0.0f, 0.0f, -1.0f);
	for( CEditorEntity* ent : m_ents )
		ent->UpdateZVal(camPos, camDir);

	std::vector<CEditorEntity*> sortedEnts = m_ents;
	std::sort(sortedEnts.begin(), sortedEnts.end(), sortEditorEntityByZ);

	for( CEditorEntity *ent : sortedEnts )
		ent->Render();

	if( GetActiveDocument() && GetActiveDocument()->GetActiveTool() )
		GetActiveDocument()->GetActiveTool()->Render3d();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
int CEditorEntityList::FindNumberByName( const std::string &name ) const
{
	int counter = 0;
	for(const CEditorEntity *ent : m_ents)
		if(ent->GetName() == name)
			counter++;
	return counter;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::vector<CEditorEntity*> CEditorEntityList::FindByName( const std::string &name ) const
{
	std::vector<CEditorEntity*> result;
	for(CEditorEntity *ent : m_ents)
		if( ent->GetName() == name )
			result.push_back(ent);
	return result;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntity* CEditorEntityList::FindByIndex( int index ) const
{
	for(CEditorEntity *ent : m_ents)
		if(ent->GetIndex() == index)
			return ent;
	return NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntity *CEditorEntityList::DuplicateEntity( const CEditorEntity * oldEnt, bool insertInList /*= true*/ )
{
	CEditorEntity *newEnt = oldEnt->Copy(m_indexNumerator++);
	if(insertInList)
		InsertExistingEntity(newEnt);
	return newEnt;
}

