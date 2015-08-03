// server/CTrigger.cpp
// server/CTrigger.cpp
// server/CTrigger.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "CTrigger.h"
#include "ServerInterfaces.h"
#include "datadesc_server.h"

LINK_ENTITY_TO_CLASSNAME_SERVER(CTrigger, trigger);

BEGIN_DATADESC_EDITOR(CTrigger)
	//FIELD_FLOAT_ARRAY("verts", m_verts, 8*3)
	FIELD_FLOAT("width", m_sizex)
	FIELD_FLOAT("height", m_sizey)
	FIELD_FLOAT("depth", m_sizez)
END_DATADESC_EDITOR()

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CTrigger::CTrigger()
{
	m_sizex = m_sizey = m_sizez = 1.0f;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTrigger::OnDatadescFinished( void )
{
	BaseClass::OnDatadescFinished();

	Vector3f verts[8];
	for(int i = 0; i < 8; i++)
	{
		verts[i] = Vector3f((i % 8 >= 4 ? -0.5f : 0.5f) * m_sizex,
							(i % 4 >= 2 ? -0.5f : 0.5f) * m_sizey,
							(i % 2 >= 1 ? -0.5f : 0.5f) * m_sizez);
	}

	SetCollidable(false);
	singletons::g_pPhysics->VCreateTrigger(this, verts);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTrigger::OnCollisionStart( const CCollisionInfo &info )
{
	CBaseEntity *activator = info.ent1 != this ? info.ent1 : info.ent2;
	FireOutput("OnTrigger", activator);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CTrigger::Destroy( void )
{
	singletons::g_pPhysics->VRemoveEntity(this);

	BaseClass::Destroy();
}



////---------------------------------------------------------------
//// Purpose: 
////---------------------------------------------------------------
//void CTrigger::SetVerts( Vector3f *verts8 )
//{
//	//Just to be sure
//	singletons::g_pPhysics->VRemoveEntity(this);
//
//	for( int i = 0; i < 8; i++ )
//		m_verts[i] = verts8[i];
//
//	singletons::g_pPhysics->VCreateTrigger(this, m_verts);
//}
