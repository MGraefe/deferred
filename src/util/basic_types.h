// util/basic_types.h
// util/basic_types.h
// util/basic_types.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__basic_types_H__
#define deferred__util__basic_types_H__

#include "maths.h"
#include <string>
#include <vector>
#include <map>
#include <list>
#include <queue>

typedef unsigned int DLIST;
typedef unsigned int RTEX;
typedef unsigned short int USHORT;
typedef unsigned int UINT;
typedef short int SHORT;
typedef unsigned char BYTE;

#define DECLARE_CLASS( BaseClassName, ClassName ) \
	private: \
	typedef BaseClassName	BaseClass; \
	typedef ClassName		ThisClass

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define EMPTY_COPY_CONSTRUCTOR(classname) classname( const classname &c ){ }

template<typename T, int size>
int ArrayLength( T(&)[size] ) {return size;}

#define CLIENT_ID_LOCAL -1
#define CLIENT_ID_SERVER 0
#define CLIENT_ID_CLIENTS_START 1

#define RECIEVER_ID_LOCAL -1
#define RECIEVER_ID_ALL 0
#define RECIEVER_ID_CLIENT_START

struct geometryquad_t
{
	Vector3f vVerts[4];
	Vector3f vNorm;
};

struct geometrytri_t
{
	Vector3f vVerts[3];
	Vector3f vNorm;
};

struct lightparams_t
{
	float constant;
	float linear;
	float quadratic;
	float dist;

	bool operator==(const lightparams_t &o) const {
		return	constant == o.constant &&
				linear == o.linear &&
				quadratic == o.quadratic &&
				dist == o.dist;
	}

	bool operator!=(const lightparams_t &o) const {
		return !((*this) == o);
	}
};

//size: 44 bytes
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
struct rendervert_t
{
	rendervert_t() {}
	rendervert_t(const Vector3f &pos, const Vector3f &norm, const Vector3f &tang,
		const Vector2f &uv, float skyVisibility) :
		vPos(pos),
		vNorm(norm),
		vTang(tang),
		pUV(uv),
		fSkyVisibility(skyVisibility)
		{ }

	Vector3f vPos;
	Vector3f vNorm;
	Vector3f vTang;
	Vector2f pUV;
	float fSkyVisibility;
};
#pragma pack(pop) /* restore original alignment from stack */

struct rendertri_t
{
	rendervert_t pVerts[3];
	Vector3f vTriNorm;
	float fHandiness;
	int iTexture;
};

struct triangle_no_tex_t
{
//	union
//	{
//		struct{ Vector3f v1,v2,v3; };
//		struct{ Vector3f verts[3]; };
//	};
	Vector3f verts[3];
};

struct triangle_with_tex_t
{
//	union
//	{
//		struct{
//			Vector3f v1,v2,v3;
//			Vector2d uv1, uv2, uv3;
//		};
//		struct{
//			Vector3f vs[3];
//			Vector2d uvs[3];
//		};
//	};
	Vector3f vs[3];
	Vector3f uvs[3];
};


class LessThanCppString : public std::binary_function< const std::string*, const std::string*, bool >
{
public:
	bool operator() (	const std::string *str1,
						const std::string *str2 ) const
	{
		//return strcmp( str1->c_str(), str2->c_str() ) < 0;
		return str1->compare( *str2 ) < 0;
	}
};


class LessThanCppStringObj : public std::binary_function< const std::string&, const std::string&, bool >
{
public:
	bool operator() (	const std::string &str1,
						const std::string &str2 ) const
	{
		//return strcmp( str1.c_str(), str2.c_str() ) < 0;
		return str1.compare(str2) < 0;
	}
};




//Search for all Keys that start with the given prefix.
//Returns a pair of two iterators, the first belongs to the first key in the range,
//and the second one belongs to the last key in range plus one.
//Additionally count returns the number of found keys.
//This is the non-constant version of this function.
template<typename MapT>
inline std::pair<
	typename MapT::iterator,
	typename MapT::iterator >
	GetStringMapPrefixRange( MapT &Map, const std::string &prefix, int &count )
{
	count = 0;
	typename MapT::iterator it = Map.lower_bound( prefix );
	typename MapT::iterator itstart = it;
	typename MapT::iterator itend = Map.end();

	for(; it != itend; it++, count++ )
	{
		if( prefix.compare(0, prefix.length(), it->first, 0, prefix.length() ) != 0 )
			break;
	}

	return std::pair<typename MapT::iterator, typename MapT::iterator>( itstart, it );
}


//Search for all Keys that start with the given prefix.
//Returns a pair of two iterators, the first belongs to the first key in the range,
//and the second one belongs to the last key in range plus one.
//Additionally count returns the number of found keys.
//This is the constant version of this function.
template<class MapT>
inline std::pair<
	typename MapT::const_iterator, 
	typename MapT::const_iterator >
	GetStringMapPrefixRange( const MapT &Map, const std::string &prefix, int &count )
{
	count = 0;
	typename MapT::const_iterator it = Map.lower_bound( prefix );
	typename MapT::const_iterator itstart = it;
	typename MapT::const_iterator itend = Map.end();

	for(; it != itend; it++, count++ )
	{
		if( prefix.compare(0, prefix.length(), it->first, 0, prefix.length() ) != 0 )
			break;
	}

	return std::pair<typename MapT::const_iterator, typename MapT::const_iterator>( itstart, it );
}


#endif
