// renderer/CSprite.h
// renderer/CSprite.h
// renderer/CSprite.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CSprite.h

#pragma once
#ifndef deferred__renderer__CSprite_H__
#define deferred__renderer__CSprite_H__

#include "CRenderEntity.h"
#include <util/dll.h>


class DLL CSprite : public CRenderEntity
{
	DECLARE_CLASS( CRenderEntity, CSprite );

public:
	enum spriteaxis_e{
		AXIS_X = 0,
		AXIS_Y,
		AXIS_Z,
		AXIS_ALL,
	};

public:
	CSprite( int index, bool pointSprite );
	~CSprite();

	void Init( const char *texname, const Vector3f &origin, float width, float height, spriteaxis_e axis = AXIS_ALL );
	void Init( int material, const Vector3f &origin, float width, float height, spriteaxis_e axis = AXIS_ALL );
	virtual void VRender( const CRenderParams &params );
	virtual void VOnRestore( void );
	virtual void VPreDelete( void ) { }

private:
	bool m_bInitialized;
	int m_material;
	spriteaxis_e m_axis;
	float m_width;
	float m_height;
	const bool m_bPointSprite;
};




#endif // deferred__renderer__CSprite_H__
