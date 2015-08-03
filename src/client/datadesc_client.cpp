// client/datadesc_client.cpp
// client/datadesc_client.cpp
// client/datadesc_client.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "datadesc_client.h"
#include <renderer/CLightList.h>

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
EditorFactoryMapClient &GetEditorFactoryMapClient( void )
{
	static EditorFactoryMapClient factMap;
	return factMap;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
const IEntityFactoryEditor<CRenderEntity> *GetEditorFactoryClient( const std::string name )
{
	EditorFactoryMapClient::const_iterator it = GetEditorFactoryMapClient().find(name);
	if( it == GetEditorFactoryMapClient().end() )
		return NULL;
	else
		return it->second;
}



//DECLARE DATADESC FOR PURE RENDERER ENTITIES HERE
LINK_ENTITY_TO_CLASSNAME_CLIENT_RENDERER(CPointLight, light);
LINK_ENTITY_TO_CLASSNAME_CLIENT_RENDERER(CSpotLight, light_spot);
LINK_ENTITY_TO_CLASSNAME_CLIENT_RENDERER(CSunLight, light_sun);
LINK_ENTITY_TO_CLASSNAME_CLIENT_RENDERER(CSpotTexLight, light_spot_tex);

