// editor/CEditorEntityOutputList.cpp
// editor/CEditorEntityOutputList.cpp
// editor/CEditorEntityOutputList.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#include "stdafx.h"
#include "CEditorEntityOutputList.h"


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntityOutputList::CEditorEntityOutputList()
{

}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntityOutputList::CEditorEntityOutputList( const CEditorEntityOutputList &other )
{
	CloneOutputList(other, *this);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntityOutputList::~CEditorEntityOutputList()
{
	cleanup();
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEditorEntityOutputList & CEditorEntityOutputList::operator=( const CEditorEntityOutputList &other )
{
	cleanup();
	CloneOutputList(other, *this);
	return *this;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntityOutputList::CloneOutputList( const CEditorEntityOutputList &from, CEditorEntityOutputList &to )
{
	for(const CEntityOutput *output : from)
		to.push_back(new CEntityOutput(*output));
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEditorEntityOutputList::cleanup()
{
	for(CEntityOutput *output : m_data)
		delete output;
	m_data.clear();
}



