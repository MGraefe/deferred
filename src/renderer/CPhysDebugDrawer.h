// renderer/CPhysDebugDrawer.h
// renderer/CPhysDebugDrawer.h
// renderer/CPhysDebugDrawer.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CPhysDebugDrawer_H__
#define deferred__renderer__CPhysDebugDrawer_H__

#include <util/IPhysicsDebug.h>
#include <map>
#include <vector>

class CVectorMapComparator
{
public:
	bool operator()( const Vector3f &left, const Vector3f &right )
	{
		if(left.x != right.x)
			return left.x < right.x;
		else if(left.y != right.y)
			return left.y < right.y;
		else if(left.z != right.z)
			return left.z < right.z;
		return false;
	}
};

//DO NOT USE MULTIPLE INHERITANCE WITH THIS OBJECT
class /*DLL*/ CPhysDebugDrawer : public IPhysicsDebug
{
public:
	CPhysDebugDrawer();
	~CPhysDebugDrawer();
	void drawLine(  const Vector3f &from, const Vector3f &to, const Vector3f &color );
	void drawContactPoint(  const Vector3f &PointOnB,
									const Vector3f &normalOnB,
									float distance,
									int lifeTime,
									const Vector3f &color );
	void reportErrorWarning( const char *warningString );
	void draw3dText( const Vector3f &location, const char* textString );
	void dispatch(void);
private:
	typedef std::pair<Vector3f, Vector3f> Line;
	typedef std::vector<Line> Lines;
	typedef std::map<Vector3f, Lines, CVectorMapComparator> LineMap;
	LineMap lineMap;
};

#endif