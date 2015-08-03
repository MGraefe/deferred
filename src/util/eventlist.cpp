// util/eventlist.cpp
// util/eventlist.cpp
// util/eventlist.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "eventlist.h"



namespace ev
{
NOINLINE void EventSystemInit( void )
{
	for( int i = 0; i < ev::LAST_EVENT; i++ )
	{
		if( !GetEventDictionary()[i] || !GetEventNameDictionary()[i] )
			error_fatal( "Not all Events are properly defined.\n"\
				"Compare definitions in eventlist.cpp and eventlist.h" );
	}
	//GetEventDictionary().resize( ev::LAST_EVENT, NULL );
	//GetEventNameDictionary().resize( ev::LAST_EVENT, NULL );
}
}

//						Classname					Event Type				Location where the
//													(unique)				Event is generated
DECLARE_EVENT_STATIC( CIntegerDataEvent,		KEYBOARD_PRESSED,			LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CIntegerDataEvent,		KEYBOARD_RELEASED,			LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CMouseButtonEvent,		MOUSE_PRESSED,				LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CIntegerDataEvent,		MOUSE_RELEASED,				LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CMouseMoveEvent,			MOUSE_POS_UPDATE,			LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CWindowResizedEvent,		WINDOW_RESIZED,				LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CIntegerDataEvent,		SWITCH_FULLSCREEN,			LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CInputEvent,				CAR_STEER_UPDATE,			LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CInputEvent,				CAR_ACCEL_UPDATE,			LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CInputEvent,				CAR_UPDOWN_UPDATE,			LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CInputEvent,				CAR_RESET,					LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CInputEvent,				CAR_BRAKE_UPDATE,			LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CFloatDataEvent,			TELL_SNAPSHOT_TIME,			LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CIntegerDataEvent,		PING_REQUEST,				LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CPointerDataEvent,		SET_PHYS_DEBUGGER,			LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CNoDataEvent,				SCOREBOARD_SHOW,			LOCATION_CLIENT )
DECLARE_EVENT_STATIC( CNoDataEvent,				SCOREBOARD_HIDE,			LOCATION_CLIENT )
DECLARE_EVENT_DYNAMIC( CNettableUpdate,			NETTABLE_UPDATE_INTERP,		LOCATION_CLIENT )
DECLARE_EVENT_DYNAMIC( CStringDataEvent,		SET_CLIENT_NAME,			LOCATION_CLIENT )

DECLARE_EVENT_STATIC( CIntegerDataEvent,		PING_ANSWER,				LOCATION_SERVER )
DECLARE_EVENT_STATIC( CCameraCreateEvent,		CAMERA_CREATE,				LOCATION_SERVER )
//DECLARE_EVENT_STATIC( CPosEvent,				ENT_POS_UPDATE,				LOCATION_SERVER )
//DECLARE_EVENT_STATIC( COriEvent,				ENT_ORI_UPDATE,				LOCATION_SERVER )
DECLARE_EVENT_STATIC( CModelCreateEvent,		MODEL_STATIC_CREATE,		LOCATION_SERVER )
DECLARE_EVENT_STATIC( CRenderWorldCreateEvent,	WORLD_CREATED,				LOCATION_SERVER )
DECLARE_EVENT_STATIC( CFloatDataEvent,			SERVER_SNAPSHOT_START,		LOCATION_SERVER )
DECLARE_EVENT_STATIC( CFloatDataEvent,			SERVER_SNAPSHOT_END,		LOCATION_SERVER )
DECLARE_EVENT_STATIC( CPlayerCarCreateEvent,	PLAYER_CAR_CREATE,			LOCATION_SERVER )
DECLARE_EVENT_STATIC( CIntegerDataEvent,		SET_LOCAL_CLIENT_ID,		LOCATION_SERVER )
DECLARE_EVENT_DYNAMIC( CEntityCreateEvent,		ENT_CREATE_INITIAL,			LOCATION_SERVER )
DECLARE_EVENT_DYNAMIC( CEntityCreateEvent,		ENT_CREATE,					LOCATION_SERVER )
DECLARE_EVENT_STATIC( CEntityDeleteEvent,		ENT_DELETE,					LOCATION_SERVER )
DECLARE_EVENT_STATIC( CCollisionEvent,			COLLISION_OCCURED,			LOCATION_SERVER )
//DECLARE_EVENT_STATIC( CCarMotorUpdateEvent,		CAR_MOTOR_UPDATE,			LOCATION_SERVER )
DECLARE_EVENT_STATIC( CPointerDataEvent,		START_PHYS_DEBUG,			LOCATION_SERVER )
DECLARE_EVENT_DYNAMIC( CNettableUpdate,			NETTABLE_UPDATE,			LOCATION_SERVER )

DECLARE_EVENT_STATIC( CNoDataEvent,				CLIENT_FINISHED_REGISTER,	LOCATION_NONE ) 
DECLARE_EVENT_STATIC( CNoDataEvent,				TIME_SYNC_MESSAGE,			LOCATION_NONE )

DECLARE_EVENT_STATIC( CIntegerDataEvent,		REMOTE_LISTENER_REGISTER,	LOCATION_BOTH ) //location both = dont send over the wire


const UINT CNettableUpdate::sMaxDataSize = IEventDynamicSized::GetPayloadSizeMax();
