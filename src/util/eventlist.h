// util/eventlist.h
// util/eventlist.h
// util/eventlist.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__eventlist_H__
#define deferred__util__eventlist_H__

#include "IEvent.h"

namespace ev
{
	enum
	{
		//keyboard
		KEYBOARD_PRESSED = 0,
		KEYBOARD_RELEASED,

		//mouse
		MOUSE_PRESSED,
		MOUSE_RELEASED,
		MOUSE_POS_UPDATE,

		//system
		WINDOW_RESIZED,
		SWITCH_FULLSCREEN,

		//client inputs
		CAR_STEER_UPDATE,
		CAR_ACCEL_UPDATE,
		CAR_UPDOWN_UPDATE,
		CAR_RESET,
		CAR_BRAKE_UPDATE,
		
		//gui
		SCOREBOARD_SHOW,
		SCOREBOARD_HIDE,

		//other
		SET_CLIENT_NAME,

		//server output
		//CAR_MOTOR_UPDATE,

		//Collision events
		COLLISION_OCCURED,

		//Camera
		CAMERA_CREATE,

		//MODELS
		MODEL_STATIC_CREATE,

		//GENERAL
		//ENT_POS_UPDATE,
		//ENT_ORI_UPDATE,
		ENT_CREATE,
		ENT_CREATE_INITIAL,
		ENT_DELETE,

		//WORLD
		WORLD_CREATED,
		
		PLAYER_CAR_CREATE,
		SET_LOCAL_CLIENT_ID,

		TELL_SNAPSHOT_TIME,
		SERVER_SNAPSHOT_START,
		SERVER_SNAPSHOT_END,

		REMOTE_LISTENER_REGISTER,
		CLIENT_FINISHED_REGISTER,

		TIME_SYNC_MESSAGE,
		PING_REQUEST,
		PING_ANSWER,

		START_PHYS_DEBUG,
		SET_PHYS_DEBUGGER,

		NETTABLE_UPDATE,
		NETTABLE_UPDATE_INTERP,

		LAST_EVENT,
		ALL_EVENTS,
	};
}


//DUMMY FOR CREATING NEW EVENTS:
//class <EventName> : public IEvent
//{
//public:
//	<EventName>( InStream &in ) { IEVENT_DESERIALIZE(); <Additional Deserializations> }
//	void Serialize( OutStream &out ) const { IEVENT_SERIALIZE(); <Additional Serializations> }
//	<EventName>( IEVENT_CONSTR_PARAMS, <Additional Constructor Params> ) : IEVENT_CONSTR_CALLS, <Additional Constructor Calls> { }
//	<Additonal Get Data Functions>
//private:
//	<Additional Data Types>
//};


//Event Classes:

//######################################################
// CNoDataEvent
//######################################################
class CNoDataEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CNoDataEvent);
public:
	IEVENT_CONSTR_DEFAULT(CNoDataEvent)
	void Deserialize( InStream &in ) { }
	void Serialize( OutStream &out ) const { }
};

class CWindowResizedEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CWindowResizedEvent);
public:
	IEVENT_CONSTR_DEFAULT(CWindowResizedEvent)
	void Deserialize( InStream &in ) { in >> m_widthBefore >> m_heightBefore >> m_widthAfter >> m_heightAfter; }
	void Serialize( OutStream &out ) const { out << m_widthBefore << m_heightBefore << m_widthAfter << m_heightAfter; }
	CWindowResizedEvent(IEVENT_CONSTR_PARAMS, int widthBefore, int heightBefore, int widthAfter, int heightAfter) :
		IEVENT_CONSTR_CALLS,
		m_widthBefore(widthBefore),
		m_heightBefore(heightBefore),
		m_widthAfter(widthAfter),
		m_heightAfter(heightAfter)
	{}

	int GetWidthBefore() { return m_widthBefore; }
	int GetHeightBefore() { return m_heightBefore; }
	int GetWidthAfter() { return m_widthAfter; }
	int GetHeightAfter() { return m_heightAfter; }
private:
	int m_widthBefore;
	int m_heightBefore;
	int m_widthAfter;
	int m_heightAfter;
};


//######################################################
// CCollisionEvent
//######################################################
class CCollisionEvent: public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CCollisionEvent);
public:
	IEVENT_CONSTR_DEFAULT(CCollisionEvent)
	void Deserialize( InStream &in ) { in >> m_vPos; in >> m_fForce; }
	void Serialize( OutStream &out ) const { out << m_vPos; out << m_fForce; }
	CCollisionEvent( IEVENT_CONSTR_PARAMS, const Vector3f &vPos, float fForce ) 
		: IEVENT_CONSTR_CALLS, m_vPos(vPos), m_fForce(fForce) { }
	float GetForce( void ) { return m_fForce; }
	const Vector3f &GetPosition( void ) { return m_vPos; }
private:
	Vector3f m_vPos;
	float m_fForce;
};


//######################################################
// CCarMotorUpdateEvent
//######################################################
class CCarMotorUpdateEvent: public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CCarMotorUpdateEvent);
public:
	IEVENT_CONSTR_DEFAULT(CCarMotorUpdateEvent)
	void Deserialize( InStream &in ) { in >> m_iLogicEntId >> m_iRpm >> m_chLoad >> m_chGear; }
	void Serialize( OutStream &out ) const { out << m_iLogicEntId << m_iRpm << m_chLoad << m_chGear; }
	CCarMotorUpdateEvent( IEVENT_CONSTR_PARAMS, int entId, float fRpm, float fLoad, int iGear ) 
		: IEVENT_CONSTR_CALLS,
		m_iLogicEntId(entId),
		m_iRpm((short int)fRpm),
		m_chLoad((char)(fLoad*100.0f)),
		m_chGear((char)iGear)
	{ }
	float GetRpm( void ) { return m_iRpm; }
	float GetLoad( void ) { return ((float)(m_chLoad))*0.01f; }
	int GetGear( void ) { return (int)m_chGear; }
	int GetLogicalEnt( void ) { return m_iLogicEntId; }
private:
	USHORT m_iLogicEntId;
	short int m_iRpm;
	char m_chLoad;
	char m_chGear;
};


//######################################################
// CCarUpdateEvent
//######################################################
class CCarUpdateEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CCarMotorUpdateEvent);
public:
	IEVENT_CONSTR_DEFAULT(CCarUpdateEvent)
	void Deserialize( InStream &in ) {
		Angle3dCompressed angs[4];
		Angle3dCompressed ang;
		Vector3fCompressed vecs[4];
		
		in >> m_iLogicEntId >> m_carPos >> ang;
		in.read((char*)vecs, sizeof(Vector3fCompressed)*4);
		in.read((char*)angs, sizeof(Angle3dCompressed)*4);
		in >> m_iRpm >> m_chLoad >> m_chGear;

		//uncompress
		m_carOri = ang.toAngle();
		for( int i = 0; i < 4; i++ ) {
			m_wheelPossRelative[i] = vecs[i].toVector(15.0f);
			m_wheelAngles[i] = angs[i].toAngle();
		}
	}

	void Serialize( OutStream &out) const {
		out << m_iLogicEntId << m_carPos << Angle3dCompressed(m_carOri);
		for( int i = 0; i < 4; i++ )
			out << Vector3fCompressed(m_wheelPossRelative[i], 15.0f);
		for( int i = 0; i < 4; i++ )
			out << Angle3dCompressed(m_wheelAngles[i]);
		out << m_iRpm << m_chLoad << m_chGear;
	}

	CCarUpdateEvent( IEVENT_CONSTR_PARAMS, int entId, const Vector3f &carPos, const Vector3f &carOri,
		Vector3f wheelPoss[4], Angle3d wheelAngles[4], int rpm, float load, int gear )
		: IEVENT_CONSTR_CALLS,
		m_iLogicEntId(entId),
		m_carPos(carPos),
		m_carOri(carOri),
		m_iRpm((short int)rpm),
		m_chLoad((char)(load*100.0f)),
		m_chGear((char)gear)
	{
		memcpy(m_wheelPossRelative, wheelPoss, sizeof(Vector3f)*4);
		memcpy(m_wheelAngles, wheelAngles, sizeof(Vector3f)*4);
	}

	const Vector3f &GetPosition( void ) const { return m_carPos; }
	const Angle3d &GetOri( void ) const { return m_carOri; }
	const Vector3f &GetWheelPos(int index) const { return m_wheelPossRelative[index]; }
	const Angle3d &GetWheelAngles(int index) const { return m_wheelAngles[index]; }
	float GetRpm( void ) const { return m_iRpm; }
	float GetLoad( void ) const { return ((float)(m_chLoad))*0.01f; }
	int GetGear( void ) const { return (int)m_chGear; }
	int GetLogicalEnt( void ) const { return m_iLogicEntId; }

private:
	USHORT m_iLogicEntId;
	Vector3f m_carPos;
	Angle3d m_carOri;
	Vector3f m_wheelPossRelative[4];
	Angle3d m_wheelAngles[4];
	short int m_iRpm;
	char m_chLoad;
	char m_chGear;
};


//######################################################
// CIntegerDataEvent
//######################################################
class CIntegerDataEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CIntegerDataEvent);
public:
	IEVENT_CONSTR_DEFAULT(CIntegerDataEvent)
	void Deserialize( InStream &in ) { in >> m_iValue; }
	void Serialize( OutStream &out ) const { out << m_iValue; }
	CIntegerDataEvent( IEVENT_CONSTR_PARAMS, int value ) : IEVENT_CONSTR_CALLS, m_iValue(value) { }
	int GetValue( void ) const { return m_iValue; }
private:
	int m_iValue;
};


//######################################################
// CFloatDataEvent
//######################################################
class CFloatDataEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CFloatDataEvent);
public:
	IEVENT_CONSTR_DEFAULT(CFloatDataEvent)
	void Deserialize( InStream &in ) { in >> m_fValue; }
	void Serialize( OutStream &out ) const { out << m_fValue; }
	CFloatDataEvent( IEVENT_CONSTR_PARAMS, float value ) : IEVENT_CONSTR_CALLS, m_fValue(value) { }
	float GetValue( void ) const { return m_fValue; }
private:
	float m_fValue;
};



//######################################################
// CPlayerCarCreateEvent
//######################################################
class CPlayerCarCreateEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CPlayerCarCreateEvent);
public:
	IEVENT_CONSTR_DEFAULT(CPlayerCarCreateEvent)
	void Deserialize( InStream &in ) { in.read(m_pFileName, 64); in >> m_iLogicEnt >> m_vPos >> m_aOri >> m_iPlayerCarID; }
	void Serialize( OutStream &out ) const { out.write(m_pFileName, 64); out << m_iLogicEnt << m_vPos << m_aOri << m_iPlayerCarID; }
	CPlayerCarCreateEvent(	IEVENT_CONSTR_PARAMS,
							int iLogicEntId,
							const char *pFileName,
							int iPlayerCarID,
							Vector3f vPos,
							Angle3d aOri ) 
				: IEVENT_CONSTR_CALLS,
				m_iLogicEnt(iLogicEntId),
				m_iPlayerCarID(iPlayerCarID),
				m_vPos(vPos),
				m_aOri(aOri)
	{ 
		strcpy_s(m_pFileName,pFileName);
	}

	char* const GetName( void ) { return m_pFileName; }
	int GetLogicEntity( void ) { return m_iLogicEnt; }
	const Vector3f &GetPos( void ) { return m_vPos; }
	const Angle3d &GetOri( void ) { return m_aOri; }
	int GetPlayerCarID( void ) { return m_iPlayerCarID; }

private:
	char m_pFileName[64];
	USHORT m_iLogicEnt;
	USHORT m_iPlayerCarID;
	Vector3f m_vPos;
	Angle3d m_aOri;
};


//######################################################
// CInputEvent
//######################################################
class CInputEvent: public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CInputEvent);
public:
	IEVENT_CONSTR_DEFAULT(CInputEvent)
	void Deserialize( InStream &in ) { in >> m_fVal >> m_pPlayerID; }
	void Serialize( OutStream &out ) const { out << m_fVal << m_pPlayerID; }
	CInputEvent( IEVENT_CONSTR_PARAMS, float val, int iPlayerID ) : IEVENT_CONSTR_CALLS, m_fVal(val), m_pPlayerID(iPlayerID) { }
	float GetVal( void ) const { return m_fVal; }
	int GetPlayerID( void ) const { return m_pPlayerID; }

private:
	float m_fVal;
	USHORT m_pPlayerID;
};


//######################################################
// CMouseMoveEvent
//######################################################
class CMouseMoveEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CMouseMoveEvent);
public:
	IEVENT_CONSTR_DEFAULT(CMouseMoveEvent)
	void Deserialize( InStream &in ) { in >> m_iX >> m_iY; }
	void Serialize( OutStream &out ) const { out << m_iX << m_iY; }
	CMouseMoveEvent( IEVENT_CONSTR_PARAMS, int x, int y ) : IEVENT_CONSTR_CALLS, m_iX(x), m_iY(y) { }
	int GetX( void ) const { return m_iX; }
	int GetY( void ) const { return m_iY; }

private:
	int m_iX;
	int m_iY;
};


//######################################################
// CMouseButtonEvent
//######################################################
class CMouseButtonEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CMouseButtonEvent);
public:
	IEVENT_CONSTR_DEFAULT(CMouseButtonEvent)
	void Deserialize( InStream &in ) { in >> m_x >> m_y >> m_keyCode; }
	void Serialize( OutStream &out ) const { out << m_x << m_y << m_keyCode; }
	CMouseButtonEvent( IEVENT_CONSTR_PARAMS, int keyCode, int x, int y ) : IEVENT_CONSTR_CALLS, m_keyCode(keyCode), m_x(x), m_y(y) { }
	int GetX( void ) const { return m_x; }
	int GetY( void ) const { return m_y; }
	int GetKeyCode( void ) const { return m_keyCode; }

private:
	int m_keyCode;
	int m_x;
	int m_y;
};


//######################################################
// CPosEvent
//######################################################
class CPosEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CPosEvent);
public: 
	IEVENT_CONSTR_DEFAULT(CPosEvent)
	void Deserialize( InStream &in ) { in >> m_vPos >> m_iLogicEntId; }
	void Serialize( OutStream &out ) const { out << m_vPos << m_iLogicEntId; }
	CPosEvent( IEVENT_CONSTR_PARAMS, int iLogicEntId, Vector3f vPos )
		: IEVENT_CONSTR_CALLS, m_vPos(vPos), m_iLogicEntId((USHORT)iLogicEntId) { }
	const Vector3f &GetPos( void ) const { return m_vPos; }
	int GetLogicEntity( void ) { return (int)m_iLogicEntId; }

private:
	Vector3f m_vPos;
	USHORT m_iLogicEntId;
};


//######################################################
// COriEvent
//######################################################
class COriEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, COriEvent);
public: 
	IEVENT_CONSTR_DEFAULT(COriEvent)
	void Deserialize( InStream &in ) { 
		
		Angle3dCompressed ang;
		in >> ang >> m_iLogicEntId;
		m_aOri = ang.toAngle();
	}
	void Serialize( OutStream &out ) const { out << Angle3dCompressed(m_aOri) << m_iLogicEntId; }
	COriEvent( IEVENT_CONSTR_PARAMS, int iLogicEntId, Angle3d aOri )
		: IEVENT_CONSTR_CALLS, m_aOri(aOri), m_iLogicEntId((USHORT)iLogicEntId) { }
	const Angle3d &GetOri( void ) const { return m_aOri; }
	int GetLogicEntity( void ) { return (int)m_iLogicEntId; }

private:
	Angle3d m_aOri;
	USHORT m_iLogicEntId;
};


//######################################################
// CModelCreateEvent
//######################################################
class CModelCreateEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CModelCreateEvent);
public:
	IEVENT_CONSTR_DEFAULT(CModelCreateEvent)
	void Deserialize( InStream &in ) { in.read(m_pFileName, 64); in >> m_iLogicEnt >> m_vPos >> m_aOri; }
	void Serialize( OutStream &out ) const { out.write(m_pFileName, 64); out << m_iLogicEnt << m_vPos << m_aOri; }
	CModelCreateEvent( IEVENT_CONSTR_PARAMS, int iLogicEntId, const char *pFileName, Vector3f vPos, Angle3d aOri ) 
		: IEVENT_CONSTR_CALLS, m_iLogicEnt(iLogicEntId), m_vPos(vPos), m_aOri(aOri) { strcpy_s(m_pFileName,pFileName); }
	char* const GetName( void ) { return m_pFileName; }
	int GetLogicEntity( void ) { return m_iLogicEnt; }
	const Vector3f &GetPos( void ) { return m_vPos; }
	const Angle3d &GetOri( void ) { return m_aOri; }

private:
	char m_pFileName[64];
	USHORT m_iLogicEnt;
	Vector3f m_vPos;
	Angle3d m_aOri;
};



//######################################################
// CEntityDeleteEvent
//######################################################
class CEntityDeleteEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CEntityDeleteEvent);
public:
	IEVENT_CONSTR_DEFAULT(CEntityDeleteEvent)
	void Deserialize( InStream &in ) { in >> m_iLogicEnt; }
	void Serialize( OutStream &out ) const { out << m_iLogicEnt; }
	CEntityDeleteEvent( IEVENT_CONSTR_PARAMS, int iLogicEntId ) 
		: IEVENT_CONSTR_CALLS, m_iLogicEnt(iLogicEntId) { }
	int GetLogicEntity( void ) { return m_iLogicEnt; }

private:
	USHORT m_iLogicEnt;
};


//######################################################
// CRenderWorldCreateEvent
//######################################################
class CRenderWorldCreateEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CRenderWorldCreateEvent);
public:
	IEVENT_CONSTR_DEFAULT(CRenderWorldCreateEvent)
	void Deserialize( InStream &in ) { in.read(m_pFileName, 64); in >> m_iLogicEnt >> m_vPos >> m_aOri; }
	void Serialize( OutStream &out ) const { out.write(m_pFileName, 64); out << m_iLogicEnt << m_vPos << m_aOri; }
	CRenderWorldCreateEvent( IEVENT_CONSTR_PARAMS, int iLogicEntId, const char *pFileName, Vector3f vPos, Angle3d aOri ) 
		: IEVENT_CONSTR_CALLS, m_iLogicEnt(iLogicEntId), m_vPos(vPos), m_aOri(aOri) { strcpy_s(m_pFileName,pFileName); }
	char* const GetName( void ) { return m_pFileName; }
	int GetLogicEntity( void ) { return m_iLogicEnt; }
	const Vector3f &GetPos( void ) { return m_vPos; }
	const Angle3d &GetOri( void ) { return m_aOri; }

private:
	char m_pFileName[64];
	USHORT m_iLogicEnt;
	Vector3f m_vPos;
	Angle3d m_aOri;
};


//######################################################
// CCameraCreateEvent
//######################################################
class CCameraCreateEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CCameraCreateEvent);
public:
	IEVENT_CONSTR_DEFAULT(CCameraCreateEvent)
	void Deserialize( InStream &in ) { in >> m_iLogicEntId >> m_bIsPlayerOneCam >> m_bIsPlayerTwoCam; }
	void Serialize( OutStream &out ) const { out << m_iLogicEntId << m_bIsPlayerOneCam << m_bIsPlayerTwoCam; }
	CCameraCreateEvent( IEVENT_CONSTR_PARAMS, int iLogicEntId, bool bIsPlayerOneCam, bool bIsPlayerTwoCam )
		: IEVENT_CONSTR_CALLS, m_iLogicEntId(iLogicEntId), m_bIsPlayerOneCam(bIsPlayerOneCam), m_bIsPlayerTwoCam(bIsPlayerTwoCam) { }
	int GetLogicEntity( void ) { return m_iLogicEntId; }
	const bool &IsPlayerOneCam( void ) { return m_bIsPlayerOneCam; }
	const bool &IsPlayerTwoCam( void ) { return m_bIsPlayerTwoCam; }

private:
	USHORT m_iLogicEntId;
	bool m_bIsPlayerOneCam;
	bool m_bIsPlayerTwoCam;
};


//######################################################
// CPointerDataEvent
//######################################################
//warning: 32 / 64 bit compability NOT given, only for very rare cases
//where server and client run on the same machine in the same process.
//DO NOT USE FOR NORMAL NETWORKING
class CPointerDataEvent : public IEventStaticSized
{
	DECLARE_CLASS(IEventStaticSized, CPointerDataEvent);
public:
	IEVENT_CONSTR_DEFAULT(CPointerDataEvent)
	void Deserialize( InStream &in ) { in >> m_ptr; }
	void Serialize( OutStream &out ) const { out << m_ptr; }
	CPointerDataEvent( IEVENT_CONSTR_PARAMS, void* ptr )
		: IEVENT_CONSTR_CALLS, m_ptr((long long)ptr) { }
	void *GetPointer(void) const { return (void*)m_ptr; }
private:
	long long m_ptr;
};


//######################################################
// CNettableUpdate
//######################################################
class CNettableUpdate : public IEventDynamicSized
{
	DECLARE_CLASS(IEventDynamicSized, CNettableUpdate);
public:
	CNettableUpdate(EventType type) : 
		BaseClass(type),
		m_data(NULL),
		m_dataSize(0)
	{}

	//If copyData is false, the data buffer is used directly. The buffer is deleted in the destructor.
	CNettableUpdate( IEVENT_CONSTR_PARAMS, char *data, UINT dataSize, bool copyData = true) :
		IEVENT_CONSTR_CALLS
	{
		Assert(dataSize <= IEventDynamicSized::GetPayloadSizeMax());
		if( copyData )
		{
			m_data = new char[dataSize];
			memcpy(m_data, data, dataSize);
		}
		else
			m_data = data;
		m_dataSize = dataSize;
	}

	virtual ~CNettableUpdate()
	{
		if( m_data )
			delete[] m_data;
	}

	void Deserialize(InStream &in)
	{
		Assert(m_data == NULL);
		m_dataSize = GetPayloadSize();
		m_data = new char[m_dataSize];
		in.read(m_data, m_dataSize);
	}

	void Serialize(OutStream &out) const
	{
		Assert(m_data != NULL);
		Assert(m_dataSize <= IEventDynamicSized::GetPayloadSizeMax());
		out.write(m_data, m_dataSize);
	}

	inline char *GetData() const {
		return m_data;
	}

	UINT GetDataSize() const {
		return GetPayloadSize();
	}

	CNettableUpdate* copy() const {
		return new CNettableUpdate(GetType(), m_data, m_dataSize, true);
	}

private:
	//private copy constructor
	CNettableUpdate(const CNettableUpdate &other) : 
		IEventDynamicSized(other) 
	{}
public:
	static const UINT sMaxDataSize;

private:
	char *m_data;
	UINT m_dataSize;
};


//######################################################
// CEntityCreateEvent
//######################################################
class CEntityCreateEvent : public IEventDynamicSized
{
	DECLARE_CLASS(IEventDynamicSized, CEntityCreateEvent);
public:
	CEntityCreateEvent(EventType type) : 
		BaseClass(type),
		m_classname(),
		m_index(-1),
		m_data(NULL),
		m_dataSize(0)
	{}

	CEntityCreateEvent(const CEntityCreateEvent &other) :
		BaseClass(other.m_type),
		m_index(other.m_index),
		m_classname(other.m_classname),
		m_dataSize(other.m_dataSize)
	{
		m_data = new char[m_dataSize];
		memcpy(m_data, other.m_data, m_dataSize);
	}

	//If copyData is false, the data buffer is used directly. The buffer is deleted in the destructor.
	CEntityCreateEvent(IEVENT_CONSTR_PARAMS, int index, const char *classname, char *data, UINT dataSize, bool copyData = true) :
		IEVENT_CONSTR_CALLS,
		m_index(index),
		m_classname(classname),
		m_dataSize(dataSize)
	{
		Assert(index < std::numeric_limits<USHORT>::max());
		Assert(dataSize + strlen(classname) + 3*sizeof(USHORT) <= IEventDynamicSized::GetPayloadSizeMax());
		if( copyData )
		{
			m_data = new char[dataSize];
			memcpy(m_data, data, dataSize);
		}
		else
			m_data = data;
	}

	virtual ~CEntityCreateEvent()
	{
		if( m_data )
			delete[] m_data;
	}

	void Deserialize(InStream &in)
	{
		Assert(m_data == NULL);
		USHORT index = -1;
		USHORT dataSize = 0;
		in >> index >> m_classname >> dataSize;
		m_index = index;
		m_dataSize = dataSize;
		m_data = new char[m_dataSize];
		in.read(m_data, m_dataSize);
	}

	void Serialize(OutStream &out) const
	{
		Assert(m_data != NULL);
		out << (USHORT)m_index << m_classname << (USHORT)m_dataSize;
		out.write(m_data, m_dataSize);
	}

	inline char *GetData() const {
		return m_data;
	}

	UINT GetDataSize() const {
		return m_dataSize;
	}

	int GetIndex() const {
		return m_index;
	}

	const std::string &GetClassname() const { 
		return m_classname;
	}

public:
	static const UINT sMaxDataSize;

private:
	std::string m_classname;
	int m_index;
	char *m_data;
	UINT m_dataSize;
};


//######################################################
// CStringDataEvent
//######################################################
class CStringDataEvent : public IEventDynamicSized
{
	DECLARE_CLASS(IEventDynamicSized, CStringDataEvent);
public:
	CStringDataEvent(EventType type) : 
		BaseClass(type),
		m_string()
	{}

	CStringDataEvent(const CStringDataEvent &other) :
		BaseClass(other.m_type),
		m_string(other.m_string)
	{
	}

	//If copyData is false, the data buffer is used directly. The buffer is deleted in the destructor.
	CStringDataEvent(IEVENT_CONSTR_PARAMS, const char *str) :
		IEVENT_CONSTR_CALLS,
		m_string(str)
	{
		Assert(strlen(str) + 4*sizeof(USHORT) <= IEventDynamicSized::GetPayloadSizeMax());
	}

	virtual ~CStringDataEvent()
	{
	}

	void Deserialize(InStream &in)
	{
		in >> m_string;
	}

	void Serialize(OutStream &out) const
	{
		out << m_string;
	}

	const std::string &GetString() const { 
		return m_string;
	}

private:
	std::string m_string;
};
#endif
