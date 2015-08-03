// renderer/CLoadingMutex.cpp
// renderer/CLoadingMutex.cpp
// renderer/CLoadingMutex.cpp
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#include "stdafx.h"


#include "CLoadingMutex.h"

CThreadMutex g_LoadingMutex_Obj;
CThreadMutex *const g_LoadingMutex = &g_LoadingMutex_Obj;

CThreadEvent g_LoadingFinishedEvent_Obj;
CThreadEvent *const g_LoadingFinishedEvent = &g_LoadingFinishedEvent_Obj;
