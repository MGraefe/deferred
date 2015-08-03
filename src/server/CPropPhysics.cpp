// server/CPropPhysics.cpp
// server/CPropPhysics.cpp
// server/CPropPhysics.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "CPropPhysics.h"
#include "CBulletPhysics.h"
#include <util/CEventManager.h>
#include "ServerInterfaces.h"
#include <util/CPhysFile.h>
#include "datadesc_server.h"

LINK_ENTITY_TO_CLASSNAME_SERVER(CPropPhysics, prop_physics);

BEGIN_DATADESC_EDITOR(CPropPhysics)
	FIELD_INPUTFUNC("Test", InputTest)
	FIELD_BOOL("static", m_static)
END_DATADESC_EDITOR()



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CPropPhysics::CPropPhysics()
{
	m_static = false;
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPropPhysics::LoadModel()
{
	//Try to open the accompany physics-file
	std::string m_sPhysFilename(GetFilename());
	m_sPhysFilename.append(".phys");
	CPhysFile *physFile = new CPhysFile();
	error_e err = physFile->Deserialize(m_sPhysFilename.c_str());
	if( err == ERR_NONE )
	{
		g_pPhysics->VCreateFromPhysFile( this, physFile, m_static );
	}
	else
	{
		error(	"Creating physics-prop with Model \"%s\" WITHOUT physical information,"
				"now creating as static (not adding to physical simulation). The System reported the error: %s",
				m_sPhysFilename.c_str(), ToString(err));
	}
	delete physFile;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPropPhysics::SetOriginAndUpdatePhysics( const Vector3f &pos )
{
	SetOrigin( pos );
	g_pPhysics->VSetActorOrigin( GetIndex(), pos );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPropPhysics::SetAnglesAndUpdatePhysics( const Angle3d &ang )
{
	SetAngle( ang );
	g_pPhysics->VSetActorAngles( GetIndex(), ang );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPropPhysics::SetAnglesAndOriginAndUpdatePhysics( const Vector3f &pos, const Angle3d &ang )
{
	SetOrigin( pos );
	SetAngle( ang );
	g_pPhysics->VSetActorOriginAndAngles( GetIndex(), pos, ang );
}

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPropPhysics::Destroy( void ) 
{
	g_pPhysics->VRemoveEntity(this);
	CBaseEntity::Destroy();
}

const float destroy_phys_ent_min_y = -10000.0f; //1000 meters
//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPropPhysics::Update( void )
{
	CBaseEntity::Update();
	if( this->GetOrigin().y < destroy_phys_ent_min_y ) {
		ConsoleMessage( "CPropPhysics#Update(): Destroying entity with model \"%s\" because it fell under the map", GetFilename().c_str() );
		Destroy();
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPropPhysics::OnDatadescFinished(void)
{
	BaseClass::OnDatadescFinished();
	LoadModel();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CPropPhysics::InputTest( const inputdata_t<CBaseEntity> &data )
{
	ConsoleMessage("Received Input with param %s", data.GetParam().c_str());
}

