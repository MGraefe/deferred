// server/datadesc_server.h
// server/datadesc_server.h
// server/datadesc_server.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//datadesc_server.h

#pragma once
#ifndef deferred__server__datadesc_server_H__
#define deferred__server__datadesc_server_H__


#define BASE_ENTITY_TYPE CBaseEntity
#include <util/datadesc_makros.h>
#include <util/networkStreams.h>

class CNetVarWriteProxyDefault
{
public:
	static bool write(OutStream &os, void *member, UINT memberSize, UINT networkSize, UINT memberCount) {
		os.write((char*)member, memberSize * memberCount);
		return os.GetError() == OutStream::ERROR_NONE;
	}

	static size_t GetNetworkSize(size_t memberSize) {
		return memberSize;
	}
};


class CNetVarWriteProxyAngles
{
public:
	static bool write(OutStream &os, void *member, UINT memberSize, UINT networkSize, UINT memberCount) {
		Assert(memberSize == sizeof(Angle3d));
		Assert(networkSize == Angle3dCompressed::sNetSize);
		for(UINT i = 0; i < memberCount; i++)
			os << Angle3dCompressed(((Angle3d*)member)[i]);
		return os.GetError() == OutStream::ERROR_NONE;
	}

	static size_t GetNetworkSize(size_t memberSize) {
		Assert(memberSize == sizeof(Angle3d));
		return Angle3dCompressed::sNetSize;
	}
};

//For basic transformations, like float to int, int to char etc.
template<typename NetT, typename MemberT>
class CNetVarWriteProxyCast
{
public:
	static bool write(OutStream &os, void *member, UINT memberSize, UINT networkSize, UINT memberCount) {
		Assert(memberSize == sizeof(MemberT));
		Assert(networkSize == sizeof(NetT));
		for(UINT i = 0; i < memberCount; i++)
			os << (NetT)(((MemberT*)member)[i]);
		return os.GetError() == OutStream::ERROR_NONE;
	}

	static size_t GetNetworkSize(size_t memberSize) {
		Assert(memberSize == sizeof(MemberT));
		return sizeof(NetT);
	}
};


//For std::string
class CNetVarWriteProxyString
{
public:
	static bool write(OutStream &os, void *member, UINT memberSize, UINT networkSize, UINT memberCount) {
		for(UINT i = 0; i < memberCount; i++)
			os << ((std::string*)member)[i];
		return os.GetError() == OutStream::ERROR_NONE;
	}

	static size_t GetNetworkSize(size_t memberSize) {
		return (size_t)-1;
	}
};


typedef IEntityFactoryEditor<CBaseEntity>::EditorFactoryMap EditorFactoryMapServer;

EditorFactoryMapServer &GetEditorFactoryMapServer( void );
const IEntityFactoryEditor<CBaseEntity> *GetEditorFactoryServer( const std::string name );


#define LINK_ENTITY_TO_CLASSNAME_SERVER( Class, ClassName ) \
	static const CEntityFactoryEditorServer<Class> Class##_EditorFactory( &GetEditorFactoryMapServer(), #ClassName ); \
	const char *Class::GetEntityClass() const { return #ClassName; } \
	const char *Class::GetEntityCppClass() const { return #Class; }

#define DECLARE_ENTITY_LINKED() \
	public: virtual const char *GetEntityClass() const; \
	public: virtual const char *GetEntityCppClass() const;

template<typename T>
class CEntityFactoryEditorServer : public IEntityFactoryEditor<CBaseEntity>
{
public:
	CEntityFactoryEditorServer( EditorFactoryMapServer *map, const std::string &editorName ) :
		IEntityFactoryEditor<CBaseEntity>::IEntityFactoryEditor(map, editorName)
	{
		//T::InitDatadescListEditor();
	}

	virtual CBaseEntity *CreateEntity( CScriptSubGroup *pScript, int index ) const
	{
		T *pEnt = new T();
		TypedDatadescList *pDataDescList = pEnt->GetDatadescListEditor();
		pDataDescList->ReadFromScript( pScript, (void*)pEnt );
		pEnt->ReadOutputs(pScript);
		pEnt->AddToList();
		pEnt->OnDatadescFinished();
		return pEnt;
	}
};



#endif // deferred__server__datadesc_server_H__
