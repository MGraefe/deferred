// util/IEvent.h
// util/IEvent.h
// util/IEvent.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__IEvent_H__
#define deferred__util__IEvent_H__

#include <map>
#include <vector>
#include "maths.h"
#include "CBinaryBufferStream.h"
#include "error.h"
#include "basic_types.h"
#include "os_specific.h"
#include "networkStreams.h"

//typedef short int EventType;
typedef BYTE EventType;

#define LOCATION_SERVER 0
#define LOCATION_CLIENT 1
#define LOCATION_NONE 2
#define LOCATION_BOTH 2

#define IEVENT_CONSTR_PARAMS EventType iIEventType
#define IEVENT_CONSTR_CALLS BaseClass(iIEventType)
//#define IEVENT_VGETSIZE(Type) int VGetSize( void ) const { return sizeof(Type); }
#define IEVENT_CONSTR_DEFAULT(classtype) classtype(EventType type) : BaseClass(type) {}

namespace ev{
	extern const char *GetEventStringName( const EventType &type );
}

#define DECLARE_EVENT_STATIC(ClassName,EventName,EventLocation) \
	namespace ev \
	{ \
		namespace util \
		{ \
			static const char EventName##_String[] = #EventName; \
			static const CEventName EventName##_Class( EventName, EventName##_String ); \
			static CEventFactoryStaticSized<ClassName> EventName##_FactoryStatic(EventName,EventLocation); \
		}\
	}

#define DECLARE_EVENT_DYNAMIC(ClassName,EventName,EventLocation) \
	namespace ev \
	{ \
		namespace util \
		{ \
			static const char EventName##_String[] = #EventName; \
			static const CEventName EventName##_Class( EventName, EventName##_String ); \
			static CEventFactoryDynamicSized<ClassName> EventName##_FactoryDynamic(EventName,EventLocation); \
		}\
	}



class IEventFactory;
typedef std::vector<IEventFactory*> EventDictionary;
typedef std::vector<const char*> EventNameDictionary;
namespace ev
{
	EventDictionary &GetEventDictionary( void );
	EventNameDictionary &GetEventNameDictionary( void );
}


class IEvent;
class IEventFactory
{
public:
	IEventFactory( EventType type, int location ) : m_type(type), m_iLocation(location) {}
	virtual IEvent *Create(void) const = 0;
	int GetLocation( void ) const;
	inline EventType GetType(void) const {return m_type;}

	void AddToList( int type ) {
		EventDictionary &dict = ev::GetEventDictionary();
		if( dict[type] != NULL )
			error_fatal( "Multiple definition of the same Event (%i). Look into eventlist.cpp", type );
		dict[type] = this;
	}
private:
	EventType m_type;
	int m_iLocation;
};


class IEventFactoryStaticSized : public IEventFactory
{
public:
	IEventFactoryStaticSized( EventType type, int location ) :
		IEventFactory(type, location)
	{}

	virtual int GetNetworkSize( void ) const = 0;
};


namespace ev
{
	IEventFactory* GetEventFactory( const EventType &eventname );
}


//virtual interface
class IEvent
{
friend class CNetSocket;
friend class CEventManager;
friend class CEventComparison;
friend class IEventFactory;

public:
	//IEvent() : m_type((EventType)(-1)) { }
	explicit	IEvent(EventType type) :
		m_iClientID(CLIENT_ID_LOCAL),
		m_iRecieverID(RECIEVER_ID_ALL),
		m_fProcessTime(-1.0f),
		m_type(type)
		{ }
	virtual		~IEvent() {}
	EventType	GetType( void ) const { return m_type; }
	bool		IsRemoteEvent( void ) { return m_iClientID != CLIENT_ID_LOCAL; }
	inline int	GetClientID( void ) const { return m_iClientID; }
	inline int	GetRecvID( void ) const { return m_iRecieverID; }

	inline virtual void VSerialize( OutStream &out ) const {
		out << m_type;
	}

	inline virtual void VDeserialize( InStream &in ) {
		in >> m_type;
	}

	inline const float &GetProcessTime( void ) const { 
		return m_fProcessTime;
	}

	inline void SetProcessTime( float time ) { 
		m_fProcessTime = time; 
	}

	virtual size_t CalcNetworkSize() const = 0;
	virtual size_t GetNetworkSize() const = 0;
	virtual void Serialize(OutStream &out) const = 0;
	virtual void Deserialize(InStream &in) = 0;
	virtual bool IsDynamicSize( void ) const = 0;

	inline static size_t GetHeaderSize() {
		return sizeof(EventType);
	}

private:
	inline void SetClientID( int id ) { m_iClientID = id; }
	inline void SetRecieverID( int id ) { m_iRecieverID = id; }

private:
	int m_iClientID;
	int m_iRecieverID;

	float m_fProcessTime;
	void SetType(EventType type) { m_type = type; }

protected:
	EventType m_type;
};


class IEventStaticSized : public IEvent
{
public:
	explicit IEventStaticSized(EventType type) : 
		IEvent(type) 
	{}

public:
	inline virtual size_t GetNetworkSize( void ) const final {
		IEventFactoryStaticSized *pFactory = (IEventFactoryStaticSized*)ev::GetEventFactory( m_type );
		if( !pFactory )
			return 0;
		else
			return pFactory->GetNetworkSize();
	}

	inline virtual size_t CalcNetworkSize() const {
		return GetNetworkSize();
	}


	inline virtual bool IsDynamicSize( void ) const final {
		return false;
	}

	inline static size_t GetHeaderSize() {
		return IEvent::GetHeaderSize();
	}

	inline size_t GetPayloadSize() const {
		return GetNetworkSize() - GetHeaderSize();
	}

	inline virtual void VSerialize(OutStream &out) const final {
		IEvent::VSerialize(out);
		Serialize(out);
	}

	inline virtual void VDeserialize(InStream &in) final {
		IEvent::VDeserialize(in);
		Deserialize(in);
	}
};


class IEventDynamicSized : public IEvent
{
private:
	friend class CNetSocket;

public:
	typedef USHORT SizeType;
	//IEventDynamicSized() : IEvent(), m_size(-1) {}
	explicit IEventDynamicSized(EventType type) : IEvent(type), m_size(SizeType(-1)) {}

	inline virtual bool IsDynamicSize( void ) const final {
		return true;
	}

	inline virtual size_t CalcNetworkSize() const {
		OutStream dummyStream(true);
		VSerialize(dummyStream);
		return m_size;
	}

	inline virtual void VSerialize(OutStream &out) const final
	{
		UINT startPos = out.tellp();
		IEvent::VSerialize(out);
		UINT sizePos = out.tellp();
		out << m_size; //dummy
		Serialize(out);
		UINT endPos = out.tellp();
		SizeType newSize = endPos - startPos;
		Assert(!IsSizeCalculated() || m_size == newSize);
		const_cast<IEventDynamicSized*>(this)->m_size = newSize; //HACKHACK

		//ConsoleMessage("Serializing dynamic sized event with size %i", m_size);

		//write size to where the dummy was previously
		out.seekp(sizePos);
		out << m_size;
		out.seekp(endPos);
	}

	inline virtual void VDeserialize(InStream &in) final
	{
		IEvent::VDeserialize(in);
		in >> m_size;
		//ConsoleMessage("Deserializing dynamic sized event with size %i", m_size);
		Deserialize(in);
	}

	inline static size_t GetHeaderSize()
	{
		return IEvent::GetHeaderSize() + sizeof(SizeType);
	}

	//Only valid if this event was received over the network OR after serialization OR
	//after you called CalcNetworkSize() while maintaining valid data
	inline size_t GetPayloadSize() const
	{
		return GetNetworkSize() - GetHeaderSize();
	}

	inline static size_t GetPayloadSizeMax()
	{
		return std::numeric_limits<SizeType>::max() - GetHeaderSize() - 1;
	}

	//Only valid if this event contains valid data
	inline virtual size_t GetNetworkSize( void ) const final {
		if( !IsSizeCalculated() )
			CalcNetworkSize();
		return m_size;
	}


private:
	inline bool IsSizeCalculated() const {
		return m_size != SizeType(-1);
	}

	inline void SetNetworkSizeReceived(SizeType size) {
		m_size = size;
	}

	//Only valid if this event was received over the network OR after serialization OR
	//after you called CalcNetworkSize() while maintaining valid data
	SizeType m_size; 
};



template <class T>
class CEventFactoryStaticSized : public IEventFactoryStaticSized
{
public:
	CEventFactoryStaticSized( EventType type, int location ) : IEventFactoryStaticSized(type, location)
	{
		T dummyEvt(type);
		if(dummyEvt.IsDynamicSize())
			error_fatal("Using Static-Sized factory for dynamic-size event of type %s", ev::GetEventStringName(type));

		AddToList( type );
		CalcNetworkSize();
	}

	inline virtual int GetNetworkSize( void ) const final
	{
		return m_iNetworkSize;
	}

	inline void CalcNetworkSize( void )
	{
		char *buf = new char[sizeof(T)];
		memset(buf,0,sizeof(T));

		CBinaryBufferStream stream(buf, sizeof(T));
		T pEvtDummy(GetType());
		pEvtDummy.VDeserialize(stream);
		pEvtDummy.VSerialize(stream);
		m_iNetworkSize = stream.tellp();
		delete[] buf;

		if( stream.tellp() == 0 )
			error( "There is an Error in VSerialize() of the Eventdata-%s. Look into eventlist.cpp.",
				typeid(T).name() );
		if( stream.tellg() == 0 )
			error( "There is an Error in the Serialization-Constructor of the Eventdata-%s. Look into eventlist.cpp",
				typeid(T).name() );
		if( stream.tellp() != stream.tellg() )
			error(	"The Eventdata-%s has different Serialization/Deserialization sizes.\n" \
					"Look into eventlist.cpp and compare the Deserialize Constructor and the VSerialize() Function.",
					typeid(T).name());
	}

	inline T* Create( void ) const
	{
		T* evt = new T(GetType());
		return evt;
	}

private:
	int m_iNetworkSize;
};


template<class T>
class CEventFactoryDynamicSized : public IEventFactory
{
public:
	CEventFactoryDynamicSized( EventType type, int location ) : IEventFactory(type, location)
	{
		T dummyEvt(type);
		if(!dummyEvt.IsDynamicSize())
			error_fatal("Using Dynamic-Sized factory for static-size event of type %s", ev::GetEventStringName(type));
		this->AddToList(type);
	}

	inline T* Create( void ) const
	{
		T* evt = new T(GetType());
		return evt;
	}
};


class CEventCounter
{
public:
	CEventCounter() : m_iCounter(0) { }
	int GetAndIncrement( void ) { m_iCounter++; return m_iCounter-1; }
	int m_iCounter;
};


//When declaring something static inside a function
//all pointers to this static actually point to the
//same thing.
namespace ev{
extern CEventCounter* GetEventCounter( void );
NOINLINE void EventSystemInit( void );
}

class CEventName
{
public:
	CEventName( EventType EventName, const char *name )
	{
		//m_iEventName = ev::GetEventCounter()->GetAndIncrement();
		ev::GetEventNameDictionary()[EventName] = name;
	}
	//EventType m_iEventName;
};


//virtual event listener
class IEventListener
{
public:
	//explicit IEventListener() {}
	//virtual ~IEventListener() = 0;

	virtual bool HandleEvent( const IEvent *evt ) = 0;
	virtual void UnregisterEvents( void ) = 0;
};


#endif
