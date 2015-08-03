// renderer/CPhysDebugDrawer.cpp
// renderer/CPhysDebugDrawer.cpp
// renderer/CPhysDebugDrawer.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"
#include "CPhysDebugDrawer.h"
#include "glfunctions.h"


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CPhysDebugDrawer::CPhysDebugDrawer()
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
CPhysDebugDrawer::~CPhysDebugDrawer()
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPhysDebugDrawer::drawLine( const Vector3f &from, const Vector3f &to, const Vector3f &color )
{
	lineMap[color].push_back(Line(from, to));
}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPhysDebugDrawer::drawContactPoint(const Vector3f &PointOnB,
										const Vector3f &normalOnB,
										float distance,
										int lifeTime,
										const Vector3f &color )
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPhysDebugDrawer::reportErrorWarning( const char *warningString )
{

}


//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPhysDebugDrawer::draw3dText( const Vector3f &location, const char* textString )
{

}

//-----------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
void CPhysDebugDrawer::dispatch(void)
{
	for( LineMap::value_type &mapPair : lineMap )
	{
		set_draw_color(mapPair.first.x, mapPair.first.y, mapPair.first.z, 1.0f);
		draw_lines(&mapPair.second[0].first, mapPair.second.size()*2);
	}
	lineMap.clear();
}
