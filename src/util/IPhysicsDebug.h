// util/IPhysicsDebug.h
// util/IPhysicsDebug.h
// util/IPhysicsDebug.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__util__IPhysicsDebug_H__
#define deferred__util__IPhysicsDebug_H__

//Pure virtual class to be implemented in the renderer
class IPhysicsDebug
{
public:
	virtual void drawLine(  const Vector3f &from, const Vector3f &to, const Vector3f &color ) = 0;
	virtual void drawContactPoint(  const Vector3f &PointOnB,
									const Vector3f &normalOnB,
									float distance,
									int lifeTime,
									const Vector3f &color ) = 0;
	virtual void reportErrorWarning( const char *warningString ) = 0;
	virtual void draw3dText( const Vector3f &location, const char* textString ) = 0;
	virtual void dispatch(void) = 0;
};

#endif