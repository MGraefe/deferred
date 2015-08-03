// server/CCollisionInfo.h
// server/CCollisionInfo.h
// server/CCollisionInfo.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
//CCollisionInfo.h

#pragma once
#ifndef deferred__server__CCollisionInfo_H__
#define deferred__server__CCollisionInfo_H__

class CBaseEntity;
class CCollisionInfo
{
public:
	CBaseEntity *ent1;
	CBaseEntity *ent2;
	Vector3f point;
	float force;
};

#endif // deferred__server__CCollisionInfo_H__
