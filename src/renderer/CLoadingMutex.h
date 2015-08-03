// renderer/CLoadingMutex.h
// renderer/CLoadingMutex.h
// renderer/CLoadingMutex.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------

#pragma once
#ifndef deferred__renderer__CLoadingMutex_H__
#define deferred__renderer__CLoadingMutex_H__

#include <util/CThreadMutex.h>
#include <util/dll.h>

//TODO: INTERFACE
extern CThreadMutex *const g_LoadingMutex;
extern DLL CThreadEvent *const g_LoadingFinishedEvent;

#endif
