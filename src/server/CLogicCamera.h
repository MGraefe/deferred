// server/CLogicCamera.h
// server/CLogicCamera.h
// server/CLogicCamera.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------


#pragma once
#ifndef deferred__server__CLogicCamera_H__
#define deferred__server__CLogicCamera_H__

#include "CBaseEntity.h"
class CLogicCamera : public CBaseEntity
{
public:
	void Spawn( bool bPlayerOneCam, bool bPlayerTwoCam );
private:
	bool m_bPlayerOneCam;
	bool m_bPlayerTwoCam;
};

#endif
