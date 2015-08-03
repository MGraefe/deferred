// renderer/datadesc_renderer.h
// renderer/datadesc_renderer.h
// renderer/datadesc_renderer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__datadesc_renderer_H__
#define deferred__renderer__datadesc_renderer_H__

#ifndef BASE_ENTITY_TYPE
#define BASE_ENTITY_TYPE CRenderEntity
#endif

#include <util/datadesc_makros.h>

class CNetVarReadProxyDefault
{
public:
	static bool read(InStream &is, void *member, UINT memberSize, UINT networkSize, UINT memberCount) {
		is.read((char*)member, memberSize * memberCount);
		return is.GetError() == InStream::ERROR_NONE;
	}

	static size_t GetNetworkSize(size_t memberSize) {
		return memberSize;
	}

	static size_t GetDynamicSize(InStream &is) {
		return -1;
	}
};


class CNetVarReadProxyAngles
{
public:
	static bool read(InStream &is, void *member, UINT memberSize, UINT networkSize, UINT memberCount) {
		Assert(memberSize == sizeof(Quaternion));
		Assert(networkSize == Angle3dCompressed::sNetSize);
		for(UINT i = 0; i < memberCount; i++)
		{
			Angle3dCompressed ang;
			is >> ang;
			((Quaternion*)member)[i].SetFromAngle(ang.toAngle());
		}
		return is.GetError() == InStream::ERROR_NONE;
	}

	static size_t GetNetworkSize(size_t memberSize) {
		Assert(memberSize == sizeof(Quaternion));
		return Angle3dCompressed::sNetSize;
	}

	static size_t GetDynamicSize(InStream &is) {
		return -1;
	}
};


//For basic transformations, like float to int, int to char etc.
template<typename NetT, typename MemberT>
class CNetVarReadProxyCast
{
public:
	static bool read(InStream &is, void *member, UINT memberSize, UINT networkSize, UINT memberCount) {
		Assert(memberSize == sizeof(MemberT));
		Assert(networkSize == sizeof(NetT));
		for(UINT i = 0; i < memberCount; i++)
		{
			NetT val;
			is >> val;
			((MemberT*)member)[i] = (MemberT)val;
		}
		return is.GetError() == InStream::ERROR_NONE;
	}

	static size_t GetNetworkSize(size_t memberSize) {
		Assert(memberSize == sizeof(MemberT));
		return sizeof(NetT);
	}

	static size_t GetDynamicSize(InStream &is) {
		return (size_t)-1;
	}
};


//For std::string
class CNetVarReadProxyString
{
public:
	static bool read(InStream &is, void *member, UINT memberSize, UINT networkSize, UINT memberCount) {
		for(UINT i = 0; i < memberCount; i++)
			is >> ((std::string*)member)[i];
		return is.GetError() == OutStream::ERROR_NONE;
	}

	static size_t GetNetworkSize(size_t memberSize) {
		return (size_t)-1;
	}

	static size_t GetDynamicSize(InStream &is) {
		USHORT size = 0;
		is >> size;
		is.seekg(is.tellg() - sizeof(USHORT));
		return size + sizeof(USHORT);
	}
};


#define DECLARE_ENTITY_LINKED() \
	public: virtual const char *GetEntityClass() const; \

#define LINK_ENTITY_TO_CLASSNAME_RENDERER(Class, ClassName) \
	const char *Class::GetEntityClass() const { return #ClassName; }
	
#endif // deferred__renderer__datadesc_renderer_H__



