// editor/CEntityTypeWorld.cpp
// editor/CEntityTypeWorld.cpp
// editor/CEntityTypeWorld.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CEntityTypeWorld.h"
#include "IEditorEntityFactory.h"
#include <renderer/include.h>
#include <renderer/CRenderWorld.h>
#include <renderer/CSkyBox.h>
#include <renderer/CSunFlare.h>
#include "renderer.h"
#include "CGLResources.h"
#include <util/StringUtils.h>

DECLARE_EDITOR_ENTITY_TYPE("WORLD", CEntityTypeWorld)


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
std::string CEntityTypeWorld::GetSerializationHeaderString( void ) const
{
	return string("WORLD");
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityTypeWorld::CEntityTypeWorld( const CScriptSubGroup *pGrp, int index ) 
	: BaseClass(pGrp, index)
{
	m_renderworld = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityTypeWorld::CEntityTypeWorld( const std::string &classname, int index )
	: BaseClass(classname, index)
{
	m_renderworld = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
CEntityTypeWorld::CEntityTypeWorld( const CEntityTypeWorld &other, int index )
	: BaseClass(other, index)
{
	m_renderworld = NULL;
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeWorld::OnCreate( void )
{
	BaseClass::OnCreate();
	loadWorld();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeWorld::OnDestroy( void )
{
	BaseClass::OnDestroy();
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeWorld::OnParamChanged( const std::string &key, const std::string &oldval, const std::string &newval )
{
	BaseClass::OnParamChanged(key, oldval, newval);

	if( key.compare("filename") == 0 )
		loadWorld();
	else if( key.compare("suntex") == 0 )
		GetRenderInterfaces()->GetRendererInterf()->GetSunFlare()->SetTexture(newval.c_str());
	else if( key.compare("sunsize") == 0 )
		GetRenderInterfaces()->GetRendererInterf()->GetSunFlare()->SetSize(StrUtils::GetFloat(newval));
	else if( key.compare("seaColorDark") == 0 )
		GetRenderInterfaces()->GetRendererInterf()->SetSeaColorDark(StrUtils::GetVector3(newval) / 255.0f);
	else if( key.compare("seaColorBright") == 0 )
		GetRenderInterfaces()->GetRendererInterf()->SetSeaColorBright(StrUtils::GetVector3(newval) / 255.0f);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CEntityTypeWorld::loadWorld( void )
{
	std::string worldname = GetValue("filename");
	if( worldname.length() > 0 )
		m_renderworld = GetGLResources()->LoadWorldFromFile(worldname.c_str());

	std::string skyname = GetValue("skyname");
	std::string suntex = GetValue("suntex");
	float sunSize = StrUtils::GetFloat(GetValue("sunsize"));
	if( skyname.length() > 0 )
		GetRenderInterfaces()->GetRendererInterf()->GetSkyBox()->LoadSkyTextures( skyname.c_str() );
	if( suntex.length() > 0 )
		GetRenderInterfaces()->GetRendererInterf()->GetSunFlare()->Init(suntex.c_str(), sunSize);
}
