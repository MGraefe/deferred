// client/CLoaderThread.cpp
// client/CLoaderThread.cpp
// client/CLoaderThread.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "CLoaderThread.h"
#include "CClientInterf.h"
#include <util/CEventManager.h>
#include "CClientPlayerCar.h"
#include "sound/CSoundSystem.h"
#include <util/CScriptParser.h>
#include "datadesc_client.h"
#include <renderer/CRenderList.h>
#include <renderer/CRenderWorld.h>
#include <renderer/CSkyBox.h>
#include <renderer/CSunFlare.h>
#include <renderer/CLoadingMutex.h>
#include <renderer/IWindow.h>
#include <util/CConVar.h>
#include "CEntityCreateListener.h"
#include <util/StringUtils.h>


CLoaderThread *g_pLoaderThread = NULL;


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
bool CLoaderThread::HandleLoadEvent( const IEvent *pEvent )
{
		switch( pEvent->GetType() )
		{
		case ev::ENT_CREATE_INITIAL:
		case ev::ENT_CREATE:
			{
				CEntityCreateListener::HandleEventStatic(pEvent);
				return true;
			}
		case ev::WORLD_CREATED:
			{
				CRenderWorldCreateEvent *pEvt = (CRenderWorldCreateEvent*)pEvent;
				LoadWorld( pEvt->GetName() );
				return true;
			}
		} //switch

		return false;
}


//---------------------------------------------------------------
// Purpose: Queue all Events until WORLD_CREATED, 
//			which is always the last resource to load
//---------------------------------------------------------------
bool CLoaderThread::HandleEvent( const IEvent *evt )
{
	if( !m_bFinishedLoading )
	{
		switch( evt->GetType() )
		{
		case ev::ENT_CREATE_INITIAL:
		case ev::ENT_CREATE:
			{
				m_EventList.push_back( new CEntityCreateEvent( *(CEntityCreateEvent*)evt ) );
				return true;
			}
		case ev::WORLD_CREATED:
			m_EventList.push_back( new CRenderWorldCreateEvent( *(CRenderWorldCreateEvent*)evt ) );
			StartLoading();
			return true;
		default:
			return false;
		}
	}
	else //When we already loaded just handle the event sychronously
	{
		return HandleLoadEvent( evt );
	}
	
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLoaderThread::RegisterEvents( void )
{
	CEventManager *pMgr = g_ClientInterf->GetClientEventMgr();
	pMgr->RegisterListener( ev::ENT_CREATE_INITIAL, this );
	pMgr->RegisterListener( ev::ENT_CREATE, this );
	pMgr->RegisterListener( ev::WORLD_CREATED, this );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLoaderThread::UnregisterEvents( void )
{
	CEventManager *pMgr = g_ClientInterf->GetClientEventMgr();
	pMgr->UnregisterListener( ev::ENT_CREATE_INITIAL, this );
	pMgr->RegisterListener( ev::ENT_CREATE, this );
	pMgr->UnregisterListener( ev::WORLD_CREATED, this );
}

CConVar client_load_async("client.load.async", "1");

//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLoaderThread::StartLoading( void )
{
	//Mute all sounds
	g_ClientInterf->GetSoundSystem()->SetMasterVolume( 0.0f );

	if( g_ClientInterf->GetWindow()->HasSecondContext() && client_load_async.GetBool() )
	{
		ConsoleMessage("CLoaderThread: Starting loading resources async");
		Start();
	}
	else //we where unable to load a second rendering context
	{
		ConsoleMessage("CLoaderThread: Starting loading resources sync");
		StartSync();
		//TODO: write threaded solution via wglMakeCurrent everywhere
	}
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLoaderThread::ThreadRun( void )
{
	g_ClientInterf->BindRenderingContext( LOADING_THREAD );
	
	ConsoleMessage("CLoaderThread: Started Loading resources");

	while( !m_EventList.empty() )
	{
		IEvent *pIEvent = m_EventList.front();
		
		//This->m_LoadMutex.SetOrWait();

		HandleLoadEvent( pIEvent );
		
		//This->m_LoadMutex.Release();

		m_EventList.pop_front();
		delete pIEvent;
	} //while

	g_ClientInterf->UnBindRenderingContext( LOADING_THREAD );
	
	ConsoleMessage("CLoaderThread: Finished loading resources");

	//finished loading, set the event
	g_LoadingFinishedEvent->Trigger();

	m_bFinishedLoading = true;

	//unmute all sounds
	g_ClientInterf->GetSoundSystem()->SetMasterVolume( 1.0f );
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLoaderThread::LoadWorld( const char *scriptname )
{
	std::string scriptpath = string("models/") + scriptname + ".dfmap";
	CScriptParser parser;
	if( !parser.ParseFile(scriptpath.c_str()) )
	{
		error("Client: could not load world-script \"%s\"", scriptpath.c_str());
		return;
	}

	CScriptSubGroup *worldgroup = parser.GetSubGroup("WORLD");
	if( !worldgroup ) 
	{
		error("Client: Could not find subgroup \"WORLD\""
			"in world-script \"%s\"", scriptpath.c_str());
		return;
	}

	CScriptSubGroup *keyvalsGroup = worldgroup->GetSubGroup("keyvals");
	if( !keyvalsGroup )
	{
		error("Client: Could not find subgroup \"keyvals\""
			"in Group \"WORLD\" in world-script \"%s\"", scriptpath.c_str());
		return;
	}


	//init the renderable world
	CRenderList *pRenderList = g_ClientInterf->GetRenderer()->GetRenderList();
	CRenderWorld *pWorld = new CRenderWorld();
	pWorld->LoadWorld( parser );
	pRenderList->AddStaticEntity( pWorld );
	g_ClientInterf->GetRenderer()->SetWorld(pWorld);

	//Init the skybox
	std::string skyname;
	std::string suntex;
	if( !keyvalsGroup->GetString("skyname", skyname) )
		error("Could not find skyname in world-script!");
	else
		g_ClientInterf->GetRenderer()->GetSkyBox()->LoadSkyTextures( skyname.c_str() );
	
	if( !keyvalsGroup->GetString("suntex", suntex) )
		error("Could not find suntex in world-script!");
	else
	{
		g_ClientInterf->GetRenderer()->GetSunFlare()->Init(suntex.c_str(), keyvalsGroup->GetFloat("sunsize", 0.05f));
	}

	std::string seaColorDark;
	if( !keyvalsGroup->GetString("seaColorDark", seaColorDark) )
		error("Could not find seaColorDark in world-script!");
	else
		g_ClientInterf->GetRenderer()->SetSeaColorDark(StrUtils::GetVector3(seaColorDark) / 255.0f);

	std::string seaColorBright;
	if( !keyvalsGroup->GetString("seaColorBright", seaColorBright) )
		error("Could not find seaColorBright in world-script!");
	else
		g_ClientInterf->GetRenderer()->SetSeaColorDark(StrUtils::GetVector3(seaColorBright) / 255.0f);


	//Load all entities
	//LoadEntitiesFromMapFile( scriptpath.c_str() );
}



//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLoaderThread::LoadStaticWorldLights( const char *pFilePath )
{
	//Replace "*.rpm" with "*.ents"
	std::string filepath( pFilePath );
	filepath.replace( filepath.length()-3, 3, "ents" );

	CScriptParser parser;
	if( !parser.ParseFile( filepath.c_str() ) )
	{
		error( "LoadStaticWorldLights(): Could not find entity-describtion file \"%s\".", filepath.c_str() );
		return;
	}
	
	const SubGroupMap *grpmap = parser.GetSubGroupMap();
	SubGroupMap::const_iterator it = grpmap->begin();
	SubGroupMap::const_iterator itEnd = grpmap->end();

	for(; it != itEnd; it++ )
	{
		//Get classname
		string classname;
		it->second->GetString( "classname", classname );
		if( classname == "light" )
			ParseAndAddPointLight(it->second);
	}
}


void CLoaderThread::ParseAndAddPointLight( const CScriptSubGroup *grp ) 
{
	Vector3f origin = ValveVectorToVector3f( grp->GetVector3f( "origin" ) );
	float attQuad = grp->GetFloat( "_quadratic_attn" );
	float attLin = grp->GetFloat( "_linear_attn" );
	float attConst = grp->GetFloat( "_constant_attn" );
	Vector4f color = grp->GetVector4d( "_light" );
	//color.x = pow( color.x / 255.0f, 2.2f );
	//color.y = pow( color.y / 255.0f, 2.2f );
	//color.z = pow( color.z / 255.0f, 2.2f );
	color.x /= 255.0f;
	color.y /= 255.0f;
	color.z /= 255.0f;
	//color.w /= 255.0f;

	//Scale light intensity
	//float scale = attConst + 100.0f * attLin + 10000.0f * attQuad;
	//if( scale > 0.0f )
	//	color.w *= scale;

	CPointLight *light = new CPointLight();
	light->SetAbsPos( origin );
	light->SetColor( color );
	light->SetQuadratic( attQuad );
	light->SetLinear( /*10.0*/ + attLin );
	light->SetConstant( /*100.0 +*/ attConst );
	//light.SetDist( 0.25f );

	g_ClientInterf->GetRenderer()->GetLightList()->AddPointLight(light);
}


//---------------------------------------------------------------
// Purpose: 
//---------------------------------------------------------------
void CLoaderThread::LoadEntitiesFromMapFile( const char *pFilePath )
{
	CRenderList *pRenderList = g_ClientInterf->GetRenderer()->GetRenderList();
	CLightList *pLightList = g_ClientInterf->GetRenderer()->GetLightList();

	CScriptParser parser;
	if( !parser.ParseFile( pFilePath ) )
	{
		error( "Mapfile \"%s\" not found.", pFilePath );
		return;
	}

	const SubGroupMap *subGroupMap = parser.GetSubGroupMap();

	SubGroupMap::const_iterator it = subGroupMap->begin();
	SubGroupMap::const_iterator itEnd = subGroupMap->end();

	for( ; it != itEnd; it++ )
	{
		if( it->first != "ENTITY" )
			continue;

		CScriptSubGroup *pSubGrp = it->second;

		//get classname, continue if not found
		std::string classname;
		if( !pSubGrp->GetString( "classname", classname ) )
			continue;

		//Get factory for classname

		const IEntityFactoryEditor<CRenderEntity> *pFactory = GetEditorFactoryClient(classname);
		if( !pFactory )
			continue;

		//The factory creates our event with all data
		CRenderEntity *pEnt = pFactory->CreateEntity(pSubGrp, -1);

		//pEnt->RegisterEvents( g_RenderInterf->GetRenderEventMgr() );

		//Check for the very special light types
		if( dynamic_cast<CPointLight*>(pEnt) )
			pLightList->AddPointLight((CPointLight*)pEnt);
		else if( dynamic_cast<CSpotLight*>(pEnt) )
			pLightList->AddSpotLight( (CSpotLight*)pEnt );
		else if( dynamic_cast<CSunLight*>(pEnt) )
			pLightList->AddSunLight( (CSunLight*)pEnt );
		else
			pRenderList->AddStaticEntity(pEnt);
	}
}
