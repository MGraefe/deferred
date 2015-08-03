// server/CLogicCamera.cpp
// server/CLogicCamera.cpp
// server/CLogicCamera.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"

#include "CLogicCamera.h"
#include "ServerInterfaces.h"
#include <util/CEventManager.h>

void CLogicCamera::Spawn( bool bPlayerOneCam, bool bPlayerTwoCam )
{
	m_bPlayerOneCam = bPlayerOneCam;
	m_bPlayerTwoCam = bPlayerTwoCam;
}

