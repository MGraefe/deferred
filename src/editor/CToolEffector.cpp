// editor/CToolEffector.cpp
// editor/CToolEffector.cpp
// editor/CToolEffector.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "CToolEffector.h"
#include "editorDoc.h"
#include "CEditorEntity.h"

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolEffector::CToolEffector( const Vector3f &color )
{
	m_baseColor = color;
	m_axis = Vector3f(1.0f, 0.0f, 0.0f);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CToolEffector::~CToolEffector()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CToolEffector::SetSelected( bool selected /*= true */ )
{
	bool ret = m_selected;
	m_selected = selected;
	return ret;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CToolEffector::SetAxis( const Vector3f &axis )
{
	m_axis = axis;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CToolEffector::IsAnySelectedEntityTranslatable( void )
{
	const SelectedEntityList &ents = GetActiveDocument()->GetSelectedEntityList();
	for( SelectedEntityList::const_iterator it = ents.begin(); it != ents.end(); ++it )
		if( (*it)->IsTranslatable() )
			return true;
	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CToolEffector::IsAnySelectedEntityRotatable( void )
{
	const SelectedEntityList &ents = GetActiveDocument()->GetSelectedEntityList();
	for( SelectedEntityList::const_iterator it = ents.begin(); it != ents.end(); ++it )
		if( (*it)->IsRotatable() )
			return true;
	return false;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CToolEffector::IsAllSelectedEntitiesTranslatable( void )
{
	const SelectedEntityList &ents = GetActiveDocument()->GetSelectedEntityList();
	for( SelectedEntityList::const_iterator it = ents.begin(); it != ents.end(); ++it )
		if( !(*it)->IsTranslatable() )
			return false;
	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CToolEffector::IsAllSelectedEntitiesRotatable( void )
{
	const SelectedEntityList &ents = GetActiveDocument()->GetSelectedEntityList();
	for( SelectedEntityList::const_iterator it = ents.begin(); it != ents.end(); ++it )
		if( !(*it)->IsRotatable() )
			return false;
	return true;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const Vector3f & CToolEffector::GetAxis( void )
{
	return m_axis;
}
