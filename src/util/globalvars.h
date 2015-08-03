// util/globalvars.h
// util/globalvars.h
// util/globalvars.h
//----------------------------------
// Deferred Source Code
// Copyright (c) 2015 Marius Graefe
// All rights reserved
// Contact: deferred@mgraefe.de
//-----------------------------------
#pragma once
#ifndef deferred__util__globalvars_H__
#define deferred__util__globalvars_H__

extern bool g_bWireframed;
extern bool g_bCulling;
extern bool g_bNoFrustumCull;
extern bool g_bWaterVBO;
extern bool g_bSimulateLag;
extern int  g_iPostEffectMethod;
extern bool g_bWaterFrustumClipping;
extern bool g_bShadowsEnabled;
extern bool g_bDebugPhysics;
extern bool g_bDebugTextPerformance;

#define POSTEFFECT_NONE 0
#define POSTEFFECT_FBO 1
#define POSTEFFECT_COPYTEX 2

//extern void UpdateGlobalKeyVars( bool *bKeys, bool *bLastKeys );

#endif
