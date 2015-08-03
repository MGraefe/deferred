// server/CBaseEntity.h
// server/CBaseEntity.h
// server/CBaseEntity.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__server__CBaseEntity_H__
#define deferred__server__CBaseEntity_H__

#include <util/timer.h>
#include <util/maths.h>
#include "CCollisionInfo.h"
#include <util/basic_types.h>
#include "datadesc_server.h"
#include <util/CEntityOutput.h>
#include <util/eventlist.h>
#include <util/CConVar.h>

class CEntityOutputList
{
public:
	typedef std::vector<CEntityOutput> OutputList;
	typedef std::map<std::string, OutputList, LessThanCppStringObj> OutputReceiverMap;

	//returns temporary pointer, not for storage!
	inline const OutputList *GetOutputList(const std::string &name) const
	{
		OutputReceiverMap::const_iterator it = m_outputReceivers.find(name);
		return it == m_outputReceivers.end() ? NULL : &it->second;
	}

	inline void AddOutput(const CEntityOutput &output)
	{
		m_outputReceivers[output.getEvent()].push_back(output);
	}

	void ReadFromScript(const CScriptSubGroup *pScript);

private:
	OutputReceiverMap m_outputReceivers;
};


class CBaseEntity
{
	DECLARE_DATADESC_EDITOR_NOBASE(CBaseEntity)

public:
	CBaseEntity();
	virtual ~CBaseEntity() { }

	virtual void PreUpdate( void );
	virtual void Update( void );
	virtual void PostUpdate( void );

	void SendNetworkSnapshot( int iPlayerID );

	virtual void Destroy( void );
	virtual void Spawn( void );
	virtual void Think( void );
	virtual void SetVisible( bool visible = true );
	virtual bool IsVisible( void );
	virtual void OnDatadescFinished( void );
	void ToggleVisibility( void ); //maybe virtual?
	virtual const char *GetEntityClass() const;
	virtual const char *GetEntityCppClass() const;

	virtual void OnCollisionStart(const CCollisionInfo &info);

	//Only called by the framework
	inline void ReadOutputs(const CScriptSubGroup *pScript) { m_outputList.ReadFromScript(pScript); }
	inline const CEntityOutputList &GetOutputList() { return m_outputList; }
	void FireOutput(const std::string &name, CBaseEntity *activator, CBaseEntity *caller = NULL, float delay = 0.0f);
	inline void SetLastProcessedOutputId(size_t id) { m_lastProcessedOutputId = id; }
	inline size_t GetLastProcessedOutputId() const { return m_lastProcessedOutputId; }

	//Invalidate ALL entries in the network table
	void SetNetworkChangedAll();
	bool IsNetworkChanged() const { return m_bNetworkChangedAll || m_netvarChangeSet.size() > 0; }
	void SerializeNetworkChanges(OutStream &os, bool allVars = false);
	void DeserializeNetworkChanges(InStream &is);
	void ResetNetworkChanged();
	
	CEntityCreateEvent *CreateCreateEvent(EventType type);

	void AddToList();
protected:
	void SetNetworkChanged(size_t memberOffset);

private:
	void SerializeNetworkChange(OutStream &os, const CNetworkTableEntry *entry);

public:
	void SetName( const char *pName );
	void SetParent( CBaseEntity* const pParent ) { m_pParent = pParent; }
	CBaseEntity *GetParent( void ) const { return m_pParent; }

	void SetOrigin( const Vector3f &v ) { m_vOrigin = v; Assert(v.IsValid()); }
	Vector3f const &GetOrigin( void ) const { return m_vOrigin; }

	void SetAngle( const Angle3d &a ) { m_aAngle = a; }
	Angle3d const &GetAngle( void ) const { return m_aAngle; }

	void SetVelocity( const Vector3f &v ) { m_vVelocity = v; }
	const Vector3f &GetVelocity( void ) const { return m_vVelocity; }

	void SetAngVelocity( const Angle3d &v ) { m_aAngVelocity = v; }
	const Angle3d &GetAngVelocity( void ) const { return m_aAngVelocity; }

	bool IsCollidable() const { return m_isCollidable; }
	virtual void SetCollidable(bool collidable);

	const char *GetName( void ) const { return m_name->c_str(); }
	void SetIndex( int i ) { m_index = i; }
	int GetIndex( void ) const { return m_index; }
	
private:
	int m_index;
	bool m_isCollidable;
	bool m_bIsVisible;
	bool m_bNetworkChangedAll;
	NetVar(std::string, m_name);
	CBaseEntity *m_pParent;

	//Vector3f m_vOrigin;
	//NetVarVector3f( m_vOrigin );
	NetVarVector3f( m_vOrigin );
	Vector3f m_vLastOrigin;

	NetVarVector3f( m_aAngle );
	Angle3d m_aLastAngle;

	Vector3f m_vVelocity;
	Vector3f m_vLastVelocity;

	Angle3d m_aAngVelocity;
	Angle3d m_aLastAngVelocity;

	CEntityOutputList m_outputList;
	size_t m_lastProcessedOutputId;

	std::set<size_t> m_netvarChangeSet;
};


extern CConVar network_tables_debug;

/* #define DECLARE_CLASS( BaseClassName, ClassName ) \
 *	typedef BaseClassName	BaseClass; \
 *	typedef ClassName		ThisClass
 */

#endif
